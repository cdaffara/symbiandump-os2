// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "UE_STD.H"

#include <pbe.h>

#define PBEDATA_NO_EXPORTED_CONSTRUCTORS
#include <pbedata.h>





EXPORT_C void RDecryptStream::OpenL(RReadStream& aHost,const CPBEncryptionBase& aKey)
/**
Open a decrypting read stream to aHost.
Prepares the source stream owned by the specified read stream interface object for 
reading through a decrypting filter.
@publishedPartner
@leave KErrNoMemory
@param aHost The read stream interface object for the source stream. 
This function does not take ownership of the source stream.
@param aKey A Password Based Encryption object for encryption handling.
*/
	{
	CPBDecryptor* decryptor = aKey.NewDecryptLC();
	iFilter.SetL(aHost.Source(),decryptor,iFilter.ERead);
	CleanupStack::Pop(decryptor);
	RReadStream::Attach(&iFilter);
	}

EXPORT_C void RDecryptStream::OpenLC(RReadStream& aHost,const CPBEncryptionBase& aKey)
/**
Open a decrypting read stream to aHost.
Prepares the source stream owned by the specified read stream interface object for 
reading through a decrypting filter, and puts a cleanup item onto the cleanup stack.
@publishedPartner
@leave KErrNoMemory
@param aHost The read stream interface object for the source stream. 
This function does not take ownership of the source stream.
@param aKey A Password Based Encryption object for encryption handling.
*/	
{
	OpenL(aHost,aKey);
	PushL();
	}

EXPORT_C void RDecryptStream::AttachL(RReadStream& aHost,const CPBEncryptionBase& aKey)
/**
Attach a decrypting read stream to aHost.
Takes ownership of the source stream owned by the specified read stream interface object, 
and prepares the stream for reading through a decrypting filter.
@publishedPartner
@leave KErrNoMemory
@param aHost The read stream interface object for the source stream.
@param aKey A Password Based Encryption object for encryption handling.
*/	{
	AttachLC(aHost,aKey);
	CleanupStack::Pop();
	}

EXPORT_C void RDecryptStream::AttachLC(RReadStream& aHost,const CPBEncryptionBase& aKey)
/**
Attach a decrypting read stream to aHost.
Takes ownership of the source stream owned by the specified read stream interface object, 
prepares the stream for reading through a decrypting filter, and puts a cleanup item onto the cleanup stack.
@publishedPartner
@leave KErrNoMemory
@param aHost The read stream interface object for the source stream.
@param aKey A Password Based Encryption object for encryption handling.
*/	{
	MStreamBuf* host=aHost.Source();
	aHost=RReadStream();
	RReadStream::Attach(host);		// initial cleanup via this
	PushL();

	CPBDecryptor* decryptor = aKey.NewDecryptLC();
	iFilter.SetL(host,decryptor,iFilter.ERead|iFilter.EAttached);
	CleanupStack::Pop(decryptor);
	
	RReadStream::Attach(&iFilter);
	}

//////////////////////////////////////////////////////////////////////////////





EXPORT_C void REncryptStream::OpenL(RWriteStream& aHost,const CPBEncryptionBase& aKey)
/**
Open an encrypting write stream over aHost.
Prepares the target stream owned by the specified write stream interface object for 
writing through an encrypting filter.
@publishedPartner
@leave KErrNoMemory
@param aHost The write stream interface object for the target stream. The function does not take ownership of the target stream.
@param aKey A Password Based Encryption object for encryption handling.
*/
	{
	CPBEncryptor* encryptor = aKey.NewEncryptLC();
	iFilter.SetL(aHost.Sink(),encryptor,iFilter.EWrite);
	CleanupStack::Pop(encryptor);
	RWriteStream::Attach(&iFilter);
    }

EXPORT_C void REncryptStream::OpenLC(RWriteStream& aHost,const CPBEncryptionBase& aKey)
/**
Open an encrypting write stream over aHost.
Prepares the target stream owned by the specified write stream interface object for 
writing through an encrypting filter and puts a cleanup item onto the cleanup stack.
@publishedPartner
@leave KErrNoMemory
@param aHost The write stream interface object for the target stream. The function does not take ownership of the target stream.
@param aKey A Password Based Encryption object for encryption handling.
*/
	{
	OpenL(aHost,aKey);
	PushL();
	}

EXPORT_C void REncryptStream::AttachL(RWriteStream& aHost,const CPBEncryptionBase& aKey)
/**
Attach an encrypting write stream to aHost.
Takes ownership of the target stream owned by the specified write stream interface object, 
and prepares the stream for writing through an encrypting filter.
@publishedPartner
@leave KErrNoMemory
@param aHost The write stream interface object for the target stream
@param aKey A Password Based Encryption object for encryption handling.
*/
	{
	AttachLC(aHost,aKey);
	CleanupStack::Pop();
	}

EXPORT_C void REncryptStream::AttachLC(RWriteStream& aHost,const CPBEncryptionBase& aKey)
/**
Attach an encrypting write stream to aHost.
Takes ownership of the target stream owned by the specified write stream interface object, 
prepares the stream for writing through an encrypting filter, and puts a cleanup item onto the cleanup stack.
@publishedPartner
@leave KErrNoMemory
@param aHost The write stream interface object for the target stream
@param aKey A Password Based Encryption object for encryption handling.
*/
	{
	MStreamBuf* host=aHost.Sink();
	aHost=RWriteStream();
	RWriteStream::Attach(host);		// initial cleanup via this
	PushL();

	CPBEncryptor* encryptor = aKey.NewEncryptLC();
	iFilter.SetL(host,encryptor,iFilter.EWrite|iFilter.EAttached);
	CleanupStack::Pop(encryptor);
	
	RWriteStream::Attach(&iFilter);
	}

/////////////////////////////////////////////////////////////////////////
// PBE data methods that depend on store, prevents static dependency
/////////////////////////////////////////////////////////////////////////

// CPBEncryptionData

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/security/crypto/source/pbe/pbedata.cpp
CPBEncryptionData::CPBEncryptionData(void)
	{
	}

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/security/crypto/source/pbe/pbedata.cpp
CPBEncryptionData::~CPBEncryptionData(void)
	{
	delete iParms;
	delete iAuth;
	}

EXPORT_C CPBEncryptionData* CPBEncryptionData::NewL(RReadStream& aStream)
	{
	CPBEncryptionData* self = NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBEncryptionData* CPBEncryptionData::NewLC(RReadStream& aStream)
	{
	CPBEncryptionData* self = new(ELeave)CPBEncryptionData();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

void CPBEncryptionData::ConstructL(RReadStream& aStream)
	{
	iAuth = CPBAuthData::NewL(aStream);
	iParms = CPBEncryptParms::NewL(aStream);
	}

EXPORT_C void CPBEncryptionData::ExternalizeL(RWriteStream& aStream) const
	{
	iAuth->ExternalizeL(aStream);
	iParms->ExternalizeL(aStream);
	}

// CPBEncryptParms

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/security/crypto/source/pbe/pbedata.cpp
CPBEncryptParms::CPBEncryptParms()
	{
	}

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/security/crypto/source/pbe/pbedata.cpp
CPBEncryptParms::~CPBEncryptParms()
	{
#ifdef SYMBIAN_PKCS12
	delete iData;
#endif	// #ifdef SYMBIAN_PKCS12
	delete iSalt;
	delete iIV;
	}

EXPORT_C CPBEncryptParms* CPBEncryptParms::NewL(RReadStream& aStream)
	{
	CPBEncryptParms* self = NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBEncryptParms* CPBEncryptParms::NewLC(RReadStream& aStream)
	{
	CPBEncryptParms* self = new(ELeave)CPBEncryptParms();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

#ifdef SYMBIAN_PKCS12

void CPBEncryptParms::ConstructL(RReadStream& aStream) 
	{
	iData = new(ELeave) TParamsData;
	
	TCardinality cipher;
	aStream >> cipher;
	TInt32 cipherInt32 = (TInt32) cipher;
	iData->iCipher = (TPBECipher)(cipherInt32 & ~KBit16);
	
	// if bit 16 of the stored cipher is set, then a KDF
	// follows.  (This preserves data compatability.)
	if ((cipherInt32 & KBit16) == 0)
		iData->iKdf = EKdfPkcs5;
	else
		{
		TInt32 kdf;
		aStream >> kdf;
		iData->iKdf = (TKdf) kdf;
		}

	iSalt = HBufC8::NewL(aStream, KMaxTInt);

	TCardinality iterations;
	aStream >> iterations;
	iIterations = iterations;

	iIV = HBufC8::NewL(aStream, KMaxTInt);
	}

#else

void CPBEncryptParms::ConstructL(RReadStream& aStream) 
	{
	TCardinality cipher;
	aStream >> cipher;
	iCipher = (TPBECipher)((TUint)(cipher));

	iSalt = HBufC8::NewL(aStream, KMaxTInt);

	TCardinality iterations;
	aStream >> iterations;
	iIterations = iterations;

	iIV = HBufC8::NewL(aStream, KMaxTInt);
	}

#endif	// #else #ifdef SYMBIAN_PKCS12

#ifdef SYMBIAN_PKCS12

EXPORT_C void CPBEncryptParms::ExternalizeL(RWriteStream& aStream) const
	{
	TUint32 cipherInt32 = iData->iCipher;
	if (iData->iKdf != EKdfPkcs5)
		cipherInt32 |= KBit16;
	aStream << TCardinality(cipherInt32);
	
	if (iData->iKdf != EKdfPkcs5)
		aStream << (TInt32) iData->iKdf;
	
	aStream << *iSalt;
	aStream << TCardinality(iIterations);
	aStream << *iIV;
	}

#else

EXPORT_C void CPBEncryptParms::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << TCardinality((TUint)iCipher);
	aStream << *iSalt;
	aStream << TCardinality(iIterations);
	aStream << *iIV;
	}

#endif	// #else #ifdef SYMBIAN_PKCS12

// CPBAuthData

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/security/crypto/source/pbe/pbedata.cpp
CPBAuthData::CPBAuthData()
	{
	}

// HPRE-5TDFK2: Remove Store/estor.dll dependency on Cryptography/pbe.dll
// This method is DUPLICATED in common/generic/security/crypto/source/pbe/pbedata.cpp
CPBAuthData::~CPBAuthData()
	{
	delete iAuthKey;
	delete iSalt;
	}

EXPORT_C CPBAuthData* CPBAuthData::NewL(RReadStream& aStream)
	{
	CPBAuthData* self = NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPBAuthData* CPBAuthData::NewLC(RReadStream& aStream)
	{
	CPBAuthData* self = new(ELeave)CPBAuthData();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

void CPBAuthData::ConstructL(RReadStream& aStream)
	{
	iAuthKey = HBufC8::NewL(aStream, KMaxTInt);
	iSalt = HBufC8::NewL(aStream, KMaxTInt);
	TCardinality iterations;
	aStream >> iterations;
	iIterations = iterations;
	}

EXPORT_C void CPBAuthData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iAuthKey;
	aStream << *iSalt;
	aStream << TCardinality(iIterations);
	}
