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




#include "CSTATMessageCOM.h"
#include <initguid.h>
#include "ECGUID.H"
#include <statcommon.h>

//-------------------------------------------------------------------------------
//MESSAGE MAP

BEGIN_MESSAGE_MAP(CSTATMessageCOM, CCmdTarget)
	//{{AFX_MSG_MAP(CSTATMessageCOM)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CSTATMessageCOM, CCmdTarget)


//constructor
CSTATMessageCOM::CSTATMessageCOM()
: 	m_pISendReq(NULL), m_pICommonSink(NULL), m_pResponseData(NULL), m_pIECEngineS(NULL),
	m_pIECMachine(NULL), m_pIECDrive(NULL), Connected(false), bExternalInterface(false)
{}

//----------------------------------------------------------------------------
//destructor
CSTATMessageCOM::~CSTATMessageCOM()
{
	Disconnect();
}

//-------------------------------------------------------------------------------

BEGIN_INTERFACE_MAP(CSTATMessageCOM, CCmdTarget)
	INTERFACE_PART(CSTATMessageCOM, IID_ICommonSink, CommonSink)
END_INTERFACE_MAP()

//-------------------------------------------------------------------------------
// IUnknown Interface 

//memory management functions
STDMETHODIMP_(ULONG) CSTATMessageCOM::XCommonSink::AddRef()
{
	METHOD_PROLOGUE(CSTATMessageCOM, CommonSink)
	ASSERT(pThis);
 
	TRACE(_T("Info::AddRef %d\n") ,pThis -> m_dwRef);
	return pThis -> ExternalAddRef(); 
}

STDMETHODIMP_(ULONG) CSTATMessageCOM::XCommonSink::Release()
{
	METHOD_PROLOGUE(CSTATMessageCOM, CommonSink)
	ASSERT(pThis);

	TRACE(_T("Info::Release %d\n"),pThis -> m_dwRef);
	return pThis -> ExternalRelease();
}

STDMETHODIMP CSTATMessageCOM::XCommonSink::QueryInterface(REFIID riid, void** ppv)
{
	METHOD_PROLOGUE(CSTATMessageCOM, CommonSink);
	TRACE(_T("CSTATMessageCOM::XPWBKInfoQueryInterface\n"));

	return pThis -> ExternalQueryInterface(&riid, ppv);
}
//-------------------------------------------------------------------------------
// ICommonSink Interface
STDMETHODIMP CSTATMessageCOM::XCommonSink::ProcessResponse(BYTE* Response, DWORD bufsize)
{
	METHOD_PROLOGUE(CSTATMessageCOM, CommonSink);
	ASSERT(pThis);

	pThis -> ProcessResponse(Response, bufsize);

	return S_OK;
}
//----------------------------------------------------------------------------
//Connection
int CSTATMessageCOM::Connect(CString& platformtype, CSTATLogFile *theLog)
{
	pLog = theLog;

	// convert to uppercase
	platformtype.MakeUpper();

	//select DLL to be initialised
	CString STATServer;
	if(platformtype == _T("ARM") || platformtype == _T("ARM4"))
		STATServer = _T("STATApiARM4.DLL");
	else if(platformtype == _T("WINS"))
		STATServer = _T("STATApiWINS.DLL");
	else if(platformtype == _T("THUMB"))
		STATServer = _T("STATApiTHUMB.DLL");
	else if(platformtype == _T("WINSCW"))
		STATServer = _T("STATApiWINSCW.DLL");
	else
		return E_BADPLATFORM;

	// disconnect if already connected
	Disconnect();

	// create an instance of the EPOC Connect Engine
	if (FAILED(CoCreateInstance(CLSID_ECEngineS, NULL, CLSCTX_INPROC_SERVER, IID_IECEngineS, (void **)&m_pIECEngineS))) 
	{
		Disconnect();
		return E_ENGINEOBJECTFAILURE;
	}
		
	// get ISendReq interface
	if (FAILED(ExternalQueryInterface(&IID_ICommonSink, (void**)&m_pICommonSink)))
	{
		Disconnect();
		return E_BADREQUESTINTERFACE;
	}
	
	IFileSystemInfoS *pFSInfo;
	LPVOID *ppFSInfo = (LPVOID *)&pFSInfo;

	// get an interface pointer to the FileSystemInfo struct
	if (!bExternalInterface)
	{
		ExternalQueryInterface(&IID_IFileSystemInfoS, ppFSInfo);
		bExternalInterface = true;
	}

	// initialise the EPOC Connect engine
	m_pIECEngineS -> Init(pFSInfo);
	if (m_pIECEngineS->Connect(15000) != S_OK) // connect, giving 15 seconds as maximum wait
	{
		Disconnect();
		return E_ENGINECONNECTFAILURE;
	}

	// create an instance of the EPOC send request
	if (FAILED(CoCreateInstance(CLSID_SendRequest, NULL, CLSCTX_INPROC_SERVER, IID_ISendRequest, (void**) &m_pISendReq)))
	{
		Disconnect();
		return E_REQUESTOBJECTFAILURE;
	}

	BSTR bSTATstrServer = STATServer.AllocSysString();	//converts from CString to BSTR

	//The 'Initialise' function starts up the generic custom server, copying it to the remote 
	//EPOC machine if it is not there. It then checks that the application-specific server DLL 
	//is loaded and up to date and, if not, copies it over to C:/system/libs. Finally, it 
	//checks that the generic custom server found the interface.
	if (FAILED(m_pISendReq -> Initialise(bSTATstrServer)))
	{
		Disconnect();
		SysFreeString(bSTATstrServer);
		return E_REQUESTINITFAILURE;
	}

	SysFreeString(bSTATstrServer);
	Connected = true;	//flagged
	return ITS_OK;
}

//-------------------------------------------------------------------------------
//Disconnection
bool CSTATMessageCOM::Disconnect()
{
	Connected = false;	//flagged as disconnected

	if (m_pIECEngineS)
	{
		m_pIECEngineS->Release();
		m_pIECEngineS = NULL;
	}

	// make sure tell EPOC side to close
	if (m_pISendReq)
	{
		m_pISendReq->Release();
		m_pISendReq = NULL;
	}

	if (bExternalInterface)
	{
		ExternalRelease();
		bExternalInterface = false;
	}

	if(m_pResponseData)
	{
		delete[] m_pResponseData;
		m_pResponseData = NULL;
	}
	
	return true;
}

//-------------------------------------------------------------------------------
//Send Message to device - no data
HRESULT CSTATMessageCOM::SendMsg(BYTE id)
{
	HRESULT hr = S_OK;

	for (int i=0;i<STAT_MAXRETRIES;i++)
	{
		if (SUCCEEDED(hr = m_pISendReq -> BuildNewRequest(1, id)))	//BuildNewRequest needed for creating header of message
			if (SUCCEEDED(hr = m_pISendReq -> SendRequest(m_pICommonSink)))
				return hr;

		pLog->Set(E_COMMANDRETRY);
		Sleep(STAT_RETRYDELAY);
	}

	pLog->Set(E_COMMANDFAILED);
	return hr;
}

//-------------------------------------------------------------------------------
//send message with data
HRESULT CSTATMessageCOM::SendMsg(BYTE id, VARIANT data)
{
	HRESULT hr = S_OK;
	
	for (int i=0;i<STAT_MAXRETRIES;i++)
	{
		//will send data that has already been created in calling function
		if (SUCCEEDED(hr = m_pISendReq -> BuildNewRequest(1, id)))
			if (SUCCEEDED(hr = m_pISendReq -> AddBody(data)))		//adds body to the header (newly converted data)
		{
			if (SUCCEEDED(hr = m_pISendReq -> SendRequest(m_pICommonSink)))
				return hr;
		}

		pLog->Set(E_COMMANDRETRY);
		Sleep(STAT_RETRYDELAY);
	}

	pLog->Set(E_COMMANDFAILED);
	return hr;
}

//-------------------------------------------------------------------------------
// Retrieves a response from EPOC device
STDMETHODIMP CSTATMessageCOM::ProcessResponse(BYTE* pResponse, DWORD bufsize)
{
	CopyResponseData(pResponse + 11, bufsize - 11);	//function called, removing the first 11 bytes (header info)
	return S_OK;
}

//-------------------------------------------------------------------------------
//function used to store newly formatted data into new variable
void CSTATMessageCOM::CopyResponseData(BYTE* pResponse, DWORD bufsize)
{
	if(m_pResponseData)
	{
		delete[] m_pResponseData;
		m_pResponseData = NULL;
	}
	
	if(bufsize)
	{
		m_pResponseData = new BYTE[bufsize];
		memcpy(m_pResponseData, pResponse, bufsize);
	}
}

//-----------------------------------------------------------------------------
