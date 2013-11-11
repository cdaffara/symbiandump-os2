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

#include <memspy/api/memspyapiprocess.h>
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyprocessdata.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>


EXPORT_C CMemSpyApiProcess::~CMemSpyApiProcess()
	{
	delete iProcessData;
	}

EXPORT_C TProcessId CMemSpyApiProcess::Id() const
	{
	return iProcessData->iId;
	}

EXPORT_C const TDesC& CMemSpyApiProcess::Name() const
	{
	return iProcessData->iName;
	}

EXPORT_C TExitCategoryName CMemSpyApiProcess::ExitCategory() const
	{
	return iProcessData->iExitCategory;
	}
	
EXPORT_C TInt CMemSpyApiProcess::ExitReason() const
	{
	return iProcessData->iExitReason;
	}
	
EXPORT_C TExitType CMemSpyApiProcess::ExitType() const
	{
	return iProcessData->iExitType;
	}
	
EXPORT_C TInt CMemSpyApiProcess::ThreadCount() const
	{
	return iProcessData->iThreadCount;
	}

EXPORT_C TUint32 CMemSpyApiProcess::SID() const
	{
	return iProcessData->iSID;
	}

EXPORT_C TUint32 CMemSpyApiProcess::VID() const
	{
	return iProcessData->iSID;
	}

EXPORT_C TProcessPriority CMemSpyApiProcess::Priority() const
	{
	return iProcessData->iPriority;
	}

EXPORT_C TBool CMemSpyApiProcess::IsDead() const
    {
    return iProcessData->iIsDead;
    }

CMemSpyApiProcess::CMemSpyApiProcess() : iProcessData(0)
	{
	}

void CMemSpyApiProcess::ConstructL(const TMemSpyProcessData& aData)
	{
	iProcessData = new (ELeave) TMemSpyProcessData(aData);
	}

CMemSpyApiProcess* CMemSpyApiProcess::NewL(const TMemSpyProcessData& aData)
	{
	CMemSpyApiProcess* self = CMemSpyApiProcess::NewLC(aData);
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiProcess* CMemSpyApiProcess::NewLC(const TMemSpyProcessData& aData)
	{
	CMemSpyApiProcess* self = new (ELeave) CMemSpyApiProcess;
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return (self);
	}

