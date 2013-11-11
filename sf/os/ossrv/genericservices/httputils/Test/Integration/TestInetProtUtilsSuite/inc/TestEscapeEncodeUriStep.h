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
// Contains declaration of CTestEsacpeEncodeUriStep class
// 
//

#ifndef __TEST_ESCAPEENCODEURI_STEP_H__
#define __TEST_ESCAPEENCODEURI_STEP_H__

#include "TestInetProUtilsUriServer.h"
// System Include
#include <escapeutils.h>

// Literal constant defined for identifying step name
_LIT(KTestEscapeEncodeUriStep, "TestEscapeEncodeUriStep");

/**
  This test step is to test the escape encoding of an URI.
  @internalTechnology
  @test
*/
class CTestEscapeEncodeUriStep : public CTestStep
	{

public:
	// Construction
	CTestEscapeEncodeUriStep();

	//Destruction
	~CTestEscapeEncodeUriStep();

	// TEF pure virtual
	virtual enum TVerdict 	doTestStepL();

protected: // Methods
    void EscEncodeAndCompareUriL(const TDesC& aDecodeComp, const TDesC& aExpEscEncodedUrl, const TDesC& aExpEncodedAut, const TDesC& aExpEncodedPath, const TDesC& aExpEncodedQuery, const TDesC& aExpEncodedNormal);
	void TestComponentEncodingL(const TDesC8& aDecodeComp, const TDesC& aExpEncodedComponent, EscapeUtils::TEscapeMode aMode );
	
	};

#endif		// __TEST_ESCAPEENCODEURI_STEP_H__



