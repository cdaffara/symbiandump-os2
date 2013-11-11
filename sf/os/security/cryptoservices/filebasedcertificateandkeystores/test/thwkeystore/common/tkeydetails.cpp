/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines the class which represents the structure of the key on 
* which Crypto Token HAI internally operates. It contains the key 
* information relevant to Crypto Token HAI.
*
*/


#include "tkeydetails.h"
#include <mctkeystore.h>

CKeyDetails::CKeyDetails()
	{}

CKeyDetails::CKeyDetails(	
		TKeyIdentifier aID,
		TKeyUsagePKCS15 aUsage,
		TUint aSize, 
		HBufC* aLabel,
		TInt aHandle,
		const TSecurityPolicy& aUsePolicy,
		const TSecurityPolicy& aManagementPolicy,
		EKeyAlgorithm aAlgorithm,
		TInt aAccessType,
		TBool aNative,
		TTime aStartDate,
		TTime aEndDate,
		HBufC8* aPKCS8AttributeSet)
		: CKeyInfoBase(	aID,aUsage,aSize,aLabel,aHandle,
						aUsePolicy,aManagementPolicy,aAlgorithm,
						aAccessType,aNative,aStartDate,aEndDate,aPKCS8AttributeSet)
		{}

CKeyDetails::~CKeyDetails()
	{
	delete iPrivateKey;
	delete iPublicKey;
	}

TInt CKeyDetails::Handle() const
	{
	return iHandle;
	}

HBufC8* CKeyDetails::PrivateKey() const
	{
	return iPrivateKey;
	}

HBufC8* CKeyDetails::PublicKey() const
    {
    return iPublicKey;
    }

CKeyDetails* CKeyDetails::NewL(	TInt aHandle, 
								const TDesC& aLabel, 
								const TDesC8& aPrivateKey, 
								const TDesC8& aPublicKey )
	{
	TKeyIdentifier keyID;
	keyID.FillZ(keyID.MaxSize());
	TKeyUsagePKCS15 usage = EPKCS15UsageNone;
	TUint size = 0;
	TInt handle = aHandle;
	const TSecurityPolicy& usePolicy = TSecurityPolicy::EAlwaysPass;
	const TSecurityPolicy& managementPolicy = TSecurityPolicy::EAlwaysPass;
	EKeyAlgorithm algorithm = EECC;
	TInt accessType = CKeyInfoBase::ENeverExtractable;
	accessType |= CKeyInfoBase::ELocal;
	TBool native = ETrue;
	TTime startDate = 0;
	TTime endDate = 0;
	HBufC8* pkcs8AttributeSet = NULL;
	
	HBufC* label = HBufC::NewLC(aLabel.Length());
	label->Des().Copy(aLabel);

	CKeyDetails* keyDetails = new (ELeave) CKeyDetails(keyID,usage,size,label,handle,usePolicy,managementPolicy,algorithm,accessType,native,startDate,endDate,pkcs8AttributeSet);
	
	CleanupStack::Pop(label);
	CleanupStack::PushL(keyDetails);
	keyDetails->ConstructL(aPrivateKey, aPublicKey);
	CleanupStack::Pop(keyDetails);
	return keyDetails;
	}


CKeyDetails* CKeyDetails::NewL(RStoreReadStream& aReadStream)
    {
    CKeyDetails* self = new (ELeave) CKeyDetails();
    CleanupStack::PushL(self);
    self->ConstructL(aReadStream);
    CleanupStack::Pop(self);
    return (self);
    }

void CKeyDetails::ConstructL( const TDesC8& aPrivateKey, const TDesC8& aPublicKey )
	{
	iPrivateKey = aPrivateKey.AllocL();
	iPublicKey = aPublicKey.AllocL();   
	}

void CKeyDetails::ConstructL(RStoreReadStream& aReadStream)
	{
	CKeyInfoBase::ConstructL(aReadStream);
	InternalizeL(aReadStream);
	}

void CKeyDetails::ExternalizeL(RWriteStream& aWriteStream) const
    {		
    CKeyInfoBase::ExternalizeL(aWriteStream);
    
    TInt stringLen = iPrivateKey->Length();
    aWriteStream.WriteInt32L(stringLen);
    TPtr8 keyPtr = iPrivateKey->Des();
    keyPtr.SetLength(stringLen);
    aWriteStream.WriteL(keyPtr);
        
    stringLen = iPublicKey->Length();
    aWriteStream.WriteInt32L(stringLen);
    keyPtr = iPublicKey->Des();
    keyPtr.SetLength(stringLen);
    aWriteStream.WriteL(keyPtr);
    
    }

void CKeyDetails::InternalizeL(RReadStream& aReadStream)
    {
    TInt stringLen = aReadStream.ReadInt32L();
    iPrivateKey = HBufC8::NewMaxL(stringLen);
    TPtr8 privateKeyPtr((TUint8*)iPrivateKey->Ptr(), stringLen, stringLen);
    privateKeyPtr.FillZ(stringLen);
    aReadStream.ReadL(privateKeyPtr);
        
    stringLen = aReadStream.ReadInt32L();
    iPublicKey = HBufC8::NewMaxL(stringLen);
    TPtr8 publicKeyPtr((TUint8*)iPublicKey->Ptr(), stringLen, stringLen);
    publicKeyPtr.FillZ(stringLen);
    aReadStream.ReadL(publicKeyPtr);
    
    }
