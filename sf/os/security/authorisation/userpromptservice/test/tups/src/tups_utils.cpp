/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "tups_utils.h" 
 
CCloseUpsStep::CCloseUpsStep()
/**
 * Constructor
 * The use of this test step is to shutdown the ups server form a test script using tupsinteg_x
 * test step, this action will cause the closure of the decisions database used by UPS server, an
 * in consecuence allows the deletion of the DB file.   
 *
 * Note: This test step will be only effective if there is not any server connected to ups server.    
 * This test step does not delete the decision DB.
 */
	{
	SetTestStepName(KCloseUps);
	}

CCloseUpsStep::~CCloseUpsStep()
/**
 * Destructor
 */
	{
 	__UHEAP_MARKEND;
	}

TVerdict CCloseUpsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 */
	{
 	__UHEAP_MARK;
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCloseUpsStep::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
	UserPromptService::RUpsSession sTestSession;
	
	TInt err = KErrNone;
	
	// Connects to UPS server.
	err = sTestSession.Connect();
	 
	if(err == KErrNone)
		{
		// Requests UPS server shut down
     	err = sTestSession.ShutdownServer();
     	
     	sTestSession.Close();
  
    	if(err == KErrNone)
			{
			INFO_PRINTF1(_L("UtilsCloseUps: UPS Server successfully shut down"));
			}
		else
			{
			ERR_PRINTF2(_L("UtilsCloseUps: Failure to shut down UPS Server: %d"),err);
			User::Leave(err);	
			}	
		}
	else
		{
		ERR_PRINTF2(_L("UtilsCloseUps: Failure to Connect to UPS Server: %d"),err);
		User::Leave(err);
		}
	
	return TestStepResult();
	}
	
TVerdict CCloseUpsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{
	return TestStepResult();
	}
