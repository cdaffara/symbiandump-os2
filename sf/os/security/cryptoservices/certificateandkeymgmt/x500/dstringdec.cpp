/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains the implementation of the directory string ASN1 object
*
*/


#include "X500dec.h"

/*

  This file contains the implementation of the following ASN1 data structures:

	DirectoryString ::= CHOICE {
		  teletexString             TeletexString (SIZE (1..MAX)),
		  printableString           PrintableString (SIZE (1..MAX)),
		  universalString           UniversalString (SIZE (1..MAX)),
		  utf8String              UTF8String (SIZE (1..MAX)),
		  bmpString               BMPString (SIZE(1..MAX))   }

*/

TASN1DecX500DirectoryString::TASN1DecX500DirectoryString(void):
iMaxLength(KMaxTInt)
	{
	}

HBufC* TASN1DecX500DirectoryString::DecodeDERL(const TDesC8& aSource,TInt& aPos, TInt aMaxLength)

	{
	TPtrC8 Ptr=aSource.Right(aSource.Length()-aPos);
	TASN1DecGeneric gen(Ptr);
	gen.InitL();
	aPos+=gen.LengthDER();
	HBufC* Object=DecodeContentsL(gen, aMaxLength);
	return Object;
	}

HBufC* TASN1DecX500DirectoryString::DecodeDERL(const TASN1DecGeneric& aSource, TInt aMaxLength)

	{
	return DecodeContentsL(aSource, aMaxLength);
	}

HBufC* TASN1DecX500DirectoryString::DecodeContentsL(const TASN1DecGeneric& aSource, TInt aMaxLength)
	{
	HBufC* res = NULL;
	switch (aSource.Tag())
		{
		case EASN1TeletexString:
			{
			TASN1DecTeletexString tStr;
			res = tStr.DecodeDERL(aSource);
			break;
			}
		case EASN1PrintableString:
			{
			TASN1DecPrintableString pStr;
			res = pStr.DecodeDERL(aSource);
			break;
			}
		case EASN1IA5String:
			{
			TASN1DecIA5String pStr;
			res = pStr.DecodeDERL(aSource);
			break;
			}
		case EASN1UTF8String:
			{
			TASN1DecUTF8String pStr;
			res = pStr.DecodeDERL(aSource);
			break;
			}												
		case EASN1BMPString:
			{
			TASN1DecBMPString pStr;
			res = pStr.DecodeDERL(aSource);
			break;	
			}									
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}
	if (res->Length() > aMaxLength)
		{
		delete res;
		User::Leave(KErrArgument);
		}
	return res;
	}

void TASN1DecX500DirectoryString::SetMaxLength(TInt aLength)
// perhaps this should have some kind of testing to make sure the current 
// contents are valid...this is not as easy as it might first appear, the 
// contents might be eight or sixteen bits.
	{
	iMaxLength=aLength;
	}
