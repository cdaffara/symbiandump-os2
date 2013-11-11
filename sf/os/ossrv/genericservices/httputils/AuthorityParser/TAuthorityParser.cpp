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

#include <authority8.h>
#include <authority16.h>
#include "TAuthorityParserInternal.h"
#include <uriutilscommon.h>


//
//
// Implementation of TAuthorityParser8
//
//

/**	
	Constructor.
	
	@since			6.0
 */
EXPORT_C TAuthorityParser8::TAuthorityParser8()
: TAuthorityC8()
	{
	}

/**
	Parses the descriptor aAuthority into authority components.
	
	@since			6.0
	@param			aAuthority	A reference to a descriptor pointer to be parsed.
	@return			KErrNone if the descriptor has been parsed into its authority components.
	@post			The object references the input descriptor.
 */
EXPORT_C TInt TAuthorityParser8::Parse(const TDesC8& aAuthority)
	{
	// Reset the Authority information and then set the authority
	if( iAuthorityDes.Ptr() )
		Reset();
	iAuthorityDes.Set(aAuthority);

	// Parse the authority
	DoParseAuthority(iAuthorityDes, iComponent);
	return KErrNone;
	}

//
//
// Implementation of TAuthorityParser16
//
//

/**
	Constructor.
	
	@since			6.0
	@deprecated Deprecated in 9.1
 */
EXPORT_C TAuthorityParser16::TAuthorityParser16()
: TAuthorityC16()
	{
	}

/**
	Parses the descriptor aAuthority into authority components.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@param			aAuthority A reference to a descriptor pointer to be parsed.
	@return			KErrNone if the descriptor has been parsed into its authority components.
	@post			The object references the input descriptor.
 */
EXPORT_C TInt TAuthorityParser16::Parse(const TDesC16& aAuthority)
	{
	// Reset the Authority information and then set the authority
	if( iAuthorityDes.Ptr() )
		Reset();
	iAuthorityDes.Set(aAuthority);

	// Parse the authority
	DoParseAuthority(iAuthorityDes, iComponent);
	return KErrNone;
	}

//
//
// Implementation of templated LOCAL functions
//
//

/**
	Templated function to parse an authority. The input argument aAuthority points to a 
	descriptor with the authority. The parsed authority component information is set in 
	the output argument aComponent. 
	
	@since			6.0
	@param			aAuthority	The descriptor pointer to the authority.
	@param			aComponent	The array of descriptor pointers to be updated.
	@pre 			The array of descriptor pointers in the output argument aComponent must
	be set to NULL.
	@post			The parsed authority components will be refered to by the descriptor 
	pointers in aComponent. If any components did not exist then the appropriate pointer 
	will remain unchanged, i.e. still be NULL.
 */
template<class TPtrCType> 
void DoParseAuthority(const TPtrCType& aAuthority, TPtrCType aComponent[])
	{
	// Parse the components
	TPtrCType authority = aAuthority;
	TInt consumed = 0;
	if( (consumed = ParseUserinfo(authority, aComponent[EAuthorityUserinfo])) > 0 )
		{
		authority.Set(authority.Mid(consumed));
		}
	if( (consumed = ParseHost(authority, aComponent[EAuthorityHost])) > 0 )
		{
		authority.Set(authority.Mid(consumed));
		}
	if( (consumed = ParsePort(authority, aComponent[EAuthorityPort])) > 0 )
		{
		authority.Set(authority.Mid(consumed));
		}
	}

/**
	Templated function that parses the descriptor for the userinfo component. The input argument 
	aAuthority must be set to the beginning of the userinfo component (ie the start of the authority) 
	for the userinfo to be correctly parsed. If the userinfo component is found, the output argument 
	aComponent will be set to refer to it. The number of characters consumed (ie the number of characters 
	to move to the end of this component, including any delimiters)	is returned.
	
	@since			6.0
	@param			aAuthority	The descriptor set to the beginning of the userinfo component.
	@param			aComponent	The output descriptor pointer to refer to the parsed component.
	@return			The number of characters consumed in parsing the component.
	@pre 			The input argument descriptor is set at the beginning of the userinfo component 
	to correctly parse it. The output descriptor aComponent is set to NULL.
	@post			If the component exists, the output descriptor will refer to it. If
	the component does not exist, then the output decriptor will remain set to NULL.
 */
template<class TPtrCType> 
TInt ParseUserinfo(const TPtrCType& aAuthority, TPtrCType& aComponent)
	{
	TInt consumed =0;
	TInt userinfoEndPos = aAuthority.Locate(KUserinfoDelimiter);

	// Check that the delimiter is there
	if( userinfoEndPos != KErrNotFound )
		{
		// Got a host - store information
		aComponent.Set(aAuthority.Left(userinfoEndPos));

		// Set consumed amount - include the trailing '@' delimiter
		consumed = userinfoEndPos + 1;
		}
	return consumed;
	}

/**
	Templated function that parses the descriptor for the host component. The input argument aAuthority 
	must be set to beginning of the host component (ie the end of the userinfo component) for the host 
	to be correctly parsed. If the host component is found,	the output argument aComponent will be set 
	to refer to it. The number of characters consumed (ie the number of characters to move to the end of 
	this component, including any delimiters) is returned.
	
	@since			6.0
	@param			aAuthority	The descriptor set to the beginning of the host component.
	@param			aComponent	The output descriptor pointer to refer to the parsed component.
	@return			The number of characters consumed in parsing the component.
	@pre 			The input argument descriptor is set at the beginning of the host component 
	to correctly parse it. The output descriptor aComponent is set to NULL.
	@post			If the component exists, the output descriptor will refer to it. If
	the component does not exist, then the output decriptor will remain set to NULL.
 */
template<class TPtrCType> 
TInt ParseHost(const TPtrCType& aAuthority, TPtrCType& aComponent)
	{
	// Get the descriptor and look for the host delimiter
	TInt consumed =0;
	
	// is this an IPv6 host?
	TInt startIPv6Host = aAuthority.Locate(KIPv6UriOpenBrace);

	if (startIPv6Host==KErrNotFound)
		{
		// it's an IPv4 address then....
		TInt hostEndPos = aAuthority.Locate(KPortDelimiter);

		// Check that the delimiter is there
		if( hostEndPos == KErrNotFound )
			hostEndPos = aAuthority.Length();

		if( hostEndPos > 0 )
			{
			// Got a host - store information
			aComponent.Set(aAuthority.Left(hostEndPos));

			// Set consumed amount
			consumed = hostEndPos;
			}
		}
	else
		{
		// This is an IPv6 address, so it MUST have the closing brace too....
		TInt endIPv6Host = aAuthority.Locate(KIPv6UriCloseBrace);

		// Return an error if the closing IPv6 delimiter isn't there.
		if (endIPv6Host==KErrNotFound)
			return KUriUtilsErrInvalidUri;

		// The host shouldnt include the '[' and ']'
		aComponent.Set(aAuthority.Mid(startIPv6Host+1, endIPv6Host - (startIPv6Host + 1) ));

		// set the consumed amount to include the closing brace
		consumed = endIPv6Host+1;
		}

	return consumed;
	}

/**
	Templated function that parses the descriptor for the port component. The input argument aAuthority 
	must be set to beginning of the port component (ie the end of the host component) for the port to be 
	correctly parsed. If the port component is found, the output argument aComponent will be set to refer 
	to it. The number of characters consumed (ie the number of characters to move to the end of this component,
	including any delimiters) is returned.
	
	@since			6.0
	@param			aAuthority	The descriptor set to the beginning of the port component.
	@param			aComponent	The output descriptor pointer to refer to the parsed component.
	@return			The number of characters consumed in parsing the component.
	@pre 			The input argument descriptor is set at the beginning of the port component 
	to correctly parse it. The output descriptor aComponent is set to NULL.
	@post			If the component exists, the output descriptor will refer to it. If
	the component does not exist, then the output decriptor will remain set to NULL.
 */
template<class TPtrCType> 
TInt ParsePort(const TPtrCType& aAuthority, TPtrCType& aComponent)
	{
	// Get the descriptor and look for the port delimiter
	TInt consumed =0;
	TInt portEndPos = aAuthority.Length();

	// We have a port
	if( portEndPos > 0 )
		{
		// Got a host - store information; need to exclude the leading ':'
		aComponent.Set(aAuthority.Mid(1, portEndPos - 1));

		// Set consumed amount - this includes the leading ':' delimiter
		consumed = portEndPos;
		}
	return consumed;
	}
