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


/**
 @file  
 @test
*/


#if (!defined MINORSTEPS_H)
#define MINORSTEPS_H
#include <test/testexecutestepbase.h> 
#include <ups/upsclient.h>
#include "tupsclientstep.h"  
#include "tupsintegdefs.h"

_LIT(KProtServAuthorisationStep,"NonProtServAuthorisation");  
_LIT(KDyingClientStep,"DyingClient"); 
_LIT(KCancelPromptStep,"CancelPrompt"); 
_LIT(KSimpleRequestStep,"SimpleRequest"); 
_LIT(KMultithreadStep,"Multithread");


class CNonProtServAuthorisation: public CTestStep
	{
	public:
	CNonProtServAuthorisation();
	virtual ~CNonProtServAuthorisation();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	private:
	// data to be read from ini file. 
	TInt  			iExpectedError; 
	TPtrC16	 		iDestination;
	TServiceId 		iServiceUID;
	TBool			iPlatSecPass;
	};

 
class CDyingClient: public CUpsClientStep
	{
	public:
	CDyingClient();
	virtual TVerdict doTestStepL();

	private:
	
	};
	
 
class CCancelUpsPrompt: public CUpsClientStep
	{
	public:
	CCancelUpsPrompt();
	virtual TVerdict doTestStepL();

	private:
	
	};


class CSimpleUpsRequest: public CUpsClientStep
	{
	public:
	CSimpleUpsRequest();
	virtual TVerdict doTestStepL();

	private:
	
	};
	
class CMultiTread: public CUpsClientStep
	{
	public:
	CMultiTread();
	virtual TVerdict doTestStepL();

	RThread iThd;
	UserPromptService::RUpsSession iTestSession;
	TInt iErrorReturned_1;
    TPtrC iDecisionReceived_1;
	
	};


#endif
