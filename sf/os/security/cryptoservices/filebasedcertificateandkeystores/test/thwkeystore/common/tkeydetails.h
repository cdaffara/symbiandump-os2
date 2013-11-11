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


/**
 @file
 @internalComponent
 @released
*/

#ifndef TKEYDETAILS_H
#define TKEYDETAILS_H

#include <e32base.h>
#include <mctkeystore.h>

class CKeyDetails : public CKeyInfoBase
	{
public:
	static CKeyDetails* NewL(	TInt aHandle, const TDesC& aLabel, 
								const TDesC8& aPrivateKey, const TDesC8& aPublicKey );
	static CKeyDetails* NewL(RStoreReadStream& aReadStream);
	~CKeyDetails();

public:
	/** Get the handle of the key. */
	TInt Handle() const;
	HBufC8* PrivateKey() const;
	HBufC8* PublicKey() const;
	void ExternalizeL(RWriteStream&) const;
	
private:
	CKeyDetails(TKeyIdentifier aID,
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
				HBufC8* aPKCS8AttributeSet);
		
	void ConstructL(const TDesC8& aPrivateKey, const TDesC8& aPublicKey );
	void ConstructL( RStoreReadStream& aReadStream);
	CKeyDetails();
	void InternalizeL(RReadStream& aReadStream);
	
private:
	HBufC8* iPrivateKey;
	HBufC8* iPublicKey;
	};

#endif // TKEYDETAILS_H
