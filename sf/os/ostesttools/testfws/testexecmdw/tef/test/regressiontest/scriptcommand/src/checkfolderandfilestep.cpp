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
* Example CTestStep derived implementation
*
*/



/**
 @file checkfolderandfilestep.cpp
*/
#include "checkfolderandfilestep.h"

CCheckFolderAndFileStep::~CCheckFolderAndFileStep()
/**
 * Destructor
 */
	{
	}

CCheckFolderAndFileStep::CCheckFolderAndFileStep()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCheckFolderAndFileStep);
	}

TVerdict CCheckFolderAndFileStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates opening up a handle to the data saved at RChunk base using template class
 * Overwrite shared data with values and also demonstrates the locking mechanism
 */
	{
	SetTestStepResult(EFail);
	_LIT(KFilePath, "FilePath");
	//_LIT(KFolderPath, "FolderPath");
	_LIT(KExpectedResult, "ExpectedResult");
	
	TPtrC filePath;
//	TPtrC folderPath;
	TInt expectedResult;
	TInt err = KErrNone;
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TEntry entry;
	
	if (!GetStringFromConfig(ConfigSection(), KFilePath(), filePath)
			//&& !GetStringFromConfig(ConfigSection(), KFolderPath(), folderPath))
			|| !GetIntFromConfig(ConfigSection(), KExpectedResult(), expectedResult))
		{
		ERR_PRINTF1(_L("Error occurs when get parameters from config file."));
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	
/*	if (0 != filePath.Length())
		{
		err = fs.Entry(filePath, entry);
		
		}*/
	err = fs.Entry(filePath, entry);
	if (err == expectedResult)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(&fs);
	
	return TestStepResult();
	}
