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
* This file contains the implementation of the ASN1 Numeric String class.
* This character set includes the digits 0,1,...,9 and the character <space>
*
*/


#include <asn1dec.h>

EXPORT_C TASN1DecNumericString::TASN1DecNumericString(void)
	{
	}

EXPORT_C HBufC* TASN1DecNumericString::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	HBufC* res = DecodeContentsL(gen.GetContentDER());
	return res;
	}

HBufC* TASN1DecNumericString::DecodeContentsL(const TDesC8& aSource)
	{
	HBufC* res = HBufC::NewL(aSource.Length());
	TPtr pRes = res->Des();
	pRes.Copy(aSource);
	return res;
	}

TInt TASN1DecNumericString::CheckValid(const TDesC8& aString)

	{
	TInt i;
	for (i=0;i<aString.Length();i++)
		{
		if ((aString[i]<'0')||(aString[i]>'9'))
			{
			if (aString[i]!=' ')
				{
				return KErrArgument;
				}
			}
		}
	return KErrNone;
	}
