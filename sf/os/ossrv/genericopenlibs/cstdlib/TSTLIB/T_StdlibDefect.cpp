// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <string.h>
#include <e32test.h> //this includes e32cmn.h
#include <e32debug.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <libc/netinet/in.h>
#include <libc/arpa/inet.h>
#include <limits.h>

#include <sys/errno.h>
#include <stdlib.h>
#include <math.h>

LOCAL_D RTest				TheTest (_L("T_StdlibDefect"));
//
//
//Test macroses and functions

static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__) 



const TInt KTestDataLen = 10000;
const TInt KTestIterations = 1000;

#ifdef __ARMCC__
const TInt KMinCharARMCC = 0;
const TInt KMaxCharARMCC = 255;
#else
const TInt KMinCharNoARMCC = -128;
const TInt KMaxCharNoARMCC = 127;
#endif

/**
DEF062679  : memcpy in stdlib is slow
*/ 
void Defect_DEF062679_memcpy()
	{

	TUint8* src1 = new TUint8[KTestDataLen];
	TEST(src1 != NULL);
	Mem::Fill(src1, KTestDataLen, 'A');
	
	TUint8* dst1 = new TUint8[KTestDataLen];
	TEST(dst1 != NULL);

	TTime startTime, stopTime;
	TInt i;
	//Loop to check time spent using Mem::Copy
	startTime.UniversalTime();
	for(i=0; i<KTestIterations ; i++)
		Mem::Copy(dst1, src1, KTestDataLen );
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
	TheTest.Printf(_L("Time taken using Mem::Copy :%d microseconds\n"), timeTaken.Int64() );
	
	//Loop to check the time spent using memcpy after the change
	startTime.UniversalTime();
	for(i=0; i<KTestIterations ; i++)
		memcpy(dst1, src1, KTestDataLen );
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken2 = stopTime.MicroSecondsFrom(startTime);
	TheTest.Printf(_L("Time taken using  memcpy: %d microseconds\n"), timeTaken2.Int64() );
	TheTest.Printf(_L("Time taken using  memcpy before the change about 613125 microseconds\n"));

	//Test memcpy works fine
	for(i=0; i<KTestIterations ; i++)
		TEST(dst1[i] == src1[i]);

	delete [] src1;
	delete [] dst1;
	
	}
	
	
	
	
/**
DEF062679  : memcpy in stdlib is slow
*/ 
void Defect_DEF062679_memcmp()
	{

	TUint8* str1 = new TUint8[KTestDataLen];
	TEST(str1 != NULL);
	Mem::Fill(str1, KTestDataLen, 'A');
	
	TUint8* str2 = new TUint8[KTestDataLen];
	TEST(str2 != NULL);
	Mem::Fill(str2, KTestDataLen, 'A');
	
	TTime startTime, stopTime;
	
	//Loop to check the time using Mem::Copy
	startTime.UniversalTime();
	TInt i,ret;
	for(i=0; i<KTestIterations ; i++)
		ret = Mem::Compare(str2,KTestDataLen, str1, KTestDataLen );
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
	TheTest.Printf(_L("Time taken using Mem::Compare :%ld microseconds\n"), timeTaken.Int64() );
	
	//Loop to check the time spent using memcpy after the change
	startTime.UniversalTime();
	
	for(i=0; i<KTestIterations ; i++)
		ret = memcmp(str2,str1, KTestDataLen );
	TEST(ret==0); //check that memcmp works fine	
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken2 = stopTime.MicroSecondsFrom(startTime);
	TheTest.Printf(_L("Time taken using  memcmp: %ld microseconds\n"), timeTaken2.Int64() );
	
	TheTest.Printf(_L("Time taken using  memcmp before changes 1007000 microseconds\n"));
	

	//Test memcmp works fine
	
	TUint8* str3 = new TUint8[KTestDataLen];
	TEST(str3 != NULL);
	Mem::Fill(str3, KTestDataLen, 'B');
	ret = memcmp(str3, str1, KTestDataLen);
	TEST(ret>0);
	ret = memcmp(str1, str3, KTestDataLen);
	TEST(ret<0);
	
	delete str1;
	delete str2;
	delete str3;
	}	

/**
INC073740: inet_addr and inet_aton returns wrong results with invalid input on STDLIB
*/ 
void Defect_INC073740()
	{
	TheTest.Next(_L("INC073740: inet_addr and inet_aton returns wrong results with invalid input on STDLIB"));
	
	int err;
	struct in_addr iaddr;	
	char* good_addr="16.33.50.67";
	char* bad_addr="256.33.50.67";
	char* worse_addr="16.1456.50.67";
	char* worst_addr="16.33.333333.67";
	
	err=inet_aton(good_addr, &iaddr);
	TEST2(err, 1);

	err=inet_aton(bad_addr, &iaddr);
	TEST2(err, 0);

	err=inet_aton(worse_addr, &iaddr);
	TEST2(err, 0);

	err=inet_aton(worst_addr, &iaddr);
	TEST2(err, 0);
	}

/**
@SYMTestCaseID         	SYSLIB-STDLIB-CT-1863
@SYMTestCaseDesc        Tests for minimum and maximum values type "char" with ARMCC macro
@SYMTestPriority        High
@SYMTestActions         Tests for checking minimum and maximum values for a variable of type "char"
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 PDEF091928
*/ 
void Defect_PDEF091928()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STDLIB-CT-1863 PDEF091928: limits.h not correct for ARM RVCT compiler "));
	
	char charmn=CHAR_MIN;
	char charmx=CHAR_MAX;
	
#ifdef __ARMCC__
	
	TEST2(charmn, KMinCharARMCC);
	TEST2(charmx, KMaxCharARMCC);
	
#else

	TEST2(charmn, KMinCharNoARMCC);
	TEST2(charmx, KMaxCharNoARMCC);

#endif
	}

/**
@SYMTestCaseID         	SYSLIB-STDLIB-UT-3612
@SYMTestCaseDesc        Tests for __errno().
@SYMTestPriority        Normal
@SYMTestActions         Tests for __errno(). Test whether it is correctly exported and functioning 
						as expected after being declared with IMPORT_C. 
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF110593
*/ 
void Defect_DEF110593()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STDLIB-UT-3612 DEF110593: IMPORT_C/EXPORT_C: ERRNO.H "));

	// randomly selected 24 macros from errno.h
	const int errMacros[24] =
		{
		EPERM,		ENOENT,		ESRCH,		EINTR,		EIO,
		EBADF,		ENOMEM,		EFAULT,		EBUSY,		ENOTDIR,
		ENFILE,		ESPIPE,		ERANGE,		ENOMSG,		EDEADLK,
		ENOLCK,		ENOTSOCK,	ENOLINK,	EBADMSG,	ENOTUNIQ,
		EBADFD,		ENOSYS,		EILSEQ, 	__ELASTERROR
		};
	
	// Step 1: setting errno using assign "=", test value returned by errno
	errno = 0;
	TEST(errno == 0);
	
	int i;
	for (i = 0; i < 24; i++)
		{
		errno = errMacros[i];
		TEST(errno == errMacros[i]);
		}
	
	// Step 2: setting errno using the library globals struct, test value returned by errno
	struct _reent *r = _REENT2;
	r->_errno = 0;
	TEST(errno == 0);
	
	for (i = 0; i < 24; i++)
		{
		r->_errno = errMacros[i];
		TEST(errno == errMacros[i]);
		}
	
	r->_errno = 0;
	TEST(errno == 0);
	
	// Step3: Test errno by using other C function in STDLIB
	// Test using ldexp(double value, int exp);
	// Giving val a huge number to make res overflow. errno should return ERANGE
	double val = 1.5E+308;
	int exp = 10;
	ldexp(val, exp);
	TEST(errno == ERANGE);
		
	//finish
	CloseSTDLIB();	
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-4001
@SYMTestCaseDesc        Test strtoul() with a string whose first character is '-' or '+'. 
@SYMTestPriority        3. Medium
@SYMTestActions         Test strtoul() with a string whose first character is '-' or '+'. 
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 PDEF114447 
*/ 
void Defect_PDEF114447 ()
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STDLIB-CT-4001 PDEF114447 : add the processing for '-' and '+' in a string passed to strtoul() "));
    
    unsigned long result;
    
    result = strtoul("+80", NULL, 10);    
    TEST2(result, 80);

    result = strtoul("-80", NULL, 10);    
    TEST2(result, -80);    
    }

/**
Calls ImpurePtr2() to allocate memory for the library globals struct then calls CloseSTDLIB() 
to release it.
Leaves if system-wide error occurs.
*/
LOCAL_C void TestImpurePtrL()
{
	struct _reent * p = ImpurePtr2();
	User::LeaveIfNull(p);
	CloseSTDLIB();
}

/**
@SYMTestCaseID         	SYSLIB-STDLIB-UT-4002
@SYMTestCaseDesc        Test checks the constructor of CLocalSystemInterface does not panics in OOM test
						or when error KErrNoMemory occurs.
@SYMTestPriority        Normal
@SYMTestActions         In an OOM test, repeats calling ImpurePtr2(), which creates CLocalSystemInterface instance. 
@SYMTestExpectedResults The test program should not panic or fail.
@SYMDEF                 DEF114383
*/ 
LOCAL_C void Defect_DEF114383()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STDLIB-UT-4002 DEF114383: STDLIB, CLocalSystemInterface::CLocalSystemInterface() panics in OOM "));		
	
	TInt err=KErrNone;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);
		TRAP(err, TestImpurePtrL());
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		
		if (err!=KErrNoMemory)
			TEST(err == KErrNone);
		
		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);
		
	TEST(err == KErrNone);
		
	TheTest.Printf(_L("- ImpurePtr2() succeeded at heap failure rate of %i\n"), tryCount);
	} 

/**
Invoke the tests
*/
LOCAL_C void RunTestsL ()
    {
	Defect_DEF062679_memcpy();
	Defect_DEF062679_memcmp();

	Defect_INC073740();
	
	Defect_PDEF091928();
	
	Defect_DEF110593(); 
	
	Defect_PDEF114447();

	Defect_DEF114383();
	}



GLDEF_C TInt E32Main()
 {		
 		
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	__UHEAP_MARK;

	TheTest.Title();
	TheTest.Start (_L("Defect Tests"));
	TInt err;
	TRAP(err, ::RunTestsL())
	TEST2(err, KErrNone);

	TheTest.End();
	TheTest.Close();

	delete tc;
	__UHEAP_MARKEND;
	
	
	
	return(KErrNone);
	
	}
	
