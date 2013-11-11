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





#ifndef STATSOCKET_H
#define STATSOCKET_H

#include "statcommon.h"
#include "stattransport.h"
#include "CClientSocket.h"

class CSTATSocket : public CSTATTransport
{
public:
	CSTATSocket();
	~CSTATSocket();
	int Initialise(void);
	int Connect(const char *pAddress = NULL);
	int Send(const char cIdentifier, const char *pCommand = NULL, const unsigned long ulLength = 0);
	int Receive(char *cIdentifier, char **ppData = NULL, unsigned long *pLength = NULL);
	int Disconnect(void);
	int Release(void);

	int Listen(const char *pPort);
	int ReceiveData();

private:

	CClientSocket* m_pSocket;
	CClientSocket* m_pListen;
	CSocketFile* m_pFile;
	CArchive* m_pArchiveIn;
	CArchive* m_pArchiveOut;

	char ID;
	char *pData;
	unsigned long Length;
	bool IsRecvData;
};

#endif
