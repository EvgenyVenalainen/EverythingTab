// EverythingTab.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "EverythingTab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "ListCtrlEx.h"
#include "atlbase.h"

static const GUID guidPane = 
{ 0x3d534ded, 0x66fa, 0x44ea, { 0x82, 0x54, 0x2, 0x31, 0xed, 0xdf, 0xe0, 0x9f } };


/////////////////////////////////////////////////////////////////////////////
// CEverythingTabApp

BEGIN_MESSAGE_MAP(CEverythingTabApp, CWinApp)
	//{{AFX_MSG_MAP(CEverythingTabApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEverythingTabApp construction

CEverythingTabApp::CEverythingTabApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstanc
	m_bAuto = 0;
//	m_bPrefix = 1;
//	m_sPrefix = "#";
//	WriteReg();
	ReadReg();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEverythingTabApp object

CEverythingTabApp theApp;

extern "C"
BOOL __declspec(dllexport) Aml_GetPluginInfo(AML_PLUGIN_INFO* pInfo)
{
	pInfo->nMinRelease=PLUG_MIN_BUILD;

	const int nMake=APPHelper::MakePluginMenuTitle(pInfo->szMenuName
		,AML_MAX_NAME-1
		,PLUG_TITLE
		,theApp.m_hInstance
		);

	if (0 == nMake)
		_tcscpy(pInfo->szMenuName,PLUG_TITLE);

	pInfo->nMenuCount=3;
	pInfo->dwEventMask=APFEM_EDITOREVENTS;

	//проверка Unicode-версий
	const BOOL bUnicodeAP=APPHelper::IsUnicodeVersionOfAmlPages(*pInfo);
#ifdef _UNICODE
	//вызов НЕ из Unicode-версии Aml Pages
	if (!bUnicodeAP) {
		return FALSE;
	}
	else {
		//для U-версии плагина при вызове из U-версии Aml Pages говорим что плагин поддерживает Unicode
		pInfo->dwFlags|=APF_PLUGINSUPPORTUNICODE;
	}

#else //ANSI-version
	if (bUnicodeAP) {
		return FALSE;
	}
#endif

	return TRUE;
}

extern "C"
BOOL __declspec(dllexport) Aml_GetMenuItemInfo(int nIndex, AML_MENU_ITEM_INFO* p)
{
	BOOL bRet=TRUE;
	const HICON hIcon = LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDI_ICONPLUGIN));
	ASSERT(hIcon);
	const BOOL bRU=APPHelper::IsAmlPagesRULanguage();

	switch (nIndex) {
	case 0:
		p->nPluginType=APPT_EXECCOMMAND;
		if (!bRU) {
			_tcscpy(p->szMenuName,TEXT("Show\\Hide EverythingTab"));
			_tcscpy(p->szTTHint,TEXT("'Everything' aided search"));
		}
		else {
			_tcscpy(p->szMenuName,TEXT("Найти файлы или папки"));
			_tcscpy(p->szTTHint, TEXT("Нижнее окно с результатами поиска файлов"));
		}
		strcpy(p->szFunctionName,"ShowCommand");
		p->hIcon=hIcon;
		break;
	case 1:
		p->nPluginType=APPT_SHOWDIALOG;
		if (!bRU) {
			_tcscpy(p->szMenuName,TEXT("Use the prefix"));
			_tcscpy(p->szTTHint,TEXT("Do search for strings with the prefix only"));
		}
		else {
			_tcscpy(p->szMenuName,TEXT("Использовать префикс"));
			_tcscpy(p->szTTHint, TEXT("Выполнять поиск только для строк с префиксом"));
		}
		strcpy(p->szFunctionName,"EnablePrefix");
		if (theApp.m_bPrefix) p->nMenuState=MF_CHECKED;
		break;
	case 2:
		p->nPluginType=APPT_SHOWDIALOG;
		if (!bRU) {
			_tcscpy(p->szMenuName, "The prefix is '" + theApp.m_sPrefix + "'");
			_tcscpy(p->szTTHint,TEXT("Point the prefix of a line to be used for a search"));
		}
		else {
			_tcscpy(p->szMenuName, "Префикс: '" + theApp.m_sPrefix + "'");
			_tcscpy(p->szTTHint, TEXT("Задать префикс строк для поиска"));
		}
		strcpy(p->szFunctionName,"SetPrefix");
		p->hIcon=LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(195)); // тэг префикса
		break;
	default:
		bRet=FALSE;
		break;
	}
	return bRet;
}

extern "C"
BOOL __declspec(dllexport) ShowCommand(AML_EXECCOMMAND* p)
{
	AML_PANECOMMAND command;
	memcpy(&command.guid,&guidPane,sizeof(GUID));
	command.dwCommand=APFPANECOMMAND_TOGGLE;

	const BOOL bCall=APPHelper::PluginPaneCommand(command);
	return TRUE;
}

extern "C"
BOOL __declspec(dllexport) EnablePrefix(HWND)
{
	theApp.m_bPrefix = !theApp.m_bPrefix;
	APPHelper::ReReadPluginsInfo(theApp.m_hInstance);
	theApp.WriteReg();
	return TRUE;
}

extern "C"
BOOL __declspec(dllexport) SetPrefix(HWND hwnd)
{
	BSTR s[64];
	CString a;
	if (APPHelper::DisplayPasswordDialog(s, TRUE, TRUE, NULL, TRUE,
		APPHelper::IsAmlPagesRULanguage() ? _T("Новый префикс") : _T("The new prefix")))
		theApp.m_sPrefix = s[0];
//	AfxMessageBox(theApp.m_sPrefix);
	APPHelper::ReReadPluginsInfo(theApp.m_hInstance);
	theApp.WriteReg();
	return TRUE;
}

extern "C"
UINT __declspec(dllexport)Aml_GetPaneInfo(UINT nIndex, AML_PANEINFO* p)
{
	if (-1 == nIndex || NULL == p)
		return 1;

	if (0 == nIndex) {
		p->hIcon=::LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDI_ICONPLUGIN));
		_tcscpy(p->lpszPaneName,PLUG_TITLE);

		CString s;
		if (!APPHelper::IsAmlPagesRULanguage())
			s=TEXT("Show the list of files for current paragraph as search query of Everything");
		else 
			s=TEXT("Показать для текущего абзаца как строки поиска в Everything список найденных файлов или папок");
		_tcsncpy(p->lpszPaneHint,s,p->cchMaxPaneHint);
		memcpy(&p->guidPane,&guidPane,sizeof(GUID));
		return TRUE;
	}

	return 0;
}

static CListCtrlEx* m_pWnd=NULL;

extern "C"
HWND __declspec(dllexport) AmlPane_Create(HWND hwndParent
										  , AML_PANECREATE* pCreate)
{
	const DWORD dwS=WS_CHILD|
		WS_VISIBLE|
		LVS_REPORT|
		WS_BORDER|
		LVS_SINGLESEL
		;
	CRect r(0,0,pCreate->sizePane.cx,pCreate->sizePane.cy);
	const HWND hwnd=CreateWindow(WC_LISTVIEW,NULL,dwS
		,r.left,r.top
		,r.Width(),r.Height()
		,hwndParent,NULL
		,theApp.m_hInstance,0		
		);

	ASSERT(hwnd);

	if (NULL == hwnd)
		return NULL;

	m_pWnd=new CListCtrlEx;

	ASSERT(m_pWnd);

	m_pWnd->m_pApp = &theApp;
	m_pWnd->SubclassWindow(hwnd);

	DWORD ExStyle = m_pWnd->GetExtendedStyle();
	ExStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES|LVS_EX_INFOTIP;
	m_pWnd->SetExtendedStyle(ExStyle);

	return hwnd;	
}

extern "C"
LRESULT __declspec(dllexport) AmlPane_NotifyReceived(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	NMHDR* const pNMHDR=(NMHDR*)lParam;
	if (NM_CUSTOMDRAW == pNMHDR->code) {
		const int n=0;
	}

	const LRESULT lret=m_pWnd->SendMessage(WM_NOTIFY,wParam,lParam);

	return lret;
}

#ifdef _UNICODE
extern "C"
BOOL __declspec(dllexport) Aml_IsSupportsPluginUnicode(LPVOID)
{
	return TRUE;
}
#endif

CString CEverythingTabApp::GetEverythingVersion()
{
	DWORD majorVersion;
	DWORD minorVersion;
	DWORD revision;

	majorVersion = Everything_GetMajorVersion();
	minorVersion = Everything_GetMinorVersion();
	revision = Everything_GetRevision();

	CString s;

	if (majorVersion > 0)
	{
		s.Format(_T("%d.%d.%d"), majorVersion, minorVersion, revision);
	}

	return s;
}


#include "windowsx.h"
//#define Edit_GetLine(hwndCtl, line, lpch, cchMax) ((*((WORD *)(lpch)) = (cchMax)), ((int)(DWORD)SNDMSG((hwndCtl), EM_GETLINE, (WPARAM)(int)(line), (LPARAM)(LPTSTR)(lpch))))

extern "C"
BOOL __declspec(dllexport) Aml_EventHandler(AML_EVENTINFO* p)
{
	switch(p->EventCode)
	{
		case AEC_EDITOR:
			switch(p->EventInfo.EditorEvent.nMessage)
			{
			case WM_KEYDOWN:
			case WM_LBUTTONDOWN:
			case EN_SELCHANGE:
				const HWND hwnd = p->EventInfo.EditorEvent.hwnd;
				const int nLineIndex = SendMessage(hwnd,EM_LINEFROMCHAR,(WPARAM)-1,0);
				TCHAR sLine[128];
				int n = Edit_GetLine(hwnd, nLineIndex, sLine, 128);
				CString s;
				if (n > 1)
				{
					sLine[n-1] = 0;
					s = sLine;
					APPHelper::SetStatusBarText(s);
				}
				else
				{
					s = "";
					APPHelper::SetStatusBarText(_T(" "));
				}
				theApp.m_sSearch = s;
				if (m_pWnd && theApp.m_bAuto == 1)
						m_pWnd->InsertSearchItems();
			};
	}

	return TRUE;
}



BOOL CEverythingTabApp::WriteReg()
{
	bool bResult(false);
	CRegKey cKey;
	if (ERROR_SUCCESS ==
		cKey.Create(HKEY_CURRENT_USER,
		_T("Software\\G&G Software\\Aml PagesU\\Plugins\\EverythingTab")))
	{
		bResult = (ERROR_SUCCESS == cKey.SetValue(m_bPrefix, _T("bPrefix")));
		bResult = (ERROR_SUCCESS == cKey.SetValue(m_sPrefix, _T("sPrefix")));
	}
	return bResult;
}


BOOL CEverythingTabApp::ReadReg()
{
	DWORD i;
	TCHAR s[255];
	DWORD size=sizeof(s);
	bool bResult(false);
	CRegKey cKey;
	
	if (ERROR_SUCCESS ==
		cKey.Open(HKEY_CURRENT_USER,
		_T("Software\\G&G Software\\Aml PagesU\\Plugins\\EverythingTab")))
	{
		if (bResult = (ERROR_SUCCESS == cKey.QueryValue(i, _T("bPrefix"))))
			m_bPrefix = i;
		else
			m_bPrefix = 0;

		if (bResult = (ERROR_SUCCESS == cKey.QueryValue(s, _T("sPrefix"), &size)))
			m_sPrefix = s;
		else
			m_sPrefix = _T(">>>");
	}
	else
	{
		m_bPrefix = 0;
		m_sPrefix = _T(">>>");
	}

	return bResult;
}

BOOL CEverythingTabApp::SetStatus(int n)
{
	theApp.m_bAuto = n;
	if (m_pWnd)
	{
		LVCOLUMN col;
		col.mask = LVCF_IMAGE;
		if (m_pWnd->GetColumn(0, &col))
		{
			col.iImage = n + 9;
			m_pWnd->SetColumn(0, &col);
		}
		if (n == 0)
			m_pWnd->InsertHelpItems();
		else if (n == 1)
			m_pWnd->InsertSearchItems();
	}
	return TRUE;
}
