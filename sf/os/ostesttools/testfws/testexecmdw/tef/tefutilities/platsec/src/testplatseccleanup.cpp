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
* This contains CTestPlatSecCleanup class functions implementation.
* This class sets the capabilities of the executables and libraries.
*
*/


#include "testplatseccleanup.h"

//TEF include
#include <test/testexecutelog.h>

/*@{*/
_LIT(KDestinationPath,		"destinationPath");
/*@}*/

// Test step name
_LIT(KCleanupStep,	"Cleanup");

/**
 Destructor
*/
CTestPlatSecCleanup::~CTestPlatSecCleanup()
	{
	}

/** 
 Constructor
*/
CTestPlatSecCleanup::CTestPlatSecCleanup()
	{
	SetTestStepName(KCleanupStep);
	}

/**
 The implementation of the CTestPlatSecCleanup::doTestStepL() pure virutal
 function. Gets the required executable (server) to be deleted. It reads 
 the  destination executable name  from the ini file and calls DeleteExtraServersL function.
 @return TVerdict - result of the test step
 @leave - system wide error codes
*/
TVerdict CTestPlatSecCleanup::doTestStepL()
	{
	INFO_PRINTF1(_L("Cleaning up the exe"));

	TPtrC ServerName(GetServerName());
	INFO_PRINTF1(_L("Server Name"));
	INFO_PRINTF2(_L("Server Name %S"), &ServerName);
	
	TPtrC destination;
	if(!GetStringFromConfig(ConfigSection(), KDestinationPath, destination))
		{
		ERR_PRINTF1(_L("Executable/Server name not provided in INI file"));
		SetTestStepResult(EFail);
		return TestStepResult();	
		}
	DeleteExtraServersL(destination);
	return TestStepResult();
	}

/**
Deletes the extra servers created with limited capabilities. Uses Plattest_Platsec_Cleanup.exe
  @param aDestination - name of the executable to be deleted
 @leave - system wide error codes
 @return None
*/
void CTestPlatSecCleanup::DeleteExtraServersL(const TDesC& aDestination)
	{
	RProcess process;
	CleanupClosePushL(process);
	TInt err=KErrNone;
	_LIT(KTEFCleanupExe, "TEFCleanup.exe");
	err = process.Create(KTEFCleanupExe,aDestination);
	if(err!=KErrNone)
		{
		ERR_PRINTF2(_L("Creating the process failed with error : %D"),err);
		SetTestStepResult(EFail);
		}
	else
		{
		TRAP(err,process.Resume();)
		if(err!=KErrNone)
			{
			ERR_PRINTF2(_L("Creating the process failed with error : %D"),err);
			SetTestStepResult(EFail);
			}
		}
	CleanupStack::PopAndDestroy(1);
	}
