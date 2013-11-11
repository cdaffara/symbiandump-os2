// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file def130695_Step.cpp
 @internalTechnology
*/
#include "def130695_Step.h"
#include "def130695_SuiteDefs.h"

C_def130695_Step1::~C_def130695_Step1()
/**
 * Destructor
 */
	{
	}

C_def130695_Step1::C_def130695_Step1()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(K_def130695_Step1);
	}


/*
 * 	
 * BASESRVCS-XML-CT-4925, DEF130695
 */

TVerdict C_def130695_Step1::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
                INFO_PRINTF1(_L("Xml buffer resize -- BASESRVCS-XML-CT-4925, DEF130695"));

		TPtrC TheString;
		TInt TheInt;

                 if( !GetStringFromConfig(ConfigSection(),K_def130695_SuiteString, TheString) ||
                     !GetIntFromConfig(ConfigSection(),K_def130695_SuiteInt,TheInt) )
			{
   			  // Leave if there's any error.
			  User::Leave(KErrNotFound);
			}
		else
			{
			  INFO_PRINTF3(_L("The test step is %S, The int-value is %d"), &TheString, TheInt); // Block end
			}
               
                
      
                SetTestStepResult(EFail);       
                TInt ret = xmlBufferResize((xmlBufferPtr) TheInt, 10);  // Assuming the integer 0 as NULL got from *.ini file.

                if( ret == 0 )                 
                 {

                  INFO_PRINTF1(_L("Success : The API recoginized the NULL pointer passed"));
  		  SetTestStepResult(EPass);
		 }
                else
                 {

                  INFO_PRINTF1(_L("Failure : The API did not recoginize the NULL pointer passed."));
		 }

      
  	        return TestStepResult();
	}

C_def130695_Step2::~C_def130695_Step2()
/**
 * Destructor
 */
	{
	}

C_def130695_Step2::C_def130695_Step2()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(K_def130695_Step2);
	}


/*
 * Libxml2 -- vulnerabilities from Debian Security Advisory : XmlSax2Characters( ) in libxml2_tree.c.
 * BASESRVCS-XML-CT-4926, DEF130695
 */
 
TVerdict C_def130695_Step2::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
                INFO_PRINTF1(_L("Xml sax characters parsing -- BASESRVCS-XML-CT-4926, DEF130695"));
                 
		TPtrC TheString;
		TInt TheInt;

                 if( !GetStringFromConfig(ConfigSection(),K_def130695_SuiteString, TheString) ||
                     !GetIntFromConfig(ConfigSection(),K_def130695_SuiteInt,TheInt) )
			{
   			  // Leave if there's any error.
			  User::Leave(KErrNotFound);
			}
		else
			{
			  INFO_PRINTF3(_L("The test step is %S, The int-value is %d"), &TheString, TheInt); // Block end
			}
               
                
      
                SetTestStepResult(EFail);       
                const xmlChar *ch = (const xmlChar *)"xml_string";

                INFO_PRINTF1(_L("Calling xmlSAX2Characters( ) by passing NULL as the first parameter"));
                INFO_PRINTF1(_L("If the test result is not obtainable, it is obvious that the API has not executed successfully. And it is declared that the test result is Fail(EFail). The tef Log will not be having this fail result"));
                
                xmlSAX2Characters((void*) TheInt, ch, 10);
                

		SetTestStepResult(EPass);
        
   	        return TestStepResult();
	}




