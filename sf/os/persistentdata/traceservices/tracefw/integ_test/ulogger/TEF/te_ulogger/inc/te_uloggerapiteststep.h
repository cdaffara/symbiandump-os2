/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_uloggerapiteststep.h
 @internalTechnology
*/
#if (!defined __TE_ULOGGERAPITESTSTEP_H__)
#define __TE_ULOGGERAPITESTSTEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggermclsuitestepbase.h"
#include <e32base.h>
#include <e32math.h>
#include <uloggerclient.h>
#include "te_setfilterparameters.h"
#include "te_utracecmds.h"
#include "te_utracekernelcmds.h"



/*should put in a general filter test step class do derive the primary and secondary tests from*/
/*pass the filter values, have get/setfilter(0) as members*/
/*derive the relavent primary/secondary specifics*/

class CULoggerAPIPrimTestStep : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerAPIPrimTestStep();
	~CULoggerAPIPrimTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};
	
class CULoggerAPISecondTestStep : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerAPISecondTestStep();
	~CULoggerAPISecondTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};
	
	
/*import UTrace code for this - sandwich e.g UTraceFunctionStep between ULooger start/stop*/
/* include more than one type of primary and secondary filter, ensure corresponding and unrelated tracing available from UTrace*/
class CULoggerAPILoggingTestStep : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerAPILoggingTestStep();
	~CULoggerAPILoggingTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

class CULoggerAPIKernelTestStep : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerAPIKernelTestStep();
	~CULoggerAPIKernelTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};
	

class CULoggerAPIPrintfTestStep : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerAPIPrintfTestStep();
	~CULoggerAPIPrintfTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

class CULoggerAPIKernelPrintfTestStep : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerAPIKernelPrintfTestStep();
	~CULoggerAPIKernelPrintfTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};


_LIT(KULoggerAPIPrimTestStep,"ULoggerAPIPrimTestStep");
_LIT(KULoggerAPISecondTestStep,"ULoggerAPISecondTestStep");
_LIT(KULoggerAPILoggingTestStep,"ULoggerAPILoggingTestStep");
_LIT(KULoggerAPIKernelTestStep,"ULoggerAPIKernelTestStep");
_LIT(KULoggerAPIPrintfTestStep,"ULoggerAPIPrintfTestStep");
_LIT(KULoggerAPIKernelPrintfTestStep,"ULoggerAPIKernelPrintfTestStep");

#endif
