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





#if !defined(AFX_STATDESKTOPDLG_H__3BAC3E09_6B00_11D5_B4C5_00C04F437BB4__INCLUDED_)
#define AFX_STATDESKTOPDLG_H__3BAC3E09_6B00_11D5_B4C5_00C04F437BB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <STATExp.h>

#include "ini.h"

#include "MessageReporterImp.h"

/////////////////////////////////////////////////////////////////////////////
// CSTATDesktopDlg dialog

class CSTATDesktopDlg : public CDialog
{
// Construction
public:
	CSTATDesktopDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSTATDesktopDlg)
	enum { IDD = IDD_STATDESKTOP_DIALOG };
	CEdit	m_Connection;
	CButton	m_ViewLog;
	CProgressCtrl	m_Progress;
	CButton	m_BrowseLog;
	CButton	m_Append;
	CButton	m_BrowseImage;
	CEdit	m_Delay;
	CListBox	m_Output;
	CButton	m_VerifyImage;
	CEdit	m_LogFileName;
	CButton	m_LogToFile;
	CEdit	m_ImageBox;
	CEdit	m_Script;
	CEdit	m_ImageLocation;
	CSliderCtrl	m_SliderPos;
	CSliderCtrl	m_PercentageDiff;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTATDesktopDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSTATDesktopDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	afx_msg void OnEditscript();
	afx_msg void OnExit();
	afx_msg void OnRun();
	afx_msg void OnLogToFile();
	afx_msg void OnReleasedcaptureSpeedslider(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeTimedelay();
	afx_msg void OnBrowseimage();
	virtual void OnCancel();
	afx_msg void OnImageverify();
	afx_msg void OnBrowseLog();
	afx_msg void OnView();
	afx_msg void OnManage();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnDoneCommand(WPARAM command, LPARAM );
	afx_msg LRESULT OnDoneScript(WPARAM iScriptExitCode, LPARAM );
	afx_msg LRESULT OnLogMesage(WPARAM, LPARAM pLogMessage );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ReadSettings();
	void SaveSettings();
	bool CallProcess(LPCTSTR szApplication, LPTSTR szCommandLine, LPCTSTR szDirectory);
	int SetImageVerification();
	void Message(const char* message);
	void MessageFormat(const char *message, ...);
	void EnableFields(bool bEnable);
	int ConnectToEngine();
	bool ShowScreenshot(const CString& filename);				//displays last screenshot in window
	void CleanUp(const CString& message, bool bEnableFields = true);
	void CleanUp(LPCTSTR message, bool bEnableFields = true);
	void OpenFile(LPTSTR file);
	void RunIt();

	//variables
	HMODULE	ihLib;
	int iHandle;
	PROC_VERSION	iptrVersion;
	PROC_GETERROR	iptrGetError;
	PROC_GETERRORTEXT	iptrGetErrorText;
	PROC_SETCONNECTIONLOGGING	iptrSetConnectionLogging;
	PROC_CLOSECONNECTIONLOGGING	iptrCloseConnectionLogging;
	PROC_CONNECT	iptrConnect;
	PROC_DISCONNECT	iptrDisconnect;
	PROC_SETCOMMANDDELAY	iptrSetCommandDelay;
	PROC_SETCOMMANDLOGGING	iptrSetCommandLogging;
	PROC_SETIMAGEVERIFICATION	iptrSetImageVerification;
	PROC_OPENSCRIPTFILE	iptrOpenScriptFile;
	PROC_RUNSCRIPT	iptrRunScript;
	PROC_SENDRAWCOMMAND	iptrSendRawCommand;
	PROC_SENDCOMMANDFILE	iptrSendCommandFile;
	PROC_GETCOMMANDCOUNT	iptrGetCommandCount;
	PROC_GETCURRENTCOMMANDNUMBER	iptrGetCurrentCommandNumber;
	PROC_STOPPROCESSING	iptrStopProcessing;
	PROC_GETSNAPSHOT	iptrGetSnapShot;

	MessageReporterImp	*iMessageReporter;

	CStatIniFile statIniFile;
	TCHAR szBuffer[2048];		// global buffer for strings
	HANDLE hThreadHandle;
	int iLastVerify;			// stores the state of verify checkbox
	int iLastLog;				// stores the state of log checkbox
	bool bRunning;				// flag whether processing or not

	/****BITMAP STUFF****/
	CPalette m_Palette;
	unsigned char *m_pDib, *m_pDibBits;
	DWORD m_dwDibSize;
	BITMAPINFOHEADER *m_pBIH;
	RGBQUAD *m_pPalette;
	int m_nPaletteEntries;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATDESKTOPDLG_H__3BAC3E09_6B00_11D5_B4C5_00C04F437BB4__INCLUDED_)
