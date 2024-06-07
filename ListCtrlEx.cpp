// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "EverythingTab.h"
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <sys/stat.h>

CString FileDate(TCHAR* s)
{
	GetFileAttributesW(s);
    struct _stat t_stat;
    _wstat(s, &t_stat);
    struct tm * timeinfo = localtime(&t_stat.st_mtime);
	CString r;
	int y = timeinfo->tm_year + 1900;
	int M = timeinfo->tm_mon + 1;
	int d = timeinfo->tm_mday;
	int q = timeinfo->tm_isdst;
	int h = timeinfo->tm_hour;
	int m = timeinfo->tm_min;
	int L = t_stat.st_size;
	r.Format(_T("%02i.%02i.%i %02i:%02i"), d, M, y, h, m);
	return r;
}

HICON FileIcon(TCHAR* s)
{
	SHFILEINFOW sfi = {0};
	HRESULT hr = SHGetFileInfo(s,-1, &sfi, sizeof(sfi), SHGFI_ICON);        
    if (SUCCEEDED(hr))
		return sfi.hIcon != NULL ? sfi.hIcon : 
//	LoadIcon(NULL,IDI_WINLOGO)
	LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(278)) // Корзина
		;
	else
		return LoadIcon(NULL,IDI_HAND);
}

CString FileSize(TCHAR* s)
{
	CString r;
    FILE *f;
	f = _tfopen(s , _T("r"));
    if (f != NULL)
	{
		fseek(f, 0, SEEK_END);
		unsigned long len = (unsigned long)ftell(f);
		fclose(f);
		if (len > 1024)
		{
			r.Format(_T("%d kB"), len/1024);
			if (r.GetLength() > 6)
				r = r.Left(r.GetLength() - 6) + " " + r.Right(6);
		}
		else if (len > 99)
			r = "1 kB";
		else
			r.Format(_T("%d"), len);
	}
	else
		r = "N/A";
	return r;
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx()
{
}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers

void CListCtrlEx::PreSubclassWindow() 
{
	CRect Rect;
	GetClientRect(&Rect);
	int W = Rect.Width() - GetSystemMetrics(SM_CXVSCROLL) - 560;
	if (W < 0) W = 100;

	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_IMAGE;
	lvColumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE;
	lvColumn.cx = 300;
	lvColumn.pszText = _T("Name");
	lvColumn.iImage = 9;
	InsertColumn(0, &lvColumn);

	lvColumn.fmt = LVCFMT_CENTER | LVCFMT_IMAGE;
	lvColumn.cx = 70;
	lvColumn.pszText = _T("Type");
	lvColumn.iImage = 11;
	InsertColumn(1, &lvColumn);

	lvColumn.fmt = LVCFMT_RIGHT | LVCFMT_IMAGE;
	lvColumn.cx = 70;
	lvColumn.pszText = _T("Size");
	lvColumn.iImage = 10;
	InsertColumn(2, &lvColumn);

	lvColumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE;
	lvColumn.cx = W;
	lvColumn.pszText = _T("Path");
	lvColumn.iImage = 8;
	InsertColumn(3, &lvColumn);

	lvColumn.fmt = LVCFMT_CENTER | LVCFMT_IMAGE;
	lvColumn.cx = 120;
	lvColumn.pszText = _T("Date");
	lvColumn.iImage = 3;
	InsertColumn(4, &lvColumn);

//	InsertColumn(0, _T("Name"), LVCFMT_LEFT, 300, 0);
//	InsertColumn(1, _T("Type"), LVCFMT_CENTER, 50, 0);
//	InsertColumn(2, _T("Size"), LVCFMT_RIGHT, 80, 0);
//	InsertColumn(3, _T("Path"), LVCFMT_LEFT, W, 0);
//	InsertColumn(4, _T("Date"), LVCFMT_CENTER, 120, 0);

	HIMAGELIST hList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 128, 128);
	m_SmallImg.Attach(hList);

	HICON hi=NULL;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	hi = LoadIcon(hInstance, MAKEINTRESOURCE(128)); // 0 AmlPages
	m_SmallImg.Add(hi);

	hi = LoadIcon(m_pApp->m_hInstance, MAKEINTRESOURCE(IDI_ICONPLUGIN)); // 1 Everything32
	m_SmallImg.Add(hi);

	hi = LoadIcon(m_pApp->m_hInstance, MAKEINTRESOURCE(IDI_ICONSERVER)); // 2 EverythingTab
	m_SmallImg.Add(hi);


	hi = LoadIcon(NULL,IDI_APPLICATION); // 3 [app]
	hi = LoadIcon(hInstance, MAKEINTRESOURCE(151)); // [date]
	m_SmallImg.Add(hi);
	hi = LoadIcon(NULL,IDI_ASTERISK); // 4 (!)
	m_SmallImg.Add(hi);
	hi = LoadIcon(NULL,IDI_EXCLAMATION); // 5 /!`.
	m_SmallImg.Add(hi);
	hi = LoadIcon(NULL,IDI_HAND); // 6 (X)
	m_SmallImg.Add(hi);
	hi = LoadIcon(NULL,IDI_QUESTION); // 7 (?)
	m_SmallImg.Add(hi);
	hi = LoadIcon(NULL,IDI_WINLOGO); // 8 [app]
	hi = LoadIcon(hInstance, MAKEINTRESOURCE(162)); // [foldeer]
	m_SmallImg.Add(hi);

	hi = LoadIcon(m_pApp->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STATUS)); // 9 статус - домик
	hi = LoadIcon(hInstance, MAKEINTRESOURCE(188)); // паззлы
	m_SmallImg.Add(hi);
	hi = LoadIcon(m_pApp->m_hInstance, MAKEINTRESOURCE(IDI_ICON_SEARCH)); // 10 поиск - солнце
	m_SmallImg.Add(hi);
	hi = LoadIcon(m_pApp->m_hInstance, MAKEINTRESOURCE(IDI_ICON_PAUSE)); // 11 пауза - снежинка
	m_SmallImg.Add(hi);

	m_SmallImg.SetImageCount(LAST_NUM + MAX_NUM);

	SetImageList(&m_SmallImg, LVSIL_SMALL);

	InsertHelpItems();

	CListCtrl::PreSubclassWindow();
}


void CListCtrlEx::InsertHelpItems()
{
	DeleteAllItems();

	InsertItem(LVIF_IMAGE | LVIF_TEXT, 0, _T("AmlPages"), 0, 0, 0, 0);
	SetItemText(0, 1, _T("exe"));

	CString s;
	AML_MODULEVERSIONINFO Aml;
	APPHelper::GetModuleVersionInfoOfAmlPages(&Aml);
	s.Format(_T("%d.%d.%d"), Aml.dwMajor, Aml.dwMinor, Aml.dwRevision);
	SetItemText(0, 2, s);

	s.Format(_T("%d > %d"), APPHelper::GetAmlPagesBuildNumber(), PLUG_MIN_BUILD);
	SetItemText(0, 3, APPHelper::IsAmlPagesRULanguage() ? _T("Установлен и работает") : _T("Has installed and is processing"));
	SetItemText(0, 4, s);

	InsertItem(LVIF_IMAGE | LVIF_TEXT, 1, _T("Everything32"), 0, 0, Everything_GetMajorVersion() > 0 ? 2 : 6, 0);

	if (Everything_GetMajorVersion() > 0)
	{

		// Set the search string to abc
		Everything_SetSearch(TEXT("abcd1234"));

		// execute the query
		if (!Everything_Query(TRUE))
		{
			DWORD dwLastError = Everything_GetLastError();
			if (dwLastError == EVERYTHING_ERROR_IPC)
			{
				// IPC not available.
			}
		}
		else
		{
			SetItemText(1, 2, CEverythingTabApp::GetEverythingVersion());
			SetItemText(1, 3, APPHelper::IsAmlPagesRULanguage() ? _T("Установлен и работает") : _T("Has installed and is processing"));
			SetItemText(1, 4, _T(">= 1.4.1"));
		}
	}
	else
	{
		SetItemText(1, 2, _T("N/A"));
		SetItemText(1, 3, APPHelper::IsAmlPagesRULanguage() ?
			_T("Для работы плагина нужен запущенный Everything (https://www.voidtools.com)") :
			_T("The plugin requires the Everything to be launched (https://www.voidtools.com)"));
		SetItemText(1, 4, _T("not OK"));
	}
	SetItemText(1, 1, _T("dll"));

	InsertItem(LVIF_IMAGE | LVIF_TEXT, 2, _T("EverythingTab"), 0, 0, 1, 0);
	SetItemText(2, 1, _T("dll"));
	SetItemText(2, 2, _T("1.0.0"));
	SetItemText(2, 3, APPHelper::IsAmlPagesRULanguage() ?
		_T("Контекстное меню (ПКМ) переключает режимы работы плагина") :
		_T("Context menu (RMB) switches plugin operating modes"));
	SetItemText(2, 4, _T("OK"));
}


void CListCtrlEx::InsertSearchItems()
{
	DeleteAllItems();
	CString s(m_pApp->m_sSearch);

	if (m_pApp->m_bPrefix)
		if (s.Left(m_pApp->m_sPrefix.GetLength()) != m_pApp->m_sPrefix)
			return;
		else
			s = s.Mid(m_pApp->m_sPrefix.GetLength());

	s.TrimLeft(); s.TrimRight();

	if (s == "") return;
	
	APPHelper::SetStatusBarText("es:" + s);

	Everything_SetSearch(s);
	Everything_SetMax(MAX_NUM);
//	Everything_SetSort(EVERYTHING_SORT_NAME_DESCENDING);
	Everything_Query(TRUE);

	CString name, path, ext, size, date;
	TCHAR buf[MAX_PATH];
	int pos;

	for(DWORD i=0;i<Everything_GetNumResults();i++)
	{
		name = Everything_GetResultFileName(i);
		path = Everything_GetResultPath(i);
		Everything_GetResultFullPathName(i,buf,sizeof(buf) / sizeof(TCHAR));
		ext.Empty();
		size.Empty();
		date = FileDate(buf);

		if (Everything_IsFileResult(i))
		{
			if ((pos = name.ReverseFind('.')) != -1)
			{
				ext = name.Mid(pos+1);
				name = name.Left(pos);
			}
			size = FileSize(buf);
		}

		m_SmallImg.Replace(LAST_NUM + i, FileIcon(buf));

		InsertItem(LVIF_IMAGE | LVIF_TEXT, i, name, 0, 0, LAST_NUM + i, 0);
		SetItemText(i, 1, ext);
		SetItemText(i, 2, size);
		SetItemText(i, 3, path);
		SetItemText(i, 4, date);
	}

}


void CListCtrlEx::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_Menu.CreatePopupMenu();
 
    POINT current_point;
    GetCursorPos(&current_point);
 
    m_Menu.AppendMenu(MF_STRING | (m_pApp->m_bAuto == 0 ? MF_CHECKED : 0), ID_STRING1, APPHelper::IsAmlPagesRULanguage() ? _T("Статус") : _T("Status"));
    m_Menu.AppendMenu(MF_STRING | (m_pApp->m_bAuto == 1 ? MF_CHECKED : 0), ID_STRING2, APPHelper::IsAmlPagesRULanguage() ? _T("Поиск") : _T("Search"));
    m_Menu.AppendMenu(MF_STRING | (m_pApp->m_bAuto == 2 ? MF_CHECKED : 0), ID_STRING3, APPHelper::IsAmlPagesRULanguage() ? _T("Пауза") : _T("Pause"));
 
    m_Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
                          current_point.x,
                          current_point.y,
                          this);
    
	m_Menu.DestroyMenu();

	*pResult = 1;
}


BOOL CListCtrlEx::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    switch (wParam)
    {
        case ID_STRING1:
			m_pApp->SetStatus(0);
            break;
        case ID_STRING2:
			m_pApp->SetStatus(1);
            break;
        case ID_STRING3:
			m_pApp->SetStatus(2);
            break;
    };
	return CListCtrl::OnCommand(wParam, lParam);
}


void CListCtrlEx::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMITEMACTIVATE * pItem = (NMITEMACTIVATE*)pNMHDR;
	TCHAR buf[MAX_PATH];
	CString s;
	if (m_pApp->m_bAuto == 0)
	{
		if (Everything_GetMajorVersion() > 0)
			m_pApp->SetStatus(1);
		else
			ShellExecute(NULL, NULL, _T("https://www.voidtools.com"), NULL, NULL, SW_SHOWNORMAL);
	}
	else if (pItem->iItem >= 0)
		if (pItem->iSubItem == 3)
		{
			s = GetItemText(pItem->iItem, pItem->iSubItem);
			if (!s.IsEmpty())
				ShellExecute(NULL, NULL, s, NULL, NULL, SW_SHOWNORMAL);
		}
		else if (pItem->iSubItem == 0)
		{
			if (Everything_GetResultFullPathName(pItem->iItem,buf,sizeof(buf) / sizeof(TCHAR)))
				ShellExecute(NULL, _T("open"), buf, NULL, NULL, SW_RESTORE);
		}

	*pResult = 0;
}



void CListCtrlEx::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMITEMACTIVATE * pItem = (NMITEMACTIVATE*)pNMHDR;
	if (m_pApp->m_bAuto == 1 && pItem->iSubItem == 1)
		m_pApp->SetStatus(2);
	else if (m_pApp->m_bAuto == 2 && pItem->iSubItem == 2)
		m_pApp->SetStatus(1);
	else if (pItem->iItem >= 0)
	{
		CString s(GetItemText(pItem->iItem, pItem->iSubItem));
		if (!s.IsEmpty())
			APPHelper::SetStatusBarText(s);
	}
	*pResult = 0;
}

void CListCtrlEx::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iSubItem <= 1)
		m_pApp->SetStatus(2);
	else if (pNMListView->iSubItem == 2)
		m_pApp->SetStatus(1);
	*pResult = 0;
}
