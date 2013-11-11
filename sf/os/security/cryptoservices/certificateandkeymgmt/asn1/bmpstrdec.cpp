/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains the implementation of the BMPString ASN1 object.
*
*/



#include <asn1dec.h>


EXPORT_C TASN1DecBMPString::TASN1DecBMPString(void)
	{
	}

EXPORT_C TInt TASN1DecBMPString::CheckValid(const TDesC8& /*aString*/)
	{
	return KErrNone;
	}

/**
Decodes an ASN.1 format BMP string.
@param aSource ASN.1 BMP string to be decoded.
@param aPos Offset in aSource from where decoding starts.
@return On return, a pointer to a buffer containing the decoded BMP string.
*/
EXPORT_C HBufC* TASN1DecBMPString::DecodeDERL(const TDesC8& aSource,TInt& aPos)
	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();	
	HBufC* res = DecodeContentsL(gen.GetContentDER());
	return res;
	}

/**
Decodes an ASN.1 format BMP string.
@param aSource ASN.1 BMP string to be decoded.
@return On return, a pointer to a buffer containing the decoded BMP string.
*/
EXPORT_C HBufC* TASN1DecBMPString::DecodeDERL(const TASN1DecGeneric& aSource)
	{
	return DecodeContentsL(aSource.GetContentDER());
	}

HBufC* TASN1DecBMPString::DecodeContentsL(const TDesC8& aSource)
	{
	TInt len = aSource.Length()/2;
	HBufC* res = HBufC::NewL(len);
	const TUint16* data16 = reinterpret_cast<const TUint16 *>(aSource.Ptr()); 
	TPtr pRes = res->Des();
	pRes.Copy(data16, len);
	return res;
	}
