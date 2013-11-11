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

// Epoc Includes
// For File URI handler API
#include <uri16.h>
#include <uri8.h>

// User Include
#include "TestGetFileNameFromUriStep.h"

/**
Constructor. Sets the test step name
*/
CTestGetFileNameFromUriStep::CTestGetFileNameFromUriStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestGetFileNameFromUriStep);
	}

/**
Does the main functionality of a test step.
Here, reads values from INI file and calls DoTestL
@internalTechnology 
@see		ExtractFileNameAndCompareL
@param		None
@return		EPass or EFail indicating the success or failure of the test step
*/
TVerdict CTestGetFileNameFromUriStep::doTestStepL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("\n"));
	// Get necessary information from INI file
	TPtrC 	fileUri;
	TPtrC 	expectedFileName;
	TPtrC 	fileType;
	TPtrC 	drive;
	TInt 	characterSet;

	if(!GetStringFromConfig(ConfigSection(),	KIniFileUri, 			fileUri			) ||
	   !GetStringFromConfig(ConfigSection(), 	KIniExpectedFileName, 	expectedFileName) ||
	   !GetStringFromConfig(ConfigSection(), 	KIniFileType, 			fileType		) ||
	   !GetIntFromConfig   (ConfigSection(), 	KIniCharacterSet, 		characterSet	) ||
	   !GetStringFromConfig(ConfigSection(), 	KIniDrive, 				drive			)
	  )
		{
		ERR_PRINTF6(_L("Problem in reading values from ini.			\
						\nExpected fields are: \n%S\n%S\n%S\n%S\n%S\n"
					  ),&KIniFileUri, &KIniExpectedFileName, &KIniFileType, &KIniCharacterSet, &KIniDrive
				   );
		SetTestStepResult(EFail);
		}
	else
		{
		TRAPD(err, DoTestL(fileUri, expectedFileName, fileType, drive, characterSet));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Leave occured in CTestGetFileNameFromUriStep::DoTestL: %D"), err);
			SetTestStepResult(EFail);
			}
		}
	INFO_PRINTF1(_L("\n"));	
	__UHEAP_MARKEND;
	return TestStepResult();
	}	// doTestStepL()

/**
Constructs fully-qualified filename in case of a private file, checks whether the 
drive is a removable drive. Populates the <drive> placeholder and calls 
ExtractFileNameAndCompareL.
*/
void CTestGetFileNameFromUriStep::DoTestL(const TPtrC& aFileUri, const TPtrC& aExpectedFileName, const TPtrC& aFileType, TPtrC& aDrive, const TInt& aCharacterSet)
	{
	TInt err = KErrNone;
	TFileName fullyQualifiedName(aExpectedFileName);
	TBool fileNotFound = EFalse;

	if(aFileType == KFileTypePrivate)
		{// The file is a private file
		// In the case of a secure vesrion of the OS
		// As the INI file contains relative file-name for private files
		// under the ExpectedFileName field, construct the fully-qualified
		// expected file-name
		if((err = CTestFileUriServer::CreateFullyQualifiedName(aExpectedFileName, aDrive, fullyQualifiedName)) != KErrNone)
			{
			ERR_PRINTF2(_L("Error returned by CTestFileUriServer::CreateFullyQualifiedName: %D"), err);
			SetTestStepResult(EFail);
			return;
			}
		}

	// Check whether drive is removable drive
	TDriveNumber driveNum;
	CTestFileUriServer::GetDriveNumber(aDrive, driveNum);
	TBool aResult;
	err = CTestFileUriServer::IsRemovableDrive(driveNum, aResult);
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Error occured while checking whether drive is removable: %D"), err);
		SetTestStepResult(EFail);
		}
	else
		{
		if(aResult)
			{// The drive is a removable drive
			INFO_PRINTF1(_L("The drive is a removable drive"));

			// Check if any other drive contains a file of the same path and name
			// but comes ahead in the alphabetical order. If yes, that is the correct
			// expected file name
			TBuf<1> correctDrive;
			err = CTestFileUriServer::FirstRemovableDriveWithSameFileName(fullyQualifiedName, correctDrive);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Error occured in FirstRemovableDriveWithSameFileName: %D"), err);
				SetTestStepResult(EFail);
				}
			else if(correctDrive == KNullDesC)
				{
				INFO_PRINTF1(_L("File not found on removable drive"));
				fileNotFound = ETrue;
				}
			else
				{
				correctDrive.LowerCase();
				TBuf<1> drive = aDrive;
				drive.LowerCase();
				if(correctDrive != drive)
					{
					fullyQualifiedName.Replace(0, 1, correctDrive);
					INFO_PRINTF1(_L("One more removable drive found with the same file name, but is ahead in alphabetical order"));
					INFO_PRINTF2(_L("Hence the correct expected file name is %S"), fullyQualifiedName);
					}	
				}
			aDrive.Set(KExtMedia());
			}
		HBufC16* uriWithDrive = NULL;
		
		// Fill the <drive> place holder if it exists
		TRAPD(err, uriWithDrive = CTestFileUriServer::CheckAndFillDriveNameL(aFileUri, aDrive));

		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Error occured while filling the drive-placeholder: %D"), err);
			SetTestStepResult(EFail);
			}
		else
			{
			CleanupStack::PushL(uriWithDrive);
			INFO_PRINTF2(_L("Character Set = %D"), aCharacterSet);
			INFO_PRINTF2(_L("File URI = %S"), uriWithDrive);
			INFO_PRINTF2(_L("Expected Filename = %S"), &fullyQualifiedName);
			fullyQualifiedName.LowerCase();	
		
			// Call template function based on the characterset.
			if(aCharacterSet == KCharSet8)
				{
				TUriParser16 uriParser8;
				HBufC16* hBufC8 = NULL;
				ExtractFileNameAndCompareL(uriParser8, hBufC8, uriWithDrive->Des(), fullyQualifiedName, aFileType, fileNotFound);
				}
			else if(aCharacterSet == KCharSet16)
				{
				TUriParser16 uriParser16;
				HBufC16* hBufC16 = NULL;
				ExtractFileNameAndCompareL(uriParser16, hBufC16, uriWithDrive->Des(), fullyQualifiedName, aFileType, fileNotFound);
				}
			else
				{
				ERR_PRINTF1(_L("Invalid CharacterSet"));
				SetTestStepResult(EFail);
				}
			CleanupStack::PopAndDestroy(uriWithDrive);	
			}
		}
	}	// DoTestL

/**
Template function that calls TUriC::GetFileNameL() after processing 
an 8-bit or 16-bit version of a URI, verifies the expected and 
actual results and sets the test step result accordingly.
*/
template <class TUriParserType, class HBufCType>
void CTestGetFileNameFromUriStep::ExtractFileNameAndCompareL(TUriParserType& uriParser8Or16, HBufCType*& fileUri8Or16, const TPtrC& aFileUri, const TFileName& aFullyQualifiedName, const TPtrC& aFileType, const TBool& aFileNotFound)
	{
	fileUri8Or16 = HBufCType::NewLC(aFileUri.Length());
	fileUri8Or16->Des().Copy(aFileUri);
	uriParser8Or16.Parse(*fileUri8Or16);
	
	// The actual API being tested
	HBufC16* fileName16 = NULL;
	TRAPD(err, fileName16 = uriParser8Or16.GetFileNameL());
	CleanupStack::PopAndDestroy(fileUri8Or16);
	
	if(aFileType == KFileTypePrivate)
		{
		}
	
	if(aFileNotFound)
		{// aFileNotFound is true when the file is not found on removable drive
		if(err == KErrNotFound || err == KErrPathNotFound)
			{
			INFO_PRINTF2(_L("Leave occured = %D. This is expected if file is not found on removable drive"), err);
			}
		else
			{
			ERR_PRINTF2(_L("Unexpected result = %D when file is not found on removable drive"), err);	
			SetTestStepResult(EFail);
			if(fileName16 != NULL)
				{
				INFO_PRINTF2(_L("File name returned = %S"), fileName16);
				delete fileName16;
				}
			}
		}
	else if(err != KErrNone)	
		{// If it comes here, it means there's some problem. But some tests
		 // expect an error. So just set the error, and leave the decision 
		 // to TEF
		ERR_PRINTF2(_L("Leave occured: %D"), err);	
		SetTestStepError(err);
		}
	else
		{// Things seem to be OK and a file-name has been returned. 
		 // Do the checking
		
		INFO_PRINTF2(_L("Returned Filename = %S"), fileName16);
		
		fileName16->Des().LowerCase();
		
		// Compare and set the verdict
		if((fileName16->Des() != aFullyQualifiedName))
			{
			INFO_PRINTF1(_L("The returned filename did not match the expected filename. Result = INCORRECT"));
			SetTestStepResult(EFail); 
			}
		else
			{
			INFO_PRINTF1(_L("The returned filename is the same as the expected filename. Result = CORRECT"));
			}
		
		delete fileName16;
		}
	}	// ExtractFileNameAndCompareL()
	

