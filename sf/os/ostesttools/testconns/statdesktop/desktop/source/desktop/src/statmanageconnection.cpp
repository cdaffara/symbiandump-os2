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





#include "stdafx.h"
#include "statdesktop.h"
#include "STATManageConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// global var to hold our selection when we exit the dialog
CString cConnectionInfo;

/////////////////////////////////////////////////////////////////////////////
// STATManageConnection dialog


STATManageConnection::STATManageConnection(CWnd* pParent /*=NULL*/)
	: CDialog(STATManageConnection::IDD, pParent)
{
	//{{AFX_DATA_INIT(STATManageConnection)
	//}}AFX_DATA_INIT
}


void STATManageConnection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(STATManageConnection)
	DDX_Control(pDX, IDC_ADDRESS, m_Address);
	DDX_Control(pDX, IDC_TRANSPORT, m_Transport);
	DDX_Control(pDX, IDC_LISTCONNECTIONS, m_ListConnections);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(STATManageConnection, CDialog)
	//{{AFX_MSG_MAP(STATManageConnection)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_LBN_SELCHANGE(IDC_LISTCONNECTIONS, OnSelchangeListconnections)
	ON_CBN_SELCHANGE(IDC_TRANSPORT, OnSelchangeTransport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// STATManageConnection message handlers

BOOL STATManageConnection::OnInitDialog()
{
	CDialog::OnInitDialog();

	cConnectionInfo.ReleaseBuffer();
	m_Transport.SetCurSel(2);
	statIniFile.SetIniFileName(STAT_INI_NAME);
		CString setting;
//	lCount = 0;
	if(statIniFile.SectionExists(ST_TEST_KEY) )
	{

		lCount = 0;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_CONNECTIONIDX,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lCount = _ttol(setting);
			m_ListConnections.SetCurSel(lCount);
			OnSelchangeListconnections();
		}
	}

	if(statIniFile.SectionExists(ST_CONNECTION_LIST) )
	{
		for( int i=0;i<lCount;++i)
		{
			TCHAR buf[6];
			_ltot(i+1, buf, 10);
			setting.Empty();
			setting=statIniFile.GetKeyValue(buf,ST_CONNECTION_LIST);
			if(!setting.IsEmpty())
				m_ListConnections.AddString(setting);
		}
	}
	return TRUE;
}


void STATManageConnection::OnAdd() 
{
	CString transport;
	CString address;
	switch(m_Transport.GetCurSel())
	{
	case 0:	// socket
		transport = "SymbianSocket:";
		break;
	case 1:	// serial
		transport = "SymbianSerial:";
		break;
	case 2:	// infra-red
		transport = "SymbianInfrared:";
		break;
	case 3:	// bluetooth
		transport = "SymbianBluetooth:";
		break;
	case 4:	// usb

		//break;
	default:
		return;
	}

	m_Address.GetWindowText(address);
	transport += address;

	if (!transport.IsEmpty() && !InList(transport))
	{
		m_ListConnections.AddString(transport);
		// highlight it in the list
		m_ListConnections.SetCurSel(m_ListConnections.FindStringExact(0, transport.GetBuffer(0)));
	}
	else
		AfxMessageBox(_T("This configuration already exists in the list."));
}

void STATManageConnection::OnRemove() 
{
	int index = m_ListConnections.GetCurSel();
	if (index != LB_ERR)
	{
		m_ListConnections.DeleteString(index);
		m_ListConnections.SetCurSel(0);
		OnSelchangeListconnections();
	}
}

void STATManageConnection::OnOK() 
{
	int index = m_ListConnections.GetCurSel();
	if (index != LB_ERR)
	{
		CString item;
		TCHAR buf[16];
		CWaitCursor oWait;
		m_ListConnections.GetText(index, cConnectionInfo);
		lCount=m_ListConnections.GetCount();
		_ltot(lCount, buf, 10);
		statIniFile.WriteKey(buf,ST_CONNECTIONIDX,ST_TEST_KEY);
		//empty connection list
		statIniFile.DeleteSection(ST_CONNECTION_LIST);
		for( int i=0;i<lCount;++i)
		{
			_ltot(i+1, buf, 10);
			m_ListConnections.GetText(i, item);
			statIniFile.WriteKey(item,buf,ST_CONNECTION_LIST);
		}
		CDialog::OnOK();
	}
}

void STATManageConnection::OnCancel() 
{
	cConnectionInfo.ReleaseBuffer();
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// 
bool STATManageConnection::InList(CString & Entry)
{
	CString item;
	int iCount = m_ListConnections.GetCount();
	for (int i=0;i<iCount;i++)
	{
		m_ListConnections.GetText(i, item);

		if (item.Compare(Entry.GetBuffer(0)) == 0)
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Converts a char * to it's Unicode equivalent
//
LPTSTR STATManageConnection::ToUnicode(const char *string)
{
#ifdef UNICODE
	static TCHAR szBuffer[MAX_UNICODE_LEN + 1] = {0};
	szBuffer[0] = (TCHAR)0;

    // Convert to UNICODE.
    if (!MultiByteToWideChar(CP_ACP,					// conversion type
							 0,							// flags
							 string,					// source
							 -1,						// length
							 szBuffer,					// dest
							 MAX_UNICODE_LEN))			// length
    {
        return _T("Could not convert");
    }

    return szBuffer;
#else
	return (LPTSTR)string;
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
// Converts a Unicode to it's char * equivalent
//
char * STATManageConnection::ToAnsi(LPCTSTR string)
{
#ifdef UNICODE
	static char szBuffer[MAX_UNICODE_LEN + 1] = {0};
	szBuffer[0] = (char)0;

    // Convert to ANSI.
    if (!WideCharToMultiByte(CP_ACP,					// conversion type
							 0,							// flags
							 string,					// source
							 -1,						// length
							 szBuffer,					// dest
							 MAX_UNICODE_LEN,			// length
							 NULL,
							 NULL ))
    {
        return "Could not convert";
    }

    return szBuffer;
#else
	return (char *)string;
#endif
}


void STATManageConnection::OnSelchangeListconnections() 
{
	CString item;
	int index = m_ListConnections.GetCurSel();
	if (index != LB_ERR)
	{
		m_ListConnections.GetText(index, item);

		int i = item.Find(_T(':'));
		if (i != -1)
		{
			CString transport = item.Left(i);

			if (transport.CompareNoCase(_T("SymbianSocket")) == 0)
			{
				m_Transport.SetCurSel(SymbianSocket - 1);
			}
			else if (transport.CompareNoCase(_T("SymbianSerial")) == 0)
			{
				m_Transport.SetCurSel(SymbianSerial - 1);
			}
			else if (transport.CompareNoCase(_T("SymbianInfrared")) == 0)
			{
				m_Transport.SetCurSel(SymbianInfrared - 1);
			}
			else if (transport.CompareNoCase(_T("SymbianBluetooth")) == 0)
			{
				m_Transport.SetCurSel(SymbianBluetooth - 1);
			}
			else if (transport.CompareNoCase(_T("SymbianUsb")) == 0)
			{

			}

			m_Address.SetWindowText(item.Mid(i + 1));
		}
	}
}

void STATManageConnection::OnSelchangeTransport() 
{
	// If the user has clicked to a selection of the transport
	// list then we warn them if they selected 'USB'.

	CString item;
	int index = m_Transport.GetCurSel();
	if (index != LB_ERR)
	{
		m_Transport.GetLBText(index, item);

		if ((item.CompareNoCase(_T("SymbianInfrared")) == 0) ||
			(item.CompareNoCase(_T("SymbianBluetooth")) == 0))
		{
			// If the transport selection is infrared we show a 
			// warning that infrared is treated the same as 
			// serial and specify that the user should enter a 
			// port number in the address field.
			CString	symbian;
			CString	appName;
			CString	prompt;

			if( ( symbian.LoadString( IDS_SYMBIAN ) ) &&
				( appName.LoadString( IDS_APPNAME ) ) &&
				( prompt.LoadString( IDS_PORTPROMPT ) ) )
			{
				CString	msg;
				msg.Format( prompt, symbian, appName );

				::AfxMessageBox( msg );
			}
		}
	}
}
