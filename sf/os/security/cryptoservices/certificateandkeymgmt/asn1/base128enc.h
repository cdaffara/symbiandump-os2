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
* Declare TASN1EncBase128DER class.
* Declare TASN1EncBase128DER class.  This encodes a given integer base 128,
* most significant digits first, with the top bit set 1 on every
* octet except the last.
* The DER encoding of numbers in base 128 is common both the the encoding
* of the tag number in high-tag-number form, and to the DER encoding of OID terms,
* hence this class to encapsulate this commonality.
*
*/


/**
 @file 
 @internalComponent 
*/

#include <e32std.h>

class TASN1EncBase128DER
	{
public:
	TASN1EncBase128DER(TUint aInt);

	TUint LengthDER() const;
	void WriteDERL(TDes8& aBuf, TUint& aPos) const;

private:
	void CalculateLengthDER();

	TUint iInt;
	TUint iLengthDER;
	};

