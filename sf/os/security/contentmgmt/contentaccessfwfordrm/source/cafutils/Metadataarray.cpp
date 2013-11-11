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


#include <s32strm.h>
#include <caf/metadataarray.h>
#include <caf/metadata.h>

using namespace ContentAccess;

EXPORT_C CMetaDataArray* CMetaDataArray::NewL()
	{
	CMetaDataArray* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CMetaDataArray* CMetaDataArray::NewLC()
	{
	CMetaDataArray* self = new (ELeave) CMetaDataArray;
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CMetaDataArray* CMetaDataArray::NewL(RReadStream& aStream)
	{
	CMetaDataArray* self = new (ELeave) CMetaDataArray;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

CMetaDataArray::CMetaDataArray()
	{
	iMaxFieldLength = 0;
	}	

		
CMetaDataArray::~CMetaDataArray()
	{	
	iArray.ResetAndDestroy();
	}

EXPORT_C void CMetaDataArray::AddL(const TDesC8& aField, const TDesC8& aData)
	{
	// Construct a new CMetaData object with 8 bit data
	CMetaData* ptr = CMetaData::NewL(aField, aData);
	CleanupStack::PushL(ptr);
	
	// Add it to the array
	User::LeaveIfError(iArray.Append(ptr));

	// Array owns the pointer so we don't need it in the cleanup stack any longer
	CleanupStack::Pop(ptr);

	// See if this field is longer than any previous field
	if(aField.Length() > iMaxFieldLength)
		{
		iMaxFieldLength = aField.Length();
		}
	}

EXPORT_C void CMetaDataArray::AddL(const TDesC& aField, const TDesC& aData)
	{
	// Construct a new CMetaData object with unicode data
	CMetaData* ptr = CMetaData::NewL(aField, aData);
	CleanupStack::PushL(ptr);

	// Add it to the array
	User::LeaveIfError(iArray.Append(ptr));
	
	// Array owns the pointer so we don't need it in the cleanup stack any longer
	CleanupStack::Pop(ptr);

	// See if this field is longer than any previous field
	if(aField.Length() > iMaxFieldLength)
		{
		iMaxFieldLength = aField.Length();
		}
	}

EXPORT_C const CMetaData& CMetaDataArray::operator [] (TInt aIndex) const
	{
	return *iArray[aIndex];
	}

EXPORT_C TInt CMetaDataArray::Count() const
	{
	return iArray.Count();
	}

EXPORT_C const TDesC& CMetaDataArray::SearchL(const TDesC& aField, TBool aMatchCase) const
	{
	TInt i;
	if(!aMatchCase)
		{
		// Allocate space for upper case version of field to search for
		HBufC* searchField = aField.AllocLC();
		TPtr searchFieldPtr = searchField->Des();
		searchFieldPtr.UpperCase();

		// Allocate space for upper case version fields in the array
		HBufC* field = HBufC::NewLC(iMaxFieldLength);
		TPtr fieldPtr = field->Des();

		// search through the array to find the data matching the given field
		for(i = 0; i < iArray.Count(); i++)
			{
			fieldPtr.Copy(iArray[i]->Field());
			fieldPtr.UpperCase();
			if(fieldPtr == searchFieldPtr)
				{
				CleanupStack::PopAndDestroy(2, searchField); // searchField, field
				return iArray[i]->Data();
				}
			}
		CleanupStack::PopAndDestroy(2, searchField); // searchField, field
		}
	else	
		{
		// search through the array to find the data matching the given field
		for(i = 0; i < iArray.Count(); i++)
			{
			if(iArray[i]->Field() == aField)
				{
				return iArray[i]->Data();
				}
			}
		}
	// not found so return an empty string
	return KNullDesC16();
	}
		
EXPORT_C const TDesC8& CMetaDataArray::SearchL(const TDesC8& aField8, TBool aMatchCase) const
	{
	TInt i;

	if(!aMatchCase)
		{
		// Allocate space for upper case version of field to search for
		HBufC8* searchField = aField8.AllocLC();
		TPtr8 searchFieldPtr = searchField->Des();
		searchFieldPtr.UpperCase();

		// Allocate space for upper case version fields in the array
		HBufC8* field = HBufC8::NewLC(iMaxFieldLength);
		TPtr8 fieldPtr = field->Des();

		// search through the array to find the data matching the given field
		for(i = 0; i < iArray.Count(); i++)
			{
			fieldPtr.Copy(iArray[i]->Field8());
			fieldPtr.UpperCase();
			if(fieldPtr == searchFieldPtr)
				{
				CleanupStack::PopAndDestroy(2, searchField); // searchField, field
				return iArray[i]->Data8();
				}
			}
		CleanupStack::PopAndDestroy(2, searchField); // searchField, field
		}
	else	
		{
		// search through the array to find the data matching the given field
		for(i = 0; i < iArray.Count(); i++)
			{
			if(iArray[i]->Field8() == aField8)
				{
				return iArray[i]->Data8();
				}
			}
		}
	// not found so return an empty string
	return KNullDesC8();
	}
	
EXPORT_C void CMetaDataArray::ExternalizeL(RWriteStream& aStream) const
	{
	TInt i;
	aStream.WriteInt32L(iArray.Count());
	for(i = 0; i < iArray.Count();i++)
		{
		aStream << *(iArray[i]);
		}
	}

void CMetaDataArray::InternalizeL(RReadStream& aStream)
	{
	TInt i;
	TInt length = 0;

	// Read the number of CMetaData objects from the stream
	TInt count = aStream.ReadInt32L();

	// Read the CMetaData objects from the stream and add them to the array
	for(i = 0; i < count; i++)
		{
		CMetaData* metaData = CMetaData::NewL(aStream);
		CleanupStack::PushL(metaData);
		User::LeaveIfError(iArray.Append(metaData));

		// See if this field is longer than any previous field
		length = metaData->Field().Length();
		if(length > iMaxFieldLength)
			{
			iMaxFieldLength = length;
			}
		
		// Finished with cleanup stack, metaData now owned by the array so don't delete
		CleanupStack::Pop(metaData);
		}
	}


