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
* This file contains the implementation of the Printable String ASN1 object.
* The characters included in this string type are:
* AB...Zab...z01...9<space>'()+,-./:?
*
*/


#include <asn1dec.h>

const TInt8 PrintableStringValid[256]=
	{
//  0 1 2 3 4 5 6 7 8 9 a b c d e f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 1
	0,0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,  // 2
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,  // 3
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 4
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,  // 5 
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 6
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,  // 7 
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 8
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 9
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // a
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // b
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // c
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // d
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // e
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // f
	};

EXPORT_C TASN1DecPrintableString::TASN1DecPrintableString(void)
	{
	}

TInt TASN1DecPrintableString::CheckValid(const TDesC8& aString)

	{
	TInt i;
	for (i=0;i<aString.Length();i++)
		{
		if (PrintableStringValid[aString[i]]==0)
			{
			return EFalse;
			}
		}
	return ETrue;
	}

EXPORT_C HBufC* TASN1DecPrintableString::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();	
	HBufC* res = DecodeContentsL(gen.GetContentDER());
	return res;
	}

EXPORT_C HBufC* TASN1DecPrintableString::DecodeDERL(const TASN1DecGeneric& aSource)

	{
	return DecodeContentsL(aSource.GetContentDER());
	}

HBufC* TASN1DecPrintableString::DecodeContentsL(const TDesC8& aSource)
	{
	HBufC* res = HBufC::NewL(aSource.Length());
	TPtr pRes = res->Des();
	pRes.Copy(aSource);
	return res;
	}
