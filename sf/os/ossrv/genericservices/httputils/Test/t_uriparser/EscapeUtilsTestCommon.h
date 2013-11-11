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

#ifndef __ESCAPEUTILSTESTCOMMON_H__
#define __ESCAPEUTILSTESTCOMMON_H__

// System includes
//
#include <e32base.h>

template<class HBufCType, class TDesCType>
LOCAL_C TInt DoTestEscapeUtilsEscapeEncode(const TDesCType& aData, const TDesCType& aExpected, EscapeUtils::TEscapeMode aEscapeMode);

template<class HBufCType, class TDesCType>
LOCAL_C TInt DoTestEscapeUtilsEscapeDecode(const TDesCType& aData, const TDesCType& aExpected);

template<class TDesCType>
LOCAL_C TInt DoTestEscapeUtilsIsEscapeTriple(const TDesCType& aData, TBool aIsTriple, TInt aValue);

#endif	// __ESCAPEUTILSTESTCOMMON_H__
