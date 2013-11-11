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

#include "TEquiv.h"
#include <uriutils.h>
#include <uriutilscommon.h>
#include "UriUtilsInternal.h"
#include <delimitedpathsegment8.h>
#include <delimitedquery8.h>
#include <escapeutils.h>

_LIT8(KParamUserFull,	";user=");
_LIT8(KParamTtlFull,	";ttl=");
_LIT8(KParamMethodFull,	";method=");
_LIT8(KParamMaddrFull,	";maddr=");
_LIT8(KParamMaddr,		"maddr");

_LIT8(KHeaderId, "call-id");
_LIT8(KHeaderIdAbbr, "i");
_LIT8(KHeaderContact, "contact");
_LIT8(KHeaderContactAbbr, "m");
_LIT8(KHeaderEncoding, "content-encoding");
_LIT8(KHeaderEncodingAbbr, "e");
_LIT8(KHeaderLength, "content-length");
_LIT8(KHeaderLengthAbbr, "l");
_LIT8(KHeaderType, "content-type");
_LIT8(KHeaderTypeAbbr, "c");
_LIT8(KHeaderFrom, "from");
_LIT8(KHeaderFromAbbr, "f");
_LIT8(KHeaderSubject, "subject");
_LIT8(KHeaderSubjectAbbr, "s");
_LIT8(KHeaderTo, "to");
_LIT8(KHeaderToAbbr, "t");

TEquiv::TEquiv(const TUriC8& aLhs, const TUriC8& aRhs)
: iLhs(aLhs), iRhs(aRhs)
	{
	}

TBool TEquiv::EquivalentL() const
	{
	if (!IsMatchSchemeL())
		return KUriUtilsErrDifferentScheme;
	
	if (!IsMatchUserInfoL())
		return KUriUtilsErrDifferentUserInfo;
	
	if (!IsMatchHostL())
		return KUriUtilsErrDifferentHost;
	
	if (!IsMatchPortL())
		return KUriUtilsErrDifferentPort;
	
	if (!IsMatchPathL())
		return KUriUtilsErrDifferentPath;
	
	if (!IsMatchQueryL())
		return KUriUtilsErrDifferentQuery;
	
	if (!IsMatchFragmentL())
		return KUriUtilsErrDifferentFragment;
	
	return KErrNone;
	}

TBool TEquiv::IsMatchSchemeL() const
	{
	HBufC8* lhsDes = DecodedSegmentLC(iLhs, EUriScheme);
	HBufC8* rhsDes = DecodedSegmentLC(iRhs, EUriScheme);
	TBool result = IsMatchCaseless(*lhsDes, *rhsDes);
	CleanupStack::PopAndDestroy(2);
	return result;
	}

TBool TEquiv::IsMatchUserInfoL() const
	{
	HBufC8* lhsDes = DecodedSegmentLC(iLhs, EUriUserinfo);
	HBufC8* rhsDes = DecodedSegmentLC(iRhs, EUriUserinfo);
	TBool result = IsMatchCaseSensitive(*lhsDes, *rhsDes);
	CleanupStack::PopAndDestroy(2);
	return result;
	}

TBool TEquiv::IsMatchHostL() const
	{
	HBufC8* lhsDes = DecodedSegmentLC(iLhs, EUriHost);
	HBufC8* rhsDes = DecodedSegmentLC(iRhs, EUriHost);
	TBool result = IsMatchCaseless(*lhsDes, *rhsDes);
	CleanupStack::PopAndDestroy(2);
	return result;
	}

TBool TEquiv::IsMatchPortL() const
	{
	HBufC8* lhsDes = DecodedSegmentLC(iLhs, EUriPort);
	HBufC8* rhsDes = DecodedSegmentLC(iRhs, EUriPort);
	TBool result = IsMatchCaseSensitive(*lhsDes, *rhsDes);
	CleanupStack::PopAndDestroy(2);
	return result;
	}

TBool TEquiv::IsMatchPathL() const
	{
	HBufC8* lhsDes = DecodedSegmentLC(iLhs, EUriPath);
	HBufC8* rhsDes = DecodedSegmentLC(iRhs, EUriPath);
	TBool result = IsMatchCaseSensitive(*lhsDes, *rhsDes);
	CleanupStack::PopAndDestroy(2);
	return result;
	}

TBool TEquiv::IsMatchQueryL() const
	{
	HBufC8* lhsDes = DecodedSegmentLC(iLhs, EUriQuery);
	HBufC8* rhsDes = DecodedSegmentLC(iRhs, EUriQuery);
	TBool result = IsMatchCaseSensitive(*lhsDes, *rhsDes);
	CleanupStack::PopAndDestroy(2);
	return result;
	}

TBool TEquiv::IsMatchFragmentL() const
	{
	HBufC8* lhsDes = DecodedSegmentLC(iLhs, EUriFragment);
	HBufC8* rhsDes = DecodedSegmentLC(iRhs, EUriFragment);
	TBool result = IsMatchCaseSensitive(*lhsDes, *rhsDes);
	CleanupStack::PopAndDestroy(2);
	return result;
	}

HBufC8* TEquiv::DecodedSegmentLC(const TUriC8& aUri, TUriComponent aSegmentType) const
	{
	HBufC8* decoded = EscapeUtils::EscapeDecodeL(aUri.Extract(aSegmentType));
	CleanupStack::PushL(decoded);
	return decoded;
	}

TEquivSip::TEquivSip(const TUriC8& aLhs, const TUriC8& aRhs)
: TEquiv(aLhs, aRhs)
	{
	}

void TEquivSip::RemoveLeadingZeros(TPtr8 aHost) const
	{
	for (TInt i=0; i < aHost.Length(); i++)
		{
		TBool startOfNumber = (i == 0 || aHost[i-1] < '0' || aHost[i-1] > '9');
		if (aHost[i] == '0' && startOfNumber)
			{
			// The character is a zero and is either at the start of the string
			// or the fist number in a sequence.
			aHost.Delete(i--,1);
			}
		}
	}

TBool TEquivSip::IsMatchHostL(const TDesC8& aLhs, const TDesC8& aRhs) const
	{
	UriUtils::TUriHostType lhsType = UriUtils::HostType(aLhs);
	UriUtils::TUriHostType rhsType = UriUtils::HostType(aRhs);
	if (lhsType != rhsType)
		return EFalse;
	if (lhsType != UriUtils::ETextHost)
		{
		// Host is IPv4 or IPv6
		// Create a copy of the hosts and remove any leading '0's
		HBufC8* lhsCopy = aLhs.AllocLC();
		HBufC8* rhsCopy = aRhs.AllocLC();
		RemoveLeadingZeros(lhsCopy->Des());
		RemoveLeadingZeros(rhsCopy->Des());
		TBool result = IsMatchCaseSensitive(*lhsCopy, *rhsCopy);
		CleanupStack::PopAndDestroy(2);
		return result;
		}

	return IsMatchCaseless(aLhs, aRhs);
	}

TBool TEquivSip::IsMatchHostL() const
	{
	HBufC8* lhsDes = DecodedSegmentLC(iLhs, EUriHost);
	HBufC8* rhsDes = DecodedSegmentLC(iRhs, EUriHost);
	TBool result = IsMatchHostL(*lhsDes, *rhsDes);
	CleanupStack::PopAndDestroy(2);
	return result;
	}

TBool TEquivSip::IsParamCompatibleL(const TDesC8& aLhsName, const TDesC8& aLhsValue, const TDesC8& aRhsName, const TDesC8& aRhsValue) const
	{
	// Were're just checking that if the segments have the same name part
	// that their values are the same. This method only returns false if the lhs and rhs
	// have the same name but their values are different.
	if (!IsMatchCaseless(aLhsName, aRhsName))
		{
		// Names don't match so we're OK
		return ETrue;
		}

	// The maddr parameter needs to be handled differently from the others
	// The address is checked for equivalence not just a straight string compare.
	if (IsMatchCaseless(aLhsName, KParamMaddr))
		{
			return IsMatchHostL(aLhsValue, aRhsValue);
		}
	else
		{
		if (!IsMatchCaseless(aLhsValue, aRhsValue))
			{
			// Names are the same but the values are different
			// We have a incompatible parameter
			return EFalse;
			}
		}

	return ETrue;
	}

TBool TEquivSip::IsParamListCompatibleL(const TDelimitedParserBase8& aLhsParser, const TDelimitedParserBase8& aRhsParser) const

	{
	TPtrC8 lhsName;
	TPtrC8 lhsValue;
	TPtrC8 rhsName;
	TPtrC8 rhsValue;

	TPtrC8 lhsSegment;
	TPtrC8 rhsSegment;

	// roll back to the start of the lhs segment parser
	aLhsParser.Reset();
	
	while( aLhsParser.GetNext(lhsSegment) == KErrNone )
		{
		// roll back to the start of the rhs segment parser
		aRhsParser.Reset();

		GetNameValuePair(lhsSegment, lhsName, lhsValue);
		while( aRhsParser.GetNext(rhsSegment) == KErrNone )
			{
			GetNameValuePair(rhsSegment, rhsName, rhsValue);
			if (!IsParamCompatibleL(lhsName, lhsValue, rhsName, rhsValue))
				return EFalse;
			}
		}

	return ETrue;
	}

TInt TEquivSip::ListLength(const TDelimitedParserBase8& aParser) const
	{
	aParser.Reset();
	
	TInt result = 0;
	while (!aParser.Eos())
		{
		aParser.Inc();
		++result;
		}
		
	aParser.Reset();
	return result;
	}

TBool TEquivSip::IsMatchPathL() const
	{
	HBufC8* lhs = DecodedSegmentLC(iLhs, EUriPath);
	HBufC8* rhs = DecodedSegmentLC(iRhs, EUriPath);

	TDelimitedPathSegmentParser8 lhsParser;
	lhsParser.Parse(*lhs);
	TDelimitedPathSegmentParser8 rhsParser;
	rhsParser.Parse(*rhs);
	
	// Check each parameter in the lhs parameter list with those in
	// the rhs parameter list. If at any point a parameter is incompatible
	// we'll return false.
	TBool result = ETrue;
	
	// Alway check the parameter list with the most parameters against the other
	// so that we don't miss any
	TInt lhsLength = ListLength(lhsParser);
	TInt rhsLength = ListLength(rhsParser);
	
	if (lhsLength > rhsLength)
		{
		result = IsParamListCompatibleL(lhsParser, rhsParser);
		}
	else
		{
		result = IsParamListCompatibleL(rhsParser, lhsParser);
		}

	// check that the special parameters, if present, are present in both
	if (result)
		{
		if ((lhs->Find(KParamUserFull) == KErrNotFound) != (rhs->Find(KParamUserFull) == KErrNotFound) ||
		    (lhs->Find(KParamTtlFull) == KErrNotFound) != (rhs->Find(KParamTtlFull) == KErrNotFound) ||
		    (lhs->Find(KParamMethodFull) == KErrNotFound) != (rhs->Find(KParamMethodFull) == KErrNotFound) ||
		    (lhs->Find(KParamMaddrFull) == KErrNotFound) != (rhs->Find(KParamMaddrFull) == KErrNotFound) )
			{
			result = EFalse;
			}
		}
	
	CleanupStack::PopAndDestroy(2);
	return result;
	}


TEquivSip::THeaderType TEquivSip::HeaderType(const TDesC8& aHeaderName) const
	{
	if (IsMatchCaseless(aHeaderName, KHeaderId) || IsMatchCaseless(aHeaderName, KHeaderIdAbbr))
		return EHeaderId;
	if (IsMatchCaseless(aHeaderName, KHeaderContact) || IsMatchCaseless(aHeaderName, KHeaderContactAbbr))
		return EHeaderContact;
	if (IsMatchCaseless(aHeaderName, KHeaderEncoding) || IsMatchCaseless(aHeaderName, KHeaderEncodingAbbr))
		return EHeaderEncoding;
	if (IsMatchCaseless(aHeaderName, KHeaderLength) || IsMatchCaseless(aHeaderName, KHeaderLengthAbbr))
		return EHeaderLength;
	if (IsMatchCaseless(aHeaderName, KHeaderType) || IsMatchCaseless(aHeaderName, KHeaderTypeAbbr))
		return EHeaderType;
	if (IsMatchCaseless(aHeaderName, KHeaderFrom) || IsMatchCaseless(aHeaderName, KHeaderFromAbbr))
		return EHeaderFrom;
	if (IsMatchCaseless(aHeaderName, KHeaderSubject) || IsMatchCaseless(aHeaderName, KHeaderSubjectAbbr))
		return EHeaderSubject;
	if (IsMatchCaseless(aHeaderName, KHeaderTo) || IsMatchCaseless(aHeaderName, KHeaderToAbbr))
		return EHeaderTo;

	return EHeaderNormal;
	}

TBool TEquivSip::IsMatchHeader(const TDesC8& aLhs, const TDesC8& aRhs) const
	{
	if (IsMatchCaseless(aLhs, aRhs))
		{
		// identical headers are always OK
		return ETrue;
		}

	// We now need to check for abbreviated headers
	TPtrC8 lhsName;
	TPtrC8 lhsValue;
	TPtrC8 rhsName;
	TPtrC8 rhsValue;

	GetNameValuePair(aLhs, lhsName, lhsValue);
	GetNameValuePair(aRhs, rhsName, rhsValue);

	if (!IsMatchCaseless(lhsValue, rhsValue))
		{
		// headers with different values can never match
		return EFalse;
		}

	// We now have only those with different header names but with the same value
	// The last check is to see if the headers are in abbreviated forms
	THeaderType lhsType = HeaderType(lhsName);
	THeaderType rhsType = HeaderType(rhsName);
	if (lhsType != EHeaderNormal && (lhsType == rhsType))
		{
		// They are both special headers of the same type
		// Everything matches
		return ETrue;
		}

	return EFalse;
	}

TBool TEquivSip::IsQueryListCompatible(const TDelimitedParserBase8& aLhsParser, const TDelimitedParserBase8& aRhsParser) const

	{
	TPtrC8 lhsSegment;
	TPtrC8 rhsSegment;

	TBool found = EFalse;
	// roll back to the start of the lhs segment parser
	aLhsParser.Reset();

	while( aLhsParser.GetNext(lhsSegment) == KErrNone )
		{
		// roll back to the start of the rhs segment parser
		aRhsParser.Reset();
		
		found = EFalse;

		while( aRhsParser.GetNext(rhsSegment) == KErrNone )
			{
			if (IsMatchHeader(lhsSegment, rhsSegment))
				{
				// a match has been found for this header so move to the next
				// header in the lhs list
				found = ETrue;
				break;
				}
			}
		if (!found)
			{
			// no match has been found so the headers are not equvalent
			return EFalse;
			}
		}

	return ETrue;
	}

TBool TEquivSip::IsMatchQueryL() const
	{
	HBufC8* lhs = DecodedSegmentLC(iLhs, EUriQuery);
	HBufC8* rhs = DecodedSegmentLC(iRhs, EUriQuery);

	TDelimitedQueryParser8 lhsParser;
	lhsParser.Parse(*lhs);
	TDelimitedQueryParser8 rhsParser;
	rhsParser.Parse(*rhs);

	TBool result = EFalse;
	
	// first check that the number of headers are the same in both lists.
	TInt lhsLength = ListLength(lhsParser);
	TInt rhsLength = ListLength(rhsParser);
	if (lhsLength == rhsLength)
		{
		// Check each parameter in the lhs parameter list with those in
		// the rhs parameter list. If at any point a parameter is incompatible
		// we'll return false.
		result = IsQueryListCompatible(lhsParser, rhsParser);
		}
	
	CleanupStack::PopAndDestroy(2);
	return result;
	}

TBool TEquivSip::IsMatchFragmentL() const
	{
	// We don't care about the fragment for SIP URIs
	return ETrue;
	}
