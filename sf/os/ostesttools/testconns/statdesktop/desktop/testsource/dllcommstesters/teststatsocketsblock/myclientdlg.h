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

class CSTATSocket;

#define STAT_TEMP_FILE	"C:\\stat_temp_file.tmp"

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
	CButton	m_Info;
	CButton	m_Receive;
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
	afx_msg void OnReceive();
	afx_msg void OnChklisten();
	virtual void OnOK();
	afx_msg void OnInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void Prepare();
	void OpenDataInNotepad(char *pContents, unsigned long ulLength);
	bool CallProcess(LPCTSTR szApplication, LPTSTR szCommandLine, LPCTSTR szDirectory);
	void AddString(char *szText);
	void StoreDeviceInformation(char *info, unsigned long length);
	void ResetDeviceInfo();
	void DisplayDeviceInformation(char *data);

	HANDLE hThreadHandle;

	// device information - retains information of connected device after <D> command is run
	char szMachine[256];
	char szCPU[256];
	char szInterface[256];
	char szFamily[256];
	char szManufacturer[256];
	long lModel;
	long lFamilyRevision;
	long lHardwareRevision;
	long lSoftwareRevision;
	long lSoftwareBuild;
	long lMemoryPageSize;
	long lMemoryRAM;
	long lMemoryRAMFree;
	long lMemoryROM;
	int iDelay;
	int iImageVerification;
	char szRefDir[256];
	int iFudge;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYCLIENTDLG_H__DCBFD0A5_CBD8_11D6_AF94_000000000000__INCLUDED_)
