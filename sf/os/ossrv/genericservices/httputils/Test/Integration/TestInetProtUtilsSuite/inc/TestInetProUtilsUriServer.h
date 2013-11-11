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

#ifndef __TEST__INETPROUTILS_URI_SERVER_H__
#define __TEST__INETPROUTILS_URI_SERVER_H__

// Epoc Include
// For accessing TEF classes 
#include <testexecuteserverbase.h>

// Literal constants for INI file field-names
_LIT(KIniUri, 		     "Uri" );
_LIT(KIniExpectedRetCode,"ExpRetCode");
_LIT(KRetCode, 		     "RetCode" );
_LIT(KIniExpectedUri,"ExpUri");
_LIT(KIniEscapeDecoded,"EscDecoded");
_LIT(KIniEscapeEncodedUri,"EscEncodedUri");
_LIT(KIniEscapeEncodedAuthority,"EscEncodedAut");
_LIT(KIniEscapeEncodedPath,"EscEncodedUriPath");
_LIT(KIniEscapeEncodedQuery,"EscEncodedUriQuery");
_LIT(KIniEscapeEncodedNormal,"EscEncodedUriNormal");
_LIT(KIniExpectedTelScheme,"ExpTelScheme");
_LIT(KIniExpectedTelHost,"ExpTelHost");
_LIT(KIniExpectedTelPath,"ExpTelPath");

/**
The test server. 
@internalTechnology
*/
class CTestInetProUtilsUriServer : public CTestServer
	{
public:
	// Construction
	static	CTestInetProUtilsUriServer* 	NewL();
	
	// Base class pure virtual
	virtual CTestStep* 				CreateTestStep(const TDesC& aStepName);
	
	const TPtrC 					ServerName();

protected:
	CTestStep*    CreateTestStepL(const TDesC& aStepName);

	};

#endif		// __TEST__INETPROUTILS_URI_SERVER_H__





