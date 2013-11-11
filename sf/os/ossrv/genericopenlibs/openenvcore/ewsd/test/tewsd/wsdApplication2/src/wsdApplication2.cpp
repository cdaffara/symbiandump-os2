// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : wsdApplication2.cpp
// Part of     : wsdApplication2 EXE
// Contains the code for the wsdApplication2 EXE
//



#include <e32test.h>
#include <e32debug.h>
#include <stdlib.h>
#include <signgam.h>
#include <math.h>
#include <dlfcn.h>
#include "commonFramework.h"
#include "commonDefs.h"
#include "wsdDll1.h"
#include "wsdDll2.h"

static RTest test(_L("wsdApplication2"));

_LIT(KStartingWsdApp2,"\n Starting process %d");
_LIT(KLibcTest,"\n Passed libc test: Random number = %d");
_LIT(KLibmTest,"\n Passed libm test: The sign of the gamma function of 10 is %d");
_LIT(KLibdlTest,"\n Passed libdl test: Successfully opened and closed libc.dll");
_LIT(KDll1GetInt1Test,"\n Passed Dll1 test: Initial int value is %d");
_LIT(KDll1GetInt2Test,"\n Passed Dll1 test: New int value is %d");
_LIT(KDll1GetBuf1Test,"\n Passed Dll1 test: Initial string value is ");
_LIT(KDll1GetBuf2Test,"\n Passed Dll1 test: New string value is ");
_LIT(KDll2GetInt1Test,"\n Passed Dll2 test: Initial int value is %d");
_LIT(KDll2GetInt2Test,"\n Passed Dll2 test: New int value is %d");
_LIT(KDll2GetBuf1Test,"\n Passed Dll2 test: Initial string value is ");
_LIT(KDll2GetBuf2Test,"\n Passed Dll2 test: New string value is ");
_LIT(KLibcTestFinished,"SUCCESS%d: Libc test complete");
_LIT(KLibmTestFinished,"SUCCESS%d: Libm test complete");
_LIT(KLibdlTestFinished,"SUCCESS%d: Libdl test complete");
_LIT(KDll1TestFinished,"SUCCESS%d: Dll1 test complete");
_LIT(KDll2TestFinished,"SUCCESS%d: Dll2 test complete");  
  
// Execution function for wsdApplication2.exe
LOCAL_C void doExampleL()
    {
    TUint64 procId = RProcess().Id().Id();
   	console->Printf(KStartingWsdApp2, procId);
	
	// 1. Call a libc API that accesses libc WSD
	// (note that this initialises the backend WSD too)
	int r1 = rand();
	int r2 = rand();

	console->Printf(KLibcTest, r1);	
	console->Printf(KLibcTest, r2);	
	RDebug::Print(KLibcTestFinished, procId);	
	// After each step we suspend the current thread (so in effect, 
	// this process) for 0.5 secs - this causes the calls to the WSD
	// array to be intertwined from different processes (see epocwind.out)
	User::After(500000); // 0.5 sec wait
	
	// 2. Call a libm API that accesses libm WSD
	int* p = __signgam();
	TInt number = *p;
	test(0 == number);
	double arg = 10;
	double result = lgamma(arg);
	p = __signgam();
	number = *p;
	test(1 == number);
	
	console->Printf(KLibmTest, number);
	RDebug::Print(KLibmTestFinished, procId);	
	User::After(500000); // 0.5 sec wait
	
	// 3. Call a libdl API that accesses libdl WSD
	void* dlHandle = dlopen("libc.dll", RTLD_NOW | RTLD_GLOBAL);
	test(dlHandle != NULL);
	int err = dlclose(dlHandle);
	test(err == 0);

	console->Printf(KLibdlTest);
	RDebug::Print(KLibdlTestFinished, procId);
	User::After(500000); // 0.5 sec wait

	// 4. Call Dll1 APIs that accesses Dll1 WSD
	
	// Check the initial WSD values are as expected
	TInt dll1Int = GetDll1Int();
	test(KDll1InitialIntValue == dll1Int);
	console->Printf(KDll1GetInt1Test, dll1Int);	
	TBuf<10> dll1Buf;
	GetDll1Buf(dll1Buf);
	test(!dll1Buf.Compare(KDll1InitialBufValue));
	console->Printf(KDll1GetBuf1Test);
	console->Printf(dll1Buf);	
	
	// Set the WSD values to different values
	SetDll1Int(KDll1NewIntValue);
	dll1Buf.Copy(KDll1NewBufValue);
	SetDll1Buf(dll1Buf);
	
	// Check the new WSD values have been stored
	dll1Int = GetDll1Int();
	test(KDll1NewIntValue == dll1Int);
	console->Printf(KDll1GetInt2Test, dll1Int);	
	GetDll1Buf(dll1Buf);
	test(!dll1Buf.Compare(KDll1NewBufValue));
	console->Printf(KDll1GetBuf2Test);
	console->Printf(dll1Buf);		
		
	RDebug::Print(KDll1TestFinished, procId);
	User::After(500000); // 0.5 sec wait

	// 5. Call Dll2 APIs that accesses Dll2 WSD	
	
	// Check the initial WSD values are as expected
	TInt dll2Int = GetDll2Int();
	test(KDll2InitialIntValue == dll2Int);
	console->Printf(KDll2GetInt1Test, dll2Int);	
	TBuf<10> dll2Buf;
	GetDll2Buf(dll2Buf);
	test(!dll2Buf.Compare(KDll2InitialBufValue));
	console->Printf(KDll2GetBuf1Test);
	console->Printf(dll2Buf);
	
	// Set the WSD values to different values
	SetDll2Int(KDll2NewIntValue);
	dll2Buf.Copy(KDll2NewBufValue);
	SetDll2Buf(dll2Buf);
	
	// Check the new WSD values have been stored
	dll2Int = GetDll2Int();
	test(KDll2NewIntValue == dll2Int);
	console->Printf(KDll2GetInt2Test, dll2Int);	
	GetDll2Buf(dll2Buf);
	test(!dll2Buf.Compare(KDll2NewBufValue));	
	console->Printf(KDll2GetBuf2Test);
	console->Printf(dll2Buf);	

	RDebug::Print(KDll2TestFinished, procId);	
	}
