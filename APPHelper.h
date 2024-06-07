// APPHelper.h: interface for the APPHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPHELPER_H__F41D99AE_3171_426F_B928_7854773D682A__INCLUDED_)
#define AFX_APPHELPER_H__F41D99AE_3171_426F_B928_7854773D682A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//заголовочный файл Aml Pages Plugin API
#include "Plugins.h"
#define ARRAYSIZE(a) (sizeof(a) / sizeof(*(a)))
/*
Вспомогательная структура для вызова сервисных функиий Aml Pages
все функции статические, т.е. для их вызова НЕ нужно создавать экземпляр структуры
следует использовать код APPHelper::FunctionName(...)
*/

enum AML_OPTIONS_COLORS {AOC_TREEBK=0,AOC_TREEFONT=1, AOC_EDITORBK=2, AOC_EDITORFONT=4};
enum AML_OPTIONS_FONTS  {AOF_TREEFONT=0, AOF_EDITORFONT=1};

struct APPHelper   
{
	
	static LRESULT Insert_New_Node_IntoActiveDoc(AML_NODEINFO& nodeInfo,LPCTSTR lpszConfirmDlgTitle);
	static LRESULT Insert_New_Node_IntoDoc(const WPARAM wDocID, AML_NODEINFO& nodeInfo,LPCTSTR lpszConfirmDlgTitle);
	static BOOL Set_AP_Settings_WriteLog(const BOOL bWriteLog);
	static BOOL RemoveAttachmentToRecycledBin(const WORD wDocID, const int nIndex);//Move attachment to Recycled
	static BOOL IsHasAPC_REMOVEATTACHMENTEX();
	static WORD GetUILanguage();//Get UI language (LANG_ENGLISH, LANG_RUSSIAN and etc)
	static UINT MessageBoxWith2Radio(const HWND hwndParent, UINT nType, LPCTSTR lpszTitle, LPCTSTR lpszText, LPCTSTR lpszRadio1, LPCTSTR lpszRadio2, UINT& nRadio);//MessageBox with radio buttons
	static int GetActivePage(const WORD wDocID=AML_ACTIVEDOCUMENTID);//get selected page of document
	static BOOL GetAttachment(const WORD wDocID, const int nIndex, AML_ATTACHINFO* p);//Get attachment data from document
	static int EnumAttachments(WORD wDocID, PFUNC_ENUMATTACHMENTSPROC pfn, LPARAM lParam, LPCTSTR lpszINFOID=NULL, int nPageID=(AML_NOCHECKPAGEID));//enumerate attachments
	static BOOL AddAttachment(WORD wDocID, AML_ATTACHINFO* p);//insert attachment
	static BOOL SetAttachment(WORD wDocID, int nIndex, AML_ATTACHINFO* p);//change attachment in document
	static BOOL RemoveAttachment(WORD wDocID, int nIndex);//Remove attachment from document (completelly)
	static BOOL AddStringToAttachment(WORD wDocID,LPCTSTR lpsz, LPCTSTR lpszInfoID, const int nPageID);//добавить C-строку как расширенную информацию к документу
	
	static LRESULT Command(UINT nCmd, WPARAM wParam, LPARAM lParam);//выполнение команды (Сохранить документ, открыть док, показать листочек и т.д.)
	
	static BOOL MessageBoxWithCheckBoxEx(AML_MBCHECKINFO* pInfo);//MessageBox с несколькими галочками выбора
	static BOOL SetStatusBarText(BSTR bstr);//текст в строку состояния Aml Pages
	static BOOL MessageBoxWithRadioBox(AML_MBRADIOINFO* pInfo);//расширенные MessageBox с возможностью множественнного выбора (radiobuttons)
	static BOOL GetModuleVersionInfoEx(HMODULE hModule, AML_MODULEVERSIONINFO* pInfo);//получить версию модуля
	static BOOL RichOut(AML_RICHEDITIO *p);//вывод текста из RichEdit
	static BOOL RichIn(AML_RICHEDITIO *p);//ввод текста в RichEdit
	static BOOL PluginAbout(AML_PLUGINABOUTINFO* pInfo);//Диалог о плагине
	static BOOL GetAutoTextCommand(AML_AUTOTEXTCOMMAND* pCmd);//Значение выражения автотекста
	static BOOL GetOptions(AML_OPTIONS* pOpt);//получить опции из Aml Pages
	static BOOL ReReadPluginsInfo(HMODULE hModule);//перестроить список плагинов в Aml Pages (hModule - модуль плагина из которого вызывается для него и будет перестроен список)
	static BOOL GetModuleVersionInfo(LPCTSTR lpszPathName/*путь к модулю*/, AML_MODULEVERSIONINFO* pVersionInfo/*указатель на структуру версии*/);
	static UINT GetAmlPagesBuildNumber();//получение номера сборки Aml Pages
	static HWND GetMainHWND();//получение HWND главного окна Aml Pages
	static BOOL ConvertText(LPCTSTR lpszInText/*входной текст*/,BSTR* pbstrOutText/*выходной текст - указатель готовит плагин*/, const BOOL bRTF2Text/*TRUE - если RTF в простой текст, FALSE - если простой текст в РТФ*/);//конвертирование текста RTF<->PlainText
	static BOOL MessageBoxWithCheckBox(LPCTSTR lpszText/*текст сообщения*/,UINT uType/*тип сообщения*/,LPCTSTR lpszCheckBoxText/*текст галочки*/,BOOL* pbChecked/*отметка галочкми, входной и выходной*/,int* pnRet/*результат работо MessageBox*/);//Показать MessageBox с галочкой
	static BOOL DisplayPasswordDialog(BSTR* pbstrPassword/*полученный пароль - указатель готовит плагин*/,BOOL bSetupPassword/*TRUE - установка пароля, повтор ввода*/,BOOL bTopMost/*TRUE - поверх всех окон*/,LPCTSTR lpszPrevPassword/*пред. пароль, может быть NULL*/,BOOL bShowOnEnter/*показывать пароль при вводе*/,LPCTSTR lpszTitle/*заголовок к окну пароля*/);//показать диалог пароля
	static BOOL PluginAbout(LPCTSTR lpszAbout, LPCTSTR lpszCopy, LPCTSTR lpszLink, LPCTSTR lpszNameVersion, LPCTSTR lpszURLToolTip, LPCTSTR lpszTitle,const HICON hIcon, LPCTSTR lpszLinkText);//показать диалог "О плагине"
	static BOOL RichInRTFTextSel(const HWND hwnd, LPCTSTR lpszText);//ввод выделенного текста RTF
	static BOOL RichInRTFTextAll(const HWND hwnd, LPCTSTR lpszText);//ввод всего текста RTF
	static BOOL RichInPlainTextSel(const HWND hwnd, LPCTSTR lpszText);//ввод простого выделенного текста
	static BOOL RichInPlainTextAll(const HWND hwnd, LPCTSTR lpszText);//ввода всего простого текста
	static BOOL SetStatusBarText(LPCTSTR lpszText);//показать текст в строке состояния
private:
	static HMODULE GetPlugin_HMODULE_DLL();
	static BOOL RichIn(const HWND hwnd, LPCTSTR lpszText, const bool bAsPlainText, const bool bSel);	
	static int MakePluginMenuTitle_Internal(LPTSTR lpszBuffer, const int nLen, LPCTSTR lpszTitle, const HMODULE hPluginDLL, const bool bWithBuild);
public:
	static BOOL DoCommandAc_Allow_DND_FromIE_NO_REPORT();
	static BOOL DoAc_Is_Allow_DND_FromIE();
	static BOOL DoCommandAc_Allow_DND_FromIE(const BOOL bWithReport, const HWND hwndReport);
	static BOOL Is_Has_Document_Windows();
	static BOOL DoMenu_Command_ShowNews();
	static BOOL WriteLog_2_AS_ERROR(LPCTSTR lpszPrefix, LPCTSTR lpszMessage);
	static BOOL WriteLog_2_AS_WARNING(LPCTSTR lpszPrefix, LPCTSTR lpszMessage);
	static BOOL WriteLog_2_AS_MESSAGE(LPCTSTR lpszPrefix, LPCTSTR lpszMessage);

	static BOOL WriteLog_2(LPCTSTR lpszPrefix, LPCTSTR lpszMessage, const DWORD dwFlags);
	static UINT APC_GetAmlPagesBuildNumber();
	static BOOL Is_AP_Settings_WriteLog();
	static BOOL DoMenu_Command_CheckForUpdates();
	static int QueryPassword_For_Setup(AML_PASSWORD_INFO& info);
	static UINT FormatVersionDLL(const HMODULE hDLL, LPTSTR lpszResult, const UINT nMaxResult, LPCTSTR lpszPrefix, const BOOL bAllowBuild);
	static BOOL BlinkCtrl(const HWND hwndCtrl);
	static BOOL WriteLog_with_PLUG_TITLE(LPCTSTR lpszMessage);
	static BOOL WriteLog(LPCTSTR lpszPrefix, LPCTSTR lpszMessage);
	
	static int MakePluginMenuTitle(LPTSTR lpszBuffer, const int nLen, LPCTSTR lpszTitle, const HMODULE hPluginDLL);
	static int MakePluginMenuTitle_WithBuild(LPTSTR lpszBuffer, const int nLen, LPCTSTR lpszTitle, const HMODULE hPluginDLL);
	static BOOL PluginPaneCommand(AML_PANECOMMAND& cmd);
	static BOOL CheckNewVersionOnInternet(AML_CHECKNEWVERSION& info);
	static BOOL CheckNewVersionOnInternet(AML_CHECKNEWVERSION& info,LPTSTR lpszURL, const UINT nLen);
	static HWND GetRightPaneHWND_AndType(DWORD& dwFlags);
	static UINT GetRightPaneTypeOnly();
	static HWND GetRightPaneHWND_Only();
	static BOOL HightLightAllInCurrentRichEdit();
	static BOOL IsUseRussianVersionOfAmlPages();
	static BOOL GetActiveNodeInfo_FromActiveDoc(AML_NODEINFO* pInfo);
	static BOOL GetNodeInfo_FromActiveDoc( AML_NODEINFO *pInfo);
	static BOOL GetNodeInfo(const WORD wDocID, AML_NODEINFO* pInfo);
	static UINT MakApdLinkURL_IsRequiredTCHARs(LPCTSTR lpszDocPathName, const int nPageID, LPCTSTR lpszBookmarkName);
	static UINT MakeApdLinkURL(LPCTSTR lpszDocPathName, const int nPageID, LPCTSTR lpszBookmarkName, LPTSTR lpszResURL, const UINT cchTextMax);
	static void FreeMBCheckInfo(AML_MBCHECKINFO& info);
	static int GetParentFolderID(const WPARAM wDocID, const int nPageID);
	static WORD OpenDocumentWithFlags(LPCTSTR lpszDocPath, const int nPageID, const BOOL bNoRestoreFromTray);
	static BOOL SelectPageEx(const WORD wDocID, AML_SELECTPAGEEX& sel_page);
	static BOOL IsAmlPagesRULanguage();
	static BOOL IsUnicodeVersionOfAmlPages(const AML_PLUGIN_INFO& info);
	static size_t GetAmlPagesOptionsIniPathNameRequiredTCHARs();
	static size_t GetAmlPagesOptionsIniPathName(const size_t nTCHARs, LPTSTR lpszPath);
	static LRESULT IsHasActiveDocumentChanges();
	static LRESULT IsHasDocumentChanges(const WORD wDocID);
	static BOOL GetDefaultSkin(AML_SKIN& skin);
	static BOOL SetSkin(const AML_SKIN& skin);
	static BOOL GetSkin(AML_SKIN& skin);
	static void GetPluginsDirectoryWithSlash(TCHAR *psz, const UINT n);
	static BOOL DisplayColorDialog(AML_DISLAYCOLORDLG& color_info);
	static BSTR ConcatenateText(BSTR lpsz1, const BOOL bRTF1, BSTR lpsz2, const BOOL bRTF2, const BOOL bResultAsRTF);
	static int GetPageType(const WORD wDocID, const int nPageID);
	static HBRUSH GetAmlBkEditorBrush();
	static BOOL SaveActiveDocumentAsDefaultCompressions();
	static BOOL InsertPictureFromFile(WPARAM wDocID, LPCTSTR lpszPicFilename);
	static BOOL ExecUserCommand(const WPARAM wParam,const LPARAM lParam);
	static WORD DocumentPathToID(LPCTSTR lpszDocumentPath);
	static BOOL GetModuleVersionInfoOfAmlPages(AML_MODULEVERSIONINFO* pVersionInfo);
	static BOOL SetOptions(const AML_OPTIONS* const pOpt);
	static BOOL DisplayActiveDocumentAsStickyNote(const int nPageID, const BOOL bDisplay);//display or hide page as sticky note in active document
	static BOOL DisplayAsStickyNote(const WORD wDocID, const int nPageID, const BOOL bDisplay);//display or hide page as sticky note
	static HICON ExtractPageIconFromActiveDocument(const int nPageID);////extract page icon from active document (user defined or standard)
	static HICON ExtractPageIcon(const WORD wDocumentID, const int nPageID);//extract page icon (user defined or standard)
	static BOOL DisplayFavoritesMenuForActiveDocument(POINT pt);//Display favorites menu on point
	static BOOL NewStickyNoteToTheDocument(const BOOL bInsertClipboardToContent, LPCTSTR lpszDocumentPath);
	static int ReadTipOfTheDay(const UINT nBufferInBytesSize, LPTSTR lpszBuffer);//read tip of the day from tips.txt
	static BOOL GetFilenameFromPath(LPCTSTR lpszPath, LPTSTR lpszFilename, const UINT nSize);//get filename.extension only from full path name
	static BOOL IsReadOnlyActiveDocument();//active document is read only?
	static BOOL IsReadOnlyDocument(const WORD wDocID);//document is read only?
	static void GetPluginsDirectory(TCHAR* psz, const UINT n);//get directory of plugins
	static UINT GetManagementFilesList(const UINT cbBufferBytes, TCHAR* const pBuffer);//get list of My Files
	static UINT GetManagementFilesListSizeInBytes();//get bytes neeeded for list of My Files
	static BOOL SelectPage(const WORD wDocID, const int nPageID, const DWORD dwCursorPos);//show dialog for node selection
	static int DisplayFolderDialogForActiveDoc(const int nSelFolderID, RECT* pRectDlg);
	
#ifdef _UNICODE
	static CHARFORMATW GetAmlEditorFontAsCHARFORMAT();//get editor font as CHARFORMAT structure
	static LOGFONTW GetAmlEditorFont();//get editor font as LOGFONT structure
	static LOGFONTW GetAmlTreeFont();//get tree font as LOGFONT structure
#else
	static CHARFORMATA GetAmlEditorFontAsCHARFORMAT();//get editor font as CHARFORMAT structure
	static LOGFONTA GetAmlEditorFont();//get editor font as LOGFONT structure
	static LOGFONTA GetAmlTreeFont();//get tree font as LOGFONT structure
#endif

	static int ImportFileToActiveDoc(LPCTSTR lpszPathName);//import file to active doc. RetVal: PageID or success, -1 or fail
	static BOOL ShowPageInTreeInActiveDoc(const int nPageID);//display node in active document tree
	static BOOL ShowPageInTree(const WORD wDocID, const int nPageID);//display node in document tree (with document ID)
	static BOOL MinimizeMainWindow();//minimize Aml Pages main window
	static BOOL RestoreMainWindow();//restore Aml Pages main window
	static WORD OpenDocumentEx(LPCTSTR lpszFullPathName, const int nPageID);//open document and select node (with ID==nPageID)
	static int GetActiveDocumentPath(UINT nBytes, LPTSTR lpszPath);//get path of the active document (or title, is not saved)
	static int GetActiveDocumentPathNeedBytes();//get bytes for active document path
	static COLORREF GetAmlColor(const AML_OPTIONS_COLORS colorType);//get colors of Aml Pages (options)
	static BOOL CopyOrMoveNode(const int nCopiedNodeID, const int nTargetNodeID, const BOOL bCopy);
	static BOOL MoveNode(const int nCopiedNodeID, const int nTargetNodeID);//copy node nCopiedNode to nTargetNodeID(Folder)
	static BOOL CopyNode(const int nCopiedNodeID, const int nTargetNodeID);//move node nCopiedNode to nTargetNodeID(Folder)
	static WORD OpenDocument(LPCTSTR lpszDocFullPathName);//opend document by full path name
	static UINT GetMruList(const UINT nBytes, LPTSTR lpszFileList);//get list of most recently used files
	static UINT GetMruListSizeInBytes();//get number of bytes needed for GetMruList
	static void NewStickyNote(const BOOL bInsertClipboardToContent);//create new sticky note is the active document
	static BOOL ShowTree(const WORD wDocID, const BOOL bShow);//show tree pane in document
	static int GetPageHeaderBytesNeed(const int nPageID);//get number of bytes required for GetPageHeader
	static int GetPageHeader(const int nPageID, LPTSTR lpszHeader);//get header of node 
	static int GetActivePageID(const WORD wDocumentID=AML_ACTIVEDOCUMENTID);//get selected node id
};

#endif // !defined(AFX_APPHELPER_H__F41D99AE_3171_426F_B928_7854773D682A__INCLUDED_)
