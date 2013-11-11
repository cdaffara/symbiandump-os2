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


#ifndef _TPADDINGPKCS1_H_
#define _TPADDINGPKCS1_H_

#include <test/testexecutestepbase.h>
#include "tpaddingTestSteps.h"

_LIT(KPadPKCS1, "PadPKCS1");
_LIT(KUnpadPKCS1, "UnpadPKCS1");

class CTestPadPKCS1 : public CPaddingStep
{
   public:
      CTestPadPKCS1();
      ~CTestPadPKCS1();
      virtual TVerdict doTestStepL();
   private:
      void TestPKCS1SignaturePadding(TInt aBlockSize);
      void TestPKCS1EncryptionPadding(TInt aBlockSize);
};

class CTestUnpadPKCS1 : public CPaddingStep
{
   public:
      CTestUnpadPKCS1();
      ~CTestUnpadPKCS1();
      virtual TVerdict doTestStepL();
   private:
      void TestPKCS1SignatureUnpadding(TInt aBlockSize);
      void TestPKCS1EncryptionUnpadding(TInt aBlockSize);
};


#endif
