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


#ifndef _TPADDINGPKCS7_H_
#define _TPADDINGPKCS7_H_

#include <e32std.h>
#include <e32cmn.h>


#include <test/testexecutestepbase.h>
#include "tpaddingTestSteps.h"

_LIT(KPadPKCS7, "PadPKCS7");
_LIT(KUnpadPKCS7, "UnpadPKCS7");
_LIT(KUnpadCorruptPKCS7, "UnpadCorruptPKCS7");
_LIT(KPaddingCorruptPKCS7, "PaddingCorruptPKCS7");


class CTestPadPKCS7 : public CPaddingStep
{
 	public:
 		CTestPadPKCS7();
 		~CTestPadPKCS7();
 		virtual TVerdict doTestStepL();
 	private:
 		void TestPKCS7Padding(TInt aBlockSize);
};

class CTestUnpadPKCS7 : public CPaddingStep
{
    public:
 		CTestUnpadPKCS7();
 		~CTestUnpadPKCS7();
 		virtual TVerdict doTestStepL();
 	private:
 		void TestPKCS7Unpadding(TInt aBlockSize);

};

class CTestUnpadCorruptPKCS7 : public CPaddingStep
 {
    public:
 		CTestUnpadCorruptPKCS7();
 		~CTestUnpadCorruptPKCS7();
 		virtual TVerdict doTestStepL();
 	private:
 		void TestCorruptPKCS7Unpadding(TInt aBlockSize, TInt aTextSize, TUint8 aCorruptPaddingByte);
 };

class CTestPaddingCorruptPKCS7 : public CPaddingStep
	{
public:
	CTestPaddingCorruptPKCS7();
    ~CTestPaddingCorruptPKCS7();
    virtual TVerdict doTestStepL();
private:
    void TestCorruptPKCS7padding(TInt aBlockSize, TInt aTextSize);
};

#endif
