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
// Contains declaration of CTestTelUriValidationStep class
// 
//

#ifndef __TEST_TELURI_VALIDATION_STEP_H__
#define __TEST_TELURI_VALIDATION_STEP_H__

#include "TestInetProUtilsUriServer.h"

// Literal constant defined for identifying step name
_LIT(KTestTelUriValidationStep, "TestTelUriValidationStep");
_LIT(KTestOomTelUriValidationStep, "TestOomTelUriValidationStep");

/**
  This test step is to test the Tel uri validation.
  @internalTechnology
  @test
*/
class CTestTelUriValidationStep : public CTestStep
	{

public:
	// Construction
	CTestTelUriValidationStep();

	//Destruction
	~CTestTelUriValidationStep();

	// TEF pure virtual
	virtual enum TVerdict 	doTestStepL();
	
protected: // Methods
	void TestTelUriValidateAndCompareL(const TDesC16& aTelUri, const TInt aExpRetCode);

	};

/**
  This test step is to test the Tel uri validation and performs OOM test.
  @internalTechnology
  @test
*/
class CTestTelUriValidationOomStep : public CTestStep
	{
public:
	// Construction
	CTestTelUriValidationOomStep();

	//Destruction
	~CTestTelUriValidationOomStep();

	// TEF pure virtual
	virtual enum TVerdict 	doTestStepL();
	
	};

#endif		// __TEST_TELURI_VALIDATION_STEP_H__


