/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <memspy/api/memspyapiserver.h>
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyserverdata.h>

EXPORT_C CMemSpyApiServer::~CMemSpyApiServer()
	{
	delete iData;
	}

EXPORT_C TProcessId CMemSpyApiServer::ProcessId() const
	{
	return iData->iProcessId;
	}

EXPORT_C TThreadId CMemSpyApiServer::ThreadId() const
    {
    return iData->iThreadId;
    }

EXPORT_C const TDesC& CMemSpyApiServer::Name() const
	{
	return iData->iName;
	}
	
EXPORT_C TInt CMemSpyApiServer::SessionCount() const
	{
	return iData->iSessionCount;
	}

CMemSpyApiServer::CMemSpyApiServer() : iData(0)
	{
	}

void CMemSpyApiServer::ConstructL(const TMemSpyServerData& aData)
	{
	iData = new (ELeave) TMemSpyServerData(aData);
	}

CMemSpyApiServer* CMemSpyApiServer::NewL(const TMemSpyServerData& aData)
	{
    CMemSpyApiServer* self = CMemSpyApiServer::NewLC(aData);
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiServer* CMemSpyApiServer::NewLC(const TMemSpyServerData& aData)
	{
    CMemSpyApiServer* self = new (ELeave) CMemSpyApiServer;
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return (self);
	}

