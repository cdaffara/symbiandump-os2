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
#include "t_logutil.h"

//Define "TheTest" variable used in the test cpp files
extern RTest TheTest;

TPtrC FileName(const TText* aFile)
    {
    TPtrC p(aFile);
    TInt ix=p.LocateReverse('\\');
    if (ix<0)
        ix=p.LocateReverse('/');
    if (ix>=0)
        p.Set(p.Mid(1+ix));
    return p;
    }

void LogTestBoolExpr(TBool aRes, const TText* aFile, TInt aLine, TBool aPrintThreadName)
    {
    if(!aRes)
        {
        TPtrC fname(FileName(aFile));
        if(aPrintThreadName)
            {
            RThread th;
            TName name = th.Name();
            RDebug::Print(_L("*** Boolean expression evaluated to false, file: %S-%d\r\n"), &fname, aLine);
            User::Panic(_L("t_logutil-1"), 1);
            }
        else
            {
            TheTest.Printf(_L("*** Boolean expression evaluated to false, file: %S-%d\r\n"), &fname, aLine);
            TheTest(EFalse, aLine);
            }
        }
    }

void LogCheck(TInt aValue, TInt aExpected, const TText* aFile, TInt aLine, TBool aPrintThreadName)
	{
	if(aValue != aExpected)
		{
        TPtrC fname(FileName(aFile));
        if(aPrintThreadName)
            {
            RThread th;
            TName name = th.Name();
            RDebug::Print(_L("*** Expected error: %d, got: %d, file: %S-%d\r\n"), aExpected, aValue, &fname, aLine);
            User::Panic(_L("t_logutil-2"), 2);
            }
        else
            {
            TheTest.Printf(_L("*** Expected error: %d, got: %d, file: %S-%d\r\n"), aExpected, aValue, &fname, aLine);
            TheTest(EFalse, aLine);
            }
		}
	}

void LogCheckU(TUint aValue, TUint aExpected, const TText* aFile, TInt aLine, TBool aPrintThreadName)
	{
	if(aValue != aExpected)
		{
	    TPtrC fname(FileName(aFile));
        if(aPrintThreadName)
            {
            RThread th;
            TName name = th.Name();
            RDebug::Print(_L("*** Expected error: %u, got: %u, file: %S-%d\r\n"), aExpected, aValue, &fname, aLine);
            User::Panic(_L("t_logutil-3"), 3);
            }
        else
            {
            TheTest.Printf(_L("*** Expected error: %u, got: %u, file: %S-%d\r\n"), aExpected, aValue, &fname, aLine);
            TheTest(EFalse, aLine);
            }
		}
	}

void LogLeave(TInt aErr, const TText* aFile, const TInt aLine)
    {
    TPtrC fname(FileName(aFile));
    TheTest.Printf(_L("*** LogEng test leave, err=%d, file: %S-%d\r\n"), aErr, &fname, aLine);
    User::Leave(aErr);
    }

void LogPanic(const TDesC& aCategory, TInt aErr, const TText* aFile, TInt aLine)
    {
    TPtrC fname(FileName(aFile));
    TheTest.Printf(_L("*** LogEng test panic'd with err=%d, category=%S, file: %S-%d\r\n"), aErr, &aCategory, &fname, aLine);
    User::Panic(aCategory, aErr);
    }

TInt KillProcess(const TDesC& aProcessName)
    {
    TFullName name;

    TheTest.Printf(_L("Find and kill \"%S\" process.\n"), &aProcessName);

    TBuf<64> pattern(aProcessName);
    TInt length = pattern.Length();
    pattern += _L("*");
    TFindProcess procFinder(pattern);

    while (procFinder.Next(name) == KErrNone)
        {
        if (name.Length() > length)
            {//If found name is a string containing aProcessName string.
            TChar c(name[length]);
            if (c.IsAlphaDigit() ||
                c == TChar('_') ||
                c == TChar('-'))
                {
                // If the found name is other valid application name
                // starting with aProcessName string.
                TheTest.Printf(_L(":: Process name: \"%S\".\n"), &name);
                continue;
                }
            }
        RProcess proc;
        if (proc.Open(name) == KErrNone)
            {
            proc.Kill(0);
            TheTest.Printf(_L("\"%S\" process killed.\n"), &name);
            }
        proc.Close();
        }
    return KErrNone;
    }
