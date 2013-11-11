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
// Contains declaration of CTestTelUriParsingStep class
// 
//

#ifndef __TEST_TELURI_PARSING_STEP_H__
#define __TEST_TELURI_PARSING_STEP_H__

#include "TestInetProUtilsUriServer.h"
#include <uri8.h>

// Literal constant defined for identifying step name
_LIT(KTestTelUriParsingStep, "TestTelUriParsingStep");

/**
This test step is to test the Tel uri parsing.
@internalTechnology
@test
*/

class CTestTelUriParsingStep : public CTestStep
	{
public:
	// Construction
	CTestTelUriParsingStep();

	//Destruction
	~CTestTelUriParsingStep();

	// TEF pure virtual
	virtual TVerdict doTestStepL();
	
protected: // Methods
	void TestTelUriParsingAndCompareL(const TDesC16& aUri, const TDesC16& aScheme, const TDesC16& aPath);
	TInt TestComponentL(const TUriParser8& aParser, const TDesC16& aExpectedComponent, TUriComponent aComponent);
	
	};


#endif		// __TEST_TELURI_PARSING_STEP_H__




