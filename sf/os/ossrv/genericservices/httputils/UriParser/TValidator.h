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
// Internal classes for PREQ748 - Adding support for the SIP scheme in URIs.
// Classes for validating URIs according to their scheme
// File contains internal classes for validatin URIs according to
// their scheme type
// 
//

/**
 @file TValidator.h
 @see Uri8.h
*/

#ifndef __TVALIDATOR_H__
#define __TVALIDATOR_H__

#include <uri8.h>
#include <delimitedparser8.h>

// Constants
//
_LIT8(KAlwaysValidChars, "-_.!~*'");
_LIT8(KCommonValidChars, "()/+$");
_LIT8(KValidUserChars, "=,;?&");
_LIT8(KValidParamChars, "[]:&");
_LIT8(KValidHeaderChars, "[]:?");
_LIT8(KUriAlwaysValidSipMarkChars, "-_.!~*'()");	
_LIT8(KUriValidSipPwdChars, "&=+$,");
_LIT8(KUriValidSipToken, "%+`"); 

const TUint32 KCharSetNumber 		= 0x001;
const TUint32 KCharSetLowerAlpha 	= 0x002;
const TUint32 KCharSetUpperAlpha 	= 0x004;
const TUint32 KCharSetAlways	 	= 0x008;
const TUint32 KCharSetCommon	 	= 0x010;
const TUint32 KCharSetUser	 		= 0x020;
const TUint32 KCharSetParam		 	= 0x040;
const TUint32 KCharSetHeader	 	= 0x080;
const TUint32 KCharSetSipMark 		= 0x100;
const TUint32 KCharSetSipPwd 		= 0x200;
const TUint32 KCharSetSipTkn		= 0x400;
const TUint32 KCharSetAlphnumberic	= KCharSetNumber | KCharSetLowerAlpha | KCharSetUpperAlpha;
const TUint32 KCharSetUserAll	 	= KCharSetUser | KCharSetAlphnumberic | KCharSetAlways | KCharSetCommon;
const TUint32 KCharSetParamAll	 	= KCharSetParam | KCharSetAlphnumberic | KCharSetAlways | KCharSetCommon;
const TUint32 KCharSetHeaderAll 	= KCharSetHeader | KCharSetAlphnumberic | KCharSetAlways | KCharSetCommon;
const TUint32 KCharSetSipPassWord = KCharSetAlphnumberic| KCharSetSipPwd | KCharSetSipMark ;
const TUint32 KCharSetSipToken	= KCharSetAlphnumberic| KCharSetAlways | KCharSetSipTkn;

/**
	Comments : This class provides Base functionality for Validation of an URI.
	which needs to be overridden by derived classes for validation Based on their RFC 
	specifications.
	
	@internalComponent
	@released
*/
class TValidatorBase
	{
public:
	TValidatorBase(const TUriC8& aUri);
	TInt Validate();

protected:
	// Implemented by derived validator classes
	virtual TBool IsValidUserInfo() const = 0;
	virtual TBool IsValidHost() const = 0;
	virtual TBool IsValidPort() const = 0;
	virtual TBool IsValidPath() const = 0;
	virtual TBool IsValidQuery() const = 0;
	virtual TBool IsValidFragment() const = 0;

	// general utility methods
	TBool IsValidCharacters(const TDesC8& aDes, TUint32 aCharTypes, TBool aEscapeValid = EFalse) const;
	TBool IsEmpty(const TDesC8& aDes) const;

private:
	TBool IsInCharSet(TText8 aChar, const TDesC8& aCharSet) const;

protected:
	const TUriC8& iUri;
	};

/**
Dependencies : TValidatorBase.
Comments : This class provides Sip Uri Validation functinality.
 		   This is implemented as specified in RFC 3261.
	
	@internalComponent
	@released
*/
class TValidatorSip: public TValidatorBase
	{
public:
	TValidatorSip(const TUriC8& aUri);
private:
	// from TValidatorBase
	TBool IsValidUserInfo() const;
	TBool IsValidHost() const;
	TBool IsValidPort() const;
	TBool IsValidPath() const;
	TBool IsValidQuery() const;
	TBool IsValidFragment() const;

	// supporting methods for the above
	TBool IsValidUser(const TDesC8& aUser) const;
	TBool IsValidPassword(const TDesC8& aPassword) const;
	TBool IsDuplicated(const TDesC8& aParamName, const TDelimitedParserBase8 aParamList) const;
	TBool IsValidParam(const TDesC8& aParam) const;
	TBool IsValidParamSegment(const TDesC8& aName, const TDesC8& aValue) const;
	TBool IsValidHeader(const TDesC8& aHeader) const;
	TBool IsValidHeaderSegment(const TDesC8& aName, const TDesC8& aValue) const;
	TBool IsValidParamToken(const TDesC8& aParam) const;
	TBool IsValidParamTtl(const TDesC8& aParam) const;
	TBool IsValidParamMaddr(const TDesC8& aParam) const;
};

/**
Dependencies : TValidatorBase.
Comments : This class provides Tel Uri Validation functinality.
 		   Only Partial support for tel-Uris specified in RFC 3966. 
		   section 5 of RFC 3966 is not supported.
	@internalComponent
	@released
*/
class TValidatorTel: public TValidatorBase
	{
public:
	TValidatorTel(const TUriC8& aUri);
private:
	// from TValidatorBase
	TBool IsValidUserInfo() const;
	TBool IsValidHost() const;
	TBool IsValidPort() const;
	TBool IsValidPath() const;
	TBool IsValidQuery() const;
	TBool IsValidFragment() const;
	
	// supporting methods for the above
	TBool IsDuplicated(const TDesC8& aParamName, const TDelimitedParserBase8 aParamList) const;
	TBool IsValidParamSegment(const TDesC8& aName, const TDesC8& aValue) const;
};

#endif	// __TVALIDATOR_H__
