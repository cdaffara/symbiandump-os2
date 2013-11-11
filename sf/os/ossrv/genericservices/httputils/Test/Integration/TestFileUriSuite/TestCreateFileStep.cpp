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
#include "TestCreateFileStep.h"

/**
Constructor. Sets the test step name
*/
CTestCreateFileStep::CTestCreateFileStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestCreateFileStep);
	}

/**
Tries to create a file mentioned in the ini file.
@internalTechnology
@param		None
@return		EPass or EFail indicating the success or failure of file creation.
*/
TVerdict CTestCreateFileStep::doTestStepL()
	{
	// Get file path and name from ini file
	TPtrC fileName;
	TPtrC fileType;
	TPtrC drive;
	TInt err = KErrNone;
	RFs fs;
	RFile file;
		
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

		// Connect to file server
		err = fs.Connect();
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Error occured while connecting to file server: %D"), err);
			SetTestStepResult(EFail);
			}

		if(TestStepResult() == EPass)
			{
			// Try opening first to see whether file already exists.
			err = file.Open(fs, fileName, EFileRead);
			switch(err)
				{
				case KErrNone:
					// File already exists, this is NOT considered as fail
					INFO_PRINTF1(_L("File already exists"));
					break;
				case KErrNotReady:
					// Drive is not ready, this is NOT considered as fail
					INFO_PRINTF1(_L("Drive not ready"));
					break;	
				case KErrPathNotFound:
					// Create directories
					err = fs.MkDirAll(fileName);
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Could not create directories. Error returned: %D"), err);
						SetTestStepResult(EFail);
						// Only if unable to create directories we break
						// else, next case should also be executed which creates the file
						break;
						}
					// No break
				case KErrNotFound:
					// Create the file
					err = file.Create(fs, fileName, EFileWrite);
					if(err != KErrNone)
						{
						ERR_PRINTF2(_L("Error occured while creating file: %D"), err);
						SetTestStepResult(EFail);
						}
					else
						{
						INFO_PRINTF1(_L("Successfully created file"));	
						}
					break;
				default:
					ERR_PRINTF2(_L("Error occured: %D"), err);
					SetTestStepResult(EFail);		
				}
				file.Close();
				fs.Close();
			}
		}
		return TestStepResult();
	}	// doTestStepL
