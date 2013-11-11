// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __FILEURITESTCOMMON_H__
#define __FILEURITESTCOMMON_H__

// System includes
//
#include <e32base.h>

template<class CUriType, class TDesCType>
TInt DoFileUriExtraction(const CUriType& aUri, const TDesCType& aExpectedFileUri);

template<class TUriParserType, class CUriType, class TDesCType, class TDesCType16>
TInt DoFileUriComponentExtraction(const CUriType& aUri, const TDesCType& aScheme, 
									const TDesCType& aPath, const TDesCType16& aExpectedFileName);

template<class TParserType, class TDesCType>
TInt TestFileName(const TParserType& aParser, const TDesCType& aExpected);

template<class TParserType, class TDesCType, class TComponentType>
TInt TestComponent(const TParserType& aParser, const TDesCType& aExpected, TComponentType aComponent);


#endif	// __FILEURITESTCOMMON_H__
