/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "asn1dec.h"

EXPORT_C TASN1DecGeneric::TASN1DecGeneric(const TDesC8& aSource):
 iEncoding(aSource)
	{
	}

EXPORT_C void TASN1DecGeneric::InitL()

// x209 6.2, 6.3
	{
	if(iEncoding.Length() < KASN1ObjectMinLength)
		{
		User::Leave(KErrArgument);
		}	
	
	TInt i=iEncoding[0];
	TInt b=i>>6;
	iClass=(TASN1Class)b;

	TInt Length=0;
	TInt LengthOfLength=1;
	TInt Pos=0;
	TInt T=0;
	TInt encodingLength = iEncoding.Length();
	// work out tag 
	if ((iEncoding[Pos]&0x1f)==0x1f)
		{
		// tag greater than 30
		Pos++;
		if (encodingLength <= Pos+1)
			{
			User::Leave(KErrArgument);
			}
		while (iEncoding[Pos]>=128)
			{
			T<<=7;
			T+=(TUint8)(iEncoding[Pos]&127);		// this raises warning if TTagType is TUint8
			Pos++;
			if (encodingLength <= Pos+1)
				{
				User::Leave(KErrArgument);
				}
			}
		T<<=7;
		T+=(TUint8)iEncoding[Pos];
		}
	else
		{
		T=(TUint8)(iEncoding[Pos]&0x1f);
		}
	iExplicitTag = T;
	Pos++;

	TUint8 l = iEncoding[Pos];
	if (l > 128)
		{
		// this is a length of length
		if (encodingLength <= Pos+1)
			{
			User::Leave(KErrArgument);
			}
		LengthOfLength=iEncoding[Pos++]&127;
		Length=0;
		while (LengthOfLength)
			{
			if (encodingLength <= Pos+1)
				{
				User::Leave(KErrArgument);
				}
			Length<<=8;
			Length+=iEncoding[Pos++];
			LengthOfLength--;
			}
		}
	if (l == 128)
		{
		//this is constructed, indefinite length: we don't support this
		User::Leave(KErrNotSupported);
		}
	if (l < 128)
		{
		//this is a straight length
		Length = iEncoding[Pos++];
		}
	if (((Length+Pos)>encodingLength) || (Length < 0))
		{
		User::Leave(KErrArgument);
		}
	iStartOfContents = Pos;
	iLength = Length;
	}

EXPORT_C TPtrC8 TASN1DecGeneric::GetContentDER(void) const
	{
	return iEncoding.Mid(iStartOfContents, iLength);
	}

EXPORT_C TInt TASN1DecGeneric::LengthDER(void) const
	{
	return iLength+iStartOfContents;
	}

EXPORT_C TInt TASN1DecGeneric::LengthDERContent(void) const
	{
	return iLength;
	}

EXPORT_C TInt TASN1DecGeneric::LengthDERHeader() const
	{
	return LengthDER() - LengthDERContent();
	}

EXPORT_C TPtrC8 TASN1DecGeneric::Encoding() const
	{
	return iEncoding.Left(LengthDER());
	}

EXPORT_C TTagType TASN1DecGeneric::Tag() const
// we can inline this
	{
	return iExplicitTag;
	}

EXPORT_C TASN1Class TASN1DecGeneric::Class() const
// we can inline this
	{
	return iClass;
	}

