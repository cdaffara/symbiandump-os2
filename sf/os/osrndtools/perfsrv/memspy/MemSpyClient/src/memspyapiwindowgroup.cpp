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

#include <f32file.h>
#include <memspy/api/memspyapiwindowgroup.h>
#include <memspy/engine/memspyenginehelperwindowserver.h>


EXPORT_C CMemSpyApiWindowGroup::~CMemSpyApiWindowGroup()
	{
	delete iData;
	}

EXPORT_C TInt CMemSpyApiWindowGroup::Id() const
	{
	return iData->iId;
	}

EXPORT_C const TDesC& CMemSpyApiWindowGroup::Name() const
	{
	return iData->iName;
	}

EXPORT_C const TDesC& CMemSpyApiWindowGroup::FullName() const
    {
    return iData->iFullName;
    }

EXPORT_C TThreadId CMemSpyApiWindowGroup::ThreadId() const
    {
    return iData->iThreadId;
    }

EXPORT_C TInt CMemSpyApiWindowGroup::OrdinalPosition() const
    {
    return iData->iOrdinalPosition;
    }

EXPORT_C TInt CMemSpyApiWindowGroup::Priority() const
    {
    return iData->iPriority;
    }

EXPORT_C TInt CMemSpyApiWindowGroup::WindowGroupHandle() const
    {
    return iData->iWindowGroupHandle;
    }

EXPORT_C TUid CMemSpyApiWindowGroup::Uid() const
    {
    return iData->iUID;
    }

EXPORT_C TBool CMemSpyApiWindowGroup::IsBusy() const
    {
    return iData->iIsBusy;
    }

EXPORT_C TBool CMemSpyApiWindowGroup::IsSystem() const
    {
    return iData->iIsSystem;
    }

EXPORT_C TBool CMemSpyApiWindowGroup::IsHidden() const
    {
    return iData->iIsHidden;
    }

EXPORT_C const TDesC& CMemSpyApiWindowGroup::Caption() const
    {
    return iData->iCaption;
    }

EXPORT_C const TDesC& CMemSpyApiWindowGroup::DocName() const
    {
    return iData->iDocName;
    }

EXPORT_C TBool CMemSpyApiWindowGroup::IsFocused() const
    {
    return iData->iIsFocused;
    }

void CMemSpyApiWindowGroup::ConstructL(const TMemSpyEngineWindowGroupDetails& aData)
	{
	iData = new (ELeave) TMemSpyEngineWindowGroupDetails( aData );
	}

CMemSpyApiWindowGroup* CMemSpyApiWindowGroup::NewL(const TMemSpyEngineWindowGroupDetails& aData)
	{
    CMemSpyApiWindowGroup* self = CMemSpyApiWindowGroup::NewLC( aData );
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiWindowGroup* CMemSpyApiWindowGroup::NewLC(const TMemSpyEngineWindowGroupDetails& aData)
	{
    CMemSpyApiWindowGroup* self = new (ELeave) CMemSpyApiWindowGroup;
	CleanupStack::PushL( self );
	self->ConstructL( aData );
	return ( self );
	}
