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
#include "tpaddingPKCS1.h"

CTestPadPKCS1::CTestPadPKCS1()
{
   SetTestStepName(KPadPKCS1);
}

CTestPadPKCS1::~CTestPadPKCS1()
{
}

TVerdict CTestPadPKCS1::doTestStepL()
{

   __UHEAP_MARK;
   
   INFO_PRINTF1(_L("Test of PKCS1 signature padding"));

   SetTestStepResult(EPass);
   INFO_PRINTF1(_L("Blocksize 512"));
   TestPKCS1SignaturePadding(512);

   SetTestStepResult(EPass);
   INFO_PRINTF1(_L("Blocksize 1024"));
   TestPKCS1SignaturePadding(1024);

   SetTestStepResult(EPass);
   INFO_PRINTF1(_L("Blocksize 2048"));
   TestPKCS1SignaturePadding(2048);

   INFO_PRINTF1(_L("Test of PKCS1 encryption padding"));

   SetTestStepResult(EPass);
   INFO_PRINTF1(_L("Blocksize 512"));
   TestPKCS1EncryptionPadding(512);

   SetTestStepResult(EPass);
   INFO_PRINTF1(_L("Blocksize 1024"));
   TestPKCS1EncryptionPadding(1024);

   SetTestStepResult(EPass);
   INFO_PRINTF1(_L("Blocksize 2048"));
   TestPKCS1EncryptionPadding(2048);
   

   __UHEAP_MARKEND;
   return TestStepResult();
}

void CTestPadPKCS1::TestPKCS1SignaturePadding(TInt aBlockSize)
{
   CPaddingPKCS1Signature *padding = CPaddingPKCS1Signature::NewLC(aBlockSize);
   /*
    * The length of the data D shall not be more than k-11 octets,
    * which is positive since the length k of the modulus is at
    * least 12 octets.
    */
   TInt noDataBytes = aBlockSize-11;
   
   for (TInt i = 0; i <= noDataBytes; i++)
   {
      HBufC8 *inData = HBufC8::NewLC(noDataBytes);
      HBufC8 *outData = HBufC8::NewLC(aBlockSize);
      TPtr8 in(inData->Des());
      TPtr8 out(outData->Des());
      TInt j;
      
      for (j = 0; j < i; j++)
      {
	 TInt text('a'+j%25);
	 in.Append(text);
      }
      
      TRAPD(err, padding->DoPadL(in, out));
      TEST(err == KErrNone);
      
      // check the padding.
      // should have the following syntax:
      // 00 || 01 || PS || 00 || D where PS is the padding octects of value 0xff
      
      TEST(out[0] == 0 && out[1] == 1);

      TInt endOfPadding = aBlockSize - 1 - in.Length();
      for (j = 2; j < endOfPadding; j++)
      {
	 TEST(out[j]==0xff);
	 
      }
      // Check for the delimiter
      TEST(out[endOfPadding] == 0);
      // Check that the rest of the data is not corrupted
      TInt startOfData = aBlockSize - in.Length();
      TEST(in == out.Mid(startOfData));
      CleanupStack::PopAndDestroy(2, inData); // inData, outData
   }
   TestStepResult() ? INFO_PRINTF1(_L("Test failed")) : INFO_PRINTF1(_L("Test passed"));
   CleanupStack::PopAndDestroy(); // padding
}

void CTestPadPKCS1::TestPKCS1EncryptionPadding(TInt aBlockSize)
{
   CPaddingPKCS1Encryption *padding = CPaddingPKCS1Encryption::NewLC(aBlockSize);
   /*
    * The length of the data D shall not be more than k-11 octets,
    * which is positive since the length k of the modulus is at
    * least 12 octets.
    */
   TInt noDataBytes = aBlockSize-11;
   
   for (TInt i = 0; i <= noDataBytes; i++)
   {
      HBufC8 *inData = HBufC8::NewLC(noDataBytes);
      HBufC8 *outData = HBufC8::NewLC(aBlockSize);
      TPtr8 in(inData->Des());
      TPtr8 out(outData->Des());
      TInt j;
      
      for (j = 0; j < i; j++)
      {
	 TInt text('a'+j%25);
	 in.Append(text);
      }
      
      TRAPD(err, padding->DoPadL(in, out));
      TEST(err == KErrNone);
      // check the padding.
      // should have the following syntax:
      // 00 || 02 || PS || 00 || D where PS is the padding octects containing random data
      
      TEST(out[0] == 0 && out[1] == 2);
      
      TInt endOfPadding = aBlockSize - 1 - in.Length();
      for (j = 2; j < endOfPadding; j++)
      {
	 TEST(out[j]!=0);
	 
      }
      // Check for the delimiter
      TEST(out[endOfPadding] == 0);
      
      // Check that the rest of the data is not corrupted
      TInt startOfData = aBlockSize - in.Length();
      TEST(in == out.Mid(startOfData));
      CleanupStack::PopAndDestroy(2, inData); // inData, outData
   }
   TestStepResult() ? INFO_PRINTF1(_L("Test failed")) : INFO_PRINTF1(_L("Test passed"));
   CleanupStack::PopAndDestroy(); // padding
}

   
CTestUnpadPKCS1::CTestUnpadPKCS1()
{
   SetTestStepName(KUnpadPKCS1);
}

CTestUnpadPKCS1::~CTestUnpadPKCS1()
{
}

TVerdict CTestUnpadPKCS1::doTestStepL()
{
   __UHEAP_MARK;

   INFO_PRINTF1(_L("Test of PKCS1 signature unpadding"));
   INFO_PRINTF1(_L("Blocksize 512"));
   SetTestStepResult(EPass);
   TestPKCS1SignatureUnpadding(512);
   INFO_PRINTF1(_L("Blocksize 1024"));
   SetTestStepResult(EPass);
   TestPKCS1SignatureUnpadding(1024);
   INFO_PRINTF1(_L("Blocksize 2048"));
   SetTestStepResult(EPass);
   TestPKCS1SignatureUnpadding(2048);

   INFO_PRINTF1(_L("Test of PKCS1 encryption unpadding"));
   INFO_PRINTF1(_L("Blocksize 512"));
   SetTestStepResult(EPass);
   TestPKCS1EncryptionUnpadding(512);
   INFO_PRINTF1(_L("Blocksize 1024"));
   SetTestStepResult(EPass);
   TestPKCS1EncryptionUnpadding(1024);
   INFO_PRINTF1(_L("Blocksize 2048"));
   SetTestStepResult(EPass);
   TestPKCS1EncryptionUnpadding(2048);

   __UHEAP_MARKEND;
   return TestStepResult();
}

void CTestUnpadPKCS1::TestPKCS1SignatureUnpadding(TInt aBlockSize)
{
   CPaddingPKCS1Signature *padding = CPaddingPKCS1Signature::NewLC(aBlockSize);
   /*
    * The length of the data D shall not be more than k-11 octets,
    * which is positive since the length k of the modulus is at
    * least 12 octets.
    */
   TInt noDataBytes = aBlockSize-11;
   
   for (TInt i = 0; i <= noDataBytes; i++)
   {
      HBufC8 *inData = HBufC8::NewLC(aBlockSize);
      HBufC8 *outData = HBufC8::NewLC(noDataBytes);
      HBufC8 *compData = HBufC8::NewLC(noDataBytes);
      TPtr8 in(inData->Des());
      TPtr8 out(outData->Des());
      TPtr8 comp(compData->Des());
      
      TInt endOfPadding = aBlockSize - 1 - i;
      in.SetLength(endOfPadding+1);
      in[0] = 0;
      in[1] = 1;
      TInt j;
      for (j = 2; j < endOfPadding; j++)
      {
	 in[j] = 0xff;
      }
      in[endOfPadding] = 0; // delimiter

      for (j = 0; j < i; j++)
      {
	 TInt text('a'+j%25);
	 in.Append(text);
	 comp.Append(text);
      }

      TRAPD(err, padding->UnPadL(in, out));
      TEST(err == KErrNone);

      TEST(out == comp);

      TEST(out.Length() == i);

      
      CleanupStack::PopAndDestroy(3, inData); // inData, outData, compData
   }
   TestStepResult() ? INFO_PRINTF1(_L("Test failed")) : INFO_PRINTF1(_L("Test passed"));
   CleanupStack::PopAndDestroy(); // padding
}

void CTestUnpadPKCS1::TestPKCS1EncryptionUnpadding(TInt aBlockSize)
{
   CPaddingPKCS1Encryption *padding = CPaddingPKCS1Encryption::NewLC(aBlockSize);
   /*
    * "The length of the data D shall not be more than k-11 octets,
    * which is positive since the length k of the modulus is at
    * least 12 octets."
    */
   TInt noDataBytes = aBlockSize-11;
   for (TInt i = 0; i <= noDataBytes; i++)
   {
      HBufC8 *inData = HBufC8::NewLC(aBlockSize);
      HBufC8 *outData = HBufC8::NewLC(noDataBytes);
      HBufC8 *compData = HBufC8::NewLC(noDataBytes);
      TPtr8 in(inData->Des());
      TPtr8 out(outData->Des());
      TPtr8 comp(compData->Des());
      
      TInt endOfPadding = aBlockSize - 1 - i;
      in.SetLength(endOfPadding+1);
      in[0] = 0;
      in[1] = 2;
      TBuf8<256> rnd(256);
      GenerateRandomBytesL(rnd);

      TInt k = 2;
      TInt j = 0;
      while (k < endOfPadding)
      {
         if (rnd[j%256])
         {
            in[k++] = rnd[j%256];
         }
         
         if (++j%256 == 0)
         {
            GenerateRandomBytesL(rnd);
         }
      }
      in[endOfPadding] = 0; // delimiter

      for (j = 0; j < i; j++)
      {
	 TInt text('a'+j%25);
	 in.Append(text);
     comp.Append(text);
      }

      TRAPD(err, padding->UnPadL(in, out));
      TEST(err == KErrNone);

	  TEST(out == comp);		  

      TEST(out.Length() == i);

      
      CleanupStack::PopAndDestroy(3, inData); // inData, outData, compData
   }
   TestStepResult() ? INFO_PRINTF1(_L("Test failed")) : INFO_PRINTF1(_L("Test passed"));
   CleanupStack::PopAndDestroy(); // padding
}
