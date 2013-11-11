// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file addfield_logStep.cpp
*/
#include "addfield_logStep.h"
#include "Te_RFileLoggerSuiteDefs.h"

_LIT(K16BitFormatText,"logger string int = %d,  string = %S");
_LIT(K16BitString,"The String16");

Caddfield_logStep::~Caddfield_logStep()
/**
 * Destructor
 */
	{
	}

Caddfield_logStep::Caddfield_logStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kaddfield_logStep);
	}

TVerdict Caddfield_logStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_RFileLoggerSuiteStepBase::doTestStepPreambleL();
		SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Caddfield_logStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	
	if(TestStepResult()==EFail) return TestStepResult();
	INFO_PRINTF1(_L("AddField test and logging test")); 

	// OS needs time to shutdown the server in case being connected previous test
	// The delay means we get server terminated from the OS
//	
	RFileFlogger ilogger;
	TInt err=ilogger.Connect();
	if(err)
	{
		INFO_PRINTF2(_L("Connect() failed. err = %d"),err);
		SetTestStepResult(EFail);
		return TestStepResult();
	}
	err=ilogger.CreateLog(KLogFile1,RFileFlogger::ELogModeAppend);
//	err=ilogger.CreateLog(KLogFile1,RFileFlogger::ELogModeOverWrite);
	//ELogModeAppend
	if(!err)
	{
		INFO_PRINTF1(_L("xml format file open passed and test start"));
	}
	else  
	{
		ERR_PRINTF2(_L("error value = %d"),err);
		SetTestStepResult(EFail);
		return TestStepResult();

	}

	TBuf<20> buf16(K16BitString);
	TInt n=0;
	ilogger.SetLogLevel(RFileFlogger::ESevrAll);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrHigh, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrMedium, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrLow, K16BitFormatText,n++,&buf16);

	INFO_PRINTF1(_L("test ESevrErr"));
	ilogger.SetLogLevel(RFileFlogger::ESevrErr);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);

	INFO_PRINTF1(_L("test log with add fileds"));
	TInt aLength = 2;
	TExtraLogField logField[2];
	logField[0].iLogFieldName.Copy(_L("SUITE_NAME"));
	logField[0].iLogFieldValue.Copy(_L("SUITE_VALUE"));
	logField[1].iLogFieldName.Copy(_L("TEST_NAME"));
	logField[1].iLogFieldValue.Copy(_L("TEST_VALUE"));
	
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrErr, aLength, logField, K16BitFormatText,n++,&buf16);


	INFO_PRINTF1(_L("test ESevrWarn"));
	n=0;
	ilogger.SetLogLevel(RFileFlogger::ESevrWarn);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);

	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrErr, K16BitFormatText,n++,&buf16);

	INFO_PRINTF1(_L("test ESevrInfo"));
	n=0;
	ilogger.SetLogLevel(RFileFlogger::ESevrInfo);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);

	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrErr, K16BitFormatText,n++,&buf16);

	INFO_PRINTF1(_L("test ESevrAll"));
	n=0;
	ilogger.SetLogLevel(RFileFlogger::ESevrAll);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);

	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrErr, K16BitFormatText,n++,&buf16);
// txt log test
	ilogger.Close();
	err=ilogger.Connect();
	if(err)
	{
		INFO_PRINTF2(_L("Connect() failed. err = %d"),err);
		SetTestStepResult(EFail);
		return TestStepResult();
	}
	INFO_PRINTF1(_L("RFileFlogger::ELogModeAppend test ..."));
	err=ilogger.CreateLog(KLogFiletxt,RFileFlogger::ELogModeAppend);
	if(!err)
	{
	    INFO_PRINTF1(_L("RFileFlogger::ELogModeAppend testing is fine"));
		INFO_PRINTF1(_L("txt format file open passed and test start"));
	}
	else  
	{
		ERR_PRINTF2(_L("error value = %d"),err);
		SetTestStepResult(EFail);
		return TestStepResult();

	}

	n=0;
	ilogger.SetLogLevel(RFileFlogger::ESevrAll);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrHigh, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrMedium, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrLow, K16BitFormatText,n++,&buf16);

	INFO_PRINTF1(_L("test ESevrErr"));
	ilogger.SetLogLevel(RFileFlogger::ESevrErr);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);

	INFO_PRINTF1(_L("test log with add fileds"));
    aLength = 2;
	logField[0].iLogFieldName.Copy(_L("SUITE_NAME"));
	logField[0].iLogFieldValue.Copy(_L("SUITE_VALUE"));
	logField[1].iLogFieldName.Copy(_L("TEST_NAME"));
	logField[1].iLogFieldValue.Copy(_L("TEST_VALUE"));
	
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrErr, aLength, logField, K16BitFormatText,n++,&buf16);

	
	INFO_PRINTF1(_L("test ESevrWarn"));
	n=0;
	ilogger.SetLogLevel(RFileFlogger::ESevrWarn);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);

	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrErr, K16BitFormatText,n++,&buf16);

	INFO_PRINTF1(_L("test ESevrInfo"));
	n=0;
	ilogger.SetLogLevel(RFileFlogger::ESevrInfo);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);

	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrErr, K16BitFormatText,n++,&buf16);

	INFO_PRINTF1(_L("test ESevrAll"));
	n=0;
	ilogger.SetLogLevel(RFileFlogger::ESevrAll);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrAll, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrInfo, K16BitFormatText,n++,&buf16);
	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrWarn, K16BitFormatText,n++,&buf16);

	ilogger.Log(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrErr, K16BitFormatText,n++,&buf16);


	ilogger.Close();
	
	INFO_PRINTF1(_L("test finished"));
	
	return TestStepResult();
	}



TVerdict Caddfield_logStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
