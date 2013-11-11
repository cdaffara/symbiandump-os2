/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <caf/dirstreamable.h>

const TUint KCDirArrayGranularity=0x200;

using namespace ContentAccess;


EXPORT_C CDirStreamable* CDirStreamable::NewL(CDir &aDir)
	{
	CDirStreamable* self = new (ELeave) CDirStreamable;
	CleanupStack::PushL(self);
	self->ConstructL(aDir);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDirStreamable* CDirStreamable::NewL()
	{
	CDirStreamable* self = new (ELeave) CDirStreamable;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CDirStreamable* CDirStreamable::NewL(RReadStream aStream)
	{
	CDirStreamable* self = new (ELeave) CDirStreamable;
	CleanupStack::PushL(self);
	self->ConstructL();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}
		
CDirStreamable::CDirStreamable()
	{
	}


void CDirStreamable::ConstructL()
	{
	iArray=new(ELeave) CArrayPakFlat<TEntry>(KCDirArrayGranularity);
	}
	
void CDirStreamable::ConstructL(CDir& aDir)
	{
	iArray=new(ELeave) CArrayPakFlat<TEntry>(KCDirArrayGranularity);

	// copy entries to ourselves
	TInt i = 0;
	for(i = 0; i < aDir.Count(); i++)
		{
		TEntry entry = aDir[i];
		AddL(entry);
		}
	}

CDirStreamable::~CDirStreamable()
	{
	}
        
EXPORT_C void CDirStreamable::AddL(const TEntry &aEntry)
	{
	CDir::AddL(aEntry);
	}

EXPORT_C TInt CDirStreamable::Count() const
	{
	return CDir::Count();	
	}
	
EXPORT_C const TEntry& CDirStreamable::operator[](TInt aIndex) const
	{
	return CDir::operator [](aIndex);	
	}
	
EXPORT_C TInt CDirStreamable::Sort(TUint aEntrySortKey)
	{
	return CDir::Sort(aEntrySortKey);	
	}
        
EXPORT_C void CDirStreamable::ExternalizeL(RWriteStream& aStream) const
	{
	TInt i = 0;
	TInt count = Count();
	aStream.WriteInt32L(count);
	for(i = 0; i < count; i++)
		{
		TEntry aEntry = (*this)[i]; 
		TPckgC <TEntry> pckg(aEntry);
		aStream.WriteL(pckg);	
		}
	}

void CDirStreamable::InternalizeL(RReadStream& aStream)
	{
	TInt i = 0;
	TInt count = aStream.ReadInt32L();
	for(i = 0; i < count; i++)
		{
		TEntry aEntry;
		TPckg<TEntry> pckg(aEntry);
		aStream.ReadL(pckg);
		
		// add to CDir base class array
		AddL(aEntry);
		}
	}
