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
//

/**
 @file
 @internalTechnology 
*/

// User Include
#include "TestDeleteFileStep.h"

/**
Constructor. Sets the test step name
*/
CTestDeleteFileStep::CTestDeleteFileStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestDeleteFileStep);
	}

/**
Tries to delete a file mentioned in the ini file.
@internalTechnology
@param		None
@return		EPass or EFail indicating the success or failure of file deletion.
*/
TVerdict CTestDeleteFileStep::doTestStepL()
	{
	// Get file path and name from ini file
	TPtrC fileName;
	TPtrC fileType;
	TPtrC drive;
	TInt err = KErrNone;
	RFs fs;
	
	if (!GetStringFromConfig(ConfigSection(), KIniFileName, fileName))
		{
		ERR_PRINTF1(_L("Unable to read filename from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("File name = %S"), &fileName);
		
		// check whether the filetype field exists in INI
		if(GetStringFromConfig(ConfigSection(), KIniFileType, fileType))
		{
		INFO_PRINTF2(_L("File type = %S"), &fileType);
		}
		
	if(fileType == KFileTypePrivate)
		{// The file is a private file. We require the drive too, as the path is relative
		if (!GetStringFromConfig(ConfigSection(), KIniDrive, drive))
			{
			ERR_PRINTF1(_L("If file type is private, drive should be provided. Unable to read drive"));
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		TFileName fullyQualifiedName(fileName);
		// In the case of a secure vesrion of the OS
		// As the INI file contains relative file-name for private files
		// under the ExpectedFileName field, construct the fully-qualified
		// expected file-name
		if((err = CTestFileUriServer::CreateFullyQualifiedName(fileName, drive, fullyQualifiedName)) != KErrNone)
			{
			ERR_PRINTF2(_L("Error returned by CTestFileUriServer::CreateFullyQualifiedName: %D"), err);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		fileName.Set(fullyQualifiedName);
		INFO_PRINTF2(_L("Fully qualified name = %S"), &fileName);
		}

		err = fs.Connect();
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Error occured while connecting to file server: %D"), err);
			SetTestStepResult(EFail);
			}

		if(TestStepResult() == EPass)
			{
			err = fs.Delete(fileName);
			if(err == KErrNone)
				{
				INFO_PRINTF1(_L("File deleted successfully"));
				}
			else if(err == KErrNotFound || err == KErrPathNotFound)
				{// File or path not found, this is NOT considered as fail
				INFO_PRINTF1(_L("File and/or path does not exist"));
				}
			else if(err == KErrNotReady)
				{// Drive is not ready, this is NOT considered as fail
				INFO_PRINTF1(_L("Drive not ready"));
				}
			else
				{
				ERR_PRINTF2(_L("Error occured while deleting file: %D"), err);
				SetTestStepResult(EFail);
				}
			fs.Close();
			}
		}
	return TestStepResult();
	}	// doTestStepL
