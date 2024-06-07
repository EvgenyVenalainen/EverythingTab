// APPHelper.cpp: implementation of the APPHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifndef _UNICODE
	#include <oleauto.h>//for BSTR support
#else
	#include <comdef.h>
#endif


#include "APPHelper.h"//поддержка сервисных функций Aml Pages

#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif//#ifdef _AFX

//для не MFC-проектов
//В MFC-проектах макрос ASSERT уже определен
#ifndef ASSERT
#ifdef _DEBUG
	#include <assert.h>
	#define ASSERT(X) assert(X)
#else// #ifdef _DEBUG
	#define ASSERT(X)
#endif//#ifdef _DEBUG
#endif//#ifndef ASSERT

//для не MFC-проектов
//В MFC-проектах макрос TRACE уже определен
#ifndef TRACE
	#ifdef _DEBUG
		#define TRACE(X) OutputDebugString(X)
	#else
		#define TRACE(X) 
	#endif
#endif

/*
Все функции этого модуля являются простыми обертками 
над сервисными функциями Aml Pages

Функции-обертки проверяют параметры, получают указатель на сервисную функцию
Aml Pages, и вызывают функцию по полученному указателю.
Если функция не поддерживается конкретной версий Aml Pages (т.е. GetProcAddress возвращает NULL)
функции-обертки этого модуля
возвращает FALSE (или иное значение идентифицирующее неудачный вызов)

Все сервисный функции Aml Pages имеют имя вида APC_ИмяФункции
и экспортируются из AmlPages.exe

Указатель на функцию получам следующим способом:
HMODULE hModule=GetModuleHandle(NULL);//получить HMODULE AmlPages.exe
GetProcAddress(hModule,имя_функции);//получить указатель на функцию [имя_функции]
*/

#define CHECK_STR_PTR(lpsz)	\
	ASSERT(lpsz);	\
	ASSERT(!::IsBadStringPtr(lpsz,INFINITE));

#define CHECK_PTR(ptr,TYPE)	\
	ASSERT(ptr);	\
	ASSERT(!::IsBadReadPtr((void*)ptr,sizeof(TYPE)));	\
	ASSERT(!::IsBadWritePtr((void*)ptr,sizeof(TYPE)));	
	

HWND APPHelper::GetMainHWND()
{
	/*получение HWND главного окна Aml Pages*/
	HMODULE h=NULL;
	__try {
		h=GetModuleHandle(NULL);
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		return NULL;
	}
	
	PFUNC_APC_GETMAINWND pfn=h ? (PFUNC_APC_GETMAINWND)GetProcAddress(h,"APC_GetMainWnd") : NULL;

	if (pfn)
		return pfn();
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_GetMainWnd function from AmlPages.exe\n"));
		return NULL;
	}
}

BOOL APPHelper::ConvertText(LPCTSTR lpszInText//исходная строка
							,BSTR* pbstrOutText//выходная строка (память распределяет и освобождает плагин)
							, const BOOL bRTF2Text//TRUE - если RTF в Plain Text, FALSE если Plain Text в RTF
							)
{
	CHECK_STR_PTR(lpszInText);
	CHECK_PTR(pbstrOutText,BSTR);

	/*конвертация строки RTF<->PlainText*/
	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_CONVERTTEXT pfn=(PFUNC_APC_CONVERTTEXT)GetProcAddress(h,"APC_ConvertText");
	if (pfn)
		return pfn(lpszInText,pbstrOutText,bRTF2Text);
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_ConvertText function from AmlPages.exe\n"));
		return FALSE;
	}
}

//показать MessageBox с галочкой
BOOL APPHelper::MessageBoxWithCheckBox(
	LPCTSTR lpszText//текст MessageBox
	,UINT uType//тип MessageBox
	,LPCTSTR lpszCheckBoxText//Текст галочки
	,BOOL* pbChecked//отмечены (in-out, НЕ может быть NULL, указатель готовит и если надо
	//освобождает плагин)
	,int* pnRet//значение вернутое из MessageBox, 
	//НЕ может быть NULL, память готовит и освобождает плагин
	)
{
	CHECK_STR_PTR(lpszText);
	CHECK_STR_PTR(lpszCheckBoxText);
	//ф-ция возвращает TRUE, если вызов удачен и FALSE если нет
	//возвращаеммые значения MessageBox и значение галочки см. в pbChecked и pnRet
	CHECK_PTR(pbChecked,BOOL);
	CHECK_PTR(pnRet,int);

	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_MESSAGEBOXWITHCHECKBOX pfn=
		(PFUNC_APC_MESSAGEBOXWITHCHECKBOX)GetProcAddress(h,"APC_MessageBoxWithCheckBox");
	if (pfn) {
		(*pnRet)=pfn(lpszText,uType,lpszCheckBoxText,pbChecked);
		return TRUE;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_MessageBoxWithCheckBox function from AmlPages.exe\n"));
		return FALSE;
	}
}

//показать окно настройки-ввода пароля
BOOL APPHelper::DisplayPasswordDialog(
     BSTR* pbstrPassword//out, строка с паролем, память готовит плагин, не может быть NULL
	,BOOL bSetupPassword//TRUE, если установка пароля, FALSE если ввод для проверки (если установка, просят повторить пароль для контроля)
	,BOOL bTopMost//даилог ввода поверх всех окон (TRUE)
	,LPCTSTR lpszPrevPassword//предыдущий пароль, если NULL предыд. пароля нет
	,BOOL bShowOnEnter//показывать пароль при вводе (TRUE), если FALSE используются астериски (звездочки)
	,LPCTSTR lpszTitle)//заголовок окна ввода
{
	CHECK_PTR(pbstrPassword,BSTR);
	//возвращает FALSE если ИЛИ вызов неудачен, или пользователь отменил ввод
	//возвращает TRUE если вызов удачен И пользователь подтвердил ввод (нажал OK)
	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_DISPLAYPASSWORDDIALOG pfn
		=(PFUNC_APC_DISPLAYPASSWORDDIALOG)GetProcAddress(h,"APC_DisplayPasswordDialog");
	if (pfn) {
		BOOL b=pfn(pbstrPassword,bSetupPassword,bTopMost,lpszPrevPassword,bShowOnEnter,lpszTitle);
		return b;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_DisplayPasswordDialog function from AmlPages.exe\n"));
		return FALSE;
	}
}

//получает номер сборки Aml Pages (последняя цифра версии, всегда увеличивается последователььно в новых версиях)
UINT APPHelper::GetAmlPagesBuildNumber()
{
	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_GETAMLPAGESBUILDNUMBER pfn=
		(PFUNC_APC_GETAMLPAGESBUILDNUMBER)GetProcAddress(h,"APC_GetAmlPagesBuildNumber");
	if (pfn) {
		const UINT nRet=pfn();
		return nRet;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_GetAmlPagesBuildNumber function from AmlPages.exe\n"));
		return 0;
	}
}

//получает информацияю о версии бинарного модуля по пути lpszPathName
BOOL APPHelper::GetModuleVersionInfo(
	LPCTSTR lpszPathName
	, AML_MODULEVERSIONINFO *pVersionInfo)
{
	CHECK_STR_PTR(lpszPathName);
	CHECK_PTR(pVersionInfo,AML_MODULEVERSIONINFO);
	//возвращает TRUE если вызов удачен
	ASSERT(pVersionInfo);
	if (!pVersionInfo) return FALSE;

	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_GETMODULEVERSIONINFO pfn=
		(PFUNC_APC_GETMODULEVERSIONINFO)GetProcAddress(h,"APC_GetModuleVersionInfo");
	if (pfn) {
		const BOOL bRet=pfn(lpszPathName,pVersionInfo);
		return bRet;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_GetModuleVersionInfo function from AmlPages.exe\n"));
		return FALSE;
	}
}

//перечитать информацию о плагине
BOOL APPHelper::ReReadPluginsInfo(
								  HMODULE hModule//если NULL перечитать все, 
								  //если не-NULL перечитать только для модуля с указанным hModule
								  )
{
	//TRUE-если вызов удачен
	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_REREADPLUGINSINFO pfn=
		(PFUNC_APC_REREADPLUGINSINFO)GetProcAddress(h,"APC_ReReadPluginsInfo");
	if (pfn) {
		const BOOL bRet=pfn(hModule);
		return bRet;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_ReReadPluginsInfo function from AmlPages.exe\n"));
		return FALSE;
	}
}

//запросить текущие опции у Aml Pages, 
//содержание структуры AML_OPTIONS зависит от флагов
//см. файл plugins.h, 
BOOL APPHelper::GetOptions(AML_OPTIONS *pOpt)
{
	//TRUE если вызов удачен
	//память готовит плагин, pOpt - не может быть NULL
	CHECK_PTR(pOpt,AML_OPTIONS);
	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_GETOPTIONS pfn=(PFUNC_APC_GETOPTIONS)GetProcAddress(h,"APC_GetOptions");
	if (pfn) {
		const BOOL bRet=pfn(pOpt);
		return bRet;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_GetOptions function from AmlPages.exe\n"));
		return FALSE;
	}
}

//получить значение выражения автотекста
//подробности см. в plugins.h
BOOL APPHelper::GetAutoTextCommand(AML_AUTOTEXTCOMMAND *pCmd)
{
	//память для pCmd распределяет и освобождает плагин
	//TRUE - если вызов удачен
	CHECK_PTR(pCmd,AML_AUTOTEXTCOMMAND );
	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_GETAUTOTEXTCOMMAND pfn=
		(PFUNC_APC_GETAUTOTEXTCOMMAND)GetProcAddress(h,"APC_GetAutoTextCommand");
	if (pfn) {
		const BOOL bRet=pfn(pCmd);
		return bRet;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_GetAutoTextCommand function from AmlPages.exe\n"));
		return FALSE;
	}
}

//показать информацию о плагине
//подробности см. в plugins.h
BOOL APPHelper::PluginAbout(AML_PLUGINABOUTINFO *pInfo)
{
	CHECK_PTR(pInfo,AML_PLUGINABOUTINFO);
	//память для pInfo распределяет и освобождает плагин
	//TRUE - если вызов удачен
	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_PLUGINABOUT pfn=
		(PFUNC_APC_PLUGINABOUT)GetProcAddress(h,"APC_PluginAbout");
	if (pfn) {
		const BOOL bRet=pfn(pInfo);
		return bRet;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_PluginAbout function from AmlPages.exe\n"));
		return FALSE;
	}
}

//ввод-вывод строк из окон RichEdit
static
BOOL RichInOut(AML_RICHEDITIO *p)
{
	CHECK_PTR(p,AML_RICHEDITIO);
	//память распределяет и освобождает плагин
	//TRUE - если вызов удачен
	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_RICHINOUT pfn=
		(PFUNC_APC_RICHINOUT)GetProcAddress(h,"APC_RichInOut");
	if (pfn) {
		const BOOL bRet=pfn(p);
		return bRet;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_RichInOut function from AmlPages.exe\n"));
		return FALSE;
	}
}

//ввод строки в окна RichEdit
BOOL APPHelper::RichIn(AML_RICHEDITIO *p)
{
	//память распределяет и освобождает плагин
	//TRUE - если вызов удачен
	CHECK_PTR(p,AML_RICHEDITIO);
	if (!p)
		return FALSE;
	
	p->dwFlags|=APFRIO_IN;
	return RichInOut(p);
}

//вывод строки из окон RichEdit
BOOL APPHelper::RichOut(AML_RICHEDITIO *p)
{
	//память распределяет и освобождает плагин
	//TRUE - если вызов удачен
	CHECK_PTR(p,AML_RICHEDITIO);
	if (!p) return FALSE;
	
	ASSERT(0==(p->dwFlags & APFRIO_IN));
	if (p->dwFlags & APFRIO_IN)
		p->dwFlags^=APFRIO_IN;

	return RichInOut(p);
}

//получение о версии бинарного модуля специфицированного hModule
BOOL APPHelper::GetModuleVersionInfoEx(HMODULE hModule
									   , AML_MODULEVERSIONINFO *pInfo)
{
	//память готовит и освобождает плагин
	//TRUE - если вызов успешен
	CHECK_PTR(pInfo,AML_MODULEVERSIONINFO );
	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_GETMODULEVERSIONINFOEX pfn=
		(PFUNC_APC_GETMODULEVERSIONINFOEX)GetProcAddress(h,"APC_GetModuleVersionInfoEx");
	if (pfn) {
		const BOOL bRet=pfn(hModule,pInfo);
		return bRet;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_GetModuleVersionInfoEx function from AmlPages.exe\n"));
		return FALSE;
	}
}

//MessageBox с radio-buttons
BOOL APPHelper::MessageBoxWithRadioBox(AML_MBRADIOINFO *pInfo)
{
	//указатель на pInfo готовит плагин, память если необходимо освобождает он же	
	CHECK_PTR(pInfo,AML_MBRADIOINFO);;//указатель НЕ может быть NULL
	ASSERT(pInfo->bstrText);//строка текста MessageBox НЕ может быть NULL
	ASSERT(pInfo->bstrButtonsText);//строка с описанием текста радиокнопок НЕ может быть NULL
	//строка с описанием текста должна быть вида "Radio Button Text 0\nRadio Button Text 1\n"
	//количество кнопок определяется как число символов "\n"+1, если "\n" в строке нет, кнопка одна

	PFUNC_APC_MESSAGEBOXWITHRADIO pfn=(PFUNC_APC_MESSAGEBOXWITHRADIO)GetProcAddress(GetModuleHandle(NULL)
		,"APC_MessageBoxWithRadioBox");
	if (pfn) {
		const BOOL b=pfn(pInfo);
		return b;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_MessageBoxWithRadioBox function from AmlPages.exe\n"));
		return FALSE;
	}
}

/*Показать MessageBox с несколькими галками*/
BOOL APPHelper::MessageBoxWithCheckBoxEx(AML_MBCHECKINFO *pInfo)
{
	CHECK_PTR(pInfo,AML_MBCHECKINFO);/*адрес структуры ?*/
	ASSERT(pInfo->bstrText);/*текст станд. MessageBox должен быть указан*/
	ASSERT(pInfo->bstrButtonsText);/*текст галочек должен быть указан*/
	ASSERT(pInfo->cbSize);/*не грех передать и sizeof(AML_MBCHECKINFO)*/
	ASSERT(pInfo->pbChecked);/*адреса значений должны быть переданы*/
	
	PFUNC_APC_MESSAGEBOXWITHCHECKBOXEX pfn=(PFUNC_APC_MESSAGEBOXWITHCHECKBOXEX)GetProcAddress(
		GetModuleHandle(NULL),"APC_MessageBoxWithCheckBoxEx");
	if (pfn)
		return pfn(pInfo);
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_MessageBoxWithCheckBoxEx function from AmlPages.exe\n"));
		return FALSE;
	}
}

static
BSTR BSTRFromLPCSTR(LPCSTR lpsz)
{
	if ((IsBadStringPtrA(lpsz,INFINITE)) || (!strlen(lpsz)))
		return NULL;

	const int nLen = MultiByteToWideChar(CP_ACP, 0, lpsz,
		strlen(lpsz) ,NULL, NULL);
	
	ASSERT(nLen);
	
	BSTR bstr = ::SysAllocStringLen(NULL, nLen);
	
	if (bstr) {
		MultiByteToWideChar(CP_ACP, 0, lpsz, strlen(lpsz),bstr, nLen);
		ASSERT(!IsBadStringPtrW(bstr,INFINITE));
	}
	
	return bstr;
}


BOOL APPHelper::PluginAbout(
	LPCTSTR lpszAbout
	, LPCTSTR lpszCopy
	, LPCTSTR lpszLink
	, LPCTSTR lpszNameVersion
	, LPCTSTR lpszURLToolTip
	, LPCTSTR lpszTitle
	, const HICON hIcon
	, LPCTSTR lpszLinkText)
{
	AML_PLUGINABOUTINFO pa;
#ifndef _UNICODE
	pa.bstrAbout=BSTRFromLPCSTR(lpszAbout);
	pa.bstrCopyrights=BSTRFromLPCSTR(lpszCopy);
	pa.bstrLink=BSTRFromLPCSTR(lpszLink);
	pa.bstrNameVersion=BSTRFromLPCSTR(lpszNameVersion);
	pa.bstrTitle=BSTRFromLPCSTR(lpszTitle);
	pa.bstrURLToolTip=BSTRFromLPCSTR(lpszURLToolTip);
	pa.lpszLinkText=lpszLinkText;
#else
	pa.bstrAbout=_bstr_t(lpszAbout);
	pa.bstrCopyrights=_bstr_t(lpszCopy);
	pa.bstrLink=_bstr_t(lpszLink);
	pa.bstrNameVersion=_bstr_t(lpszNameVersion);
	pa.bstrTitle=_bstr_t(lpszTitle);
	pa.bstrURLToolTip=_bstr_t(lpszURLToolTip);
	pa.lpszLinkText=_bstr_t(lpszLinkText);
#endif

	pa.hIcon=hIcon;

	const BOOL b=PluginAbout(&pa);
	if (pa.bstrAbout)
		SysFreeString(pa.bstrAbout);
	if (pa.bstrCopyrights)
		SysFreeString(pa.bstrCopyrights);
	if (pa.bstrLink)
		SysFreeString(pa.bstrLink);
	if (pa.bstrNameVersion)
		SysFreeString(pa.bstrNameVersion);
	if (pa.bstrTitle)
		SysFreeString(pa.bstrTitle);
	if (pa.bstrURLToolTip)
		SysFreeString(pa.bstrURLToolTip);

	return b;
}

//положить текст в строку состояния Aml Pages
BOOL APPHelper::SetStatusBarText(BSTR bstr)
{
	//bstr строка в формате BSTR, готовит и освобождает память плагин, не может быть NULL

	ASSERT(bstr);
	
	PFUNC_APC_SETSTATUSBARTEXT pfn=(PFUNC_APC_SETSTATUSBARTEXT)GetProcAddress(GetModuleHandle(NULL)
		,"APC_SetStatusBarText");

	if (pfn)
		return pfn(bstr);
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_SetStatusBarText function from AmlPages.exe\n"));
		return FALSE;
	}
}

BOOL APPHelper::SetStatusBarText(LPCTSTR lpszText)
{
	ASSERT(!IsBadStringPtr(lpszText,INFINITE));
	ASSERT(_tcslen(lpszText));

#ifndef _UNICODE
	const BSTR bstr=BSTRFromLPCSTR(lpszText);
#else
	const BSTR bstr=_bstr_t(lpszText);
#endif
	ASSERT(bstr);

	const BOOL b=APPHelper::SetStatusBarText(bstr);
	
	if(bstr)
		SysFreeString(bstr);

	return b;
}

BOOL APPHelper::RichInPlainTextAll(const HWND hwnd, LPCTSTR lpszText)
{
	return RichIn(hwnd,lpszText,true,false);
}

BOOL APPHelper::RichInPlainTextSel(const HWND hwnd, LPCTSTR lpszText)
{
	return RichIn(hwnd,lpszText,true,true);
}

BOOL APPHelper::RichInRTFTextAll(const HWND hwnd, LPCTSTR lpszText)
{
	return RichIn(hwnd,lpszText,false,false);
}

BOOL APPHelper::RichInRTFTextSel(const HWND hwnd, LPCTSTR lpszText)
{
	return RichIn(hwnd,lpszText,false,true);
}

BOOL APPHelper::RichIn(const HWND hwnd
					   , LPCTSTR lpszText
					   , const bool bAsPlainText
					   , const bool bSel)
{
	ASSERT(IsWindow(hwnd));
	ASSERT(!IsBadStringPtr(lpszText,INFINITE));
	ASSERT(_tcslen(lpszText));

	AML_RICHEDITIO io;
#ifndef _UNICODE
	io.bstrText=BSTRFromLPCSTR(lpszText);
#else
	io.bstrText=_bstr_t(lpszText);
#endif
	io.hwndRichEdit=hwnd;
	if (bAsPlainText)
		io.dwFlags|=APFRIO_PLAINTEXT;
	if(bSel)
		io.dwFlags|=APFRIO_SEL;

	const BOOL b=RichIn(&io);
	if (io.bstrText)
		SysFreeString(io.bstrText);

	return b;
}

int APPHelper::EnumAttachments(WORD wDocID
 							 , PFUNC_ENUMATTACHMENTSPROC pfn
 							 , LPARAM lParam
 							 , LPCTSTR lpszINFOID
 							 , int nPageID)
{
	ASSERT(!IsBadCodePtr((FARPROC)pfn));
 #ifdef _DEBUG
 	if (lpszINFOID) {
 		ASSERT(!IsBadStringPtr(lpszINFOID,INFINITE));
 		ASSERT(_tcslen(lpszINFOID));
 	}
 #endif
 
 	const HMODULE h=GetModuleHandle(NULL);
 	PFUNC_APC_ENUMATTACHMENTS p=
 		(PFUNC_APC_ENUMATTACHMENTS)GetProcAddress(h,"APC_EnumAttachments");

	if (NULL==p)
		p=(PFUNC_APC_ENUMATTACHMENTS)GetProcAddress(h,"APC_EnumExtraInfo");

 	if (p) {
 		const int nRet=p(wDocID,pfn,lpszINFOID,nPageID,lParam);
 		return nRet;
 	}
 	else
 		return -1;
}


BOOL APPHelper::RemoveAttachment(WORD wDocID, int nIndex)
{
	ASSERT(nIndex>=0);
 	const HMODULE h=GetModuleHandle(NULL);
 	
	PFUNC_APC_REMOVEATTACHMENT p=
		(PFUNC_APC_REMOVEATTACHMENT)GetProcAddress(h,"APC_RemoveAttachment");

	if (NULL==p)
		p=(PFUNC_APC_REMOVEATTACHMENT)GetProcAddress(h,"APC_RemoveExtraInfo");

 	if (p) {
 		const int nRet=p(wDocID,nIndex);
 		return nRet;
 	}
 	else
 		return -1;
}

BOOL APPHelper::SetAttachment(WORD wDocID, int nIndex, AML_ATTACHINFO *p)
{
	ASSERT(nIndex>=0);
	ASSERT(!IsBadReadPtr(p,sizeof(AML_ATTACHINFO)));

	const HMODULE h=GetModuleHandle(NULL);

	PFUNC_APC_SETATTACHMENT pFunc=
		(PFUNC_APC_SETATTACHMENT)GetProcAddress(h,"APC_SetAttachment");

	if (NULL==pFunc)
		pFunc=(PFUNC_APC_SETATTACHMENT)GetProcAddress(h,"APC_SetExtraInfo");
 	
	if (pFunc) {
 		const int nRet=pFunc(wDocID,nIndex,p);
 		return nRet;
 	}
 	else
		return FALSE;
}

struct GET_ATTACH_PARAM {
	AML_ATTACHINFO& ae;
	const int nIndex;
	BOOL bRet;
	GET_ATTACH_PARAM (AML_ATTACHINFO& e, int index):ae(e),nIndex(index),bRet(FALSE)
	{
	}	
};

static
BOOL GetAttachmentProc(int nIndex, AML_ATTACHINFO* pInfo, LPARAM lParam)
{
	GET_ATTACH_PARAM* const pParam=reinterpret_cast<GET_ATTACH_PARAM*>(lParam);
	ASSERT(!IsBadWritePtr((void*)pParam,sizeof(GET_ATTACH_PARAM)));
	if (pParam->nIndex==nIndex) {
		if (pInfo->dwInfoSize) {
			//pParam->ae.pInfo=new unsigned char[pInfo->dwInfoSize];
			pParam->ae.Alloc(pInfo->dwInfoSize);
			ASSERT(!IsBadWritePtr(pParam->ae.pInfo,pInfo->dwInfoSize));
			memcpy(pParam->ae.pInfo,pInfo->pInfo,pInfo->dwInfoSize);
			pParam->ae.dwInfoSize=pInfo->dwInfoSize;
		}
		pParam->ae.nPageID=pInfo->nPageID;
		pParam->ae.stDateCreated=pInfo->stDateCreated;
		pParam->ae.stDateModified=pInfo->stDateModified;
		pParam->bRet=TRUE;
		
		return FALSE;//end enum
	}

	return TRUE;	//continue enum
}

BOOL APPHelper::GetAttachment(const WORD wDocID, const int nIndex, AML_ATTACHINFO *p)
{
	ASSERT(nIndex>=0);
	ASSERT(!IsBadWritePtr(p,sizeof(AML_ATTACHINFO)));
	if(IsBadWritePtr(p,sizeof(AML_ATTACHINFO)))
		return FALSE;

	ASSERT(NULL==p->pInfo);
	ASSERT(0==p->dwInfoSize);

	GET_ATTACH_PARAM param(*p,nIndex);
	APPHelper::EnumAttachments(wDocID,GetAttachmentProc,(LPARAM)&param,p->lpszInfoID,p->nPageID);

	return param.bRet;	
}

int APPHelper::GetActivePage(const WORD wDocID)
{
	const HMODULE h=GetModuleHandle(NULL);
 	
	PFUNC_APC_GETACTIVEPAGE p=
		(PFUNC_APC_GETACTIVEPAGE )GetProcAddress(h,"APC_GetActivePage");
 	
	if (p) {
 		const int nRet=p(wDocID);
 		return nRet;
 	}
 	else
 		return -1;
}

WORD APPHelper::GetUILanguage()
{
	const HMODULE h=GetModuleHandle(NULL);
 	
	PFUNC_APC_GETUILANGUAGE p=
		(PFUNC_APC_GETUILANGUAGE)GetProcAddress(h,"APC_GetUILanguage");
 	
	if (p) {
 		const int nRet=p();
 		return nRet;
 	}
 	else
 		return 0;
}

UINT APPHelper::MessageBoxWith2Radio(const HWND hwndParent
									 , UINT nType
									 , LPCTSTR lpszTitle
									 , LPCTSTR lpszText
									 , LPCTSTR lpszRadio1
									 , LPCTSTR lpszRadio2
									 , UINT& nRadio)
{
	const int nLen=_tcslen(lpszRadio1)+2+_tcslen(lpszRadio2)+2;
	TCHAR* const p=new TCHAR[nLen];
	memset(p,0,nLen);
	_tcscat(p,lpszRadio1);
	_tcscat(p,TEXT("\n"));
	_tcscat(p,lpszRadio2);

	AML_MBRADIOINFO mb;
	mb.cbSize=sizeof(mb);
#ifndef _UNICODE
	mb.bstrButtonsText=BSTRFromLPCSTR(p);
	mb.bstrCaption=BSTRFromLPCSTR(lpszTitle);
	mb.bstrText=BSTRFromLPCSTR(lpszText);
#else
	mb.bstrButtonsText=_bstr_t(p);
	mb.bstrCaption=_bstr_t(lpszTitle);
	mb.bstrText=_bstr_t(lpszText);
#endif

	mb.hwndParent=hwndParent;
	mb.nChecked=nRadio;
	mb.uType=nType;

	const BOOL b=APPHelper::MessageBoxWithRadioBox(&mb);
	UINT nRet=0;
	if (b) {
		nRet=mb.nRetValue;
		nRadio=mb.nChecked;
	}
	SysFreeString(mb.bstrButtonsText);
	SysFreeString(mb.bstrCaption);
	SysFreeString(mb.bstrText);
	
	delete[] p;

	return nRet;
}

BOOL APPHelper::IsHasAPC_REMOVEATTACHMENTEX()
{
	const HMODULE h=GetModuleHandle(NULL);
 	
	void* const p=
		GetProcAddress(h,"APC_RemoveAttachmentEx");
	if (NULL==p)
		GetProcAddress(h,"APC_RemoveExtraInfoEx");

	return (NULL!=p);
}

BOOL APPHelper::RemoveAttachmentToRecycledBin(const WORD wDocID, const int nIndex)
{
	const HMODULE h=GetModuleHandle(NULL);
 	
	PFUNC_APC_REMOVEATTACHMENTEX pfunc=
		(PFUNC_APC_REMOVEATTACHMENTEX)GetProcAddress(h,"APC_RemoveAttachmentEx");
	
	if (NULL==pfunc)
		pfunc=(PFUNC_APC_REMOVEATTACHMENTEX)GetProcAddress(h,"APC_RemoveExtraInfoEx");

	if (NULL==pfunc)
		return FALSE;
	else
		return pfunc(wDocID,nIndex,AML_REMOVEATTACHMENTTORECYCLED);
}

BOOL APPHelper::AddAttachment(WORD wDocID, AML_ATTACHINFO* p)
{
	ASSERT(!IsBadReadPtr(p,sizeof(AML_ATTACHINFO)));

	const HMODULE h=GetModuleHandle(NULL);
	PFUNC_APC_ADDATTACHMENT pfn=
		(PFUNC_APC_ADDATTACHMENT)GetProcAddress(h,"APC_AddAttachment");

	if (NULL==pfn)
		pfn=(PFUNC_APC_ADDATTACHMENT)GetProcAddress(h,"APC_AddExtraInfo");

	if (pfn) {
		const BOOL b=pfn(wDocID,p);
		return b;
	}
	else
		return FALSE;

}

BOOL APPHelper::AddStringToAttachment(WORD wDocID,LPCTSTR lpsz, LPCTSTR lpszInfoID, const int nPageID)
{
	ASSERT(!IsBadStringPtr(lpsz,INFINITE));
	ASSERT(_tcslen(lpsz));
#ifdef _DEBUG
	if (lpszInfoID) {
		ASSERT(!IsBadStringPtr(lpszInfoID,INFINITE));
		ASSERT(_tcslen(lpszInfoID));
	}
#endif

	AML_ATTACHINFO ae;
	ae.dwInfoSize=_tcslen(lpsz);
	ae.pInfo=(unsigned char*)lpsz;
	ae.lpszInfoID=lpszInfoID;
	ae.nPageID=nPageID;

	return AddAttachment(wDocID,&ae);
}

int APPHelper::GetActivePageID(const WORD wDocumentID)
{
	return Command(AC_GETACTIVEPAGEID,wDocumentID,0);
}

int APPHelper::GetPageHeader(const int nPageID, LPTSTR lpszHeader)
{
	return Command(AC_GETPAGEHEADER,nPageID,(LPARAM)lpszHeader);
}

int APPHelper::GetPageHeaderBytesNeed(const int nPageID)
{
	return GetPageHeader(nPageID,NULL);
}

BOOL APPHelper::ShowTree(const WORD wDocID, const BOOL bShow)
{
	return Command(AC_SHOWTREE,wDocID,bShow);
}

UINT APPHelper::GetMruListSizeInBytes()
{
	return Command(AC_GETMRUFILESLIST,0,NULL);
}

UINT APPHelper::GetMruList(const UINT nBytes, LPTSTR lpszFileList)
{
	ASSERT(!IsBadWritePtr(lpszFileList,nBytes));
	return Command(AC_GETMRUFILESLIST,nBytes,(LPARAM)lpszFileList);
}

WORD APPHelper::OpenDocument(LPCTSTR lpszDocFullPathName)
{
	ASSERT(!IsBadStringPtr(lpszDocFullPathName,INFINITE));
	ASSERT(_tcslen(lpszDocFullPathName));
	return (WORD)Command(AC_OPENDOCUMENT,NULL,(LPARAM)lpszDocFullPathName);
}

BOOL APPHelper::CopyNode(const int nCopiedNodeID, const int nTargetNodeID)
{
	return APPHelper::CopyOrMoveNode(nCopiedNodeID, nTargetNodeID,TRUE);
}

BOOL APPHelper::MoveNode(const int nCopiedNodeID, const int nTargetNodeID)
{
	return APPHelper::CopyOrMoveNode(nCopiedNodeID, nTargetNodeID,FALSE);
}

BOOL APPHelper::CopyOrMoveNode(const int nCopiedNodeID
					, const int nTargetNodeID, const BOOL bCopy)
{
	return APPHelper::Command(bCopy ? AC_COPYNODE : AC_MOVENODE,nCopiedNodeID,nTargetNodeID);
}

COLORREF APPHelper::GetAmlColor(const AML_OPTIONS_COLORS colorType)
{
	AML_OPTIONS opt;
	switch(colorType) {
	case AOC_TREEBK:
		opt.dwMask=APFOPT_BKTREECOLOR;
		break;
	case AOC_TREEFONT:
		opt.dwMask=APFOPT_TREECOLOR;
		break;
	case AOC_EDITORFONT:
		opt.dwMask=APFOPT_EDITORCOLOR;
		break;
	case AOC_EDITORBK:
		opt.dwMask=APFOPT_BKEDITORCOLOR;		
		break;
	}//switch(colorType) {

	const BOOL b=APPHelper::GetOptions(&opt);
	if (!b)
		return 0;
	
	switch(colorType) {
	case AOC_TREEBK:
		return opt.dwBkTreeColor;
		break;
	case AOC_TREEFONT:
		return opt.dwTreeColor;
		break;
	case AOC_EDITORFONT:
		return opt.dwEditorColor;
		break;
	case AOC_EDITORBK:
		return opt.dwBkEditorColor;
		break;
	default:
		ASSERT(FALSE);
		return 0;
	}//switch(colorType) {
}

int APPHelper::GetActiveDocumentPathNeedBytes()
{
	const int n=Command(AC_GETACTIVEDOCUMENTPATH,0,NULL);
	return n;
}

int APPHelper::GetActiveDocumentPath(UINT nBytes, LPTSTR lpszPath)
{
	ASSERT(!IsBadWritePtr(lpszPath,nBytes));

	const int n=Command(AC_GETACTIVEDOCUMENTPATH,nBytes,(LPARAM)lpszPath);
	return n;
}

WORD APPHelper::OpenDocumentEx(LPCTSTR lpszFullPathName, const int nPageID)
{
	return (WORD)Command(AC_OPENDOCUMENTEX,nPageID,(LPARAM)lpszFullPathName);
}

BOOL APPHelper::RestoreMainWindow()
{
	return Command(AC_SHOWMAINWINDOW,SC_RESTORE,0);
}

BOOL APPHelper::MinimizeMainWindow()
{
	return Command(AC_SHOWMAINWINDOW,SC_MINIMIZE,0);
}

BOOL APPHelper::ShowPageInTree(const WORD wDocID, const int nPageID)
{
	return Command(AC_SHOWPAGEINTREE,wDocID,nPageID);
}

BOOL APPHelper::ShowPageInTreeInActiveDoc(const int nPageID)
{
	return ShowPageInTree(AML_ACTIVEDOCUMENTID,nPageID);
}

int APPHelper::ImportFileToActiveDoc(LPCTSTR lpszPathName)
{
	ASSERT(!IsBadStringPtr(lpszPathName,INFINITE));
	return Command(AC_IMPORTFILE,-1,(LPARAM)lpszPathName);
}

#ifdef _UNICODE
LOGFONTW APPHelper::GetAmlTreeFont()
#else
LOGFONTA APPHelper::GetAmlTreeFont()
#endif
{
	AML_OPTIONS opt;
	opt.dwMask=APFOPT_TREEFONT;
	const BOOL b=GetOptions(&opt);
	ASSERT(b);

	return opt.lfTreeFont;
}

#ifdef _UNICODE
LOGFONTW APPHelper::GetAmlEditorFont()
#else
LOGFONTA APPHelper::GetAmlEditorFont()
#endif

{
	AML_OPTIONS opt;
	opt.dwMask=APFOPT_EDITORFONT;
	const BOOL b=GetOptions(&opt);
	
	return opt.lfEditorFont;
}

#ifdef _UNICODE
CHARFORMATW APPHelper::GetAmlEditorFontAsCHARFORMAT()
#else
CHARFORMATA APPHelper::GetAmlEditorFontAsCHARFORMAT()
#endif

{
	AML_OPTIONS opt;
	opt.dwMask=APFOPT_EDITORFONT;
	const BOOL b=GetOptions(&opt);
	ASSERT(b);

	return opt.cfEditorFont;
}

int APPHelper::DisplayFolderDialogForActiveDoc(const int nSelFolderID, RECT *pRectDlg)
{
	return Command(AC_DISPLAYFOLDERDIALOG,nSelFolderID,(LPARAM)pRectDlg);
}

BOOL APPHelper::SelectPage(const WORD wDocID, const int nPageID, const DWORD dwCursorPos)
{
	AML_PAGEINFO pi;
	memset(&pi,0,sizeof(pi));
	pi.cbSize=sizeof(pi);
	pi.nPageID=nPageID;
	pi.lCursorPos=dwCursorPos;

	return Command(AC_SELECTPAGE,wDocID,(LPARAM)&pi);
}

UINT APPHelper::GetManagementFilesListSizeInBytes()
{
	return Command(AC_GETMANAGEMENTFILESLIST,0,NULL);
}

UINT APPHelper::GetManagementFilesList(const UINT cbBufferBytes, TCHAR *const pBuffer)
{
	return Command(AC_GETMANAGEMENTFILESLIST,cbBufferBytes,(LPARAM)pBuffer);
}

void APPHelper::GetPluginsDirectory(TCHAR *psz, const UINT n)
{
	ASSERT(!IsBadWritePtr(psz,n*sizeof(TCHAR)));
	
	TCHAR szPath[MAX_PATH*2]={0};
	GetModuleFileName(NULL,szPath,ARRAYSIZE(szPath));
	if (0 == szPath[0])
		return;

	TCHAR szDrive[_MAX_DRIVE]={0}; 
	TCHAR szDir[_MAX_DIR]={0};

	_tsplitpath(szPath,szDrive,szDir,NULL,NULL);
	_tcscpy(psz,szDrive);
	_tcscat(psz,szDir);
	
	const int nLen=_tcslen(psz);
	if (nLen && _tcscmp((psz+nLen-1),TEXT("\\")))
		_tcscat(psz,TEXT("\\"));

	_tcscat(psz,TEXT("Plugins"));
}

BOOL APPHelper::IsReadOnlyDocument(const WORD wDocID)
{
	return Command(AC_ISREADONLYDOCUMENT,wDocID,0);
}

BOOL APPHelper::IsReadOnlyActiveDocument()
{
	return IsReadOnlyDocument(AML_ACTIVEDOCUMENTID);
}

BOOL APPHelper::GetFilenameFromPath(LPCTSTR lpszPath
									, LPTSTR lpszFilename
									, const UINT nSize)
{
	ASSERT(!IsBadStringPtr(lpszPath,INFINITE));
	ASSERT(!IsBadWritePtr(lpszFilename,nSize));

	if(IsBadStringPtr(lpszPath,INFINITE))
		return false;
	if(nSize && IsBadWritePtr(lpszFilename,nSize))
		return false;

	TCHAR szFNAME[_MAX_FNAME]={0};
	TCHAR szExt[_MAX_EXT]={0};

	_tsplitpath(lpszPath,NULL,NULL,szFNAME,szExt);
	const UINT nRequired=_tcslen(szFNAME)*sizeof(TCHAR)
		+_tcslen(szExt)*sizeof(TCHAR)
		+sizeof(TCHAR);

	if (!nSize)
		return nRequired;
	else {
		if (nRequired>nSize)
			return 0;
		else {
			_tcsncpy(lpszFilename,szFNAME,nSize);
			_tcsncat(lpszFilename,szExt,nSize);
			return nRequired;
		}
	}
}

int APPHelper::ReadTipOfTheDay(const UINT nBufferInBytesSize
							   , LPTSTR lpszBuffer)
{
	ASSERT(!IsBadWritePtr(lpszBuffer,nBufferInBytesSize));
	return Command(AC_READTIPOFTHEDAY
		,nBufferInBytesSize
		,(LPARAM)lpszBuffer);
}

BOOL APPHelper::NewStickyNoteToTheDocument(const BOOL bInsertClipboardToContent
										   , LPCTSTR lpszDocumentPath)
{
	ASSERT(!IsBadStringPtr(lpszDocumentPath,INFINITE));
	ASSERT(_tcslen(lpszDocumentPath));

	const BOOL b=Command(AC_NEWSTICKYNOTE
		,bInsertClipboardToContent
		,(LPARAM)lpszDocumentPath);
	return b;
}

void APPHelper::NewStickyNote(const BOOL bInsertClipboardToContent)
{
	Command(AC_NEWSTICKYNOTE,bInsertClipboardToContent,0);
}


BOOL APPHelper::DisplayFavoritesMenuForActiveDocument(POINT pt)
{
	return Command(AC_DISPLAYFAVORITESMENU,AML_ACTIVEDOCUMENTID,MAKELPARAM(pt.x,pt.y));
}

HICON APPHelper::ExtractPageIcon(const WORD wDocumentID, const int nPageID)
{
	return (HICON)Command(AC_EXTRACTPAGEICON,wDocumentID,nPageID);
}

HICON APPHelper::ExtractPageIconFromActiveDocument(const int nPageID)
{
	return ExtractPageIcon(AML_ACTIVEDOCUMENTID,nPageID);
}

BOOL APPHelper::DisplayAsStickyNote(const WORD wDocID, const int nPageID, const BOOL bDisplay)
{
	const WPARAM wParam=MAKEWPARAM(wDocID,bDisplay);
	return Command(AC_DISPLAYASSTICKYNOTE,wParam,nPageID);
}

BOOL APPHelper::DisplayActiveDocumentAsStickyNote(const int nPageID, const BOOL bDisplay)
{
	return DisplayAsStickyNote(AML_ACTIVEDOCUMENTID,nPageID,bDisplay);
}

BOOL APPHelper::SetOptions(const AML_OPTIONS *const pOpt)
{
	ASSERT(!IsBadReadPtr(pOpt,SIZE_AML_OPTIONS_VER3));
	return Command(AC_SETOPTIONS,0,(LPARAM)pOpt);
}

BOOL APPHelper::GetModuleVersionInfoOfAmlPages(AML_MODULEVERSIONINFO *pVersionInfo)
{
	TCHAR szPath[MAX_PATH+1]={0};
	GetModuleFileName(NULL,szPath,sizeof(szPath)/sizeof(TCHAR));
	
	return GetModuleVersionInfo(szPath,pVersionInfo);
}

WORD APPHelper::DocumentPathToID(LPCTSTR lpszDocumentPath)
{
	ASSERT(!IsBadStringPtr(lpszDocumentPath,INFINITE));
	ASSERT(_tcslen(lpszDocumentPath));

	const WORD wDoc=(WORD)Command(AC_DOCUMENTPATHTOID,(WPARAM)lpszDocumentPath,0);
	return wDoc;
}

BOOL APPHelper::ExecUserCommand(const WPARAM wParam, const LPARAM lParam)
{
	ASSERT(wParam >= AMLUC_FIRST);
	ASSERT(lParam <= AMLUC_LAST);

	return Command(AC_EXECUSERCOMMAND,wParam,lParam);
}

BOOL APPHelper::InsertPictureFromFile(WPARAM wDocID, LPCTSTR lpszPicFilename)
{
	ASSERT(!IsBadStringPtr(lpszPicFilename,INFINITE));
	ASSERT(_tcslen(lpszPicFilename));

	return Command(AC_INSERTPICTUREFROMFILE,wDocID,(LPARAM)lpszPicFilename);
}

BOOL APPHelper::SaveActiveDocumentAsDefaultCompressions()
{
	return Command(AC_EXECUSERCOMMAND,AMLUC_FILE_DOCUMENTSAVE,0);
}

HBRUSH APPHelper::GetAmlBkEditorBrush()
{
	AML_OPTIONS opt;
	opt.cbSize=sizeof(opt);
	opt.dwMask=APFOPT_BKEDITORBRUSH;

	if (!GetOptions(&opt)) {
		return NULL;
	}
	if (opt.dwMask & APFOPT_BKEDITORBRUSH) {
		return opt.hBkEditorBrush;
	}
	else
		return NULL;
}

LRESULT APPHelper::Command(UINT nCmd, WPARAM wParam, LPARAM lParam)
{
	const HMODULE h=GetModuleHandle(NULL);
	ASSERT(h);
	PFUNC_APC_COMMAND pfn=NULL;
#ifdef _DEBUG
	__try {
#endif
		pfn=(PFUNC_APC_COMMAND)GetProcAddress(h,"APC_Command");
#ifdef _DEBUG
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		TRACE(TEXT("Exception on GetProcAddress\n"));
		return 0;
	}
#endif
	if (pfn) {
		ASSERT(!IsBadCodePtr((FARPROC)pfn));

		const LRESULT nRet=pfn(nCmd,wParam,lParam);
		return nRet;
	}
	else {
		TRACE(TEXT("WARNING: Cannot get pointer to APC_COMMAND function from AmlPages.exe\n"));
		return 0;
	}
}

int APPHelper::GetPageType(const WORD wDocID, const int nPageID)
{
	return Command(AC_GETPAGETYPE,wDocID,nPageID);
}

BSTR APPHelper::ConcatenateText(BSTR lpsz1, const BOOL bRTF1, BSTR lpsz2, const BOOL bRTF2, const BOOL bResultAsRTF)
{
	ASSERT(lpsz1);
	ASSERT(lpsz2);
	
	AML_CONCTEXT param;
	param.bstrFirst=lpsz1;
	param.ptFirst=(bRTF1 ? PT_RTF : PT_TEXT);

	param.bstrSecond=lpsz2;
	param.ptSecond=(bRTF2 ? PT_RTF : PT_TEXT);

	param.ptResult=(bResultAsRTF ? PT_RTF : PT_TEXT);

	if (!Command(AC_CONCATENATETEXT,0,(LPARAM)&param)) {
		ASSERT(NULL == param.bstrResult);
		return NULL;
	}
	else
		return param.bstrResult;
}

BOOL APPHelper::DisplayColorDialog(AML_DISLAYCOLORDLG &color_info)
{
	ASSERT(!IsBadReadPtr(&color_info,sizeof(color_info)));
	ASSERT(!IsBadWritePtr(&color_info,sizeof(color_info)));

	ASSERT(color_info.cbSize == sizeof(color_info));

	const BOOL bRet=Command(AC_DISPLAYCOLORDLG,0,(LPARAM)&color_info);
	return bRet;		
}

void APPHelper::GetPluginsDirectoryWithSlash(TCHAR *psz, const UINT n)
{
	ASSERT(!IsBadWritePtr(psz, n*sizeof(TCHAR)));

	GetPluginsDirectory(psz,n);
	const int nLen=_tcslen(psz);
	ASSERT(nLen > 0);

	if (nLen>0) {
		const TCHAR sz=*(psz+(nLen-1));
		if (sz != '\\')
			_tcscat(psz, TEXT("\\"));
	}//if (nLen>0) {
}

BOOL APPHelper::GetSkin(AML_SKIN &skin)
{
	ASSERT(!IsBadReadPtr(&skin, sizeof(AML_SKIN)));
	ASSERT(!IsBadWritePtr(&skin, sizeof(AML_SKIN)));

	ASSERT(sizeof(AML_SKIN) == skin.cbSize);

	const BOOL bRetVal=Command(AC_GETSKIN,0,(LPARAM)&skin);
	return bRetVal;
}

BOOL APPHelper::SetSkin(const AML_SKIN &skin)
{
	ASSERT(!IsBadReadPtr(&skin, sizeof(AML_SKIN)));

	ASSERT(sizeof(AML_SKIN) == skin.cbSize);

	const BOOL bRetVal=Command(AC_SETSKIN,0,(LPARAM)&skin);
	return bRetVal;
}

BOOL APPHelper::GetDefaultSkin(AML_SKIN &skin)
{
	return (0 != Command(AC_GETDEDAULTSKIN,0,(LPARAM)&skin));
}

LRESULT APPHelper::IsHasDocumentChanges(const WORD wDocID)
{
	return Command(AC_DOCUMENTHASCHANGES,wDocID,0);
}

LRESULT APPHelper::IsHasActiveDocumentChanges()
{
	return IsHasDocumentChanges(AML_ACTIVEDOCUMENTID);
}

size_t APPHelper::GetAmlPagesOptionsIniPathName(const size_t nTCHARs, LPTSTR lpszPath)
{
	if (nTCHARs) { 
		ASSERT(lpszPath);
		ASSERT(!IsBadWritePtr(lpszPath,nTCHARs * sizeof(TCHAR)));
	}

	return Command(AC_GETOPTIONINIPATH,nTCHARs,(LPARAM)lpszPath);
}

size_t APPHelper::GetAmlPagesOptionsIniPathNameRequiredTCHARs()
{
	return Command(AC_GETOPTIONINIPATH,0,NULL);
}

BOOL APPHelper::IsUnicodeVersionOfAmlPages(const AML_PLUGIN_INFO &info)
{
	const BOOL bANSI=(info.cbSize <= 544);
	return (!bANSI);
}

BOOL APPHelper::IsAmlPagesRULanguage()
{
	const WORD wLang=GetUILanguage();
	return (1049 == wLang);
}

BOOL APPHelper::SelectPageEx(const WORD wDocID, AML_SELECTPAGEEX &sel_page)
{
	return (Command(AC_SELECTPAGEEX,wDocID,(LPARAM)&sel_page));
}

WORD APPHelper::OpenDocumentWithFlags(LPCTSTR lpszDocPath, const int nPageID, const BOOL bNoRestoreFromTray)
{
	ASSERT(!IsBadStringPtr(lpszDocPath,INFINITE));
	ASSERT(_tcslen(lpszDocPath));

	AML_OPENDOCUMENT param;
	param.lpszDocumentPath=lpszDocPath;
	param.nPageID=nPageID;
	if (bNoRestoreFromTray)
		param.dwFlags|=APFOD_DONTRESTOREFROMTRAY;

	const WORD wDocID=(WORD)(Command(AC_OPENDOCUMENTEX2,0,(LPARAM)&param));
	return wDocID;
}

int APPHelper::GetParentFolderID(const WPARAM wDocID, const int nPageID)
{
	return (Command(AC_GETPARENTFOLDERID,wDocID,nPageID));
}

static
void FreeStr(BSTR& bstr)
{
	if (bstr) {
		SysFreeString(bstr);
		bstr=NULL;
	}
}

void APPHelper::FreeMBCheckInfo(AML_MBCHECKINFO &info)
{
	FreeStr(info.bstrCaption);
	FreeStr(info.bstrButtonsText);
	FreeStr(info.bstrText);	
}

UINT APPHelper::MakeApdLinkURL(LPCTSTR lpszDocPathName
							   , const int nPageID
							   , LPCTSTR lpszBookmarkName
							   , LPTSTR lpszResURL
							   , const UINT cchTextMax)
{
	ASSERT(lpszDocPathName);
	ASSERT(!IsBadStringPtr(lpszDocPathName,INFINITE));
	ASSERT(_tcslen(lpszDocPathName));

	if (lpszBookmarkName) {
		ASSERT(!IsBadStringPtr(lpszBookmarkName,INFINITE));
		ASSERT(_tcslen(lpszBookmarkName));
	}

	if (lpszResURL) {
		ASSERT(cchTextMax);
		ASSERT(!IsBadWritePtr(lpszResURL,cchTextMax*sizeof(TCHAR)));
	}

	AML_MAKEAPDLINKURL s;
	s.lpszDocPathName=lpszDocPathName;
	s.nPageID=nPageID;
	s.lpszBookmarkName=lpszBookmarkName;
	s.lpszLinkResult=lpszResURL;
	s.cchTextMax=cchTextMax;

	const UINT n=Command(AC_MAKELINKAPDURL,0,(LPARAM)&s);
	return n;
}	

UINT APPHelper::MakApdLinkURL_IsRequiredTCHARs(LPCTSTR lpszDocPathName
											   , const int nPageID
											   , LPCTSTR lpszBookmarkName)
{
	ASSERT(lpszDocPathName);
	ASSERT(!IsBadStringPtr(lpszDocPathName,INFINITE));
	ASSERT(_tcslen(lpszDocPathName));

	if (lpszBookmarkName) {
		ASSERT(!IsBadStringPtr(lpszBookmarkName,INFINITE));
		ASSERT(_tcslen(lpszBookmarkName));
	}

	return MakeApdLinkURL(lpszBookmarkName,nPageID,lpszBookmarkName,NULL,0);
}

BOOL APPHelper::GetNodeInfo(const WORD wDocID, AML_NODEINFO *pInfo)
{
	return Command(AC_GETNODEINFO,wDocID,(LPARAM)pInfo);
}

BOOL APPHelper::GetNodeInfo_FromActiveDoc(AML_NODEINFO *pInfo)
{
	return GetNodeInfo(AML_ACTIVEDOCUMENTID,pInfo);
}

BOOL APPHelper::GetActiveNodeInfo_FromActiveDoc(AML_NODEINFO *pInfo)
{
	const int id=GetActivePageID(AML_ACTIVEDOCUMENTID);
	pInfo->nPageID=id;
	return GetNodeInfo_FromActiveDoc(pInfo);
}

BOOL APPHelper::IsUseRussianVersionOfAmlPages()
{
	enum {LANG_RUS=1049};
	return (LANG_RUS==GetUILanguage());
}

BOOL APPHelper::HightLightAllInCurrentRichEdit()
{
	return Command(AC_HIGHLIGHTALLINRE,0,0);
}

HWND APPHelper::GetRightPaneHWND_Only()
{
	return (HWND)Command(AC_GETRIGHTPANEHWND,AML_ACTIVEDOCUMENTID,NULL);
}

UINT APPHelper::GetRightPaneTypeOnly()
{
	DWORD dwFlags=0;
	const HWND hwnd=GetRightPaneHWND_AndType(dwFlags);
	if (NULL == hwnd)
		return 0;

	return dwFlags;
}

HWND APPHelper::GetRightPaneHWND_AndType(DWORD &dwFlags)
{
	return (HWND)Command(AC_GETRIGHTPANEHWND,AML_ACTIVEDOCUMENTID,(LPARAM)&dwFlags);
}

BOOL APPHelper::CheckNewVersionOnInternet(AML_CHECKNEWVERSION& info)
{
	return CheckNewVersionOnInternet(info,NULL,0);
}


BOOL APPHelper::CheckNewVersionOnInternet(AML_CHECKNEWVERSION& info,LPTSTR lpszURL, const UINT nLen)
{
	info.lpszURL=lpszURL;
	info.cchURL=nLen;

	const LRESULT lret=Command(AC_CHECKNEWVERSION,0,(LPARAM)&info);
	return (0 != lret);
}


BOOL APPHelper::PluginPaneCommand(AML_PANECOMMAND &cmd)
{
	return Command(AC_PANECOMMAND,0,(LPARAM)&cmd);
}

BOOL APPHelper::WriteLog_with_PLUG_TITLE(LPCTSTR lpszMessage)
{
	ASSERT(lpszMessage);
	ASSERT(!IsBadStringPtr(lpszMessage,INFINITE));
	ASSERT(_tcslen(lpszMessage));

	TCHAR szPrefix[AML_MAX_NAME+1]={0};
#ifdef PLUG_TITLE
	_tcsncpy(szPrefix,PLUG_TITLE,ARRAYSIZE(szPrefix)-1);
	LPCTSTR lpszPrefix=szPrefix;
#else
	LPCTSTR lpszPrefix=NULL;
#endif

	return WriteLog(lpszPrefix,lpszMessage);
}

BOOL APPHelper::BlinkCtrl(const HWND hwndCtrl)
{
	ASSERT(IsWindow(hwndCtrl));
	
	return Command(AC_BLINKCTRL,0,(LPARAM)hwndCtrl);

}

UINT APPHelper::FormatVersionDLL(const HMODULE hDLL
								 , LPTSTR lpszResult
								 , const UINT nMaxResult
								 , LPCTSTR lpszPrefix
								 , const BOOL bAllowBuild)
{
	ASSERT(hDLL);
	if (lpszResult && nMaxResult) {
		ASSERT(!IsBadWritePtr(lpszResult,nMaxResult*sizeof(TCHAR)));
	}

	TCHAR szPath[MAX_PATH+1] = {0};
	const DWORD dwLen=GetModuleFileName(hDLL,szPath,ARRAYSIZE(szPath)-1);
	if (0 == dwLen)
		return 0;

	AML_FORMAT_VERSION fv;
	fv.lpszPathToFile=szPath;
	fv.lpszResult=lpszResult;
	fv.cchMaxResult=nMaxResult;
	fv.lpszPrefix=lpszPrefix;
	if (bAllowBuild)
		fv.dwFlags |= APF_FV_WITHBUILD;

	const UINT n = Command(AC_FORMATVERSION,0,(LPARAM)&fv);
	return n;
}

int APPHelper::QueryPassword_For_Setup(AML_PASSWORD_INFO &info)
{
	ASSERT(info.lpszPassword);
	ASSERT(!IsBadStringPtr(info.lpszPassword,INFINITE));
	ASSERT(info.cchMaxPwd > 0) ;

	info.dwFlags |= APF_SETUPPWD;

	const int n=Command(AC_SHOW_PASSWORD_DLG,0,(LPARAM)&info);
	return n;
}

BOOL APPHelper::Is_AP_Settings_WriteLog()
{
	AML_SETTINGS ss;
	memset(&ss,0,sizeof(ss));
	ss.cbSize=sizeof(AML_SETTINGS);
	ss.dwMask=APF_SETTINGS_WRITELOG;

	const BOOL bCmd=Command(AC_GETSETTINGS,0,(LPARAM)&ss);
	if (!bCmd) {
		ASSERT(FALSE);
		return FALSE;
	}

	const BOOL bWriteLog=(APF_SETTINGS_WRITELOG==(ss.dwValue & APF_SETTINGS_WRITELOG));
	return bWriteLog;
}



BOOL APPHelper::Set_AP_Settings_WriteLog(const BOOL bWriteLog)
{
	AML_SETTINGS ss;
	memset(&ss,0,sizeof(ss));
	ss.cbSize=sizeof(AML_SETTINGS);
	ss.dwMask=APF_SETTINGS_WRITELOG;

	if (bWriteLog)
		ss.dwValue |= APF_SETTINGS_WRITELOG;

	const BOOL b=Command(AC_SETSETTINGS,0,(LPARAM)&ss);
	return b;
}

BOOL APPHelper::WriteLog(LPCTSTR lpszPrefix, LPCTSTR lpszMessage)
{
	if (lpszPrefix) {
		ASSERT(lpszPrefix);
		ASSERT(!IsBadStringPtr(lpszPrefix,INFINITE));
		ASSERT(_tcslen(lpszPrefix));
	}

	ASSERT(lpszMessage);
	ASSERT(!IsBadStringPtr(lpszMessage,INFINITE));
	ASSERT(_tcslen(lpszMessage));

	enum {MIN_BUILD_FOR_WRITELOG2=2797};

	const UINT nAp_Build_Number=APC_GetAmlPagesBuildNumber();
	if (nAp_Build_Number >= MIN_BUILD_FOR_WRITELOG2) {
		if (WriteLog_2(lpszPrefix,lpszMessage,APF_LOG_MESSAGE))
			return TRUE;
	}


	return Command(AC_WRITELOG,(WPARAM)lpszPrefix,(LPARAM)lpszMessage);
}

HMODULE APPHelper::GetPlugin_HMODULE_DLL()
{
	const HMODULE hKernel=GetModuleHandle(TEXT("kernel32.DLL"));
	ASSERT(hKernel);
	if (NULL == hKernel) {
		ASSERT(FALSE);
		return NULL;
	}
		

#ifdef _UNICODE
	typedef BOOL (WINAPI *PFN_GETMODULEHANDLEX)(DWORD, LPCWSTR,HMODULE*);
	LPCSTR lpszFUNC_NAME="GetModuleHandleExW";
#else
	typedef BOOL (WINAPI *PFN_GETMODULEHANDLEX)(DWORD, LPCSTR,HMODULE*);
	LPCSTR lpszFUNC_NAME="GetModuleHandleExA";
#endif
	PFN_GETMODULEHANDLEX pfn=(PFN_GETMODULEHANDLEX)GetProcAddress(hKernel,lpszFUNC_NAME);
	ASSERT(pfn);
 
	if (NULL == pfn)
		return NULL;

#ifndef GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
	enum {GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS=(0x00000004)};
#endif

	
	HMODULE hDLL=NULL;
	const BOOL b=pfn(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,(LPCTSTR)GetPlugin_HMODULE_DLL,&hDLL);
	ASSERT(b); 

#ifdef _DEBUG
	if (hDLL) {
		TCHAR szPathDLL[MAX_PATH*2]={0};
		GetModuleFileName(hDLL,szPathDLL,ARRAYSIZE(szPathDLL)-1);
	}
#endif

	return hDLL;
}

BOOL APPHelper::WriteLog_2(LPCTSTR lpszPrefix, LPCTSTR lpszMessage, const DWORD dwFlags)
{
	LPCTSTR lpszParam_PRefix=NULL;
		
	if (lpszPrefix) {
		ASSERT(!IsBadStringPtr(lpszPrefix,INFINITE));
		lpszParam_PRefix= lpszPrefix;
	}
		
	const HMODULE hPlugDLL=GetPlugin_HMODULE_DLL();
	
	if (NULL == lpszParam_PRefix) {
		TCHAR szTempBuff[MAX_PATH*5]={0};
#ifdef PLUG_TITLE
		_tcsncpy(szTempBuff,PLUG_TITLE,ARRAYSIZE(szTempBuff)-1);
		ASSERT(!IsBadStringPtr(szTempBuff,INFINITE));		
#endif

		TCHAR szPlugPathName[MAX_PATH*2]={0};
		GetModuleFileName(hPlugDLL,szPlugPathName,ARRAYSIZE(szPlugPathName)-1);

		TCHAR szFNAME[_MAX_FNAME+1]={0};
		TCHAR szExt[_MAX_EXT+1]={0};
		_tsplitpath(szPlugPathName,NULL,NULL,szFNAME,szExt );

		if (TEXT('\0') != szTempBuff[0]) {
			_tcscat(szTempBuff,TEXT(", "));
		}

		if (_tcslen(szFNAME)) {
			_tcscat(szTempBuff,szFNAME);
		}

		if (_tcslen(szExt)) {
			_tcscat(szTempBuff,szExt);
		}

		if (TEXT('\0') != szTempBuff[0])
			lpszParam_PRefix=szTempBuff;
	}

	AML_WRITELOG2 wl;
	wl.Init(hPlugDLL,lpszParam_PRefix,lpszMessage,dwFlags);

	const BOOL b=Command(AC_WRITELOG2,0,(LPARAM)&wl);
	return b;
}


UINT APPHelper::APC_GetAmlPagesBuildNumber()
{
	const HMODULE hExeAp=GetModuleHandle(NULL);
	ASSERT(hExeAp);

	if (NULL == hExeAp)
		return 0;

	
	typedef UINT (__stdcall *PFUNC_GETBUILD)(void);
	PFUNC_GETBUILD pfn=(PFUNC_GETBUILD)GetProcAddress(hExeAp,"APC_GetAmlPagesBuildNumber");
	ASSERT(pfn);
	if (NULL ==pfn)
		return 0;

	const UINT nBuildNumber=pfn();
	ASSERT(nBuildNumber);

	return nBuildNumber;
}


int APPHelper::MakePluginMenuTitle_WithBuild(LPTSTR lpszBuffer, const int nLen, LPCTSTR lpszTitle, const HMODULE hPluginDLL)
{
	return MakePluginMenuTitle_Internal(lpszBuffer,nLen,lpszTitle,hPluginDLL,true);
}


int APPHelper::MakePluginMenuTitle(LPTSTR lpszBuffer
									, const int nLen
									, LPCTSTR lpszTitleIN
									, const HMODULE hPluginDLL)
{
	return MakePluginMenuTitle_Internal(lpszBuffer,nLen,lpszTitleIN,hPluginDLL,false);
}

int APPHelper::MakePluginMenuTitle_Internal(LPTSTR lpszBuffer
	, const int nLen, LPCTSTR lpszTitleIN
	, const HMODULE hPluginDLL, const bool bWithBuild)
{
	ASSERT(hPluginDLL);
	if (lpszBuffer && nLen) {
		ASSERT(!IsBadWritePtr(lpszBuffer,nLen*sizeof(TCHAR)));
	}
		
	if (NULL == hPluginDLL)
		return 0;

	if (lpszBuffer && nLen) {
		if (IsBadWritePtr(lpszBuffer,nLen*sizeof(TCHAR)))
			return 0;
	}

	enum {MAX_TEMP=MAX_PATH*10};
	TCHAR szTemp[MAX_TEMP]={0};

	const bool bHas_In_Title=(lpszTitleIN && !IsBadStringPtr(lpszTitleIN,INFINITE));

	LPCTSTR lpszTitle=(bHas_In_Title)? lpszTitleIN : TEXT("");
	if (bHas_In_Title)
		_tcsncpy(szTemp,lpszTitle, ARRAYSIZE(szTemp)-1);

	
	AML_MODULEVERSIONINFO mv;
	mv.cbSize=sizeof(AML_MODULEVERSIONINFO);

	const BOOL  bGetVersion=GetModuleVersionInfoEx(hPluginDLL,&mv);
	
	if (bGetVersion) {
		const int _len=_tcslen(szTemp);
		if (_len > 0)
			_tcscat(szTemp,TEXT(" "));

		TCHAR szVersion[100]={0};
		_stprintf(szVersion,TEXT("%d.%d%d"),mv.dwMajor,mv.dwMinor,mv.dwRevision);

		_tcscat(szTemp,szVersion);
	}

	if (bWithBuild && bGetVersion) {
		const int _len=_tcslen(szTemp);
		if (_len > 0)
			_tcscat(szTemp,TEXT(" "));

		TCHAR szBN[100]={0};
		_stprintf(szBN,TEXT("%d"),mv.dwBuild);

		_tcscat(szTemp,TEXT("build "));
		_tcscat(szTemp,szBN);
	}

	const int nLenResult=_tcsclen(szTemp);
	

	if (NULL == lpszBuffer || 0 == nLen)
		return nLenResult+2;

	if (nLenResult >= nLen)
		return 0;

	_tcsncpy(lpszBuffer,szTemp,nLen);

	return nLenResult;

}

BOOL APPHelper::WriteLog_2_AS_ERROR(LPCTSTR lpszPrefix, LPCTSTR lpszMessage)
{
	return WriteLog_2(lpszPrefix,lpszMessage,APF_LOG_ERROR);
}

BOOL APPHelper::WriteLog_2_AS_WARNING(LPCTSTR lpszPrefix, LPCTSTR lpszMessage)
{
	return WriteLog_2(lpszPrefix,lpszMessage,APF_LOG_WARNING);
}

BOOL APPHelper::WriteLog_2_AS_MESSAGE(LPCTSTR lpszPrefix, LPCTSTR lpszMessage)
{
	return WriteLog_2(lpszPrefix,lpszMessage,APF_LOG_MESSAGE);
}

BOOL APPHelper::DoMenu_Command_ShowNews()
{
	const BOOL b=Command(AC_SHOW_NEWS,0,0);
	return b;
}

BOOL APPHelper::DoMenu_Command_CheckForUpdates()
{
	const BOOL b=Command(AC_CHECK_FOR_UPDATES,0,0);
	return b;
}

LRESULT APPHelper::Insert_New_Node_IntoActiveDoc(AML_NODEINFO& nodeInfo
	,LPCTSTR lpszConfirmDlgTitle)
{
	if (lpszConfirmDlgTitle) {
		ASSERT(!IsBadStringPtr(lpszConfirmDlgTitle,INFINITE));
		ASSERT(_tcslen(lpszConfirmDlgTitle));
	}

	return Insert_New_Node_IntoDoc(AML_ACTIVEDOCUMENTID,nodeInfo,lpszConfirmDlgTitle);
}

LRESULT APPHelper::Insert_New_Node_IntoDoc(const WPARAM wDocID, AML_NODEINFO& nodeInfo,LPCTSTR lpszConfirmDlgTitle)
{
	if (lpszConfirmDlgTitle) {
		ASSERT(!IsBadStringPtr(lpszConfirmDlgTitle,INFINITE));
		ASSERT(_tcslen(lpszConfirmDlgTitle));
	}

	if (lpszConfirmDlgTitle) {
		if (!IsBadStringPtr(lpszConfirmDlgTitle,INFINITE)) {
			nodeInfo.lpszQueryDlgTitle=lpszConfirmDlgTitle;
		}
	}

	const HMODULE hPlugin=GetPlugin_HMODULE_DLL();
	nodeInfo.hModuleFromPlugin=hPlugin;

	const LRESULT lret= Command(AC_INSERTNODE,wDocID,(LPARAM)&nodeInfo);
	return lret;
}

BOOL APPHelper::Is_Has_Document_Windows()
{
	const LRESULT lret=IsHasDocumentChanges(AML_ACTIVEDOCUMENTID);
	const BOOL bNoHas=(APFDS_DOCERROR == lret);

	return !bNoHas;
}

BOOL APPHelper::DoCommandAc_Allow_DND_FromIE_NO_REPORT()
{
	return DoCommandAc_Allow_DND_FromIE(FALSE,NULL);
}

BOOL APPHelper::DoCommandAc_Allow_DND_FromIE(const BOOL bWithReport, const HWND hwndReport)
{
	if (bWithReport) {
		if (hwndReport) {
			ASSERT(IsWindow(hwndReport));
		}
	}

	const BOOL bRes= Command(AC_ALLOW_DND_FROM_IE,bWithReport,(LPARAM)hwndReport);
	return bRes;
}

BOOL APPHelper::DoAc_Is_Allow_DND_FromIE()
{
	//BOOL bAllow=FALSE;
	const BOOL bRes=Command(AC_ISALLOW_DND_FROM_IE,0,0);//(LPARAM)&bAllow);
	return bRes;
	//ASSERT(bRes);
	//if (bRes)
	//	return bAllow;
	//else
	//	return FALSE;
}