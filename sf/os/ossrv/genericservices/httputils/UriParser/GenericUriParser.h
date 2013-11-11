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
// This file contains the Parsing functionality for Generic URIs as 
// specified in RFC 2396.
// 
//

/**
 @file GenericUriParser.h
*/
#ifndef __GENERICURIPARSER_H__
#define __GENERICURIPARSER_H__

// System includes
#include <e32base.h>
#include <uriutilscommon.h>

// Local includes
#include "TUriParserInternal.h"
#include "UriUtilsInternal.h"

/**
	Dependencies : CUriParserBase.
	Comments : Provides Parsing functionality for the Uri objects.
	This is implemented as per RFC2396 for Parsing the Generic Uris. 
	It uses 8-bit descriptors.

	@internalComponent
	@released
*/
class CGenericUriParser: public CBase
	{
public:	

	static CGenericUriParser* NewL();
	virtual ~CGenericUriParser();
    inline void DoParseUri(const TPtrC8& aUri, TPtrC8 aComponent[]);

protected:	

	CGenericUriParser();
	void ConstructL();

	virtual TInt ParseScheme(const TPtrC8& aUri, TPtrC8& aScheme);
	virtual TInt ParseAuthority(const TPtrC8& aUri, TPtrC8& aUserinfo, TPtrC8& aHost, TPtrC8& aPort, TBool aUseNetworkDelimiter);
	virtual TInt ParsePath(const TPtrC8& aUri, TPtrC8& aPath);
	virtual TInt ParseQuery(const TPtrC8& aUri, TPtrC8& aQuery);
	virtual TInt ParseFragment(const TPtrC8& aUri, TPtrC8& aFragment);

	//Internal Supporting method for Parsing
	virtual TInt FindFirstUriDelimiter(const TPtrC8& aString, TDelimiterSearchFlag aSearchFlag);
	};

inline void CGenericUriParser::DoParseUri(const TPtrC8& aUri, TPtrC8 aComponent[])
	{
	// Parse the components
	TPtrC8 uri(aUri);
	TInt consumed = 0;
	TPtrC8& scheme = aComponent[EUriScheme];
	if( (consumed = ParseScheme(uri, scheme)) > 0 )
		{
		uri.Set(uri.Mid(consumed));
		}
	if( (consumed = ParseAuthority(uri, aComponent[EUriUserinfo], 
		 aComponent[EUriHost], aComponent[EUriPort], IsNetworkScheme(scheme) )) > 0 )
		{
		uri.Set(uri.Mid(consumed));
		}
	if( (consumed = ParsePath(uri, aComponent[EUriPath])) > 0 )
		{
		uri.Set(uri.Mid(consumed));
		}
	if( (consumed = ParseQuery(uri, aComponent[EUriQuery])) > 0 )
		{
		uri.Set(uri.Mid(consumed));
		}
	if( (consumed = ParseFragment(uri, aComponent[EUriFragment])) > 0 )
		{
		uri.Set(uri.Mid(consumed));
		}
	}

#endif	// __GENERICURIPARSER_H__

