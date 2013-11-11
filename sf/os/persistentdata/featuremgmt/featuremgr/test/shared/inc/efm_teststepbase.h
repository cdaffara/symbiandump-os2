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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __EFMTESTSTEPBASE__
#define __EFMTESTSTEPBASE__
                                                         
#include <test/testexecutestepbase.h>
#include "efm_test_consts.h"
/*
test type section name 
test type section indicates whether the test step is performed in normal 
or low capabilty environment
*/
_LIT(KTestLowCap, "testlowcap");
_LIT(KTestOOM, "testOOM");

//macros used for testing
#define TESTDIAGNOSTIC(cond,message)\
   if (!(cond))\
   {\
   ERR_PRINTF1( message);\
   SetTestStepResult(EFail);\
   }

#define TESTDIAGNOSTICERROR(cond,message,error)\
   if (!(cond))\
   {\
   ERR_PRINTF2( message, error );\
   SetTestStepResult(EFail);\
   }   
   
/** Test step that publishes features required by the rest of test steps
*/
_LIT(KEFMTestStepPublishFeatures, "EFMTestStepPublishFeatures");

class CEFMTestStepPublishFeatures : public CTestStep
   {
public:
   CEFMTestStepPublishFeatures();
   virtual TVerdict doTestStepL(void);
   };

/** Base class for configurable test steps
*/
class CEFMConfigurableTestStepBase : public CTestStep
   {
public:
   	virtual TVerdict doTestStepPreambleL(void);   
protected:
	virtual void CheckCondition(TBool aMainCondition, TBool aOOMCondition, TPtrC aLoggingMessage, TInt aErrorCode);

   TBool iLowCap;
   TBool iOOM;
   };    
   
#endif
