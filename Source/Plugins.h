//Aml Pages Plugin API Version
#define AML_PAGES_PLUGIN_API_VERSION TEXT("9.94 build 2858")

/***************************************************
*	Aml Pages Plugin API header
*	Version: 9.94 build 2858 ( May 17, 2022 )
*
*	(�) 1998-2022, Mazov Gosha aka Carc
*
*	WWW:				http://www.amlpages.com/home.php
*	Email:				amlpages@amlpages.com, carc@rambler.ru
*	Forum:				http://amlpages.com/home_forum.php
* 
*	Documentation:		http://www.amlpages.com/fordev.shtml (En)
*						http://www.amlpages.com/Rus/plugring.htm (Ru)
*   Plugins Binaries:	http://www.amlpages.com/plugins.shtml (En)
*						http://www.amlpages.com/Rus/plugins.htm (Ru)
						
*   Support: http://www.amlpages.com/support_home.php
*   ----------------------------------------------------
*	ATTENTION!
*	#define AML_PAGES_PLUGIN macro before #include "plugins.h"
*   ----------------------------------------------------
*
*   This code is free for personal and commercial use, providing this 
*   notice remains intact in the source files and all eventual changes are
*   clearly marked with comments.
*   Send bug reports, bug fixes, enhancements, requests, flames, etc. to
*   amlpages@amlpages.com or post them at the message board at the site.
***************************************************/
#define AML_SAFE_COPY_STR(szTarget,lpszSrc)	_tcsncpy(szTarget,lpszSrc, sizeof(szTarget)/sizeof(szTarget[0]) -1)
#define AML_SAFE_CAT_STR(szTarget,lpszSrc)	_tcsncat(szTarget,lpszSrc, sizeof(szTarget)/sizeof(szTarget[0]) -1)

#ifndef AML_PLUGIN_INCLUDE_STORAGE //storage
#define AML_PLUGIN_INCLUDE_STORAGE
#pragma pack(push)
#pragma pack(8)//8-bytes structure alignment

#include "PluginsEventCode.h"

//Types of pages 
#ifndef AML_PAGES_PLUGIN//if plugins.h in AmlPages.exe
	#include "PageTypeDecl.h"
#else//if plugins.h in plugin project
	enum PAGE_TYPE {PT_TEXT				=0  //Plain Text Page
					,PT_MHT				=1	//Web Archive
					,PT_RTF				=2  //Formatted Text Page
					,PT_HTML			=3  //Web Page
					,PT_FOLDER			=4  //Folder
					,PT_RECYCLED		=5  //Recycled
					,PT_FOLDEROFSTICKS	=6  //Folder of new sticky notes
	};//enum PAGE_TYPE
#endif//#ifndef AML_PAGES_PLUGIN

#ifdef AML_PAGES_PLUGIN
	#ifndef ASSERT
		#ifdef _DEBUG
			#include <assert.h>
			#define ASSERT(X) assert(X)
		#else//#ifdef _DEBUG
			#define ASSERT(X)
		#endif//#ifdef _DEBUG
	#endif//#ifndef ASSERT
#endif//#ifdef AML_PAGES_PLUGIN

#define AML_MAX_NAME 260	

#ifdef AML_PAGES_PLUGIN	//��� ����������� ����� ������ � ������� (���� plugins.h ������ ������� �������)
#ifndef __wtypes_h__
	#include <wtypes.h>//BSTR defintion
#endif//#ifndef __wtypes_h__

#ifndef _RICHEDIT_//if not included RichEdit.h
	#include <richedit.h>// RichEdit API definitions
#endif//#ifndef _RICHEDIT_

#endif//#ifndef AML_PAGES_PLUGIN

/***************************************************
*	(Type Definitions (for non-VC)
****************************************************/
#ifndef UINT
	typedef unsigned int UINT;//unsigned integer (32 bits)
#endif//#ifndef UINT

#ifndef DWORD
	typedef unsigned long DWORD;//unsigned integer (32 bits)
#endif//#ifndef DWORD

//Extra Information of Pages
#define AML_ACTIVEDOCUMENTID			0	//ID of active document
#define AML_NOCHECKPAGEID				-2	//no check Page ID in EnumAttachments
#define AML_REMOVEATTACHMENTTORECYCLED	1	//Remove Attachments to extra info

#define AML_MACRO_SUPPORTS_BUILD	2835
#define AML_MACRO_ACTIVEDOCPATH		TEXT("%AML_MACRO_ACTIVEDOCPATH%")		//The macro expand to full path of the active doc
#define AML_MACRO_ACTIVEDOCTITLE	TEXT("%AML_MACRO_ACTIVEDOCTITLE%")	//The macro expand to tiltle (filename only) of the active doc

enum {APFOD_DONTRESTOREFROMTRAY=1};

struct AML_OPENDOCUMENT {
	DWORD	cbSize;
	LPCTSTR lpszDocumentPath;
	int		nPageID;
	DWORD	dwFlags;
	AML_OPENDOCUMENT()
	{
		memset(this,0,sizeof(AML_OPENDOCUMENT));
		cbSize=sizeof(AML_OPENDOCUMENT);
		nPageID=-1;		
	}
};

struct AML_ATTACHINFO {
	DWORD cbSize;				//Size in bytes of the structure
	int nPageID;				//Page ID of Document
	LPCTSTR lpszInfoID;			//string id of creator
	SYSTEMTIME stDateCreated;	//creation date
	SYSTEMTIME stDateModified;	//modification date
	DWORD dwInfoSize;			//sizeof of array on pInfo
	unsigned char* pInfo;		//pointer to extra info

	AML_ATTACHINFO():cbSize(sizeof(AML_ATTACHINFO))
		,nPageID(-1)
		,lpszInfoID(NULL)
		,dwInfoSize(0)
		,pInfo(NULL)
	{
		GetLocalTime(&stDateCreated);
		stDateModified=stDateCreated;
	}

	~AML_ATTACHINFO()
	{
		Free();
	}
	void Free()
	{
		if (pInfo) {
			const BOOL bSuccess=VirtualFree(pInfo,0,MEM_RELEASE);
			ASSERT(bSuccess);
			pInfo=NULL;
		}
	}

	void Alloc(const DWORD dwSize) {
		ASSERT(0!=dwSize);
		ASSERT(NULL==pInfo);
		pInfo=(unsigned char*)VirtualAlloc(NULL,dwSize,MEM_COMMIT,PAGE_READWRITE);
	}
		
};//struct AML_ATTACHINFO{

//callback function fop AML_ATTACHINFO enumeration
typedef BOOL (*PFUNC_ENUMATTACHMENTSPROC)(int nIndex, AML_ATTACHINFO* pInfo, LPARAM lParam);


//Module Version (.exe, or .dll)
struct AML_MODULEVERSIONINFO {
	DWORD cbSize;		//[in] -  Size in bytes of the structure
	DWORD dwMajor;		//[out] - Major number (e.g. 9.x.x.xxxx)
	DWORD dwMinor;		//[out] - Minor number (����� ������ �����), (e.g. x.0.x.xxxx)
	DWORD dwRevision;	//[out] - Revision number (����� ������ �����),  (e.g. x.x.1.xxxx)
	DWORD dwBuild;		//[out] - Build number (e.g. x.x.x.1814)

	AML_MODULEVERSIONINFO():cbSize(sizeof(AML_MODULEVERSIONINFO))
		,dwMajor(0)
		,dwMinor(0)
		,dwRevision(0)
		,dwBuild(0) {}
};//struct AML_MODULEVERSIONINFO

enum {
	APF_FV_WITHBUILD=0x0001
};

struct AML_FORMAT_VERSION {
	DWORD	cbSize;
	LPCTSTR	lpszPathToFile;
	LPCTSTR	lpszPrefix;
	LPTSTR	lpszResult;
	UINT	cchMaxResult;
	
	DWORD	dwFlags;
	AML_FORMAT_VERSION()
	{
		memset(this,0,sizeof(AML_FORMAT_VERSION));
		cbSize=sizeof(AML_FORMAT_VERSION);
	}

	BOOL IsFlag(const UINT flag) const
	{
		return (flag == (dwFlags & flag));
	}

	BOOL IsWithBuild() const
	{
		return IsFlag(APF_FV_WITHBUILD);
	}
};

#define APF_CNV_BETAVERSION	1

struct AML_CHECKNEWVERSION {
	DWORD					cbSize;
	AML_MODULEVERSIONINFO*	pMV;
	LPTSTR					lpszURL;
	UINT					cchURL;
	DWORD					dwFlags;
	AML_CHECKNEWVERSION()
	{
		memset(this,0,sizeof(AML_CHECKNEWVERSION));
		cbSize=sizeof(AML_CHECKNEWVERSION);
		pMV=new AML_MODULEVERSIONINFO;
		memset(pMV,0,sizeof(AML_MODULEVERSIONINFO));
		pMV->cbSize=sizeof(AML_MODULEVERSIONINFO);

	}
	~AML_CHECKNEWVERSION()
	{
		delete pMV;
	}
};

//����� ��� �����-������ �� RichEdit
enum {
	APFRIO_SEL			=0x0001,	//IO selected text only (or all text)
	APFRIO_PLAINTEXT	=0x0002,	//IO as plaint text (or as RTF)
	APFRIO_IN			=0x0004		//if typed - in text, other out text
};//enum 

//Callback-function for RichEdit IO
typedef void (CALLBACK * PFUNC_AML_RICHEDITIO)(long,long,LPARAM);

//��������� ��� �����-������ ����� �� RichEdit
struct AML_RICHEDITIO {
	DWORD cbSize;						//[in] - ������ ���������, ����������� sizeof(AML_RICHEDITIO)
	DWORD dwFlags;						//[in] - ����� ����� ������
	BSTR bstrText;						//[in\out] - BSTR ������, ������ ������������ ������ (���� ������ ���� APFRIO_IN �������� �������� ������, ����� ����� ������ �������� ���������� �����)
	HWND hwndRichEdit;					//[in] - HWND ���� RichEdit
	PFUNC_AML_RICHEDITIO pfnCallBack;	//[in] - ������� ��������� ������ (�������������)
	LPARAM lParam;						//[in\out] - LPARAM ��� ������� ��������� ������ (�������������)

	AML_RICHEDITIO():cbSize(sizeof(AML_RICHEDITIO))
		,dwFlags(0)
		,bstrText(NULL)
		,hwndRichEdit(NULL)
		,pfnCallBack(NULL)
		,lParam(NULL)
	{
	}
};//struct AML_RICHEDITIO {

//flags of AML_OPTIONS::dwMask member
enum {
	APFOPT_EDITORCOLOR		=1		//member dwEditorColor is valid
   ,APFOPT_BKEDITORCOLOR	=2		//member dwBkEditorColor is valid
   ,APFOPT_EDITORFONT		=4		//members lfEditFont, cfEditorFont is valid
   ,APFOPT_TREECOLOR		=8		//member dwTreeColor is valid 
   ,APFOPT_BKTREECOLOR		=16		//member dwBkTreeColor is valid 
   ,APFOPT_TREEFONT			=32		//member lfTreeFont is valid
   ,APFOPT_BKEDITORBRUSH	=64		//member hBkEditorBrush is valid
   ,APFOPT_BKEDITORFN		=128	//member lpszBkEditorFN is valid
   ,APFOPT_BKSPLITTERFN		=256	//member lpszBkSplitterFN is valid
};//enum 

//Aml Pages Options
struct AML_OPTIONS {
	DWORD cbSize;				//[in]  - Size in bytes of the structure
	DWORD dwMask;				//[in/out]  - members mask
	DWORD dwEditorColor;		//[out] - Editor Font Color, use APFOPT_EDITORCOLOR
	DWORD dwBkEditorColor;		//[out] - Editor Background Color, use APFOPT_BKEDITORCOLOR
#ifdef _UNICODE
	CHARFORMATW cfEditorFont;	//[out] - Editor Font As CHARFORMAT, use APFOPT_EDITORFONT
	LOGFONTW lfEditorFont;		//[out] - Editor Font As LOGFONT, use APFOPT_EDITORFONT
	LOGFONTW lfTreeFont;		//[out] - Tree Font, use APFOPT_TREEFONT
#else
	CHARFORMATA cfEditorFont;	//[out] - Editor Font As CHARFORMAT, use APFOPT_EDITORFONT
	LOGFONTA lfEditorFont;		//[out] - Editor Font As LOGFONT, use APFOPT_EDITORFONT
	LOGFONTA lfTreeFont;		//[out] - Tree Font, use APFOPT_TREEFONT
#endif
	DWORD dwTreeColor;			//[out] - Tree Color, user APFOPT_TREECOLOR
	DWORD dwBkTreeColor;		//[out] - Tree Background Color, use APFOPT_BKTREECOLOR
	HBRUSH hBkEditorBrush;		//[out]	- Editor Background Brush,
	TCHAR  szBkEditorFN[AML_MAX_NAME];		//[in/out] - Editor background picture (full pathname)
	TCHAR  szBkSplitterFN[AML_MAX_NAME];	//[in/out] - Splitter background picture (full pathname)

	AML_OPTIONS()
	{
		memset(this,0,sizeof(AML_OPTIONS));
		cbSize=sizeof(AML_OPTIONS);
		cfEditorFont.cbSize=sizeof(cfEditorFont);		
	}
};//struct AML_OPTIONS {

//����� ��� AML_AUTOTEXTCOMMAND
enum {
	APFAT_CMDRICHEDIT2=0x001,//��� RichEdit 2.0 ��� ����
};//enum 

//��������� ����������� ������� ����������
struct AML_AUTOTEXTCOMMAND {
	DWORD cbSize;	//[in] - ������ ���������, ����������� sizeof(AML_AUTOTEXTCOMMAND)
	BSTR bstrText;	//[in\out] - ��������� �� ������ ������� � ���������� �������
	DWORD dwFlags;	//[in] - ��� RichEdit ������ 2.0 ��� ����
	int nCursorPos;	//[out] - ��������� ���������� ������� � ������� ����� �������
	
	AML_AUTOTEXTCOMMAND():cbSize(sizeof(AML_AUTOTEXTCOMMAND)),bstrText(NULL)
		,dwFlags(0),nCursorPos(0) 
	{}
};//struct AML_AUTOTEXTCOMMAND

//��������� ��� ������ ������� "� �������"
struct AML_PLUGINABOUTINFO {
	DWORD cbSize;			//[in] ������ ���������, ����������� sizeof(AML_PLUGINABOUTINFO)
	HICON hIcon;			//[in] ������ �������
	BSTR bstrTitle;			//[in] �������� ������� (� ��������� �������) (������ ���� ������)
	BSTR bstrNameVersion;	//[in] �������� � ������ ������� (������ ���� ������)
	BSTR bstrAbout;			//[in] ���������� � ������� (��������� �����)
	BSTR bstrLink;			//[in] ����������� �������	
	BSTR bstrCopyrights;	//[in] ��������� ����� (������ ���� ������)
	BSTR bstrURLToolTip;	//[in] ����������� ��������� �����������
	LPCTSTR lpszLinkText;	//[in] ����� ����������� ��� NULL

#ifdef AML_PAGES_PLUGIN//����������� ����� ������ �� ��� Aml Pages
	AML_PLUGINABOUTINFO()
		:cbSize(sizeof(AML_PLUGINABOUTINFO))
		,hIcon(NULL)
		,bstrTitle(NULL)
		,bstrNameVersion(NULL)
		,bstrAbout(NULL)
		,bstrLink(NULL)
		,bstrURLToolTip(NULL)
		,lpszLinkText(NULL)
	{
	}
#endif//#ifdef AML_PAGES_PLUGIN
};//struct AML_PLUGINABOUTINFO

struct AML_MBCHECKINFO {
	DWORD cbSize;				//[in] ������ ��������� � ������, ����������� sizeof(AML_MBCHECKINFO)
	BSTR bstrCaption;			//[in] ��������� ���� MessageBox 
	BSTR bstrText;				//[in] ����� MessageBox
	HWND hwndParent;			//[in] HWND ������������� ����
	UINT uType;					//[in] ��� ������������� (Windows SDK: MB_ICON, MB_OKCANCEL � �.�.)
	BSTR bstrButtonsText;		//[in] ����� Checkbox`�� (���� "������1\n������2\n...\n������N", N ����� checkbox`��)
	BOOL* pbChecked;			//[in\out] ������ ��������� checkbox`��, TRUE(��-����) ��������, FALSE - �� �������� 
	int nRetValue;				//[out] �������� ������������ ����������� �������� MessageBox
	UINT* pnDisabledChecks;		//[in] ������ �������� ����������� ������ (0-N), �� �����������, ����� ���� NULL
	UINT cbDisabledChecksSize;	//[in] ������ ������� pnDisabledChecks 
	HICON hIcon;				//[in] ���������������� ������, ����� ���� NULL
	DWORD dwReserved;			//	   ��������������� ��� ������������� � �������

#ifdef AML_PAGES_PLUGIN
	AML_MBCHECKINFO():cbSize(sizeof(AML_MBCHECKINFO))
		,bstrCaption(NULL)
		,bstrText(NULL)
		,hwndParent(NULL)
		,uType(MB_OK)
		,bstrButtonsText(NULL)
		,pbChecked(NULL)
		,nRetValue(0)
		,pnDisabledChecks(NULL)
		,cbDisabledChecksSize(NULL)
		,hIcon(NULL)
		,dwReserved(NULL)
	{
	}
#endif//#ifdef AML_PAGES_PLUGIN
};//struct AML_MBCHECKINFO {

struct AML_MBRADIOINFO {
	DWORD cbSize;				//[in] ������ ��������� � ������, ����������� sizeof(AML_MBRADIOINFO)
	BSTR bstrCaption;			//[in] ��������� ���� MessageBox
	BSTR bstrText;				//[in] ����� MessageBox
	HWND hwndParent;			//[in] ������������ ����
	UINT uType;					//[in] ��� �������������  (Windows SDK: MB_ICON, MB_OKCANCEL � �.�.)
	BSTR bstrButtonsText;		//[in] ����� ����������� (���� "������1\n������2\n...\n������N", N ����� �����������)
	int nChecked;				//[in\out] ��������� ����������� �� ���������, ����� ������: ��������� �������������
	int nRetValue;				//[out] �������� ������������ �������� MessageBox
	UINT* pnDisabledRadio;		//[in] ������ ����������� ������, �������������, ����� ���� NULL
	UINT cbDisabledRadioSize;	//[in] ������ ������� pnDisabledRadio
	HICON hIcon;				//[in] ���������������� ������, �������������, ����� ���� NULL

#ifdef AML_PAGES_PLUGIN
	AML_MBRADIOINFO():cbSize(sizeof(AML_MBRADIOINFO))
		,bstrCaption(NULL)
		,bstrText(NULL)
		,hwndParent(NULL)
		,uType(MB_OK)
		,bstrButtonsText(NULL)
		,nChecked(0)
		,nRetValue(0)
		,pnDisabledRadio(NULL)
		,cbDisabledRadioSize(NULL)
		,hIcon(NULL)
	{
	}
#endif//#ifdef AML_PAGES_PLUGIN
};//struct AML_MBRADIOINFO {

enum APF_RESOURSEID {
	 APFRID_MENUMAINFRAME	=1	//main menu ID
	,APFRID_MENUDOCUMENT	=2	//menu of document window ID
	,APFRID_ACCMAINFRAME	=3	//accelerator table of main window
};

//���� �� ���������� AML_PAGES_PLUGIN ����� plugins.h ������ ������� Aml Pages
#ifdef AML_PAGES_PLUGIN
	//��������� ������� (������������� AmlPages.exe)
	typedef	LRESULT (__stdcall *PFUNC_APC_COMMAND)					(UINT,WPARAM,LPARAM);					//��������� ������� Aml Pages			(required Aml Pages 9.00 b1742 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_MESSAGEBOXWITHCHECKBOXEX)	(AML_MBCHECKINFO*);						//MessageBox � ����������� CheckBox		(required Aml Pages 9.00 b1740 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_SETSTATUSBARTEXT)			(BSTR);									//�������� ����� � ������ ���������		(required Aml Pages 9.00 b1726 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_MESSAGEBOXWITHRADIO)		(AML_MBRADIOINFO*);						//MessageBox � �������������			(required Aml Pages 9.00 b1726 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_GETMODULEVERSIONINFOEX)	(HMODULE,AML_MODULEVERSIONINFO*);		//�������� ������ ������				(required Aml Pages 9.00 b1711 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_GETMODULEVERSIONINFO)		(LPCTSTR,AML_MODULEVERSIONINFO*);		//�������� ������ ������				(required Aml Pages 9.00 b1625 or later)
	typedef HWND	(__stdcall *PFUNC_APC_GETMAINWND)				(void);									//�������� HWND �������� ���� Aml Pages
	typedef BOOL	(__stdcall *PFUNC_APC_CONVERTTEXT)				(LPCTSTR,BSTR*,BOOL);					//������������� ����� (PlaintText<->RTF)
	typedef int		(__stdcall *PFUNC_APC_MESSAGEBOXWITHCHECKBOX)	(LPCTSTR,UINT,LPCTSTR,BOOL*);			//MessageBox � ��������					(required Aml Pages 9.00 b1593 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_DISPLAYPASSWORDDIALOG)	(BSTR*,BOOL,BOOL,LPCTSTR,BOOL,LPCTSTR);	//������ ������							(required Aml Pages 9.00 b1622 or later)
	typedef UINT	(__stdcall *PFUNC_APC_GETAMLPAGESBUILDNUMBER)	(void);									//�������� ����� ������ Aml Pages		(required Aml Pages 9.00 b1622 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_REREADPLUGINSINFO)		(HMODULE);								//�������� ���� ��������				(required Aml Pages 9.00 b1631 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_GETAUTOTEXTCOMMAND)		(AML_AUTOTEXTCOMMAND*);					//�������� ��������� ����������			(required Aml Pages 9.00 b1672) or later)
	typedef BOOL	(__stdcall *PFUNC_APC_GETOPTIONS)				(AML_OPTIONS*);							//�������� ����� Aml Pages				(required Aml Pages 9.00 b1672 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_PLUGINABOUT)				(AML_PLUGINABOUTINFO*);					//������� "� �������"					(required Aml Pages 9.00 b1688 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_RICHINOUT)				(AML_RICHEDITIO*);						//����-����� �� RichEdit
	typedef BOOL	(__stdcall *PFUNC_APC_ADDATTACHMENT)			(WORD,AML_ATTACHINFO*);					//�������� ����������� ����������
	typedef BOOL	(__stdcall *PFUNC_APC_ENUMATTACHMENTS)			(WORD,PFUNC_ENUMATTACHMENTSPROC, LPCTSTR,int,LPARAM);	//������������ ����������� ����������
	typedef BOOL	(__stdcall *PFUNC_APC_REMOVEATTACHMENT)			(WORD,int);								//�������� ����������� ����������
	typedef BOOL	(__stdcall *PFUNC_APC_SETATTACHMENT)			(WORD,int,AML_ATTACHINFO*);				//��������� ����������� ����������
	typedef int		(__stdcall *PFUNC_APC_GETACTIVEPAGE)			(WORD wDocID);							//��������� �������� �������� ���������	(required Aml Pages 9.01 b1862 or later)
	typedef WORD	(__stdcall *PFUNC_APC_GETUILANGUAGE)			(void);									//��������� ����� �����. ���������� Aml Pages (required Aml Pages 9.01 b1862 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_REMOVEATTACHMENTEX)		(WORD,int,DWORD);						//Extended attachments removing (to recycled bin)(required Aml Pages 9.01 b1870 or later)
	typedef LRESULT (__stdcall *PFUNC_APC_GETRESOURSEID)			(DWORD,DWORD);							//Get Resource ID of AmlPages.exe (1. DWORD - reserved, 2. DWORD - resource type, see enum APF_RESOURSEID)(required Aml Pages 9.46 b2416 or later)
	/***************************************************
	*	������� �������� ��� ������� �������
	*	(������ ��� ������� __��__ ����� AMLPAGES.EXE)
	***************************************************/
	//������� �������� ���������� � �������
	#define SIZE_AML_MENU_ITEM_INFO_VER2 1080	//AML_MENU_ITEM_INFO, ������ 2
	#define SIZE_AML_MENU_ITEM_INFO_VER3 1340	//AML_MENU_ITEM_INFO, ������ 3
	#define SIZE_AML_PAGEINFO_VER2 88			//AML_TREE_PASS, ������
	#define SIZE_AML_PAGECONTENT_VER2 36		//AML_PAGECONTENT, + ���� hwnd
	#define SIZE_AML_INIT_PAGECONTENT_VER2 36	//AML_INIT_PAGECONTENT, + ���� hwnd
	#define SIZE_AML_PLUGIN_INFO_VER2 544//++dwEventMask,++dwFlags,++wLanguage
	#define SIZE_AML_EVENTEDITOR2 48 //+wParam, lParam. ������� Aml Pages 9.05 ������ 1964
	#define SIZE_AML_PAGEINFO_VER3 96			//AML_TREE_PASS, ������ 3
#endif//#ifdef AML_PAGES_PLUGIN

//������������ ����� ������� (������������ � ������ Aml_GetMenuItemInfo, � ����� nPluginType ��������� AML_MENU_ITEM_INFO)
enum PLUGIN_TYPE { APPT_TREEPASS		=1		//����������� ����� ������ ���������
					,APPT_SHOWDIALOG	=2		//����� ������� 
					,APPT_TEXTREAD		=3		//������ ������ ��������, ��������� ������ ��� �������������, ����������� APPT_PAGECONTENT
					,APPT_TEXTREADSEL	=4		//������ ����������� ������ ��������, ��������� ��� �������������, ����������� APPT_PAGECONTENT
					,APPT_PAGECONTENT	=5		//������ (���������) ������ �������� 
					,APPT_EXECCOMMAND	=6		//���������� ������� ���� (reuqired 9.00 b1746 or later)
					};//enum PLUGIN_TYPE

#define AML_STICKYNOTESFOLDER -2
#define AML_ACTIVEFOLDER 0

struct AML_NODEINFO {
	DWORD		cbSize;		//������ ��������� � ������
	int			nPageID;		//ID ��������
	int			nParentID;		//ID parent �����
	PAGE_TYPE	pageType;	//��� ����������� ��������
	LPCTSTR		lpszHeader;	//��������� ����������� ��������
	LPCTSTR		lpszContent;//����������
	DWORD		dwFlags;	//�����, ������� �� ���������
	BSTR		bstrHeader;	//header
	BSTR		bstrContent;//content
	BSTR		bstrTags;		//tags comma delimitted
	SYSTEMTIME	stDateCreated;
	SYSTEMTIME	stDateModified;
	LPCTSTR		lpszQueryDlgTitle;	//Window title for dialog confirmation (added in the Aml Pages 9.89 b2811)
	HMODULE		hModuleFromPlugin;	//HMODULE of plugin for the command as AC_INSERTNODE, confirmation (added in the Aml Pages 9.89 b2811)
	DWORD		_dw_reserved;

	AML_NODEINFO()
	{
		memset(this,0,sizeof(AML_NODEINFO));
		cbSize=sizeof(AML_NODEINFO);
	}
};

#define APFNI2_HEADER		1
#define APFNI2_CONTENT		2
#define APFNI2_TAGS			4
#define APFNI2_DTCREATED	8
#define APFNI2_DTMODIFIED	16

struct AML_NODEINFO2
{
	DWORD cbSize;		//sizeof(AML_NODESINFO2)
	DWORD dwFlags;		//����� ����� ���� ����� APFNI2_HEADER | APFNI2_CONTENT
	int nPageID;		//ID of node 
	BSTR bstrHeader;	//Node header (see flags
	BSTR bstrContent;	//Node content (see flags)
	
	AML_NODEINFO2():cbSize(sizeof(AML_NODEINFO2))
		,nPageID(-1)
		,bstrHeader(NULL)
		,bstrContent(NULL)

	{
	}
};

enum AML_NODESINTERESSED {
	 AMLNI_LASTCREATED				=1	//get info about nodes of last created
	,AMLNI_LASTSTICKYNOTES			=2	//get info about nodes of last sticky notes
	,AMLNI_LASTCREATED_NOWEB		=3	//get info about nodes of last created (exclude HTML and Web Archives)
	,AMLNI_LASTSTICKYNOTES_NOWEB	=4	//get info about nodes of last sticky notes (exclude HTML and Web Archives)
};

struct AML_NODESINFO2
{
	DWORD cbSize;			//sizeof(AML_NODESINFO2)
	AML_NODEINFO2* pData;	//pointer to AML_NODEINFO array
	UINT nCount;			//count of array items
	AML_NODESINTERESSED nNodesInteressed;	//reserved for future
	AML_NODESINFO2():cbSize(sizeof(AML_NODESINFO2))
		,pData(NULL)
		,nCount(0)
		,nNodesInteressed(AMLNI_LASTCREATED)
	{
	}
};

struct AML_CONCTEXT {
	DWORD		cbSize;
	DWORD		dwReserved;
	BSTR		bstrFirst;
	PAGE_TYPE	ptFirst;
	BSTR		bstrSecond;
	PAGE_TYPE	ptSecond;
	BSTR		bstrResult;
	PAGE_TYPE	ptResult;
	AML_CONCTEXT() {
		memset(this,0, sizeof(AML_CONCTEXT));
		cbSize=sizeof(AML_CONCTEXT);
	}
};

struct AML_ADDTEXTTONODE
{
	DWORD cbSize;		//sizeof(AML_ADDTEXTTONODE)
	LPCTSTR lpszText;	//LPCTSTR to added text
	int nAddToPageID;	//int nPageID to add
	AML_ADDTEXTTONODE():cbSize(sizeof(AML_ADDTEXTTONODE))
		,lpszText(NULL)
		,nAddToPageID(-1)
	{
	}
};

enum AML_EVENTMASK {//����� ������� ������ ������ �������� (��. dwEventMask � AML_PLUGIN_INFO)
 	 APFEM_DOCEVENTS	=1	//�������� ������� ����������
	,APFEM_EDITOREVENTS	=2	//�������� ������� ��������� ������
	,APFEM_PAGESEVENTS	=4	//�������� ������� ������� (����\���������\�������
	,APFEM_LOGEVENTS	=8	//�������� ������� �����
};//enum AML_EVENTMASK 


struct AML_EVENTPAGE {
	DWORD		cbSize;		//������ ��������� � ������
	WORD		wDocID;		//ID ���������
	int			nPageID;	//ID ��������
	PAGE_TYPE	pageType;	//��� ��������
	LPCTSTR		lpszDocPath;//���� � ��������� ��� NULL
	LPCTSTR		lpszPageTitle;//��������� �������� ��� NULL
};

struct AML_EVENTDOC {//���������� � ������� � �����������
	DWORD cbSize;				//������ ��������� � ������
	WORD  wDocID;				//��������� ID ���������
	TCHAR szPath[AML_MAX_NAME];	//���� � ���������
	DWORD dwFlags;				//�����
	DWORD dwReserved;			//reserved
};//struct AML_EVENTDOC {

#define APF_DOCUMENTCHANGED		1//���������� ����������� ���������

//dwFlags
enum {APFEE_STICKYNOTESEVENT	=1//event on sticky note
	};

//message EN_CHANGE, EN_SELCHANGE, WM_SETFOCUS, WM_KILLFOCUS
//WM_KEYDOWN,WM_KEYUP,WM_CHAR, WM_MOUSEMOVE
//WM_CREATE,WM_DESTROY
struct AML_EVENTEDITOR {//���������� � ������� ��������� ������
	DWORD cbSize;		//������ ��������� � ������
	HWND hwnd;			//HWND ���� ��������� ������
	UINT nMessage;		//��������� Windows (����������, ����, �����������, WM_NULL � �.�.)
	DWORD dwFlags;		//dwFlags: ��������� � Aml Pages 9.32b2258 ������ � Aml Pages 9.05 build 1964 ����� "��������" ��� ������������ ���������
	POINT point;		//��������� ���� (������ ��� "�������" ���������)
	UINT nReserved2;	//nChar, ������ � Aml Pages 9.05 build 1964 ��� WM_KEYUP, WM_KEYDOWN,WM_CHAR
	UINT nReserver3;	//nRepCnt, ������ � Aml Pages 9.05 build 1964 ��� WM_KEYUP, WM_KEYDOWN,WM_CHAR
	CHARRANGE crSel;	//������� ��������� � ��������� ������
	WPARAM wParam;		//wParam ������� � �������� ���������. ������� Aml Pages 9.05 build 1964
	LPARAM lParam;		//lParam ������� � �������� ���������. ������� Aml Pages 9.05 build 1964
};//struct AML_EVENTEDITORMOUSE {

struct AML_EVENTPANE {
	DWORD	cbSize;			//sizeof(AML_EVENTPANE)
	HWND	hwndPaneHost;	//����-������
	UINT	nWndMessage;	//��������� Windows
	WPARAM	wParamMessage;	//WPARAM ��������� Windows
	LPARAM	lParamMessage;	//LPARAM ��������� Windows
	LPARAM	lPaneData;		//LPARAM ����������� �������� �� ����� ������
};

#define APF_LOG_MESSAGE		1
#define APF_LOG_WARNING		2
#define APF_LOG_ERROR		4

struct AML_EVENTLOG {
	DWORD	cbSize;			//sizeof(AML_EVENTLOG)
	DWORD	dwReserved;		//reserved
	DWORD	dwFlags;		//reserved, do not use now
	LPCTSTR	lpszMessage;	//log message as null-terminated string (Unicode)
	SYSTEMTIME timeEvent;	//local time
	HMODULE	hModuleFrom;	//event generator from where? Aml Pages? Other Plugins?
#ifndef AML_PAGES_PLUGIN
	void Init(const DWORD flags)
	{
		memset(this,0,sizeof(AML_EVENTLOG));
		cbSize=sizeof(AML_EVENTLOG);
		GetLocalTime(&timeEvent);
		dwFlags=flags;
	}
#endif
};

struct AML_EVENTINFO {//���������� � �������					
	DWORD cbSize;						//������ ��������� � ������
	AML_EVENTCODE EventCode;			//��� ������� (��. �������� AML_EVENTCODE)
	union EventInfo {					//���������� � �������
		AML_EVENTDOC	DocEvent;		//���������� � ������� � �����������
		AML_EVENTEDITOR EditorEvent;	//���������� � ������� ��������� ������
		AML_EVENTPAGE	PageEvent;		//���������� � �������� ��������
		AML_EVENTLOG	LogEvent;		//���-c��������
	} EventInfo;//union {

#ifndef AML_PAGES_PLUGIN
	AML_EVENTINFO(AML_EVENTCODE code)
	{
		memset(this,0,sizeof(AML_EVENTINFO));
		cbSize=sizeof(AML_EVENTINFO);
		ASSERT(sizeof(AML_EVENTINFO) == cbSize);
		ASSERT(code);
		ASSERT((code >= AEC_FIRST) && (code <= AEC_LAST));
		EventCode=code;
		switch(EventCode) {
		case AEC_DOCOPEN:
		case AEC_DOCCLOSE:
		case AEC_DOCACTIVATE:
		case AEC_DOCSAVE:
			EventInfo.DocEvent.cbSize=sizeof(AML_EVENTDOC);
			break;
		
		case AEC_EDITOR:
			EventInfo.EditorEvent.cbSize=sizeof(AML_EVENTEDITOR);
			break;			
		
		case AEC_EXITAMLPAGES:
		case AEC_STARTAMLPAGES:
		case AEC_CLOSEMAINWND:
			break;
		
		case AEC_PAGEOPEN:
		case AEC_PAGESAVE:
		case AEC_PAGEDELETE:
		case AEC_PAGEADDED:
			EventInfo.PageEvent.cbSize=sizeof(AML_EVENTPAGE);
			break;

		case AEC_LOGMESSAGE:
			EventInfo.LogEvent.cbSize=sizeof(AML_EVENTLOG);
			break;

		default:
			ASSERT(FALSE);
			break;
		};
	}
#endif//#ifndef AML_PAGES_PLUGIN
};//struct AML_EVENTINFO {

#define APF_APUNICODEVERSION		1 //flag is valid for Unicode version of Aml Pages (see AML_PLUGIN_INFO::dwFlags)
#define APF_PLUGINSUPPORTUNICODE	2 //Plugin support Unicode (required for U-version of plugins, Set the flag AML_PLUGIN_INFO::dwFlags on Aml_GetPluginInfo)

//��������� ���������� � �������
struct AML_PLUGIN_INFO {
	DWORD  cbSize;						//[in] ������ � ��������� ������, ����������� sizeof(AML_PLUGIN_INFO)
	UINT   nMenuCount;					//[out] ����� ��������� ���� (������� ��� �������� Aml_GetMenuItemInfo)
	TCHAR  szMenuName[AML_MAX_NAME];	//[in\out] �������� ������� ������� (���� nMenuCount>1)
	TCHAR  szDescription[AML_MAX_NAME];	//[in\out] �������� ������� ������� � ������ ���������
	UINT   nMinRelease;					//[out] ����������� ����� ������ Aml Pages ��� ������ � ��������, ���� 0 (�� ���������):�������� ����� Aml Pages
	DWORD  dwEventMask;					//[out] ������� ��������� � ������� ������ �������� ������
	DWORD  dwFlags;						//[in\out] ��������������� ��� ������������� � �������
	WORD   wLanguage;					//[in] language of Aml Pages interface (required Aml Pages 9.00.1842), see Windows primary languages ID (may LANG_NEUTRAL if english)

#ifndef AML_PAGES_PLUGIN//��������� � ������������ ����� ������ � Aml Pages �����
	AML_PLUGIN_INFO& operator=(const AML_PLUGIN_INFO& info) 
	{
		if (&info != this) {
			cbSize		=info.cbSize;
			nMenuCount	=info.nMenuCount;
			
//			
//			_tcscpy(szDescription,info.szDescription,sizeof(szDescription)/sizeof(TCHAR));
			ASSERT(!IsBadStringPtr(info.szMenuName,INFINITE));
			memset(szMenuName,0, sizeof(szMenuName));
			_tcsncpy(szMenuName,info.szMenuName,ARRAYSIZE(szMenuName)-1);
			ASSERT(!IsBadStringPtr(szMenuName,ARRAYSIZE(szMenuName)));
			szMenuName[ARRAYSIZE(szMenuName)-1]=TEXT('\0');
			
			ASSERT(!IsBadStringPtr(info.szDescription,INFINITE));
			memset(szDescription,0,sizeof(szDescription));
			_tcsncpy(szDescription,info.szDescription, ARRAYSIZE(szDescription)-1);
			ASSERT(!IsBadStringPtr(szDescription,ARRAYSIZE(szDescription)));
			szDescription[ARRAYSIZE(szDescription)-1]=TEXT('\0');
			
			nMinRelease	= info.nMinRelease;
			dwEventMask	= info.dwEventMask;
			dwFlags		= info.dwFlags;
			wLanguage	= info.wLanguage;
		}

		return *this;
	}
	AML_PLUGIN_INFO():
		 nMenuCount(0)
		,dwEventMask(0)
		,dwFlags(0)
	{
		memset(this,0,sizeof(AML_PLUGIN_INFO));
		cbSize=sizeof(AML_PLUGIN_INFO);
		szMenuName[0]=TEXT('\0');
		szDescription[0]=TEXT('\0');
		nMinRelease=0;
#ifdef _UNICODE
		dwFlags|=APF_APUNICODEVERSION;
#endif
	}
	AML_PLUGIN_INFO(const AML_PLUGIN_INFO& info)
	{
		(*this)=info;
	}
#endif//#ifndef AML_PAGES_PLUGIN
};//struct AML_PLUGIN_INFO

/***************************************************
*	����� ��� ���������� � ����������� ����
*	��������� � ������ (������ � �����������)
***************************************************/

enum {
	 APF_MENUTREE			=0x0001		//�������� � ���� ������
	,APF_MENUEDIT			=0x0002		//�������� � ���� ���������
	,APF_MENUSHLIST			=0x0004		//�������� � ���� �������������� ��������� (9.0.0.1743 � ������)
	,APF_MENUNOPLUGINSMENU	=0x0008		//�� �������� � ���� "�������" (9.0.0.1743 � ������)
	,APF_MENUABOUTINFO		=0x0010		//��������� ��� ������ "� ������� ���_�������" � ������ "� ���������"
	,APF_MENUSYSTRAY		=0x0020		//��������� � ���� ��������� ������
	,APF_MENUFAVORITES		=0x0040		//insert into "Favorites" menu
	,APF_MENUTOOLBARS		=0x0080		//insert into "View\ToolBars" menu
	,APF_MENUINSERT			=0x0100		//insert into "Insert" menu
	,APF_MENUTOOLS			=0x0200		//insert into "Insert" menu
	,APF_MENUFILESAVEGROUP	=0x0400		//insert into "File" menu group
	,APF_MENUVIEW			=0x0800		//insert into "View" menu group
	,APF_MENUEXPORT			=0x1000		//insert into "Export" menu group
	,APF_MENUSPLITTER		=0x2000		//insert into Splitter (tree\editor) popup menu
	,APF_MENUSETTINGS		=0x4000		//insert into Quick Settings popup menu
};//enum 

//��������� ���������� �� �������� ����
struct AML_MENU_ITEM_INFO {
	DWORD cbSize;							//[in] ������ ��������� � ������, ����������� sizeof(AML_MENU_ITEM_INFO)
	TCHAR szMenuName[AML_MAX_NAME];			//[out] �������� �������� ����
	TCHAR szSBHint[AML_MAX_NAME];			//[out] ����� � ������ ��������� (�������������)
	TCHAR szTTHint[AML_MAX_NAME];			//[out] ����� �� ������. ��������� (�������������)
	char szFunctionName[AML_MAX_NAME];		//[out] ��� ������������� ������� � DLL �������, ���������� ��� ������ ����
	PLUGIN_TYPE nPluginType;					//[out] ��� ���������� ������� DLL ������� (��. ������������ PLUGIN_TYPE)
	LPARAM lParam;							//[out] ���������������� ��������, � AmlPages.exe �� ������������, �� ��������� 0
	HICON hIcon;							//[out] ������ � ������ ����
	COLORREF crTextColor;					//      ���� ������ � ����
	DWORD reserved2;						//      ��������� ������ ��� ������������� � �������� �������� ��������,/*BOOL bCanModified*/;
	UINT nMinRelease;						//[out] �����.����� ������ ��������� Aml Pages � ������� ����� �������� ����� ����. ���� 0 (�� ���������) ����� Aml Pages
	UINT nMenuState;						//[out] ���� ������� MF_CHECKED ���� ��������
	DWORD reserved3;						//		���������������,/*BOOL bReserved2*/
	DWORD dwFlags;							//[out] ����� ��������� �������� � �������������� ����
	char szInitFunctionName[AML_MAX_NAME];	//[out] ��� ������� ������������� (����� �� � ����� ������� ��������� � szFunctionName, �������������

#ifndef AML_PAGES_PLUGIN//������������ � ��������� ����� ������ ������ ������� Aml Pages
	AML_MENU_ITEM_INFO()
	{
		memset(this,0,sizeof(AML_MENU_ITEM_INFO));
		
		cbSize=sizeof(AML_MENU_ITEM_INFO);
		szMenuName[0]=TEXT('\0');
		szSBHint[0]=TEXT('\0');
		szTTHint[0]=TEXT('\0');
		szFunctionName[0]=TEXT('\0');
		lParam=0;
		hIcon=NULL;
		nMinRelease=0;
		crTextColor=GetSysColor(COLOR_MENUTEXT);
		reserved2=0;
		nMenuState=0;
		reserved3=0;
		dwFlags=0;	//�� ��������� ������ �� ����� ���� �������� � ���������� ���� �������� � ������
		szInitFunctionName[0]=TEXT('\0');
		nPluginType=(PLUGIN_TYPE)0;
	}
	
	AML_MENU_ITEM_INFO& operator=(const AML_MENU_ITEM_INFO& info) 
	{
#ifdef _DEBUG//compile-time �������� ����� char
		TCHAR compile_time_test_char_size[sizeof(TCHAR)-sizeof(TCHAR)+1]={0};
#endif//#ifdef _DEBUG

		if (this != (&info)) {		
			nPluginType=info.nPluginType;
			
			memset(szMenuName,0,sizeof(szMenuName));
			_tcsncpy(szMenuName,info.szMenuName,ARRAYSIZE(szMenuName)-1);
			szMenuName[ARRAYSIZE(szMenuName)-1]=TEXT('\0');
	
			memset(szSBHint,0,sizeof(szSBHint));
			_tcsncpy(szSBHint,info.szSBHint,ARRAYSIZE(szSBHint)-1);
			szSBHint[ARRAYSIZE(szSBHint)-1]=TEXT('\0');


			memset(szTTHint,0,sizeof(szTTHint));
			_tcsncpy(szTTHint,info.szTTHint,ARRAYSIZE(szTTHint)-1);
			szTTHint[ARRAYSIZE(szTTHint)-1]=TEXT('\0');

			memset(szFunctionName,0,sizeof(szFunctionName));
			strncpy(szFunctionName,info.szFunctionName,ARRAYSIZE(szFunctionName)-1);
			szFunctionName[ARRAYSIZE(szFunctionName)-1]=TEXT('\0');

			lParam=info.lParam;
			nMinRelease=info.nMinRelease;
			nMenuState=info.nMenuState;
			reserved2=info.reserved2;
			//ASSERT(GetSysColor(COLOR_MENUTEXT)==info.crTextColor);
			crTextColor=info.crTextColor;
			reserved3=info.reserved3;
			
			if (hIcon) {
				::DestroyIcon(hIcon);
			}

			if (info.hIcon) {
				hIcon=::CopyIcon(info.hIcon);			
			}
			else {
				hIcon=NULL;
			}

			if (info.hIcon) {
				::DestroyIcon(info.hIcon);
				const_cast<AML_MENU_ITEM_INFO&>(info).hIcon=NULL;
			}

			dwFlags=info.dwFlags;
			memset(szInitFunctionName,0,sizeof(szInitFunctionName));
			strncpy(szInitFunctionName,info.szInitFunctionName,ARRAYSIZE(szInitFunctionName)-1);
			szInitFunctionName[ARRAYSIZE(szInitFunctionName)-1]=TEXT('\0');
		}//if (this!=(&info)) {
		
		return *this;
	}
	AML_MENU_ITEM_INFO(const AML_MENU_ITEM_INFO& info) 
	{
		(*this)=info;
	}
	~AML_MENU_ITEM_INFO()
	{
		if (hIcon) {
			const BOOL b=::DestroyIcon(hIcon);
			ASSERT(b);
		}
	}
#endif//#ifndef AML_PAGES_PLUGIN
};//struct AML_MENU_ITEM_INFO

/***************************************************
*	��������� ���������� �����, ���������
*	������ ��� ������������� � �������� ��������
*	���������
***************************************************/	
struct AML_TEXTREAD {
	DWORD cbSize;		//[in] ������ ���������, ����������� sizeof(AML_TEXTREAD)
	LPCTSTR lpszText;	//[in] ����� �������� ��������� Aml Pages
	DWORD cbBufferSize;	//[in] ������ � ������ ������ � lpszText

#ifndef AML_PAGES_PLUGIN//����������� ����� ������ � Aml Pages
	AML_TEXTREAD():cbSize(sizeof(AML_TEXTREAD)),lpszText(NULL),cbBufferSize(0) {}
#endif//#ifndef AML_PAGES_PLUGIN
};//struct AML_TEXTREAD

//����� ������ ������
enum {
	APFTP_STARTSELFOLDER=0x0001	//�������� ����� ������ � ���������� �����, ���� 0 (�� ���������), �� � �������� ����� ��������� Aml Pages
	,APFTP_APPLYSH=0x0002		//�������� �������� � ���������� ������������, ���� PT_TEXT �� ������� ������������� � RTF
};//enum 


/***************************************************
*	��������� ������������ � ����� � �
*	������ ������ ������
***************************************************/
struct AML_INIT_TREEPASS {
	DWORD cbSize;	//[in] ������ ��������� � ������, ������������ sizeof(AML_INIT_TREEPASS)
	BOOL bStart;	//[in] TRUE ����� ������� ������, � FALSE � ����� ������
	LPARAM lParam;	//[in\out] ���������������� ��������, ��������� � Aml_GetMenuItemInfo
	BOOL bContinue;	//[in\out] ������������ ���� bStart==FALSE
	DWORD dwFlags;	//[out] �����, 0 ����� � ����, APFTP_STARTSELFOLDER  ����� � ���������� �����

#ifndef AML_PAGES_PLUGIN//����������� ����� ������ � ����� Aml Pages
	AML_INIT_TREEPASS():cbSize(sizeof(AML_INIT_TREEPASS)),bStart(TRUE)
		,lParam(NULL),bContinue(TRUE),dwFlags(0) {}
#endif//#ifndef AML_PAGES_PLUGIN
};//struct AML_INIT_TREEPASS

/***************************************************
*	��������� ��� ������ ������
***************************************************/
struct AML_PAGEINFO {
	DWORD cbSize;				//[in] ������ ���������, ����������� sizeof(AML_PAGEINFO)
	SYSTEMTIME stDateCreated;	//[in] ���� �������� ��������
	SYSTEMTIME stDateModified;	//[in] ���� ����������� ��������
	LPCTSTR lpszFullPagePath;	//[in] ������ ���� �������� � ��������� Aml Pages (������ � ����� C, � ����������� '\0')
	HICON hIcon;				//[in] node icon
	LPCTSTR lpszHeader;			//[in] ��������� ��������\����� (������ � ����� C, � ����������� '\0')
	DWORD nParentPageID;			//     support Aml Pages 9.02 b1898 or later
	LPCTSTR lpszContent;		//[in] ����� �������� (������ � ����� C, � ����������� '\0')
	DWORD lCursorPos;			//[in] ��������� ������� � ������ support Aml Pages 9.02 b1909 or later
	BOOL reserved4;				//[in] ��������� ��� ������������� � ������. �������� �������� (bFolder)(bFormattedText)
	BOOL* pbContinue;			//[in\out] ���������� �� �����? �� ��������� ������ (TRUE), ���������� (FALSE)
	BOOL bRoot;					//[in] ��������� ����� ������ (TRUE), ����� (FALSE)
	PAGE_TYPE pageType;			//[in] ��� �������� (��. ������������ ����)
	UINT nNumber;				//[in] ����� ��������� �������� 0 ��� ������, ������ 1, 2, 3 � �.�.
	LPARAM lParam;				//[in\out] ���������������� ��������
	int nPageID;				//[in] ���������� ������������� ��������(�����) � ���������
	BYTE* pbContent;			
	DWORD dwLenContent;
	DWORD dwCodePage;


#ifndef AML_PAGES_PLUGIN//����������� ����� ������ � ����� Aml Pages
	AML_PAGEINFO()
	{
		memset(this,0,sizeof(AML_PAGEINFO));
		cbSize=sizeof(AML_PAGEINFO);
		nParentPageID=nPageID=(-1);
	}
#endif//#ifndef AML_PAGES_PLUGIN
} ;//struct AML_PAGEINFO

enum {
	 APFSP_PTTEXT			=1
	,APFSP_PTRTF			=2
	,APFSP_PTHTML			=4
	,APFSP_PTMHT			=8
	,APFSP_PTRECYCLED		=16
	,APFSP_PTFOLDEROFSTICKS	=32
};

struct AML_SELECTPAGEEX {
	DWORD		cbSize;
	DWORD		dwFlags;
	int			nPageID;
	PAGE_TYPE	ptType;
	LPCTSTR		lpszDlgTitle;
	RECT		rectDlg;
	int			nExcludePageID;
	AML_SELECTPAGEEX()
	{
		memset(this,0,sizeof(AML_SELECTPAGEEX));
		cbSize=sizeof(AML_SELECTPAGEEX);
		ptType=(PAGE_TYPE)-1;
		dwFlags=APFSP_PTFOLDEROFSTICKS|APFSP_PTTEXT|APFSP_PTTEXT;
		nExcludePageID=-1;
	}
};

enum {
	 APFS_EDITORCOLOR		=1	//edit text color is valid
	,APFS_EDITORBK			=2	//editor background color is valid
	,APFS_TREECOLOR			=4	//tree text color is valid
	,APFS_TREEBK			=8	//tree background color is valid
	,APFS_EDITORBKPIC		=16	//editor background picture is valid
	//,APFS_EDITORFONTLF	=32	//lfEditorFont is valid, reserved for future
	,APFS_TREEFONTLF		=64	//lfTreeFont is valid,
	,APFS_EDITORFONTCF		=128//cfEditorFont is valid
	,APFS_TREEBORDERCOLOR	=256//color of border of selected item
};

enum {
	 APFS2_BOOKMARK_BK_ENABLED		=1	//�������� �������� ���� ��� �������
	,APFS2_BOOKMARK_BK_COLOR		=2	//���� ��� �������
	,APFS2_BK_ON_KILLFOCUS_ENABLED	=4	//�������� �� ������ ���� ��� ������ ������
	,APFS2_BK_ON_KILLFOCUS_DELTA	=8	//������ ��� ������ �����
	,APFS2_FULL_PATHNAME_IN_TITLE	=16	//���������� �� ���� � ��������� ����
	,APFS2_XP_STYLES_FOR_DOCS_TABS	=32	//������������ ����� XP ��� ������ �������� ����������
};

//Aml Pages Skin
struct AML_SKIN {
	DWORD cbSize;
	DWORD dwMask;
	DWORD dwEditorColor;				//[in/out] - Editor Font Color, use APFS_EDITORCOLOR
	DWORD dwBkEditorColor;				//[in/out] - Editor Background Color, use APFS_EDITORBK
	DWORD dwTreeColor;					//[in/out] - Tree Color, user APFS_TREECOLOR
	DWORD dwBkTreeColor;				//[in/out] - Tree Background Color, use APFS_TREEBK
	TCHAR  szBkEditorFN[AML_MAX_NAME];	//[in/out] - Editor background picture (full pathname),APFS_EDITORBKPIC
	//reserved for future using
#ifdef _UNICODE
	CHARFORMATW cfEditorFont;			//[out] - Editor Font As CHARFORMAT, APFS_EDITORFONTCF
	LOGFONTW reservedlfEditorFont;				//[out] - Editor Font As LOGFONT, APFS_EDITORLF
	LOGFONTW lfTreeFont;					//[out] - Tree Font, APFS_TREEFONTLF
#else
	CHARFORMATA cfEditorFont;			//[out] - Editor Font As CHARFORMAT, APFS_EDITORFONTCF
	LOGFONTA reservedlfEditorFont;				//[out] - Editor Font As LOGFONT, APFS_EDITORLF
	LOGFONTA lfTreeFont;					//[out] - Tree Font, APFS_TREEFONTLF
#endif
	DWORD dwTreeBorderColor;			//[out]	- Color of border of selected item of tree
	DWORD dwMask2;
	DWORD dwBkBookMarkColor;
	BYTE  bDeltaBkColorOnKillFocus;
	DWORD dwValue2;
	AML_SKIN()
	{
		InitDefault();
	}

	void InitDefault()
	{
		memset(this,0,sizeof(AML_SKIN));
		cbSize=sizeof(AML_SKIN);		
	}
};

struct AML_EXECCOMMAND {
	DWORD cbSize;		//[in] ������ ��������� � ������, ����������� sizeof(AML_EXECCOMMAND)
	LPARAM lParam;		//[in\out] lParam �� ����, ��� ������� ��������������
	DWORD dwReserved;	//	��������������� ��� ������������� � �������

#ifndef AML_PAGES_PLUGIN//����������� ������������ ������ � ������� Aml Pages
	AML_EXECCOMMAND(LPARAM _lparam)
		:cbSize(sizeof(AML_EXECCOMMAND))
		,lParam(_lparam)
		,dwReserved(0)
	{
	}
#endif// #ifndef AML_PAGES_PLUGIN
};//struct AML_EXECCOMMAND

struct AML_INIT_EXECCOMMAND {
	DWORD cbSize;		//[in] ������ ��������� � ������, ������������ sizeof(AML_INIT_EXECCOMMAND)
	BOOL bStart;		//[in] TRUE ���� ����� �������������, FALSE ���� ����� ����������
	BOOL bContinue;		//[in\out] ���������� �� ������ (������������ ���� bStart==FALSE)
	LPARAM lParam;		//[in\out] ���������������� �������� �� ������ Aml_GetMenuItemInfo
	DWORD dwReserved;	//��������������� ��� ������������� � �������

#ifndef AML_PAGES_PLUGIN//����������� ������������ ������ � ������� Aml Pages
	AML_INIT_EXECCOMMAND(BOOL start,LPARAM _lparam):cbSize(sizeof(AML_INIT_EXECCOMMAND))
		,lParam(_lparam)
		,bStart(start)
		,bContinue(TRUE)
		,dwReserved(0)
	{
	}
#endif// #ifndef AML_PAGES_PLUGIN
};//struct AML_INIT_EXECCOMMAND

/***************************************************
��������� ��� ��������� ������ �������� ��������,
***************************************************/
struct AML_PAGECONTENT {
	const DWORD cbSize;			//[in] ������ ��������� � ������, ����������� sizeof(AML_PAGECONTENT)
	BSTR bstrContent;			//[in\out] ����� ��������
	DWORD dwFlags;				//[in\out] ����� ��������� ������
	const PAGE_TYPE pageType;	//[in] ��� �������� ������
	BOOL bChanged;				//[out] ���������� � TRUE (��-����), ���� ������ �������� ��������
	CHARRANGE crSel;			//[in\out] ��������� � ��������� ������
	LPARAM lParam;				//[in\out] ���������������� ��������
	const HWND hwnd;			//[in] HWND ���� ��������� ������ (required Aml Pages 9.00 b1818 or later)
	const int nPageID;
	const WORD wDocID;

#ifndef AML_PAGES_PLUGIN//����������� ����� ������ � ����� Aml Pages	
	AML_PAGECONTENT(PAGE_TYPE page_type
		, const HWND _hwnd
		, const int page_id
		, const WORD doc_id
		):pageType(page_type)
		,cbSize(sizeof(AML_PAGECONTENT))
		,dwFlags(0)
		,bChanged(FALSE) 
		,lParam(0)
		,bstrContent(NULL)
		,hwnd(_hwnd)
		,nPageID(page_id)
		,wDocID(doc_id)
	{
		crSel.cpMin=crSel.cpMax=0;
	}		
#endif//#ifndef AML_PAGES_PLUGIN
};//struct AML_PAGECONTENT

struct AML_DISLAYCOLORDLG {
	DWORD cbSize;
	HWND hwndParent;
	POINT ptTopLeft;
	COLORREF crColor;
#ifdef AML_PAGES_PLUGIN//����������� ����� ������ � ����� Aml Pages	
	AML_DISLAYCOLORDLG()
	{
		memset(this,0,sizeof(AML_DISLAYCOLORDLG));
		cbSize=sizeof(AML_DISLAYCOLORDLG);
	}
#endif//AML_PAGES_PLUGIN//����������� ����� ������ � ����� Aml Pages	
};//struct AML_DISLAYCOLORDLG {

//����� ������� AP_PAGECONTENT (��������� AML_PAGECONTENT.dwFlags)
enum {
	 APFPC_SEL=0x0001		//���������� � ������ ������ ���������� � ��������� �����, ���� �� ����������� (����) ���������� ���� ����� ��������
	,APFPC_PLAINTEXT=0x0002	//���������� ����� � ������� Plain Text, ���� �� ����������� � ������� RTF
};//enum 

struct AML_INIT_PAGECONTENT {
	const DWORD cbSize;			//[in] ������ ��������� � ������,����������� sizeof(AML_INIT_PAGECONTENT)
	BOOL bStart;				//[in] TRUE: ������������� (����� �� �������� �������), FALSE: ���������� (����� ����� �������� �������)
	const PAGE_TYPE pageType;	//[in] ��� ��������, �� ������������ � PageTypeDecl.h
	DWORD dwFlags;				//[in\out] ����� ��������� ������, �� ����� APFPC_xxxxx
	CHARRANGE crSel;			//[in] ��������� � ��������� ������
	BOOL bContinue;				//[in\out] ��������� �� ������ (������������ ����� �������� �������), ���� �������� ���������� � FALSE (�� ��������� TRUE)
	LPARAM lParam;				//[in\out] ���������������� ��������
	const HWND hwnd;			//[in] HWND ���� ��������� ������ (required Aml Pages 9.00 b1818 or later)
	const int nPageID;
	const WORD wDocID;


#ifndef AML_PAGES_PLUGIN//����������� ����� ������ � ����� Aml Pages	
	AML_INIT_PAGECONTENT(PAGE_TYPE page_type
		, const HWND _hwnd
		, int page_id
		, WORD doc_id
		):pageType(page_type)
		,cbSize(sizeof(AML_INIT_PAGECONTENT))
		,bStart(TRUE)
		,dwFlags(0)
		,bContinue(TRUE)
		,lParam(0)
		,hwnd(_hwnd)
		,nPageID(page_id)
		,wDocID(doc_id)
	{
		crSel.cpMin=crSel.cpMax=0;
	}
#endif//#ifndef AML_PAGES_PLUGIN
};//struct AML_INIT_PAGECONTENT

struct AML_PANEINFO {
	DWORD	cbSize;				//sizeof(AML_PANEINFO)
	DWORD	dwFlags;			//flags, reserved
	LPARAM	lPaneData;			//user defined data associated with pane
	LPTSTR	lpszPaneName;		//pane title (required)
	UINT	cchMaxPaneName;	//max of pane title (symbols)
	LPTSTR	lpszPaneHint;		//pane tooltip (optional)
	UINT	cchMaxPaneHint;		//max of pane tooltip (symbols)
	HICON	hIcon;
	GUID	guidPane;			//unique GUID identifer (reserved, need for saving)
	
#ifndef AML_PAGES_PLUGIN
	AML_PANEINFO()
	{
		memset(this,0,sizeof(AML_PANEINFO));
		cbSize=sizeof(AML_PANEINFO);
	}
#endif
};

enum { APFPANECOMMAND_SHOW		= 0	//show pane
	  ,APFPANECOMMAND_HIDE		= 1	//hide pane
	  ,APFPANECOMMAND_TOGGLE	= 2	//tooggle pane visible
};

struct AML_PANECOMMAND {
	DWORD cbSize;
	GUID guid;
	DWORD dwCommand;

	AML_PANECOMMAND()
	{
		memset(this,0,sizeof(AML_PANECOMMAND));
		cbSize=sizeof(AML_PANECOMMAND);
	}

};

struct AML_PANECREATE {
	DWORD	cbSize;
	GUID	guidPane;
	LPARAM	lPaneData;
	SIZE	sizePane;
	int		nIndex;
#ifndef AML_PAGES_PLUGIN
	AML_PANECREATE()
	{
		memset(this,0,sizeof(AML_PANECREATE));
		cbSize=sizeof(AML_PANECREATE);
	}
#endif
};



#ifndef AML_PAGES_PLUGIN 
	/***************************************************
	*	FUNCTIONS
	*	(��������� �� ������� ����� ������ � �����
	*	������� Aml Pages)
	/***************************************************/
	//������� ��������� ������� ������
	typedef LRESULT (*PFUNC_PANEEVENTPROC)(int nIndex, AML_EVENTPANE*);
	//��������� ���������� � ������� (-1 || p==NULL - get panels count, else get pane info by index)
	typedef UINT (*PFUNC_AML_GETPANELINFO)(int nIndex, AML_PANEINFO*);
	//������������ �� ������ Unicode (�������� ������, �� Aml Pages
	typedef BOOL (*PFUNC_AML_ISSUPPORTSPLUINUNICODE)(LPVOID);

	//��������� ���������� � �������, ����������� ��� ����������
	typedef BOOL (*PFUNC_AML_GETPLUGININFO)(AML_PLUGIN_INFO* pInfo);
	
	//��������� ���������� � �������� ���� ,����������� ��� ����������
	typedef BOOL (*PFUNC_AML_GETMENUITEMINFO)(UINT nIndex, AML_MENU_ITEM_INFO* pInfo);

	//��������� ���������� � �������
	typedef UINT (*PFUNC_AML_GETPANEINFO)(UINT nIndex, AML_PANEINFO*);

	//�������� ������
	//typedef HWND (*PFUNC_AMLPANE_CREATE)(UINT nIndex, HWND hwndParent, SIZE size, LPARAM lParam);
	typedef HWND (*PFUNC_AMLPANE_CREATE)(HWND hwndParent, AML_PANECREATE* p);

	typedef LRESULT (*PFUNC_AML_UNLOADPLUGIN)(LPVOID);

	typedef LRESULT (*PFUC_AMLPANE_NOTIFYRECEIVED)(HWND hwnd, WPARAM, LPARAM);

	//����� ������, �������������� �� �������,�������� ������������ PLUGIN_TYPE: AP_TREEPASS
	typedef BOOL (*PFUNC_AML_TREEPASS)(AML_PAGEINFO* pTreePass);
	
	//������� �������������,�������� ������������ PLUGIN_TYPE: AP_TREEPASS
	typedef BOOL (*PFUNC_AML_INIT_TREEPASS)(AML_INIT_TREEPASS*);
	
	//����� �������, �������� ������������ PLUGIN_TYPE: AP_SHOWDIALOG
	typedef BOOL (*PFUNC_AML_SHOWDIALOG)(HWND);
	
	/*������ ������ �� ��������,
	*	��������
	*	��������� ������ ��� ������������� � ����������� ��������,
	*	����������� AP_PAGECONTENT	*/
	typedef BOOL (*PFUNC_AML_TEXTREAD)(AML_TEXTREAD*);
	
	//��������� ������ ��������, (�������� ������������ PLUGIN_TYPE: AP_PAGECONTENT)
	typedef BOOL (*PFUNC_AML_PAGECONTENT)(AML_PAGECONTENT*);
	
	//������� �������������, (�������� ������������ PLUGIN_TYPE:)
	typedef BOOL (*PFUNC_AML_INIT_PAGECONTENT)(AML_INIT_PAGECONTENT*);
	
	//���������� ������� ����, �������� ������������ PLUGIN_TYPE:AP_EXECCOMMAND
	typedef BOOL (*PFUNC_AML_EXECCOMMAND)(AML_EXECCOMMAND*);

	//������� �������������, �������� ������������ PLUGIN_TYPE:AP_EXECCOMMAND
	typedef BOOL (*PFUNC_AML_INIT_EXECCOMMAND)(AML_INIT_EXECCOMMAND*);
	//������� �������, ���������� ������� Aml Pages
	typedef BOOL (*PFUNC_AML_EVENTHANDLER)(AML_EVENTINFO*);
	//������� �������������� RTF2HTML
	typedef BOOL (*PFUNC_AML_RTF2HTML)(BSTR);
#endif //#ifndef AML_PAGES_PLUGIN

#define SIZE_AML_SKIN_VER1 464
#define SIZE_AML_SKIN_VER2 840

enum {
	APF_SETUPPWD	=0x0001
};

struct AML_PASSWORD_INFO {
	DWORD	cbSize;
	DWORD	dwFlags;
	LPCTSTR	lpszDlgTitle;
	LPTSTR	lpszPassword;
	UINT	cchMaxPwd;
	LPTSTR	lpszHint;
	UINT	cchMaxHint;
	LPCTSTR lpszPrevPassword;
	HICON	hIcon;
	DWORD	dwReserved;

	AML_PASSWORD_INFO() {
		memset(this,0,sizeof(AML_PASSWORD_INFO));
		cbSize=sizeof(AML_PASSWORD_INFO);
	}

private:
	BOOL _IsFlag(const DWORD flag) const
	{
		return (flag == (dwFlags & flag));
	}
public:
	BOOL IsSetupPassword() const
	{
		return _IsFlag(APF_SETUPPWD);
	}
};

struct AML_WRITELOG2	{
	DWORD	cbSize;
	DWORD	dwFlags;
	LPCTSTR lpszMessage;
	LPCTSTR	lpszPrefix;
	HMODULE	hModuleFromPlugin;
	DWORD	dwReserved;
	AML_WRITELOG2() {
		memset(this,0,sizeof(AML_WRITELOG2));
	}

	void Init(const HMODULE hDLLFromPlugin, LPCTSTR _lpszPrefix, LPCTSTR _lpszMessage, const DWORD flagsReason)
	{
		memset(this,0,sizeof(AML_WRITELOG2));
		cbSize = sizeof(AML_WRITELOG2);
		hModuleFromPlugin=hDLLFromPlugin;
		dwFlags=flagsReason;
		if (_lpszPrefix) {
			ASSERT(!IsBadStringPtr(_lpszPrefix,INFINITE));
			lpszPrefix=_lpszPrefix;
		}

		if (_lpszMessage) {
			ASSERT(!IsBadStringPtr(_lpszMessage,INFINITE));
			lpszMessage=_lpszMessage;
		}
	}
};

#ifndef AML_PAGES_PLUGIN 
	/***************************************************
	*	������� �������� ����� ������ Aml Pages
	*
	*	������� ������ ������ �������� (������ ��,
	*	������� ���������� � AMLPAGES.EXE)
	***************************************************/
	//(� ������� ��� ���������������� ������������� � ������������)
	#define SIZE_AML_OPTIONS_VER2			76//������ AML_OPTIONS ������ 2
	#define SIZE_AML_PLUGINABOUTINFO_VER2	32//������ ��������� AML_PLUGINABOUTINFO ������ 2
	#define SIZE_AML_PLUGINABOUTINFO_VER3	36//������ ��������� AML_PLUGINABOUTINFO ������ 3
	#define SIZE_AML_PAGECONTENT_VER2		38//+hwnd
	#define SIZE_AML_PAGECONTENT_VER3		44//+wDocID, +nPageID
	#define SIZE_AML_INIT_PAGECONTENT_VER3	44//+wDocID, +nPageID
	#define SIZE_AML_SELECTPAGEEX_VER2		40//+nExcludePageID
#endif //#ifndef AML_PAGES_PLUGIN 


	#define SIZE_AML_OPTIONS_VER3 204//������ AML_OPTIONS ������ 3(+dwBkTreeColor,dwTreeColor,lfTreeFont, build 1867)
	#define SIZE_AML_OPTIONS_VER4 208	 //������ AML_OPTIONS ������ 4(+hBkEditorBrush, build 2000)
	#define SIZE_AML_OPTIONS_VER5 728	 //������ AML_OPTIONS ������ 5(sz, build 2078)
	#define INVALID_DOCUMENT_ID -1
	enum {
	 AMLUC_FILE_DOCUMENTSAVE			=1//lParam=0   - with default compression, lParam>0 with comression, lParam<0 - without compression
	,AMLUC_FIRST						= AMLUC_FILE_DOCUMENTSAVE
	,AMLUC_TOOLS_SETDOCUMENTPASSWORD	=2//lParam !=0 - password for saving, lParam==0 password for openining
	,AMLUC_TOOLS_CURSORTOALL			=3//lParam !=0 - all cursors to end, lParam == 0 all cursor to begin
	,AMLUC_EDIT_SAVEPOSITION			=4//remeber current position (text caret and selected node)
	,AMLUC_TOOLS_DOCUMENTPARAMETRS		=5//Display document parameters dialog for the active document. Reguired Aml Pages 9.05 b1953
	,AMLUC_FILE_FILESMANAGEMENT			=6//Display files management dialog. Reguired Aml Pages 9.05 b1953
	,AMLUC_LAST							= AMLUC_FILE_FILESMANAGEMENT
	};//enum

	//Document has not saved changes? see AC_DOCUMENTHASCHANGES
	enum {
		 APFDS_DOCHASCHANGES	=1	//���� � ��������� ������������� ���������
		,APFDS_DOCNOHASCHANGES	=-1	//� ��������� ��� ������������� ���������
		,APFDS_DOCERROR			=0	//������ (��� ������ ��������� ���������)
	};
	enum AC_COMMAND //commands for APC_Command
	{
	 AC_GETACTIVEPAGEID			=1	//wParam - DocID (AML_ACTIVEDOCUMENTID), retval=Pages ID, AmlPages 9.01 build1882
	,AC_GETPAGEHEADER			=2	//wParam - Page ID, lParam LPTSTR (is NULL) return bytes
	,AC_SHOWTREE				=3	//wParam - Doc ID AML_ACTIVEDOCUMENTID), lParam==1 - show, ==0, Hide, retval=TRUE on success
	//,AC_RELOADLINKSEXT		=4	//OBSOLETE Reload linksext.ini file
	,AC_NEWSTICKYNOTE			=5	//New Sticky Note, wParam==TRUE (insert clipboard in content), LPARAM - path to document (LPARAM support by Aml Pages 9.03 build 1922 or later)
	,AC_GETMRUFILESLIST			=6	//Get MRU files list from Aml Pages, lParam - LPTSTR, WPARAM - number of BYTES!!! in lParam, ret val "file1\nfile2\nfile3...\file_last\n\n", required Aml Pages 9.02 Beta build 1987
	,AC_OPENDOCUMENT			=7	//Open document file, LPARAM zero-based string to full pathname. required RetVal: DocID or INVALID_DOCUMENT_ID on fail. Aml Pages 9.02 Beta build 1897
	,AC_COPYNODE				=8	//Copy document node wParam - src node id, lParam - nParent node id, TRUE on success. required Aml Pages 9.02 Beta build 1897 
	,AC_MOVENODE				=9	//Move document node wParam - src node id, lParam - nParent node id, TRUE on success. required Aml Pages 9.02 Beta build 1897 
	,AC_GETACTIVEDOCUMENTPATH	=10	//Get active document file path or title, wParam - size in bytes, lParam - LPCTSTR, ret val == size in bytes. Required Aml Pages 9.02 Beta build 1898
	,AC_OPENDOCUMENTEX			=11	//open document and activate page, wParam - Page ID, LPARAM - full document file path name. RetVal: DocID or INVALID_DOCUMENT_ID on fail. Required Aml Pages 9.02 Beta build 1898
	,AC_SHOWMAINWINDOW			=12	//show Aml Pages main window (wParam=SC_RESTORE,SC_MINIMIZE) Required Aml Pages 9.02 Beta 2 build 1900
	,AC_SHOWPAGEINTREE			=13	//show node of page in tree, wParam - DocID, lParam - nodeID  Required Aml Pages 9.02 Beta 2 build 1903
	,AC_IMPORTFILE				=14	//Import file to active doc, WPARAM - Folder|-1 to active folder, LPARAM - LPCTSTR lpszFullPath.Required Aml Pages 9.02 Final build 1908, Imported ID or -1 on fail
	,AC_DISPLAYFOLDERDIALOG		=15	//Display Dialog With Selection Folder,wParam Selected ID, Pointer to rect. RetVal=selected folder id, or -1, Reqired Aml Pages 9.02 Final build 1909
	,AC_SELECTPAGE				=16	//Select page in document. wParam - DocID, lParam poiter to AML_PAGEINFO. RetVal=TRUE|FALSE. Required Aml Pages 9.02 Final build 1909
	,AC_GETMANAGEMENTFILESLIST	=17	//Get Managed files list from Aml Pages, lParam - LPTSTR, WPARAM - number of BYTES!!! in lParam, ret val "file1\nfile2\nfile3...\file_last\n\n", required Aml Pages 9.03 Beta build 1920
	,AC_ISREADONLYDOCUMENT		=18	//Is Read Only Document. wParam==Document ID. RetVal: TRUE|FALSE. Required Aml Pages 9.03 Beta build 1920 or later
	,AC_READTIPOFTHEDAY			=19	//Read tip of the day, wParam - buffer size in bytes, LPARAM - LPTSTR. Required Aml Pages 9.03 Beta build 1920 or later
	,AC_DISPLAYFAVORITESMENU	=20	//Display favorites menu of active document, LPARAM - MAKELPARAM(x,y) (screen coordinate)  Required Aml Pages 9.03 Beta build 1922 or later
	,AC_DOCUMENTPATHTOID		=21	//Get Doc ID by path (if document opened). WPARAM - full path to document, RetVal: Doc ID or INVALID_DOCUMENT_ID on fail; Required Aml Pages 9.03 Beta build 1933 or later
	,AC_EXTRACTPAGEICON			=22 //Extract Icon of page, wParam DocID, LPARAM nPageID, retval=hIcon or NULL, Required Aml Pages 9.03 Beta build 1933 or later
	,AC_DISPLAYASSTICKYNOTE		=23 //Display page of document as sticky note. LOWORD (wParam) = Document ID, HIWORD(wParam) - TRUE - display, FALSE undisplay, LPARAM nPageID. RetValue: Not 0 on success, 0 on fail. Required Aml Pages 9.03 Beta build 1934 or later
	,AC_SETOPTIONS				=24 //Set options. LPARAM AML_OPTIONS pointer, RetVal = TRUE on success, else FALSE. Required Aml Pages 9.03 Final Build 1938
	,AC_INSERTNODE				=25 //Insert node to document. wParam=DocID, LPARAM pointer AC_INFO. RetVal new node ID or -1 or fail. Required Aml Pages 9.04 Beta build 1942 or later
	,AC_NEWDOCUMENT				=26 //Create new document. No parameters. Ret Val=TRUE if successfully, or FALSE. Required Aml Pages 9.04 Beta build 1942 or later
	,AC_EXECUSERCOMMAND			=27 //Send WM_COMMAND for the active document. WPARAM - User Command, LPARAM paramters. Ret Val=TRUE if sucessfully, or FALSE. Required Aml Pages 9.04 build 1951 or later. See commands and parameters top in enum {AMLUC_***}
	,AC_INSERTPICTUREFROMFILE	=28 //Insert picture to text editor from file. WPARAM - Document ID, LPARAM - LPCTSTR lpszFilename, RetValue: TRUE - successfully, FALSE or fail. Required Aml Pages 9.05 build 1953 or later.
	,AC_GETPAGETYPE				=29 //Get page type. wParam == wDocID, lParam ==nPageID, Return value page_type, or -1 or fail. Required Aml Pages 9.10 build 2019 or later.
	,AC_GETNODESINFO			=30	//Get information about sticky notes info wParam - Document ID, LPARAM pointer to AML_NODESINFO, RetValue - count of filled on -1 or error. Required Aml Pages 9.14 build 2068 or later.
	,AC_ADDTEXTTONODE			=31	//Add new text to node data wParam - Document ID, LPARAM pointer to AML_ADDTEXTTONODE, RetValue - TRUE or FALSE on fail. Required Aml Pages 9.14 build 2068 or later.
	,AC_CONCATENATETEXT			=32 //Concatenate two text of varranty types. LPARAM pointer to AML_CONCTEXT, RetValue - TRUF if successfully, FALSE on fail. Required Aml Pages 9.15 build 2076 or later
	,AC_GETSKIN					=33 //Get Aml Pages Skin. LPARAM pointer to AML_SKIN. RetValue - TRUE is successfully. FALSE on fail. Required Aml Pages 9.16 build 2092 or later
	,AC_SETSKIN					=34 //Set Aml Pages Skin. LPARAM pointer to AML_SKIN. RetValue - TRUE is successfully. FALSE on fail. Required Aml Pages 9.16 build 2092 or later
	,AC_GETDOCSTABSLIST			=35	//Get document tabs list from Aml Pages (all opened document and permanent tabs), lParam - LPTSTR, WPARAM - number of BYTES!!! in lParam, ret val "file1\nfile2\nfile3...\file_last\n\n", required Aml Pages 9.15 Alpha build 2079
	,AC_DISPLAYCOLORDLG			=36 //Show Advances Color Dialog, LPARAM pointer to AML_DISLAYCOLORDLG, ret-value: BOOL is color choosed, Required Aml Pages 9.16 build 2102
	,AC_GETDEDAULTSKIN			=37 //Retrieve default skin from Aml Pages options (lParam - pointer to AML_SKIN, ret_value: BOOL is successfull, FALSE on failure. Required Aml Pages 9.21 build 2159
	,AC_DOCUMENTHASCHANGES		=38	//Retrieve has document not saved changed.(wParam - Document ID or ACTIVEDOCUMENT_ID),retvalue: see enum APFDS_***. Required Aml Pages 9.23 build 2178
	,AC_GETOPTIONINIPATH		=39	//Retirieve full pathname to Aml Pages options INI-file. WPARAM - number of TCHARs, LPARAM LPTSTR lpszPath. RetValue: number of copied TCHAR, if wParam=0, number of need TCHARs. 0 - Error. Required Aml Pages 9.23 build 2178
	,AC_SELECTPAGEEX			=40 //Select page in document. wParam - DocID, lParam pointer to AML_SELECTPAGEEX. RetVal=TRUE|FALSE. Required Aml Pages 9.31 build 2254
	,AC_OPENDOCUMENTEX2			=41	//Open document ex 2 with flags, lParam pointer to AML_OPENDOCUMENT. RetVal: DocID or INVALID_DOCUMENT_ID on fail. Required Aml Pages 9.33 Beta build 2274
	,AC_GETPARENTFOLDERID		=42	//Get Parent Folder ID - wParam - wDocID, LPARAM - nPageID;RetVal: DocID Parent Folder ID. Required Aml Pages 9.46 Beta 3 build 2393 or later
	,AC_MAKELINKAPDURL			=43	//Make link apd://, lParam pointer to AML_MAKELINKAPDURL,  Required Aml Pages 9.57 build 2507 or later, RerValue: number of TCHAR in result
	,AC_GETNODEINFO				=44	//Get Node Info, wParam - Document ID, lParam - pointer to AML_NODEINFO, ,  Required Aml Pages 9.58 build 2511 or later, RerValue: TRUE|FALSE
	,AC_HIGHLIGHTALLINRE		=46	//Highligt All (ahs, bookmarks) in current RichEdit. RetVal - TRUE, Required Aml Pages 9.58 build 2511 or later
	,AC_GETRIGHTPANEHWND		=47	//Get HWND of rigt pane of Aml Pages (text editor, folder and etc), Required Aml Pages 9.58 build 2512 or later. WPARAM - document id, LPARAM* pointer to DWORD (optional, flags)Ret val: HWND of right pane
	,AC_CHECKNEWVERSION			=48	//Check new version of Aml Pages on Internet. Required Aml Pages 9.62 build 2538 or later. , LPARAM* pointer to AML_MODULEVERSIONINFO. Ret val: LRESULT  - TRUE if success
	,AC_PANECOMMAND				=49	//Handle command of plugin Pane, (LPARAM - ptr to AML_PANECOMMAND)
	,AC_WRITELOG				=50	//Write message to the log file, (WPARAM) - LPCTSTR to the prefix, (LPARAM) - LPCTSTR to the messageRetVal - TRUE, Required Aml Pages 9.66 build 2571 or later
	,AC_BLINKCTRL				=51	//Blink control	- LPARAM (HWND) of control
	,AC_FORMATVERSION			=52	//Format version, LPARAM - pointer to AML_FORMAT_VERSION. Required Aml Pages 9.82 build 2712 or later. Ret val: LRESULT  - UINT count of format symbols if success, 0 - is failure
	,AC_SHOW_PASSWORD_DLG		=53	//Show password dialog,LPARAM � pointer to AML_PASSWORD_INFO, ret val - 1, is press OK, or 0 - is press FALSE, or -1 on errir
	,AC_CHECK_FOR_UPDATES		=54	//Click on menu command "Help\Check For Updates"
	,AC_GETSETTINGS				=55	//Get Settings of Aml Pages lParam - pointer to AmL_SETTINGS, , Required Aml Pages 9.88 build 2796 or later
	,AC_SETSETTINGS				=56 //set Settings of Aml Pages lParam - pointer to AmL_SETTINGS, , Required Aml Pages 9.88 build 2796 or later
	,AC_WRITELOG2				=57	//WriteLog2, lParam - ptr to AML_WRITELOG2, retval=BOOL, , Required Aml Pages 9.88 build 2797 or later
	,AC_SHOW_NEWS				=58	//Click on menu command "View\Bars\News"retval=BOOL, Required Aml Pages 9.88 build 2809 or later
	,AC_ALLOW_DND_FROM_IE		=59	//Allow DND from protected IE mode, wParam - TRUE, with user report, LPARAM - HWND of parent report (NULL use Aml Pages main window) , Required Aml Pages 9.94 build 2858 or later
	,AC_ISALLOW_DND_FROM_IE		=60	//IS Allow DND from protected IE mode, LPARAM - ptr to BOOL, Required Aml Pages 9.94 build 2858 or later
};

//Settings of AP
struct AML_SETTINGS	 {
	DWORD cbSize;		//sizeof (AML_SETTINGS)
	DWORD dwMask;		//mask for query/set settings values
	DWORD dwValue;		//values settings specified in mask
	AML_SETTINGS() {
		memset(this,0,sizeof(AML_SETTINGS));
	}
};

//////////////////////////////////////////////////////////////////////////
//mask/values of settings of Aml Pages for struct AML_SETTINGS
#define APF_SETTINGS_WRITELOG		1	//Write Log is enabled

#define APFRIGHTPANETYPE_RICHEDIT	1
#define APFRIGHTPANETYPE_HTMLVIEW	2
#define APFRIGHTPANETYPE_LISVIEW	4

struct AML_MAKEAPDLINKURL {
	DWORD	cbSize;
	WORD	wDocID;
	LPCTSTR	lpszDocPathName;
	int		nPageID;
	LPCTSTR	lpszBookmarkName;
	LPTSTR	lpszLinkResult;
	UINT	cchTextMax;

#ifndef AML_PAGES_PLUGIN 
	AML_MAKEAPDLINKURL()
	{
		memset(this,0,sizeof(AML_MAKEAPDLINKURL));
		cbSize=sizeof(AML_MAKEAPDLINKURL);
		nPageID=-1;
		wDocID=INVALID_DOCUMENT_ID;
	}
#endif
};

#define AML_NODEINFO_SIZE2	36
#define AML_NODEINFO_SIZE3	68
#define AML_NODEINFO_SIZE4	80

 
#pragma pack(pop)
#endif //#ifndef AML_PLUGIN_INCLUDE_STORAGE
