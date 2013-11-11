// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __INETPROTTEXTUTILSLOCAL_H__
#define __INETPROTTEXTUTILSLOCAL_H__

#include <e32std.h>

#include "inetprottextutils.h"

template<class TPtrCType>
TInt DoRemoveWhiteSpace(TPtrCType& aData, InetProtTextUtils::TRemoveMode aMode);

template<class TPtrCType>
TInt FindWhiteSpaceLeft(const TPtrCType& aData);

template<class TPtrCType>
TInt FindWhiteSpaceRight(const TPtrCType& aData);

template<class TPtrCType>
TInt DoExtractNextTokenFromList(TPtrCType& aBuffer, TPtrCType& aToken, TChar aSeparator);

template<class TPtrCType>
TInt DoExtractNextTokenFromList(TPtrCType& aBuffer, TPtrCType& aToken, const TPtrCType& aSeparator);

template<class TPtrCType>
TInt DoExtractQuotedStringL(TPtrCType& aBuffer, TPtrCType& aQuotedString);

template<class HBufCType>
void DoConvertIntToDescriptorL(TInt aInt, HBufCType*& aBuffer, TRadix aRadix);

template<class TPtrCType>
TInt DoConvertDescriptorToInt(const TPtrCType& aData, TInt& aInt, TRadix aRadix);

#endif	// __INETPROTTEXTUTILSLOCAL_H__
