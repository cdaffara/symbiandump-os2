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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyclientDlg message handlers

BOOL CMyclientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		exit(0);
	}

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
	
	m_SendMsg.EnableWindow(FALSE);
	m_Send.EnableWindow(FALSE);

	m_Machine.SetWindowText("LON-PHILH01");
	m_Port.SetWindowText("701");

	hThreadHandle = 0;

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
			if (pSocket->Send('X', szData, strlen(szData)) != ITS_OK)
				m_RecvList.AddString("Could not send.");
		}
	}
}

void CMyclientDlg::OnConnect() 
{
	bool IsConnected = false;

	pSocket = new CSTATSocket;
	if (pSocket)
	{
		CWaitCursor oWait;

		if (!m_ChkListen.GetCheck())
		{
			// get machine name and port number
			char szMachine[256];
			m_Machine.GetWindowText(szMachine, 251);
			strcat(szMachine, ":");
			m_Port.GetWindowText(szMachine + strlen(szMachine), 4);

			if (pSocket->Connect(szMachine) == ITS_OK)
			{
				IsConnected = true;
				m_RecvList.AddString("Connected.");
			}
			else
				m_RecvList.AddString("Could not connect.");
		}
		else
		{
			int ret = 0;
			char szPort[4];
			m_Port.GetWindowText(szPort, 4);
			if ((ret = pSocket->Listen(szPort)) == ITS_OK)
			{
				m_RecvList.AddString("Listening for connection...");
				IsConnected = true;
			}
			else
				m_RecvList.AddString("Could not connect.");
		}

		if (IsConnected)
		{
			m_SendMsg.EnableWindow(TRUE);
			m_Send.EnableWindow(TRUE);
			m_Connect.EnableWindow(FALSE);
			m_ChkListen.EnableWindow(FALSE);
			m_Machine.EnableWindow(FALSE);
			m_Port.EnableWindow(FALSE);
			GetMessages();
		}
	}
	else
		m_RecvList.AddString("Memory error.");
}

// thread entry point
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	// lpParameter can be a pointer to a class to allow method calls
	CMyclientDlg *pDlg = (CMyclientDlg *)lpParameter;

	char ID = 0;
	unsigned long Length = 0;
	char *pData = NULL;
	char szBuffer[1024];

	while(1)
	{
		if (pDlg->pSocket->Receive(&ID, &pData, &Length) == ITS_OK)
		{
			memset(szBuffer, 0, 1024);
			sprintf(szBuffer, "%c %ld %s", ID, Length, pData);

			pDlg->m_RecvList.AddString(szBuffer);
			pDlg->UpdateWindow();
		}

		Sleep(100);
	}

	return 1;
}

void CMyclientDlg::GetMessages() 
{
	DWORD dwThreadID;
	if (!(hThreadHandle = CreateThread( NULL,				// security attributes
											0,				// stack size
											ThreadProc,		// proc to call
											this,		// proc parameter
											0,				// creation flags
											&dwThreadID)))	// thread identifier
	{
		AfxMessageBox("Could not create thread");
	}
}

void CMyclientDlg::OnOK() 
{
	if (hThreadHandle)
		TerminateThread(hThreadHandle, -1);	

	CDialog::OnOK();
}
