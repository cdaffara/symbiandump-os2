/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#include "trecogstep.h"

// Literals & Constants
_LIT(KCompleted, "Completed.");
_LIT(KMethodName, "byName");
_LIT(KMethodHandle, "byHandle");
_LIT(KMethodBuffer, "byBuffer");
_LIT8(KEmptyBuffer8,"");


class RDeletefile
	{
public:
	RDeletefile(RFs &aFs, TDesC &aFileName)
		: iFs(aFs),
		  iFileName(aFileName)
		{
		}
	void Close()
		{
		(void) iFs.Delete(iFileName);
		}
	
private:
	RFs &iFs;
	TDesC &iFileName;
	};

void CRecogStep::TestRecognizeDataL()
	{
	TDataRecognitionResult recogResult;
	
	TPtrC tmp;
	GetStringFromConfig(ConfigSection(), _L("fileName"), tmp);
	iFileName = tmp;
	

	TPtrC expectedDataType16;
	GetStringFromConfig(ConfigSection(), _L("expectedDataType"), expectedDataType16);
	HBufC8 *expectedDataBuf8 = ConvertDes16toHBufC8LC(expectedDataType16);

	TInt expectedConfidence;
	GetIntFromConfig(ConfigSection(), _L("expectedConfidence"), expectedConfidence);
	
	TInt maxDataBufSize;
	if (GetIntFromConfig(ConfigSection(), _L("maxDataBufSize"), maxDataBufSize))
		{
		iLs.SetMaxDataBufSize(maxDataBufSize);
		}
		
	TPtrC method;
	GetStringFromConfig(ConfigSection(), _L("method"), method);
		
	TBool checkSpecific = EFalse;
	TBool matchedSpecificMimeType = EFalse;
	TPtrC specificMimeType;
	if (method == KMethodHandle)
		{		
		checkSpecific = GetStringFromConfig(ConfigSection(), _L("checkSpecificMimeType"), specificMimeType);
		}

	TInt usePrivateFile = 0;
	GetIntFromConfig(ConfigSection(), _L("usePrivateFile"), usePrivateFile);
	
	RDeletefile deletefile(iTheFs, iFileName);
	if(!usePrivateFile)
		{
		// Tweak file modification time to defeat the apparch recognizer result cache....
		TTime time;
		User::LeaveIfError(iTheFs.Modified(iFileName, time));
		time += TTimeIntervalSeconds(1);
		User::LeaveIfError(iTheFs.SetModified(iFileName, time));
		}
	else
		{
		// Copy file to private dir, this will make it inaccesible to the recognizer code (except via handle).
		ConvertFileToPrivateL();	
		CleanupClosePushL(deletefile);
		}
	
	
	if (method == KMethodName)
		{
		INFO_PRINTF2(_L("Test Recognizing %S by File Name"), &iFileName);
	    User::LeaveIfError(iLs.RecognizeData(iFileName, KEmptyBuffer8, recogResult));
		}
	else if((method == KMethodHandle) || (method == KMethodBuffer))
		{
		RFile fileToRead;	
		User::LeaveIfError(fileToRead.Open(iTheFs, iFileName, EFileShareReadersOrWriters | EFileRead | EFileStream));
		CleanupClosePushL(fileToRead);
		if(method == KMethodHandle)
			{
			if (checkSpecific)
				{
				HBufC8* specificMimeType8 = ConvertDes16toHBufC8LC(specificMimeType);
				TDataType mime(*specificMimeType8);																					
				INFO_PRINTF2(_L("Test matching specific mime-type %S by Handle"), &iFileName);		
				User::LeaveIfError(iLs.RecognizeSpecificData(fileToRead, mime, matchedSpecificMimeType));
				CleanupStack::PopAndDestroy(specificMimeType8);
				}
			else 
				{
				INFO_PRINTF2(_L("Test Recognizing %S by Handle"), &iFileName);		
			    User::LeaveIfError(iLs.RecognizeData(fileToRead, recogResult));				
				}
			}
		else
			{
			INFO_PRINTF2(_L("Test Recognizing %S by Buffer"), &iFileName);		
			TInt size;
			User::LeaveIfError(fileToRead.Size(size));

			TInt maxBufferSize;
			if(GetIntFromConfig(ConfigSection(), _L("maxBufferSize"), maxBufferSize))
				{
				if(size > maxBufferSize)
					{
					size = maxBufferSize;
					}
				}

			HBufC8* memForFile = HBufC8::NewLC(size);
			TPtr8 fileContent(memForFile->Des());		
			User::LeaveIfError(fileToRead.Read(fileContent, size));
			User::LeaveIfError(iLs.RecognizeData(iFileName, fileContent, recogResult));
			CleanupStack::PopAndDestroy(); //memForFile,
			}
		CleanupStack::PopAndDestroy(&fileToRead);
		}
	else
		{
		ERR_PRINTF1(_L("method not set correctly"));		
		User::Leave(KErrArgument);
		}
	
	
	if (checkSpecific)
		{
		if (matchedSpecificMimeType)
			{
			INFO_PRINTF2(_L("Specific type '%S' matched\n"), &specificMimeType);
			SetTestStepResult(EPass);			
			}
		else 
			{
			INFO_PRINTF2(_L("Specific type '%S' not matched\n"), &specificMimeType);
			SetTestStepResult(EFail);			
			}
		}
    else 
    	{
    	TPtrC result16 = recogResult.iDataType.Des();
    	if (// Expected failure
		((expectedConfidence == CApaDataRecognizerType::ENotRecognized) && 
		 (recogResult.iDataType != expectedDataBuf8->Des())) ||
		// Expected success
		((recogResult.iConfidence == expectedConfidence) && 
		 (recogResult.iDataType == expectedDataBuf8->Des())))
		    {
			INFO_PRINTF3(_L("PASSED - type '%S' confidence=%d\n"), 
						 &result16,
						 recogResult.iConfidence);		
		    SetTestStepResult(EPass);	
		    }
		else
			{
			ERR_PRINTF5(_L("FAILED - expected '%S', got type '%S' - expected confidence=%d got confidence %d\n"), 
						&expectedDataType16,
						&result16,
						expectedConfidence,
						recogResult.iConfidence);		
			SetTestStepResult(EFail);		
			}
    	}
	
	if(usePrivateFile)
		{
		CleanupStack::PopAndDestroy(&deletefile);
		}
	
	CleanupStack::PopAndDestroy(expectedDataBuf8);
	INFO_PRINTF1(KCompleted);
   	}

HBufC8* CRecogStep::ConvertDes16toHBufC8LC(TDesC& source)
	{
	HBufC8 *buf = HBufC8::NewL(source.Length());
	CleanupStack::PushL(buf);
	TPtr8 ptr=buf->Des();

	CCnvCharacterSetConverter *converter = CCnvCharacterSetConverter::NewLC();
	converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierAscii, iTheFs);
	converter->ConvertFromUnicode(ptr, source);
	CleanupStack::PopAndDestroy(converter);
	return buf;
	}



void CRecogStep::RunTestCasesL()
	{
	__UHEAP_MARK;
	TRAPD(r,TestRecognizeDataL());
	TEST(r==KErrNone);
	__UHEAP_MARKEND;
	}


/**
   Destructor
 */
CRecogStep::~CRecogStep()
	{
	iTheFs.Close();
	iLs.Close();
	}

/**
   Constructor
 */
CRecogStep::CRecogStep()
	{

	TInt ret=iTheFs.Connect();
	TEST(ret==KErrNone);
	iTheFs.ShareProtected();

	ret = iLs.Connect();
	TEST(ret==KErrNone);

	
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KRecogStep);
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CRecogStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CRecogStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}


TVerdict CRecogStep::doTestStepL()
	{
	TRAPD(ret,RunTestCasesL())
	TEST(ret==KErrNone);
	return TestStepResult();
	}

void CRecogStep::ConvertFileToPrivateL()
	{
	// Read source file
	RFile fromFile;	
	User::LeaveIfError(fromFile.Open(iTheFs, iFileName, EFileShareReadersOrWriters | EFileRead | EFileStream));
	CleanupClosePushL(fromFile);
	TInt size;
	User::LeaveIfError(fromFile.Size(size));
	HBufC8* memForFileContents = HBufC8::NewLC(size);
	TPtr8 fileContents(memForFileContents->Des());		
	User::LeaveIfError(fromFile.Read(fileContents, size));
	
	// Create destination path
	TDriveUnit sysDrive = RFs::GetSystemDrive();
	(void)iTheFs.CreatePrivatePath(static_cast<TInt> (sysDrive));
	TFileName newFileName;
	User::LeaveIfError(iTheFs.PrivatePath(newFileName));
	newFileName.Insert(0, sysDrive.Name());
	TParsePtr parseFrom(iFileName);
	newFileName.Append(parseFrom.NameAndExt());

	// Make sure destination does not already exist.
	(void) iTheFs.Delete(newFileName);

	// Copy file to private location
	RFile toFile;
	User::LeaveIfError(toFile.Create(iTheFs, newFileName, EFileShareExclusive | EFileWrite | EFileStream));
	CleanupClosePushL(toFile);

	User::LeaveIfError(toFile.Write(fileContents));

	// Update iFileName
	iFileName = newFileName;
	
	// Cleanup
	CleanupStack::PopAndDestroy(&toFile);
	CleanupStack::PopAndDestroy(memForFileContents);
	CleanupStack::PopAndDestroy(&fromFile);
	}

// End of file
