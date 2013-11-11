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
* Big-Endian converter
*
*/


#include <e32std.h>
#include <convdata.h>
#include "unicode.h"

#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))


GLREF_D const SCnvConversionData unicodeConversionDataBig=
	{
	SCnvConversionData::EFixedBigEndian,
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

GLREF_C void IsCharacterSetUnicodeBig(TInt& aConfidenceLevel, const TDesC8& aSample)
	{
	
	TInt sampleLength = aSample.Length();
	aConfidenceLevel =70;
	if (sampleLength < 2)
		return;

	if (aSample[0]==0xfe)
		{
		// The first byte is a possible ByteOrderMark
		// Try matching the next character 
		if(aSample[1]==0xff)
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
 				// Reduce the confidence level
 				aConfidenceLevel-= 25;
 				}
 			}
		}

	// if not 100% confident already, check if most even bytes are 0
	#define MAX_SAMPLE_LENGTH 2048
	if ( aConfidenceLevel < 100 )
		{	
		TInt repeat=0;
		// only check the first 2k if big sample
		TInt length =( sampleLength > MAX_SAMPLE_LENGTH ? MAX_SAMPLE_LENGTH : sampleLength);
		
		// start from 0 and check the even bytes
		for (TInt i = 0; i < length-1; i+=2)
			{
			if (aSample[i] == 0x0) 
				repeat ++;
			}

		// if more than 80% even bytes zero then this IS big Endian
		if ( (repeat * 100) /  (length * 5) >= 8)
			aConfidenceLevel  = 100;
		}

	aConfidenceLevel =(aConfidenceLevel >0)? ((aConfidenceLevel > 100)? 100: aConfidenceLevel): 0;
	}

