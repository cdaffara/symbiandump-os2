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

LOCAL_D const TUint16 keyedTables16OfIndexedTables16_indexedEntries_codePage1252ToUnicode_1[]=
	{
	0x201a,
	0x0192,
	0x201e,
	0x2026,
	0x2020,
	0x2021,
	0x02c6,
	0x2030,
	0x0160,
	0x2039,
	0x0152
	};

LOCAL_D const TUint16 keyedTables16OfIndexedTables16_indexedEntries_codePage1252ToUnicode_2[]=
	{
	0x2018,
	0x2019,
	0x201c,
	0x201d,
	0x2022,
	0x2013,
	0x2014,
	0x02dc,
	0x2122,
	0x0161,
	0x203a,
	0x0153
	};

LOCAL_D const TUint16 keyedTables16OfIndexedTables16_indexedEntries_codePage1252ToUnicode_3[]=
	{
	0x017e,
	0x0178
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable16OfIndexedTables16::SKeyedEntry keyedTables16OfIndexedTables16_keyedEntries_codePage1252ToUnicode_1[]=
	{
		{
		0x82,
		0x8c,
		keyedTables16OfIndexedTables16_indexedEntries_codePage1252ToUnicode_1
		},
		{
		0x91,
		0x9c,
		keyedTables16OfIndexedTables16_indexedEntries_codePage1252ToUnicode_2
		},
		{
		0x9e,
		0x9f,
		keyedTables16OfIndexedTables16_indexedEntries_codePage1252ToUnicode_3
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToCodePage1252_1[]=
	{
		{
		0x0152,
		0x8c
		},
		{
		0x0153,
		0x9c
		},
		{
		0x0160,
		0x8a
		},
		{
		0x0161,
		0x9a
		},
		{
		0x0178,
		0x9f
		},
		{
		0x017d,
		0x8e
		},
		{
		0x017e,
		0x9e
		},
		{
		0x0192,
		0x83
		},
		{
		0x02c6,
		0x88
		},
		{
		0x02dc,
		0x98
		},
		{
		0x2013,
		0x96
		},
		{
		0x2014,
		0x97
		},
		{
		0x2018,
		0x91
		},
		{
		0x2019,
		0x92
		},
		{
		0x201a,
		0x82
		},
		{
		0x201c,
		0x93
		},
		{
		0x201d,
		0x94
		},
		{
		0x201e,
		0x84
		},
		{
		0x2020,
		0x86
		},
		{
		0x2021,
		0x87
		},
		{
		0x2022,
		0x95
		},
		{
		0x2026,
		0x85
		},
		{
		0x2030,
		0x89
		},
		{
		0x2039,
		0x8b
		},
		{
		0x203a,
		0x9b
		},
		{
		0x20ac,
		0x80
		},
		{
		0x2122,
		0x99
		}
	};

LOCAL_D const SCnvConversionData::SVariableByteData::SRange codePage1252VariableByteDataRanges[]=
	{
		{
		0x00,
		0xff,
		0,
		0
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange codePage1252ToUnicodeDataRanges[]=
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
		0xa0,
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
			STATIC_CAST(TUint, 8236),
			0
			}
		},
		{
		0x8e,
		0x8e,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, 239),
			0
			}
		},
		{
		0x82,
		0x9f,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable16OfIndexedTables16,
		0,
		0,
			{
			UData_SKeyedTable16OfIndexedTables16(keyedTables16OfIndexedTables16_keyedEntries_codePage1252ToUnicode_1)
			}
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange unicodeToCodePage1252DataRanges[]=
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
		0x00a0,
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
		0x0152,
		0x2122,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		1,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToCodePage1252_1)
			}
		}
	};

GLREF_D const SCnvConversionData codePage1252ConversionData=
	{
	SCnvConversionData::EUnspecified,
		{
		ARRAY_LENGTH(codePage1252VariableByteDataRanges),
		codePage1252VariableByteDataRanges
		},
		{
		ARRAY_LENGTH(codePage1252ToUnicodeDataRanges),
		codePage1252ToUnicodeDataRanges
		},
		{
		ARRAY_LENGTH(unicodeToCodePage1252DataRanges),
		unicodeToCodePage1252DataRanges
		},
	NULL,
	NULL
	};

GLREF_C void IsCharacterSetCP1252(TInt& aConfidenceLevel, const TDesC8& aSample)
	{
	aConfidenceLevel = 60;
	TInt sampleLength = aSample.Length();

	for (TInt i=0; i<sampleLength; ++i)
		{
		// CP1252 includes ASCII as well
		// first check if the char is in the range 0x80 - 0x9f (controls codes in ISO88591)
		// If it is in that range then the likelihood that it's CP1252 is a bit higher
		if ((aSample[i] >= 0x80) && (aSample[i] <= 0x9f))
			{
			if((aSample[i]==0x81)||(aSample[i]==0x8D)||(aSample[i]==0x8f)||
				(aSample[i]==0x90)||(aSample[i]==0x9d))
				{
				// These code values are not supported by the Codepage CP1252
				aConfidenceLevel = 0;
				break;
				}
			else
				{
				// problem: UTF8 uses the values 0x80-0x9f in more than 50% of it's multibyte representation
				// so if the text was UTF8 .... the confidence here would hit the roof. Could check to make 
				// sure that this is not UTF8
				aConfidenceLevel+=1;
				}
			}
		TInt increment1 = i+1;
		TInt decrement1 = i-1;
		// 0xf7 is the division symbol in CP1252.
		// 0xd7 is the division symbol in CP1252.If char on either side of the division
		// symbol is a number then the confidence that it's ISO88591 increases
		if( decrement1>= 0 && ((aSample[i]==0xf7) || (aSample[i]==0xd7)) && increment1<sampleLength)
			{
			
			if (increment1 >= sampleLength)
				break;
			if ( (aSample[decrement1] >= 0x30) && (aSample[decrement1] <= 0x39) &&  // char before is a number
				 (aSample[increment1] >= 0x30) && (aSample[increment1] <= 0x39) )   // char after is a number
				{
				aConfidenceLevel+=5;
				}
			}
		// Can also use the currency symbol to increase confidence if the char after a 
		// currency symbol is numeric
		if((aSample[i]>=0xa2) && (aSample[i] <= 0xa5) && increment1<sampleLength)
			{
			if ((aSample[increment1] >= 0x30) && (aSample[increment1] <= 0x39))
				{
				aConfidenceLevel+=5; 
				}
			}
		} // for loop
	aConfidenceLevel =(aConfidenceLevel >0)? ((aConfidenceLevel > 100)? 100: aConfidenceLevel): 0;
	}
