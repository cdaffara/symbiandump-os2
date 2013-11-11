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
#include <pbedata.h>
#include "pkcs5kdf.h"
#include "pbencryptor.h"
#include "pbe.h"
#include <securityerr.h>
#include "pbesymmetricfactory.h"

EXPORT_C CPBEncryptSet* CPBEncryptSet::NewL(const TPBPassword& aPassword)
	{
	CPBEncryptSet* self = NewLC(aPassword);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptSet* CPBEncryptSet::NewLC(const TPBPassword& aPassword)
	{
	CPBEncryptSet* self = new(ELeave) CPBEncryptSet;
	CleanupStack::PushL(self);
	self->ConstructL(aPassword.Password());
	return self;
	}

EXPORT_C CPBEncryptSet* CPBEncryptSet::NewL(const TPBPassword& aPassword, 
	const TPBECipher aCipher)
	{
	CPBEncryptSet* self = NewLC(aPassword, aCipher);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptSet* CPBEncryptSet::NewLC(const TPBPassword& aPassword, 
	const TPBECipher aCipher)
	{
	CPBEncryptSet* self = new(ELeave) CPBEncryptSet;
	CleanupStack::PushL(self);
	self->ConstructL(aPassword.Password(), aCipher);
	return self;
	}

EXPORT_C CPBEncryptSet* CPBEncryptSet::NewL(const TPBPassword& aPassword,
	const CPBEncryptParms& aParms)
	{
	CPBEncryptSet* self = NewLC(aPassword, aParms);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptSet* CPBEncryptSet::NewLC(const TPBPassword& aPassword, 
	const CPBEncryptParms& aParms)
	{
	CPBEncryptSet* self = new(ELeave) CPBEncryptSet;
	CleanupStack::PushL(self);
	self->ConstructL(aPassword.Password(), aParms);
	return self;
	}

EXPORT_C CPBEncryptSet* CPBEncryptSet::NewL(const CPBEncryptionData& aData, 
	const TDesC8& aEncryptedKey, const TPBPassword& aPassword)
	{
	CPBEncryptSet* self = NewLC(aData, aEncryptedKey, aPassword);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptSet* CPBEncryptSet::NewLC(const CPBEncryptionData& aData,
	const TDesC8& aEncryptedKey, const TPBPassword& aPassword)
	{
	CPBEncryptSet* self = new(ELeave) CPBEncryptSet;
	CleanupStack::PushL(self);
	self->ConstructL(aData, aEncryptedKey, aPassword);
	return self;
	}

void CPBEncryptSet::ConstructL(const TDesC8& aPassword)
	{
	CPBEncryptElement::ConstructL(aPassword);
	ConstructMasterKeyL();
	}

void CPBEncryptSet::ConstructL(const TDesC8& aPassword,
	const TPBECipher aCipher)
	{
	CPBEncryptElement::ConstructL(aPassword, aCipher);
	ConstructMasterKeyL();
	}

void CPBEncryptSet::ConstructL(const TDesC8& aPassword, 
	const CPBEncryptParms& aParms)
	{
	CPBEncryptElement::ConstructL(aPassword, aParms);
	ConstructMasterKeyL();
	}

void CPBEncryptSet::ConstructMasterKeyL(void)
	{
	TBuf8<KAESKeyBytes256> masterKey(KAESKeyBytes256);
	TRandom::RandomL(masterKey);
	iEncryptedMasterKey = HBufC8::NewL(KAESKeyBytes256);
	EncryptMasterKeyL(masterKey);
	}

void CPBEncryptSet::ConstructL(const CPBEncryptionData& aData,
	const TDesC8& aEncryptedMasterKey, const TPBPassword& aPassword)
	{
	CPBEncryptElement::ConstructL(aData, aPassword);
	iEncryptedMasterKey = aEncryptedMasterKey.AllocL();
	}

EXPORT_C void CPBEncryptSet::ChangePasswordL(const TPBPassword& aNewPassword)
	{
	//1) Decrypt master key with old encrypt key 
	TBuf8<KPBEMaxCipherKeyBytes> masterKey;
	DecryptMasterKeyL(masterKey);

	//2) create new encrypt parms

	TBuf8<KPBEMaxSaltBytes> authSalt(KPBEMaxSaltBytes);
	TRandom::RandomL(authSalt);

	//3) create a totally new CPBEncryptionData representing the new password
	CPBEncryptionData* newData = CPBEncryptionData::NewL(
		aNewPassword.Password(), authSalt, iData->EncryptParms());

	delete iData;
	iData = newData;

	// regenerate the password using a maximum length salt.
	CPBEncryptParms& epNonConst =
		const_cast<CPBEncryptParms&>(iData->EncryptParms());
	epNonConst.ResizeSaltL(KPBEMaxSaltBytes);

	TPtr8 iEncryptKeyBuf(iEncryptKey->Des());
	iEncryptKeyBuf.SetLength(PBE::GetKeyBytes(iData->EncryptParms().Cipher()));
	
	iData->EncryptParms().DeriveKeyL(aNewPassword.Password(), iEncryptKeyBuf);

	//4) Encrypt master key with new encrypt key
	EncryptMasterKeyL(masterKey);
	}

EXPORT_C const TDesC8& CPBEncryptSet::EncryptedMasterKey(void) const
	{
	return *iEncryptedMasterKey;
	}

CPBEncryptor* CPBEncryptSet::NewEncryptLC(void) const
	{
	CPBEncryptor* encryptor = NewEncryptL();
	CleanupStack::PushL(encryptor);
	return encryptor;
	}

CPBEncryptor* CPBEncryptSet::NewEncryptL(void) const
	{
	TBuf8<KPBEMaxCipherKeyBytes> masterKey;
	DecryptMasterKeyL(masterKey);

	CPBEncryptor* encryptor = 0;
	//make sure the masterkey we pass is exactly the right length for the cipher
	encryptor = CPBEncryptorSet::NewL(iData->EncryptParms().Cipher(),
		masterKey.Left(PBE::GetKeyBytes(iData->EncryptParms().Cipher())));		
	return encryptor;
	}

CPBDecryptor* CPBEncryptSet::NewDecryptLC(void) const
	{
	CPBDecryptor* decryptor = NewDecryptL();
	CleanupStack::PushL(decryptor);
	return decryptor;
	}

CPBDecryptor* CPBEncryptSet::NewDecryptL(void) const
	{
	TBuf8<KPBEMaxCipherKeyBytes> masterKey;
	DecryptMasterKeyL(masterKey);

	CPBDecryptor* decryptor = 0;
	//make sure the masterkey we pass is exactly the right length for the cipher
	decryptor = CPBDecryptorSet::NewL(iData->EncryptParms().Cipher(),
		masterKey.Left(PBE::GetKeyBytes(iData->EncryptParms().Cipher())));		
	return decryptor;
	}

void CPBEncryptSet::DecryptMasterKeyL(TDes8& aMasterKey) const
	{
	CPBDecryptorElement* decryptor = CPBDecryptorElement::NewLC(
		iData->EncryptParms().Cipher(), *iEncryptKey, iData->EncryptParms().IV());
	aMasterKey.SetLength(0);
	decryptor->Process(*iEncryptedMasterKey, aMasterKey);
	CleanupStack::PopAndDestroy(decryptor);
	}

void CPBEncryptSet::EncryptMasterKeyL(const TDesC8& aMasterKey)
	{
	CPBEncryptorElement* encryptor = CPBEncryptorElement::NewLC(
		iData->EncryptParms().Cipher(), *iEncryptKey, iData->EncryptParms().IV());
	TPtr8 encryptedMasterKeyBuf(iEncryptedMasterKey->Des());
	encryptedMasterKeyBuf.SetLength(0);
	encryptor->Process(aMasterKey, encryptedMasterKeyBuf);
	CleanupStack::PopAndDestroy(encryptor);
	}

CPBEncryptSet::CPBEncryptSet()
	{
	}

CPBEncryptSet::~CPBEncryptSet()
	{
	delete iEncryptedMasterKey;
	}

// Warning: This function is only valid BEFORE you call NewEncryptL
// After creating the cipher, ask it about itself, not me!
// This is _very_ dodgy as I assume all sorts of things about the encryptor.
// 1) That it uses SSLv3 or similar style padding
// 2) That it stores the IV for that stream at the front.
// This is here for specific application that requires this and aren't able to
// actually construct the cipher and ask it.  In almost all other cases you
// should construct the cipher and ask it.
TInt CPBEncryptSet::MaxCiphertextLength(TInt aPlaintextLength) const
    {
	TUint blocksize = PBE::GetBlockBytes(iData->EncryptParms().Cipher());
	TUint padding = blocksize - aPlaintextLength % blocksize;
	//totallength = blocksize of iv hidden at beginning + inputLength + padding
	return blocksize + aPlaintextLength + padding;
    }

// Warning: This function is only valid BEFORE you call NewDecryptL
// After creating the cipher, ask it about itself, not me!
TInt CPBEncryptSet::MaxPlaintextLength(TInt aCiphertextLength) const
    {
	/*It's impossible to determine anything about how much padding will be
	 * removed.  So we'll return a max length that is longer than will ever
	 * happen by at most a blocksize - 1.
	 */
	//In all cases SSLv3 padding has at least one byte of padding.
	TUint blocksize = PBE::GetBlockBytes(iData->EncryptParms().Cipher());
	//totallength = inputlength - iv hidden at beginning - 1 byte of padding
    return aCiphertextLength - blocksize - 1;
    }

