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
// File contains internal classes for comparing two URIs to determine
// if they identify the same resource
// 
//

/**
 @file TEquiv.h
 @see Uri8.h
*/

#ifndef __TEQUIV_H__
#define __TEQUIV_H__

#include <uri8.h>
#include <delimitedparser8.h>

/**
	Comments : 
	
	@internalComponent
	@released
*/
class TEquiv
	{
public:
	enum THeaderType
		{
		EHeaderNormal,
		EHeaderId,
		EHeaderContact,
		EHeaderEncoding,
		EHeaderLength,
		EHeaderType,
		EHeaderFrom,
		EHeaderSubject,
		EHeaderTo
		};

public:
	TEquiv(const TUriC8& aLhs, const TUriC8& aRhs);
	TInt EquivalentL() const;

protected:
	// Default implementation is for network schemes
	// Overide the following methods where appropriate for other schemes
	virtual TBool IsMatchSchemeL() const;
	virtual TBool IsMatchUserInfoL() const;
	virtual TBool IsMatchHostL() const;
	virtual TBool IsMatchPortL() const;
	virtual TBool IsMatchPathL() const;
	virtual TBool IsMatchQueryL() const;
	virtual TBool IsMatchFragmentL() const;

	// general utility methods
	HBufC8* DecodedSegmentLC(const TUriC8& aUri, TUriComponent aSegmentType) const;
	inline TBool IsMatchCaseless(const TDesC8& aLhs, const TDesC8& aRhs) const;
	inline TBool IsMatchCaseSensitive(const TDesC8& aLhs, const TDesC8& aRhs) const;

protected:
	const TUriC8& iLhs;
	const TUriC8& iRhs;
	};

/**
	Comments : This class provides the functionality for Comparing SIP URIs.
	
	@internalComponent
	@released
*/
class TEquivSip: public TEquiv
	{
public:
	TEquivSip(const TUriC8& aLhs, const TUriC8& aRhs);
	// from TEquiv
private:
	virtual TBool IsMatchHostL() const;
	virtual TBool IsMatchPathL() const;
	virtual TBool IsMatchQueryL() const;
	virtual TBool IsMatchFragmentL() const;

	// supporting methods
	void RemoveLeadingZeros(TPtr8 aHost) const;
	TBool IsMatchHostL(const TDesC8& aLhs, const TDesC8& aRhs) const;
	TBool IsParamCompatibleL(const TDesC8& aLhsName, const TDesC8& aLhsValue, const TDesC8& aRhsName, const TDesC8& aRhsValue) const;
	TBool IsParamListCompatibleL(const TDelimitedParserBase8& aLhsParser, const TDelimitedParserBase8& aRhsParser) const;
	TInt ListLength(const TDelimitedParserBase8& aParser) const;
	TBool HasMoreParameters(const TDelimitedParserBase8& aLhsParser, const TDelimitedParserBase8& aRhsParser) const;
	THeaderType HeaderType(const TDesC8& aHeaderName) const;
	TBool IsMatchHeader(const TDesC8& aLhs, const TDesC8& aRhs) const;
	TBool IsQueryListCompatible(const TDelimitedParserBase8& aLhsParser, const TDelimitedParserBase8& aRhsParser) const;
};

inline TBool TEquiv::IsMatchCaseless(const TDesC8& aLhs, const TDesC8& aRhs) const
	{
	return (aLhs.CompareF(aRhs) == 0);
	}

inline TBool TEquiv::IsMatchCaseSensitive(const TDesC8& aLhs, const TDesC8& aRhs) const
	{
	return (aLhs.Compare(aRhs) == 0);
	}

#endif	// __TEQUIV_H__
