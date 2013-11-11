/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This contains CTestPlatSecLaunchApp class functions implementation.
* This class sets the capabilities of the executables and libraries.
*
*/


#include "testplatseclaunchapp.h"

//TEF include
#include <test/testexecutelog.h>

/*@{*/
_LIT(KExecutableName,		"executable");
_LIT(KCommandLineParams,	"CommandLineParams");

/*@}*/

// Test step name
_LIT(KLaunchAppStep,	"LaunchApp");

/**
 Destructor
*/
CTestPlatSecLaunchApp::~CTestPlatSecLaunchApp()
	{
	}

/** 
 Constructor
*/
CTestPlatSecLaunchApp::CTestPlatSecLaunchApp()
	{
	SetTestStepName(KLaunchAppStep);
	}

/**
 The implementation of the CTestStepPlatTest::doTestStepL() pure virutal
 function.It reads the application name from Config file and tries to 
 launch the application.
 @return TVerdict - result of the test step
 @leave - system wide error codes
*/
TVerdict CTestPlatSecLaunchApp::doTestStepL()
	{
	

	TPtrC ServerName(GetServerName());
	INFO_PRINTF1(_L("Server Name"));
	INFO_PRINTF2(_L("Server Name %S"), &ServerName);
	
	//read the executable name from the ini file
	TPtrC	appName;
    TPtrC   commandLine;
	if(!GetStringFromConfig(ConfigSection(), KExecutableName, appName))
		{
		ERR_PRINTF1(_L("Executable/Server name not provided in INI file"));
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("Executable name read from the ini file is  %S"),&appName); 
        if(!GetStringFromConfig(ConfigSection(), KCommandLineParams, commandLine))
            {
            INFO_PRINTF1(_L("No Command line arguments provided"));
            }
        RProcess process;
        CleanupClosePushL(process);
        TInt err=KErrNone;
        err = process.Create(appName,commandLine);
        if(err!=KErrNone)
            {
            INFO_PRINTF2(_L("Launching the app failed with error : %D"),err);
            SetTestStepError(err);
            }
        else
            {
            TRAPD(error,process.Resume();)
                if(error!=KErrNone)
                    {
                    ERR_PRINTF2(_L("Launching the app failed with error: %D"),err);
                    SetTestStepError(err);
                    SetTestStepResult(EFail);
                    }
            }
        CleanupStack::PopAndDestroy(1);
		}
	return TestStepResult();
	}

