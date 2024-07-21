#if !defined(AFX_LISTCTRLEX_H__8EBFDFF5_8EB0_4427_92B4_7FC7370C017E__INCLUDED_)
#define AFX_LISTCTRLEX_H__8EBFDFF5_8EB0_4427_92B4_7FC7370C017E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window

#define MAX_NUM 32
#define LAST_NUM 12

class CListCtrlEx : public CListCtrl
{
// Construction
public:
	CListCtrlEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void InsertSearchItems();
	CEverythingTabApp* m_pApp;
	virtual ~CListCtrlEx();
	void InsertHelpItems();
	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEx)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:

    CMenu m_Menu;
	CImageList m_SmallImg;
    enum
    {
        ID_STRING1=10000,
        ID_STRING2,
        ID_STRING3
    };
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEX_H__8EBFDFF5_8EB0_4427_92B4_7FC7370C017E__INCLUDED_)
