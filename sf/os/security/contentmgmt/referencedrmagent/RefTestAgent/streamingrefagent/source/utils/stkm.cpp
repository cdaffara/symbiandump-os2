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
* srautils.h
* Defines STKM object which carries short term TEKs for DRM profile.
*
*/


#include "stkm.h"

CStkm::CStkm()
//Constructor
	{
	//empty
	}

EXPORT_C CStkm::~CStkm()
//Destructor
	{
	delete iEncryptionKey;
	delete iAuthenticationKey;
	}


EXPORT_C CStkm* CStkm::NewLC(RReadStream& aStream)
	{
	CStkm* self = new (ELeave) CStkm();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}


EXPORT_C CStkm* CStkm::NewL(RReadStream& aStream)
	{
	CStkm* self = CStkm::NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CStkm* CStkm::NewLC(TUint16 aFlags, TUint32 aSpi, const TDesC8& aEncryptionKey, const TDesC8& aAuthenticationKey)
	{
	CStkm* self = new (ELeave) CStkm();
	CleanupStack::PushL(self);
	self->ConstructL(aFlags, aSpi, aEncryptionKey, aAuthenticationKey);
	return self;
	}

EXPORT_C CStkm* CStkm::NewL(TUint16 aFlags, TUint32 aSpi, const TDesC8& aEncryptionKey, const TDesC8& aAuthenticationKey)
	{
	CStkm* self = CStkm::NewLC(aFlags, aSpi, aEncryptionKey, aAuthenticationKey);
	CleanupStack::Pop(self);
	return self;
	}


void CStkm::InternalizeL(RReadStream& aStream)
	{
	iFlags = aStream.ReadUint16L();
	
	TInt protocolValue = (iFlags & TKM_ALGO_FILTER) >> 5;
	switch(protocolValue)
		{
		case TKM_ALGO_IPSEC:
			{
			// The algorithm is IPSec. The next expected field is SPI.
			iSpi = aStream.ReadUint32L();
			iEncryptedKeyLength = aStream.ReadUint8L();
			
			if(iEncryptedKeyLength < KMinKeyMaterialLength || iEncryptedKeyLength > KMaxKeyMaterialLength)
				{
				// Encrypted key material is not in the expected range. The message might have been corrupted.
				User::Leave(KErrCorrupt);
				}
			
			// The IPSec encryption key is identical to the first 16 bytes.
			HBufC8* tmp = HBufC8::NewLC(iEncryptedKeyLength);
			TPtr8 ptr(tmp->Des());
			aStream.ReadL(ptr, iEncryptedKeyLength);
			
			iEncryptionKey = HBufC8::New(KIpsecEncryptionKeyLength);
			TPtr8 ptrEk(iEncryptionKey->Des());
			ptrEk.Copy(ptr.Ptr(), KIpsecEncryptionKeyLength);
			
			// if traffic authentication is used, the IPSec authentication key is obatined from the remaining bytes.
			if(iEncryptedKeyLength > KIpsecEncryptionKeyLength)
				{
				iAuthenticationKey = HBufC8::New(iEncryptedKeyLength - KIpsecEncryptionKeyLength);
				TPtr8 ptrAk(iAuthenticationKey->Des());
				ptrAk.Copy(ptr.Ptr() + KIpsecEncryptionKeyLength, iEncryptedKeyLength - KIpsecEncryptionKeyLength);
				}
			
			CleanupStack::PopAndDestroy(tmp);
			break;
			}
		case TKM_ALGO_SRTP:
		case TKM_ALGO_ISMACRYP:
			// Neither SRTP nor ISMACryp are supported by the reference agent.
			User::Leave(KErrNotSupported);
		}
	}


void CStkm::ConstructL(TUint16 aFlags, TUint32 aSpi, const TDesC8& aEncryptionKey, const TDesC8& aAuthenticationKey)
	{
	iFlags = aFlags;
	iSpi = aSpi;
	iEncryptionKey = aEncryptionKey.AllocL();
	iAuthenticationKey = aAuthenticationKey.AllocL();
	iEncryptedKeyLength = iEncryptionKey->Length() + iAuthenticationKey->Length();
	}


EXPORT_C TUint32 CStkm::Spi() const
	{
	return iSpi;
	}


EXPORT_C HBufC8* CStkm::EncryptionKey() const
	{
	return iEncryptionKey;
	}

EXPORT_C HBufC8* CStkm::AuthenticationKey() const
	{
	return iAuthenticationKey;
	}

EXPORT_C void CStkm::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint16L(iFlags);
	aStream.WriteUint32L(iSpi);
	aStream.WriteUint8L(iEncryptedKeyLength);
	aStream.WriteL(*iEncryptionKey);
	aStream.WriteL(*iAuthenticationKey);
	}
