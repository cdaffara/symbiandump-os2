/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <convgeneratedcpp.h>
#include "gb2312.h"

struct SCnvConversionData;

EXPORT_C const TDesC8& CnvGb2312::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return ReplacementForUnconvertibleUnicodeCharacters_internal();
	}

EXPORT_C const SCnvConversionData& CnvGb2312::ConversionData()
	{
	return conversionData;
	}

EXPORT_C TBool CnvGb2312::IsCharGBBased(TInt& aConfidenceLevel, const TDesC8& aSample)
	{
	TInt sampleLength = aSample.Length();
	aConfidenceLevel = 0;
	//WBB the following is for distiguish between big5 and GBK
	TInt totalWeight=0;		//sum of the weights of 20 most frequent chars
	TInt sumOfGoodChar=0;		//the number of chars whose first byte and second are both in the range
	TInt sumOfWeight=0;		//sum of the weights of the chars which are included in the sample
	TInt sumOutChar=0;		//the number of chars which are not common
	TInt sumOfBadSecondByte=0;//the number of chars whose first byte is in the range but not the second
	TInt sumOfBadSingleByte=0;	//the number of bad single byte, which is not in valid range
	struct referenceChar
		{
		TUint charGBK;
		TInt weight;
		};

	referenceChar refGbk[20];
	static const TInt iniWeight[20]=
		{
		//occurence per 1000 chars
		30,20,20,10,10,10,10,10,5,5,
		5,5,5,5,5,5,5,5,5,5
		};

	static const TUint iniChar[20]=
		{
		0xa3ac,0xb5c4,0xc1cb,0xb8f6,0xb2bb,0xb0d1,0xd2bb,0xcac7,0xd2aa,0xbecd,
		0xd2b2,0xccec,0xc9cf,0xbacd,0xd6d0,0xd4da,0xd0a1,0xc8cb,0xcfc2,0xd6d0,
		};

	for (TInt k=0; k<20; k++)
		{
		refGbk[k].charGBK=iniChar[k];
		refGbk[k].weight=iniWeight[k];
		totalWeight=totalWeight+iniWeight[k];
		}

	
	//WBB
	for (TInt i = 0; i < sampleLength; ++i)
		{
		//GBK encoding first byte range 0x81-0xfe
		//              second byte range 0x40-0x7e, 0x80-0xfe
		if((aSample[i] >= 0x81) && (aSample[i] <= 0xfe))
			{
			TInt increment1 = i+1;
			if (increment1 >= sampleLength)
				break;
			if (((aSample[increment1] >=0x40) && (aSample[increment1] <= 0x7e)) ||
				((aSample[increment1] >=0x80) && (aSample[increment1] <= 0xfe)))
				{
				//WBB
				TUint charGbk=(aSample[i]<<8)|(aSample[increment1]);
				TInt j;
				for (j=0; j<20; j++)
					{
					if (charGbk==refGbk[j].charGBK)
						{
						sumOfWeight=sumOfWeight+refGbk[j].weight;
						break;
						}
					}
				if ((aSample[i]>=0xa4)&&(aSample[i]<=0xaf))
					sumOutChar++;
				sumOfGoodChar++;
				i++;
				//WBB
				}
			else
				{
				sumOfBadSecondByte++;				
				}
			}
		// if seldom used characters
		else if (aSample[i] < 0x20 || aSample[i] > 0x7F ) 
			{
			if (aSample[i]!=0x09 && aSample[i]!=0x0A && aSample[i]!=0x0D)
				sumOfBadSingleByte++;
			}
		} // for 

	TInt limit;
	limit = (10*sampleLength)/100;
	if (sumOfGoodChar > limit)
		{
		aConfidenceLevel=sumOfGoodChar*100/(sumOfBadSecondByte+sumOfGoodChar+sumOfBadSingleByte);
		aConfidenceLevel=aConfidenceLevel-Max(0,((totalWeight-sumOfWeight)*sumOfGoodChar/1000));//against frequent chars 
		aConfidenceLevel=aConfidenceLevel-(sumOutChar*100/sumOfGoodChar);//against gap
		aConfidenceLevel=(aConfidenceLevel < 0)?0:aConfidenceLevel;
		}
	else
		aConfidenceLevel=0;
	return ETrue;
	}
