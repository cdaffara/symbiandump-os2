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
#include "tpaddingNone.h"

CTestPadNone::CTestPadNone()
{
   SetTestStepName(KPadNone);
}

CTestPadNone::~CTestPadNone()
{
}

TVerdict CTestPadNone::doTestStepL()
{
   SetTestStepResult(EPass);
   __UHEAP_MARK;

   INFO_PRINTF1(_L("Test of padding with type None"));
   
   TInt blockSize = 0; 
   if (!GetIntFromConfig(ConfigSection(), _L("blocksize"), blockSize))
 		{
 		ERR_PRINTF1(_L("Missing parameter - blockSize"));
 		}

   // Block size is arbitrary as no padding actually occurs
   CPaddingNone *padding = CPaddingNone::NewLC(blockSize);
   for (TInt i = 1 ; i < blockSize; i++)
   {
      HBufC8 *padInData = HBufC8::NewLC(i);
      HBufC8 *padOutData = HBufC8::NewLC(i);
      TPtr8 in(padInData->Des());
      TPtr8 out(padOutData->Des());
      for (TInt j = 0; j < i; j++)
      {
	 in.Append(_L8("a"));
      }

      // Perform the padding
      TRAPD(err, padding->PadL(in, out));
      TEST(err == KErrNone);
      
      // As no padding actually occurs, the in string should equal the out string
      TEST(in == out);
      CleanupStack::PopAndDestroy(2, padInData); // padInData, padOutData
   }
   
   CleanupStack::PopAndDestroy(); // padding
   __UHEAP_MARKEND;
   return TestStepResult();
}

CTestUnpadNone::CTestUnpadNone()
{
   SetTestStepName(KUnpadNone);
}

CTestUnpadNone::~CTestUnpadNone()
{
}

TVerdict CTestUnpadNone::doTestStepL()
{
   SetTestStepResult(EPass);
   __UHEAP_MARK;

   INFO_PRINTF1(_L("Test of unpadding with type None"));
   
   TInt blockSize = 0; 
   if (GetIntFromConfig(ConfigSection(), _L("blocksize"), blockSize))
 		{
 		ERR_PRINTF1(_L("Missing parameter - blockSize"));
 		}

   // Block size is arbitrary as no padding actually occurs
   CPaddingNone *padding = CPaddingNone::NewLC(blockSize);
   for (TInt i = 1 ; i < blockSize; i++)
   {
      HBufC8 *padInData = HBufC8::NewLC(i);
      HBufC8 *padOutData = HBufC8::NewLC(i);
      TPtr8 in(padInData->Des());
      TPtr8 out(padOutData->Des());
      for (TInt j = 0; j < i; j++)
      {
	 in.Append(_L8("a"));
      }

      // Perform the unpadding
      TRAPD(err, padding->UnPadL(in, out));
      TEST(err == KErrNone);

      // As no unpadding actually occurs, the in string should equal the out string
      TEST(in == out);
      CleanupStack::PopAndDestroy(2, padInData); // padInData, padOutData
   }
   CleanupStack::PopAndDestroy(); // padding
   __UHEAP_MARKEND;
   return TestStepResult();
}

