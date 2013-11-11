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
 @file checkresultstep.cpp
*/
#include "checklogfilestep.h"

CCheckLogFileStep::~CCheckLogFileStep()
/**
 * Destructor
 */
	{
	}

CCheckLogFileStep::CCheckLogFileStep()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCheckLogFileStep);
	}

TVerdict CCheckLogFileStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates opening up a handle to the data saved at RChunk base using template class
 * Overwrite shared data with values and also demonstrates the locking mechanism
 */
	{
	INFO_PRINTF1(_L("Check log file."));
	SetTestStepResult(EFail);
	_LIT(KLogFilePath, "LogFilePath");
	_LIT(KCheckString, "CheckString");
	_LIT(KExpectedTimes, "ExpectedTimes");
	
	TPtrC logFilePath;
	if (!GetStringFromConfig(ConfigSection(), KLogFilePath(), logFilePath))
		{
		ERR_PRINTF1(_L("Error occured when getting log file path"));
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	
	TPtrC checkString;
	if (!GetStringFromConfig(ConfigSection(), KCheckString(), checkString))
		{
		ERR_PRINTF1(_L("Error occured when getting check string"));
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile file;
	User::LeaveIfError(file.Open(fs, logFilePath, EFileRead|EFileShareAny));
	CleanupClosePushL(file);
	TInt fileSize;
	User::LeaveIfError(file.Size(fileSize));
	HBufC8* fileBuf8 = HBufC8::NewLC(fileSize);
	TPtr8 fileBufPtr8 = fileBuf8->Des();
	file.Read(fileBufPtr8);
	TBuf<512> lineBuf;
	TLex8 fileLex8(fileBufPtr8);
	TInt times = 0;
	
	while (!fileLex8.Eos())
		{
		ReadLineL(fileLex8, lineBuf);
		if (KErrNotFound != lineBuf.Find(checkString))
			{
			++times;
			}
		}
	
	TInt expectedTimes;
	if (GetIntFromConfig(ConfigSection(), KExpectedTimes(), expectedTimes))
		{
		if (times == expectedTimes)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			ERR_PRINTF3(_L("Test Failed! Expected Times: %d, Actual Times: %d"), expectedTimes, times);
			}
		}
	
	CleanupStack::PopAndDestroy(3, &fs);
	return TestStepResult();
	}

void CCheckLogFileStep::ReadLineL(TLex8& aFileLex, TDes& aLineBuf)
	{
	aLineBuf.Zero();
	TChar ch;
	while ((ch=aFileLex.Get()) != '\n')
		{
		aLineBuf.Append(ch);
		}
	}
