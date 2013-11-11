/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef REMOVE_CAF1

#include <caf/caftypes.h>
#include <s32strm.h>
#include <caf/cafmimeheader.h>
#include "mimefieldanddata.h"

using namespace ContentAccess;

EXPORT_C CCafMimeHeader* CCafMimeHeader::NewL(const TDesC8 &aContentType)
	{
	CCafMimeHeader *self = new (ELeave) CCafMimeHeader();
	CleanupStack::PushL(self);
	self->ConstructL(aContentType);
	CleanupStack::Pop(self);
	return self;
	}

CCafMimeHeader::CCafMimeHeader() 
	{
	}

void CCafMimeHeader::ConstructL(const TDesC8 &aContentType) 
	{
	SetStandardMimeDataL(EContentType, aContentType);
	}

EXPORT_C CCafMimeHeader::~CCafMimeHeader()
	{
	TInt i=0;
	iNonStandardMimeHeaders.ResetAndDestroy();
	iNonStandardMimeHeaders.Close();
	
	for(i = 0; i < EMimeMax; i++)
		{
		delete iStandardMimeHeader[i];
		}
	}

EXPORT_C void CCafMimeHeader::SetStandardMimeDataL(const TMimeFields &aIndex, const TDesC8& aData)
	{
	HBufC8 *data = aData.AllocLC();

	// make sure mime types are lower case
	if(aIndex == EContentType)
		{
		data->Des().LowerCase();
		}
	
	// replace whatever value was there before
	delete iStandardMimeHeader[aIndex];
	iStandardMimeHeader[aIndex] = data;
		
	CleanupStack::Pop(data); 
	}

EXPORT_C TPtrC8 CCafMimeHeader::StandardMimeData(const TMimeFields &aIndex) const
	{
	// If the field value has not been set, return a null string
	if(iStandardMimeHeader[aIndex] == NULL)
		{
		return KNullDesC8();
		}
	return TPtrC8(iStandardMimeHeader[aIndex]->Des());
	}


EXPORT_C TInt CCafMimeHeader::NonStandardMimeCount() const
	{
	return iNonStandardMimeHeaders.Count();
	}

EXPORT_C void CCafMimeHeader::AddNonStandardMimeL(const TDesC8& aFieldName, const TDesC8& aData)
	{
	CMimeFieldAndData *mimeData = CMimeFieldAndData::NewL(aFieldName, aData);
	CleanupStack::PushL(mimeData);
	iNonStandardMimeHeaders.Append(mimeData);
	CleanupStack::Pop(mimeData);
	}

EXPORT_C TPtrC8 CCafMimeHeader::NonStandardMimeField(TInt aIndex) const
	{
	return iNonStandardMimeHeaders[aIndex]->FieldName();
	}

EXPORT_C TPtrC8 CCafMimeHeader::NonStandardMimeData(TInt aIndex) const
	{
	return iNonStandardMimeHeaders[aIndex]->Data();
	}

EXPORT_C void CCafMimeHeader::ExternalizeL(RWriteStream& aStream) const
	{
	TInt i=0;
	
	// Write the contents of this class out to a stream

	// Write the number of standard mime headers
	aStream.WriteInt32L(EMimeMax);
	
	// Write out the standard mime data, each one preceeded by it's length
	for(i = 0; i < EMimeMax; i++)
		{
		TInt length=0;
		
		// Length is zero if the mime field has not been set, 
		if(iStandardMimeHeader[i] == NULL)
			{
			length = 0;
			}
		else 
			{
			length = iStandardMimeHeader[i]->Length();
			}

		// write the field length
		aStream.WriteInt16L(length);
		if(length > 0)
			{
			// write the field value if length > 0
			aStream.WriteL(*iStandardMimeHeader[i]);
			}
		}

	aStream.WriteInt32L(iNonStandardMimeHeaders.Count());

	// Write out non standard mime headers and data, each Des preceeded by it's length
	for(i = 0; i < iNonStandardMimeHeaders.Count(); i++)
		{
		aStream.WriteInt16L(iNonStandardMimeHeaders[i]->FieldName().Length());
		if(iNonStandardMimeHeaders[i]->FieldName().Length() > 0)
			{
			aStream.WriteL(iNonStandardMimeHeaders[i]->FieldName());
			}
		aStream.WriteInt16L(iNonStandardMimeHeaders[i]->Data().Length());
		if(iNonStandardMimeHeaders[i]->Data().Length() > 0)
			{
			aStream.WriteL(iNonStandardMimeHeaders[i]->Data());
			}
		}
	}

EXPORT_C void CCafMimeHeader::InternalizeL(RReadStream& aStream)
	{
	TInt i=0;
	TInt fieldCount=0;
	TInt16 length=0;
	HBufC8 *field=NULL;
	HBufC8 *data=NULL;
	

	// clear any existing headers	
	iNonStandardMimeHeaders.ResetAndDestroy();
	for(i = 0; i < EMimeMax; i++)
		{
		// delete the existing data for the mime field
		delete iStandardMimeHeader[i];
		iStandardMimeHeader[i] = NULL;
		}


	fieldCount= aStream.ReadInt32L();
	if(fieldCount != EMimeMax)
		{
		// something is seriously wrong, should always be EMimeMax standard headers
		User::Leave(KErrGeneral);
		}
	
	for(i = 0; i < EMimeMax; i++)
		{
		// read the length from the stream
		length = aStream.ReadInt16L();
		if(length > 0)
			{
			// read in non zero fields, others will remain NULL
			data = HBufC8::NewLC(length);
			TPtr8 dataPtr = data->Des();
			aStream.ReadL(dataPtr,length);
			SetStandardMimeDataL(static_cast <TMimeFields>(i), dataPtr);
			CleanupStack::PopAndDestroy(data);
			}
		}

	fieldCount= aStream.ReadInt32L();

	for(i = 0; i < fieldCount; i++)
		{
		// Read mime field name
		length = aStream.ReadInt16L();
		if(length < 0)
			{
			User::Leave(KErrCorrupt);
			}

		field = HBufC8::NewLC(length);
		TPtr8 fieldPtr = field->Des();
		if(length > 0)
			{
			aStream.ReadL(fieldPtr,length);
			}
		
		// Read data
		length = aStream.ReadInt16L();
		if(length < 0)
			{
			User::Leave(KErrCorrupt);
			}

		data = HBufC8::NewLC(length);
		TPtr8 dataPtr = data->Des();
		if(length > 0)
			{
			aStream.ReadL(dataPtr,length);
			}

		// Add a new mime field and data to our array
		AddNonStandardMimeL(fieldPtr, dataPtr);

		// free memory allocated
		CleanupStack::PopAndDestroy(data);
		CleanupStack::PopAndDestroy(field); 
		}
	}


#endif // #ifndef REMOVE_CAF1
