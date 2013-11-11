/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Little-Endian converter
*
*/


#include <e32std.h>
#include <convdata.h>
#include "unicode.h"

#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))


GLREF_D const SCnvConversionData unicodeConversionDataLittle=
	{
	SCnvConversionData::EFixedLittleEndian,
		{
		ARRAY_LENGTH(unicodeVariableByteDataRanges),
		unicodeVariableByteDataRanges
		},
		{
		ARRAY_LENGTH(unicodeTounicodeDataRanges),
		unicodeTounicodeDataRanges
		},
		{
		ARRAY_LENGTH(unicodeTounicodeDataRanges),
		unicodeTounicodeDataRanges
		},
	NULL,
	NULL
	};

GLREF_C void IsCharacterSetUnicodeLittle(TInt& aConfidenceLevel, const TDesC8& aSample)
	{
	
	TInt sampleLength = aSample.Length();
	aConfidenceLevel =70;
	if (sampleLength < 2)
		return;

	if (aSample[0]==0xff)
		{
		// The first byte is a possible ByteOrderMark
		// Try matching the next character 
		if(aSample[1]==0xfe)
			{
			// the byte order mark could be 0xFEFF or 0xFFFE depending on 
			// endianness of the sample text.
			aConfidenceLevel=100;
			}
		}

	for (TInt i = 0; i < sampleLength-1; ++i)
		{
		if (aSample[i] == 0x0d)
 			{
 			if (aSample[i+1] == 0x0a)
 				{
 				// reduce the confidence level
 				aConfidenceLevel -= 25;
 				}
 			}
		}

	// if not 100% confident already, check if most odd bytes zero 
	#define MAX_SAMPLE_LENGTH 2048
	if ( aConfidenceLevel < 100 )
		{	
		TInt repeat=0;

		// only check the first MAX_SAMPLE_LENGTH if big sample
		TInt length =( sampleLength > MAX_SAMPLE_LENGTH ? MAX_SAMPLE_LENGTH : sampleLength);

		// start from 1 and check the odd bytes
		for (TInt i = 1; i < length-1; i+=2)
			{
			if (aSample[i] == 0x0) 
				repeat ++;
			}

		// if more than 80% odd bytes zero, then this IS little Endian
		if ( (repeat * 100) /  (length * 5) >= 8)
			aConfidenceLevel  = 100;
		}

	aConfidenceLevel =(aConfidenceLevel >0)? ((aConfidenceLevel > 100)? 100: aConfidenceLevel): 0;
	}

