/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#ifndef NUMBERCONVERSIONIMP_H_
#define NUMBERCONVERSIONIMP_H_

#include <e32std.h>
#include "NumberConversion.h"

class StandardDigitMatch
/**
@internalComponent
*/
	{
public:
	static TInt Match(const TDesC& aText, TInt& aLength, TDigitType& aDigitType,
		NumberConversion::TDigitMatchType aDigitMatchType);
	static TInt LeadingZeros(const TDesC& aText);
	static TDigitType DigitType(TChar aChar);
	static void AppendFormat(TDes& aText, TInt aNumber, TDigitType aDigitType);
	static TInt LengthOfFormattedNumber(TInt aNumber);
	};

#endif
