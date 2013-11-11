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
// File contains local functions for use by the Uri parser classes defined in
// Uri8.h and Uri16.h
// 
//

/**
 @file TUriParserInternal.h
 @see Uri8.h, Uri16.h
*/

#ifndef __TURIPARSERINTERNAL_H__
#define __TURIPARSERINTERNAL_H__

// System includes
//
#include <e32base.h>

/** 
	enum TDelimiterSearchFlag
	Enum defining a set of flags used for search delimiter searching.
	@internalComponent
	@released
	@since 6.0	

 */
enum TDelimiterSearchFlag
	{
	/* Specifier for colon delimiter */
	EColonDelimiterFlag		=	0x001,
	/* Specifier for slash delimiter */
	ESlashDelimiterFlag		=	0x002,
	/* Specifier for query delimiter */
	EQueryDelimiterFlag		=	0x004,
	/* Specifier for hash delimiter */
	EHashDelimiterFlag		=	0x008,
	/* Specifier for semi-colon delimiter */
	ESemiColonDelimiterFlag	=	0x010,
	/* Specifier for query delimiter search */
	EQueryDelimiterSearch	=	EHashDelimiterFlag,
	/* Specifier for path delimiter seacrh */
	EPathDelimiterSearch	=	EQueryDelimiterSearch | EQueryDelimiterFlag,
	/* Specifier for authority delimiter search */
	EAuthDelimiterSearch	=	EPathDelimiterSearch | ESlashDelimiterFlag,
	/* Specifier for scheme delimiter seacrh */
	ESchemeDelimiterSearch	=	EAuthDelimiterSearch | EColonDelimiterFlag
	};

template<class TPtrCType>
void DoParseUri(const TPtrCType& aUri, TPtrCType aComponent[]);

template<class TPtrCType>
TInt ParseScheme(const TPtrCType& aUri, TPtrCType& aScheme);

template<class TPtrCType>
TInt ParseAuthority(const TPtrCType& aUri, TPtrCType& aUserinfo, TPtrCType& aHost, TPtrCType& aPort, TBool aUseNetworkDelimiter);


template<class TPtrCType>
TInt ParsePath(const TPtrCType& aUri, TPtrCType& aPath);

template<class TPtrCType>
TInt ParseQuery(const TPtrCType& aUri, TPtrCType& aQuery);


template<class TPtrCType>
TInt ParseFragment(const TPtrCType& aUri, TPtrCType& aFragment);

template<class TPtrCType>
TInt FindFirstUriDelimiter(const TPtrCType& aString, TDelimiterSearchFlag aSearchFlag);

#endif	// __TURIPARSERINTERNAL_H__
