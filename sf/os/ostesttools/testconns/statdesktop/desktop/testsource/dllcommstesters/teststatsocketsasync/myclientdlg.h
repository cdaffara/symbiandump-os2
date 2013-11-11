/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/




// myclientDlg.h : header file
//

#if !defined(AFX_MYCLIENTDLG_H__DCBFD0A5_CBD8_11D6_AF94_000000000000__INCLUDED_)
#define AFX_MYCLIENTDLG_H__DCBFD0A5_CBD8_11D6_AF94_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <statsocket.h>

/////////////////////////////////////////////////////////////////////////////
// CMyclientDlg dialog

class CMyclientDlg : public CDialog
{
// Construction
public:
	CMyclientDlg(CWnd* pParent = NULL);	// standard constructor

	CSTATSocket* pSocket;

// Dialog Data
	//{{AFX_DATA(CMyclientDlg)
	enum { IDD = IDD_MYCLIENT_DIALOG };
	CButton	m_ChkListen;
	CEdit	m_Port;
	CEdit	m_Machine;
	CButton	m_Connect;
	CButton	m_Send;
	CEdit	m_SendMsg;
	CListBox	m_RecvList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyclientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyclientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSend();
	afx_msg void OnConnect();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void GetMessages();

	HANDLE hThreadHandle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYCLIENTDLG_H__DCBFD0A5_CBD8_11D6_AF94_000000000000__INCLUDED_)
