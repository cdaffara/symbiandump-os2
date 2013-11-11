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


#include "X509time.h"

TASN1DecX509Time::TASN1DecX509Time()

	{
	}

TTime TASN1DecX509Time::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric Gen(Source);
	Gen.InitL();
	aPos+=Gen.LengthDER();
	TTime t = TASN1DecX509Time::DecodeContentsL(Gen);
	return t;
	}

TTime TASN1DecX509Time::DecodeDERL(const TASN1DecGeneric& aSource)
	{
	return DecodeContentsL(aSource);
	}

TTime TASN1DecX509Time::DecodeContentsL(const TASN1DecGeneric& aSource)
	{
	TTime result;
	result.HomeTime();

	switch (aSource.Tag())
		{
	case EASN1UTCTime:
		{
		TASN1DecUTCTime encT;
		result = encT.DecodeDERL(aSource);
		break;
		}
	case EASN1GeneralizedTime:
		{
		TASN1DecGeneralizedTime encT;
		result = encT.DecodeDERL(aSource);
		break;
		}
	default:
		User::Leave(KErrArgument);
		break;
		}

	return result;
	}
