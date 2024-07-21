//Aml Pages Plugin API Version
#define AML_PAGES_PLUGIN_API_VERSION TEXT("9.94 build 2858")

/***************************************************
*	Aml Pages Plugin API header
*	Version: 9.94 build 2858 ( May 17, 2022 )
*
*	(©) 1998-2022, Mazov Gosha aka Carc
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

#ifdef AML_PAGES_PLUGIN	//эти определения нужны только в плагине (если plugins.h внутри проекта плагина)
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
	DWORD dwMinor;		//[out] - Minor number (после первой точки), (e.g. x.0.x.xxxx)
	DWORD dwRevision;	//[out] - Revision number (после второй точки),  (e.g. x.x.1.xxxx)
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

//флаги для ввода-вывода из RichEdit
enum {
	APFRIO_SEL			=0x0001,	//IO selected text only (or all text)
	APFRIO_PLAINTEXT	=0x0002,	//IO as plaint text (or as RTF)
	APFRIO_IN			=0x0004		//if typed - in text, other out text
};//enum 

//Callback-function for RichEdit IO
typedef void (CALLBACK * PFUNC_AML_RICHEDITIO)(long,long,LPARAM);

//структура для ввода-вывода строк из RichEdit
struct AML_RICHEDITIO {
	DWORD cbSize;						//[in] - размер структуры, используйте sizeof(AML_RICHEDITIO)
	DWORD dwFlags;						//[in] - флаги ввода вывода
	BSTR bstrText;						//[in\out] - BSTR строка, память распределяет плагин (если указан флаг APFRIO_IN содержит вводимую строку, иначе после вызова содержит выведенный текст)
	HWND hwndRichEdit;					//[in] - HWND окна RichEdit
	PFUNC_AML_RICHEDITIO pfnCallBack;	//[in] - функция обратного вызова (необязательно)
	LPARAM lParam;						//[in\out] - LPARAM для функции обратного вызова (необязательно)

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

//флаги для AML_AUTOTEXTCOMMAND
enum {
	APFAT_CMDRICHEDIT2=0x001,//для RichEdit 2.0 или выше
};//enum 

//структура описывающая команду автотекста
struct AML_AUTOTEXTCOMMAND {
	DWORD cbSize;	//[in] - размер структуры, используйте sizeof(AML_AUTOTEXTCOMMAND)
	BSTR bstrText;	//[in\out] - указатель на строку команды и результата команды
	DWORD dwFlags;	//[in] - для RichEdit версии 2.0 или выше
	int nCursorPos;	//[out] - положение текстового курсора в команде после вставки
	
	AML_AUTOTEXTCOMMAND():cbSize(sizeof(AML_AUTOTEXTCOMMAND)),bstrText(NULL)
		,dwFlags(0),nCursorPos(0) 
	{}
};//struct AML_AUTOTEXTCOMMAND

//структура для показа диалога "О плагине"
struct AML_PLUGINABOUTINFO {
	DWORD cbSize;			//[in] размер структуры, используете sizeof(AML_PLUGINABOUTINFO)
	HICON hIcon;			//[in] иконка плагина
	BSTR bstrTitle;			//[in] название плагина (в заголовке диалога) (только одна строка)
	BSTR bstrNameVersion;	//[in] название и версия плагина (только одна строка)
	BSTR bstrAbout;			//[in] информаиия о плагине (несколько строк)
	BSTR bstrLink;			//[in] гиперссылка плагина	
	BSTR bstrCopyrights;	//[in] авторские права (только одна строка)
	BSTR bstrURLToolTip;	//[in] всплывающая подсказка гиперссылки
	LPCTSTR lpszLinkText;	//[in] текст гиперссылки или NULL

#ifdef AML_PAGES_PLUGIN//конструктор нужен только ВО ВНЕ Aml Pages
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
	DWORD cbSize;				//[in] размер структуры в байтах, используйте sizeof(AML_MBCHECKINFO)
	BSTR bstrCaption;			//[in] заголовок окна MessageBox 
	BSTR bstrText;				//[in] текст MessageBox
	HWND hwndParent;			//[in] HWND родительского окна
	UINT uType;					//[in] тип мессайджбокса (Windows SDK: MB_ICON, MB_OKCANCEL и т.д.)
	BSTR bstrButtonsText;		//[in] текст Checkbox`ов (вида "строка1\nстрока2\n...\nстрокаN", N число checkbox`ов)
	BOOL* pbChecked;			//[in\out] массив состояния checkbox`ов, TRUE(не-ноль) отмечена, FALSE - не отмечена 
	int nRetValue;				//[out] значение возвращенное стандартной функцией MessageBox
	UINT* pnDisabledChecks;		//[in] массив индексов отключенных кнопок (0-N), не обязательно, может быть NULL
	UINT cbDisabledChecksSize;	//[in] размер массива pnDisabledChecks 
	HICON hIcon;				//[in] пользовательская иконка, может быть NULL
	DWORD dwReserved;			//	   зарезервировано для использования в будущем

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
	DWORD cbSize;				//[in] размер структуры в байтах, используйте sizeof(AML_MBRADIOINFO)
	BSTR bstrCaption;			//[in] заголовок окна MessageBox
	BSTR bstrText;				//[in] текст MessageBox
	HWND hwndParent;			//[in] родительское окно
	UINT uType;					//[in] тип мессайджбокса  (Windows SDK: MB_ICON, MB_OKCANCEL и т.д.)
	BSTR bstrButtonsText;		//[in] текст радиокнопок (вида "строка1\nстрока2\n...\nстрокаN", N число радиокнопок)
	int nChecked;				//[in\out] выбранная радиокнопка по умолчанию, после вызова: выбранная пользователем
	int nRetValue;				//[out] значение возвращенное функцией MessageBox
	UINT* pnDisabledRadio;		//[in] массив отключенных кнопок, необязательно, может быть NULL
	UINT cbDisabledRadioSize;	//[in] размер массива pnDisabledRadio
	HICON hIcon;				//[in] пользовательская иконка, необязательно, может быть NULL

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

//если не определена AML_PAGES_PLUGIN тогда plugins.h внутри проекта Aml Pages
#ifdef AML_PAGES_PLUGIN
	//сервисные функции (предоставляет AmlPages.exe)
	typedef	LRESULT (__stdcall *PFUNC_APC_COMMAND)					(UINT,WPARAM,LPARAM);					//Выполнить команду Aml Pages			(required Aml Pages 9.00 b1742 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_MESSAGEBOXWITHCHECKBOXEX)	(AML_MBCHECKINFO*);						//MessageBox с несколькими CheckBox		(required Aml Pages 9.00 b1740 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_SETSTATUSBARTEXT)			(BSTR);									//Показать текст в строке состояния		(required Aml Pages 9.00 b1726 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_MESSAGEBOXWITHRADIO)		(AML_MBRADIOINFO*);						//MessageBox с радиокнопками			(required Aml Pages 9.00 b1726 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_GETMODULEVERSIONINFOEX)	(HMODULE,AML_MODULEVERSIONINFO*);		//Получить версию модуля				(required Aml Pages 9.00 b1711 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_GETMODULEVERSIONINFO)		(LPCTSTR,AML_MODULEVERSIONINFO*);		//Получить версию модуля				(required Aml Pages 9.00 b1625 or later)
	typedef HWND	(__stdcall *PFUNC_APC_GETMAINWND)				(void);									//Получить HWND главного окна Aml Pages
	typedef BOOL	(__stdcall *PFUNC_APC_CONVERTTEXT)				(LPCTSTR,BSTR*,BOOL);					//Преобразовать текст (PlaintText<->RTF)
	typedef int		(__stdcall *PFUNC_APC_MESSAGEBOXWITHCHECKBOX)	(LPCTSTR,UINT,LPCTSTR,BOOL*);			//MessageBox с галочкой					(required Aml Pages 9.00 b1593 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_DISPLAYPASSWORDDIALOG)	(BSTR*,BOOL,BOOL,LPCTSTR,BOOL,LPCTSTR);	//Диалог пароля							(required Aml Pages 9.00 b1622 or later)
	typedef UINT	(__stdcall *PFUNC_APC_GETAMLPAGESBUILDNUMBER)	(void);									//Получить номер сборки Aml Pages		(required Aml Pages 9.00 b1622 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_REREADPLUGINSINFO)		(HMODULE);								//Обновить меню плагинов				(required Aml Pages 9.00 b1631 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_GETAUTOTEXTCOMMAND)		(AML_AUTOTEXTCOMMAND*);					//Получить выражение автотекста			(required Aml Pages 9.00 b1672) or later)
	typedef BOOL	(__stdcall *PFUNC_APC_GETOPTIONS)				(AML_OPTIONS*);							//Получить опции Aml Pages				(required Aml Pages 9.00 b1672 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_PLUGINABOUT)				(AML_PLUGINABOUTINFO*);					//Диалога "О плагине"					(required Aml Pages 9.00 b1688 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_RICHINOUT)				(AML_RICHEDITIO*);						//Ввод-вывод из RichEdit
	typedef BOOL	(__stdcall *PFUNC_APC_ADDATTACHMENT)			(WORD,AML_ATTACHINFO*);					//Добавить расширенную информацию
	typedef BOOL	(__stdcall *PFUNC_APC_ENUMATTACHMENTS)			(WORD,PFUNC_ENUMATTACHMENTSPROC, LPCTSTR,int,LPARAM);	//Перечисление расширенной информации
	typedef BOOL	(__stdcall *PFUNC_APC_REMOVEATTACHMENT)			(WORD,int);								//Удаление расширенной информации
	typedef BOOL	(__stdcall *PFUNC_APC_SETATTACHMENT)			(WORD,int,AML_ATTACHINFO*);				//Изменение расширенной информации
	typedef int		(__stdcall *PFUNC_APC_GETACTIVEPAGE)			(WORD wDocID);							//получение активной страницы документа	(required Aml Pages 9.01 b1862 or later)
	typedef WORD	(__stdcall *PFUNC_APC_GETUILANGUAGE)			(void);									//получение языка польз. интерфейса Aml Pages (required Aml Pages 9.01 b1862 or later)
	typedef BOOL	(__stdcall *PFUNC_APC_REMOVEATTACHMENTEX)		(WORD,int,DWORD);						//Extended attachments removing (to recycled bin)(required Aml Pages 9.01 b1870 or later)
	typedef LRESULT (__stdcall *PFUNC_APC_GETRESOURSEID)			(DWORD,DWORD);							//Get Resource ID of AmlPages.exe (1. DWORD - reserved, 2. DWORD - resource type, see enum APF_RESOURSEID)(required Aml Pages 9.46 b2416 or later)
	/***************************************************
	*	РАЗМЕРЫ СТРУКТУР ДЛЯ ФУНКЦИЙ ПЛАГИНА
	*	(ТОЛЬКО ТЕХ КОТОРЫЕ __НЕ__ НУЖНЫ AMLPAGES.EXE)
	***************************************************/
	//Размеры структур информации о плагине
	#define SIZE_AML_MENU_ITEM_INFO_VER2 1080	//AML_MENU_ITEM_INFO, версия 2
	#define SIZE_AML_MENU_ITEM_INFO_VER3 1340	//AML_MENU_ITEM_INFO, версия 3
	#define SIZE_AML_PAGEINFO_VER2 88			//AML_TREE_PASS, версия
	#define SIZE_AML_PAGECONTENT_VER2 36		//AML_PAGECONTENT, + член hwnd
	#define SIZE_AML_INIT_PAGECONTENT_VER2 36	//AML_INIT_PAGECONTENT, + член hwnd
	#define SIZE_AML_PLUGIN_INFO_VER2 544//++dwEventMask,++dwFlags,++wLanguage
	#define SIZE_AML_EVENTEDITOR2 48 //+wParam, lParam. Требует Aml Pages 9.05 сборка 1964
	#define SIZE_AML_PAGEINFO_VER3 96			//AML_TREE_PASS, версия 3
#endif//#ifdef AML_PAGES_PLUGIN

//перечисление типов функций (используется в вызове Aml_GetMenuItemInfo, в члене nPluginType структуры AML_MENU_ITEM_INFO)
enum PLUGIN_TYPE { APPT_TREEPASS		=1		//Рекурсивный обход дерева документа
					,APPT_SHOWDIALOG	=2		//Показ диалога 
					,APPT_TEXTREAD		=3		//Чтение текста страницы, оставлено ТОЛЬКО для совместимости, используйте APPT_PAGECONTENT
					,APPT_TEXTREADSEL	=4		//Чтение выделенного текста страницы, оставлено для совместимости, используйте APPT_PAGECONTENT
					,APPT_PAGECONTENT	=5		//Чтение (изменение) текста страницы 
					,APPT_EXECCOMMAND	=6		//Выполнение команды меню (reuqired 9.00 b1746 or later)
					};//enum PLUGIN_TYPE

#define AML_STICKYNOTESFOLDER -2
#define AML_ACTIVEFOLDER 0

struct AML_NODEINFO {
	DWORD		cbSize;		//размер структуры в байтах
	int			nPageID;		//ID страницы
	int			nParentID;		//ID parent папки
	PAGE_TYPE	pageType;	//тип вставляемой страницы
	LPCTSTR		lpszHeader;	//заголовок вставляемой страницы
	LPCTSTR		lpszContent;//содержание
	DWORD		dwFlags;	//флаги, зависят от контекста
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
	DWORD dwFlags;		//флаги какая инфа нужна APFNI2_HEADER | APFNI2_CONTENT
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

enum AML_EVENTMASK {//какие события плагин должен получать (см. dwEventMask в AML_PLUGIN_INFO)
 	 APFEM_DOCEVENTS	=1	//получать события документов
	,APFEM_EDITOREVENTS	=2	//получать события Редактора текста
	,APFEM_PAGESEVENTS	=4	//получать события страниц (откр\сохранить\удалить
	,APFEM_LOGEVENTS	=8	//получать события логов
};//enum AML_EVENTMASK 


struct AML_EVENTPAGE {
	DWORD		cbSize;		//размер структуры в байтах
	WORD		wDocID;		//ID документа
	int			nPageID;	//ID страницы
	PAGE_TYPE	pageType;	//Тип страницы
	LPCTSTR		lpszDocPath;//Путь к документу или NULL
	LPCTSTR		lpszPageTitle;//заголовок страницы или NULL
};

struct AML_EVENTDOC {//информация о событии с документами
	DWORD cbSize;				//размер структуры в байтах
	WORD  wDocID;				//внутрений ID документа
	TCHAR szPath[AML_MAX_NAME];	//путь к документу
	DWORD dwFlags;				//флаги
	DWORD dwReserved;			//reserved
};//struct AML_EVENTDOC {

#define APF_DOCUMENTCHANGED		1//сохранение измененного документа

//dwFlags
enum {APFEE_STICKYNOTESEVENT	=1//event on sticky note
	};

//message EN_CHANGE, EN_SELCHANGE, WM_SETFOCUS, WM_KILLFOCUS
//WM_KEYDOWN,WM_KEYUP,WM_CHAR, WM_MOUSEMOVE
//WM_CREATE,WM_DESTROY
struct AML_EVENTEDITOR {//информация о событии Редактора текста
	DWORD cbSize;		//размер структуры в байтах
	HWND hwnd;			//HWND окна Редактора текста
	UINT nMessage;		//сообщение Windows (клавиатура, мышь, нотификация, WM_NULL и т.д.)
	DWORD dwFlags;		//dwFlags: Добавлено в Aml Pages 9.32b2258 Убрано в Aml Pages 9.05 build 1964 флаги "мышинных" или клавиатурных сообщений
	POINT point;		//положение мыши (только для "мышиных" сообщений)
	UINT nReserved2;	//nChar, Убрано в Aml Pages 9.05 build 1964 для WM_KEYUP, WM_KEYDOWN,WM_CHAR
	UINT nReserver3;	//nRepCnt, Убрано в Aml Pages 9.05 build 1964 для WM_KEYUP, WM_KEYDOWN,WM_CHAR
	CHARRANGE crSel;	//текущее выделение в Редакторе текста
	WPARAM wParam;		//wParam мышиных и клавовых сообщений. Требует Aml Pages 9.05 build 1964
	LPARAM lParam;		//lParam мышиных и клавовых сообщений. Требует Aml Pages 9.05 build 1964
};//struct AML_EVENTEDITORMOUSE {

struct AML_EVENTPANE {
	DWORD	cbSize;			//sizeof(AML_EVENTPANE)
	HWND	hwndPaneHost;	//хост-панель
	UINT	nWndMessage;	//сообщение Windows
	WPARAM	wParamMessage;	//WPARAM сообщения Windows
	LPARAM	lParamMessage;	//LPARAM сообщения Windows
	LPARAM	lPaneData;		//LPARAM сохраненное плагином на этапе опроса
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

struct AML_EVENTINFO {//информация о событии					
	DWORD cbSize;						//размер структуры в байтах
	AML_EVENTCODE EventCode;			//код события (см. значение AML_EVENTCODE)
	union EventInfo {					//информация о событии
		AML_EVENTDOC	DocEvent;		//информация о событии с документами
		AML_EVENTEDITOR EditorEvent;	//информация о событии Редактора текста
		AML_EVENTPAGE	PageEvent;		//информация о событиях страницы
		AML_EVENTLOG	LogEvent;		//лог-cообщение
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

//структура информации о плагине
struct AML_PLUGIN_INFO {
	DWORD  cbSize;						//[in] размер в структуры байтах, используете sizeof(AML_PLUGIN_INFO)
	UINT   nMenuCount;					//[out] число элементов меню (сколько раз вызывать Aml_GetMenuItemInfo)
	TCHAR  szMenuName[AML_MAX_NAME];	//[in\out] название подменю плагина (если nMenuCount>1)
	TCHAR  szDescription[AML_MAX_NAME];	//[in\out] описание подменю плагина в строке состояния
	UINT   nMinRelease;					//[out] минимальный номер сборки Aml Pages для работы с плагином, если 0 (по умолчанию):подходит любая Aml Pages
	DWORD  dwEventMask;					//[out] события извещения о которых должен получать плагин
	DWORD  dwFlags;						//[in\out] зарезервировано для использования в будущем
	WORD   wLanguage;					//[in] language of Aml Pages interface (required Aml Pages 9.00.1842), see Windows primary languages ID (may LANG_NEUTRAL if english)

#ifndef AML_PAGES_PLUGIN//операторы и конструкторы нужны только в Aml Pages самой
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
*	Флаги для добавления в контекстные меню
*	редактора и дерева (только в расширенные)
***************************************************/

enum {
	 APF_MENUTREE			=0x0001		//включать в меню дерева
	,APF_MENUEDIT			=0x0002		//включать в меню редактора
	,APF_MENUSHLIST			=0x0004		//включать в меню синтаксических подсветок (9.0.0.1743 и старше)
	,APF_MENUNOPLUGINSMENU	=0x0008		//НЕ включать в меню "Плагины" (9.0.0.1743 и старше)
	,APF_MENUABOUTINFO		=0x0010		//добавлять как ссылку "О плагине Имя_плагина" в диалог "О программе"
	,APF_MENUSYSTRAY		=0x0020		//добавлять в меню системной панели
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

//структура информации об элементе меню
struct AML_MENU_ITEM_INFO {
	DWORD cbSize;							//[in] размер структуры в байтах, используете sizeof(AML_MENU_ITEM_INFO)
	TCHAR szMenuName[AML_MAX_NAME];			//[out] название элемента меню
	TCHAR szSBHint[AML_MAX_NAME];			//[out] текст в строке состояния (необязательно)
	TCHAR szTTHint[AML_MAX_NAME];			//[out] текст во всплыв. подсказке (необязательно)
	char szFunctionName[AML_MAX_NAME];		//[out] имя экпортируемой функции в DLL плагина, вызываемой при выборе меню
	PLUGIN_TYPE nPluginType;					//[out] тип вызываемой функции DLL плагина (см. перечисление PLUGIN_TYPE)
	LPARAM lParam;							//[out] пользовательское значение, в AmlPages.exe не используется, по умолчанию 0
	HICON hIcon;							//[out] иконка в пункте меню
	COLORREF crTextColor;					//      цвет шрифта в меню
	DWORD reserved2;						//      оставлено ТОЛЬКО для совместимости с прошлыми версиями плагинов,/*BOOL bCanModified*/;
	UINT nMinRelease;						//[out] миним.номер сборки требуемой Aml Pages в котором будет работать пункт меню. Если 0 (по умолчанию) любая Aml Pages
	UINT nMenuState;						//[out] если указано MF_CHECKED меню отмечено
	DWORD reserved3;						//		зарезервировано,/*BOOL bReserved2*/
	DWORD dwFlags;							//[out] флаги включения элемента в дополнительные меню
	char szInitFunctionName[AML_MAX_NAME];	//[out] имя функции инициализации (вызов до и после функции указанной в szFunctionName, необязательно

#ifndef AML_PAGES_PLUGIN//конструкторы и операторы нужны только внутри проекта Aml Pages
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
		dwFlags=0;	//по умолчанию плагин не может себя добавить в контексные меню редатора и дерева
		szInitFunctionName[0]=TEXT('\0');
		nPluginType=(PLUGIN_TYPE)0;
	}
	
	AML_MENU_ITEM_INFO& operator=(const AML_MENU_ITEM_INFO& info) 
	{
#ifdef _DEBUG//compile-time проверка типов char
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
*	Структура содержащая текст, оставлена
*	ТОЛЬКО для совместимости с прошлыми версиями
*	программы
***************************************************/	
struct AML_TEXTREAD {
	DWORD cbSize;		//[in] размер структуры, используйте sizeof(AML_TEXTREAD)
	LPCTSTR lpszText;	//[in] текст страницы документа Aml Pages
	DWORD cbBufferSize;	//[in] размер в байтах текста в lpszText

#ifndef AML_PAGES_PLUGIN//конструктор нужен только в Aml Pages
	AML_TEXTREAD():cbSize(sizeof(AML_TEXTREAD)),lpszText(NULL),cbBufferSize(0) {}
#endif//#ifndef AML_PAGES_PLUGIN
};//struct AML_TEXTREAD

//флаги обхода дерева
enum {
	APFTP_STARTSELFOLDER=0x0001	//начинать обход дерева с выделенной папки, если 0 (по умолчанию), то с корневой папки документа Aml Pages
	,APFTP_APPLYSH=0x0002		//отдавать страницы с подсветкой подсвеченные, если PT_TEXT то сначала преобразовать в RTF
};//enum 


/***************************************************
*	Структура передаваемая в конце и в
*	начале обхода дерева
***************************************************/
struct AML_INIT_TREEPASS {
	DWORD cbSize;	//[in] размер структуры в байтах, используется sizeof(AML_INIT_TREEPASS)
	BOOL bStart;	//[in] TRUE перед началом обхода, и FALSE в после обхода
	LPARAM lParam;	//[in\out] пользовательское значение, указанное в Aml_GetMenuItemInfo
	BOOL bContinue;	//[in\out] игнорируется если bStart==FALSE
	DWORD dwFlags;	//[out] флаги, 0 обход с рута, APFTP_STARTSELFOLDER  обход с выделенной папки

#ifndef AML_PAGES_PLUGIN//конструктор нужен только в самой Aml Pages
	AML_INIT_TREEPASS():cbSize(sizeof(AML_INIT_TREEPASS)),bStart(TRUE)
		,lParam(NULL),bContinue(TRUE),dwFlags(0) {}
#endif//#ifndef AML_PAGES_PLUGIN
};//struct AML_INIT_TREEPASS

/***************************************************
*	Структура для обхода дерева
***************************************************/
struct AML_PAGEINFO {
	DWORD cbSize;				//[in] размер структуры, используйте sizeof(AML_PAGEINFO)
	SYSTEMTIME stDateCreated;	//[in] дата создания страницы
	SYSTEMTIME stDateModified;	//[in] дата модификации страницы
	LPCTSTR lpszFullPagePath;	//[in] полный путь страницы в документе Aml Pages (строка в стиле C, с завершающим '\0')
	HICON hIcon;				//[in] node icon
	LPCTSTR lpszHeader;			//[in] заголовок страницы\папки (строка в стиле C, с завершающим '\0')
	DWORD nParentPageID;			//     support Aml Pages 9.02 b1898 or later
	LPCTSTR lpszContent;		//[in] текст страницы (строка в стиле C, с завершающим '\0')
	DWORD lCursorPos;			//[in] положение курсора в тексте support Aml Pages 9.02 b1909 or later
	BOOL reserved4;				//[in] оставлено для совместимости с предыд. версиями плагинов (bFolder)(bFormattedText)
	BOOL* pbContinue;			//[in\out] продолжать ли обход? по умолчанию истина (TRUE), прекратить (FALSE)
	BOOL bRoot;					//[in] начальная папка обхода (TRUE), иначе (FALSE)
	PAGE_TYPE pageType;			//[in] тип страницы (см. перечисление выше)
	UINT nNumber;				//[in] номер обходимой страницы 0 для первой, дальше 1, 2, 3 и т.д.
	LPARAM lParam;				//[in\out] пользовательское значение
	int nPageID;				//[in] уникальный идентификатор страницы(папки) в документе
	BYTE* pbContent;			
	DWORD dwLenContent;
	DWORD dwCodePage;


#ifndef AML_PAGES_PLUGIN//конструктор нужен только в самой Aml Pages
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
	 APFS2_BOOKMARK_BK_ENABLED		=1	//включено изменять цвет под метками
	,APFS2_BOOKMARK_BK_COLOR		=2	//цвет под метками
	,APFS2_BK_ON_KILLFOCUS_ENABLED	=4	//ВКЛючено ли менять цвет при потере фокуса
	,APFS2_BK_ON_KILLFOCUS_DELTA	=8	//дельта при потере цвета
	,APFS2_FULL_PATHNAME_IN_TITLE	=16	//показывать ли путь в заголовке окна
	,APFS2_XP_STYLES_FOR_DOCS_TABS	=32	//использовать стили XP для панели закладок документов
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
	DWORD cbSize;		//[in] размер структуры в байтах, используйте sizeof(AML_EXECCOMMAND)
	LPARAM lParam;		//[in\out] lParam из меню, или функции иниициализации
	DWORD dwReserved;	//	зарезервировано для использования в будущем

#ifndef AML_PAGES_PLUGIN//конструктор используется только в проекте Aml Pages
	AML_EXECCOMMAND(LPARAM _lparam)
		:cbSize(sizeof(AML_EXECCOMMAND))
		,lParam(_lparam)
		,dwReserved(0)
	{
	}
#endif// #ifndef AML_PAGES_PLUGIN
};//struct AML_EXECCOMMAND

struct AML_INIT_EXECCOMMAND {
	DWORD cbSize;		//[in] размер структуры в байтах, используется sizeof(AML_INIT_EXECCOMMAND)
	BOOL bStart;		//[in] TRUE если вызов инициализации, FALSE если вызов терминации
	BOOL bContinue;		//[in\out] продолжать ли работу (игнорируется если bStart==FALSE)
	LPARAM lParam;		//[in\out] пользовательское значение из вызова Aml_GetMenuItemInfo
	DWORD dwReserved;	//зарезервировано для использования в будущем

#ifndef AML_PAGES_PLUGIN//конструктор используется только в проекте Aml Pages
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
Структура для изменения текста активной страницы,
***************************************************/
struct AML_PAGECONTENT {
	const DWORD cbSize;			//[in] размер структуры в байтах, используете sizeof(AML_PAGECONTENT)
	BSTR bstrContent;			//[in\out] текст страницы
	DWORD dwFlags;				//[in\out] флаги состояния текста
	const PAGE_TYPE pageType;	//[in] тип страницы текста
	BOOL bChanged;				//[out] установите в TRUE (не-ноль), если данные изменены плагином
	CHARRANGE crSel;			//[in\out] выделение в редакторе текста
	LPARAM lParam;				//[in\out] пользовательское значение
	const HWND hwnd;			//[in] HWND окна Редактора текста (required Aml Pages 9.00 b1818 or later)
	const int nPageID;
	const WORD wDocID;

#ifndef AML_PAGES_PLUGIN//конструктор нужен только в самой Aml Pages	
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
#ifdef AML_PAGES_PLUGIN//конструктор нужен только в самой Aml Pages	
	AML_DISLAYCOLORDLG()
	{
		memset(this,0,sizeof(AML_DISLAYCOLORDLG));
		cbSize=sizeof(AML_DISLAYCOLORDLG);
	}
#endif//AML_PAGES_PLUGIN//конструктор нужен только в самой Aml Pages	
};//struct AML_DISLAYCOLORDLG {

//флаги функции AP_PAGECONTENT (структура AML_PAGECONTENT.dwFlags)
enum {
	 APFPC_SEL=0x0001		//передавать в плагин только выделенный в редакторе текст, если не установлено (ноль) передавать весь текст страницы
	,APFPC_PLAINTEXT=0x0002	//передавать текст в формате Plain Text, если не установлено в формате RTF
};//enum 

struct AML_INIT_PAGECONTENT {
	const DWORD cbSize;			//[in] размер структуры в байтах,используете sizeof(AML_INIT_PAGECONTENT)
	BOOL bStart;				//[in] TRUE: инициализация (вызов ДО основной функции), FALSE: завершение (вызов ПОСЛЕ основной функции)
	const PAGE_TYPE pageType;	//[in] тип страницы, см перечисление в PageTypeDecl.h
	DWORD dwFlags;				//[in\out] флаги состояния текста, см флаги APFPC_xxxxx
	CHARRANGE crSel;			//[in] выделение в редакторе текста
	BOOL bContinue;				//[in\out] продожать ли работу (осуществлять вызов основной функции), если прервать установите в FALSE (по умолчанию TRUE)
	LPARAM lParam;				//[in\out] пользовательское значение
	const HWND hwnd;			//[in] HWND окна Редактора текста (required Aml Pages 9.00 b1818 or later)
	const int nPageID;
	const WORD wDocID;


#ifndef AML_PAGES_PLUGIN//конструктор нужен только в самой Aml Pages	
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
	*	(указатели на функции нужны ТОЛЬКО в самом
	*	проекте Aml Pages)
	/***************************************************/
	//функция обработки события панели
	typedef LRESULT (*PFUNC_PANEEVENTPROC)(int nIndex, AML_EVENTPANE*);
	//получение информации о панелях (-1 || p==NULL - get panels count, else get pane info by index)
	typedef UINT (*PFUNC_AML_GETPANELINFO)(int nIndex, AML_PANEINFO*);
	//поддерживает ли плагин Unicode (вызывает первой, из Aml Pages
	typedef BOOL (*PFUNC_AML_ISSUPPORTSPLUINUNICODE)(LPVOID);

	//получение информации о плагине, обязательна для реализации
	typedef BOOL (*PFUNC_AML_GETPLUGININFO)(AML_PLUGIN_INFO* pInfo);
	
	//получение информации о элементе меню ,обязательна для реализации
	typedef BOOL (*PFUNC_AML_GETMENUITEMINFO)(UINT nIndex, AML_MENU_ITEM_INFO* pInfo);

	//получение информации о панелях
	typedef UINT (*PFUNC_AML_GETPANEINFO)(UINT nIndex, AML_PANEINFO*);

	//создание панели
	//typedef HWND (*PFUNC_AMLPANE_CREATE)(UINT nIndex, HWND hwndParent, SIZE size, LPARAM lParam);
	typedef HWND (*PFUNC_AMLPANE_CREATE)(HWND hwndParent, AML_PANECREATE* p);

	typedef LRESULT (*PFUNC_AML_UNLOADPLUGIN)(LPVOID);

	typedef LRESULT (*PFUC_AMLPANE_NOTIFYRECEIVED)(HWND hwnd, WPARAM, LPARAM);

	//обход дерева, экспортируется из плагина,значение перечисления PLUGIN_TYPE: AP_TREEPASS
	typedef BOOL (*PFUNC_AML_TREEPASS)(AML_PAGEINFO* pTreePass);
	
	//функция инцицализации,значение перечисления PLUGIN_TYPE: AP_TREEPASS
	typedef BOOL (*PFUNC_AML_INIT_TREEPASS)(AML_INIT_TREEPASS*);
	
	//Показ диалога, значение перечисления PLUGIN_TYPE: AP_SHOWDIALOG
	typedef BOOL (*PFUNC_AML_SHOWDIALOG)(HWND);
	
	/*Чтение текста из страницы,
	*	ВНИМАНИЕ
	*	оставлено ТОЛЬКО для совместимости с предыдущиме версиями,
	*	используйте AP_PAGECONTENT	*/
	typedef BOOL (*PFUNC_AML_TEXTREAD)(AML_TEXTREAD*);
	
	//Изменение текста страницы, (значение перечисления PLUGIN_TYPE: AP_PAGECONTENT)
	typedef BOOL (*PFUNC_AML_PAGECONTENT)(AML_PAGECONTENT*);
	
	//Функция инициализации, (значение перечисления PLUGIN_TYPE:)
	typedef BOOL (*PFUNC_AML_INIT_PAGECONTENT)(AML_INIT_PAGECONTENT*);
	
	//Выполнение команды меню, значение перечисления PLUGIN_TYPE:AP_EXECCOMMAND
	typedef BOOL (*PFUNC_AML_EXECCOMMAND)(AML_EXECCOMMAND*);

	//Функция инициализации, значение перечисления PLUGIN_TYPE:AP_EXECCOMMAND
	typedef BOOL (*PFUNC_AML_INIT_EXECCOMMAND)(AML_INIT_EXECCOMMAND*);
	//Функция плагина, обработчик событий Aml Pages
	typedef BOOL (*PFUNC_AML_EVENTHANDLER)(AML_EVENTINFO*);
	//функция преобразования RTF2HTML
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
	*	размеры структур нужны только Aml Pages
	*
	*	РАЗМЕРЫ РАЗНЫХ ВЕРСИЙ СТРУКТУР (ТОЛЬКО ТЕ,
	*	КОТОРЫЕ ИСПОЛЬЗУЕТ И AMLPAGES.EXE)
	***************************************************/
	//(в плагине все инициализируется автотатически в конструкторе)
	#define SIZE_AML_OPTIONS_VER2			76//размер AML_OPTIONS Версии 2
	#define SIZE_AML_PLUGINABOUTINFO_VER2	32//размер структуры AML_PLUGINABOUTINFO версии 2
	#define SIZE_AML_PLUGINABOUTINFO_VER3	36//размер структуры AML_PLUGINABOUTINFO версии 3
	#define SIZE_AML_PAGECONTENT_VER2		38//+hwnd
	#define SIZE_AML_PAGECONTENT_VER3		44//+wDocID, +nPageID
	#define SIZE_AML_INIT_PAGECONTENT_VER3	44//+wDocID, +nPageID
	#define SIZE_AML_SELECTPAGEEX_VER2		40//+nExcludePageID
#endif //#ifndef AML_PAGES_PLUGIN 


	#define SIZE_AML_OPTIONS_VER3 204//размер AML_OPTIONS Версии 3(+dwBkTreeColor,dwTreeColor,lfTreeFont, build 1867)
	#define SIZE_AML_OPTIONS_VER4 208	 //размер AML_OPTIONS Версии 4(+hBkEditorBrush, build 2000)
	#define SIZE_AML_OPTIONS_VER5 728	 //размер AML_OPTIONS Версии 5(sz, build 2078)
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
		 APFDS_DOCHASCHANGES	=1	//есть в документе несохраненные изменения
		,APFDS_DOCNOHASCHANGES	=-1	//в документе нет несохраненных изменений
		,APFDS_DOCERROR			=0	//ошибка (нет такого открытого документа)
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
	,AC_SHOW_PASSWORD_DLG		=53	//Show password dialog,LPARAM — pointer to AML_PASSWORD_INFO, ret val - 1, is press OK, or 0 - is press FALSE, or -1 on errir
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
