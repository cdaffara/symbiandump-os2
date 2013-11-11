// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
#ifndef t_logutil_h
#define t_logutil_h

#include <e32test.h>

void LogTestBoolExpr(TBool aRes, const TText* aFile, TInt aLine, TBool aPrintThreadName = EFalse);
void LogCheck(TInt aValue, TInt aExpected, const TText* aFile, TInt aLine, TBool aPrintThreadName = EFalse);
void LogCheckU(TUint aValue, TUint aExpected, const TText* aFile, TInt aLine, TBool aPrintThreadName = EFalse);

#define TEST_STRING(s) _S(s)

#define TEST(arg)                   LogTestBoolExpr((arg), TEST_STRING(__FILE__), __LINE__)
#define TEST2(aValue, aExpected)    LogCheck((aValue), (aExpected), TEST_STRING(__FILE__), __LINE__)
#define TEST2U(aValue, aExpected)   LogCheckU((aValue), (aExpected), TEST_STRING(__FILE__), __LINE__)

#define TTEST(arg)                  LogTestBoolExpr((arg), TEST_STRING(__FILE__), __LINE__, ETrue)
#define TTEST2(aValue, aExpected)   LogCheck((aValue), (aExpected), TEST_STRING(__FILE__), __LINE__, ETrue)
#define TTEST2U(aValue, aExpected)  LogCheckU((aValue), (aExpected), TEST_STRING(__FILE__), __LINE__, ETrue)

void LogLeave(TInt aErr, const TText* aFile, TInt aLine);

#define LEAVE(err)           LogLeave(err, TEST_STRING(__FILE__), __LINE__)
#define LEAVE_IF_ERROR(err)  do {if(err < KErrNone) LogLeave(err, TEST_STRING(__FILE__), __LINE__);} while(0)


void LogPanic(const TDesC& aCategory, TInt aErr, const TText* aFile, TInt aLine);

#define PANIC(cat, err)     LogPanic(cat, err, TEST_STRING(__FILE__), __LINE__)

TPtrC FileName(const TText* aFile);
TInt KillProcess(const TDesC& aProcessName);

#endif//t_logutil_h
