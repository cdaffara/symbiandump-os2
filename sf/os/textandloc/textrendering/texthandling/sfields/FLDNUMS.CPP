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
#include "FLDSTD.H"

#include "FLDNUMS.H"

LOCAL_D const TInt sDeneryArray[13] = {  1000, 900,  500, 400, 100, 90,  50, 40,  10, 9,   5,  4,   1  };
LOCAL_D const TText* const LsRomanArray[13] = { _S("M"),  _S("CM"), _S("D"), _S("CD"),_S("C"), _S("XC"),_S("L"),_S("XL"),_S("X"),_S("IX"),_S("V"),_S("IV"),_S("I") };

//////////////////////////////
// TDeneryToCharBase
//////////////////////////////

EXPORT_C TInt TDeneryToCharBase::DeneryToChar(TPtr& aValueText,TInt aDenary)const
// if aValueText is big enough to hold the result then the Roman Numeral representation of aDenery is inserted
// else the required size of aValueText is returned
//
	{
	aValueText.SetLength(0);
	TInt numChars = NumChars(aDenary);
	if (aValueText.MaxLength() < numChars)
		return numChars;
	else 
		{
		DoDeneryToChar(aValueText,aDenary);
		return KErrNone;
		}
	}


//////////////////////////////
// TRomanNumeral
//////////////////////////////

EXPORT_C TInt TRomanNumeral::NumChars(TInt aDenery)const
// returns the number of characters in a given "roman number"
//
	{
	TInt numChars=0;
	TInt i=0;

	while (aDenery > 0)
		{
		if (sDeneryArray[i] <= aDenery)
			{
			numChars += _L(sRomanArray[i]).Length();
			aDenery -= sDeneryArray[i];
			}
		else
			i++;
		}

	return numChars;
	}


EXPORT_C void TRomanNumeral::DoDeneryToChar(TPtr& aValueText,TInt aDenery)const
// sets aValueText to the Roman Numeral representation of aDenery
//
	{
	aValueText.SetLength(0);
	TInt i=0;

	while (aDenery > 0)
		{
		if (sDeneryArray[i] <= aDenery)
			{
			aValueText.Append(_L(sRomanArray[i]));
			aDenery -= sDeneryArray[i];
			}
		else
			i++;
		}
	}


/////////////////////////
// TArabicNumeral
/////////////////////////

EXPORT_C TInt TArabicNumeral::NumChars(TInt aNum)const
// returns num digits in an integer (including sign bit if negative)
//
	{
	TInt chars=1;
	if (aNum<0)
		{
		chars++;
		aNum = aNum*(-1);
		}
	while (aNum>=10)
		{
		aNum = aNum/10;
		chars++;
		}
	return chars;
	}


EXPORT_C void TArabicNumeral::DoDeneryToChar(TPtr& aValueText,TInt aDenery)const
	{
	aValueText.Num(aDenery);
	}


/////////////////////////
// TAlphabeticNumeral
/////////////////////////

EXPORT_C TInt TAlphabeticNumeral::NumChars(TInt aNum)const
// returns num digits in an integer (including sign bit if negative)
//
	{
	TInt chars=1;
	if (aNum<0)
		{
		chars++;
		aNum = aNum*(-1);
		}
	while (aNum>26)
		{
		aNum = aNum/26;
		chars++;
		}
	return chars;
	}


EXPORT_C void TAlphabeticNumeral::DoDeneryToChar(TPtr& aValueText,TInt aDenery)const
	{
	TInt digit=0;
	while (aDenery>0)
		{
		digit = aDenery%26; // remainder gives least significant figure
		PropendDigitAsChar(aValueText,digit);
		aDenery -= digit;
		aDenery = aDenery/26;
		}
	}


void TAlphabeticNumeral::PropendDigitAsChar(TPtr& aValueText,TInt aDigit)const
	{
	__ASSERT_ALWAYS(aDigit>0,Panic(ECharOutOfRange));
	__ASSERT_ALWAYS(aDigit<27,Panic(ECharOutOfRange));
	__ASSERT_ALWAYS(aValueText.Length()<aValueText.MaxLength(),Panic(EBufferFull));

	TBuf<1> buf;
	buf.Append(TChar(aDigit+64));
	aValueText.Insert(0,buf); // insert at position zero, ie propend
	}


