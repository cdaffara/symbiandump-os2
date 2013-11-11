/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include "cryptostrength.h"
#include <securityerr.h>

#ifdef CRYPTO_STRONG_BUILD
const TInt KDTIMaxSymmetricKeyBits = KMaxTInt;
const TInt KDTIMaxAsymmetricKeyBits = KMaxTInt;
const TCrypto::TStrength KCryptoStrength=TCrypto::EStrong;
#else //CRYPTO_WEAK_BUILD
const TInt KDTIMaxSymmetricKeyBits = 56;
const TInt KDTIMaxAsymmetricKeyBits = 512;
const TCrypto::TStrength KCryptoStrength=TCrypto::EWeak;
#endif //CRYPTO_STRONG_BUILD

EXPORT_C TCrypto::TStrength TCrypto::Strength(void)
	{
	return (KCryptoStrength);
	}

EXPORT_C TBool TCrypto::IsSymmetricWeakEnoughL(TInt aSymmetricKeyBits)
	{
	if(aSymmetricKeyBits > KDTIMaxSymmetricKeyBits)
		{
		User::Leave(KErrKeyNotWeakEnough);
		}
	return ETrue;
	}

EXPORT_C TBool TCrypto::IsAsymmetricWeakEnoughL(TInt aAsymmetricKeyBits)
	{
	if(aAsymmetricKeyBits > KDTIMaxAsymmetricKeyBits)
		{
		User::Leave(KErrKeyNotWeakEnough);
		}
	return ETrue;
	}
 
