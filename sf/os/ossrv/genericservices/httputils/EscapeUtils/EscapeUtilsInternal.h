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
// File contains local functions for use by the Escaping utility functions
// defined in EscapeUtils.h.
// 
//

/**
 @file EscapeUtilsInternal.h
 @see EscapeUtils.h
*/

#ifndef __ESCAPEUTILSINTERNAL_H__
#define __ESCAPEUTILSINTERNAL_H__

// System includes
//
#include <e32base.h>

template<class TDesCType, class TPtrType>
TInt EscapeEncodeData(const TDesCType& aData, const TDesCType& aReservedChars, TPtrType& aEncodedData);

template<class TDesCType, class TPtrType>
TInt EscapeDecodeData(const TDesCType& aData, TPtrType& aDecodedData);

template<class TDesCType>
TBool CheckAndConvertEscapeTriple(const TDesCType& aData, TInt& aHexValue);

#endif	// __ESCAPEUTILSINTERNAL_H__
