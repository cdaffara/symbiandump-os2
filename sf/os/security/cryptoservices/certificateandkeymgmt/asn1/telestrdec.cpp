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
* This file contains the implementation of the teletex string ASN1 object.
* The teletex string includes the following characters:
* <character set currently unknown - awaiting ITU spec>
*
*/


#include <asn1dec.h>

EXPORT_C TASN1DecTeletexString::TASN1DecTeletexString(void)
	{
	}

EXPORT_C TInt TASN1DecTeletexString::CheckValid(const TDesC8& /*aSource*/)

	{
	return KErrNone;
	}

/**
Decodes an ASN.1 format Teletex string.
@param aSource ASN.1 Teletex string to be decoded.
@param aPos Offset in aSource from where decoding starts.
@return On return, a pointer to a buffer containing the raw data converted to 16bits.
*/
EXPORT_C HBufC* TASN1DecTeletexString::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	HBufC* res = DecodeContentsL(gen.GetContentDER());
	return res;
	}

/**
Decodes an ASN.1 format Teletex string.
@param aSource ASN.1 Teletex string to be decoded.
@return On return, a pointer to a buffer containing the raw data converted to 16bits.
*/
EXPORT_C HBufC* TASN1DecTeletexString::DecodeDERL(const TASN1DecGeneric& aSource)

	{	
	return DecodeContentsL(aSource.GetContentDER());
	}

HBufC* TASN1DecTeletexString::DecodeContentsL(const TDesC8& aSource)
	{
	HBufC* res = HBufC::NewL(aSource.Length());
	TPtr pRes = res->Des();
	pRes.Copy(aSource);
	return res;
	}




