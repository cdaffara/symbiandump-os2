// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mppSmokeTestStepsDom.cpp
// 
//


#include "mppsmoketeststeps.h"

#include <e32svr.h>
#include <e32def.h>

#include <xml/parser.h>
#include <xml/parserfeature.h>

//#include <string.h>
#include <charconv.h>
#include <utf.h>
#include <bautils.h>

	
TVerdict CParseBase64Encoding::doTestStepL()
	{
	CMultipartTestContainer* tester = CMultipartTestContainer::NewLC(Logger());

	TBool success;
	TPtrC inputFile;
	success = GetStringFromConfig(ConfigSection(), _L("InputFile"), inputFile);
	if (success == EFalse)
		{
		User::Leave(KErrArgument);
		}
	RFs fs;
	fs.Connect();
	success = BaflUtils::FileExists(fs, inputFile);
	if (success == EFalse)
		{
		User::Leave(KErrArgument);
		}
	fs.Close();	
	TPtrC url;
	success = GetStringFromConfig(ConfigSection(), _L("Url"), url);	
	if (success == EFalse)
		{
		User::Leave(KErrArgument);
		}
	TPtrC outputFile;
	success = GetStringFromConfig(ConfigSection(), _L("OutputFile"), outputFile);
	if (success == EFalse)
		{
		User::Leave(KErrArgument);
		}
	
	TInt ret;
	TRAP(ret, tester->ParseFileL(inputFile, url, outputFile));
	if (ret != KErrNone)
		{
		User::Leave(KErrGeneral);
		}

	// find out if the test passed or failed
	TPtrC filename1;
	success = GetStringFromConfig(ConfigSection(), _L("CompareFile1"), filename1);	
	if (success == EFalse)
		{
		User::Leave(KErrArgument);
		}
	TPtrC filename2;
	success = GetStringFromConfig(ConfigSection(), _L("CompareFile2"), filename2);
	if (success == EFalse)
		{
		User::Leave(KErrArgument);
		}
	TBool skipWhitespace = EFalse;
	success = GetBoolFromConfig(ConfigSection(), _L("SkipWhitespace"), skipWhitespace);
	if (success == EFalse)
		{
		User::Leave(KErrArgument);
		}
	
	ret = tester->CompareFilesL(filename1, filename2, skipWhitespace);

   fs.Connect();
   CleanupClosePushL(fs);
   //Must not be read-only on hardware or files cannot be
   //  deleted, set attr in .iby file. 
   User::LeaveIfError(fs.Delete(outputFile));
   CleanupStack::PopAndDestroy(1, &fs);
   fs.Close();
	
	CleanupStack::PopAndDestroy(1);
	if (ret == KErrNone)
		 {
		 SetTestStepResult(EPass);
		 return EPass;
		 }		
	else
		 {
		 SetTestStepResult(EFail);
		 INFO_PRINTF2(_L("CompareFiles error code: %d"), ret);
		 return EFail;
		 }			
	}




