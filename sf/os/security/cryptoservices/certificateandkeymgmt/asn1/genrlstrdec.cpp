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
* This file contains the implemenation of the General String ASN1 object.
* The general string can contain the following characters:
* <character set unknown pending arrival of ITU spec>
*
*/


#include <asn1dec.h>

EXPORT_C TASN1DecGeneralString::TASN1DecGeneralString(void)
	{
	}

EXPORT_C TInt TASN1DecGeneralString::CheckValid(const TDesC8& /*aSource*/)

	{
	return KErrNone;
	}

EXPORT_C HBufC* TASN1DecGeneralString::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	HBufC* res = DecodeContentsL(gen.GetContentDER());
	return res;
	}

HBufC* TASN1DecGeneralString::DecodeContentsL(const TDesC8& aSource)
	{
	HBufC* res = HBufC::NewL(aSource.Length());
	TPtr pRes = res->Des();
	pRes.Copy(aSource);
	return res;
	}
