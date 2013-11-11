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

#include "SipUriParser.h"

/** 
 * CSIPUriParser Constructor.
 */ 
CSIPUriParser::CSIPUriParser()
: CGenericUriParser()
	{
	}

/** 
 * CSIPUriParser Static Factory Construction.
 */
CSIPUriParser* CSIPUriParser::NewL()
	{
	CSIPUriParser* self = new (ELeave) CSIPUriParser;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**	
	Second phase of two-phase construction method. 
	
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed and initialized.
 */
void CSIPUriParser::ConstructL()
	{
	// Does nothing.
	}

/**
 * Default Destructor.
 */
CSIPUriParser::~CSIPUriParser()
	{
	}

/**
	Function to parse a descriptor for a scheme component. If a scheme is found 
	then the output argument aScheme is set to refer to it.
						
	@param			aUri	The descriptor containing the uri to be parsed for a scheme.
	@param			aScheme	The output descriptor to refer to the scheme
	@return			The number of characters consumed in parsing the scheme.
	@pre 			The output descriptor has been initialized so that the pointer
					to the associated descriptor buffer is NULL. The input descriptor 
					is set to the start of the uri.
	@post			If a scheme component exists then the output descriptor refers
					to it, otherwise the output descriptor is left unchanged.
*/
TInt CSIPUriParser::ParseScheme(const TPtrC8& aUri, TPtrC8& aScheme)
	{
	// Get the descriptor and look for scheme delimiter
	TInt consumed =0;
	TInt endSchemePos = FindFirstUriDelimiter(aUri, ESipSchemeDelimiterSearch);

	if( endSchemePos != KErrNotFound )
		{
		// Got a scheme - store information
		aScheme.Set(aUri.Left(endSchemePos));

		// Set consumed amount move past scheme delimiter
		consumed = endSchemePos + 1;
		}
	return consumed;
	}
	
/**
	Function to parse a descriptor for an authority component. If an authority is 
	found then the output arguments aUserinfo, aHost and aPort are set to refer to those parts 
	of the authority component. If an authority component exists then the host part exists. The 
	userinfo and port parts are optional.
						
	@param			aUri		The descriptor containing the uri to be parsed 
					for an authority.
	@param			aUserinfo	The output descriptor for the userinfo component.
	@param			aHost		The output descriptor for the host component.
	@param			aPort		The output descriptor for the port component.
	@param			aUseNetworkDelimiter is not used as we know the Sip scheme 
					not uses the network delimeter '//'
	@return			The number of characters consumed in parsing the authority.
	@pre 			The output descriptors have been initialized so that the pointer
					to the their associated descriptor buffers is NULL. The input descriptor 
					is set to the start of the start of the authority component.
	
	@post			If an authority component exists then the output descriptors refer
					to the userinfo part (if exists), the host part and the port part (if exists), 
					otherwise the output descriptors are left unchanged.
*/
TInt CSIPUriParser::ParseAuthority(const TPtrC8& aUri, TPtrC8& aUserinfo, TPtrC8& aHost, TPtrC8& aPort, TBool aUseNetworkDelimiter)
	{
	// Get uri descriptor and see if authority exists - if aUseNetworkDelimiter is true it has to start with '//' 
	TInt consumed =0;
	const TInt prefixLength = aUseNetworkDelimiter ? KUriNetworkAuthorityDelimiterLength : 0;
  	if( !aUseNetworkDelimiter || 
  		(aUri.Length() >= prefixLength && aUri[0] == KSlashDelimiter && aUri[1] == KSlashDelimiter ))
		{
		// There is an authority
		TPtrC8 authority( aUri.Mid(prefixLength) );

		// Authority delimited by ';', '?' or the end of the string
		TInt authorityEndPos = FindFirstUriDelimiter(authority, ESipAuthDelimiterSearch);

		// Got authority - parse it for its components
		authority.Set(authority.Left(authorityEndPos));

		// Get the userinfo...
		TInt userinfoEndPos = authority.Locate(KUserinfoDelimiter);
		if( userinfoEndPos != KErrNotFound )
			{
			// Store the information 
			aUserinfo.Set(authority.Left(userinfoEndPos));
			  
			// Move past the userinfo and the delimiter '@'
			authority.Set(authority.Mid(userinfoEndPos + 1));
			}

		// Authority is also delimited by ';' but this is valid in the userinfo so can only be checked now
		TInt semicolonPos = FindFirstUriDelimiter(authority, ESipSemiColonDelimiterFlag);
		if ( semicolonPos != KErrNotFound )
			{
			authority.Set(authority.Left(semicolonPos));
			authorityEndPos = semicolonPos + userinfoEndPos + 1;
			}
		
		// Set consumed amount to move past authority
		consumed += prefixLength + authorityEndPos;
		
		// Check if this is an IPv6 address	by looking for the opening '['
		TInt startHostIPv6 = authority.Locate(KIPv6UriOpenBrace);

		if (startHostIPv6 == KErrNotFound)
			{
			// This isn't an IPv6 address.....

			// Get host...
			TInt hostEndPos = authority.Locate(KPortDelimiter);

			// Host also delimited by the end of the authority
			if( hostEndPos == KErrNotFound )
				{
				hostEndPos = authority.Length();	
				}

			// There's always a host, but can be empty - store information
			aHost.Set(authority.Left(hostEndPos));

			// Move past the host
			authority.Set(authority.Mid(hostEndPos));
			}
		else
			{
		// First, move past the opening brace
		    authority.Set(authority.Mid(startHostIPv6 + 1));
		    // auth now = X:X:X]?????

			// This is an IPv6 address, so it MUST have the closing brace too....
			TInt endIPv6Host = authority.Locate(KIPv6UriCloseBrace);

			// Return an error if the closing IPv6 delimiter isn't there.
			// or Host is empty
			if (endIPv6Host == KErrNotFound || endIPv6Host == startHostIPv6 + 1)
				{
				return KUriUtilsErrInvalidUri;	
				}

			// It's an ipv6  address, with an opening and closing brace. So now just extract it
			// auth = [X:X:X]?????


			// Set the host, and need to remove the closing brace
			aHost.Set(authority.Left(endIPv6Host));
			// host = X:X:X

			// Move past the host
			authority.Set(authority.Mid(endIPv6Host + 1 ));
			}
		
		// Get the port...
		TInt portEndPos = authority.Length();
		if( portEndPos )
			{
			// Store the port - remove leading ':'
			aPort.Set(authority.Mid(1, portEndPos - 1));
			}
		}
	return consumed;
	}

/**
	Function to parse a descriptor for a path/parameter component.There is always a path component.
	The ouput argument aPath is set to the path/parameter component found.
						
	@param			aUri	The descriptor containing the uri to be parsed for a path/parameter.
	@param			aComponent	The output descriptor to refer to the path.
	@return			The number of characters consumed in parsing the path.
	@pre 			The output descriptor has been initialized so that the pointer
					to the associated descriptor buffer is NULL. The input descriptor 
					is set to the start of the path.
					
	@post			The output descriptor refers to the path component.
*/	
TInt CSIPUriParser::ParsePath(const TPtrC8& aUri, TPtrC8& aComponent)
	{
	// Get descriptor with the path
	TInt consumed =0;

	// Path is delimited by '?' or the end of the string
	TInt pathEndPos = FindFirstUriDelimiter(aUri, ESipPathDelimiterSearch);

	// Check for presence of path
	if( pathEndPos != KErrNotFound )
		{
		// Got path - store information
		aComponent.Set(aUri.Left(pathEndPos));
	
		// Set consumed amount to move past path
		consumed = pathEndPos;
		}
	return consumed;
	}

/**
	Function to parse a descriptor for a query/header component.If a query is found then 
	the output argument aQuery is set to refer to it.
						
	@param			aUri	The descriptor containing the uri to be parsed for a query/Header.
	@param			aComponent	The output descriptor to refer to the query
	@return			The number of characters consumed in parsing the query.
	@pre 			The output descriptor has been initialized so that the pointer
					to the associated descriptor buffer is NULL. The input descriptor is set to
					the start of the query.
	@post			If a query component exists then the output descriptor refers
					to it, otherwise the output descriptor is left unchanged.
*/
TInt CSIPUriParser::ParseQuery(const TPtrC8& aUri, TPtrC8& aComponent)
	{
	// Get descriptor with the query
	TInt consumed =0;

	// Query/Header is delimited by end of the string 
	TInt queryEndPos = aUri.Length();

	// Check for presence of query
	if( queryEndPos )
		{
		// Got query - store information; need to excluded leading '?'
		aComponent.Set(aUri.Mid(1, queryEndPos - 1));
	
		// Set consumed amount to move past query
		consumed = queryEndPos;
		}
	return consumed;
	}
	
/**
	Function to parse a descriptor for a fragment component. 
	In Sip Uris no Fragment Component it contains. This is an dummy implementation.
						
	@param			aUri not used.
	@param			aComponent	not used.
	@return			returns zero.
*/
TInt CSIPUriParser::ParseFragment(const TPtrC8& /*aUri*/, TPtrC8& /* aComponent*/)
	{
	// Sip Uri Does not conatin Fragment
	return 0;
	}
	
/**
	Function to find the position of the first delimiter in the descriptor specified 
	by the delimiter flags. Note that the end of the descriptor is also a delimiter if there are 
	no others. In the case of the scheme delimiter search, the position returned depends on the 
	position of the colon delimiter with respect to the other delimiters for a scheme.
						
	@param			aUri The descriptor containing the section of a uri to be searched.
	@param			aSearchFlag	The enum specifying the delimiters to search for.
	@return			The position of nearest delimiter to start of the descriptor, where
					zero is the start (left-most) position.
*/
TInt CSIPUriParser::FindFirstUriDelimiter(const TPtrC8& aUri, TSipDelimiterSearchFlag aSearchFlag)
	{
	// Set ultimate delimiter - string length
	TInt endPos = aUri.Length();

	if( aSearchFlag & ESipQueryDelimiterFlag )
		{
		TInt queryPos = aUri.Locate(KQueryDelimiter);
		if( queryPos != KErrNotFound && queryPos < endPos )
			{
			TInt userinfoPos = aUri.Locate(KUserinfoDelimiter);
			//QueryDelimiter may be in Userinfo
			if(userinfoPos != KErrNotFound && userinfoPos < endPos && userinfoPos > queryPos ) 
				{
				//Find QueryDelimiter after Userinfo
				queryPos = aUri.Mid(userinfoPos).Locate(KQueryDelimiter);
				if (queryPos != KErrNotFound  && queryPos < endPos )
					{
					queryPos += userinfoPos;	
					}
				}
			if(queryPos != KErrNotFound)
				{
				endPos = queryPos;		
				}
			}
		}

	if( aSearchFlag & ESipSemiColonDelimiterFlag )
		{
		TInt semiColonPos = aUri.Locate(KParamDelimiter);
		if( semiColonPos != KErrNotFound && semiColonPos < endPos )
			{
			TInt userinfoPos = aUri.Locate(KUserinfoDelimiter);
			//ParamDelimiter may be in Userinfo
			if(userinfoPos != KErrNotFound && userinfoPos < endPos && userinfoPos > semiColonPos ) 
				{
				//Find ParamDelimiter after Userinfo
				semiColonPos = aUri.Mid(userinfoPos).Locate(KParamDelimiter);
				if (semiColonPos != KErrNotFound  && semiColonPos < endPos )
					{
					semiColonPos += userinfoPos;	
					}
				}
			if(semiColonPos != KErrNotFound)
				{
				endPos = semiColonPos;		
				}
			}
		}
	if( aSearchFlag & ESipColonDelimiterFlag )
		{
		TInt schemePos = aUri.Locate(KSchemeDelimiter);
		if( schemePos != KErrNotFound && schemePos < endPos )
			{
			// There is a scheme
			endPos = schemePos;
			}
		else if( aSearchFlag == ESipSchemeDelimiterSearch )
			{
			// Ok different if looking for scheme delimiter - no scheme, return KErrNotFound
			endPos = KErrNotFound;
			}
		}
	return endPos;
	}

