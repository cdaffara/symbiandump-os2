/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

#ifndef __T_STREAM_H__
#define __T_STREAM_H__

#include <test/testexecutestepbase.h>
#include <s32file.h>
#include <flddef.h>
#include <fldbase.h>
#include <fldset.h>

class CT_STREAM : public CTestStep
    {
public:
	CT_STREAM();
protected:  
    TVerdict doTestStepL();
private:
    void testStoreRestore(CTextFieldSet* aCopy,const CTextFieldSet* aOriginal);
    template <class T>
    void testCopyPaste(T* aCopy, T* aOriginal,TInt aCopyPos,TInt aLen,TInt aPastePos,TInt aPasteLen=-1);
    TBool IsEqual(const CTextFieldSet* aCopy,const CTextFieldSet* anOriginal);
    TBool UpdateField(TInt aPos,CTextFieldSet* aSet);
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();
    void test7();
    void test8();
    void test9();
    void test10();
    void TestStreamingL();
    void setupCleanup();
    void DeleteDataFile(const TDesC& aFullName);
    };

_LIT(KTestStep_T_STREAM, "T_STREAM");

#endif
