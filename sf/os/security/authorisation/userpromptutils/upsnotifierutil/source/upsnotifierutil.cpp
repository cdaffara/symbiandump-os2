/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "upsnotifierutil.h"

using namespace UserPromptService;

EXPORT_C CPromptData* CPromptData::NewL() 
/**
Factory function that creates a new, blank CPromptData object.
@return A pointer to the new CPromptData object.
*/
	{
	CPromptData* self = new(ELeave) CPromptData();
	return self;
	}

CPromptData::CPromptData()
/**
 Constructor
 */
	{
	}

CPromptData::~CPromptData()
/**
 Destructor
 */
	{
	Reset();
	}
	
void CPromptData::Reset() 
/**
Frees all internal memory in-case the object is internalised multiple times.
*/
	{
	iClientName.Close();
	iVendorName.Close();
	iClientSid.iId = 0;
	iServerSid.iId = 0;
	iServiceId.iUid = 0;
	iDestination.Close();
	iOpaqueData.Close();
	iDescriptions.ResetAndDestroy();	
	}

EXPORT_C void CPromptData::InternalizeL(RReadStream& aStream)
/**
 Internalizes the prompt data from the specified stream/
 @param aStream The read stream
*/
	{
	Reset();
	iClientName.CreateL(aStream, KMaskDesLength16);
	iVendorName.CreateL(aStream, KMaskDesLength16);
	aStream >> iClientSid.iId;
	aStream >> iServerSid.iId;
	aStream >> iServiceId.iUid;
	iDestination.CreateL(aStream, KMaskDesLength16);
	iOpaqueData.CreateL(aStream, KMaskDesLength8);
	
	TInt32 count = aStream.ReadUint32L();
		
	for (TInt i = 0; i < count; ++i)
		{
		TInt32 l = aStream.ReadInt32L();
		HBufC* d = HBufC::NewMaxLC(l);
		TPtr wptr = d->Des();
		aStream.ReadL(wptr, l);
		iDescriptions.AppendL(d);
		CleanupStack::Pop(d);
		}
	aStream >> iOptions;
	aStream >> iFlags;
	}

EXPORT_C void CPromptData::ExternalizeL(RWriteStream& aStream) const
/**
 Externalizes the prompt data to the specified stream.
 @param aStream The write stream.
 */
	{
	aStream << iClientName;
	aStream << iVendorName;
	aStream << iClientSid.iId;
	aStream << iServerSid.iId;
	aStream << iServiceId.iUid;
	aStream << iDestination;
	aStream << iOpaqueData;
	TInt count = iDescriptions.Count();
	aStream.WriteUint32L(count);
	
	for (TInt i = 0; i < count; ++i)
		{
		aStream.WriteInt32L(iDescriptions[i]->Length());
		aStream.WriteL(*iDescriptions[i]);
		}
	aStream << iOptions;
	aStream << iFlags;
	}
