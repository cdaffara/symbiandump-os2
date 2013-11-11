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


#include <random.h>
#include <padding.h>
#include <securityerr.h>
#include "tpaddingPKCS7.h"
#include <cryptopanic.h>

static void GenerateInput(TInt aBytes, TPtr8& in)
 	{
 	for (TInt j = 0; j < aBytes; j++)
 		{
 		TInt text('a'+j%25);
 		in.Append(text);
 		}
 	}

CTestPadPKCS7::CTestPadPKCS7()
	{
	SetTestStepName(KPadPKCS7);
	}

CTestPadPKCS7::~CTestPadPKCS7()
	{
	}

TVerdict CTestPadPKCS7::doTestStepL()
	{
	SetTestStepResult(EPass);
	__UHEAP_MARK;
   
	INFO_PRINTF1(_L("Test of PKCS7  padding"));

	for (TInt i = 1; i < 255; i++)
		{
		TestPKCS7Padding(i);
		}
  
	__UHEAP_MARKEND;
	return TestStepResult();
	}


void CTestPadPKCS7::TestPKCS7Padding(TInt aBlockSize)
	{
   	CPaddingPKCS7 *padding = CPaddingPKCS7::NewLC(aBlockSize);
	// Starts with zero input size(total block data is filled/padded with block size) 
	for (TInt i = 0 ; i <= aBlockSize; i++)
		{
		HBufC8 *padInData = HBufC8::NewLC(i);
		HBufC8 *padOutData = HBufC8::NewLC(i+(aBlockSize-i%aBlockSize));
		TPtr8 in(padInData->Des());
		TPtr8 out(padOutData->Des());

		GenerateInput(i, in);
		TRAPD(err, padding->PadL(in, out));
		
		TEST(err == KErrNone);
		
		TInt totalLength = out.Length();
		TUint paddingLength = aBlockSize - in.Length()%aBlockSize;
		// Test that the total length is a multiple of blockSize
		TEST((totalLength % aBlockSize) == 0);
      
		// Test that the padding bytes are equal in value to the paddingLength,
		// ie, if padding length is 5 the 5 last octets in the out array should be 0x05
		for (TInt j = paddingLength; j > 0 ; j--)
			{
			TEST(out[out.Length()-j] == paddingLength);
			}
      
		// Test that the data has not been corrupted
		TEST(in == out.Left(out.Length() - paddingLength));
		
		CleanupStack::PopAndDestroy(2, padInData); // padInData, padOutData
		}
	CleanupStack::PopAndDestroy(padding);
	}
   
CTestUnpadPKCS7::CTestUnpadPKCS7()
	{
   	SetTestStepName(KUnpadPKCS7);
	}

CTestUnpadPKCS7::~CTestUnpadPKCS7()
	{
	}

TVerdict CTestUnpadPKCS7::doTestStepL()
	{
	SetTestStepResult(EPass);
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test of PKCS7 unpadding"));
	for (TInt i = 1; i < 255; i++)
		{
		TestPKCS7Unpadding(i);
		}

	__UHEAP_MARKEND;
	return TestStepResult();
	}


void CTestUnpadPKCS7::TestPKCS7Unpadding(TInt aBlockSize)
	{
	CPaddingPKCS7 *padding = CPaddingPKCS7::NewLC(aBlockSize);

	// Input must be < aBlockSize otherwise this wouldn't be 
	// a padded block
	for (TInt i = 0 ; i < aBlockSize; i++)
		{
		// Input to un-padding should always be an entire block
		// for correctly data.
		HBufC8 *padInData = HBufC8::NewLC(aBlockSize);
		HBufC8 *padOutData = HBufC8::NewLC(i);
		HBufC8 *padCompareData = HBufC8::NewLC(i);
		TPtr8 in(padInData->Des());
		TPtr8 out(padOutData->Des());
		TPtr8 comp(padCompareData->Des());

		GenerateInput(i, in);
		comp.Copy(in);		

		in.SetLength(aBlockSize);
		TInt paddingBytes = aBlockSize - (i % aBlockSize);
		for (TInt j = 1; j <= paddingBytes; j++)
			{
			in[in.Length()-j] = (TUint8)paddingBytes;
			}

		TRAPD(err, padding->UnPadL(in, out));
		if (err != KErrNone)
			{
			INFO_PRINTF3(_L("The Error returned for block size %d is %d"), aBlockSize,err);
			}
		TEST(err == KErrNone); // Verify UnPadL leave code 
		TEST(out == comp); // Verify  UnPadL output data with expected data
		CleanupStack::PopAndDestroy(3, padInData); // padInData, padOutData, padCompareData
		}
	CleanupStack::PopAndDestroy(padding);
	}

 
CTestUnpadCorruptPKCS7::CTestUnpadCorruptPKCS7()
	{
	SetTestStepName(KUnpadCorruptPKCS7);
	}
 
CTestUnpadCorruptPKCS7::~CTestUnpadCorruptPKCS7()
	{
	}
 
TVerdict CTestUnpadCorruptPKCS7::doTestStepL()
 	{
 	SetTestStepResult(EPass);
 	__UHEAP_MARK;
 
 	TInt blockSize;
 	TInt textSize;
 	TInt paddingNum = 0;
 	
 	if (GetIntFromConfig(ConfigSection(), _L("blocksize"), blockSize))
 		{
 		if (GetIntFromConfig(ConfigSection(), _L("textsize"), textSize))
 			{
 			if (GetIntFromConfig(ConfigSection(), _L("paddingbyte"), paddingNum))
 				{
 				INFO_PRINTF1(_L("Test of PKCS7 unpadding with corrupt data"));
 				TUint8 paddingByte = Min(paddingNum, 255);
   				TestCorruptPKCS7Unpadding(blockSize, textSize, paddingByte);	  					
 				}				
 			else
 				{
 				ERR_PRINTF1(_L("Missing parameter - paddingbyte"));
 				}
 			}
 		else
 			{
 			ERR_PRINTF1(_L("Missing parameter - textsize"));
 			}
 		}
 	else
		{
 		ERR_PRINTF1(_L("Missing parameter - blocksize"));
 		}
 
 	__UHEAP_MARKEND;
 	return TestStepResult();
 	}
 
void CTestUnpadCorruptPKCS7::TestCorruptPKCS7Unpadding(TInt aBlockSize, TInt aTextSize, TUint8 aPaddingByte)
	{
 	CPaddingPKCS7 *padding = CPaddingPKCS7::NewLC(aBlockSize);
 	TInt paddingBytes = (aBlockSize - aTextSize % aBlockSize);
 	
	HBufC8 *padInData = HBufC8::NewLC(aTextSize + paddingBytes);
 	HBufC8 *padOutData = HBufC8::NewLC(aTextSize);
 	TPtr8 in(padInData->Des());
 	TPtr8 out(padOutData->Des());
 
 	GenerateInput(aTextSize, in);
 
 	in.SetLength(in.Length() + paddingBytes);
 	for (TInt j = 1; j <= paddingBytes; j++)
 		{
 		in[in.Length()-j] = (TUint8) aPaddingByte;
 		}
 
 	TRAPD(err, padding->UnPadL(in, out));
 	
 	if ( err == KErrInvalidPadding )
		{
  		INFO_PRINTF2(_L("The PKCS7 unpadding UnPadL method returned error is %d"), err);
  		TEST(err == KErrInvalidPadding);
  		SetTestStepResult(EPass);
  		}
	else if ( err == KErrNone )
		{
		TEST(err == KErrNone);
		}
	CleanupStack::PopAndDestroy(3, padding); // padding, padInData, padOutData
	}

CTestPaddingCorruptPKCS7::CTestPaddingCorruptPKCS7()
	{
	SetTestStepName(KPaddingCorruptPKCS7);
	}
 
CTestPaddingCorruptPKCS7::~CTestPaddingCorruptPKCS7()
	{
	}
 
TVerdict CTestPaddingCorruptPKCS7::doTestStepL()
 	{
 	SetTestStepResult(EPass);
 	__UHEAP_MARK;
 
 	TInt blockSize;
 	TInt textSize;
 	 	
 	if (GetIntFromConfig(ConfigSection(), _L("blocksize"), blockSize))
 		{
 		if (GetIntFromConfig(ConfigSection(), _L("textsize"), textSize))
 			{
 			INFO_PRINTF1(_L("Test of PKCS7 unpadding with corrupt data"));
   			TestCorruptPKCS7padding(blockSize, textSize);	  					
 			}
 		else
 			{
 			ERR_PRINTF1(_L("Missing parameter - textsize"));
 			}
 		}
 	else
		{
 		ERR_PRINTF1(_L("Missing parameter - blocksize"));
 		}
 
 	__UHEAP_MARKEND;
 	return TestStepResult();
 	}
 
void CTestPaddingCorruptPKCS7::TestCorruptPKCS7padding(TInt aBlockSize, TInt aTextSize)
	{
	CPaddingPKCS7 *padding = CPaddingPKCS7::NewLC(aBlockSize);

	TInt paddingBytes = 0;
	//Divide by 0 is undefined.
	if(aBlockSize != 0)
		{
		paddingBytes = aBlockSize - (aTextSize % aBlockSize);
		}
		
 	HBufC8 *padOutData = HBufC8::NewLC(aTextSize + paddingBytes);
 	HBufC8 *padInData = HBufC8::NewLC(aTextSize);
 	TPtr8 in(padInData->Des());
 	TPtr8 out(padOutData->Des());
 
 	GenerateInput(aTextSize, in);
 	TRAPD(err, padding->PadL(in, out));

	INFO_PRINTF2(_L("The PKCS7 padding PadL method returned error is %d"), err);
	
	//check expected result
	TPtrC expectedContent;
 	if (GetStringFromConfig(ConfigSection(), _L("case"), expectedContent))
 		{
 		if(expectedContent.Compare(_L("InvalidPadding")) ==0)
 			{
 			TEST(err == KErrInvalidPadding);
 			}
 		else if(expectedContent.Compare(_L("Valid")) ==0)
 			{
 			TEST(err == KErrNone);
 			}
 		else if(expectedContent.Compare(_L("CorruptBlockSize")) ==0)
 			{
 			TEST(err == KErrArgument);
 			}
 		}
 	
 	//skip the checking on padded data if padding is unsuccessful(no padding is done), 
 	//otherwise the erroneous operation on output descriptor will panic.
 	if(err != KErrNone)
 		{
 		CleanupStack::PopAndDestroy(3, padding);
 		return;
 		}
	
	TInt totalLength = out.Length();
	TInt inLength = in.Length();
	
	TUint paddingLength = 0;
	//Divide by 0 is undefined.
	if(aBlockSize != 0)
		{
		paddingLength = aBlockSize - inLength%aBlockSize;
		// Test that the total length is a multiple of blockSize
		TEST((totalLength % aBlockSize) == 0);
		}			
	
 	// Test that the padding bytes are equal in value to the paddingLength,
	// ie, if padding length is 5 the 5 last octets in the out array should be 0x05
	for (TInt j = paddingLength; j > 0 ; j--)
		{
		TEST(out[out.Length()-j] == paddingLength);
		}

	
 	
 	// Test that the data has not been corrupted
	TEST(in == out.Left(out.Length() - paddingLength));
 	
 	CleanupStack::PopAndDestroy(3, padding); // padInData, padOutData, padCompareData, padding
	}

