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
* This file contains the implementation of the ASN1 integer class.
* As noted in Notes.txt there are two approaches to the storage of data by
* this class, it can either hold a copy of the data or a pointer to something
* else which holds a copy of the data.
* The object contained within this file encapsulates the ASN1 integer object
* as described in X208 section 14 and the encoding described in X209 section
* 8. 
*
*/


#include "asn1dec.h"
#include <bigint.h>

EXPORT_C TASN1DecInteger::TASN1DecInteger()

	{
	}

EXPORT_C TInt TASN1DecInteger::DecodeDERShortL(const TDesC8& aSource,TInt& aPos)
	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	TInt res = DecodeDERShortL(gen);
	aPos+=gen.LengthDER();
	return res;
	}

EXPORT_C TInt TASN1DecInteger::DecodeDERShortL(const TASN1DecGeneric& aSource)
	{
	TInt res = 0;
	TPtrC8 ptr(aSource.GetContentDER());
	//stop stupid compiler warning
	TUint length = (TUint)(ptr.Length());
	if (length == 0) // DEF055722 
		{
		User::Leave(KErrArgument);
		}
	if (length > sizeof(TInt))
		{
		User::Leave(KErrTooBig);
		}
	TInt8 i;
	if (ptr[0]&128)
		{
		res = -1;
		}
	for (i=0;i<ptr.Length();i++)
		{
		res<<=8;
		res+=ptr[i];
		}
	return res;
	}

EXPORT_C RInteger TASN1DecInteger::DecodeDERLongL(const TDesC8& aSource,TInt& aPos)
	{
	TPtrC8 Source=aSource.Right(aSource.Length() - aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	RInteger res = DecodeDERLongL(gen);
	aPos+=gen.LengthDER();
	return res;
	}

EXPORT_C RInteger TASN1DecInteger::DecodeDERLongL(const TASN1DecGeneric& aSource)
	{
	const TDesC8& data = aSource.GetContentDER();
//	JCS 14/03/2003: Changed call to NewL from NewSignedL() because current
//	problem in RInteger for signed code means this sometimes returns incorrect
//	results causing failure in certstore testcode.  However, making this change
//	does cause problems in TASN1 now (posted defect DEF021796).
	RInteger res = RInteger::NewL(data);//aSource.GetContentDER());
	return (res);
	}
