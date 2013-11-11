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



#include "pkcs5kdf.h"
#include "pkcs12kdf.h"
#include "pbedata.h"
#include "pbesymmetricfactory.h"
#include "cryptostrength.h"

EXPORT_C CPBEncryptionData* CPBEncryptionData::NewL(const TDesC8& aPassword,
	TPBECipher aCipher, const TDesC8& aAuthSalt, 
	const TDesC8& aEncryptSalt, const TDesC8& aIV, TUint aIterations)
	{
	CPBEncryptionData* self = NewLC(aPassword, aCipher, aAuthSalt, aEncryptSalt,
		aIV, aIterations);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBEncryptionData* CPBEncryptionData::NewLC(const TDesC8& aPassword,
	TPBECipher aCipher, const TDesC8& aAuthSalt, 
	const TDesC8& aEncryptSalt, const TDesC8& aIV, TUint aIterations)
	{
	CPBEncryptionData* self = new(ELeave)CPBEncryptionData();
	CleanupStack::PushL(self);
	self->ConstructL(aPassword, aCipher, aAuthSalt, aEncryptSalt, aIV,
		aIterations);
	return self;
	}

EXPORT_C CPBEncryptionData* CPBEncryptionData::NewL(
	const CPBEncryptionData& aData)
	{
	CPBEncryptionData* self = NewLC(aData);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBEncryptionData* CPBEncryptionData::NewLC(
	const CPBEncryptionData& aData)
	{
	CPBEncryptionData* self = new(ELeave)CPBEncryptionData(); 
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return self;
	}

EXPORT_C CPBEncryptionData* CPBEncryptionData::NewL(
	const TDesC8& aPassword, const TDesC8& aAuthSalt,
	const CPBEncryptParms& aParms)
/**
	This factory function takes the user-supplied password
	and the randomly-generated authentication salt, along
	with the encryption paramaters.  It is provided so the
	encryption parameters can be extended without having to
	provide multiple factory functions.

	@param	aPassword		User-supplied password.  This
							password is not transformed so
							if it needs to be in a particular
							format, e.g. for PKCS#12, the
							transformation must be applied before
							this function is called.
	@param	aAuthSalt		The salt is used to derive the
							authentication key; not the encryption
							key.
	@param	aParms			Encryption parameters describe how the
							data is encrypted.
	@return					New instance of CPBEncryptionData.
 */
	{
	CPBEncryptionData* self = new(ELeave) CPBEncryptionData;
	CleanupStack::PushL(self);
	self->ConstructL(aPassword, aAuthSalt, aParms);
	CleanupStack::Pop(self);
	return self;
	}

void CPBEncryptionData::ConstructL(
	const TDesC8& aPassword, const TDesC8& aAuthSalt,
	const CPBEncryptParms& aParms)
/**
	Second-phase constructor for factory function with
	same signature.
 */
	{
	iParms = CPBEncryptParms::NewL(aParms);
	iAuth = CPBAuthData::NewL(
		aPassword,
		aAuthSalt,
		PBE::GetKeyBytes(aParms.Cipher()),
		aParms.Iterations());
	}

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/syslibs/store/ucrypt/ue_strm.cpp
EXPORT_C CPBEncryptionData::CPBEncryptionData(void)
	{
	}

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/syslibs/store/ucrypt/ue_strm.cpp
CPBEncryptionData::~CPBEncryptionData(void)
	{
	delete iParms;
	delete iAuth;
	}

void CPBEncryptionData::ConstructL(const TDesC8& aPassword,
	TPBECipher aCipher, const TDesC8& aAuthSalt, 
	const TDesC8& aEncryptSalt, const TDesC8& aIV, TUint aIterations)
	{
	iParms = CPBEncryptParms::NewL(aCipher, aEncryptSalt, aIV, aIterations);
	iAuth = CPBAuthData::NewL(aPassword, aAuthSalt,
		PBE::GetKeyBytes(aCipher), aIterations);
	}

void CPBEncryptionData::ConstructL(const CPBEncryptionData& aData)
	{
	iParms = CPBEncryptParms::NewL(aData.EncryptParms());
	iAuth = CPBAuthData::NewL(aData.AuthData());
	}

EXPORT_C const CPBEncryptParms& CPBEncryptionData::EncryptParms(void) const
	{
	return *iParms;
	} 
EXPORT_C const CPBAuthData& CPBEncryptionData::AuthData(void) const
	{
	return *iAuth;
	}

/* CPBEncryptParms */
EXPORT_C CPBEncryptParms* CPBEncryptParms::NewL()
/**
	This factory function allocates an encryption
	parameters object with default settings.  The
	individual settings can be retrieved and modified
	with the accessor and mutator functions after
	this object has been created.

	This factory function is provided so that individual
	parameters can be modified without providing many
	factory functions.

	@return					New instance of CPBEncryptParms.
 */
	{
	CPBEncryptParms* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBEncryptParms* CPBEncryptParms::NewLC()
/**
	Similar to the NewL overload which takes no
	arguments, this function additionally puts the
	allocated instance of CPBEncryptParms on the
	cleanup stack.

	@return					New instance of CPBEncryptParms.
 */
	{
	CPBEncryptParms* self = new(ELeave) CPBEncryptParms;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CPBEncryptParms::ConstructL()
/**
	Initialize this object with default cipher, kdf (PKCS#5,)
	salt length, iteration count, and IV.
 */
	{
	iData = new(ELeave) TParamsData;
	iData->iKdf = EKdfPkcs5;
	
	iSalt = HBufC8::NewMaxL(KPBEDefaultSaltBytes);
	TPtr8 saltDes = iSalt->Des();
	TRandom::RandomL(saltDes);
	
	iIterations = KDefaultIterations;
	
	iIV = HBufC8::NewMaxL(KPBEMaxCipherIVBytes);
	
	SetCipher(
			(TCrypto::Strength() == TCrypto::EStrong)
		?	KPBEDefaultStrongCipher : KPBEDefaultWeakCipher );
	}

EXPORT_C CPBEncryptParms* CPBEncryptParms::NewL(TPBECipher aCipher,
	const TDesC8& aSalt, const TDesC8& aIV, TUint aIterations)
	{
	CPBEncryptParms* self = NewLC(aCipher, aSalt, aIV, aIterations);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBEncryptParms* CPBEncryptParms::NewLC(TPBECipher aCipher,
	const TDesC8& aSalt, const TDesC8& aIV, TUint aIterations) 
	{
	CPBEncryptParms* self = new(ELeave)CPBEncryptParms();
	CleanupStack::PushL(self);
	self->ConstructL(aCipher, aSalt, aIV, aIterations);
	return self;
	}

EXPORT_C CPBEncryptParms* CPBEncryptParms::NewL(const CPBEncryptParms& aParms)
	{
	CPBEncryptParms* self = NewLC(aParms);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBEncryptParms* CPBEncryptParms::NewLC(const CPBEncryptParms& aParms)
	{
	CPBEncryptParms* self = new(ELeave)CPBEncryptParms();
	CleanupStack::PushL(self);
	self->ConstructL(aParms);
	return self;
	}

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/syslibs/store/ucrypt/ue_strm.cpp
EXPORT_C CPBEncryptParms::CPBEncryptParms()
	{
	}

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/syslibs/store/ucrypt/ue_strm.cpp
CPBEncryptParms::~CPBEncryptParms()
	{
	delete iData;
	delete iSalt;
	delete iIV;	
	}

void CPBEncryptParms::ConstructL(TPBECipher aCipher, const TDesC8& aSalt,
	const TDesC8& aIV, TUint aIterations)
	{
	iData = new(ELeave) TParamsData;
	iData->iCipher = aCipher;
	iData->iKdf = EKdfPkcs5;
	iSalt = aSalt.AllocL();
	iIV = aIV.AllocL();
	iIterations = aIterations;
	}

void CPBEncryptParms::ConstructL(const CPBEncryptParms& aParms)
	{
	iData = new(ELeave) TParamsData;
	iData->iCipher = aParms.Cipher();
	iData->iKdf = aParms.iData->iKdf;
	iSalt = aParms.Salt().AllocL();
	iIterations = aParms.Iterations();
	iIV = aParms.IV().AllocL();
	}

EXPORT_C TPBECipher CPBEncryptParms::Cipher() const
	{
	return iData->iCipher;
	}

EXPORT_C void CPBEncryptParms::SetCipher(TPBECipher aCipher)
 /**
  *	Replace the current cipher.  This function resizes the
  *	IV and replaces its existing contents.
  *	
  *	@param aCipher	New cipher.
  * 
  * @deprecated		Use SetCipherL instead.
  * @see			SetCipherL
  */
	{
	TPtr8 ivDes = iIV->Des();
	ivDes.SetLength(PBE::GetBlockBytes(aCipher));
	TRandom::RandomL(ivDes);
	
	iData->iCipher = aCipher;
	}

EXPORT_C CPBEncryptParms::TKdf CPBEncryptParms::Kdf() const
/**
	Accessor function returns the key derivation function
	(KDF) specified by this object.

	@return					KDF specified by this object.
 */
	{
	return iData->iKdf;
	}

EXPORT_C void CPBEncryptParms::SetKdf(CPBEncryptParms::TKdf aKdf)
/**
	Replace the current key derivation function.

	@param	aKdf			Key derivation function.
 */
	{
	iData->iKdf = aKdf;
	}

EXPORT_C TPtrC8 CPBEncryptParms::Salt() const
	{
	return TPtrC8(*iSalt);
	}

EXPORT_C void CPBEncryptParms::ResizeSaltL(TInt aNewLen)
/**
	Resize the current salt and replace its contents.

	@param	aNewLen			New salt length.
 */
	{
	iSalt = iSalt->ReAllocL(aNewLen);
	TPtr8 saltDes = iSalt->Des();
	TRandom::RandomL(saltDes);
	}

EXPORT_C TInt CPBEncryptParms::Iterations() const
	{
	return iIterations;
	}

EXPORT_C void CPBEncryptParms::SetIterations(TInt aIterCount)
/**
	Replace the current iteration count with the supplied value.
	
	@param	aIterCount		Number of iterations to apply in
							the KDF.
 */
	{
	ASSERT(aIterCount >= 0);
	iIterations = aIterCount;
	}

EXPORT_C TPtrC8 CPBEncryptParms::IV() const
	{
	return TPtrC8(*iIV);
	}

EXPORT_C void CPBEncryptParms::SetIV(const TDesC8& aNewIv)
/**
	Replace the initialization vector.
	
	@param	aNewIv			New initialization vector length.
							This must have no more than
							KPBEMaxCipherIVBytes bytes.
 */
	{
	iIV->Des().Copy(aNewIv);
	}

void CPBEncryptParms::DeriveKeyL(const TDesC8& aPassword, TDes8& aKeyBuf) const
/**
	Derive a key from this object's kdf, salt, amd iteration count.
	
	@param	aPassword		User-supplied password used to generate key.
	@param	aKeyBuf			Buffer to populate with new key.
							On entry it must be set to the required
							key length.
 */
	{
	switch (iData->iKdf)
		{
	case CPBEncryptParms::EKdfPkcs5:
		TPKCS5KDF::DeriveKeyL(aKeyBuf, aPassword, *iSalt, iIterations);
		break;
	
	case CPBEncryptParms::EKdfPkcs12:
		PKCS12KDF::DeriveKeyL(aKeyBuf, PKCS12KDF::EIDByteEncryptKey, aPassword, *iSalt, iIterations);
		break;
	
	default:
		ASSERT(EFalse);
		break;
		}
	}

/* CPBAuthData */

EXPORT_C CPBAuthData* CPBAuthData::NewL(const TDesC8& aPassword,
	const TDesC8& aSalt, TUint aKeySize, TUint aIterations)
	{
	CPBAuthData* self = NewLC(aPassword, aSalt, aKeySize, aIterations);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBAuthData* CPBAuthData::NewLC(const TDesC8& aPassword,
	const TDesC8& aSalt, TUint aKeySize, TUint aIterations) 
	{
	CPBAuthData* self = new(ELeave)CPBAuthData();
	CleanupStack::PushL(self);
	self->ConstructL(aPassword, aSalt, aKeySize, aIterations);
	return self;
	}

EXPORT_C CPBAuthData* CPBAuthData::NewL(const CPBAuthData& aData)
	{
	CPBAuthData* self = NewLC(aData);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBAuthData* CPBAuthData::NewLC(const CPBAuthData& aData)
	{
	CPBAuthData* self = new(ELeave)CPBAuthData();
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return self;
	}

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/syslibs/store/ucrypt/ue_strm.cpp
EXPORT_C CPBAuthData::CPBAuthData()
	{
	}

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/syslibs/store/ucrypt/ue_strm.cpp
CPBAuthData::~CPBAuthData()
	{
	delete iAuthKey;
	delete iSalt;
	}

void CPBAuthData::ConstructL(const TDesC8& aPassword, const TDesC8& aSalt, 
	TUint aKeySize, TUint aIterations)
	{
	iSalt = aSalt.AllocL();
	iIterations = aIterations;
	iAuthKey = HBufC8::NewMaxL(aKeySize);
	TPtr8 authKeyPtr = iAuthKey->Des();
	TPKCS5KDF::DeriveKeyL(authKeyPtr, aPassword, *iSalt, iIterations);
	}

void CPBAuthData::ConstructL(const CPBAuthData& aData)
	{
	iAuthKey = aData.Key().AllocL();
	iSalt = aData.Salt().AllocL();
	iIterations = aData.Iterations();
	}

EXPORT_C TPtrC8 CPBAuthData::Key() const
	{
	return TPtrC8(*iAuthKey);
	}

EXPORT_C TPtrC8 CPBAuthData::Salt() const
	{
	return TPtrC8(*iSalt);
	}

EXPORT_C TInt CPBAuthData::Iterations() const
	{
	return iIterations;
	}

EXPORT_C TBool CPBAuthData::operator==(const CPBAuthData& aAuth) const
	{
	//if the key's are equal, the its true, as the other members are used in key derivation
	return (*iAuthKey == aAuth.Key());
	}

