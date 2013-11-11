/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation for TASN1EncBase128 class.
*
*/


#include "base128enc.h"


TASN1EncBase128DER::TASN1EncBase128DER(TUint aInt) : iInt(aInt), iLengthDER(0)
	{
	CalculateLengthDER();
	}


TUint TASN1EncBase128DER::LengthDER() const
	{
	return iLengthDER;
	}


void TASN1EncBase128DER::CalculateLengthDER()
	{
	iLengthDER = 1;
	TUint working = iInt;
	while (working >>= 7)
		{
		++iLengthDER;
		}
	}


void TASN1EncBase128DER::WriteDERL(TDes8& aBuf, TUint& aPos) const
	{
	__ASSERT_DEBUG(aBuf.Length() - aPos >= STATIC_CAST(TUint8, iLengthDER), 
		User::Leave(KErrBadDescriptor));

	TInt last = aPos + iLengthDER - 1;
	TUint working = iInt;
	for (TUint cursor = last; cursor >= aPos; --cursor)
		{
		// Cast takes least significant 8 bits only (actually, we only need 7)
		aBuf[cursor] = STATIC_CAST(TUint8, working);
		aBuf[cursor] |= 0x80; // Top bit always set to 1...
		working >>= 7;
		}
	// ...except top bit on last byte is 0
	aBuf[last] &= 0x7F;

	aPos += iLengthDER;
	}
