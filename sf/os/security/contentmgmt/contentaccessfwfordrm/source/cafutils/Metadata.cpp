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


#include <charconv.h>
#include <s32strm.h>
#include <caf/metadata.h>
#include "cafutils.h"


using namespace ContentAccess;

CMetaData* CMetaData::NewL(const TDesC8& aField, const TDesC8& aData)
	{
	CMetaData* self = new (ELeave) CMetaData(EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aField, aData);
	CleanupStack::Pop(self);
	return self;
	}

CMetaData* CMetaData::NewL(const TDesC16& aField, const TDesC16& aData)
	{
	CMetaData* self = new (ELeave) CMetaData(ETrue);
	CleanupStack::PushL(self);
	self->ConstructL(aField, aData);
	CleanupStack::Pop(self);
	return self;
	}

CMetaData* CMetaData::NewL(RReadStream& aStream)
	{
	CMetaData* self = new (ELeave) CMetaData(ETrue);
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

CMetaData::CMetaData(TBool aUnicode) : iUnicode(aUnicode)
	{
	}

void CMetaData::ConstructL(const TDesC8& aField, const TDesC8& aData)
	{
	// Store supplied data in both 8 bit and unicode forms
	iField8 = aField.AllocL();
	iData8 = aData.AllocL();

	// Allocate space for 16 bit versions
	iField = HBufC::NewL(iField8->Length());
	iData = HBufC::NewL(iData8->Length());

	// populate 16 bit versions with 8 bit values
	iField->Des().Copy(*iField8);
	iData->Des().Copy(*iData8);
	}

void CMetaData::ConstructL(const TDesC16& aField, const TDesC16& aData)
	{
	// Store supplied data in both 8 bit and unicode forms
	iField = aField.AllocL();
	iData = aData.AllocL();

	// Allocate space for 8 bit versions
	iField8 = HBufC8::NewL(iField->Des().Length());
	iData8 = HBufC8::NewL(iData->Des().Length());

	// populate 8 bit versions with 16 bit values
	iField8->Des().Copy(*iField);
	iData8->Des().Copy(*iData);
	}

CMetaData::~CMetaData()
	{
	delete iField;
	delete iData;
	delete iField8;
	delete iData8;
	}

EXPORT_C const TDesC& CMetaData::Field() const
	{
	return *iField;
	}
		
EXPORT_C const TDesC& CMetaData::Data() const
	{
	return *iData;
	}

EXPORT_C const TDesC8& CMetaData::Field8() const
	{
	return *iField8;
	}
		
EXPORT_C const TDesC8& CMetaData::Data8() const
	{
	return *iData8;
	}
	
void CMetaData::InternalizeL(RReadStream& aStream)
	{
	// Read the unicode field from the stream
	iField = TCafUtils::ReadDescriptor16L(aStream);

	// Read the unicode data from the stream
	iData = TCafUtils::ReadDescriptor16L(aStream);

	// Allocate space for 8 bit versions
	iField8 = HBufC8::NewL(iField->Length());
	iData8 = HBufC8::NewL(iData->Length());

	// populate 8 bit versions with 16 bit values
	iField8->Des().Copy(*iField);
	iData8->Des().Copy(*iData);
	}

void CMetaData::ExternalizeL(RWriteStream& aStream) const
	{
	// Write unicode field to the stream
	TCafUtils::WriteDescriptor16L(aStream, *iField);
	TCafUtils::WriteDescriptor16L(aStream, *iData);
	}
