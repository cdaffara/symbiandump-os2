/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
 @file testruloggerapi_ext.h
 @internalTechnology
*/
#if (!defined __TESTRULOGGERAPI_EXT_H__)
#define __TESTRULOGGERAPI_EXT_H__
#include <test/testexecutestepbase.h>
#include "te_uloggerclientsuitestepbase.h"
#include <uloggerclient.h>
using namespace Ulogger;

class CTestRULoggerAPIStep : public CTestUloggerClientApiStepBase
	{
public:
	CTestRULoggerAPIStep();
	~CTestRULoggerAPIStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	TInt SetGetPrimaryFiletr_TestL();
	TInt SetGetSecondaryFiletr_TestL();
	TInt SetGetSecondaryFiltering_TestL();
	TInt SetGetOutputPlugin_TestL();
	TInt GetInstalledOutputPlugins_TestL();
	TInt SetGetInputPlugin_TestL();
	TInt GetInstalledInputPlugins_TestL();
	TInt SetGetRemovePluginConf_TestL();
	TInt SetGetTraceBuffer_TestL();
	TInt SetGetDnsBuffer_TestL();
	TInt SetGetBufferMode_TestL();
	
	//data
	RULogger iUlogger;
	};

_LIT(KTestRULoggerAPIStep, "TestRULoggerAPIStep");

#endif
