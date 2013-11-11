// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Simple STDLIB tests.
// 
//

#include <e32test.h>
#include <e32svr.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include "ESTLIB.H"

//CPP file is used for C tests, because by default any console opened from a C file
//expects a key to be pressed when it is about to be closed. That makes impossible
//the creation of automated C tests.

//
// Globals

static RTest TheTest(_L("TMisc3"));
const double KPi = 3.14159265358979323846;

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

#define TEST_NAN (0.0/0.0)
#define TEST_POS_INFINITY (1.0/0.0)
#define TEST_NEG_INFINITY (-1.0/0.0)

//
// Tests

static void TestCType()
	{
	char c;
	for(c='a';c<='z';++c)
		{
		TEST(isalnum(c));
		TEST(isalpha(c));
		TEST(!iscntrl(c));
		TEST(!isdigit(c));
		TEST(isgraph(c));
		TEST(islower(c));
		TEST(isprint(c));
		TEST(!ispunct(c));
		TEST(!isspace(c));
		TEST(!isupper(c));
		TEST(c >= 'a' && c <= 'f' ? isxdigit(c) : !isxdigit(c));
		}
	for(c='A';c<='Z';++c)
		{
		TEST(isalnum(c));
		TEST(isalpha(c));
		TEST(!iscntrl(c));
		TEST(!isdigit(c));
		TEST(isgraph(c));
		TEST(!islower(c));
		TEST(isprint(c));
		TEST(!ispunct(c));
		TEST(!isspace(c));
		TEST(isupper(c));
		TEST(c >= 'A' && c <= 'F' ? isxdigit(c) : !isxdigit(c));
		}
	for(c='0';c<='9';++c)
		{
		TEST(isalnum(c));
		TEST(!isalpha(c));
		TEST(!iscntrl(c));
		TEST(isdigit(c));
		TEST(isgraph(c));
		TEST(!islower(c));
		TEST(isprint(c));
		TEST(!ispunct(c));
		TEST(!isspace(c));
		TEST(!isupper(c));
		TEST(isxdigit(c));
		}
	for(c=0;c<' ';++c)
		{
		TEST(!isalnum(c));
		TEST(!isalpha(c));
		TEST(iscntrl(c));
		TEST(!isdigit(c));
		TEST(!isgraph(c));
		TEST(!islower(c));
		TEST(!isprint(c));
		TEST(!ispunct(c));
		if(c != '\t' && c != '\n' && c != '\r' && c != '\v' && c != '\f')
			{
			TEST(!isspace(c));
			}
		else
			{
			TEST(isspace(c));
			}
		TEST(!isupper(c));
		}
	}

static void TestMath()
	{
	double eps = 0.000001;

	double res = fabs(21.091);
	TEST((21.091 - res) < eps);
	res = fabs(-1.91);
	TEST((res - 1.91) < eps);

	res = atan(0.0);
	TEST(fabs(res) < eps);
	// 90 degrees
	res = tan(KPi/2);
	TEST(res > 1000000000.0);
	res = atan(res);
	TEST(fabs(res - KPi/2) < eps);
	// 45 degrees
	res = tan(KPi/4);
	TEST(fabs(res - 1.0) < eps);
	res = atan(res);
	TEST(fabs(res - KPi/4) < eps);
	// 135 degrees
	res = tan((3 * KPi) / 4);
	TEST(fabs(res + 1.0) < eps);
	res = atan(res);
	TEST(fabs((KPi + res) - (3 * KPi) / 4) < eps);

	// 0 degrees
	res = cos(0.0);
	TEST(fabs(res - 1) < eps);
	// 90 degrees
	res = cos(KPi/2);
	TEST(res < eps);
	// 180 degrees
	res = cos(KPi);
	TEST(fabs(res + 1.0) < eps);

	// 0 degrees
	res = sin(0.0);
	TEST(res < eps);
	// 90 degrees
	res = sin(KPi/2);
	TEST(fabs(res - 1) < eps);
	// 180 degrees
	res = sin(KPi);
	TEST(res < eps);

	res = tanh(1.0);
	TEST(fabs(res - 0.761594) < eps);

	int exponent;
	res = frexp(0.51E+2, &exponent);
	TEST((0.51E+2 - res * pow(2.0, exponent)) < eps);

	double integer;
	res = modf(34.567, &integer);
	TEST(fabs(res - 0.567) < eps);
	TEST(fabs(integer - 34.0) < eps);
	res = modf(-35.567, &integer);
	TEST(fabs(res + 0.567) < eps);
	TEST(fabs(integer + 35.0) < eps);

	res = ceil(245.8903);
	TEST(fabs(res - 246.0) < eps);
	res = ceil(-11.91);
	TEST(fabs(res + 11.0) < eps);

	res = floor(245.8903);
	TEST(fabs(res - 245.0) < eps);
	res = floor(-11.91);
	TEST(fabs(res + 12.0) < eps);

	res = copysign(4.789, -9.001);
	TEST((res + 4.789) < eps);
	res = copysign(-4.789, 9.001);
	TEST((res - 4.789) < eps);

	}

static void TestDef113884()
	{
	int exponent;

	errno = 0;
	frexp(0.51E+2, &exponent);
	pow(2.0, exponent);
	TEST2(errno, 0);
	errno = 0;
	pow(-1.0, 0.5);
	TEST2(errno, EDOM);
	errno = 0;
	// Added 'LL' as GCC needs to know these are explicitly 'long long'
	pow(34523543434234LL , 23423432234LL);
	TEST2(errno, ERANGE);
	errno = 0;
	pow(-34523543434234LL , -23423432234LL);
	TEST2(errno, ERANGE);

	errno = 0;
	atan(0.0);
	TEST2(errno, 0);

	errno = 0;
	cos(0.0);
	TEST2(errno, 0);

	errno = 0;
	sin(0.5);
	TEST2(errno, 0);

	errno = 0;
	tan(KPi/2);
	TEST2(errno, 0);

	errno = 0;
	acos(0.5);
	TEST2(errno, 0);
	errno = 0;
	acos(HUGE_VAL);
	TEST2(errno, EDOM);

	errno = 0;
	asin(0.5);
	TEST2(errno, 0);

	errno = 0;
	exp(2);
	TEST2(errno, 0);
	errno = 0;
	exp(HUGE_VAL);
	TEST2(errno, ERANGE);
	errno = 0;
	exp(-1 * HUGE_VAL);
	TEST2(errno, ERANGE);

	errno = 0;
	log(2);
	TEST2(errno, 0);
	errno = 0;
	log(-1);
	TEST2(errno, EDOM);
	errno = 0;
	log(0);
	TEST2(errno, ERANGE);

	errno = 0;
	log10(2);
	TEST2(errno, 0);
	errno = 0;
	log10(-1);
	TEST2(errno, EDOM);
	errno = 0;
	log10(0);
	TEST2(errno, ERANGE);

	errno = 0;
	sqrt(2);
	TEST2(errno, 0);
	errno = 0;
	sqrt(-1);
	TEST2(errno, EDOM);
#ifdef __ARMCC__
#pragma diag_suppress 222  //armv5 Warning #222-D is an echo of the following test.  We want to ensure that the test reports the error as well.
#else
#pragma warning (disable: 222)  //winscw Warning #222-D is an echo of the following test.  We want to ensure that the test reports the error as well.
#endif
	errno = 0;
	sqrt(2.0 * HUGE_VAL);
	TEST2(errno, ERANGE);
#ifdef __ARMCC__
#pragma diag_default 222  //armv5 Warning #222-D turned back on.
#else
#pragma warning (default: 222)  //winscw Warning #222-D turned back on.
#endif

	errno = 0;
	atan2(2.0, 2.0);
	TEST2(errno, 0);
#pragma warning (disable: 222)  //Warning #222-D is an echo of the following test.
	errno = 0;
	atan2(0.0, 0.0);
	TEST2(errno, EDOM);
#pragma warning (default: 222)  //Warning #222-D is an echo of the following test.

	errno = 0;
	fmod(2.0, 2.0);
	TEST2(errno, 0);
	errno = 0;
	fmod(1.0, 0.0);
	TEST2(errno, EDOM);

	errno = 0;
	rint(2.129999);
	TEST2(errno, 0);

	}


//
//

static void MainL()
    {
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-STDLIB-LEGACY-TMISC3-0001 CTYPE tests "));
	::TestCType();

	TheTest.Next(_L("MATH tests"));
	::TestMath();

	TheTest.Next(_L("MATH DEF113884 tests"));
	::TestDef113884();

	CloseSTDLIB();
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* tc = CTrapCleanup::New();
    TEST(tc != NULL);

	TheTest.Title();
    TRAPD(err, ::MainL());
	TEST2(err, KErrNone);

	TheTest.End();
	TheTest.Close();

	delete tc;

	__UHEAP_MARKEND;

	User::Heap().Check();
	return KErrNone;
	}
