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

#ifndef __TEXTUTILSTESTCOMMON_H__
#define __TEXTUTILSTESTCOMMON_H__

// System includes
//
#include <e32base.h>

template<class TPtrCType, class TDesCType>
LOCAL_C TInt DoTestTextUtilsRemove(const TDesCType& aBase, const TDesCType& aRemoveLeft, TInt aConsumedLeft,
								   const TDesCType& aRemoveRight, TInt aConsumedRight,
								   const TDesCType& aRemoveBoth, TInt aConsumedBoth);

template<class TDesCType>
LOCAL_C TInt DoTestTextUtilsDesToIntConversion(const TDesCType& aDes, TInt aInt, TBool aValid);

template<class HBufCType, class TDesCType>
LOCAL_C TInt DoTestTextUtilsIntToDesConversion(TInt aInt, const TDesCType& aDes);

template<class TDesCType>
LOCAL_C TInt DoTestTextUtilsDesToHexConversion(const TDesCType& aDes, TInt aHex, TBool aValid);

template<class HBufCType, class TDesCType>
LOCAL_C TInt DoTestTextUtilsHexToDesConversion(TInt aHex, const TDesCType& aDes);

template<class TPtrCType, class TDesCType>
LOCAL_C TInt DoTestExtractQuotedStringL(
									   const TDesCType& aBuffer, 
									   const TDesCType& aString, 
									   const TDesCType& aRemainder,
									   TInt				aExpectedError
									   );

template<class TPtrCType, class TDesCType>
LOCAL_C TInt DoExtractTokenFromListMultipleSeparator(
												const TDesCType& aBuffer,
												const TDesCType& aToken0,
												const TDesCType& aToken1,
												const TDesCType& aToken2,
												const TDesCType& aRemainder,
												const TDesCType& aSeparator
												);

template<class TPtrCType, class TDesCType>
LOCAL_C TInt DoExtractTokenFromListSingleSeparator(
												const TDesCType&	aBuffer,
												const TDesCType&	aToken0,
												const TDesCType&	aToken1,
												const TDesCType&	aToken2,
												const TDesCType&	aRemainder,
												TChar				aSeparator
												);
#endif	// __TEXTUTILSTESTCOMMON_H__
