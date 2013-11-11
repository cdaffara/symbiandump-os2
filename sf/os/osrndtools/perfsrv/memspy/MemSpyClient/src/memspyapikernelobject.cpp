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

#include <memspy/api/memspyapikernelobject.h>
#include <memspy/engine/memspykernelobjectdata.h>
//#include <memspy/engine/memspyengineobjectprocess.h>
//#include <memspy/engine/memspyengineobjectcontainer.h>


EXPORT_C CMemSpyApiKernelObject::~CMemSpyApiKernelObject()
	{
	delete iKernelObjectData;
	}


CMemSpyApiKernelObject::CMemSpyApiKernelObject() : iKernelObjectData(0)
	{
	}

CMemSpyApiKernelObject* CMemSpyApiKernelObject::NewL(const TMemSpyKernelObjectData& aData)
	{
	CMemSpyApiKernelObject* self = CMemSpyApiKernelObject::NewLC(aData);
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiKernelObject* CMemSpyApiKernelObject::NewLC(const TMemSpyKernelObjectData& aData)
	{
	CMemSpyApiKernelObject* self = new (ELeave) CMemSpyApiKernelObject;
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return (self);
	}

void CMemSpyApiKernelObject::ConstructL(const TMemSpyKernelObjectData& aData)
	{
	iKernelObjectData = new (ELeave) TMemSpyKernelObjectData(aData);
	}

EXPORT_C TMemSpyDriverContainerType CMemSpyApiKernelObject::Type() const
	{
	return iKernelObjectData->iType;
	}

EXPORT_C const TDesC& CMemSpyApiKernelObject::Name() const
	{
	return iKernelObjectData->iName;
	}
					
EXPORT_C TInt CMemSpyApiKernelObject::Count() const
	{
	return iKernelObjectData->iCount;
	}
	
EXPORT_C TInt64 CMemSpyApiKernelObject::Size() const
	{
	return iKernelObjectData->iSize;
	}
