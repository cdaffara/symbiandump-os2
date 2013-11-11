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




// dlltestDlg.h : header file
//

#if !defined(AFX_DLLTESTDLG_H__04DFB726_4E37_11D6_BC5F_00B0D065107F__INCLUDED_)
#define AFX_DLLTESTDLG_H__04DFB726_4E37_11D6_BC5F_00B0D065107F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <statcommon.h>
#include <ini.h>

/////////////////////////////////////////////////////////////////////////////
// CDlltestDlg dialog

class CDlltestDlg : public CDialog
{
// Construction
public:
	CDlltestDlg(CWnd* pParent = NULL);	// standard constructor
	void Run();

// Dialog Data
	//{{AFX_DATA(CDlltestDlg)
	enum { IDD = IDD_DLLTEST_DIALOG };
	CButton	m_RemoveImages;
	CButton	m_Run;
	CButton	m_ChkLogtofile;
	CButton	m_BrowseSnapshot;
	CButton	m_BrowseRefImages;
	CButton	m_BrowseLog;
	CButton	m_BrowseCmdFile;
	CButton	m_CheckCmdFile;
	CButton	m_CheckVerification;
	CButton	m_CheckSnapshot;
	CButton	m_CheckRawCmd;
	CEdit	m_Address;
	CComboBox	m_Connection;
	CProgressCtrl	m_Progress;
	CEdit	m_Snapshot;
	CEdit	m_Delay;
	CEdit	m_Fudge;
	CEdit	m_RefDir;
	CEdit	m_RawCmd;
	CEdit	m_LogFile;
	CEdit	m_CmdFile;
	CEdit	m_Iterations;
	CComboBox	m_Platform;
	CComboBox	m_Link;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlltestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDlltestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRun();
	afx_msg void OnChangeConnection();
	virtual void OnOK();
	afx_msg void OnResetsettings();
	afx_msg void OnResettests();
	afx_msg void OnBrowselog();
	afx_msg void OnCheckcmdfile();
	afx_msg void OnCheckrawcmd();
	afx_msg void OnChecksnapshot();
	afx_msg void OnCheckverification();
	afx_msg void OnBrowsecmdfile();
	afx_msg void OnBrowsesnapshot();
	afx_msg void OnBrowserefimages();
	afx_msg void OnChklogtofile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void LoadEntrypoint(HINSTANCE inst, FARPROC *fn, const char *name);
	bool LoadFunctionEntrypoints(HINSTANCE inst);
	void Evaluate(int ret, int handle);
	bool WriteBitmap(char *file, TBitmapFileHeader *fileheader, TBitmapInfoHeader * bmpHeader, char *bmpBits, unsigned long lSize);
	int GetIncrement();
	void ReadSettings();
	void SaveSettings();
	void AddConnectPlatforms();
	void AddPortPlatforms();
	void ChangeConnection(int iSelection);
	void EnableFields(bool bEnable);
	void GetLogFolder(CString& logFileFolder) const;
	void GetLogFilePath(const char* logFileName, CString& logFilePath) const;

	bool bRunning;				// flag set if currently processing a script
	HANDLE hThreadHandle;		// handle to our worker thread
	char szBuffer[2048];		// global buffer for strings
	int iValue;					// global buffer for ints
	int h;						// handle to stat
	bool bErrors;
	CStatIniFile statIniFile;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLTESTDLG_H__04DFB726_4E37_11D6_BC5F_00B0D065107F__INCLUDED_)
