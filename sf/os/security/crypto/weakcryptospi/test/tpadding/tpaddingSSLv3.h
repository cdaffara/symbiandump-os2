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


#ifndef _TPADDINGSSLV3_H_
#define _TPADDINGSSLV3_H_

#include <test/testexecutestepbase.h>
#include "tpaddingTestSteps.h"

_LIT(KPadSSLv3, "PadSSLv3");
_LIT(KUnpadSSLv3, "UnpadSSLv3");

class CTestPadSSLv3 : public CPaddingStep
{
   public:
      CTestPadSSLv3();
      ~CTestPadSSLv3();
      virtual TVerdict doTestStepL();
   private:
      void TestSSLv3Padding(TInt aBlockSize);
	  void TestSSLv3CorruptPadding(TInt aBlockSize, TInt aTextSize);
   private:
	   TPtrC iExpectedResult;
	   TPtrC iTestCaseName;

};

class CTestUnpadSSLv3 : public CPaddingStep
{
   public:
      CTestUnpadSSLv3();
      ~CTestUnpadSSLv3();
      virtual TVerdict doTestStepL();
   private:
      void TestSSLv3Unpadding(TInt aBlockSize);
	  void TestSSLv3CorruptUnpadding(TInt aBlockSize, TInt aTextSize);
   private:
	   TPtrC iExpectedResult;
	   TPtrC iTestCaseName;
};


#endif
