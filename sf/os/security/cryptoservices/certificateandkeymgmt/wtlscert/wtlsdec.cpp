/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "wtlsdec.h"

//stolen from stdlib
#define WTLS_UNIX_BASE   TTime(MAKE_TINT64(0x00dcddb3,0x0f2f8000))    // 00:00, Jan 1st 1970
const TInt KTimeLength = 4;


TWTLSDecVector::TWTLSDecVector(const TDesC8& aSource, TInt aMinLength, TInt aMaxLength)
	:iEncoding(aSource), iMinLength(aMinLength), iMaxLength(aMaxLength), iLengthOfContents(0), iLengthOfLength(0)
	{
	}

void TWTLSDecVector::InitL()
	{
	if (iMaxLength <= 255)
		{
		iLengthOfLength = 1;
		}
	else
		{
		if (iMaxLength <= 65535)
			{
			iLengthOfLength = 2;
			}
		else
			{
			if (iMaxLength <= 16777215)
				{
				iLengthOfLength = 3;
				}
			else
				{
				iLengthOfLength = 4;
				}
			}
		}
	if (iEncoding.Length() < (iLengthOfLength + iMinLength))
		{
		User::Leave(KErrArgument);
		}
	TWTLSDecUnsignedInteger decLength;
	TInt pos = 0;
	iLengthOfContents = decLength.DecodeShortL(iEncoding, pos, iLengthOfLength);
	if (iEncoding.Length() < (iLengthOfLength + iLengthOfContents))
		{
		User::Leave(KErrArgument);
		}
	}
/*
TPtrC8 TWTLSDecVector::Encoding() const
	{
	return iEncoding.Left(EncodingLength());
	}
*/
TInt TWTLSDecVector::EncodingLength() const
	{
	return iLengthOfLength + iLengthOfContents;
	}
/*	
TPtrC8 TWTLSDecVector::Content() const
	{
	return iEncoding.Mid(iLengthOfLength, iLengthOfContents);
	}

TInt TWTLSDecVector::ContentLength() const
	{
	return iLengthOfContents;
	}
	
TInt TWTLSDecVector::HeaderLength() const
	{
	return iLengthOfLength;
	}
*/
TWTLSDecUnsignedInteger::TWTLSDecUnsignedInteger()
	{
	}

TInt TWTLSDecUnsignedInteger::DecodeShortL(const TDesC8& aSource,TInt& aPos, TInt aLength)
	{
	if (aLength > 4)
		{
		User::Leave(KErrOverflow);
		}
	TInt sourceLength = aSource.Length();
	TInt res=0;
	while (aLength)
		{
		if (sourceLength <= aPos)
			{
			User::Leave(KErrArgument);
			}
		res<<=8;
		res+=aSource[aPos++];
		aLength--;
		}
	return res;
	}

RInteger TWTLSDecUnsignedInteger::DecodeLongL(const TDesC8& aSource,TInt& aPos, TInt aLength)
	{
	if (aSource.Length() < (aPos + aLength))
		{
		User::Leave(KErrArgument);
		}
	const TPtrC8 ptr = aSource.Mid(aPos, aLength);
	aPos += aLength;
	RInteger res = RInteger::NewL(ptr);
	return res;
	}


TWTLSDecTime::TWTLSDecTime()
	{
	}

TTime TWTLSDecTime::DecodeL(const TDesC8& aSource, TInt& aPos)
	{
	if ((aSource.Length() - aPos) < KTimeLength)
		{
		User::Leave(KErrArgument);
		}
	TWTLSDecUnsignedInteger decInt;
	TInt inc = decInt.DecodeShortL(aSource, aPos, 4);
	return WTLS_UNIX_BASE + TTimeIntervalSeconds(inc);
	}
