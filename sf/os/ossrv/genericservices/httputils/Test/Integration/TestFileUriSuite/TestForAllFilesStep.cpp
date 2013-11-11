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
// TESTGENERATEFILEURIFORALLFILESSTEP.CPP
// 
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
#include "TestForAllFilesStep.h"

/**
Constructor. Sets the test step name
*/
CTestForAllFilesStep::CTestForAllFilesStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestForAllFilesStep);
	}

/**
Does the main functionality of a test step. Here just calls DoTestL.
@internalTechnology 
@param		None
@return		EPass or EFail indicating the success or failure of the test step
*/
TVerdict CTestForAllFilesStep::doTestStepL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("\n"));
	TRAPD(err, DoTestL());
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Leave occured in CTestForAllFilesStep::DoTestL: %D"), err);
		SetTestStepResult(EFail);
		}
	INFO_PRINTF1(_L("\n"));	
	__UHEAP_MARKEND;
	return TestStepResult();	
	}		// doTestStepL()

/**
Scans all the directories and calls TestBothWaysL that tests the
creation of URI from a filename and vice versa for all the files.
*/
void CTestForAllFilesStep::DoTestL()
	{
	RFs fs;
	TInt err = fs.Connect();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Error occured while connecting to file server: %D"), err);
		SetTestStepResult(EFail);
		}
	else
		{
		TBuf<4> rootDir;
		CDirScan *dirScan = CDirScan::NewLC(fs);
		TInt drive;
		for(drive = EDriveA; drive <= EDriveZ; ++drive)
			{
			rootDir.Format(_L("%c:\\"), static_cast<char>(KLetterA + drive));
			dirScan->SetScanDataL(rootDir, KEntryAttNormal, ESortNone);
			CDir* entryList = NULL;
			FOREVER
				{
				TRAPD(err, dirScan->NextL(entryList));
				if(err == KErrNotReady)
					{
					INFO_PRINTF2(_L("Drive %S not ready"), &rootDir);
					break;
					}
				if (entryList==NULL)
					{
					break;
					}
				CleanupStack::PushL(entryList);
				TFileName filename;
				TInt index;
				for(index = 0; index < entryList->Count(); ++index)
					{
					const TEntry& entry = (*entryList)[index];
					filename = dirScan->FullPath();
					filename.Append(entry.iName);
					INFO_PRINTF1(_L("\n"));
					INFO_PRINTF2(_L("The next file name in the dir-scan = %S"), &filename);
					INFO_PRINTF1(_L("Calling 8-bit versions"));
					
					// To keep VC compiler happy as it does not support
					// explicit template calls.Hence passing empty variables 
					// cUri8 and tUriParser8
					CUri8* cUri8 = NULL;
					TUriParser8 tUriParser8;
					TestBothWaysL(cUri8, tUriParser8, filename);
					INFO_PRINTF1(_L("\n"));
					INFO_PRINTF1(_L("Calling 16-bit versions"));
					CUri16* cUri16 = NULL;
					TUriParser16 tUriParser16;
					TestBothWaysL(cUri16, tUriParser16, filename);
					}
				CleanupStack::PopAndDestroy(entryList);	
				}	// FOREVER
			}
		fs.Close();
		CleanupStack::PopAndDestroy(dirScan);	
		}
	}	// DoTestL

/**
Template function that calls CUri::CreateFileUriL to generate a URI and 
calls TUriC::GetFileNameL() with this URI, to verify that the filename so got
is the same as the original one.
*/
template <class CUriType, class TUriParserType>
void CTestForAllFilesStep::TestBothWaysL(CUriType*& cUri8Or16, TUriParserType& tUriParser8Or16, TFileName& aFileName)
	{
	TUint flags = KErrNone;
	TDriveNumber driveNum;
	TParse parser;
	parser.Set(aFileName, 0, 0);
	TBuf<1> drive = parser.Drive().Left(1);
	drive.LowerCase();
		
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
			}
		
		// Generate the URI	
		TRAPD(err, cUri8Or16 = CUriType::CreateFileUriL(aFileName, flags));
			
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Error occured in CreateFileUriL: %D"), err);
			SetTestStepResult(EFail);
			}
		else
			{
			CleanupStack::PushL(cUri8Or16);
			// Just in case, create a 16-bit heap desc and print.
			HBufC16* tempBuf = HBufC16::NewL(cUri8Or16->Uri().UriDes().Length());
			tempBuf->Des().Copy(cUri8Or16->Uri().UriDes());
			INFO_PRINTF2(_L("File URI returned by CreateFileUriL = %S"), tempBuf);
			delete tempBuf;
			
			if(aResult)
				{
				TBuf<1> correctDrive;
				err = CTestFileUriServer::FirstRemovableDriveWithSameFileName(aFileName, correctDrive);
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("Error occured in FirstRemovableDriveWithSameFileName: %D"), err);
					SetTestStepResult(EFail);
					}
				else
					{
					correctDrive.LowerCase();
					if(correctDrive != drive)
						{
						aFileName.Replace(0, 1, correctDrive);
						INFO_PRINTF1(_L("One more removable drive found with the same file name, but is ahead in alphabetical order"));
						INFO_PRINTF2(_L("Hence the correct expected file name is %S"), aFileName);
						}	
					}
				}
				
			// Convert the URI to filename back again
			HBufC16* returnedFileName = NULL;
			
			tUriParser8Or16.Parse(cUri8Or16->Uri().UriDes());
			TRAPD(err, returnedFileName = tUriParser8Or16.GetFileNameL());
			CleanupStack::PopAndDestroy(cUri8Or16);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Error occured in GetFileNameL: %D"), err);
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF2(_L("The filename returned by GetFileNameL = %S"), returnedFileName);
			
				// Verify the result
				aFileName.LowerCase();
				returnedFileName->Des().LowerCase();
				
				if(returnedFileName->Compare(aFileName) != KErrNone)
					{
					INFO_PRINTF1(_L("The returned filename did not match the original filename. Result = INCORRECT"));
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("The returned filename is same as the original filename. Result = CORRECT"));
					}	
				delete returnedFileName;
				}	
			}
		}
	}	// TestBothWaysL


