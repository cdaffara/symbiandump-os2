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
#include <charconv.h>
#include "big5.h"
#include <ecom/implementationproxy.h>
#include <charactersetconverter.h>

class CBIG5ConverterImpl : public CCharacterSetConverterPluginInterface
	{

public:
	virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();

	virtual TInt ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters);

	virtual TInt ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);

	virtual TBool IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample);

	static CBIG5ConverterImpl* NewL();
	virtual ~CBIG5ConverterImpl();

private:
	CBIG5ConverterImpl();

	};


const TDesC8& CBIG5ConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return CnvBig5::ReplacementForUnconvertibleUnicodeCharacters();
	}

TInt CBIG5ConverterImpl::ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	return CCnvCharacterSetConverter::DoConvertFromUnicode(CnvBig5::ConversionData(), aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters);
	}

TInt CBIG5ConverterImpl::ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& /*aState*/, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	return CCnvCharacterSetConverter::DoConvertToUnicode(CnvBig5::ConversionData(), aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	}

TBool CBIG5ConverterImpl::IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample)
	{
	aSetToTrue=ETrue;
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
		TUint charBig5;
		TInt weight;
		};

	referenceChar refBig5[20];
	static const TInt iniWeight[20]=
		{
		//occurence per 1000 chars
		30,20,20,10,10,10,10,10,5,5,
		5,5,5,5,5,5,5,5,5,5
		};

	static const TUint iniChar[20]=
		{
		0xa141,0xaaba,0xa446,0xadd3,0xa4a3,0xa7e2,0xa440,0xac4f,0xad6e,0xa45d,
		0xa4d1,0xa457,0xa457,0xa94d,0xa4a4,0xa569,0xa662,0xa470,0xa448,0xa455
		};

	for (TInt k=0; k<20; k++)
		{
		refBig5[k].charBig5=iniChar[k];
		refBig5[k].weight=iniWeight[k];
		totalWeight=totalWeight+iniWeight[k];
		}
	//WBB
	for (TInt i = 0; i < sampleLength; ++i)
		{
		// Big 5 encoding first byte range 0xA1-0xFE 
		//                second byte range 0x40-0x7E  0xA1-0xFE
		if((aSample[i] >= 0xa1) && (aSample[i] <= 0xfe))
			{
			TInt increment1 = i+1;
			if (increment1 >= sampleLength)
				break;
			if(((aSample[increment1] >= 0x40) && (aSample[increment1] <= 0x7e)) ||
				((aSample[increment1] >= 0xa1) && (aSample[increment1] <= 0xfe)))
				{
				TUint charBig5=(aSample[i]<<8)|(aSample[increment1]);
				if (charBig5>=0xc6a1)//Kanas start and rare chars follow after 
					sumOutChar++;
				TInt j;
				for (j=0; j<20; j++)
					{
					if (charBig5==refBig5[j].charBig5)
						{
						sumOfWeight=sumOfWeight+refBig5[j].weight;
						break;
						}
					}
				sumOfGoodChar++;
				i++;
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

	if (sumOfGoodChar)
		{
		aConfidenceLevel=sumOfGoodChar*100/(sumOfBadSecondByte+sumOfGoodChar+sumOfBadSingleByte);
		aConfidenceLevel=aConfidenceLevel-Max(0,((totalWeight-sumOfWeight)*sumOfGoodChar/1000));//against frequent chars 
		aConfidenceLevel=aConfidenceLevel-sumOutChar*100/sumOfGoodChar;//against gap
		aConfidenceLevel=(aConfidenceLevel < 0)?0:aConfidenceLevel;
		}
	else
		aConfidenceLevel=0;
	return ETrue;
	}

CBIG5ConverterImpl* CBIG5ConverterImpl::NewL()
	{
	CBIG5ConverterImpl* self = new(ELeave) CBIG5ConverterImpl();
	return self;
	}

CBIG5ConverterImpl::~CBIG5ConverterImpl()
	{
	}

CBIG5ConverterImpl::CBIG5ConverterImpl()
	{
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10000FBF,CBIG5ConverterImpl::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	
