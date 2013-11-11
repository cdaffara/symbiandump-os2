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




// testcomDlg.h : header file
//

#if !defined(AFX_TESTCOMDLG_H__F5EDDB58_81F0_11D6_BC69_00B0D065107F__INCLUDED_)
#define AFX_TESTCOMDLG_H__F5EDDB58_81F0_11D6_BC69_00B0D065107F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestcomDlg dialog

class CTestcomDlg : public CDialog
{
// Construction
public:
	CTestcomDlg(CWnd* pParent = NULL);	// standard constructor
	~CTestcomDlg();
	void AddString(char *szText);
	int ReadTransferFile(const char *pFile, char **ppContents, unsigned long *pLength);
	void ReleaseData(char **ppData);

	HANDLE hThread2Out;

// Dialog Data
	//{{AFX_DATA(CTestcomDlg)
	enum { IDD = IDD_TESTCOM_DIALOG };
	CEdit	m_Port;
	CComboBox	m_Transport;
	CListBox	m_List1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestcomDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestcomDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTransfer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCOMDLG_H__F5EDDB58_81F0_11D6_BC69_00B0D065107F__INCLUDED_)
