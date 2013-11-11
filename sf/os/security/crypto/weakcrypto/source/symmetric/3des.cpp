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


#include "3des.h"
#include "../common/inlines.h"
#include "des.inl"
#include <cryptostrength.h>

const TInt K3DESBlockBytes = 8;
const TInt K3DESKeyBytes = 24;
const TInt KDESKeyBytes = 8;

void C3DES::Transform(TDes8& aBlock)
	{
	assert(aBlock.Size() == K3DESBlockBytes);
		
	TUint32 l, r;
//	Split the block into 2 word-sized big endian portions
	GetBlockBigEndian((TUint8*)&aBlock[0], l, r);

	IPerm(l,r);

	DoTransform(l, r, iK1);
	DoTransform(r, l, iK2);
	DoTransform(l, r, iK3);
		
	FPerm(l,r);

//	Put the portions back into the block as little endian
	PutBlockBigEndian((TUint8*)&aBlock[0], r, l);
	}

TInt C3DES::BlockSize() const
	{
	return K3DESBlockBytes;
	}

TInt C3DES::KeySize() const
	{
	return K3DESKeyBytes;
	}

C3DES::C3DES()
	{
	}

void C3DES::ConstructL(const TDesC8& aKey)
	{
	assert(aKey.Size() == K3DESKeyBytes);

	iKey = aKey.AllocL();
	DoSetKey(*iKey);
	}

void C3DES::Reset()
	{
	DoSetKey(*iKey);
	}

/* C3DESEncryptor */

EXPORT_C C3DESEncryptor* C3DESEncryptor::NewL(const TDesC8& aKey)
	{
	C3DESEncryptor* me = C3DESEncryptor::NewLC(aKey);
	CleanupStack::Pop(me);
	return (me);	
	}

EXPORT_C C3DESEncryptor* C3DESEncryptor::NewLC(const TDesC8& aKey)
	{
	C3DESEncryptor* me = new (ELeave) C3DESEncryptor();
	CleanupStack::PushL(me);
	me->ConstructL(aKey);
	// DES only used 7 bits out of every key byte
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()) - aKey.Size());
	return (me);	
	}

void C3DESEncryptor::DoSetKey(const TDesC8& aKey)
	{
	// Encryptor key	
	SetKey(aKey.Mid(0, KDESKeyBytes), iK1);
	// Decryptor key
	SetKey(aKey.Mid(KDESKeyBytes, 2*KDESKeyBytes), iK2);
	ReverseKeySchedule(iK2); // Reverse key schedule order
	// Encryptor key
	SetKey(aKey.Mid(2*KDESKeyBytes), iK3);
	}
 
/* C3DESDecryptor */

EXPORT_C C3DESDecryptor* C3DESDecryptor::NewL(const TDesC8& aKey)
	{
	C3DESDecryptor* me = C3DESDecryptor::NewLC(aKey);
	CleanupStack::Pop(me);
	return (me);
	}

EXPORT_C C3DESDecryptor* C3DESDecryptor::NewLC(const TDesC8& aKey)
	{
	C3DESDecryptor* me = new (ELeave) C3DESDecryptor();
	CleanupStack::PushL(me);
	me->ConstructL(aKey);
	// DES only used 7 bits out of every key byte
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()) - aKey.Size());
	return (me);
	}

void C3DESDecryptor::DoSetKey(const TDesC8& aKey)
	{
	// 3DES decryption, reverse through key
	// Decryptor key
	CDES::SetKey(aKey.Mid(2*KDESKeyBytes), iK1);
	ReverseKeySchedule(iK1);	//	Reverse key schedule order	

	// Encryptor key
	CDES::SetKey(aKey.Mid(KDESKeyBytes, 2*KDESKeyBytes), iK2);
	
	// Decryptor key
	CDES::SetKey(aKey.Mid(0, KDESKeyBytes), iK3);
	ReverseKeySchedule(iK3);	//	Reverse key schedule order
	}

