/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pkcs12.h"
#include "pkcs7signedobject.h"

using namespace PKCS12;

CDecPkcs12::CDecPkcs12()
	{
	}
	
EXPORT_C CDecPkcs12* CDecPkcs12::NewL(const TDesC8& aRawData) 
    {
    CDecPkcs12* self = NewLC(aRawData);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CDecPkcs12* CDecPkcs12::NewLC(const TDesC8& aRawData) 
    { 
    CDecPkcs12* self = new (ELeave) CDecPkcs12;
    CleanupStack::PushL(self);
    self->ConstructL(aRawData);
    return self;
    }

EXPORT_C CDecPkcs12* CDecPkcs12::NewL(RReadStream& aStream) 
    {
    CDecPkcs12* self = NewLC(aStream);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CDecPkcs12* CDecPkcs12::NewLC(RReadStream& aStream) 
    { 
    CDecPkcs12* self = new (ELeave) CDecPkcs12;
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    return self;
    }

CDecPkcs12::~CDecPkcs12()
	{
	iContentInfos.ResetAndDestroy();
	iContentInfos.Close();
	delete iAuthenticatedSafeData;
	delete iMacData;	
	}
    
void CDecPkcs12::ConstructL(const TDesC8& aRawData)
	{	
	TASN1DecGeneric decGen(aRawData);
	decGen.InitL();
	if(decGen.Tag() != EASN1Sequence || decGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
		
	// aRawData conatins PFX Sequence
	TASN1DecSequence decSeq;
	CArrayPtr<TASN1DecGeneric>* seqContents = decSeq.DecodeDERLC(decGen);

    // Check if both the version and the authSafe are present, since macData is optional, 
	TInt pfxCount = seqContents->Count();
	if (pfxCount > 3 || pfxCount < 2)
		{
		User::Leave(KErrArgument);
		}
				
	// Decodes Version, Version is an Integer
	const TASN1DecGeneric* seqContentsAt0 = seqContents->At(0);
	if (seqContentsAt0->Tag() != EASN1Integer || seqContentsAt0->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecInteger intDecoder;
	iVersion = intDecoder.DecodeDERShortL(*seqContentsAt0);
	if (iVersion != KPkcs12Version)
		{
		User::Leave(KErrArgument);
		}
	
	// Decodes the AuthSafe, AuthSafe is a Sequence
	// If AuthSafe is not present then leave
	const TASN1DecGeneric* seqContentsAt1 = seqContents->At(1);
	if(seqContentsAt1->Tag() != EASN1Sequence || seqContentsAt1->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecSequence authSafeSeq;
	CArrayPtr<TASN1DecGeneric>* authSafeContents = authSafeSeq.DecodeDERLC(*seqContentsAt1);
	const TASN1DecGeneric* authSafeContentsAt0 = authSafeContents->At(0);
	if(authSafeContentsAt0->Tag() == EASN1ObjectIdentifier || authSafeContentsAt0->Class() == EUniversal)
		{
		TASN1DecObjectIdentifier oid;
		HBufC* objectId = oid.DecodeDERL(*authSafeContentsAt0);
		CleanupStack::PushL(objectId);
		if(*objectId == KPkcs7DataOID || *objectId == KPkcs7SignedDataOID)
			{
			// Decode this Sequence, If there is an OID for Data/Signed Data then use PKCS7 library
			iAuthenticatedSafeData = CPKCS7ContentInfo::NewL(seqContentsAt1->Encoding());
			}
		else
			{
			User::Leave(KErrArgument);
			}
		CleanupStack::PopAndDestroy(objectId);
		}
	// AuthSafe is absent
	else 
		{
		User::Leave(KErrArgument);
		}
	
	// Integrity Mode	
	if(iAuthenticatedSafeData->ContentType() == KPkcs7Data)
		{
		iMode = EPasswordIntegrityMode;	
		}
	else if(iAuthenticatedSafeData->ContentType() == KPkcs7SignedData)
		{
		iMode = EPublicKeyIntegrityMode;
		}
	
	TPtrC8 contentData(KNullDesC8);
	// In case of Password Integrity Mode, ContentType is Data
	if(iMode == EPasswordIntegrityMode)
		{
		// Check if MacData is present, Decodes MacData
		if (seqContents->Count() == 3)
  			{
  			const TASN1DecGeneric* seqContentsAt2 = seqContents->At(2);
    		if (seqContentsAt2->Tag() != EASN1Null || seqContentsAt2->Class() == EUniversal)
				{
				iMacData = CDecPkcs12MacData::NewL(seqContentsAt2->Encoding(),iAuthenticatedSafeData->ContentData());
			    } 
			}
		contentData.Set(iAuthenticatedSafeData->ContentData());
		}
    // Public-Key Integrity Mode, ContentType is SignedData
	else if(iMode == EPublicKeyIntegrityMode)
		{
		// Create a pkcs7signed data object
		CPKCS7SignedObject* signedData = CPKCS7SignedObject::NewL(*iAuthenticatedSafeData);
		CleanupStack::PushL(signedData);
		// Obtain the ContentInfo present in Signed Data
		const CPKCS7ContentInfo* contentInfo = &(signedData->ContentInfo());
		
		// Get the type of ContentInfo
		if (contentInfo->ContentType() != KPkcs7Data)
			{
			User::Leave(KErrNotSupported);
			}
		contentData.Set(contentInfo->ContentData());
		CleanupStack::PopAndDestroy(signedData);
		}
	else
		{
		User::Leave(KErrArgument);
		}

	TASN1DecGeneric decGen1(contentData);
	decGen1.InitL(); 					
	if(decGen1.Tag() != EASN1Sequence || decGen1.Class() != EUniversal)      
		{
		User::Leave(KErrArgument);
		}
	
	// Desequence the ContentData present in ContentInfo which is in AuthSafe
    TASN1DecSequence seq;
    CArrayPtr<TASN1DecGeneric>* contentInfoSequences = seq.DecodeDERLC(decGen1);
    
    // The number of ContentInfos present         
    TInt contentInfoCount = contentInfoSequences->Count();
                    
    for(TInt index =0; index < contentInfoCount; index++)
    	{
        CPKCS7ContentInfo* contentInfo = CPKCS7ContentInfo::NewL(contentInfoSequences->At(index)->Encoding());
        CleanupStack::PushL(contentInfo);
        iContentInfos.AppendL(contentInfo);
        CleanupStack::Pop(contentInfo);
        }
	CleanupStack::PopAndDestroy(3,seqContents); // seqContents, authSafeContents, contentInfoSequences
  	} 

void CDecPkcs12::InternalizeL(RReadStream& aStream)
	{
	MStreamBuf* streamBuf = aStream.Source();
	TInt pos = streamBuf->SizeL();
	streamBuf->SeekL(MStreamBuf::ERead, EStreamBeginning, 0);
	HBufC8* temp= HBufC8::NewLC(pos);	
	TPtr8 ptr = temp->Des();	
	aStream.ReadL(ptr,pos);	
    ConstructL(*temp);    
	CleanupStack::PopAndDestroy(temp); 
	}	

EXPORT_C TInt CDecPkcs12::Version() const
	{
	return iVersion;
   	}

EXPORT_C CDecPkcs12::TIntegrityMode CDecPkcs12::IntegrityMode() const
	{
	return iMode;
   	}
	
EXPORT_C const RPointerArray<CPKCS7ContentInfo>& CDecPkcs12::AuthenticatedSafeContents() const
	{
	return iContentInfos;
	}

EXPORT_C const CPKCS7ContentInfo& CDecPkcs12::AuthenticatedSafe() const
	{
	return *iAuthenticatedSafeData;
	}

EXPORT_C const CDecPkcs12MacData* CDecPkcs12::MacData() const
	{
	return iMacData;
	}

















