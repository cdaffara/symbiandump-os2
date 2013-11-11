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
* This file contains the implementation of the ASN1 Null class.
*
*/


#include "asn1dec.h"

EXPORT_C TASN1DecNull::TASN1DecNull(void)
	{
	}

EXPORT_C void TASN1DecNull::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	aPos+=gen.LengthDER();
	DecodeDERL(gen);
	}

EXPORT_C void TASN1DecNull::DecodeDERL(const TASN1DecGeneric& aSource)
	{
	TPtrC8 contents(aSource.Encoding());
	if ((contents.Length() != 2)	||
		(contents[1] & 0xff)		||
		(!(contents[0] & 0x05)))
		{
		User::Leave(KErrArgument);
		}
	}

