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




// testcomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testcom.h"
#include "testcomDlg.h"
#include <statcommon.h>
#include <statcomms.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD WINAPI ThreadProcOut(LPVOID lpParameter);
void ProcessInput(STATComms *pComms, CTestcomDlg *pDlg);

bool bFinished = true;
bool bReset = true;
char szBitmapFile[] = "C:\\basicscreen.mbm";
char szReceiveFile[] = "C:\\basicapp.exe";

/////////////////////////////////////////////////////////////////////////////
// CTestcomDlg dialog

CTestcomDlg::CTestcomDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestcomDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestcomDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTestcomDlg::~CTestcomDlg()
{
  	if (hThread2Out)
  		CloseHandle(hThread2Out);
}

void CTestcomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestcomDlg)
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_TRANSPORT, m_Transport);
	DDX_Control(pDX, IDC_LIST1, m_List1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestcomDlg, CDialog)
	//{{AFX_MSG_MAP(CTestcomDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TRANSFER, OnTransfer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestcomDlg message handlers

BOOL CTestcomDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// defaults
	m_Transport.SetCurSel(1);
	m_Port.SetWindowText("3000");
	m_Port.SetLimitText(25);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestcomDlg::OnPaint() 
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

HCURSOR CTestcomDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


///////////////////////////////////////////////////////////////////////////////////
// Start it all
///////////////////////////////////////////////////////////////////////////////////
void CTestcomDlg::OnTransfer() 
{
	if (bFinished)
	{
		bReset = true;
		DWORD dwThreadID2Out;
		if (!(hThread2Out = CreateThread( NULL,				// security attributes
												0,				// stack size
												ThreadProcOut,		// proc to call
												this,			// proc parameter
												0,				// creation flags
												&dwThreadID2Out)))	// thread identifier
		{
			AfxMessageBox("Could not create listening thread");
		}
	}
	else
	{
		bFinished = true;	// signal thread to stop
		AddString("Stopping...");
		Sleep(250);			// ...and wait for it
	}
}


///////////////////////////////////////////////////////////////////////////////////
// Thread entry point
///////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI ThreadProcOut(LPVOID lpParameter)
{
	CTestcomDlg *pDlg = (CTestcomDlg *)lpParameter;

	STATComms *pComms;
	pComms = new STATComms();
	if (pComms)
	{
		pComms->SetTransport((STATCONNECTTYPE)(pDlg->m_Transport.GetCurSel() + 1));
		if (pComms->Initialise() == ITS_OK)
		{
			char szPort[25];
			pDlg->m_Port.GetWindowText(szPort, 25);

			while (bReset)
			{
				pDlg->AddString("Listening...");
				if (pComms->Connect(szPort) == ITS_OK)
				{
					bFinished = false;
					bReset = false;
					pDlg->AddString("Connected.");
					pDlg->AddString(szPort);

					ProcessInput(pComms, pDlg);

					pComms->Disconnect();
				}
				else
				{
					pDlg->AddString("Could not connect.");
					pDlg->AddString(szPort);
				}

				pDlg->AddString("Stopped.");
			}

			pComms->Release();
		}
		else
		{
			pDlg->AddString("Could not initialise.");
		}
	}
	else
		pDlg->AddString("Out of memory.");

	return 1;
}


///////////////////////////////////////////////////////////////////////////////////
// Listen on the port
///////////////////////////////////////////////////////////////////////////////////
void
ProcessInput(STATComms *pComms, CTestcomDlg *pDlg)
{
	int ret = GENERAL_FAILURE;
	char szBuf[512] = {0};
	char cIdentifier;
	char *pExtra = NULL;
	unsigned long ulReadLength = 0;

	while (!bFinished)
	{
		if ((ret = pComms->Receive(&cIdentifier, &pExtra, &ulReadLength)) == ITS_OK)
		{
			// display what we received
			if (ulReadLength && ulReadLength < MAX_LOG_MSG_LEN)
			{
				char szContents[MAX_LOG_MSG_LEN + 1];
				strncpy(szContents, pExtra, ulReadLength);
				*(szContents + ulReadLength) = (char)0;
				sprintf(szBuf, "%c   [%s]   (%ld)", cIdentifier, szContents, ulReadLength);
			}
			else
				sprintf(szBuf, "%c   (%ld)", cIdentifier, ulReadLength);
			pDlg->AddString(szBuf);

			// special behaviour
			switch(cIdentifier)
			{
				case 'T':
				{
					pDlg->ReleaseData(&pExtra);
					ulReadLength = 0;
					break;
				}
				case 'S':
				{
					// read bitmap data
					pDlg->ReleaseData(&pExtra);
					ret = pDlg->ReadTransferFile(szBitmapFile, &pExtra, &ulReadLength);
					if (ret != ITS_OK)
					{
						pDlg->AddString("Read of bitmap failed!!!");
						pDlg->AddString(szBitmapFile);
					}
					break;
				}
				case 'R':
				case 'X':
				{
					// read binary
					pDlg->ReleaseData(&pExtra);
					ret = pDlg->ReadTransferFile(szReceiveFile, &pExtra, &ulReadLength);
					if (ret != ITS_OK)
					{
						pDlg->AddString("Read of application file failed!!!");
						pDlg->AddString(szReceiveFile);
					}
					break;
				}
			}

			// now send data back as a response to show we got it ok
			if (pComms->Send(cIdentifier, pExtra, ulReadLength) != ITS_OK)
				pDlg->AddString("Send failed!!!");

			// need to delete received data once finished with it
			pDlg->ReleaseData(&pExtra);
		}
		else
		{
			// restart the connection
			if (ret != NO_DATA_AT_PORT)
			{
				bReset = true;
				break;
			}
		}

		Sleep (250);
	}
}


///////////////////////////////////////////////////////////////////////////////////
// Release memory resources
///////////////////////////////////////////////////////////////////////////////////
void
CTestcomDlg::ReleaseData(char **ppData)
{
	if (*ppData)
	{
		delete [] (*ppData);
		(*ppData) = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////////
// Add a string to the list box
///////////////////////////////////////////////////////////////////////////////////
void
CTestcomDlg::AddString(char *szText)
{
	m_List1.AddString(szText);
	int nCount = m_List1.GetCount();
	if (nCount > 0)
		m_List1.SetTopIndex(nCount - 1);
	UpdateWindow();
}


///////////////////////////////////////////////////////////////////////////////////
// Read in a file
///////////////////////////////////////////////////////////////////////////////////
int CTestcomDlg::ReadTransferFile(const char *pFile, char **ppContents, unsigned long *pLength)
{
	int ret = E_BADFILENAME;
	CString path = pFile;
	CFile script_file;
	if (script_file.Open(path, CFile::modeRead))
	{
		try
		{
			(*pLength) = script_file.GetLength();
			(*ppContents) = new char [*pLength];
			if (*ppContents)
			{
				script_file.Read(*ppContents, *pLength);
				ret = ITS_OK;
			}
			else
				ret = E_OUTOFMEM;
		}
		catch(CFileException *e)
		{
			e->Delete();
			ret = GENERAL_FAILURE;
		}

		script_file.Abort();
	}

	return ret;
}
