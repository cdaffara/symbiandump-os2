// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SIPURIPARSER_H__
#define __SIPURIPARSER_H__

#include "GenericUriParser.h"

/** 
	enum TSipDelimiterSearchFlag
	Enum defining a set of flags used for search delimiter searching.
 */
enum TSipDelimiterSearchFlag
	{
	// Specifier for colon delimiter 
	ESipColonDelimiterFlag		=	0x001,
	// Specifier for semicolon delimiter 
	ESipSemiColonDelimiterFlag	=	0x002,
	// Specifier for query/header delimiter 
	ESipQueryDelimiterFlag		=	0x004,
	// Specifier for path delimiter seacrh 
	ESipPathDelimiterSearch	=	ESipQueryDelimiterFlag,
	// Specifier for authority delimiter search 
	ESipAuthDelimiterSearch	=	ESipPathDelimiterSearch | ESipSemiColonDelimiterFlag,
	// Specifier for scheme delimiter seacrh 
	ESipSchemeDelimiterSearch	=	ESipAuthDelimiterSearch | ESipColonDelimiterFlag
	};

/**
Dependencies : CUriParserBase.
Comments : Provides Parsing functionality for the Sip Uri objects.
This is implemented as per RFC3261 for Parsing of Uris. 
It uses 8-bit descriptors.

@internalComponent
@released
*/
class CSIPUriParser: public CGenericUriParser
	{
public:	

	static CSIPUriParser* NewL();
	virtual ~CSIPUriParser();

private:	

	CSIPUriParser();
	void ConstructL();

	TInt ParseScheme(const TPtrC8& aUri, TPtrC8& aScheme);
	TInt ParseAuthority(const TPtrC8& aUri, TPtrC8& aUserinfo, TPtrC8& aHost, TPtrC8& aPort, TBool aUseNetworkDelimiter);
	TInt ParsePath(const TPtrC8& aUri, TPtrC8& aPath);
	TInt ParseQuery(const TPtrC8& aUri, TPtrC8& aQuery);
	TInt ParseFragment(const TPtrC8& aUri, TPtrC8& aFragment);

	//Internal Supporting method for Parsing
	TInt FindFirstUriDelimiter(const TPtrC8& aString, TSipDelimiterSearchFlag aSearchFlag);
	};

#endif	// __SIPURIPARSER_H__


