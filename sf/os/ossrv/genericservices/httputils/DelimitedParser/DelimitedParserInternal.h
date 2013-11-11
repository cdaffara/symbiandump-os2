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
// This file provides an internal API used by the classes defined in 
// DelimitedParser.h
// 
//

/**
 @file DelimitedParserInternal.h
 @see DelimitedParser.h
*/

#ifndef __DELIMITEDINTERNAL_H__
#define __DELIMITEDINTERNAL_H__

// System includes
//
#include <e32base.h>

template<class TDesCType>
TInt NextDelimiterPosition(const TDesCType& aData, TInt aStartPos, TInt aDelimiter, TDelimitedDataParseMode aMode);

template<class TDesCType>
TInt PrevDelimiterPosition(const TDesCType& aData, TInt aStartPos, TInt aDelimiter, TDelimitedDataParseMode aMode);

template<class TDesCType>
TInt RightDelimiterPosition(const TDesCType& aData, TInt aStartPos, TInt aDelimiter);

template<class TDesCType>
TInt LeftDelimiterPosition(const TDesCType& aData, TInt aStartPos, TInt aDelimiter);

template<class TDesCType>
TInt InitialDelimiterPosition(const TDesCType& aData, TDelimitedDataParseMode aMode);

#endif	// __DELIMITEDINTERNAL_H__
