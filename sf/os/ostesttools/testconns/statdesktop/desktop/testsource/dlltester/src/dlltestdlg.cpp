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




// dlltestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dlltest.h"
#include "dlltestDlg.h"

#include <utils.h>
#include <statexp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// function pointer prototypes
typedef	char * (*LPFN_Version)();
typedef	char * (*LPFN_GetError)(const int handle);
typedef	int (*LPFN_SetConnectionLogging)(const char * filename);
typedef	void (*LPFN_CloseConnectionLogging)();
typedef	int (*LPFN_Connect)(const STATCONNECTTYPE iConnectType, const char * pszPlatformType);
typedef	int (*LPFN_Disconnect)(const int handle);
typedef	int (*LPFN_SetCommandDelay)(const int handle, const unsigned int iMillisecondDelay);
typedef	int (*LPFN_SetCommandLogging)(const int handle, const char * pszLogPath, HWND messageWindow, const STATLOGLEVEL iLevel, const bool bAppend);
typedef	int (*LPFN_SetImageVerification)(const int handle, const char * pszRefDir, const bool bRemoveOldImages, const int iFactor);
typedef	int (*LPFN_SendRawCommand)(const int handle, const char * pszText);
typedef	int (*LPFN_SendCommandFile)(const int handle, const char * pszFile);
typedef	int (*LPFN_GetSnapshot)(const int handle, TBitmapFileHeader **ppFile, TBitmapInfoHeader **ppBitmap, char **ppData, unsigned long *pSize);

// initialise pointers
LPFN_Version pfn_Version = NULL;
LPFN_GetError pfn_GetError = NULL;
LPFN_SetConnectionLogging pfn_SetConnectionLogging = NULL;
LPFN_CloseConnectionLogging pfn_CloseConnectionLogging = NULL;
LPFN_Connect pfn_Connect = NULL;
LPFN_Disconnect pfn_Disconnect = NULL;
LPFN_SetCommandDelay pfn_SetCommandDelay = NULL;
LPFN_SetCommandLogging pfn_SetCommandLogging = NULL;
LPFN_SetImageVerification pfn_SetImageVerification = NULL;
LPFN_SendRawCommand pfn_SendRawCommand = NULL;
LPFN_SendCommandFile pfn_SendCommandFile = NULL;
LPFN_GetSnapshot pfn_GetSnapshot = NULL;

//////////////////////////////////////////////////////////////////////////
// This is the main thread function which listens on a socket and
// processes any incoming scripts
DWORD WINAPI
ThreadProc(LPVOID lpParameter)
{
	CDlltestDlg *pWorker = (CDlltestDlg *)lpParameter;

	pWorker->Run();

	return 1;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlltestDlg dialog

CDlltestDlg::CDlltestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlltestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlltestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDlltestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlltestDlg)
	DDX_Control(pDX, IDC_OVERWRITE, m_RemoveImages);
	DDX_Control(pDX, IDC_RUN, m_Run);
	DDX_Control(pDX, IDC_CHKLOGTOFILE, m_ChkLogtofile);
	DDX_Control(pDX, IDC_BROWSESNAPSHOT, m_BrowseSnapshot);
	DDX_Control(pDX, IDC_BROWSEREFIMAGES, m_BrowseRefImages);
	DDX_Control(pDX, IDC_BROWSELOG, m_BrowseLog);
	DDX_Control(pDX, IDC_BROWSECMDFILE, m_BrowseCmdFile);
	DDX_Control(pDX, IDC_CHECKCMDFILE, m_CheckCmdFile);
	DDX_Control(pDX, IDC_CHECKVERIFICATION, m_CheckVerification);
	DDX_Control(pDX, IDC_CHECKSNAPSHOT, m_CheckSnapshot);
	DDX_Control(pDX, IDC_CHECKRAWCMD, m_CheckRawCmd);
	DDX_Control(pDX, IDC_ADDRESS, m_Address);
	DDX_Control(pDX, IDC_CONNECTION, m_Connection);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_SNAPSHOT, m_Snapshot);
	DDX_Control(pDX, IDC_DELAY, m_Delay);
	DDX_Control(pDX, IDC_FUDGE, m_Fudge);
	DDX_Control(pDX, IDC_REFDIR, m_RefDir);
	DDX_Control(pDX, IDC_RAWCMD, m_RawCmd);
	DDX_Control(pDX, IDC_LOGFILE, m_LogFile);
	DDX_Control(pDX, IDC_CMDFILE, m_CmdFile);
	DDX_Control(pDX, IDC_ITERATIONS, m_Iterations);
	DDX_Control(pDX, IDC_PLATFORM, m_Platform);
	DDX_Control(pDX, IDC_LINK, m_Link);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlltestDlg, CDialog)
	//{{AFX_MSG_MAP(CDlltestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RUN, OnRun)
	ON_CBN_SELCHANGE(IDC_CONNECTION, OnChangeConnection)
	ON_BN_CLICKED(IDC_RESETSETTINGS, OnResetsettings)
	ON_BN_CLICKED(IDC_RESETTESTS, OnResettests)
	ON_BN_CLICKED(IDC_BROWSELOG, OnBrowselog)
	ON_BN_CLICKED(IDC_CHECKCMDFILE, OnCheckcmdfile)
	ON_BN_CLICKED(IDC_CHECKRAWCMD, OnCheckrawcmd)
	ON_BN_CLICKED(IDC_CHECKSNAPSHOT, OnChecksnapshot)
	ON_BN_CLICKED(IDC_CHECKVERIFICATION, OnCheckverification)
	ON_BN_CLICKED(IDC_BROWSECMDFILE, OnBrowsecmdfile)
	ON_BN_CLICKED(IDC_BROWSESNAPSHOT, OnBrowsesnapshot)
	ON_BN_CLICKED(IDC_BROWSEREFIMAGES, OnBrowserefimages)
	ON_BN_CLICKED(IDC_CHKLOGTOFILE, OnChklogtofile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlltestDlg message handlers

BOOL CDlltestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_Snapshot.SetLimitText(sizeof(szBuffer));
	m_RefDir.SetLimitText(sizeof(szBuffer));
	m_RawCmd.SetLimitText(sizeof(szBuffer));
	m_LogFile.SetLimitText(sizeof(szBuffer));
	m_CmdFile.SetLimitText(sizeof(szBuffer));

	// initialise our settings
	OnResetsettings();
	OnResettests();
	ReadSettings();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

//////////////////////////////////////////////////////////////////////////////////////

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDlltestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//////////////////////////////////////////////////////////////////////////////////////

HCURSOR CDlltestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnRun() 
{
	if (bRunning)
	{
		if (IDYES == AfxMessageBox(_T("The work task has not completed its current job and stopping it now may cause it to be left in an unstable state.  Do you wish to abort the current task?"), MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2))
		{
			TerminateThread(hThreadHandle, (unsigned long)-1);
			EnableFields(TRUE);
		}
	}
	else
	{
		//call Run to run through the script (with no appending to log file in this case) - spawn a thread...
		DWORD dwThreadID;
		if (hThreadHandle = CreateThread( NULL,					// security attributes
												0,				// stack size
												ThreadProc,		// proc to call
												this,			// proc parameter
												0,				// creation flags
												&dwThreadID))	// thread identifier
		{
			EnableFields(FALSE);

			MSG msg;
			while(WaitForSingleObject(hThreadHandle, STAT_THREAD_SLEEP_TIME) == WAIT_TIMEOUT)
			{
				PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE);
				DispatchMessage(&msg);
			}

			EnableFields(TRUE);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::Run()
{
	struct TBitmapFileHeader *pFile = NULL;
	struct TBitmapInfoHeader *pInfo = NULL;
	char *pData = NULL;
	char szPrevious[10] = {0};
	char szRemaining[10] = {0};
	unsigned long lSize = 0;

	bErrors = false;

	m_Progress.SetRange(0, 100);
	m_Progress.SetPos(0);
	m_Progress.SetStep(GetIncrement());

	CString logFile;		// We use this file name throughout the
							// function and we should save it.
	CString logFilePath;	// Also, we have a bit of a mixture
							// of calls to make to set up logging
							// and some require a file name (and assume a
							// folder) and some require a full path.
							// Generate a full path (based on the file name
							// above) and use whichever variable suits
							// each function call.

	if (m_ChkLogtofile.GetCheck())
	{
		m_LogFile.GetWindowText(logFile);
		GetLogFilePath(logFile,logFilePath);
	}

	// connect to runtime dll
	if (m_Link.GetCurSel() == 1)
	{
		// display version
		MessageBox(Version(), "STAT DLL Version", MB_OK);

		CWaitCursor oWait;

		// log connections
		if (m_ChkLogtofile.GetCheck())
		{
			if (0 != logFilePath.GetLength())
			{
				SetConnectionLogging(logFilePath);
			}
		}

		// get the connect string
		switch(m_Connection.GetCurSel())
		{
		case 1:	// serial
		case 2:	// infra-red
		case 3:	// bluetooth
		case 4:	// USB
			m_Platform.GetLBText(m_Platform.GetCurSel(), szBuffer);
			break;
		case 0:	// socket
			m_Address.GetWindowText(szBuffer, sizeof(szBuffer));
			break;
		}

		// connect
		Evaluate(h = Connect((STATCONNECTTYPE)(m_Connection.GetCurSel() + 1), szBuffer, NULL, NULL), h);

		if (h)
		{
			m_Progress.StepIt();
			UpdateWindow();

			// log commands
			if (m_ChkLogtofile.GetCheck())
			{
				if (0 != logFile.GetLength())
					Evaluate(SetCommandLogging(h, logFile, NULL, EVerbose, true, NULL, NULL), h);
			}

			if (!bErrors)
			{
				// set command delay
				m_Delay.GetWindowText(szBuffer, sizeof(szBuffer));
				iValue = atoi(szBuffer);
				Evaluate(SetCommandDelay(h, iValue), h);
			}

			if (!bErrors)
			{
				// set image verification
				if (m_CheckVerification.GetCheck())
				{
					m_Fudge.GetWindowText(szBuffer, sizeof(szBuffer));
					iValue = atoi(szBuffer);
					m_RefDir.GetWindowText(szBuffer, sizeof(szBuffer));
					if (*szBuffer)
						Evaluate(SetImageVerification(h, szBuffer, m_RemoveImages.GetCheck()?true:false, iValue), h);
				}
			}

			m_Iterations.GetWindowText(szPrevious, sizeof(szPrevious));
			int iMax = atoi(szPrevious);
			for (int i=0;i<iMax;i++)
			{
				if (!bErrors)
				{
					// execute raw command
					if (m_CheckRawCmd.GetCheck())
					{
						m_RawCmd.GetWindowText(szBuffer, sizeof(szBuffer));
						if (*szBuffer)
						{
							Evaluate(SendRawCommand(h, szBuffer), h);
							m_Progress.StepIt();
							UpdateWindow();
						}
					}
				}

				if (!bErrors)
				{
					// execute command file
					if (m_CheckCmdFile.GetCheck())
					{
						m_CmdFile.GetWindowText(szBuffer, sizeof(szBuffer));
						if (*szBuffer)
						{
							Evaluate(SendCommandFile(h, szBuffer), h);
							m_Progress.StepIt();
							UpdateWindow();
						}
					}
				}

				if (!bErrors)
				{
					// get a snapshot
					if (m_CheckSnapshot.GetCheck())
					{
						m_Snapshot.GetWindowText(szBuffer, sizeof(szBuffer));
						if (*szBuffer)
						{
							DeleteFile(szBuffer);
							Evaluate(GetSnapshot(h, &pFile, &pInfo, &pData, &lSize), h);

							if (!bErrors)
								if (!WriteBitmap(szBuffer, pFile, pInfo, pData, lSize))
									bErrors = true;

							m_Progress.StepIt();
							UpdateWindow();
						}
					}
				}

				// decrement the iteration count
				itoa(iMax - i - 1, szRemaining, 10);
				m_Iterations.SetWindowText(szRemaining);
				UpdateWindow();
			}

			// disconnect
			Evaluate(Disconnect(h), h);
			m_Progress.SetPos(100);
			m_Iterations.SetWindowText(szPrevious);
		}

		if (m_ChkLogtofile.GetCheck())
			CloseConnectionLogging();
	}
	else
	{
		// connect to loaded dll
#if defined _DEBUG
		HINSTANCE inst = LoadLibrary("statd.dll");
#else // defined _DEBUG
		HINSTANCE inst = LoadLibrary("stat.dll");
#endif // defined _DEBUG
		if (inst && LoadFunctionEntrypoints(inst))
		{
			// display version
			MessageBox((*pfn_Version)(), "STAT DLL Version", MB_OK);

			CWaitCursor oWait;

			// log connections
			if (m_ChkLogtofile.GetCheck())
			{
				if (0 != logFilePath.GetLength())
				{
					(*pfn_SetConnectionLogging)(logFilePath);
				}
			}

			// get the connect string
			switch(m_Connection.GetCurSel())
			{
			case 1:	// serial
			case 2:	// infra-red
			case 3:	// bluetooth
			case 4:	// USB
				m_Platform.GetLBText(m_Platform.GetCurSel(), szBuffer);
				break;
			
			case 0:	// socket
				m_Address.GetWindowText(szBuffer, sizeof(szBuffer));
				break;
			}

			// connect
			Evaluate(h = (*pfn_Connect)((STATCONNECTTYPE)(m_Connection.GetCurSel() + 1), szBuffer), h);

			if (h)
			{
				m_Progress.StepIt();
				UpdateWindow();

				// log commands
				if (m_ChkLogtofile.GetCheck())
				{
					if (0 != logFile.GetLength())
						Evaluate((*pfn_SetCommandLogging)(h, logFile, NULL, EVerbose, true), h);
				}

				// set command delay
				if (!bErrors)
				{
					m_Delay.GetWindowText(szBuffer, sizeof(szBuffer));
					iValue = atoi(szBuffer);
					Evaluate((*pfn_SetCommandDelay)(h, iValue), h);
				}

				// set image verification
				if (!bErrors)
				{
					if (m_CheckVerification.GetCheck())
					{
						m_Fudge.GetWindowText(szBuffer, sizeof(szBuffer));
						iValue = atoi(szBuffer);
						m_RefDir.GetWindowText(szBuffer, sizeof(szBuffer));
						if (*szBuffer)
							Evaluate((*pfn_SetImageVerification)(h, szBuffer, m_RemoveImages.GetCheck()?true:false, iValue), h);
					}
				}

				m_Iterations.GetWindowText(szPrevious, sizeof(szPrevious));
				int iMax = atoi(szPrevious);
				for (int i=0;i<iMax;i++)
				{
					if (!bErrors)
					{
						// execute raw command
						if (m_CheckRawCmd.GetCheck())
						{
							m_RawCmd.GetWindowText(szBuffer, sizeof(szBuffer));
							if (*szBuffer)
							{
								Evaluate((*pfn_SendRawCommand)(h, szBuffer), h);
								m_Progress.StepIt();
								UpdateWindow();
							}
						}
					}

					if (!bErrors)
					{
						// execute command file
						if (m_CheckCmdFile.GetCheck())
						{
							m_CmdFile.GetWindowText(szBuffer, sizeof(szBuffer));
							if (*szBuffer)
							{
								Evaluate((*pfn_SendCommandFile)(h, szBuffer), h);
								m_Progress.StepIt();
								UpdateWindow();
							}
						}
					}

					if (!bErrors)
					{
						// get a snapshot
						if (m_CheckSnapshot.GetCheck())
						{
							m_Snapshot.GetWindowText(szBuffer, sizeof(szBuffer));
							if (*szBuffer)
							{
								DeleteFile(szBuffer);
								Evaluate((*pfn_GetSnapshot)(h, &pFile, &pInfo, &pData, &lSize), h);

								if (!bErrors)
									if (!WriteBitmap(szBuffer, pFile, pInfo, pData, lSize))
										bErrors = true;

								m_Progress.StepIt();
								UpdateWindow();
							}
						}
					}

					// decrement the iteration count
					itoa(iMax - i - 1, szRemaining, 10);
					m_Iterations.SetWindowText(szRemaining);
					UpdateWindow();
				}

				// disconnect
				Evaluate((*pfn_Disconnect)(h), h); // start of list
				m_Progress.SetPos(100);
				m_Iterations.SetWindowText(szPrevious);
			}

			// close logging
			if (m_ChkLogtofile.GetCheck())
				if (pfn_CloseConnectionLogging)	// doesn't exist in 3.2
					(*pfn_CloseConnectionLogging)();
		}
		else
			MessageBox("STAT.DLL could not be loaded successfully", NULL, MB_OK|MB_ICONERROR);

		// finished
		if (inst)
			FreeLibrary(inst);
	}

	if (m_ChkLogtofile.GetCheck() && (0 != logFile.GetLength()))
	{
		if ((bErrors && (IDYES == AfxMessageBox("Do you wish to view the log file?", MB_YESNO | MB_ICONINFORMATION | MB_DEFBUTTON1))) ||
			(!bErrors && (IDYES == AfxMessageBox("All tests completed successfully.  Do you wish to view the log file?", MB_YESNO | MB_ICONINFORMATION | MB_DEFBUTTON2))))
		{
			CString logFileDirectory;
			GetLogFolder(logFileDirectory);
			::ShellExecute(GetSafeHwnd(), NULL, logFile, NULL, 
				logFileDirectory, SW_SHOW);
		}
	}
	else
	{
		if (!bErrors)
			AfxMessageBox("All tests completed successfully.", MB_OK | MB_ICONINFORMATION);
	}
}

//////////////////////////////////////////////////////////////////////////////////////

int CDlltestDlg::GetIncrement()
{
	int increment = 0;

	if (m_CheckRawCmd.GetCheck())
		increment++;

	if (m_CheckCmdFile.GetCheck())
		increment++;

	if (m_CheckSnapshot.GetCheck())
		increment++;

	m_Iterations.GetWindowText(szBuffer, sizeof(szBuffer));
	int iMax = atoi(szBuffer);
	if (iMax)
		increment *= iMax;

	// for Connect()/Disconnect()
	increment++;

	return (int) 100 / increment;
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::LoadEntrypoint(HINSTANCE inst, FARPROC *fn, const char *name)
{
	if (!*fn)
		*fn = GetProcAddress(inst, name);
}

//////////////////////////////////////////////////////////////////////////////////////

bool CDlltestDlg::LoadFunctionEntrypoints(HINSTANCE inst)
{
	// get
	LoadEntrypoint(inst, (FARPROC *)&pfn_Version, "Version");
	LoadEntrypoint(inst, (FARPROC *)&pfn_GetError, "GetError");
	LoadEntrypoint(inst, (FARPROC *)&pfn_SetConnectionLogging, "SetConnectionLogging");
	LoadEntrypoint(inst, (FARPROC *)&pfn_CloseConnectionLogging, "CloseConnectionLogging");
	LoadEntrypoint(inst, (FARPROC *)&pfn_Connect, "Connect");
	LoadEntrypoint(inst, (FARPROC *)&pfn_Disconnect, "Disconnect");
	LoadEntrypoint(inst, (FARPROC *)&pfn_SetCommandDelay, "SetCommandDelay");
	LoadEntrypoint(inst, (FARPROC *)&pfn_SetCommandLogging, "SetCommandLogging");
	LoadEntrypoint(inst, (FARPROC *)&pfn_SetImageVerification, "SetImageVerification");
	LoadEntrypoint(inst, (FARPROC *)&pfn_SendRawCommand, "SendRawCommand");
	LoadEntrypoint(inst, (FARPROC *)&pfn_SendCommandFile, "SendCommandFile");
	LoadEntrypoint(inst, (FARPROC *)&pfn_GetSnapshot, "GetSnapshot");


	// check
	if (pfn_Version && pfn_GetError && pfn_SetConnectionLogging && pfn_Connect && pfn_Disconnect &&
		pfn_SetCommandDelay && pfn_SetCommandLogging && pfn_GetSnapshot && pfn_CloseConnectionLogging &&
		pfn_SetImageVerification && pfn_SendRawCommand && pfn_SendCommandFile )
	{
		return true;
	}

	MessageBox("Not all required STAT entrypoints could be located.  This application is compatible with STAT DLL 3.3 (September 2002) and later.", NULL, MB_OK|MB_ICONERROR);
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::Evaluate(int ret, int handle)
{
	if (!ret)
	{
		if (m_Link.GetCurSel() == 1)
			strcpy(szBuffer, GetError(handle));
		else
			strcpy(szBuffer, pfn_GetError(handle));

		bErrors = true;
		MessageBox(szBuffer, NULL, MB_OK|MB_ICONERROR);
	}
}

//////////////////////////////////////////////////////////////////////////////////////

bool CDlltestDlg::WriteBitmap(char *file, TBitmapFileHeader *fileheader, TBitmapInfoHeader * bmpHeader, char *bmpBits, unsigned long lSize)
{
	bool success = false;

	// write the whole lot out to file
	HANDLE infile;
	if (INVALID_HANDLE_VALUE != (infile = CreateFile(file,
										   GENERIC_WRITE,
										   0, 
										   NULL, 
										   CREATE_ALWAYS,
										   FILE_ATTRIBUTE_NORMAL,
										   0)))
	{
		DWORD dwBytesWritten = 0;

		// write the file info
		if (WriteFile(infile, (LPVOID *)fileheader, sizeof(TBitmapFileHeader), &dwBytesWritten, NULL) &&
			dwBytesWritten == sizeof(TBitmapFileHeader))
		{
			// write the bitmap info
			if (WriteFile(infile, (LPVOID *)bmpHeader, sizeof(TBitmapInfoHeader), &dwBytesWritten, NULL) &&
				dwBytesWritten == sizeof(TBitmapInfoHeader))
			{
				// write the bitmap data
				if (WriteFile(infile, (LPVOID *)bmpBits, lSize, &dwBytesWritten, NULL) &&
					dwBytesWritten == lSize)
				{
					success = true;
				}
			}
		}

		CloseHandle(infile);
	}

	return success;
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnChangeConnection() 
{
	ChangeConnection(0);
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::ChangeConnection(int iSelection) 
{
	switch(m_Connection.GetCurSel())
	{
	case 0:	// socket
		m_Address.EnableWindow();
		m_Platform.EnableWindow(FALSE);
		break;
	
	case 1:	// serial
	case 2:	// infra-red
	case 3:	// bluetooth
	case 4:	// USB
		m_Platform.ResetContent();
		m_Platform.AddString("COM1");
		m_Platform.AddString("COM2");
		m_Platform.AddString("COM3");
		m_Platform.AddString("COM4");
		m_Platform.AddString("COM5");
		m_Platform.AddString("COM6");
		m_Platform.AddString("COM7");
		m_Platform.SetCurSel(iSelection);
		m_Platform.EnableWindow(TRUE);
		m_Address.EnableWindow(FALSE);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::SaveSettings()
{
	(*szBuffer) = (char)0;
	m_Iterations.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_ITERATIONS,ST_TEST_KEY);

	(*szBuffer) = (char)0;
	m_Delay.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_DELAY,ST_TEST_KEY);

	(*szBuffer) = (char)0;
	m_LogFile.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_LOGFILE,ST_TEST_KEY);

	(*szBuffer) = (char)0;
	m_CmdFile.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_CMDFILE,ST_TEST_KEY);

	(*szBuffer) = (char)0;
	m_RawCmd.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_RAWCMD,ST_TEST_KEY);

	(*szBuffer) = (char)0;
	m_Fudge.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_FUDGE,ST_TEST_KEY);

	(*szBuffer) = (char)0;
	m_Snapshot.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_SNAPSHOT,ST_TEST_KEY);


	(*szBuffer) = (char)0;
	m_RefDir.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_REFDIR,ST_TEST_KEY);

	(*szBuffer) = (char)0;
	m_Address.GetWindowText(szBuffer, sizeof(szBuffer));
	statIniFile.WriteKey(szBuffer,ST_ADDRESS,ST_TEST_KEY);

	_ltot( m_Link.GetCurSel(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_LINK,ST_TEST_KEY);

	_ltot( m_Connection.GetCurSel(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_CONNECTIONDLL,ST_TEST_KEY);

	_ltot( m_Platform.GetCurSel(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_PLATFORM,ST_TEST_KEY);

	_ltot( m_CheckRawCmd.GetCheck(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_CHKRAWCMD,ST_TEST_KEY);

	_ltot( m_CheckCmdFile.GetCheck(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_CHKCMDFILE,ST_TEST_KEY);

	_ltot( m_CheckVerification.GetCheck(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_CHKVERIF,ST_TEST_KEY);

	_ltot( m_RemoveImages.GetCheck(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_VERIFYREMOVEIMAGES,ST_TEST_KEY);

	_ltot( m_CheckSnapshot.GetCheck(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_CHKSNAP,ST_TEST_KEY);

	_ltot( m_ChkLogtofile.GetCheck(), szBuffer, 10);
	statIniFile.WriteKey(szBuffer,ST_CHKLOGTOFILE,ST_TEST_KEY);
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::ReadSettings()
{
	long lCount = 0;
	statIniFile.SetIniFileName(STAT_INI_NAME);
	*(szBuffer) = (char)0;
	if(statIniFile.SectionExists(ST_TEST_KEY) )
	{
		CString setting;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_ITERATIONS,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			if (_ttoi(setting)<1)
				setting=_T("1");

			m_Iterations.SetWindowText(setting);
		}


		setting=statIniFile.GetKeyValue(ST_DELAY,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			int iPos = _ttoi(setting);
			if (iPos < 100)
				setting= _T("100");

			if (iPos > 30000)
				setting= _T("30000");

			m_Delay.SetWindowText(setting);
		}

	// logging settings
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_LOGFILE,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			m_LogFile.SetWindowText(setting);
		}

		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_CHKLOGTOFILE,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount = _ttoi(setting);
			m_ChkLogtofile.SetCheck(lCount);
		}



	// command file settings
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_CMDFILE,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			m_CmdFile.SetWindowText(setting);
		}

		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_CHKCMDFILE,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount = _ttoi(setting);
			m_CheckCmdFile.SetCheck(lCount);
		}

	// raw command settings
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_RAWCMD,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			m_RawCmd.SetWindowText(setting);
		}
		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_CHKRAWCMD,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
			m_CheckRawCmd.SetCheck(lCount);
		}


	// snapshot settings
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_SNAPSHOT,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			m_Snapshot.SetWindowText(setting);
		}
		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_CHKSNAP,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
			m_CheckSnapshot.SetCheck(lCount);
		}


	// image verification settings
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_FUDGE,ST_TEST_KEY);
		if(!setting.IsEmpty())
			m_Fudge.SetWindowText(setting);

		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_REFDIR,ST_TEST_KEY);
		if(!setting.IsEmpty())
			m_RefDir.SetWindowText(setting);
		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_CHKVERIF,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
			m_CheckVerification.SetCheck(lCount);
		}

		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_VERIFYREMOVEIMAGES,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			m_RemoveImages.SetWindowText(setting);
		}
		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_VERIFYREMOVEIMAGES,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
			m_RemoveImages.SetCheck(lCount);
		}


	// connection settings
		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_LINK,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
			m_Link.SetCurSel(lCount);
		}
		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_CONNECTIONDLL,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
			m_Connection.SetCurSel(lCount);
		}
		setting.Empty();
		lCount=0;
		setting=statIniFile.GetKeyValue(ST_PLATFORM,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount=_ttol(setting);
			ChangeConnection(lCount);
		}
		else
			ChangeConnection(0);
		
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_ADDRESS,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			m_Address.SetWindowText(setting);
		}
	}
	// enable/disable accordingly
	OnCheckcmdfile();
	OnCheckrawcmd();
	OnChecksnapshot();
	OnCheckverification();

	bRunning = false;

	UpdateWindow();
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnOK() 
{
	CWaitCursor oWait;
	SaveSettings();
	CDialog::OnOK();
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnResetsettings() 
{

	GetDlgItem(IDC_LOGFILE)->SetWindowText("statdllv4cdecltest.log");
	m_ChkLogtofile.SetCheck(TRUE);
	OnChklogtofile();

	GetDlgItem(IDC_ADDRESS)->SetWindowText("");
	GetDlgItem(IDC_DELAY)->SetWindowText("100");
	GetDlgItem(IDC_ITERATIONS)->SetWindowText("1");
	m_Link.SetCurSel(0);
	m_Connection.SetCurSel(0);
	ChangeConnection(0);

	UpdateWindow();
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnResettests() 
{
	// set some default values

	GetDlgItem(IDC_CMDFILE)->SetWindowText("statdllv4test.txt");

	// tests
	GetDlgItem(IDC_ITERATIONS)->SetWindowText("1");
	GetDlgItem(IDC_RAWCMD)->SetWindowText("<B><S><E>");
	GetDlgItem(IDC_REFDIR)->SetWindowText("refimages");
	GetDlgItem(IDC_FUDGE)->SetWindowText("20");
	GetDlgItem(IDC_SNAPSHOT)->SetWindowText("mysnapshot.bmp");

	m_CheckCmdFile.SetCheck(TRUE);
	m_CheckVerification.SetCheck(TRUE);
	m_RemoveImages.SetCheck(TRUE);
	m_CheckSnapshot.SetCheck(TRUE);
	m_CheckRawCmd.SetCheck(TRUE);
	OnCheckcmdfile();
	OnCheckrawcmd();
	OnChecksnapshot();
	OnCheckverification();

	UpdateWindow();
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnBrowselog() 
{
	// use the folder of the current script as a starting point
	m_LogFile.GetWindowText(szBuffer, sizeof(szBuffer));
	CString buf = szBuffer;
	if (!buf.IsEmpty())
	{
		int index = buf.ReverseFind('\\');
		if (index != -1)
		{
			CString folder = buf.Left(index);
			SetCurrentDirectory(folder.GetBuffer(0));
		}
	}

	CFileDialog dlgStatFileOpen(TRUE, 
								NULL, 
								NULL, 
								OFN_PATHMUSTEXIST, 
								_T("Log Files (*.log)|*.log|All Files (*.*)|*.*||"));

	// new script file to use
	if (dlgStatFileOpen.DoModal() == IDOK)	//when ok is pressed
		m_LogFile.SetWindowText(dlgStatFileOpen.GetPathName());
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnChklogtofile() 
{
	if (m_ChkLogtofile.GetCheck())
	{
		m_LogFile.EnableWindow(TRUE);
		m_BrowseLog.EnableWindow(TRUE);
	}
	else
	{
		m_LogFile.EnableWindow(FALSE);
		m_BrowseLog.EnableWindow(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnCheckcmdfile() 
{
	if (m_CheckCmdFile.GetCheck())
	{
		m_CmdFile.EnableWindow(TRUE);
		m_BrowseCmdFile.EnableWindow(TRUE);
	}
	else
	{
		m_CmdFile.EnableWindow(FALSE);
		m_BrowseCmdFile.EnableWindow(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnCheckrawcmd() 
{
	if (m_CheckRawCmd.GetCheck())
	{
		m_RawCmd.EnableWindow(TRUE);
	}
	else
	{
		m_RawCmd.EnableWindow(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnChecksnapshot() 
{
	if (m_CheckSnapshot.GetCheck())
	{
		m_Snapshot.EnableWindow(TRUE);
		m_BrowseSnapshot.EnableWindow(TRUE);
	}
	else
	{
		m_Snapshot.EnableWindow(FALSE);
		m_BrowseSnapshot.EnableWindow(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnCheckverification() 
{
	if (m_CheckVerification.GetCheck())
	{
		m_RefDir.EnableWindow(TRUE);
		m_Fudge.EnableWindow(TRUE);
		m_BrowseRefImages.EnableWindow(TRUE);
		m_RemoveImages.EnableWindow(TRUE);
	}
	else
	{
		m_RefDir.EnableWindow(FALSE);
		m_Fudge.EnableWindow(FALSE);
		m_BrowseRefImages.EnableWindow(FALSE);
		m_RemoveImages.EnableWindow(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnBrowsecmdfile() 
{
	// use the folder of the current script as a starting point
	m_CmdFile.GetWindowText(szBuffer, sizeof(szBuffer));
	CString buf = szBuffer;
	if (!buf.IsEmpty())
	{
		int index = buf.ReverseFind('\\');
		if (index != -1)
		{
			CString folder = buf.Left(index);
			SetCurrentDirectory(folder.GetBuffer(0));
		}
	}

	CFileDialog dlgStatFileOpen(TRUE, 
								NULL, 
								NULL, 
								OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, 
								_T("Script Files (*.txt)|*.txt|All Files (*.*)|*.*||"));

	// new script file to use
	if (dlgStatFileOpen.DoModal() == IDOK)	//when ok is pressed
		m_CmdFile.SetWindowText(dlgStatFileOpen.GetPathName());
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnBrowsesnapshot() 
{
	// use the folder of the current script as a starting point
	m_Snapshot.GetWindowText(szBuffer, sizeof(szBuffer));
	CString buf = szBuffer;
	if (!buf.IsEmpty())
	{
		int index = buf.ReverseFind('\\');
		if (index != -1)
		{
			CString folder = buf.Left(index);
			SetCurrentDirectory(folder.GetBuffer(0));
		}
	}

	CFileDialog dlgStatFileOpen(TRUE, 
								NULL, 
								NULL, 
								OFN_PATHMUSTEXIST,
								_T("Bitmap Files (*.bmp)|*.txt|All Files (*.*)|*.*||"));

	// new script file to use
	if (dlgStatFileOpen.DoModal() == IDOK)	//when ok is pressed
		m_Snapshot.SetWindowText(dlgStatFileOpen.GetPathName());
}

//////////////////////////////////////////////////////////////////////////////////////

void CDlltestDlg::OnBrowserefimages() 
{
	TCHAR chName[MAX_PATH];

	BROWSEINFO bi = {0};
	bi.hwndOwner = m_hWnd;
	bi.lpszTitle = _T("Please select folder where the reference bitmap files are stored...");

	LPITEMIDLIST lpIDList = ::SHBrowseForFolder(&bi); //now display dialog box 

	::SHGetPathFromIDList(lpIDList, chName); //Converts an item identifier list to a file system path

	m_RefDir.SetWindowText(chName);	//pastes location into edit box
}


//////////////////////////////////////////////////////////////////////////////////////
// Set up image verification
void CDlltestDlg::EnableFields(bool bEnable)
{
	GetDlgItem(IDC_LINK)->EnableWindow(bEnable);
	GetDlgItem(IDC_DELAY)->EnableWindow(bEnable);
//	GetDlgItem(IDC_ITERATIONS)->EnableWindow(bEnable);
	GetDlgItem(IDC_RESETSETTINGS)->EnableWindow(bEnable);
	GetDlgItem(IDC_CONNECTION)->EnableWindow(bEnable);
	GetDlgItem(IDC_PLATFORM)->EnableWindow(bEnable);
	GetDlgItem(IDC_ADDRESS)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHKLOGTOFILE)->EnableWindow(bEnable);
	GetDlgItem(IDC_LOGFILE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BROWSELOG)->EnableWindow(bEnable);

	GetDlgItem(IDC_CHECKRAWCMD)->EnableWindow(bEnable);
	GetDlgItem(IDC_RAWCMD)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECKCMDFILE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CMDFILE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BROWSECMDFILE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECKSNAPSHOT)->EnableWindow(bEnable);
	GetDlgItem(IDC_SNAPSHOT)->EnableWindow(bEnable);
	GetDlgItem(IDC_BROWSESNAPSHOT)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECKVERIFICATION)->EnableWindow(bEnable);
	GetDlgItem(IDC_OVERWRITE)->EnableWindow(bEnable);
	GetDlgItem(IDC_REFDIR)->EnableWindow(bEnable);
	GetDlgItem(IDC_BROWSEREFIMAGES)->EnableWindow(bEnable);
	GetDlgItem(IDC_FUDGE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RESETTESTS)->EnableWindow(bEnable);

	if (bEnable)
	{
		OnChklogtofile();
		OnCheckcmdfile();
		OnCheckrawcmd();
		OnChecksnapshot();
		OnCheckverification();

		m_Run.SetWindowText(_T("Run"));
		bRunning = false;
	}
	else
	{
		m_Run.SetWindowText(_T("Stop"));
		bRunning = true;
	}

	UpdateWindow();
}

void CDlltestDlg::GetLogFolder(CString& logFileFolder) const
{
	// Get the log file folder location.
	// If this is specified in the 'ini' file then we
	// use the specified location.
	// if it is not specified in the 'ini' file then we
	// use a default value.

	logFileFolder.Empty();

	if(statIniFile.SectionExists(ST_TEST_KEY))
	{
		CString setting;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_LOGFILEPATH,ST_TEST_KEY);
		if(!setting.IsEmpty())
			logFileFolder = setting;
	}

	if(logFileFolder.IsEmpty())
	{
		logFileFolder = STAT_LOGFILEPATH_VALUE;
	}
}

void CDlltestDlg::GetLogFilePath(const char* logFileName, CString& logFilePath) const
{
	// There are a mixture of method calls in this window
	// that specify logging.
	// Also, in this window we allow the user to specify a file
	// name or a full path in the interface.
	// Because of this we have a mix of specifying log file details
	// with or without a full path.
	// This function will create a full path for the log file from the
	// name passed.  If the name passed already has a folder
	// specified (at least on '\' characters) then we copy
	// that to the out parameter.
	// If the name passed does not have a folder specification
	// within it then we prepend the log file folder.

	static const char folder = '\\';

	if(NULL != ::strchr(logFileName, folder))
	{
		logFilePath = logFileName;
	}
	else
	{
		GetLogFolder(logFilePath);
		logFilePath += "\\";
		logFilePath += logFileName;
	}
}

