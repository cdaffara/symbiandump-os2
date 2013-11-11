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
*
*/


#include <e32std.h>
#include <convdata.h>

#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))

LOCAL_D const SCnvConversionData::SVariableByteData::SRange iso88591VariableByteDataRanges[]=
	{
		{
		0x00,
		0xff,
		0,
		0
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange iso88591ToUnicodeDataRanges[]=
	{
		{
		0x00,
		0x7f,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		0,
		0,
			{
			0,
			0
			}
		},
		{
		0x81,
		0xff,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		0,
		0,
			{
			0,
			0
			}
		},
		{
		0x80,
		0x80,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, 8236)
			}
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange unicodeToIso88591DataRanges[]=
	{
		{
		0x0000,
		0x007f,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		1,
		0,
			{
			0,
			0
			}
		},
		{
		0x0081,
		0x00ff,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		1,
		0,
			{
			0,
			0
			}
		},
		{
		0x20ac,
		0x20ac,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -8236)
			}
		}
	};

GLREF_D const SCnvConversionData iso88591ConversionData=
	{
	SCnvConversionData::EUnspecified,
		{
		ARRAY_LENGTH(iso88591VariableByteDataRanges),
		iso88591VariableByteDataRanges
		},
		{
		ARRAY_LENGTH(iso88591ToUnicodeDataRanges),
		iso88591ToUnicodeDataRanges
		},
		{
		ARRAY_LENGTH(unicodeToIso88591DataRanges),
		unicodeToIso88591DataRanges
		},
	NULL,
	NULL
	};

GLREF_C void IsCharacterSetISO88591(TInt& aConfidenceLevel, const TDesC8& aSample)
	{
	TInt sampleLength = aSample.Length();
	aConfidenceLevel = 75;

	for (TInt i=0; i<sampleLength; ++i)
		{
		// ISO88591 includes ASCII as well
		// first check if the char is in the range 0x80 - 0x9f (controls codes)
		// If it is in that range then it's not ISO88591
		if ((aSample[i] >= 0x80) && (aSample[i] <= 0x9f))
			{
			aConfidenceLevel=0;
			break;
			}
		// 0xf7 is the division symbol in ISO88591.
		// 0xd7 is the division symbol in ISO88591.If char on either side of the division
		// symbol is a number then the confidence that it's ISO88591 increases
		if( i>0 && ((aSample[i]==0xf7) || (aSample[i]==0xd7)) && ((i+1)<sampleLength) )
			{
			if ( (aSample[i-1] >= 0x30) && (aSample[i-1] <= 0x39) &&  // char before is a number
				 (aSample[i+1] >= 0x30) && (aSample[i+1] <= 0x39) )   // char after is a number
				{
				aConfidenceLevel+=5;
				}
			}
		// Can also use the currency symbol to increase confidence if the char after a 
		// currency symbol is numeric
		if((aSample[i]>=0xa2) && (aSample[i] <= 0xa5) && ((i+1)<sampleLength))
			{
			if ((aSample[i+1] >= 0x30) && (aSample[i+1] <= 0x39))
				{
				aConfidenceLevel+=5; 
				}
			}
		} // for loop
	aConfidenceLevel =(aConfidenceLevel >0)? ((aConfidenceLevel > 100)? 100: aConfidenceLevel): 0;
	}
