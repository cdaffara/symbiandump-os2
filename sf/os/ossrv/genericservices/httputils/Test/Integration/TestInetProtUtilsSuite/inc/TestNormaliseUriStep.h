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
// Contains declaration of CTestNormaliseUriStep class
// 
//

#ifndef __TEST_NORMALISEURI_STEP_H__
#define __TEST_NORMALISEURI_STEP_H__

#include "TestInetProUtilsUriServer.h"

// Literal constant defined for identifying step name
_LIT(KTestNormaliseUriStep, "TestNormaliseUriStep");
_LIT(KTestOomNormaliseUriStep, "TestOomNormaliseUriStep");

/**
  This test step is to test the syntax normalisation of an URI.
  @internalTechnology
  @test
*/
class CTestNormaliseUriStep : public CTestStep
	{

public:
	// Construction
	CTestNormaliseUriStep();

	//Destruction
	~CTestNormaliseUriStep();

	// TEF pure virtual
	virtual enum TVerdict 	doTestStepL();

protected: // Methods
	void NormaliseAndCompareUriL(const TDesC& aUri, const TDesC& aExpUri);
	
	};

/**
  This test step is to test the syntax normalisation of an URI and OOM test.
  @internalTechnology
  @test
*/
class CTestNormaliseUriOomStep : public CTestStep
	{

public:
	// Construction
	CTestNormaliseUriOomStep();

	//Destruction
	~CTestNormaliseUriOomStep();

	// TEF pure virtual
	virtual enum TVerdict 	doTestStepL();

	};
	
#endif		// __TEST_NORMALISEURI_STEP_H__

