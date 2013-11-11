/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __SQLITE_TEST_UTL_H__
#define __SQLITE_TEST_UTL_H__

#ifdef __cplusplus
extern "C" {
#endif

void TestOpen(const char* title);
void TestTitle(void);
void TestStart(const char* title);
void TestNext(const char* title);
void TestClose(void);
void TestEnd(void);
void TestAbort(TInt aLine);
void TestTestLine(int aResult, int aLine);
void TestTest(int aResult);
void TestPrintf(const char* title);

void TestHeapMark(void);
void TestHeapMarkEnd(void);

void Print(const char* msg);
void PrintI(const char* fmt, int a1);
void PrintIII(const char* fmt, int a1, int a2, int a3);
void PrintII64I64(const char* fmt, int a1, __int64 a2, __int64 a3);
void PrintSI(const char* fmt, const char* s, int a1);
void PrintS(const char* fmt, const char* s);

void CreatePrivateDir(void);

#ifdef __cplusplus
}
#endif

#endif
