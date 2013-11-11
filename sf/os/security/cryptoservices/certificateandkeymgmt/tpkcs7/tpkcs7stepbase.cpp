/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tpkcs7stepbase.h"
#include <test/testexecutelog.h>

CTPKCS7StepBase::~CTPKCS7StepBase()
	{
	delete iRawData;
	iFs.Close ();
	}

CTPKCS7StepBase::CTPKCS7StepBase()
	{
	}

HBufC8* CTPKCS7StepBase::readFileL (TPtrC tag)
	{
	TPtrC fileName;
	if (GetStringFromConfig(ConfigSection(), tag, fileName) == EFalse)
		{
		return NULL;
		}

	RFile file;
	if (file.Open(iFs, fileName, EFileRead) != KErrNone)
		{
		INFO_PRINTF2(_L("Cannot open file %S for reading"), &fileName);
		return NULL;
		}
	CleanupClosePushL(file);

	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));

	HBufC8* result = HBufC8::NewL(fileSize);
	result->Des().SetLength(fileSize);
	
	TPtr8 rawDataPtr(result->Des());
	rawDataPtr.SetLength(fileSize);
	file.Read (rawDataPtr);

	CleanupStack::PopAndDestroy (&file);

	INFO_PRINTF3(_L("Read %d octets from %S"), result->Size(), &fileName);

	return result;
	}

TVerdict CTPKCS7StepBase::doTestStepPreambleL()
	{
	User::LeaveIfError (iFs.Connect());

	SetTestStepResult(EPass);

	iRawData = readFileL (_L("File"));
	if (iRawData == NULL)
		{
		INFO_PRINTF1(_L("Failed to read 'File' section of script"));
		SetTestStepResult(ETestSuiteError);
		}

	HBufC8* certificate = readFileL (_L("RootCertificate"));
	if (certificate == NULL)
		{
		INFO_PRINTF1(_L("No 'RootCertificate' available"));
		}
	else
		{
		CleanupStack::PushL (certificate);
		iRootCertificate = CX509Certificate::NewL(*certificate);
		CleanupStack::PopAndDestroy (certificate);
		}

     //for encrypted_data
     
     //get Salt value for comparison
     iSaltValue = readFileL (_L("Salt"));
     if(iSaltValue == NULL)
     	{
     	INFO_PRINTF1(_L("No 'Salt' available"));
     	}
      //get content data 
     iContentData = readFileL (_L("ContentData"));
     if(iContentData == NULL)
     	{
     	INFO_PRINTF1(_L("No 'ContentData' available"));
		}
      
     //for digest_Info
     //Digest
     iDigest = readFileL (_L("DigestValue")); 
     if(iDigest == NULL)
     	{
     	INFO_PRINTF1(_L("No 'DigestValue' available"));
     	} 	
     
     //EncodeParams
     iEncodedParams = readFileL (_L("EncodedParams"));  		
     if(iEncodedParams == NULL)
     	{
     	INFO_PRINTF1(_L("No 'EncodedParams' available"));
		}
   
	return TestStepResult();
	}

