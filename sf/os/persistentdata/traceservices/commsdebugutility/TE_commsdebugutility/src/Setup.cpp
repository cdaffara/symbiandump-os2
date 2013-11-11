// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// See the test specification for details of what these test cases do.


#include <f32file.h>

#include "TE_comsdbgServer.h"
#include "TestMessage.h"
#include "Setup.h"





/**
* Function  Name	: CFloggerTest_Setup
* Input parameters	: None
* Output parameters	: None
* Description		: This is the constructor
*/

CFloggerTest_Setup::CFloggerTest_Setup()
	{
	// store the name of this test case
	SetTestStepName(_L("Setup_File"));
	}


/**
* Function  Name	: ~CFloggerTest_Setup
* Input parameters	: None
* Output parameters	: None
* Description		: This is the destructor
*/


CFloggerTest_Setup::~CFloggerTest_Setup()
	{
	}

/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function sets the flogger.ini file to run the test 
						harness for flogger
*/




TVerdict CFloggerTest_Setup::doTestStepL( )
	{

	TInt ret = KErrNone;
	TInt r;
	RFs fileSystem;	
	ret = fileSystem.Connect();
	if (ret != 0)
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	
	CFileMan *fman = CFileMan::NewL(fileSystem);
	// rename flogger.ini to a backup name. If not found, ignore.
	// overwrite backup if found.
	ret = fileSystem.SetAtt(KFloggerIniFile,KEntryAttNormal, KEntryAttReadOnly);

	ret = fman->Rename(KFloggerIniFile, KTempPreserveFloggerIniFile, CFileMan::EOverWrite );
	
	TRAP(r,constructFloggerIniL(KDefaultIniFileSettings));
	
	if (r == KErrNone)
		{
		ret = fileSystem.SetAtt(KFloggerIniFile,KEntryAttNormal, KEntryAttReadOnly);
		}
	else
		{
		ret = r;
		}
	delete fman;
	fileSystem.Close();
	
	// Check to see if the setup step was successful.
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("Setup completed."));
		}
	else
		{
		INFO_PRINTF2(_L("Setup failed with error %d."), ret);		}

	if (ret == 0)
		{
		SetTestStepResult(EPass);
		}
	else 
		{
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}	




/**
Simply initializes the flogger.ini to the default test config.
*/

CFloggerTest_InitializeIniFile::CFloggerTest_InitializeIniFile()
	{
	// store the name of this test case
	SetTestStepName(_L("Setup_InitializeIniFile"));
	}


CFloggerTest_InitializeIniFile::~CFloggerTest_InitializeIniFile()
	{
	}



TVerdict CFloggerTest_InitializeIniFile::doTestStepL( )
	{
	
	TRAPD(ret,constructFloggerIniL(KDefaultIniFileSettings));
	
	if (ret == KErrNone)
		{
         SetTestStepResult(EPass);		return TestStepResult();
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}	





/**
* Function  Name	: CFloggerTest_Restore
* Input parameters	: None
* Output parameters	: None
* Description		: This is the constructor
*/


CFloggerTest_Restore::CFloggerTest_Restore()
	{
	// store the name of this test case
	SetTestStepName(_L("Restore_File"));
	}


/**
* Function  Name	: ~CFloggerTest_Restore
* Input parameters	: None
* Output parameters	: None
* Description		: This is the destructor
*/


CFloggerTest_Restore::~CFloggerTest_Restore()
	{
	}

/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function restores the orginal flogger.ini file 
*/



TVerdict CFloggerTest_Restore::doTestStepL( )
	{
	RFs fileSystem;
	TInt ret = 0;
	ret = fileSystem.Connect();
	if (ret == KErrNone)
		{
		// if replace fails, ignore since most likely the preserved file is not there
		fileSystem.Replace(KTempPreserveFloggerIniFile, KFloggerIniFile);
		}
	fileSystem.Close();	
		
	if (ret == 0)
		{
         SetTestStepResult(EPass);		return TestStepResult();
		}
	else 
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}	



