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


#include <e32std.h>
#include <random.h>
#include <cryptostrength.h>
#include <securityerr.h>
#include <pbedata.h>
#include "pkcs5kdf.h"
#include "pbe.h"
#include <pbencryptor.h>
#include "pbesymmetricfactory.h"

EXPORT_C CPBEncryptElement* CPBEncryptElement::NewL(const TPBPassword& aPassword)
	{
	CPBEncryptElement* self = NewLC(aPassword);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptElement* CPBEncryptElement::NewLC(const TPBPassword& aPassword)
	{
	CPBEncryptElement* self = new(ELeave) CPBEncryptElement;
	CleanupStack::PushL(self);
	self->ConstructL(aPassword.Password());
	return self;
	}

EXPORT_C CPBEncryptElement* CPBEncryptElement::NewL(const TPBPassword& aPassword, 
	const TPBECipher aCipher)
	{
	CPBEncryptElement* self = NewLC(aPassword, aCipher);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptElement* CPBEncryptElement::NewLC(const TPBPassword& aPassword, 
	const TPBECipher aCipher)
	{
	CPBEncryptElement* self = new(ELeave) CPBEncryptElement;
	CleanupStack::PushL(self);
	self->ConstructL(aPassword.Password(), aCipher);
	return self;
	}

EXPORT_C CPBEncryptElement* CPBEncryptElement::NewL(const TPBPassword& aPassword, 
	const CPBEncryptParms& aParms)
	{
	CPBEncryptElement* self = NewLC(aPassword, aParms);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptElement* CPBEncryptElement::NewLC(const TPBPassword& aPassword, 
	const CPBEncryptParms& aParms)
	{
	CPBEncryptElement* self = new(ELeave) CPBEncryptElement;
	CleanupStack::PushL(self);
	self->ConstructL(aPassword.Password(), aParms);
	return self;
	}

EXPORT_C CPBEncryptElement* CPBEncryptElement::NewL(
	const CPBEncryptionData& aData, const TPBPassword& aPassword)
	{
	CPBEncryptElement* self = CPBEncryptElement::NewLC(aData, aPassword);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptElement* CPBEncryptElement::NewLC(
	const CPBEncryptionData& aData, const TPBPassword& aPassword)
	{
	CPBEncryptElement* self = new(ELeave) CPBEncryptElement;
	CleanupStack::PushL(self);
	self->ConstructL(aData, aPassword);
	return self;
	}

CPBEncryptElement::CPBEncryptElement(void) 
	{
	}

CPBEncryptElement::~CPBEncryptElement(void)
	{
	delete iData;
	delete iEncryptKey;
	}

void CPBEncryptElement::ConstructL(const TDesC8& aPassword)
	{
	// Construct based on cryptography strength
	if (TCrypto::Strength() == TCrypto::EStrong)
		{
		ConstructL(aPassword, KPBEDefaultStrongCipher);
		}
	else
		{
		ConstructL(aPassword, KPBEDefaultWeakCipher);
		}
	}

void CPBEncryptElement::ConstructL(const TDesC8& aPassword, TPBECipher aCipher)
	{
	TBuf8<KPBEMaxCipherIVBytes> iv(KPBEMaxCipherIVBytes);
	iv.SetLength(PBE::GetBlockBytes(aCipher));
	TRandom::RandomL(iv);

	TBuf8<KPBEDefaultSaltBytes> encryptSalt(KPBEDefaultSaltBytes);
	TRandom::RandomL(encryptSalt);

	TBuf8<KPBEDefaultSaltBytes> authSalt(KPBEDefaultSaltBytes);
	TRandom::RandomL(authSalt);

	iData = CPBEncryptionData::NewL(aPassword, aCipher, authSalt, encryptSalt,
		iv, KDefaultIterations);

	MakeEncryptKeyL(PBE::GetKeyBytes(aCipher), aPassword);
	}

void CPBEncryptElement::ConstructL(const CPBEncryptionData& aData, 
	const TPBPassword& aPassword)
	{
	iData = CPBEncryptionData::NewL(aData);
	if(!AuthenticateL(aPassword))
		{
		User::Leave(KErrBadPassphrase);
		}
	}

void CPBEncryptElement::ConstructL(const TDesC8& aPassword,
	const CPBEncryptParms& aParms)
	{
	TUint keySize = PBE::GetKeyBytes(aParms.Cipher());

	TBuf8<KPBEDefaultSaltBytes> authSalt(KPBEDefaultSaltBytes);
	TRandom::RandomL(authSalt);

	//Recreate parms with given data and create a totally new auth
	iData = CPBEncryptionData::NewL(aPassword, authSalt, aParms);
	MakeEncryptKeyL(keySize, aPassword);
	}

const CPBEncryptionData& CPBEncryptElement::EncryptionData(void) const
	{
	return *iData;
	}

CPBEncryptor* CPBEncryptElement::NewEncryptL() const 
	{
	CPBEncryptor* encryptor = NewEncryptLC();
	CleanupStack::Pop();
	return encryptor;
	}

CPBEncryptor* CPBEncryptElement::NewEncryptLC() const
	{
	CPBEncryptor* encryptor = CPBEncryptorElement::NewLC( 
		iData->EncryptParms().Cipher(), *iEncryptKey, 
		iData->EncryptParms().IV());
	return encryptor;
	}

TBool CPBEncryptElement::AuthenticateL(const TPBPassword& aPassword)
	{
	TBool retval = EFalse;

	//create a new auth to test against the existing one
	//therefore we use the same key size, and salt as the current one
	CPBAuthData* auth = CPBAuthData::NewLC(aPassword.Password(),
		iData->AuthData().Salt(), iData->AuthData().Key().Size(),
		iData->AuthData().Iterations());
	if(*auth==iData->AuthData())
		{
		//We've got a valid password, regenerate the key so they can decrypt
		//stuff.  We don't the change the length of iEncryptKey as we assume the
		//previous key size is appropriate for the new one
		MakeEncryptKeyL(PBE::GetKeyBytes(iData->EncryptParms().Cipher()),
			aPassword.Password());
		retval = ETrue;
		}
	CleanupStack::PopAndDestroy(auth);
	return retval;
	}

CPBDecryptor* CPBEncryptElement::NewDecryptL() const
	{
	CPBDecryptor* decryptor = NewDecryptLC();
	CleanupStack::Pop();
	return decryptor;
	}

CPBDecryptor* CPBEncryptElement::NewDecryptLC() const
	{
	CPBDecryptor* decryptor = CPBDecryptorElement::NewLC(
		iData->EncryptParms().Cipher(), *iEncryptKey,
		iData->EncryptParms().IV());
	return decryptor;
	}

// Warning: This function is only valid BEFORE you call NewEncryptL
// After creating the cipher, ask it about itself, not me!
// This is _very_ dodgy as I assume all sorts of things about the encryptor.
// 1) That it uses SSLv3 or similar style padding
// 2) That it stores the IV for that stream at the front.
// This is here for specific application that requires this and aren't able to
// actually construct the cipher and ask it.  In almost all other cases you
// should construct the cipher and ask it.
TInt CPBEncryptElement::MaxCiphertextLength(TInt aPlaintextLength) const
	{
	TUint blocksize = PBE::GetBlockBytes(iData->EncryptParms().Cipher());
	TUint padding = blocksize - aPlaintextLength % blocksize;
	//len = inputLength + padding
	return aPlaintextLength + padding;
	}

// Warning: This function is only valid BEFORE you call NewDecryptL
// After creating the cipher, ask it about itself, not me!
TInt CPBEncryptElement::MaxPlaintextLength(TInt aCiphertextLength) const
	{
	/*It's impossible to determine anything about how much padding will be
	 * removed.  So we'll return a max length that is longer than will
	 * ever happen by at most a blocksize - 1.
	 */
	//totallength = inputlength - 1 byte of padding min
	return aCiphertextLength - 1;
	}

void CPBEncryptElement::MakeEncryptKeyL(TUint aKeySize, const TDesC8& aPassword)
	{
	iEncryptKey = HBufC8::NewMaxL(aKeySize);
	TPtr8 encryptKeyBuf = iEncryptKey->Des();
	iData->EncryptParms().DeriveKeyL(aPassword, encryptKeyBuf);
	}

EXPORT_C TPBPassword::TPBPassword(const TDesC8& aPassword)
	{
	iPassword.Set(aPassword);
	}

EXPORT_C TPBPassword::TPBPassword(const TDesC16& aPassword)
	{
	iPassword.Set(reinterpret_cast<const TUint8*>(aPassword.Ptr()), aPassword.Size());
	}

EXPORT_C const TDesC8& TPBPassword::Password(void) const
	{
	return iPassword;
	}


