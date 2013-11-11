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
// 
//

/**
 @file UriUtilsInternal.h
*/

#ifndef __URIUTILSINTERNAL_H__
#define __URIUTILSINTERNAL_H__

// System includes
//
#include <e32base.h>
#include <uriutils.h>

// Constants
//
_LIT(KSipScheme, "sip");
_LIT(KSipsScheme, "sips");
_LIT8(KSipScheme8, "sip");
_LIT8(KSipsScheme8, "sips");
_LIT8(KTelScheme8, "tel");	

const TInt KUriNetworkAuthorityDelimiterLength	= 2;

/**
	enum specifies scheme type whether it is Sip or Tel
	@internalComponent
	@released
*/
enum TUriSchemeType
	{
	ESchemeTypeUnknown,
	ESchemeTypeSip,
	ESchemeTypeTel 
	};

template<class TDesCType>
LOCAL_C TBool CheckForExcludedChars(const TDesCType& aData);


template<class TDesCType>
LOCAL_C UriUtils::TUriHostType CheckHostType(const TDesCType& aHost);

template<class TDesCType>
LOCAL_C TBool CheckValidTextHost(const TDesCType& aHost);

LOCAL_C void PercentEncodeL(CUri8* aNormalisedUri);
LOCAL_C void CaseNormaliseL(CUri8* aNormalisedUri);
LOCAL_C void RemoveDotSegmentsL(CUri8* aNormalisedUri);

//supporting methods for case normalisation
LOCAL_C void DoPercentEncodeL(CUri8* aNormalisedUri, TUriComponent aComponent);
LOCAL_C void DoCaseNormaliseL(CUri8* aNormalisedUri, TUriComponent aComponent);
LOCAL_C void updateStrings(TPtr8& inputBuf, TPtr8& outPutBuf, TInt length);
LOCAL_C TBool ValidateAndConvertPercentEncodedTriple(TDesC8& aData, TDes8& aCaseNormalizedData);
void RemoveExtraneousDotSegmentsL(HBufC8* aUriInputPath);

TBool IsNetworkScheme(const TDesC8& aScheme);
TBool IsNetworkScheme(const TDesC16& aScheme);

TUriSchemeType SchemeType(const TDesC8& aScheme);
TUriSchemeType SchemeType(const TDesC16& aScheme);

TBool IsTextHostValid(const TDesC8& aHost);
TBool IsTextHostValid(const TDesC16& aHost);
void GetNameValuePair(const TDesC8& aSegment, TPtrC8& aName, TPtrC8& aValue);

#endif	// __URIUTILSINTERNAL_H__
