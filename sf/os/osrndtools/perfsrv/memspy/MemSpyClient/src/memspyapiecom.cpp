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
#include <memspy/api/memspyapiecom.h>
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyecomdata.h>


EXPORT_C CMemSpyApiEComCategory::~CMemSpyApiEComCategory()
	{
	delete iData;
	}

EXPORT_C TUid CMemSpyApiEComCategory::Id() const
	{
	return iData->iId;
	}

EXPORT_C const TDesC& CMemSpyApiEComCategory::Name() const
	{
	return iData->iName;
	}

EXPORT_C TInt CMemSpyApiEComCategory::InterfaceCount() const
    {
    return iData->iInterfaceCount;
    }

void CMemSpyApiEComCategory::ConstructL(const TMemSpyEComCategoryData& aData)
	{
	iData = new (ELeave) TMemSpyEComCategoryData( aData );
	}

CMemSpyApiEComCategory* CMemSpyApiEComCategory::NewL(const TMemSpyEComCategoryData& aData)
	{
    CMemSpyApiEComCategory* self = CMemSpyApiEComCategory::NewLC( aData );
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiEComCategory* CMemSpyApiEComCategory::NewLC(const TMemSpyEComCategoryData& aData)
	{
    CMemSpyApiEComCategory* self = new (ELeave) CMemSpyApiEComCategory;
	CleanupStack::PushL( self );
	self->ConstructL( aData );
	return ( self );
	}








EXPORT_C CMemSpyApiEComInterface::~CMemSpyApiEComInterface()
    {
    delete iData;
    }

EXPORT_C TUid CMemSpyApiEComInterface::Id() const
    {
    return iData->iId;
    }

EXPORT_C const TDesC& CMemSpyApiEComInterface::Name() const
    {
    return iData->iName;
    }

EXPORT_C TInt CMemSpyApiEComInterface::ImplementationCount() const
    {
    return iData->iImplementationCount;
    }

void CMemSpyApiEComInterface::ConstructL(const TMemSpyEComInterfaceData& aData)
    {
    iData = new (ELeave) TMemSpyEComInterfaceData( aData );
    }

CMemSpyApiEComInterface* CMemSpyApiEComInterface::NewL(const TMemSpyEComInterfaceData& aData)
    {
    CMemSpyApiEComInterface* self = CMemSpyApiEComInterface::NewLC( aData );
    CleanupStack::Pop(self);
    return (self);
    }

CMemSpyApiEComInterface* CMemSpyApiEComInterface::NewLC(const TMemSpyEComInterfaceData& aData)
    {
    CMemSpyApiEComInterface* self = new (ELeave) CMemSpyApiEComInterface;
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    return ( self );
    }







EXPORT_C CMemSpyApiEComImplementation::~CMemSpyApiEComImplementation()
    {
    delete iData;
    }

EXPORT_C TUid CMemSpyApiEComImplementation::ImplementationUid() const
    {
    return iData->iImplementationUid;
    }
    
EXPORT_C const TDesC& CMemSpyApiEComImplementation::Name() const
    {
    return iData->iName;
    }
    
EXPORT_C TInt CMemSpyApiEComImplementation::Version() const
    {
    return iData->iVersion;
    }
    
EXPORT_C const TDesC& CMemSpyApiEComImplementation::DataType() const
    {
    return iData->iDataType;
    }
        
EXPORT_C const TDesC& CMemSpyApiEComImplementation::OpaqueData() const
    {
    return iData->iOpaqueData;
    }
    
EXPORT_C TDriveUnit CMemSpyApiEComImplementation::Drive() const
    {
    return iData->iDrive;
    }
        
EXPORT_C TBool CMemSpyApiEComImplementation::RomOnly() const
    {
    return iData->iRomOnly;
    }
        
EXPORT_C TBool CMemSpyApiEComImplementation::RomBased() const
    {
    return iData->iRomBased;
    }
        
EXPORT_C TVendorId CMemSpyApiEComImplementation::VendorId() const
    {
    return iData->iVendorId;
    }
        
EXPORT_C TBool CMemSpyApiEComImplementation::Disabled() const
    {
    return iData->iDisabled;
    }

void CMemSpyApiEComImplementation::ConstructL(const TMemSpyEComImplementationData& aData)
    {
    iData = new (ELeave) TMemSpyEComImplementationData( aData );
    }

CMemSpyApiEComImplementation* CMemSpyApiEComImplementation::NewL(const TMemSpyEComImplementationData& aData)
    {
    CMemSpyApiEComImplementation* self = CMemSpyApiEComImplementation::NewLC( aData );
    CleanupStack::Pop(self);
    return (self);
    }

CMemSpyApiEComImplementation* CMemSpyApiEComImplementation::NewLC(const TMemSpyEComImplementationData& aData)
    {
    CMemSpyApiEComImplementation* self = new (ELeave) CMemSpyApiEComImplementation;
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    return ( self );
    }
