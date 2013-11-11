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
* This file contains the implementation of the Octet String ASN1 class.
*
*/


#include "asn1dec.h"

EXPORT_C TASN1DecOctetString::TASN1DecOctetString(void)
	{
	}

EXPORT_C HBufC8* TASN1DecOctetString::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	HBufC8* res = gen.GetContentDER().AllocL();
	aPos+=gen.LengthDER();
	return res;
	}

EXPORT_C HBufC8* TASN1DecOctetString::DecodeDERL(const TASN1DecGeneric& aSource)
	{
	return aSource.GetContentDER().AllocL();
	}

EXPORT_C TInt TASN1DecOctetString::CheckValid(const TDesC8& /*aSource*/)
	{
	return KErrNone;
	}
