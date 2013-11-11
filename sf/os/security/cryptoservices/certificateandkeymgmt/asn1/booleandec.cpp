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


#include "asn1dec.h"

EXPORT_C TASN1DecBoolean::TASN1DecBoolean(void)
	{
	}

EXPORT_C TBool TASN1DecBoolean::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Ptr=aSource.Right(aSource.Length()-aPos);
	TASN1DecGeneric gen(Ptr);
	gen.InitL();
	aPos+=gen.LengthDER();
	TBool res = DecodeDERL(gen);
	return res;
	}

EXPORT_C TBool TASN1DecBoolean::DecodeDERL(const TASN1DecGeneric& aSource)

	{
	TPtrC8 content = aSource.GetContentDER();
	if (content.Length() != 1)
		{
		User::Leave(KErrArgument);
		}
	if (content[0] == 0)
		{
		return EFalse;
		}
	if (content[0] == 0xFF)
		{
		return ETrue;
		}
	User::Leave(KErrArgument);
	return ETrue;//never gets to here
	}
