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

#include <memspy/api/memspyapithread.h>
#include <memspy/engine/memspythreaddata.h>


EXPORT_C CMemSpyApiThread::~CMemSpyApiThread()
	{
	delete iThreadData;
	}

EXPORT_C TThreadId CMemSpyApiThread::Id() const
	{
	return iThreadData->iId;
	}

EXPORT_C TProcessId CMemSpyApiThread::ProcessId() const
	{
	return iThreadData->iPid;
	}

EXPORT_C const TDesC& CMemSpyApiThread::Name() const
	{
	return iThreadData->iName;
	}

EXPORT_C TInt CMemSpyApiThread::SID() const
	{
	return iThreadData->iSID;
	}


EXPORT_C TInt CMemSpyApiThread::VID() const
	{
	return iThreadData->iVID;
	}

EXPORT_C TThreadPriority CMemSpyApiThread::ThreadPriority() const
	{
	return iThreadData->iThreadPriority;
	}

EXPORT_C TProcessPriority CMemSpyApiThread::ProcessPriority() const
	{
	return iThreadData->iProcessPriority;
	}

EXPORT_C TInt CMemSpyApiThread::RequestCount() const
	{
	return iThreadData->iRequestCount;
	}

EXPORT_C TInt CMemSpyApiThread::ThreadHandles() const
	{
	return iThreadData->iThreadHandles;
	}

EXPORT_C TInt CMemSpyApiThread::ProcessHandles() const
	{
	return iThreadData->iProcessHandles;
	}

EXPORT_C TInt CMemSpyApiThread::ThreadNumberUsing() const
	{
	return iThreadData->iThreadNumberUsing;
	}

EXPORT_C TInt CMemSpyApiThread::ProcessNumberUsing() const
	{
	return iThreadData->iProcessNumberUsing;
	}

EXPORT_C TInt CMemSpyApiThread::Attributes() const
	{
	return iThreadData->iAttributes;
	}

EXPORT_C TInt CMemSpyApiThread::CpuUse() const
	{
	return iThreadData->iCpuUse;
	}

EXPORT_C TExitType CMemSpyApiThread::ExitType() const
	{
	return iThreadData->iExitType;
	}

CMemSpyApiThread::CMemSpyApiThread() : iThreadData(0)
	{
	}

void CMemSpyApiThread::ConstructL(const TMemSpyThreadData& aData)
	{
	iThreadData = new (ELeave) TMemSpyThreadData(aData);
	}

CMemSpyApiThread* CMemSpyApiThread::NewL(const TMemSpyThreadData& aData)
	{
	CMemSpyApiThread* self = CMemSpyApiThread::NewLC(aData);
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiThread* CMemSpyApiThread::NewLC(const TMemSpyThreadData& aData)
	{
	CMemSpyApiThread* self = new (ELeave) CMemSpyApiThread;
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return (self);
	}
