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
* This file contains the implementation of the visible string ASN1 object.
* The visible string can contain the following characters:
* <character set currently unknown, awaiting arrival of ITU specs>
*
*/


#include <asn1dec.h>

EXPORT_C TASN1DecVisibleString::TASN1DecVisibleString(void)
	{
	}

EXPORT_C TInt TASN1DecVisibleString::CheckValid(const TDesC8& /*aSource*/)
	{
	return KErrNone;
	}

EXPORT_C HBufC* TASN1DecVisibleString::DecodeDERL(const TDesC8& aSource,TInt& aPos)
	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	HBufC* res = DecodeContentsL(gen.GetContentDER());
	aPos+=gen.LengthDER();
	return res;
	}

HBufC* TASN1DecVisibleString::DecodeContentsL(const TDesC8& aSource)
	{
	HBufC* res = HBufC::NewL(aSource.Length());
	TPtr pRes = res->Des();
	pRes.Copy(aSource);
	return res;
	}
