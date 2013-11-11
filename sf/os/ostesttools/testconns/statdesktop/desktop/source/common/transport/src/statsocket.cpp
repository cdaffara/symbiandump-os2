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
#include "statsocket.h"

////////////////////////////////////////////////////////////////////////////////////////
// 
CSTATSocket::CSTATSocket()
: m_pSocket((CClientSocket*)NULL),
  m_pFile((CSocketFile*)NULL),
  m_pArchiveIn((CArchive*)NULL),
  m_pArchiveOut((CArchive*)NULL),
  m_pListen((CClientSocket*)NULL),
  IsRecvData(false),
  ID(0), Length(0), pData(NULL)
{
}


////////////////////////////////////////////////////////////////////////////////////////
// 
CSTATSocket::~CSTATSocket()
{
	Disconnect();
	Release();
}


////////////////////////////////////////////////////////////////////////////////////////
// 
int CSTATSocket::Initialise(void)
{
	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// 
int CSTATSocket::Connect(const char *pAddress)
{
	m_pSocket = new CClientSocket(this);

	if (m_pSocket)
	{
		// if address supplied then we are attempting a connection
		// else we are listening for a connection
		if (pAddress && (*pAddress))
		{
			if (!m_pSocket->Create())
			{
				Disconnect();
				return GENERAL_FAILURE;
			}

			char *p = strrchr(pAddress, ':');
			int iPort = 700;
			if (p)
			{
				(*p) = (char)0;

				p++;
				if (p && *p)
					iPort = atoi(p);
			}
			int iRetry = 0;
			while (!m_pSocket->Connect(pAddress, iPort))
			{
				iRetry++;
				if (iRetry > 5)
				{
					Disconnect();
					return E_TOOMANYERRORS;
				}

				Sleep(250);
			}
		}
		else
		{
			if (!m_pListen->Accept(*m_pSocket))
			{
				Disconnect();
				return E_CONNECTIONFAILED;
			}

			// don't need this object anymore
			BYTE Buffer[50];
			m_pListen->ShutDown();

			while(m_pListen->Receive(Buffer,50) > 0);

			delete m_pListen;
			m_pListen = NULL;
		}

		m_pFile = new CSocketFile(m_pSocket);
		m_pArchiveIn = new CArchive(m_pFile,CArchive::load);
		m_pArchiveOut = new CArchive(m_pFile,CArchive::store);

		// send acknowledgement message
		Send('A', "Connected", 9);
	}
	else
		return E_OUTOFMEM;

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// 
int CSTATSocket::Listen(const char *pPort)
{
	m_pListen = new CClientSocket(this);

	if (m_pListen)
	{
		if (!m_pListen->Create(atoi(pPort)))
			return E_CONNECTIONFAILED;

		if (!m_pListen->Listen())
			return E_CONNECTIONFAILED;
	}
	else
		return E_OUTOFMEM;

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// 
int CSTATSocket::Send(const char cIdentifier, const char *pData, const unsigned long ulLength)
{
	if (m_pArchiveOut != NULL)
	{
		TRY
		{
			(*m_pArchiveOut) << cIdentifier;
			(*m_pArchiveOut) << ulLength;

			if (ulLength)
				m_pArchiveOut->Write(pData, ulLength);

			m_pArchiveOut->Flush();
		}
		CATCH(CFileException, e)
		{
			Disconnect();
			return E_WRITEFAILED;
		}
		END_CATCH
	}

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// External: passed received data back to calling function
int CSTATSocket::Receive(char *cIdentifier, char **ppData, unsigned long *pLength)
{
	int ret = NO_DATA_AT_PORT;

	if (IsRecvData)
	{
		(*cIdentifier) = ID;
		(*pLength) = Length;
		(*ppData) = pData;

		IsRecvData = false;
		ret = ITS_OK;
	}

	return ret;
}
	
	
////////////////////////////////////////////////////////////////////////////////////////
// Internal: Receives data from the socket
int CSTATSocket::ReceiveData()
{
	if (!IsRecvData)
	{
		unsigned long ActualRead = 0;

		(*m_pArchiveIn) >> ID;
		(*m_pArchiveIn) >> Length;

		if(pData)
		{
			delete [] pData;
		}

		if (Length)
		{
			pData = new char [Length + 1];
			if (pData)
			{
				ActualRead = m_pArchiveIn->Read(pData, Length);
				*(pData + Length) = (char)0;
			}
			else
				Length = 0;
		}

		IsRecvData = true;
		return true;
	}

	return false;
}


////////////////////////////////////////////////////////////////////////////////////////
// 
int CSTATSocket::Disconnect(void)
{
	if (m_pArchiveOut)
	{
		delete m_pArchiveOut;
		m_pArchiveOut = NULL;
	}

	if (m_pArchiveIn)
	{
		delete m_pArchiveIn;
		m_pArchiveIn = NULL;
	}

	if (m_pFile)
	{
		delete m_pFile;
		m_pFile = NULL;
	}

	if (m_pSocket)
	{
		BYTE Buffer[50];
		m_pSocket->ShutDown();

		while(m_pSocket->Receive(Buffer,50) > 0);

		delete m_pSocket;
		m_pSocket = NULL;
	}

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// 
int CSTATSocket::Release(void)
{
	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
//	PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////
