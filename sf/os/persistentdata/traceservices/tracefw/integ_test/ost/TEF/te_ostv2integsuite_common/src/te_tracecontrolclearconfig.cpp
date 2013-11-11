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
// Example CTestStep derived implementation
//



/**
 @file te_tracecontrolclearconfig.cpp
 @internalTechnology
*/
#include "te_tracecontrolclearconfig.h"
#include "te_ostv2integsuite_defs.h"

using namespace Ulogger; //CA:added so can use old ulogger api
CClearTraceControlConfigWrapper::~CClearTraceControlConfigWrapper()
/**
 * Destructor
 */
	{
	}

CClearTraceControlConfigWrapper::CClearTraceControlConfigWrapper()
/**
 * Constructor
 */
	{
	}

CClearTraceControlConfigWrapper* CClearTraceControlConfigWrapper::NewLC()
	{
	CClearTraceControlConfigWrapper* self = new (ELeave)CClearTraceControlConfigWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CClearTraceControlConfigWrapper* CClearTraceControlConfigWrapper::NewL()
	{
	CClearTraceControlConfigWrapper* self=CClearTraceControlConfigWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CClearTraceControlConfigWrapper::ConstructL()
	{
	}

TAny* CClearTraceControlConfigWrapper::GetObject()
	{
	return NULL;
	}
TBool CClearTraceControlConfigWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{

		RULogger logger;
	    TInt result =0;
        result=logger.Connect();
        INFO_PRINTF2(_L("Connection to Ulogger: %d, attempt: 1"), result);
        if (result!=KErrNone)
             {
             INFO_PRINTF1(_L("Connection to Ulogger failed. Will try to connect max. 10 times"));
             for (TInt i=2; i<12;i++)
                 {
                     User::After(1000);
                     result = logger.Connect();
                     INFO_PRINTF3(_L("Connection to Ulogger: %d, attempt: %d"), result, i);
                     if (result==KErrNone)
                     {
                     break;
                     }
                 }
             }
	    CClearConfig configIni;
	    configIni.ClearL(logger);		
     	SetBlockResult(EPass);
		return ETrue;
	}

