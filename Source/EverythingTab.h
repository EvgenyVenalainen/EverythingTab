// EverythingTab.h : main header file for the EVERYTHINGTAB DLL
//

#if !defined(AFX_EVERYTHINGTAB_H__EC9D0D96_93D9_471C_A5E1_5847D8A04B32__INCLUDED_)
#define AFX_EVERYTHINGTAB_H__EC9D0D96_93D9_471C_A5E1_5847D8A04B32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEverythingTabApp
// See EverythingTab.cpp for the implementation of this class
//

class CEverythingTabApp : public CWinApp
{
public:
	BOOL SetStatus(int n);
	BOOL ReadReg();
	BOOL WriteReg();
	int m_bAuto;
	int m_bPrefix;
	CString m_sPrefix;
	CString m_sSearch;
	static CString GetEverythingVersion();
	CEverythingTabApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEverythingTabApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CEverythingTabApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVERYTHINGTAB_H__EC9D0D96_93D9_471C_A5E1_5847D8A04B32__INCLUDED_)
