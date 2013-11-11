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




// myclientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myclient.h"
#include "myclientDlg.h"
#include <statsocket.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CMyclientDlg dialog

CMyclientDlg::CMyclientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyclientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyclientDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyclientDlg)
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_RECEIVE, m_Receive);
	DDX_Control(pDX, IDC_CHKLISTEN, m_ChkListen);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_MACHINE, m_Machine);
	DDX_Control(pDX, IDC_CONNECT, m_Connect);
	DDX_Control(pDX, IDC_SEND, m_Send);
	DDX_Control(pDX, IDC_SENDMSG, m_SendMsg);
	DDX_Control(pDX, IDC_RECVLIST, m_RecvList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyclientDlg, CDialog)
	//{{AFX_MSG_MAP(CMyclientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_RECEIVE, OnReceive)
	ON_BN_CLICKED(IDC_CHKLISTEN, OnChklisten)
	ON_BN_CLICKED(IDC_INFO, OnInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyclientDlg message handlers

BOOL CMyclientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	
	hThreadHandle = 0;
	pSocket = NULL;

	m_Machine.SetWindowText("LON-PHILH03");
	m_Port.SetWindowText("3001");
	m_SendMsg.SetWindowText("<B><S><E>");

	Prepare();
	ResetDeviceInfo();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMyclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyclientDlg::OnPaint() 
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

HCURSOR CMyclientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMyclientDlg::OnSend() 
{
	if (pSocket)
	{
		char szData[256];
		m_SendMsg.GetWindowText(szData, 256);
		if (*szData)
		{
			CWaitCursor oWait;

			if (pSocket->Send(SymbianScript, szData, strlen(szData)) != ITS_OK)
			{
				AddString("Could not send.  Try to re-connect.");
				Prepare();
			}
		}
	}
	else
		AddString("Bad socket");
}

void CMyclientDlg::OnReceive() 
{
	if (pSocket)
	{
		CWaitCursor oWait;

		char ID = 0;
		unsigned long Length = 0;
		char *pData = NULL;

		if (pSocket->Receive(&ID, &pData, &Length) != ITS_OK)
		{
			AddString("Could not receive.  Try to re-connect.");
			Prepare();
		}
		else
		{
			char szBuffer[120] = {0};

			switch(ID)
			{
			case SymbianScript:
				if ((Length < 80) && pData)
					sprintf(szBuffer, "%ld [%s]", Length, pData);
				else
				{
					sprintf(szBuffer, "Received %ld bytes of data", Length);
					OpenDataInNotepad(pData, Length);
				}
				break;
			case SymbianError:
				if ((Length < 100) && pData)
					sprintf(szBuffer, "ERROR: %ld [%s]", Length, pData);
				else
				{
					sprintf(szBuffer, "ERROR: Received %ld bytes of data", Length);
					OpenDataInNotepad(pData, Length);
				}
				break;
			case SymbianDeviceInfo:
				StoreDeviceInformation(pData, Length);
				DisplayDeviceInformation(pData);
				break;
			};

			AddString(szBuffer);
		}

		if (pData)
		{
			delete [] pData;
			pData = NULL;
		}
	}
	else
		AddString("Bad socket");
}

void CMyclientDlg::OnConnect() 
{
	bool IsConnected = false;

	pSocket = new CSTATSocket;
	if (pSocket)
	{
		CWaitCursor oWait;

		if (pSocket->Initialise() != ITS_OK)
			return;

		char szMachine[256] = {0};
		if (!m_ChkListen.GetCheck())
		{
			// get machine name and port number
			m_Machine.GetWindowText(szMachine, 247);
			strcat(szMachine, ":");
			m_Port.GetWindowText(szMachine + strlen(szMachine), 8);
		}
		else
		{
			m_Port.GetWindowText(szMachine, 8);
		}

		if (pSocket->Connect(szMachine) == ITS_OK)
		{
			AddString("Connected.");
			m_SendMsg.EnableWindow(TRUE);
			m_Send.EnableWindow(TRUE);
			m_Receive.EnableWindow(TRUE);
			m_Connect.EnableWindow(FALSE);
			m_ChkListen.EnableWindow(FALSE);
			m_Machine.EnableWindow(FALSE);
			m_Port.EnableWindow(FALSE);
			m_Info.EnableWindow(TRUE);
		}
		else
			AddString("Could not connect.");
	}
	else
		AddString("Memory error.");
}


void CMyclientDlg::OnChklisten() 
{
	if (m_ChkListen.GetCheck())
	{
		m_Machine.EnableWindow(FALSE);
	}
	else
	{
		m_Machine.EnableWindow(TRUE);
	}
}

void CMyclientDlg::OnOK() 
{
	Prepare();

	// delete once we've finished with it
	DeleteFile(STAT_TEMP_FILE);

	CDialog::OnOK();
}


void CMyclientDlg::OnInfo() 
{
	if (pSocket)
	{
		if (pSocket->Send(SymbianDeviceInfo) != ITS_OK)
		{
			AddString("Could not send.  Try to re-connect.");
			Prepare();
		}
		else
		{
			OnReceive();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// Add a string to the list box
///////////////////////////////////////////////////////////////////////////////////
void CMyclientDlg::AddString(char *szText)
{
	m_RecvList.AddString(szText);
	int nCount = m_RecvList.GetCount();
	if (nCount > 0)
		m_RecvList.SetTopIndex(nCount - 1);
	UpdateWindow();
}


//////////////////////////////////////////////////////////////////////////////////////
// Open a text file in Notepad
void CMyclientDlg::OpenDataInNotepad(char *pContents, unsigned long ulLength)
{
	// save data
	int ret = E_BADFILENAME;

	CFile script_file;
	if (script_file.Open(STAT_TEMP_FILE, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
	{
		try
		{
			// might be a zero-length file
			if (pContents && ulLength)
				script_file.Write(pContents, ulLength);

			ret = ITS_OK;
		}
		catch(CFileException *e)
		{
			e->Delete();
			ret = GENERAL_FAILURE;
		}

		script_file.Flush();
		script_file.Close();
	}


	if (ret == ITS_OK)
	{
		// open file
		TCHAR szCurrentDir[MAX_PATH + 1];
		if (GetWindowsDirectory(szCurrentDir, sizeof(szCurrentDir)))
		{
			CString cBuf;
			cBuf = _T("\"");
			cBuf += szCurrentDir;
			cBuf += _T("\\Notepad.exe\" ");
			cBuf += STAT_TEMP_FILE;

			if (!CallProcess(NULL, cBuf.GetBuffer(0), NULL))
				MessageBox(_T("Could not invoke Notepad.exe to view script file"), NULL, MB_OK);
		}
		else
			MessageBox(_T("Could not locate Windows directory"), NULL, MB_OK);
	}
}


//////////////////////////////////////////////////////////////////////////////////////
// Invoke a process
bool CMyclientDlg::CallProcess(LPCTSTR szApplication, LPTSTR szCommandLine, LPCTSTR szDirectory)
{
	bool valid = false;
	STARTUPINFO startInfo = {0};
	startInfo.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION procInfo = {0};

	// event attributes for the child process
	SECURITY_ATTRIBUTES eventAttr;
	eventAttr.nLength = sizeof(eventAttr);
	eventAttr.lpSecurityDescriptor = NULL;
	eventAttr.bInheritHandle = TRUE;

	if (CreateProcess(szApplication, szCommandLine, NULL, NULL, FALSE, NULL, NULL,
					  szDirectory, &startInfo, &procInfo))
	{
		valid = true;
		CloseHandle(procInfo.hThread);
		CloseHandle(procInfo.hProcess);
	}

	return valid;
}


//////////////////////////////////////////////////////////////////////////////////////
// Reset dialog buttons
void CMyclientDlg::Prepare()
{
	m_Info.EnableWindow(FALSE);
	m_SendMsg.EnableWindow(FALSE);
	m_Send.EnableWindow(FALSE);
	m_Receive.EnableWindow(FALSE);
	m_Connect.EnableWindow(TRUE);
	m_ChkListen.EnableWindow(TRUE);

	if (pSocket)
	{
		pSocket->Disconnect();
		pSocket->Release();
		delete pSocket;
		pSocket = NULL;
	}
}


//----------------------------------------------------------------------------
// Break up newline delimited list of device info and store
void CMyclientDlg::StoreDeviceInformation(char *info, unsigned long length)
{
	ResetDeviceInfo();
	if (length && info)
	{
		char szInfo[2048] = {0};
		char *pEnd = szInfo;
		strncpy(szInfo, info, length);

		char *p[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		int i = 0;

		// skip over leading CR/LF's
		while ((*pEnd) && (*pEnd) == '\r' || (*pEnd) == '\n')
		{
			(*pEnd) = (char)0;
			pEnd++;
		}

		// split
		while(*pEnd)
		{
			p[i] = pEnd;

			// step over setting
			while ((*pEnd) && (*pEnd) != '\r' && (*pEnd) != '\n')
				pEnd++;

			// null terminate
			while ((*pEnd) && (*pEnd) == '\r' || (*pEnd) == '\n')
			{
				(*pEnd) = (char)0;
				pEnd++;
			}

			i++;
			if (i > 18)
				break;
		}
		*pEnd = (char)0;

		// assign
		strcpy(szMachine, p[0]);
		strcpy(szCPU, p[1]);
		strcpy(szInterface, p[2]);
		strcpy(szFamily, p[3]);
		strcpy(szManufacturer, p[4]);
		lModel = atol(p[5]);
		lFamilyRevision = atol(p[6]);
		lHardwareRevision = atol(p[7]);
		lSoftwareRevision = atol(p[8]);
		lSoftwareBuild = atol(p[9]);
		lMemoryPageSize = atol(p[10]);
		lMemoryRAM = atol(p[11]);
		lMemoryRAMFree = atol(p[12]);
		lMemoryROM = atol(p[13]);
		iDelay = atoi(p[14]);
		iImageVerification = atoi(p[15]);
		if (iImageVerification)
		{
			strcpy(szRefDir, p[16]);
			iFudge = atoi(p[17]);
		}
	}
}


//----------------------------------------------------------------------------
// Clear any device info settings
void CMyclientDlg::ResetDeviceInfo()
{
	// device info
	memset(&szMachine, 0, sizeof(szMachine));
	memset(&szCPU, 0, sizeof(szCPU));
	memset(&szInterface, 0, sizeof(szInterface));
	memset(&szFamily, 0, sizeof(szFamily));
	memset(&szManufacturer, 0, sizeof(szManufacturer));
	lModel = 0;
	lFamilyRevision = 0;
	lHardwareRevision = 0;
	lSoftwareRevision = 0;
	lSoftwareBuild = 0;
	lMemoryPageSize = 0;
	lMemoryRAM = 0;
	lMemoryRAMFree = 0;
	lMemoryROM = 0;
	iDelay = 0;
	iImageVerification = 0;
	memset(&szRefDir, 0, sizeof(szRefDir));
	iFudge = 0;
}


//----------------------------------------------------------------------------
// Clear any device info settings
void CMyclientDlg::DisplayDeviceInformation(char *data)
{
	MessageBox(data, "Info", MB_OK);
}

