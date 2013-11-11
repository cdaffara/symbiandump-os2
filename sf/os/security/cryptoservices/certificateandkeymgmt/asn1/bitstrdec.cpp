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
* This file contains the object implementing the Bitstring ASN1 object.
* There is no handling for constructed form of Bitstring right now.
*
*/


#include "asn1dec.h"

EXPORT_C TASN1DecBitString::TASN1DecBitString(void)
	{
	}

EXPORT_C HBufC8* TASN1DecBitString::DecodeDERL(const TDesC8& /*aSource*/,TInt& /*aPos*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

EXPORT_C HBufC8* TASN1DecBitString::DecodeDERL(const TASN1DecGeneric& /*aSource*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

EXPORT_C HBufC8* TASN1DecBitString::ExtractOctetStringL(const TDesC8& aSource,TInt& aPos)
	{
	TASN1DecGeneric gen(aSource.Right(aSource.Length() - aPos));
	gen.InitL();
	HBufC8* res = ExtractOctetStringL(gen);
	aPos+=gen.LengthDER();
	return res;
	}

EXPORT_C HBufC8* TASN1DecBitString::ExtractOctetStringL(const TASN1DecGeneric& aSource)
	{
	TPtrC8 encoding = aSource.GetContentDER();
	
	// Special case. The empty bit string. (No padding length octet)
	if (encoding.Length() == 0 )
		{
		return encoding.AllocL();				
		}
	
	// Length must either be 0 for the empty string or >= 2 because
	// of the padding length octet.
	if (encoding.Length() == 1)
		{
		User::Leave(KErrArgument);
		}
		
	TUint8 paddingLength = encoding[0];
	if (paddingLength > 7)
		{
		User::Leave(KErrArgument);
		}
	TPtrC8 pEncoding = encoding.Right(encoding.Length()-1);
	HBufC8* octetStr = pEncoding.AllocL();
	TPtr8 pEncKey = octetStr->Des();

	//stop stupid compiler warning
	TUint8 mask =(TUint8)(255 << paddingLength);
	//stop stupid compiler warning
	pEncKey[pEncKey.Length()-1] = (TUint8) ((pEncKey[pEncKey.Length()-1]) & mask);
	return octetStr;
	}
