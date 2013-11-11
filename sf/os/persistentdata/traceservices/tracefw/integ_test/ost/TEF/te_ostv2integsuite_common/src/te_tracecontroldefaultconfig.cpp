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
 @file te_tracecontroldefaultconfig.cpp
 @internalTechnology
*/
#include "te_tracecontroldefaultconfig.h"
#include "te_ostv2integsuite_defs.h"

using namespace Ulogger; //CA:added so can use old ulogger api
CDefaultTraceControlConfigWrapper::~CDefaultTraceControlConfigWrapper()
/**
 * Destructor
 */
	{
	}

CDefaultTraceControlConfigWrapper::CDefaultTraceControlConfigWrapper()
/**
 * Constructor
 */
	{
	}

CDefaultTraceControlConfigWrapper* CDefaultTraceControlConfigWrapper::NewLC()
	{
	CDefaultTraceControlConfigWrapper* self = new (ELeave)CDefaultTraceControlConfigWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CDefaultTraceControlConfigWrapper* CDefaultTraceControlConfigWrapper::NewL()
	{
	CDefaultTraceControlConfigWrapper* self=CDefaultTraceControlConfigWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CDefaultTraceControlConfigWrapper::ConstructL()
	{
	}

TAny* CDefaultTraceControlConfigWrapper::GetObject()
	{
	return NULL;
	}
TBool CDefaultTraceControlConfigWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
		RULogger logger;
		TInt result =0;
	    result = logger.Connect();
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
	    configIni.DefaultL(logger);		
     	SetBlockResult(EPass);
		return ETrue;
	}
