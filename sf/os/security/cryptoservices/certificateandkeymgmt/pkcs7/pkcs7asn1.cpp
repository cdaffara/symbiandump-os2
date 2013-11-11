/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pkcs7asn1.h"
#include <asn1dec.h>

CArrayPtr<TASN1DecGeneric>* PKCS7ASN1::DecodeSequenceLC(const TDesC8& aRawData)
	{
	CArrayPtr<TASN1DecGeneric>* items = NULL;
	
	// Check we've got a sequence
	TASN1DecGeneric decGen(aRawData);
	decGen.InitL();
	if(decGen.Tag() != EASN1Sequence && decGen.Tag() != EASN1Set)
		{
		User::Leave(KErrArgument);
		}
	else
		{
		// Decode the sequence		
		TASN1DecSequence decSeq;
		items = decSeq.DecodeDERLC(decGen);
		}
	return items;
	}

CArrayPtr<TASN1DecGeneric>* PKCS7ASN1::DecodeSequenceLC(const TDesC8& aRawData,
															const TInt aMinTerms,
															const TInt aMaxTerms)
	{
	CArrayPtr<TASN1DecGeneric>* items = DecodeSequenceLC(aRawData);
	TInt count = items->Count();
	if(count < aMinTerms || count > aMaxTerms)
		{
		User::Leave(KErrArgument);
		}

	return items;
	}
