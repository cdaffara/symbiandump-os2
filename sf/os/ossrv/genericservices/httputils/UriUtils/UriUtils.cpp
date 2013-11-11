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

#include <uriutils.h>
#include <uriutilscommon.h>
#include "UriUtilsInternal.h"
#include <escapeutils.h>

_LIT8(KDot, ".");
_LIT8(KDotDot, "..");
_LIT8(KDotSlash, "./");
_LIT8(KDotDotSlash, "../");
_LIT8(KSlash, "/");	
_LIT8(KSlashDot, "/.");
_LIT8(KSlashDotDot, "/..");
_LIT8(KSlashDotSlash, "/./");
_LIT8(KSlashDotDotSlash, "/../");

_LIT(KHexDigit, "0123456789ABCDEF");
_LIT(KUnreserved, "-.~_");
#ifdef _DEBUG
_LIT(KNormalisationUriPanicCategory, "URI-NORMALIZATION");
#endif
const TInt KEscapeIndicator					= '%';
const TInt KEscapeTripleLength				= 3;
const TInt KEscDelimiterPos					= 0;
const TInt KMostSignificantNibblePos		= 1;
const TInt KLeastSignificantNibblePos		= 2;
const TInt KSubstringLength					= 3;
const TInt KUpdateLength					= 2;
const TInt KAttachLength					= 1;

const TInt KDotLength						= 1;
const TInt KDotDotLength					= 2;
const TInt KDotDotSlashLength				= 3;
const TInt KSlashDotDotSlashLength			= 4;

//
//
// Implementation of UriUtils
//
//

/**
	Converts a 16-bit format uri into its internet form. Any Unicode characters 
	are converted into Utf8 representation and then any excluded characters are 
	escape encoded.  Reserved characters specified in RFC2396 will not be escape 
	encoded however, these include ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ",". 
	For example http://localhost will not be encoded to http%3A%2F%2Flocalhost.
	
	@since			6.0
	@deprecated	 Deprecated in 9.1
	@leave			KUriUtilsCannotConvert. When the input data cannot be converted.
	@leave			KUriUtilsErr16BitChar. When the input data has a 16-Bit character to be escape encoded.
	@param			aUri	The 16-bit format uri.
	@return			A pointer to a newly created 8-bit uri.
 */
EXPORT_C CUri8* UriUtils::ConvertToInternetFormL(const TUriC16& aUri)
	{
	// Need to convert to utf8
	HBufC8* utf8Buf = EscapeUtils::ConvertFromUnicodeToUtf8L(aUri.UriDes());
	CleanupStack::PushL(utf8Buf);

	// Ok need to parse for the uri without the fragment
	TUriParser8 parser;
	parser.Parse(*utf8Buf);
	TPtrC8 uriNoFragment;
	parser.UriWithoutFragment(uriNoFragment);

	// Now escape encode the uri without the fragment
	HBufC8* escapedBuf = EscapeUtils::EscapeEncodeL(uriNoFragment, EscapeUtils::EEscapeNormal);
	CleanupStack::PushL(escapedBuf);

	// Now escape encode the fragment if there is one...
	HBufC8* escapedFragmentBuf = NULL;
	if( parser.IsPresent(EUriFragment) )
		{
		escapedFragmentBuf = EscapeUtils::EscapeEncodeL(parser.Extract(EUriFragment), EscapeUtils::EEscapeNormal);
		CleanupStack::PushL(escapedFragmentBuf);
		}

	// Parse and then create the CUri8 object
	parser.Parse(*escapedBuf);
	CUri8* netForm = CUri8::NewL(parser);

	// Set the fragment if there was one...
	if( escapedFragmentBuf != NULL )
		{
		CleanupStack::PushL(netForm);
		netForm->SetComponentL(*escapedFragmentBuf, EUriFragment);
		CleanupStack::Pop(netForm);
		CleanupStack::PopAndDestroy(escapedFragmentBuf);
		}

	// Cleanup and return
	CleanupStack::PopAndDestroy(2, utf8Buf);	// utf8Buf, escapedBuf
	return netForm;
	}

/**
	Converts an 8-bit format uri its into display form. Any escape tripes are decoded and 
	sets of Utf8 format characters are converted into Unicode.
	
	@since			6.0
	@deprecated	 Deprecated in 9.1	
	@leave			KUriUtilsCannotConvert. When the input data cannot be converted.
	@param			aUri	The 8-bit format uri.
	@return			A pointer to a newly created 16-bit uri.
 */
EXPORT_C CUri16* UriUtils::ConvertToDisplayFormL(const TUriC8& aUri)
	{
	// Need decode escape triples
	HBufC8* unescapedBuf = EscapeUtils::EscapeDecodeL(aUri.UriDes());
	CleanupStack::PushL(unescapedBuf);

	// Now need to convert utf8 to unicode
	HBufC16* utf8Buf = EscapeUtils::ConvertToUnicodeFromUtf8L(*unescapedBuf);
	CleanupStack::PushL(utf8Buf);

	// Parse and then create the CUri16 object
	TUriParser16 parser;
	parser.Parse(*utf8Buf);
	CUri16* displayForm = CUri16::NewL(parser);

	// Cleanup and return
	CleanupStack::PopAndDestroy(2, unescapedBuf);	// unescapedBuf, utf8Buf
	return displayForm;
	}
	
/** 
	Create a new CUri8 object from a Unicode descriptor.
	
	@param aUri a Unicode string containing the URI to parse.
	@return the new CUri8 object
	@leave EUriUtilsParserErrInvalidUri  if the descriptor is an invalid URI.
 */
EXPORT_C CUri8* UriUtils::CreateUriL(const TDesC& aUri) 
	{
	// convert to UTF8
	HBufC8* unsafe8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aUri);
     	CleanupStack::PushL(unsafe8);
	// escape encode only those characters that cannot be in a URI. assume all %hh are %encoded already
	HBufC8* uri8desc = EscapeUtils::ReEscapeEncodeL(*unsafe8);
	CleanupStack::PopAndDestroy(unsafe8);
     	CleanupStack::PushL(uri8desc);
	TUriParser8 parser;
	// parse the descriptor into a URI, Leave if it cannot be parsed
	User::LeaveIfError( parser.Parse(*uri8desc) );

	CUri8* uri8 = CUri8::NewL(parser);
	CleanupStack::PopAndDestroy(uri8desc);
	return uri8;
	}

/** 
	Create a new CAuthority8 object from a Unicode descriptor.
	
	@param aAuthority a Unicode string containing the Authority to parse.
	@return the new CAuthority8 object
	@leave EUriUtilsParserErrInvalidUri  if the descriptor is an invalid Authority.
 */
EXPORT_C CAuthority8* UriUtils::CreateAuthorityL(const TDesC& aAuthority) 
	{
	// convert to UTF8
	HBufC8* unsafe8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aAuthority);
     	CleanupStack::PushL(unsafe8);
	// escape encode only those characters that cannot be in the authority. assume all %s are %encoded already
	HBufC8* authority8desc = EscapeUtils::ReEscapeEncodeL(*unsafe8);
	CleanupStack::PopAndDestroy(unsafe8);
     	CleanupStack::PushL(authority8desc);
	TAuthorityParser8 parser;
	// parse the descriptor into the authority, Leave if it cannot be parsed
	User::LeaveIfError( parser.Parse(*authority8desc) );

	CAuthority8* authority8 = CAuthority8::NewL(parser);
	CleanupStack::PopAndDestroy(authority8desc);
	return authority8;
	}

/**
	Checks a descriptor for excluded (invalid) characters. Excluded characters include all 
	control characters (values 0x00 to 0x1F and greater than 0x7F), space (0x20), delimiter 
	characters ('<', '>', '#', '%',	'"') and unwise characters ('{', '}', '|', '\', '^', '[', ']', '`').
	
	@since			6.0
	@param			aData	The descriptor to be checked.
	@return			A boolean value of ETrue if the descriptor contains invalid
	characters, otherwise EFalse.
 */
EXPORT_C TBool UriUtils::HasInvalidChars(const TDesC8& aData)
	{
	return CheckForExcludedChars(aData);
	}
	
/**
	Checks a descriptor for excluded (invalid) characters. Excluded characters include all 
	control characters (values 0x00 to 0x1F and	greater than 0x7F), space (0x20), delimiter 
	characters ('<', '>', '#', '%','"') and unwise characters ('{', '}', '|', '\', '^', '[', ']', '`').
	
	@since			6.0
	@param			aData	The descriptor to be checked.
	@return			A boolean value of ETrue if the descriptor contains invalid
	characters, otherwise EFalse.
 */
EXPORT_C TBool UriUtils::HasInvalidChars(const TDesC16& aData)
	{
	return CheckForExcludedChars(aData);
	}

/**
	Checks the supplied host for an IPv4, IPv6 or text format host
	
	@since			7.0
	@param			aHost	The descriptor containing the host to check
	@return			A TUriHostType enum of either EIPv6, EIPv4, EText or EUnknown
 */
EXPORT_C UriUtils::TUriHostType UriUtils::HostType(const TDesC8& aHost)
	{
	return CheckHostType(aHost);
	}

/**
	Checks the supplied host for an IPv4, IPv6 or text format host
	
	@since			7.0
	@param			aHost	The descriptor containing the host to check
	@return			A TUriHostType enum of either EIPv6, EIPv4, EText or EUnknown
 */
EXPORT_C UriUtils::TUriHostType UriUtils::HostType(const TDesC16& aHost)
	{
	return CheckHostType(aHost);
	}



//
//
// Implementation of component internal functions
//
//

/**
	@internalComponent

	Checks whether the given scheme is a network scheme or not
	
	@param aScheme The descriptor with the scheme.
	@return A boolean value of EFalse if the scheme is SIP.	For all other schemes returns ETrue.
 */
TBool IsNetworkScheme(const TDesC8& aScheme)
	{
	TUriSchemeType scheme = SchemeType(aScheme);
	if (scheme == ESchemeTypeSip)
		{
		return EFalse;
		}
	return ETrue;
	}

/**
	@internalComponent

	Checks whether the given scheme is a network scheme or not
	
	@param aScheme The descriptor with the scheme.
	@return A boolean value of EFalse if the scheme is SIP.	For all other schemes returns ETrue.
 */
TBool IsNetworkScheme(const TDesC16& aScheme)
	{
	TUriSchemeType scheme = SchemeType(aScheme);
	if (scheme == ESchemeTypeSip)
		{
		return EFalse;
		}
	return ETrue;
	}

/**
	@internalComponent

	Returns the type of the URIs scheme
	
	@param			aScheme	The descriptor with the scheme.
	@return			The scheme type
 */
TUriSchemeType SchemeType(const TDesC8& aScheme)
	{
	// Compares the scheme with both sip and sips
	if (aScheme.CompareF(KSipScheme8()) == 0 || aScheme.CompareF(KSipsScheme8()) == 0)
		{
		// there's a match so this is a sip scheme
		return ESchemeTypeSip;
		}
	//Compares the scheme with tel 
	else if (aScheme.CompareF(KTelScheme8()) == 0) 
		{
		return ESchemeTypeTel;
		} 

	return ESchemeTypeUnknown;
	}

/**
	@internalComponent

	Returns the type of the URIs scheme
	
	@param			aScheme	The descriptor with the scheme.
	@return			The scheme type
 */
TUriSchemeType SchemeType(const TDesC16& aScheme)
	{
	// Compares the scheme with both sip and sips
	if (aScheme.CompareF(KSipScheme()) == 0 || aScheme.CompareF(KSipsScheme()) == 0)
		{
		// there's a match so this is a sip scheme
		return ESchemeTypeSip;
		}

	return ESchemeTypeUnknown;
	}

/**
	@internalComponent

	Checks that a text host is in a valid form
	
	@param			aHost	The descriptor containing the host to check
	@return			ETrue if the host is valid otherwise EFalse
 */
TBool IsTextHostValid(const TDesC8& aHost)
	{
	return CheckValidTextHost(aHost);
	}

/**
	@internalComponent

	Checks that a text host is in a valid form
	
	@param			aHost	The descriptor containing the host to check
	@return			ETrue if the host is valid otherwise EFalse
 */
TBool IsTextHostValid(const TDesC16& aHost)
	{
	return CheckValidTextHost(aHost);
	}


/**
	@internalComponent

	Parses a segment of the form name=value and returns the name and value parts
	
	@param			aSegment	the name-value segemnt to parse
	@param			aName		the name part that is returned
	@param			aValue		the value part that is returned
 */
void GetNameValuePair(const TDesC8& aSegment, TPtrC8& aName, TPtrC8& aValue)
	{
	TPtrC8 value;
	TInt sepPos = aSegment.Locate(KEqualsSeparator);
	if (sepPos != KErrNotFound)
		{
		aName.Set(aSegment.Left(sepPos));
		value.Set(aSegment.Mid(sepPos+1));
		}
	else
		{
		aName.Set(aSegment);
		}

	aValue.Set(value);
	}


//
//
// Implementation of LOCAL functions
//
//

/**
	Checks the descriptor for any excluded characters. These are characters that 
	should have been escaped encoded or ocnverted to Utf8 from Unicode.
						
	@since			6.0
	@param			aData	The descriptor to be checked.
	@return		A boolean value of ETrue if the descriptor contains excluded
				characters, EFalse if it does not.
 */
template<class TDesCType>
LOCAL_C TBool CheckForExcludedChars(const TDesCType& aData)
	{
	// Run through the descriptor
	TBool valid = ETrue;
	const TInt length = aData.Length();
	TInt i=0;
	while( valid && i<length )
		{
		TInt notUsed;
		// See if the character is an excluded one, or is part of an escape triple...
		if( EscapeUtils::IsExcludedChar(aData[i]) && !EscapeUtils::IsEscapeTriple(aData.Mid(i), notUsed) )
			{
			valid = EFalse;
			}
		else
			{
			++i;
			}
		}
	return !valid;
	}

/**
	Checks the supplied host for an IPv4, IPv6 or text format host
	
	@since			7.0
	@param			aHost	The descriptor containing the host to check
	@return			A TUriHostType enum of either EIPv6, EIPv4, EText or EUnknown
 */
template<class TDesCType>
LOCAL_C UriUtils::TUriHostType CheckHostType(const TDesCType& aHost)
	{
	UriUtils::TUriHostType hostType;

	TInt dotCount=0;
	TBool colonPresent=EFalse;
	TBool numeric=ETrue;

	TInt len = aHost.Length();
	for (TInt ii=0; ii < len && !colonPresent; ++ii)
		{
		TChar ch(aHost[ii]);

		// host contains a character that is not '0'..'9' or '.'
		if ((ch < 48 || ch > 57) && ch != 46)
			numeric=EFalse;

		// need to check that IPv4 address has the 3 dots
		if (ch == 46)
			++dotCount;
		else
			if (ch == 58)
				colonPresent=ETrue;
		}

	if (colonPresent) // if theres a colon, it has to be an IPv6 address
		hostType = UriUtils::EIPv6Host;
	else
		if (numeric  && (dotCount==3)) // if its numeric only, and has three seperators...
			hostType = UriUtils::EIPv4Host;
		else
			hostType = UriUtils::ETextHost;

	return hostType;
	}

/**
	@internalComponent

	Checks that a text host is in a valid form
	
	@param			aHost	The descriptor containing the host to check
	@return			ETrue if the host is valid otherwise EFalse
 */
template<class TDesCType>
LOCAL_C TBool CheckValidTextHost(const TDesCType& aHost)
	{
	TInt len = aHost.Length();
	if (len == 0)
		return EFalse;

	// host name can't start with a dot or dash
	TChar firstChar(aHost[0]);
	if (firstChar == '-' || firstChar == '.')
		return EFalse;

	TChar prev = '\0';
	TInt ii;
	for (ii=0; ii < len; ii++)
		{
		TChar ch(aHost[ii]);

		// Valid characters are a-z, 0-9, '-' and '.'
		if ((ch < 'A' || ch > 'Z') && (ch < 'a' || ch > 'z') && (ch < '0' || ch > '9') && ch != '-' && ch != '.')
			{
			return EFalse;
			}

		// dot is the section separator. Check the previous section is not empty
		if (ch == '.' && prev == '.')
			{
			// can't have an empty section
			return EFalse;
			}
			prev = ch;
		}

	// host name can't end with a dot or dash
	if (prev == '-' || prev == '.')
		return EFalse;

	return ETrue;
	}

/**
	Supports Syntax-Based Normalization as specifed in section 6.2.2 of RFC3986.
	returns a new CUri8 object containing a normalised URI from a parsed URI object.
	
	@param aUri	A reference to a parsed uri object.
	@return A pointer to a CUri8 object containing normalised URI.
	@leave KErrNoMemory
	@internalAll
 */
EXPORT_C CUri8* UriUtils:: NormaliseUriL(const TUriC8& aUri)	
	{
	CUri8* normalisedUri = CUri8::NewLC(aUri);
	PercentEncodeL(normalisedUri); 
	CaseNormaliseL(normalisedUri);
	RemoveDotSegmentsL(normalisedUri);
	CleanupStack::Pop(normalisedUri);
	return normalisedUri;
	}
	
/**
	Performs Case Normalization for CUri8 object as specified 
	in section 6.2.2.1 of RFC3986.
	
	@param aNormalisedUri It is an in-out parameter. aNormalisedUri is a pointer 
	to CUri8 object with an uri needs to be case normalised and returns with 
	case normalised.
	@leave KErrNoMemory
 */
void CaseNormaliseL(CUri8* aNormalisedUri )	
	{
	//Case normalise the scheme
	DoCaseNormaliseL(aNormalisedUri, EUriScheme);	
	//Case normalise the Userinfo
	DoCaseNormaliseL(aNormalisedUri, EUriUserinfo);	
	//Case normalise the Host
	DoCaseNormaliseL(aNormalisedUri, EUriHost);	
	//Case normalise the Port
	DoCaseNormaliseL(aNormalisedUri, EUriPort);	
	//Case normalise the Path
	DoCaseNormaliseL(aNormalisedUri, EUriPath);	
	//Case normalise the Query
	DoCaseNormaliseL(aNormalisedUri, EUriQuery);	
	//Case normalise the Fragment
	DoCaseNormaliseL(aNormalisedUri, EUriFragment);	
	}

/**
	Performs Case Normalization for specified sub component of URI.
	
	@param aNormalisedUri It is an in-out parameter. aNormalisedUri is a pointer 
	to CUri8 object with an uri needs to be case normalised and returns with 
	case normalised for specified sub component.
	@param aComponent Enumeration of TUriComponent.
	@leave KErrNoMemory
*/
void DoCaseNormaliseL(CUri8* aNormalisedUri, TUriComponent aComponent)
	{
	const TUriC8& uri(aNormalisedUri->Uri());
	if(!uri.IsPresent(aComponent) )
		{
		return;
		}
	//extracts subcomponent of uri which needs to be case-normalised
	HBufC8* heapBuf = uri.Extract(aComponent).AllocLC(); 	
	TPtr8 normalisedComponent(heapBuf->Des());
	TBool normalised = EFalse;
	if(aComponent == EUriScheme || aComponent == EUriHost )
		{
		//change this component to lower case
		normalisedComponent.LowerCase();
		normalised = ETrue;
		}
	
	TInt len =  normalisedComponent.Length();
	TBuf8<KSubstringLength> subString;
	//case normalise the component
	for (TInt pos = 0; pos < len; pos++)
		{
		if (normalisedComponent[pos] == KEscapeIndicator )
			{
			__ASSERT_DEBUG( ((len-pos) >= KSubstringLength), User::Panic(KNormalisationUriPanicCategory, KUriUtilsErrBadEscapeTriple) );
			TPtrC8 componentBuf(normalisedComponent.Mid(pos,KSubstringLength));
			if (ValidateAndConvertPercentEncodedTriple(componentBuf,subString))
				{
				normalisedComponent.Replace(pos,KSubstringLength,subString);
				pos += KUpdateLength;
				normalised = ETrue;
				subString.Zero();
				}
			}
		}

	//updating the uri with normalised string
	if( normalised )
		{
		if(aComponent<EUriMaxComponents && aComponent >=EUriScheme)
		   {
		   aNormalisedUri->SetComponentL(normalisedComponent, aComponent);
		   }
		else
		   {
			User::Leave(KErrArgument);	
		   }

		}
	CleanupStack::PopAndDestroy(heapBuf);
	}

/**
	Validates and Converts the valid Percent encoded triplets to Uppercase for specified 
	sub component of URI. For eg: Converts %3a to %3A
	
	@param aData A reference to a string to be validated and converted to upper case.
	@param aCaseNormalizedData A reference to a descriptor that is converted to 
	uppercase that is to be returned.
	@return returns a bool whether it is a valid Percent encoded triplet
*/
TBool ValidateAndConvertPercentEncodedTriple(TDesC8& aData , TDes8& aCaseNormalizedData )	
	{
	// See if the descriptor is actually long enough and
	// Check that the three characters form an escape triple - first char is '%'
	if( aData.Length() < KEscapeTripleLength || aData[KEscDelimiterPos] != KEscapeIndicator )
		{
		return EFalse;//do nothing
		}
	
	// Check that next two characters are valid
	TInt mostSignificantDigitValue = KHexDigit().LocateF(aData[KMostSignificantNibblePos] );
	TInt leastSignificantDigitValue = KHexDigit().LocateF(aData[KLeastSignificantNibblePos] );

	if( mostSignificantDigitValue== KErrNotFound || leastSignificantDigitValue == KErrNotFound )
		{
		// Either of the characters were not a valid hex character
		return EFalse;
		}
	aCaseNormalizedData.Zero();
	aCaseNormalizedData.Append(KEscapeIndicator); 
	
	//Coverts most significant hex character to uppercase
	(mostSignificantDigitValue >= 0 && mostSignificantDigitValue <= 0xF) ? 
		aCaseNormalizedData.Append(KHexDigit().Mid(mostSignificantDigitValue,1)) :
		aCaseNormalizedData.Append(KHexDigit().Mid(mostSignificantDigitValue,1));
	
	//Coverts least significant hex character to uppercase
	(leastSignificantDigitValue >= 0 && leastSignificantDigitValue <= 0xF) ? 
		aCaseNormalizedData.Append(KHexDigit().Mid(leastSignificantDigitValue,1)) :
		aCaseNormalizedData.Append(aData[KLeastSignificantNibblePos]);
	
	return ETrue;
	}

/**
	Performs Percent-Encoding Normalization for CUri8 object as specifed in 
	section 6.2.2.2 of RFC3986.
	
	@param aNormalisedUri It is an in-out parameter. aNormalisedUri is a pointer to 
	CUri8 object with an uri needs to be Percent-Encoded and returns with Percent-Encode 
	normalised form.
	@leave KErrNoMemory
 */
void  PercentEncodeL(CUri8* aNormalisedUri)
	{
	//PercentEncode the scheme
	DoPercentEncodeL(aNormalisedUri, EUriScheme);	
	//PercentEncode the Userinfo
	DoPercentEncodeL(aNormalisedUri, EUriUserinfo);	
	//PercentEncode the Host
	DoPercentEncodeL(aNormalisedUri, EUriHost);	
	//PercentEncode the Port
	DoPercentEncodeL(aNormalisedUri, EUriPort);	
	//PercentEncode the Path
	DoPercentEncodeL(aNormalisedUri, EUriPath);	
	//PercentEncode the Query
	DoPercentEncodeL(aNormalisedUri, EUriQuery);	
	//PercentEncode the Fragment
	DoPercentEncodeL(aNormalisedUri, EUriFragment);	
	}

/**
	Performs Percent-Encoding for specified sub component of URI.
	
	@param aNormalisedUri It is an in-out parameter. aNormalisedUri is a pointer to 
	CUri8 object with an uri needs to be Percent-Encoded and returns with Percent-Encoded 
	for specified sub component.
	@param aComponent Enumeration of TUriComponent.
	@leave KErrNoMemory
*/
void DoPercentEncodeL(CUri8* aNormalisedUri, TUriComponent aComponent)
	{
	const TUriC8& uri(aNormalisedUri->Uri());
	if(!uri.IsPresent(aComponent))
		{
		return;
		}
	
	HBufC8* heapBuf = uri.Extract(aComponent).AllocLC();
	TPtr8 percentNormalisedComponent(heapBuf->Des());
	TBool normalised = EFalse;
	TInt len = percentNormalisedComponent.Length();	
	for (TInt pos = 0; pos < len; pos++)
		{
		TInt hex;
		// check for and decode '%' encoded characters
		if (percentNormalisedComponent[pos] == KEscapeIndicator && EscapeUtils::IsEscapeTriple(percentNormalisedComponent.Mid(pos, KSubstringLength), hex))
			{
			TChar replacedChar(hex);
			if( KUnreserved().LocateF(hex) != KErrNotFound || replacedChar.IsAlphaDigit() )
				{
				TBuf8<KAttachLength> subString;
				subString.Append(replacedChar);
				percentNormalisedComponent.Replace(pos, KSubstringLength, subString);
				normalised = ETrue;
				len = percentNormalisedComponent.Length();
				}
			}
		}
	if( normalised )
		{
		if(aComponent<EUriMaxComponents && aComponent >=EUriScheme)
		   {
		    aNormalisedUri->SetComponentL(percentNormalisedComponent, aComponent);
		   }
		else
		   {
			User::Leave(KErrArgument);	
		   }

		}
	CleanupStack::PopAndDestroy(heapBuf); 	
	}

/**
	Performs Path Segment Normalization for CUri8 object as specifed in 
	section 6.2.2.3 of RFC3986.
	
	@param aNormalisedUri It is an in-out parameter. aNormalisedUri is a pointer to 
	CUri8 object with uri needs to be Path Segment normalised and returns with 
	Path Segment normalised form.
	@leave KErrNoMemory
 */
void  RemoveDotSegmentsL(CUri8* aNormalisedUri)
	{
	const TUriC8& uri( aNormalisedUri->Uri() );
	if(uri.IsPresent(EUriPath))
		{
		HBufC8* dotSegmentsPath = uri.Extract(EUriPath).AllocLC();
		RemoveExtraneousDotSegmentsL(dotSegmentsPath);
		aNormalisedUri->SetComponentL(*dotSegmentsPath, EUriPath);	
		CleanupStack::PopAndDestroy(dotSegmentsPath);	
		}
	}

/**
	Performs Remove_dot_segments algorithm as specifed in section 5.2.4 of RFC3986.
	
	@param aUriInputPath It is an in-out parameter. aUriInputPath is a pointer to the 
	path descriptor to be normalised for extraneous dot_segments and returns with 
	normalised dot_segments.
	@leave KErrNoMemory
*/
void RemoveExtraneousDotSegmentsL(HBufC8* aUriInputPath)
	{
	TPtr8 uriPathBuf(aUriInputPath->Des());
	TInt length = uriPathBuf.Length();	
	HBufC8* path = HBufC8::NewLC(length);
	TPtr8 transitionalBuf(path->Des());

	while(length > 0)	
		{
		//step a of section 5.2.4 of RFC 3986
		if(length >= KDotDotSlashLength && 
			KDotDotSlash().Compare(uriPathBuf.Mid(0, KDotDotSlashLength)) == 0 )
			{
			uriPathBuf.Delete(0,KDotDotSlashLength);
			}
		//step a of section 5.2.4 of RFC 3986
		else if(length >= KDotDotLength && 
				KDotSlash().Compare(uriPathBuf.Mid(0, KDotDotLength)) == 0)
			{
			uriPathBuf.Delete(0,KDotDotLength);	
			}
		//step b of section 5.2.4 of RFC 3986
		else if(length >= KDotDotSlashLength && 
				KSlashDotSlash().Compare(uriPathBuf.Mid(0, KDotDotSlashLength)) == 0)
			{
			uriPathBuf.Replace(0, KDotDotSlashLength, KSlash);
			}
		//step c of section 5.2.4 of RFC 3986
		else if(length >= KSlashDotDotSlashLength && 
				KSlashDotDotSlash().Compare(uriPathBuf.Mid(0, KSlashDotDotSlashLength)) == 0)
			{
			updateStrings(uriPathBuf, transitionalBuf, KSlashDotDotSlashLength);
			}
		//step c of section 5.2.4 of RFC 3986 --complete path segment
		else if(length == KDotDotSlashLength && 
				KSlashDotDot().Compare(uriPathBuf.Mid(0, KDotDotSlashLength)) == 0)
			{
			updateStrings(uriPathBuf, transitionalBuf, KDotDotSlashLength);
			}
		//step b of section 5.2.4 of RFC 3986--complete path segment
		else if(length == KDotDotLength && 
				KSlashDot().Compare(uriPathBuf.Mid(0, KDotDotLength)) == 0)
			{
			uriPathBuf.Replace(0, KDotDotLength, KSlash);
			}
		//step d of section 5.2.4 of RFC 3986
		else if(length == KDotDotLength && 
				KDotDot().Compare(uriPathBuf.Mid(0)) == 0)
			{
			uriPathBuf.Delete(0,KDotDotLength);	
			}
		//step d of section 5.2.4 of RFC 3986
		else if(length == KDotLength && 
				KDot().Compare(uriPathBuf.Mid(0)) == 0)
			{
			uriPathBuf.Delete(0,KDotLength);	
			}
		//step e of section 5.2.4 of RFC 3986
		else 
			{
			//get the first path segment including initial / (if any)from uriPathBuf
			// till next slash (but not including next slash)..append it to the output Buf	
			TInt substrLength;
			TInt nextSlashPos = uriPathBuf.Find(KSlash);
			if(nextSlashPos == 0 && length > KDotLength)
				//replace with locate next
				{
				nextSlashPos = uriPathBuf.Mid(1).Find(KSlash);
				if(nextSlashPos != KErrNotFound)
					{
					++nextSlashPos;
					}
				}
			if(length == KDotLength)
				//only '/' is exist
				{
				substrLength = length;	
				}
			else
				{
				substrLength = nextSlashPos == KErrNotFound ? length : nextSlashPos ;	
				}
			transitionalBuf.Append(uriPathBuf.Mid(0,substrLength));
			uriPathBuf.Delete(0,substrLength);	
			}
		length = uriPathBuf.Length();
		}
	uriPathBuf.Copy(transitionalBuf);
	CleanupStack::PopAndDestroy(path);
	}

/**
	Updates the strings specified in step c of section 5.2.4 of RFC 3986
	
	@param aInputBuf A reference to the inputBuf needs to be modified 
	@param aOutPutBuf A reference to the outPutBuf needs to be modified
	@param aLength length of the string to be replaced.
 */
void  updateStrings(TPtr8& aInputBuf, TPtr8& aOutPutBuf, TInt aLength)
	{
	aInputBuf.Replace(0,aLength,KSlash);

	//In outPutBuf to remove the last segment starting with / (if exist)
	//eg: /abc/def/fgh --> /abc/def
	TInt outputBufLength = aOutPutBuf.Length();
	TInt pos = aOutPutBuf.LocateReverse('/');	
	//remove the last segment including '/'
	pos != KErrNotFound ? aOutPutBuf.Delete( pos, outputBufLength - pos ) : aOutPutBuf.Delete( 0,outputBufLength );
	}

