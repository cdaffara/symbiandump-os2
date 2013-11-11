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




#if !defined(AFX_STATMANAGECONNECTION_H__49661E91_DF72_11D6_BC83_00B0D065107F__INCLUDED_)
#define AFX_STATMANAGECONNECTION_H__49661E91_DF72_11D6_BC83_00B0D065107F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// STATManageConnection.h : header file
//

#include <STATExp.h>

#include "ini.h"

/////////////////////////////////////////////////////////////////////////////
// STATManageConnection dialog

class STATManageConnection : public CDialog
{
// Construction
public:
	STATManageConnection(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(STATManageConnection)
	enum { IDD = IDD_MANAGECONNECTION };
	CEdit	m_Address;
	CComboBox	m_Transport;
	CListBox	m_ListConnections;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(STATManageConnection)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(STATManageConnection)
	afx_msg void OnRemove();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd();
	virtual void OnCancel();
	afx_msg void OnSelchangeListconnections();
	afx_msg void OnSelchangeTransport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool InList(CString & Entry);
	static LPTSTR ToUnicode(const char *string);					// convert ANSI to Unicode
	static char * ToAnsi(LPCTSTR string);							// convert Unicode to ANSI

	CStatIniFile statIniFile;
	long lCount;
};

extern CString cConnectionInfo;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATMANAGECONNECTION_H__49661E91_DF72_11D6_BC83_00B0D065107F__INCLUDED_)
