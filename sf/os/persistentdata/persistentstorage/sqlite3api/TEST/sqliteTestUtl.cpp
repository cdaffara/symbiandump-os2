// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <e32debug.h>
#include <f32file.h>
#include "sqliteTestUtl.h"

static TBuf8<500> TheBuf8;
static TBuf16<500> TheBuf16;

static RTest* TheTest;

extern "C" void TestOpen(const char* title)
	{
	TPtrC8 p((const unsigned char*)title);
	TheBuf16.Copy(p);
	TheTest = new RTest(TheBuf16);
	}

extern "C" void TestTitle()
	{
	TheTest->Title();
	}

extern "C" void TestStart(const char* title)
	{
	TPtrC8 p((const unsigned char*)title);
	TheBuf16.Copy(p);
	TheTest->Start(TheBuf16);
	}

extern "C" void TestNext(const char* title)
	{
	TPtrC8 p((const unsigned char*)title);
	TheBuf16.Copy(p);
	TheTest->Next(TheBuf16);
	}

extern "C" void TestClose(void)
	{
	TheTest->Close();
	delete TheTest;	
	}

extern "C" void TestEnd(void)
	{
	TheTest->End();	
	}

extern "C" void TestAbort(int aLine)
	{
	TheTest->operator()(0, aLine);	
	}

extern "C" void TestTestLine(int aResult, int aLine)
	{
	TheTest->operator()(aResult, aLine);
	}

extern "C" void TestTest(int aResult)
	{
	TheTest->operator()(aResult);
	}

extern "C" void TestPrintf(const char* title)
	{
	TPtrC8 p((const unsigned char*)title);
	TheBuf16.Copy(p);
	TheTest->Printf(TheBuf16);
	}

extern "C" void TestHeapMark(void)
	{
	__UHEAP_MARK;
	}

extern "C" void TestHeapMarkEnd(void)
	{
	__UHEAP_MARKEND;
	}

extern "C" void Print(const char* msg)
	{
	TPtrC8 p((const unsigned char*)msg);
	TheBuf16.Copy(p);
	RDebug::Print(TheBuf16);
	}

extern "C" void PrintI(const char* fmt, int a1)
	{
	TPtrC8 p((const unsigned char*)fmt);
	TheBuf16.Copy(p);
	RDebug::Print(TheBuf16, a1);
	}

extern "C" void PrintIII(const char* fmt, int a1, int a2, int a3)
	{
	TPtrC8 p((const unsigned char*)fmt);
	TheBuf16.Copy(p);
	RDebug::Print(TheBuf16, a1, a2, a3);
	}

extern "C" void PrintII64I64(const char* fmt, int a1, __int64 a2, __int64 a3)
	{
	TPtrC8 p((const unsigned char*)fmt);
	TheBuf16.Copy(p);
	RDebug::Print(TheBuf16, a1, a2, a3);
	}

extern "C" void PrintSI(const char* fmt, const char* s, int a1)
	{
	TPtrC8 p((const unsigned char*)fmt);
	TheBuf8.Format(TPtrC8((const TUint8*)fmt), s, a1);
	TheBuf16.Copy(TheBuf8);
	RDebug::Print(TheBuf16);
	}

extern "C" void PrintS(const char* fmt, const char* s)
	{
	TPtrC8 p((const unsigned char*)fmt);
	TheBuf8.Format(TPtrC8((const TUint8*)fmt), s);
	TheBuf16.Copy(TheBuf8);
	RDebug::Print(TheBuf16);
	}

extern "C" void CreatePrivateDir(void)
	{
	RFs TheFs;
	TInt err = TheFs.Connect();
	TheTest->operator()(err == KErrNone);
	err = TheFs.CreatePrivatePath(EDriveC);
	TheFs.Close();
	TheTest->operator()(err == KErrNone || err == KErrAlreadyExists);
	}
	
