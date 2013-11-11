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

LOCAL_D const SCnvConversionData::SVariableByteData::SRange asciiVariableByteDataRanges[]=
	{
		{
		0x00,
		0xff,
		0,
		0
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange asciiToUnicodeDataRanges[]=
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
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange unicodeToAsciiDataRanges[]=
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
		}
	};

GLREF_D const SCnvConversionData asciiConversionData=
	{
	SCnvConversionData::EUnspecified,
		{
		ARRAY_LENGTH(asciiVariableByteDataRanges),
		asciiVariableByteDataRanges
		},
		{
		ARRAY_LENGTH(asciiToUnicodeDataRanges),
		asciiToUnicodeDataRanges
		},
		{
		ARRAY_LENGTH(unicodeToAsciiDataRanges),
		unicodeToAsciiDataRanges
		},
	NULL,
	NULL
	};

GLREF_C void IsCharacterSetAscii(TInt& aConfidenceLevel, const TDesC8& aSample)
	{
	// loop through the aSample text checking the range of the character
	// If greater than 127 then it's not ASCII (gotta be harsh!)  ... 
	TInt sampleLength = aSample.Length();
	if (sampleLength == 0)
		{
		aConfidenceLevel = 91;
		return;
		}
	aConfidenceLevel = 100;
	
	
	_LIT8(KAsciiEsc,"\x28\x42");
	_LIT8(KJisRomanEsc,"\x28\x4a");
	_LIT8(KJisCEsc,"\x24\x40");
	_LIT8(KJisX0208Esc,"\x24\x42");
	_LIT8(KJisX0212Esc,"\x24\x28\x44");
	_LIT8(KHz1Esc,"\x7e\x7e");
	_LIT8(KHz2Esc,"\x7e\x7b");
	_LIT8(KHz3Esc,"\x7e\x7b");
	
	TInt asciiResult = 0;
	TInt jisRomanResult = 0; 
	TInt jisCResult = 0;
	TInt jisX0208Result =0;
	TInt jisX0212Result =0;
	TInt hz1Result=0;
	TInt hz2Result=0;
	TInt hz3Result=0;
	
	TInt escSequences = 0; 
	TInt controls = 0;


	for (TInt i = 0; i < sampleLength; ++i)
		{
		if ((aSample[i]&0x80)!=0x00)
			{
			aConfidenceLevel = 0;
			break;
			}

		if (i > asciiResult)
			{
			asciiResult=(aSample.Right(sampleLength-i)).Find(KAsciiEsc);
			if (asciiResult!=KErrNotFound) //aConfidenceLevel-=2;
				escSequences += 2;
			}

		if (i > jisRomanResult)
			{
			jisRomanResult=(aSample.Right(sampleLength-i)).Find(KJisRomanEsc);
			if (jisRomanResult!=KErrNotFound) //aConfidenceLevel-=2;
				escSequences += 2;
			}

		if (i > jisCResult)
			{
			jisCResult=(aSample.Right(sampleLength-i)).Find(KJisCEsc);
			if (jisCResult!=KErrNotFound) //aConfidenceLevel-=2;
				escSequences += 2;
			}

		if (i > jisX0208Result)
			{
			jisX0208Result=(aSample.Right(sampleLength-i)).Find(KJisX0208Esc);
			if (jisX0208Result!=KErrNotFound) //aConfidenceLevel-=2;
				escSequences += 2;
			}

		if (i > jisX0212Result)
			{
			jisX0212Result=(aSample.Right(sampleLength-i)).Find(KJisX0212Esc);
			if (jisX0212Result!=KErrNotFound) //aConfidenceLevel-=2;
				escSequences += 2;
			}

		if (i > hz1Result)
			{
			hz1Result=(aSample.Right(sampleLength-i)).Find(KHz1Esc);
			if (hz1Result!=KErrNotFound) //aConfidenceLevel-=2;
				escSequences += 2;
			}

		if (i > hz2Result)
			{
			hz2Result=(aSample.Right(sampleLength-i)).Find(KHz2Esc);
			if (hz2Result!=KErrNotFound) //aConfidenceLevel-=2;
				escSequences += 2;
			}

		if (i > hz3Result)
			{
			hz3Result=(aSample.Right(sampleLength-i)).Find(KHz3Esc);
			if (hz3Result!=KErrNotFound) //aConfidenceLevel-=2;
				escSequences += 2;
			}

		if (aSample[i]==0x7f)
			// 0x7f is the control code for delete ... 
			{
			aConfidenceLevel = 0;
			break;
			}

		 if (aSample[i]==0x1b)
			{
			static const TInt smsExtensionTable[12] = 
				{0x0a, 0x14, 0x1b, 0x24, 0x28, 0x29, 0x2f, 0x3c, 0x3d, 0x3e, 0x40, 0x65};
			for (TInt j=0; j < 12; ++j) // change the hard coded number to the Array length 
				{
				TInt increment1 = i+1;
				if (increment1 >= sampleLength)
					break;
				if (aSample[increment1] == smsExtensionTable[j])
					{
					aConfidenceLevel-=5;
					// /x1b/x24 & /x1b/x28 are the first two characters of a few
					// of  JIS & ISO2022JP escape sequence (That's why 0x24 was added in smsExtensionTable)
					// So if what's up next matches any JIS & IS02022JP escape sequence..... more deduction 
					// of the confidence Level 
					TInt increment2 = i+2;
					TInt increment3 = i+3;
					if((increment2 >= sampleLength)||((increment3) >= sampleLength))
						break;
					if (smsExtensionTable[j]==0x24)
						{
						// 24 -> 40,42 (28,44)
						if ((aSample[increment2]==0x40) || (aSample[increment2]==0x42) ||
							((aSample[increment2]==0x28)&&(aSample[increment3]==0x44)))
							{
							aConfidenceLevel=0;
							break;
							}
						}
					else if (smsExtensionTable[j]==0x28)
						{
						// 28 -> 42, 49, 4a
						if ((aSample[increment2]==0x42) || (aSample[increment2]==0x49) || (aSample[increment2]==0x4a))
							{
							aConfidenceLevel=0;
							break;
							}
						}
					}
				}
			if(aConfidenceLevel==0)
				break;
			}
			if (controls < 100 && aSample[i] < 0x20 && 	aSample[i] != '\r' && aSample[i] != '\n' && aSample[i] != '\t')
			// a few more control codes besides LF, CR, TAB 
			{
			controls ? controls *= 3 : controls = 3; 
			}
		}
		
	aConfidenceLevel -= controls;
	aConfidenceLevel = aConfidenceLevel - ((escSequences*100)/sampleLength);
	aConfidenceLevel =(aConfidenceLevel >0)? aConfidenceLevel: 0;


	}


