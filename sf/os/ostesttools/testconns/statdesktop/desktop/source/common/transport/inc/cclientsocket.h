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




#ifndef STATCLIENTSOCKET_H
#define STATCLIENTSOCKET_H

#include <afxsock.h>

class CSTATSocket;

class CClientSocket : public CSocket
{
	DECLARE_DYNAMIC(CClientSocket);

	public:
		CClientSocket(CSTATSocket* pSoc);
		CSTATSocket* m_pSoc;

	protected:
		virtual void OnReceive(int nErrorCode);
		virtual void OnAccept(int nErrorCode);
};

#endif
