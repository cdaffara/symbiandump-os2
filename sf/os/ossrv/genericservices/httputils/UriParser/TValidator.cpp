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

// System includes
#include <uriutilscommon.h>
#include <uriutils.h>
#include <delimitedpathsegment8.h>
#include <delimitedquery8.h>
#include <escapeutils.h>

//User includes
#include "UriUtilsInternal.h"
#include "TValidator.h"

//Constants
_LIT8(KTransport, "transport");
_LIT8(KUser, "user"); 
_LIT8(KMethod, "method");
_LIT8(KTtl, "ttl");
_LIT8(KMaddr, "maddr");
_LIT8(KLr, "lr");
_LIT8(KExtension, "ext" );
_LIT8(KIsdnSubAddress, "isub" );
_LIT8(KContext, "phone-context" );
const TInt KMaxHostAddr = 255;


/**
	Constructor.
 */
TValidatorBase::TValidatorBase(const TUriC8& aUri)
: iUri(aUri)
	{
	}

/**
	Checks the Uri to be valid. 
	If there is no valid Host, Port, userinfo, Path, Query or Fragment then the 
	return value indicates an appropriate invalid Component. else returns zero, 
	which indicates given uri is valid.

	@return whether the Uri is Valid by returning zero or appropriate error value 
			for Invalid Uri.
 */
TInt TValidatorBase::Validate()
	{
	if (!IsValidHost())
		{
		return KUriUtilsErrInvalidHost;
		}
	
	if (!IsValidPort())
		{
		return KUriUtilsErrInvalidPort;
		}
		
	if (!IsValidUserInfo())
		{
		return KUriUtilsErrInvalidUserInfo;
		}
	
	if (!IsValidPath())
		{
		return KUriUtilsErrInvalidParam;
		}
	
	if (!IsValidQuery())
		{
		return KUriUtilsErrInvalidHeaders;
		}
	
	if (!IsValidFragment())
		{
		return KUriUtilsErrInvalidFragment;
		}
	
	return KErrNone;
	}

/**
    Checks whether the given character is in Valid Set of characters.
 	@param		aChar A Character needs to be checked against Set of characters.
 	@param		aCharSet A set of Characters Descriptor.
 	@return 	returns ETrue if aChar is a Valid Character else returns EFalse.
 */
TBool TValidatorBase::IsInCharSet(TText8 aChar, const TDesC8& aCharSet) const
	{
	for (TInt i = 0; i < aCharSet.Length(); i++)
		{
		if (aChar == aCharSet[i])
			{
			return ETrue;
		}
		}
	return EFalse;		
	}

/**
    Checks whether the given descriptor is Valid or not.
 	@param		aDes A Descriptor needs to be checked against Set of characters 
 				defined in aCharTypes.
 	@param		aCharTypes A set of Characters Descriptor.
 	@param		aEscapeValid For the given aCharTypes whether the Escape encoded is valid 
 				or not while Validating aDes, is specified by setting ETrue or EFalse 
 	@return 	returns ETrue if aDes is a Valid descriptor else returns EFalse.
 */
TBool TValidatorBase::IsValidCharacters(const TDesC8& aDes, TUint32 aCharTypes, TBool aEscapeValid) const
	{
	TInt len = aDes.Length();
	for (TInt i=0; i < len; i++)
		{
		TUint8 ch = aDes[i];
		// check for and decode '%' encoded characters
		if (aEscapeValid && ch == '%')
			{
			TInt hex;
			if (!EscapeUtils::IsEscapeTriple(aDes.Mid(i,3), hex))
				{
				// not a valid encoding
				return EFalse;
				}
			i += 2;
			continue;
			}
			
		if ((aCharTypes & KCharSetNumber) && (ch >= '0' && ch <= '9'))
			{
			continue; 
			} 
		if ((aCharTypes & KCharSetLowerAlpha) && (ch >= 'a' && ch <= 'z'))
			{
			continue; 
			}
		if ((aCharTypes & KCharSetUpperAlpha) && (ch >= 'A' && ch <= 'Z'))
			{
			continue; 
			}
		if ((aCharTypes & KCharSetAlways) && IsInCharSet(ch, KAlwaysValidChars()))
			{
			continue; 
			}
		if ((aCharTypes & KCharSetCommon) && IsInCharSet(ch, KCommonValidChars))
			{
			continue; 
			}
		if ((aCharTypes & KCharSetUser) && IsInCharSet(ch, KValidUserChars))
			{
			continue;
			}
		if ((aCharTypes & KCharSetParam) && IsInCharSet(ch, KValidParamChars))
			{
			continue; 
			}
		if ((aCharTypes & KCharSetHeader) && IsInCharSet(ch, KValidHeaderChars))
			{
			continue; 
			}
		if ((aCharTypes & KCharSetSipMark) && IsInCharSet(ch, KUriAlwaysValidSipMarkChars))
			{
			continue; 	
			}
			
		if ((aCharTypes & KCharSetSipPwd) && IsInCharSet(ch, KUriValidSipPwdChars))
			{
			continue; 	
			}
		
		if ((aCharTypes & KCharSetSipTkn) && IsInCharSet(ch, KUriValidSipToken))
			{
			continue; 	
			}
		
		// character is not valid so exit
		return EFalse;
		}
	return ETrue;
	}

/**
	The meaning empty here is that the component is present but it's value is not present
	This occures when the delimeter for a component is in the URI but there is 
	nothing in the URI after the delimeter. For example:
	1. sip:loc.com		- a URI with no port
	2. sip:loc.com:		- a URI with a port delimeter but no port value. The port is 'empty'.
	3. sip:loc.com:1666	- a URI with a port
	
	@param		aDes A Descriptor.
 	@return		ETrue or EFalse.
 */
TBool TValidatorBase::IsEmpty(const TDesC8& aDes) const
	{
	TInt len = aDes.Length();
	const TUint8* ptr = aDes.Ptr();
	if (!len && ptr)
		{
		// An entry for this item has been created so the delimeter was encountered
		// However, no value has been found. The item is empty
		return ETrue;
		}
	return EFalse;
	}


//
//		Implementatin of support for Sip-Uris as specified in RFC 3261.    //
//

/**
	Constructor.
 */
TValidatorSip::TValidatorSip(const TUriC8& aUri) 
: TValidatorBase(aUri)
	{
	}

/**
	Checks that a Host is in a valid form as specified in RFC 3261.

	@return		A boolean value of ETrue if uri contains valid Host,
				EFalse if it does not.
 */	
TBool TValidatorSip::IsValidHost() const
	{
	const TDesC8& host = iUri.Extract(EUriHost);
	if (host.Length() == 0)
		{
		return EFalse;
		}
	
	// IPv4 and IPv6 hosts are validated so just check text hosts
	if (UriUtils::HostType(host) == UriUtils::ETextHost &&	!IsTextHostValid(host))
		{
		return EFalse;
		}
	return ETrue;
	}
	
/**
	Checks that a Port is in a valid form as specified in RFC 3261.

	@return		A boolean value of ETrue if uri contains valid Port,
				EFalse if it does not.
 */	
TBool TValidatorSip::IsValidPort() const
	{
	const TDesC8& port = iUri.Extract(EUriPort);
	if (IsEmpty(port))
		{
		return EFalse;
		}
	return IsValidCharacters(port, KCharSetNumber);
	}
	
/**
	Checks whether Userinfo contains valid characters in the sip-uri as specified in RFC 3261.
							
	@param		aUser	The descriptor to be checked.
	@return		A boolean value of ETrue if uri contains valid Userinfo,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidUser(const TDesC8& aUser) const
	{
	return IsValidCharacters(aUser, KCharSetUserAll, ETrue);
	}

/**
	Checks whether Password contains valid characters in the sip-uri as specified in RFC 3261.
							
	@param		aPassword	The descriptor to be checked.
	@return		A boolean value of ETrue if uri contains valid Password,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidPassword(const TDesC8& aPassword) const
	{
	if (!aPassword.Length())
		{
		return EFalse;
		}
	return IsValidCharacters(aPassword, KCharSetSipPassWord, ETrue);
	}

/**
	Checks that a UserInfo is in a valid form as specified in RFC 3261.

	@return		A boolean value of ETrue if uri contains valid UserInfo,
				EFalse if it does not.
 */
TBool TValidatorSip::IsValidUserInfo() const
	{
	const TDesC8& userInfo = iUri.Extract(EUriUserinfo);
	
	if (IsEmpty(userInfo))
		{
		// The '@' UserInfo sparator was found in the URI but no username/password 
		// is present
		return EFalse;
		}
	
	TInt separatorPos = userInfo.Locate(KUserPwdSeparator);
	if (separatorPos != KErrNotFound)
		{
		// seperator found so there is a username and password
		// the username is left of the separator
		if (!IsValidUser(userInfo.Left(separatorPos)))
			{
			return EFalse;
			}
		// the password is right of the separator
		return IsValidPassword(userInfo.Right(userInfo.Length() - separatorPos-1));
		}
		
	// there is no password element
	return IsValidUser(userInfo);
	}

/**
	Checks whether any duplicate parameter names exist in the 
	whole path of the sip-uri.
							
	@param		aParamName	The descriptor to be checked.
	@param		aParamList	the path/parameter part of uri, which conatians all parameters and values.
	@return		A boolean value of ETrue if uri contains duplicate parameters,
				EFalse if it does not.
*/
TBool TValidatorSip::IsDuplicated(const TDesC8& aParamName, const TDelimitedParserBase8 aParamList) const
	{
	// roll back to the start of the lhs segment parser
	while(aParamList.Dec() != KErrNotFound) 
	{
	//Nothing to do
	}
	
	TPtrC8 name;
	TPtrC8 value;
	TPtrC8 segment;
	TInt count = 0;
	while( aParamList.GetNext(segment) == KErrNone )
		{
		GetNameValuePair(segment, name, value);
		if (aParamName.CompareF(name) == 0)
			{
			count++;
			}
		if (count > 1)
			{
			// The parameter name appears more than once in the paramter list
			return ETrue;
			}
		}
	return EFalse;	
	}

/**
	Checks whether parameter contains valid characters in the sip-uri as specified in RFC 3261.
							
	@param		aParam	The descriptor to be checked.
	@return		A boolean value of ETrue if uri contains valid parameter,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidParam(const TDesC8& aParam) const
	{
	return IsValidCharacters(aParam, KCharSetParamAll, ETrue);
	}

/**
	Checks whether ParamSegment contains valid characters in 
	Parameter name and Parameter value as specified in RFC 3261.
							
	@param		aName	The descriptor for parameter name to be checked as per RFC 3261.
	@param		aValue	The descriptor for value to be checked as per RFC 3261.
	@return		A boolean value of ETrue if uri contains valid parameters and values,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidParamSegment(const TDesC8& aName, const TDesC8& aValue) const
	{
	if (!aName.Length() || !IsValidParam(aName) ) 
		{
		return EFalse;	
		}
	
	/**************************************************************************************************************
	* As per RFC 3261 if the uri-parameter contains name as Tranport or User or Method then its Value must be     *
	* the characters as specified in Token. else if the name is Ttl then its value must lie in between 0 - 255.   *
	* else if the name is Maddr then its value must be a host, else if the name is lr then it must not conatin    *
	* any value, else it must be a other-parameter whose value must conatin the characters specified in paramchar.*
	* As per INC 115503 and due to IOP issue Validation of lr paramater is not performed.						  *
	***************************************************************************************************************/
	if ( ( (aName.CompareF(KTransport()) == 0 || aName.CompareF(KUser()) == 0 || aName.CompareF(KMethod()) == 0 ) 	
	   	   	  && !IsValidParamToken(aValue) )
		|| ( aName.CompareF(KTtl()) == 0 && !IsValidParamTtl(aValue) )
		|| ( aName.CompareF(KMaddr()) == 0  && !IsValidParamMaddr(aValue) )
		|| ( aName.CompareF(KTransport()) != 0 && aName.CompareF(KUser()) != 0 && aName.CompareF(KMethod()) != 0 && aName.CompareF(KTtl()) != 0 
			&& aName.CompareF(KMaddr()) != 0 && aName.CompareF(KLr()) != 0 && !IsValidParam(aValue))
		)
		{
		return EFalse;	
		}

	return ETrue;
	}

/**
	Checks whether parameters "transport", "User" and "Method" contains valid characters in the 
	sip-uri as specified in RFC 3261.
								
	@param		aParam	The descriptor to be checked.
	@return		A boolean value of ETrue if uri contains valid parameter,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidParamToken(const TDesC8& aParam) const
	{
	if (!aParam.Length())
		{
		return EFalse;	
		}
	return IsValidCharacters(aParam, KCharSetSipToken, EFalse);
	}
	
/**
	Checks whether parameter "ttl" contains valid characters in the sip-uri as specified in RFC 3261.
	
							
	@param		aParam	The descriptor to be checked.
	@return		A boolean value of ETrue if uri contains valid parameter,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidParamTtl(const TDesC8& aParam) const
	{
	if (!aParam.Length() || aParam.Length() > 3 || !IsValidCharacters(aParam, KCharSetNumber, EFalse) )
		{
		return EFalse;	
		}
	
	TLex8 lex(aParam);
	TInt address = 0;
	lex.Val(address);
	if( address > KMaxHostAddr )
		{
		return EFalse;	
		}
		
	return ETrue;
	}
	
/**
	Checks whether parameter "maddr" contains valid characters in the sip-uri as specified in RFC 3261.
							
	@param		aParam	The descriptor to be checked.
	@return		A boolean value of ETrue if uri contains valid parameter,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidParamMaddr(const TDesC8& aParam) const
	{
	if (!aParam.Length() && UriUtils::HostType(aParam) == UriUtils::ETextHost && !IsTextHostValid(aParam))
		{
		return EFalse;
		}
	return ETrue;
	}
	
/**
	Checks that a Path/uri-parameter is in a valid form as specified in RFC 3261.
	
	@return		A boolean value of ETrue if uri contains valid Path/uri-parameter,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidPath() const
	{
	const TDesC8& parameters = iUri.Extract(EUriPath);
	TDelimitedPathSegmentParser8 parser;
	parser.Parse(parameters);
	
	// sip parameters should start with a ';'
	if (parameters.Length() && parameters[0] != ';')
		{
		return EFalse;
		}
		
	TPtrC8 name;
	TPtrC8 value;
	TPtrC8 segment;
	while( parser.GetNext(segment) == KErrNone )
		{
		GetNameValuePair(segment, name, value);
		if (IsEmpty(value) || IsDuplicated(name, parser) || !IsValidParamSegment(name, value))
			{
			return EFalse;
		}
		}

	return ETrue;
	}

/**
	Checks whether Header contains valid characters as specified in RFC 3261.
							
	@param		aHeader	The descriptor to be checked as per RFC 3261.
	@return		A boolean value of ETrue if uri contains valid Header,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidHeader(const TDesC8& aHeader) const
	{
	return IsValidCharacters(aHeader, KCharSetHeaderAll, ETrue);
	}
	
/**
	Checks whether HeaderSegment contains valid name and values as specified in RFC 3261.
							
	@param		aName	The descriptor for Header name to be checked as per RFC 3261.
	@param		aValue	The descriptor for Header value to be checked as per RFC 3261.
	@return		A boolean value of ETrue if uri contains valid Header name and Header values,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidHeaderSegment(const TDesC8& aName, const TDesC8& aValue) const
	{
	if (!aName.Length() || !IsValidHeader(aName) || !IsValidHeader(aValue))
		{
		return EFalse;
		}
	return ETrue;
	}
	
/**
	Checks that a Query/Header is in a valid form as specified in RFC 3261.

	@return		A boolean value of ETrue if uri contains valid Query/Header,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidQuery() const
	{
	const TDesC8& headers = iUri.Extract(EUriQuery);
	if (IsEmpty(headers))
		{
		return EFalse;
		}

	TDelimitedQueryParser8 parser;
	parser.Parse(headers);

	TPtrC8 name;
	TPtrC8 value;
	TPtrC8 segment;
	while( parser.GetNext(segment) == KErrNone )
		{
		// must be in the form name=value even if the value part is empty
		if (segment.Locate(KEqualsSeparator) == KErrNotFound)
			{
			return EFalse;
			}
		
		GetNameValuePair(segment, name, value);
		if (IsDuplicated(name, parser) || !IsValidHeaderSegment(name, value))
			{
			return EFalse;
		}
		}
	return ETrue;
	}

/**
	Checks that a Fragment is in a valid form as specified in RFC 3261.
	Infact, SIP URIs should not contain a fragment. It it contains a 
	fragment then it is not a valid fragment.
	
	@return		A boolean value of ETrue if uri contains valid Fragment,
				EFalse if it does not.
*/
TBool TValidatorSip::IsValidFragment() const
	{
	const TDesC8& frag = iUri.Extract(EUriFragment);
	if (IsEmpty(frag))
		{
		return EFalse;
		}
	// SIP URIs should not contain a fragment
	if (frag.Length())
		{
		return EFalse;
		}
	return ETrue;
	}

	
//
//	Implementatin of partial support for tel-Uris specified in RFC 3966.   //
//	Parsing of phone number separators as specified in RFC 3966 		   //
//  section 5 will not be supported. 									   //
//  The Implementation supports only of the form tel:36789017;isub=1411    //
//  It does not support lexicographical order of parameters for Validation.//
//

/**
	Constructor.
	@param		aUri A reference to a parsed uri object.
 */
TValidatorTel::TValidatorTel(const TUriC8& aUri) 
: TValidatorBase(aUri)
	{
	}

/**
	Checks whether valid parameter names and values exist in the 
	whole path of the tel-uri.
								
	@param		aName	The descriptor for parameter name to be checked as per RFC3966.
	@param		aValue	The descriptor for value to be checked as per RFC3966.
	@return		A boolean value of ETrue if uri contains valid parameters and values,
				EFalse if it does not.
 */
TBool TValidatorTel::IsValidParamSegment(const TDesC8& aName, const TDesC8& aValue) const
	{
	//Validation of the Name
	if (!aName.Length() || !IsValidCharacters(aName, KCharSetParamAll) )
		{
		return EFalse;	
		}
	//Validation of the Value based on ISDN, EXTN, Phone-context or any.
	if( ( KIsdnSubAddress().CompareF(aName) == 0 && !IsValidCharacters(aValue, KCharSetParamAll, ETrue) ) ||
		( KExtension().CompareF(aName) == 0 && !IsValidCharacters(aValue, KCharSetNumber) ) ||
		( KContext().CompareF(aName) == 0 && !IsValidCharacters(aValue, KCharSetParamAll) ) ||
		( KIsdnSubAddress().CompareF(aName) != 0 && 
		KExtension().CompareF(aName) != 0	&&
		KContext().CompareF(aName) != 0 &&
		!IsValidCharacters(aValue, KCharSetParamAll, ETrue) ) )
		{
		return EFalse;	
		}
	return ETrue;
	}

/**
	Checks whether any duplicate parameter names exist in the 
	whole path of the tel-uri, and also checks whether the both ISDN and EXTN 
	parameters exist in tel-uri.
							
	@param		aParamName	The descriptor to be checked.
	@param		aParamList	the path part of uri, which conatians all parameters and values.
	@return		A boolean value of ETrue if uri contains duplicate parameters or 
				both isdn and extn parameters exist, EFalse if it does not.
 */
TBool TValidatorTel::IsDuplicated(const TDesC8& aParamName, const TDelimitedParserBase8 aParamList) const
	{
	// roll back to the start of the lhs segment parser
	while(aParamList.Dec() != KErrNotFound) 
		{
		//do nothing
		}
	aParamList.Inc(); //To exclude phone number from the list.
	
	TPtrC8 name;
	TPtrC8 value;
	TPtrC8 segment;
	TInt count = 0;
	while( aParamList.GetNext(segment) == KErrNone )
		{
		GetNameValuePair(segment, name, value);
		if (aParamName.CompareF(name) == 0)
			{
			count++;	
			}
		if (count > 1)
			{
			// The parameter name appears more than once in the paramter list
			return ETrue;
			}
		if( ( KIsdnSubAddress().CompareF(aParamName) == 0 && KExtension().CompareF(name) == 0 ) || 
			( KExtension().CompareF(aParamName) == 0 && KIsdnSubAddress().CompareF(name) == 0 ) )
			{
			//Both ISDN and EXTN should not exist in Uri
			return ETrue;	
			}
		}

	return EFalse;	
	}

/**
	Checks that a path is in a valid form
	
	@return	ETrue if the path is valid otherwise EFalse
 */
TBool TValidatorTel::IsValidPath() const
	{
	const TDesC8& path = iUri.Extract(EUriPath);
	//empty path is invalid
	if(!path.Length())
		{
		return EFalse;	
		}
	
	//Implementation of all the steps specified in section 2.5.2.2 
	//Validation of the path components of tel uri
	
	TDelimitedPathSegmentParser8 parser;
	parser.Parse(path);
	
	// tel parameters should start with a '+' or 'digit'
	TChar ch( path[0] );
	if(! (ch == '+' || ch.IsDigit()) )
		{
		return EFalse;	
		}
			
	TPtrC8 name;
	TPtrC8 value;
	TPtrC8 segment;
	//First segemnt must be telephone number
	if (parser.GetNext(segment) == KErrNone)
		{
		GetNameValuePair(segment, name, value);	
		//contains only digits
		if(!IsValidCharacters(name.Mid(1), KCharSetNumber))
			{
			return EFalse;	
			}
		}
	//Remainig all the segments
	while( parser.GetNext(segment) == KErrNone )
		{
		GetNameValuePair(segment, name, value);
		if(IsEmpty(segment))
			{
			return ETrue;	
			}
		if ( IsEmpty(value) || IsDuplicated(name, parser) || !IsValidParamSegment(name, value) )
			{
			return EFalse;	
			}
		}
	return ETrue;
	}

/**
	Checks that a host is in a valid form
	
	@return	ETrue. "tel" uri does not conatin host
 */
TBool TValidatorTel::IsValidHost() const
	{
	//do nothing
	return ETrue;
	}

/**
	Checks that a UserInfo is in a valid form
	
	@return	ETrue. "tel" uri does not conatin UserInfo
 */
TBool TValidatorTel::IsValidUserInfo() const
	{ 
	//do nothing
	return ETrue;
	}

/**
	Checks that a Port is in a valid form
	
	@return	ETrue. "tel" uri does not conatin Port
 */
TBool TValidatorTel::IsValidPort() const
	{
	//do nothing
	return ETrue;
	}

/**
	Checks that a Query is in a valid form
	
	@return	ETrue. "tel" uri does not conatin Query
 */
TBool TValidatorTel::IsValidQuery() const
	{
	//do nothing
	return ETrue;
	}

/**
	Checks that a Fragment is in a valid form
	
	@return	ETrue. "tel" uri does not conatin Fragment
 */
TBool TValidatorTel::IsValidFragment() const
	{
	//do nothing
	return ETrue;
	}

