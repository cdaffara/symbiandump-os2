/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <padding.h>
#include "tpaddingSSLv3.h"
#include <securityerr.h>

CTestPadSSLv3::CTestPadSSLv3()
	{
   SetTestStepName(KPadSSLv3);
	}

CTestPadSSLv3::~CTestPadSSLv3()
	{
	}

TVerdict CTestPadSSLv3::doTestStepL()
	{
    SetTestStepResult(EPass);
    __UHEAP_MARK;

	INFO_PRINTF1(_L("Test of padding with type SSLv3"));

	TInt blockSize;
    TInt textSize;

	if (!GetStringFromConfig(ConfigSection(), _L("TestCaseName"), iTestCaseName))
 		{
 		INFO_PRINTF1(_L("Could not find TestCaseName in tpadSSLv3.ini"));
      	return EFail;
 		}
   
    if (!GetIntFromConfig(ConfigSection(), _L("BlockSize"), blockSize))
 		{
 		INFO_PRINTF1(_L("Could not find blocksize in tpadSSLv3.ini"));
      	return EFail;
 		}
 		
  	if(iTestCaseName.Compare(_L("CipherAES_CBC"))==0 || iTestCaseName.Compare(_L("CipherDES_CBC"))==0 || iTestCaseName.Compare(_L("CipherRC2_CBC"))==0)
 		{
 		TestSSLv3Padding(blockSize);
 		}
 	else
 		{
 		if (!GetIntFromConfig(ConfigSection(), _L("TextSize"), textSize))
   			{
      		INFO_PRINTF1(_L("Could not find TextSize in tpadSSLv3.ini"));
      		return EFail;
   			}
   		if (!GetStringFromConfig(ConfigSection(), _L("ExpectedResult"), iExpectedResult))
 			{
 			INFO_PRINTF1(_L("Could not find TextSize in tpadSSLv3.ini"));
      		return EFail;
 			}
 		TestSSLv3CorruptPadding(blockSize, textSize);	
 		}
	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CTestPadSSLv3::TestSSLv3Padding(TInt aBlockSize)
	{
	CPaddingSSLv3 *padding = CPaddingSSLv3::NewLC(aBlockSize);

	for (TInt i = 0 ; i <= aBlockSize; i++)
		{
		HBufC8 *padInData = HBufC8::NewLC(i);
		HBufC8 *padOutData = HBufC8::NewLC(i+(aBlockSize-i%aBlockSize));
		TPtr8 in(padInData->Des());
		TPtr8 out(padOutData->Des());
		TInt j;
      
		for (j = 0; j < i; j++)
			{
			TInt text('a'+j%25);
			in.Append(text);
			}
		TRAPD(err, padding->DoPadL(in, out));
		INFO_PRINTF3(_L("The error returned for input size %d is %d"), i, err);
		TEST(err == KErrNone);

		TInt totalLength = out.Length();
		TInt paddingLength = aBlockSize - in.Length()%aBlockSize;
		// Test that the total length is a multiple of blockSize
		TEST((totalLength % aBlockSize) == 0);
      
		// Test that the padding bytes are equal in value to the paddingLength,
		// ie, if padding length is 5 the 5 last octets in the out array should be 0x05
		// This is according to RFC2246 (TLS1.0). The padding content in SSL3.0 is arbitrary.
		for (TInt i = paddingLength; i > 0 ; i--)
			{
			TEST(out[out.Length()-i] == paddingLength - 1);
			}

		// Test that the data has not been corrupted
		TEST(in == out.Left(out.Length() - paddingLength));
      
		CleanupStack::PopAndDestroy(2, padInData); // padInData, padOutData
		}
	CleanupStack::PopAndDestroy(padding);
	}

void CTestPadSSLv3::TestSSLv3CorruptPadding(TInt aBlockSize, TInt aTextSize)
	{
	CPaddingSSLv3 *padding = CPaddingSSLv3::NewLC(aBlockSize);

	HBufC8 *padInData = HBufC8::NewLC(aTextSize);
	TInt paddingBytes = 0;
	//If BlockSize is 0, Divide by 0 is undefined
	if(aBlockSize != 0)
		{
		paddingBytes = (aBlockSize - aTextSize % aBlockSize);
		}
	HBufC8 *padOutData = HBufC8::NewLC(aTextSize + paddingBytes);
	TPtr8 in(padInData->Des());
	TPtr8 out(padOutData->Des());
	TInt j;
      
	for (j = 0; j < aTextSize; j++)
   		{
	 	TInt text('a'+j%25);
	 	in.Append(text);
      	}
	TRAPD(err, padding->DoPadL(in, out));
  
	if(iExpectedResult.Compare(_L("CorruptBlockSize")) ==0)
 		{
 		TEST(err == KErrArgument);
 		}
	else if(iExpectedResult.Compare(_L("Valid")) ==0)
 		{
 		TEST(err == KErrNone);
 		}
	else if(iExpectedResult.Compare(_L("InvalidPadding")) == 0)
		{
		TEST(err == KErrInvalidPadding);
		}	

	TInt totalLength = out.Length();
	TInt paddingLength = 0;
	//If BlockSize is 0, Divide by 0 is undefined
	if(aBlockSize != 0)
		{
      	paddingLength = aBlockSize - in.Length()%aBlockSize;
		}
	//If BlockSize is 0, Divide by 0 is undefined
	if(aBlockSize != 0)
		{
   		// Test that the total length is a multiple of blockSize
		TEST((totalLength % aBlockSize) == 0);
		}
      
	// Test that the padding bytes are equal in value to the paddingLength,
	// ie, if padding length is 5 the 5 last octets in the out array should be 0x05
	// This is according to RFC2246 (TLS1.0). The padding content in SSL3.0 is arbitrary.
	for (TInt i = paddingLength; i > 0 ; i--)
		{
	 	TEST(out[out.Length()-i] == paddingLength - 1);
      	}

	if(aBlockSize > 0)
   		{
   		// Test that the data has not been corrupted
   		TEST(in == out.Left(out.Length() - paddingLength));
   		}
          
	CleanupStack::PopAndDestroy(2, padInData); // padInData, padOutData
	CleanupStack::PopAndDestroy(padding);
	}

CTestUnpadSSLv3::CTestUnpadSSLv3()
	{
	SetTestStepName(KUnpadSSLv3);
	}

CTestUnpadSSLv3::~CTestUnpadSSLv3()
	{
	}

TVerdict CTestUnpadSSLv3::doTestStepL()
	{
	SetTestStepResult(EPass);
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test of unpadding with type SSLv3"));

	TInt blockSize;
	TInt textSize;
	if (!GetIntFromConfig(ConfigSection(), _L("BlockSize"), blockSize))
 		{
 		INFO_PRINTF1(_L("Could not find blocksize in tpadSSLv3.ini"));
      	return EFail;
 		}
 		
 	if (!GetStringFromConfig(ConfigSection(), _L("TestCaseName"), iTestCaseName))
 		{
 		INFO_PRINTF1(_L("Could not find TestCaseName in tpadSSLv3.ini"));
      	return EFail;
 		}	
 	
 	if(iTestCaseName.Compare(_L("CipherAES_CBC"))==0 || iTestCaseName.Compare(_L("CipherDES_CBC"))==0 || iTestCaseName.Compare(_L("CipherRC2_CBC"))==0)
 		{
 		TestSSLv3Unpadding(blockSize);
 		}
 	else
 		{
 		if (!GetIntFromConfig(ConfigSection(), _L("TextSize"), textSize))
   			{
      		INFO_PRINTF1(_L("Could not find TextSize in tpadSSLv3.ini"));
      		return EFail;
   			}
   			
   		if (!GetStringFromConfig(ConfigSection(), _L("ExpectedResult"), iExpectedResult))
 			{
 			INFO_PRINTF1(_L("Could not find TextSize in tpadSSLv3.ini"));
      		return EFail;
 			}
 		TestSSLv3CorruptUnpadding(blockSize, textSize);	
 		}

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CTestUnpadSSLv3::TestSSLv3Unpadding(TInt aBlockSize)
	{
	CPaddingSSLv3 *padding = CPaddingSSLv3::NewLC(aBlockSize);

	for (TInt i = 0 ; i <= aBlockSize; i++)
		{
		HBufC8 *padInData = HBufC8::NewLC(i+(aBlockSize - i%aBlockSize));
		HBufC8 *padOutData = HBufC8::NewLC(i);
		HBufC8 *padCompareData = HBufC8::NewLC(i);
		TPtr8 in(padInData->Des());
		TPtr8 out(padOutData->Des());
		TPtr8 comp(padCompareData->Des());
		TInt j;
      
		// build up a padded string here
		for (j = 0; j < i; j++)
			{
			TInt text('a'+j%25);
			in.Append(text);
			}
		comp.Append(in);
      
		TInt paddingBytes = aBlockSize - i%aBlockSize;
		in.SetLength(in.Length() + paddingBytes);

		// pad with arbitary data, to test unpadding of SSL 3.0 padded data
		in[in.Length()-1] = (TUint8) (paddingBytes - 1);
		for (j = 2; j <= paddingBytes; j++)
			{
			in[in.Length()-j] = (TUint8) ('a' + j%25);
			}
		TRAPD(err, padding->UnPadL(in, out));
		INFO_PRINTF3(_L("The error returned for input size %d is %d"), i, err);
		TEST(err == KErrNone);

		// test if the unpadding was correct.
		TEST(out == comp);
      
		CleanupStack::PopAndDestroy(3, padInData); // padInData, padOutData, padCompareData
		}
	CleanupStack::PopAndDestroy(padding);
	}

void CTestUnpadSSLv3::TestSSLv3CorruptUnpadding(TInt aBlockSize, TInt aTextSize)
	{
	CPaddingSSLv3 *padding = CPaddingSSLv3::NewLC(aBlockSize);
   
	TInt paddingBytes = 0;
	//If BlockSize is 0, Divide by 0 is undefined
	if(aBlockSize != 0)
   		{
   		paddingBytes = aBlockSize - aTextSize%aBlockSize;
   		}
	HBufC8 *padInData = HBufC8::NewLC(aTextSize+ paddingBytes);
	HBufC8 *padOutData = HBufC8::NewLC(aTextSize);
	HBufC8 *padCompareData = HBufC8::NewLC(aTextSize);
	TPtr8 in(padInData->Des());
	TPtr8 out(padOutData->Des());
	TPtr8 comp(padCompareData->Des());
	TInt j;
   
	if(in.Length() < aTextSize)
		{
		for (j = 0; j < in.Length(); j++)
   			{
	 		TInt text('a'+j%25);
	 		in.Append(text);
      		}
		}
	else
		{
		// build up a padded string here
		for (j = 0; j < aTextSize; j++)
   			{
	 		TInt text('a'+j%25);
	 		in.Append(text);
      		}
		}
	comp.Append(in);
   
	in.SetLength(in.Length() + paddingBytes);

	if(aBlockSize > 0)
  		{
   		// pad with arbitary data, to test unpadding of SSL 3.0 padded data
   		in[in.Length()-1] = (TUint8) (paddingBytes - 1);
   		for (j = 2; j <= paddingBytes; j++)
   			{
	 		in[in.Length()-j] = (TUint8) ('a' + j%25);
      		}
  		 }  
	TRAPD(err, padding->UnPadL(in, out));
   
    if(iExpectedResult.Compare(_L("CorruptBlockSize")) ==0)
 		{
 		TEST(err == KErrArgument);
 		}
	else if(iExpectedResult.Compare(_L("Valid")) ==0)
 		{
 		TEST(err == KErrNone);
 		}
	else if(iExpectedResult.Compare(_L("InvalidPadding")) == 0)
		{
		TEST(err == KErrInvalidPadding);
		}
	// test if the unpadding was correct.
	TEST(out == comp);
	CleanupStack::PopAndDestroy(3, padInData); // padInData, padOutData, padCompareData
	CleanupStack::PopAndDestroy(padding);
	}
