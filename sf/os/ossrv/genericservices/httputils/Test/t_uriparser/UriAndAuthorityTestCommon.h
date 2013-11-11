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

#ifndef __URIANDAUTHORITYTESTCOMMON_H__
#define __URIANDAUTHORITYTESTCOMMON_H__

// System includes
//
#include <e32base.h>

template<class TUriParserType, class TDesCType>
TInt DoUriComponentExtraction(const TDesCType& aUri, const TDesCType& aScheme,
									  const TDesCType& aUserinfo, const TDesCType& aHost,
									  const TDesCType& aPort, const TDesCType& aPath,
									  const TDesCType& aQuery, const TDesCType& aFragment);

void DoUriComponentExtractionL(const TDesC& aUri, const TDesC& aScheme,
									  const TDesC& aUserinfo, const TDesC& aHost,
									  const TDesC& aPort, const TDesC& aPath,
									  const TDesC& aQuery, const TDesC& aFragment);

template<class TAuthorityParserType, class TDesCType>
TInt DoAuthorityComponentExtraction(const TDesCType& aAuthority, const TDesCType& aUserInfo, 
											 const TDesCType& aHost, const TDesCType& aPort);

void DoAuthorityComponentExtractionL(const TDesC& aAuthority, const TDesC& aUserInfo, 
											 const TDesC& aHost, const TDesC& aPort);


template<class TUriParserType, class CUriType, class TDesCType>
TInt DoTestResolve(const TDesCType& aBase, const TDesCType& aReference, const TDesCType& aExpected);

template<class TUriParserType, class TDesCType>
TInt DoUriCompare(const TDesCType& aUri1, const TDesCType& aUri2, TInt aMatchFlags);

// Compares the specific component against what is expected. The parser object is
// assumed to the been parsed already, otherwise it panics.
//
// In:
// aParser			- parsed parser object
// aExpected		- the expected component value
// aComponent		- the specific component
//
// Rtn: the result of the comparision - 0 is a match, non-zero is not a match.
//
template<class TParserType, class TDesCType, class TComponentType>
TInt TestComponent(const TParserType& aParser, const TDesCType& aExpected, TComponentType aComponent);

template<class TParserType, class TComponentType>
TInt TestCompare(const TParserType& aFull, const TParserType& aPartial, TComponentType aComponent);

template<class TAuthorityParserType, class TDesCType>
TInt DoAuthorityCompare(const TDesCType& aAuthority1, const TDesCType& aAuthority2, TInt aMatchFlags);

template<class TUriParserType, class TDesCType>
TInt DoUriComponentPresence(const TDesCType& aUri, TInt aPresenceFlags);

template<class TAuthorityParserType, class TDesCType>
TInt DoAuthorityComponentPresence(const TDesCType& aAuthority, TInt aPresenceFlags);

template<class TParserType, class TComponentType>
TInt TestPresence(const TParserType& aData, TComponentType aComponent, TBool aExpectedPresence);

template<class TUriParserType, class TDesCType>
TInt DoUriDes(const TDesCType& aUri);

void DoUriDesL(const TDesC& aUri);

template<class TUriParserType, class TDesCType>
TInt DoCompareUriDes(const TUriParserType& aParser, const TDesCType& aExpected);

template<class TAuthorityParserType, class TDesCType>
TInt DoAuthorityDes(const TDesCType& aAuthority);

void DoAuthorityDesL(const TDesC& aAuthority);

template<class TUriParserType, class TDesCType>
TInt DoUriSchemeValidation(const TDesCType& aUri, TBool aValidScheme);

template<class TUriParserType, class TPtrCType, class TDesCType>
TInt DoUriWithoutFragment(const TDesCType& aUri, const TDesCType& aExpected);

template<class CUriType, class TDesCType>
TInt DoUriConstruction(const TDesCType& aUri_Scheme, const TDesCType& aUri_Host, const TDesCType& aUri_Userinfo, 
							   const TDesCType& aUri_Port, const TDesCType& aUri_Path, const TDesCType& aUri_Query, 
							   const TDesCType& aUri_Fragment, const TDesCType& aScheme, const TDesCType& aHost, 
							   const TDesCType& aUserinfo, const TDesCType& aPort, const TDesCType& aPath, 
							   const TDesCType& aQuery, const TDesCType& aFragment);

template<class CUriType,  class TDesCType>
TInt DoUriSetAndCompareComponent(CUriType aUri, TUriComponent aComponent, const TDesCType& aData, const TDesCType& aExpected);

template<class CUriType, class TUriParserType, class TDesCType>
TInt DoUriDestruction(const TDesCType& aUri_NoScheme, const TDesCType& aUri_NoHost, const TDesCType& aUri_NoUserinfo, 
							  const TDesCType& aUri_NoPort, const TDesCType& aUri_NoPath, const TDesCType& aUri_NoQuery, 
							  const TDesCType& aUri_NoFragment, const TDesCType& aUri_Whole);

template<class CUriType, class TDesCType>
TInt DoUriRemoveAndCompareComponent(CUriType* aUri, TUriComponent aComponent, const TDesCType& aExpected);

template<class CAuthorityType, class TDesCType>
TInt DoAuthorityConstruction(const TDesCType& aAuthority_Host, const TDesCType& aAuthority_Userinfo, const TDesCType& aAuthority_Port, 
							   const TDesCType& aHost, const TDesCType& aUserinfo, const TDesCType& aPort);

template<class CAuthorityType, class TDesCType>
TInt DoAuthoritySetAndCompareComponent(CAuthorityType* aUri, TAuthorityComponent aComponent, const TDesCType& aData, const TDesCType& aExpected);

template<class CAuthorityType, class TDesCType>
TInt DoAuthorityEscapedConstruction(const TDesCType& aAuthority_Host, const TDesCType& aAuthority_Userinfo, const TDesCType& aAuthority_Port, 
											const TDesCType& aHost, const TDesCType& aUserinfo, const TDesCType& aPort);

template<class CAuthorityType, class TDesCType>
TInt DoAuthoritySetAndEscapeAndCompareComponent(CAuthorityType* aAuthority, TAuthorityComponent aComponent, const TDesCType& aData, const TDesCType& aExpected);

template<class CAuthorityType, class TAuthorityParserType, class TDesCType>
TInt DoAuthorityDestruction(const TDesCType& aAuthority_NoHost, const TDesCType& aAuthority_NoUserinfo, 
									const TDesCType& aAuthority_NoPort, const TDesCType& aAuthority_Whole);

template<class CAuthorityType, class TDesCType>
TInt DoAuthorityRemoveAndCompareComponent(CAuthorityType* aAuthority, TAuthorityComponent aComponent, const TDesCType& aExpected);

#endif	// __URIANDAUTHORITYTESTCOMMON_H__
