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


#include "arc4.h"
#include "../common/inlines.h"
#include <e32base.h>
#include <cryptostrength.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
/** The size of the substitution box (i.e. lookup table) in bytes. */
const TInt KSBoxSize = 256;
#endif

inline TUint8 CARC4::GenerateByte()
	{
	TUint8 a = iState[ix];
	iy = (TUint8)((iy + a) & 0xff);
	TUint8 b = iState[iy];

	iState[ix] = b;
	iState[iy] = a;
	ix = (TUint8)((ix + 1) & 0xff);
	return (iState[(a + b) & 0xff]);
	}

CARC4::CARC4(const TDesC8& aKey, TUint aDiscardBytes)
	: ix(1), iy(0), iDiscardBytes(aDiscardBytes)
	{
	iKey.Copy(aKey);
	GenerateSBox();
	}

EXPORT_C CARC4* CARC4::NewL(const TDesC8& aKey, TUint aDiscardBytes)
	{
	CARC4* self = NewLC(aKey, aDiscardBytes);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CARC4* CARC4::NewLC(const TDesC8& aKey, TUint aDiscardBytes)
	{
	CARC4* self = new(ELeave)CARC4(aKey, aDiscardBytes);
	CleanupStack::PushL(self);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()));
	return self;
	}

void CARC4::DoProcess(TDes8& aData)
	{
	TInt blockLen = aData.Size();

	if (blockLen > 0)
		{
		TUint8* blockPtr = (TUint8*)&aData[0];	
		do
			{
			*blockPtr++ ^= GenerateByte();
			} 
		while (--blockLen);
		}
	}

void CARC4::Reset()
	{
	ix = 1;
	iy = 0;
	GenerateSBox();
	}

TInt CARC4::KeySize() const
	{
	return (iKey.Size());
	}

void CARC4::DiscardBytes(TInt aDiscardBytes)
	{	
	if (aDiscardBytes > 0)
		{
		do
			{
			GenerateByte();
			}
		while(--aDiscardBytes);
		}
	}

void CARC4::GenerateSBox(void)
	{
	TUint keyBytes = iKey.Size();
		
	TInt i = 0;
	for (; i < KSBoxSize; i++)
		iState[i] = (TUint8)i;
	
	TUint keyIndex = 0, stateIndex = 0;
	i = 0;
	for (; i < KSBoxSize; i++)
		{
		TUint a = iState[i];
		stateIndex += iKey[keyIndex] + a;
		stateIndex &= 0xff;
		iState[i] = iState[stateIndex];
		iState[stateIndex] = (TUint8)a;
		if (++keyIndex >= (TUint)keyBytes)
			keyIndex = 0;
		}

	DiscardBytes(iDiscardBytes);
	}
