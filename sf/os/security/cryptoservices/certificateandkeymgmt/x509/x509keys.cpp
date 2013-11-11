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


#include <x509keys.h>
#include <asn1dec.h>
#include <asn1enc.h>
#include <x509cert.h>
#include "x509keyencoder.h"

// TX509KeyEncoder Class Implementation

TX509KeyEncoder::TX509KeyEncoder(TAlgorithmId aDigestAlg)
    : iDigestAlg(aDigestAlg)
	{
	}

EXPORT_C  CASN1EncSequence* TX509KeyEncoder::EncodeDigestAlgorithmLC() const
	{
	CASN1EncSequence* digestAlgId = CASN1EncSequence::NewLC();

	CASN1EncObjectIdentifier* oid = NULL;
	
	switch (iDigestAlg)
		{
		case EMD2:
			oid = CASN1EncObjectIdentifier::NewLC(KMD2);
			break;

		case EMD5:
			oid = CASN1EncObjectIdentifier::NewLC(KMD5);
			break;

		case ESHA1:
			oid = CASN1EncObjectIdentifier::NewLC(KSHA1);
			break;
					
		default:
			User::Leave(KErrNotSupported);
			break;			
		}
	digestAlgId->AddAndPopChildL(oid);
	
	// No parameters : NULL
	CASN1EncNull* nullparams = CASN1EncNull::NewLC();
	digestAlgId->AddAndPopChildL(nullparams);

	return digestAlgId;
	}
