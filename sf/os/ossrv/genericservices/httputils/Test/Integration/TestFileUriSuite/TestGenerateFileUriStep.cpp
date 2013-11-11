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
#include <escapeutils.h>

// User Include
#include "TestGenerateFileUriStep.h"

/**
Constructor. Sets the test step name
*/
CTestGenerateFileUriStep::CTestGenerateFileUriStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestGenerateFileUriStep);
	}

/**
Does the main functionality of a test step.
Here, reads values from INI file and calls DoTestL
@internalTechnology 
@see 		GenerateFileUriAndCompareL
@param		None
@return		EPass or EFail indicating the success or failure of the test step
*/
TVerdict CTestGenerateFileUriStep::doTestStepL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("\n"));
	// Get necessary information from INI file
	TPtrC fileName;
	TPtrC expectedFileUri;
	TPtrC fileType;
	TInt characterSet;
	TPtrC drive;
	
	if(!GetStringFromConfig(ConfigSection(), 	KIniFileName, 			fileName		) ||
	   !GetStringFromConfig(ConfigSection(), 	KIniExpectedFileUri,	expectedFileUri	) ||
	   !GetStringFromConfig(ConfigSection(), 	KIniFileType, 			fileType		) ||
	   !GetIntFromConfig   (ConfigSection(), 	KIniCharacterSet, 		characterSet	) ||
	   !GetStringFromConfig(ConfigSection(), 	KIniDrive, 				drive			)
	  )
		{
		ERR_PRINTF6(_L("Problem in reading values from ini.			\
						\nExpected fields are: \n%S\n%S\n%S\n%S\n%S\n"
					  ),&KIniFileName, &KIniExpectedFileUri, &KIniFileType, &KIniCharacterSet, &KIniDrive
				   );
		SetTestStepResult(EFail);
		}
	else
		{// No problem in reading values from INI file. Proceed.
		TRAPD(err, DoTestL(fileName, expectedFileUri, fileType, drive, characterSet));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Leave occured in CTestGenerateFileUriStep::DoTestL: %D"), err);
			SetTestStepResult(EFail);
			}
		}
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("\n"));	
	return TestStepResult();
	}	// doTestStepL()

/**
Checks whether the drive is a removable drive. Populates the <drive> placeholder. 
And calls ExtractFileNameAndCompareL.
*/
void CTestGenerateFileUriStep::DoTestL(const TPtrC& aFileName, const TPtrC& aExpectedFileUri, const TPtrC& aFileType, const TPtrC& aDrive, const TInt& aCharacterSet)
	{
	INFO_PRINTF2(_L("Character Set = %D"), aCharacterSet);
	INFO_PRINTF2(_L("File name = %S"), &aFileName);
	
	// Eliminate any characters like colon, slash etc.
	TPtrC drive(aDrive.Left(1));

	TUint flags = KErrNone;
	TDriveNumber driveNum;
	CTestFileUriServer::GetDriveNumber(drive, driveNum);
	TBool aResult;
	TInt err = CTestFileUriServer::IsRemovableDrive(driveNum, aResult);
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
			flags = EExtMedia;
			drive.Set(KExtMedia());
			}
		HBufC16* expectedUriWithDrive = NULL;
		
		// Fill the <drive> place holder if it exists
		TRAPD(err, expectedUriWithDrive = CTestFileUriServer::CheckAndFillDriveNameL(aExpectedFileUri, drive));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Error occured while filling the drive-placeholder: %D"), err);
			SetTestStepResult(EFail);
			}
		else
			{// All preliminary operations OK so far, go ahead and perform the test.
			CleanupStack::PushL(expectedUriWithDrive);
			INFO_PRINTF2(_L("Expected File URI = %S"), expectedUriWithDrive);	
			if(aCharacterSet == KCharSet8)
				{
				CUri16* cUri8 = NULL;
				GenerateFileUriAndCompareL(cUri8, aFileName, expectedUriWithDrive, flags, driveNum, aFileType);	
				}
			else if(aCharacterSet == KCharSet16)
				{
				CUri16* cUri16 = NULL;
				GenerateFileUriAndCompareL(cUri16, aFileName, expectedUriWithDrive, flags, driveNum, aFileType);	
				}
			else
				{
				ERR_PRINTF1(_L("Invalid CharacterSet"));
				SetTestStepResult(EFail);
				}
			CleanupStack::PopAndDestroy(expectedUriWithDrive);
			}
		}
	}	// DoTestL
	
/**	
Template function that calls the 8 or 16-bit version of 
CreatePrivateFileUriL() or CreateFileUriL(), verifies the expected and 
actual results and sets the test step result accordingly.
*/
template <class CUriType>
void CTestGenerateFileUriStep::GenerateFileUriAndCompareL(CUriType*& cUri8Or16, const TPtrC& aFileName, HBufC16*& aExpectedFileUri8Or16, const TUint& aFlags, const TInt& aDriveNum, const TPtrC& aFileType)
	{
	TInt err = KErrNone;
	
	// Get the URI object
	if(aFileType == KFileTypePrivate)
		{// Call private version
		TRAP(err, cUri8Or16 = CUriType::CreatePrivateFileUriL(aFileName, static_cast<TDriveNumber>(aDriveNum), aFlags));
		
		// Test it in case of NON-secure version too, but in that case
		// the expected result is a Leave with KErrNotSupported.
		} 
	else
		{// Call non-private version
		TRAP(err, cUri8Or16 = CUriType::CreateFileUriL(aFileName, aFlags));	
		}
		
	if(err != KErrNone)
		{// If it comes here, it means there's some problem. But some tests
	 	 // expect an error. So just set the error, and leave the decision 
	 	 // to TEF
		ERR_PRINTF2(_L("Leave occured: %D"), err);	
		SetTestStepError(err);
		}
	else
		{// Things seem to be OK and a URI has been returned. Do the checking
		
		CleanupStack::PushL(cUri8Or16);
		
		// Construct a fully lowercase 16-bit version of the returned URI, so that we
		// can do print, compare etc.
		HBufC16* lowerCaseUri16 = HBufC16::NewL(cUri8Or16->Uri().UriDes().Length());
		lowerCaseUri16->Des().Copy(cUri8Or16->Uri().UriDes());
		CleanupStack::PopAndDestroy(cUri8Or16);
		
		INFO_PRINTF2(_L("The returned URI = %S"), lowerCaseUri16);
		
		lowerCaseUri16->Des().LowerCase();
		
		// Make expected URI too, fully lowercase
		aExpectedFileUri8Or16->Des().LowerCase();

		// Compare and set the verdict
		if (lowerCaseUri16->Des() != aExpectedFileUri8Or16->Des())
			{
			INFO_PRINTF1(_L("The returned and expected URIs did not match. Result = INCORRECT"));
			SetTestStepResult(EFail); 
			}
		else
			{
			INFO_PRINTF1(_L("The returned and expected URIs matched. Result = CORRECT")); 	
			}
		delete lowerCaseUri16;
		}
	}		// GenerateFileUriAndCompareL()

