// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This test has been converted from it original TMISC.C .
// The original was a manaul test and didn't work!
// Test code for bsearch, qsort etc. These are all functions which work directly
// without using the MSystemInterface - as such none of them use errno to indicate
// errors.
// 
//

#include <e32test.h>
#include <e32svr.h>

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>  /* HUGE_VAL */
#include <errno.h> /* ERANGE */
#include "ESTLIB.H"

// Defined in ctest.h but contains lots of other declarations we don't want
#define test_Data		struct __testdata __td	/* declares global variable __td */

//
// Globals

static RTest TheTest(_L("TMisc"));

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

//
// Tests


/* random numbers */

void random_numbers()
	{
	int i,r;
	int rs[20];

	TheTest.Next(_L("Random numbers"));

	for (i=0; i<20; i++)
		{
		r = rand();
		printf("%d ", r);
		}
	printf("\n");
	TheTest.Next(_L("Using srand()"));
	srand(12345678);
	for (i=0; i<20; i++)
		{
		rs[i]=rand();
		printf("%d ",rs[i]);
		}
	printf("\n");
	srand(12345678);
	for (i=0; i<20; i++)
		TEST(rand()==rs[i]);
	}

/* Sorting */

typedef struct {
	int a;
	int b;
	int c;
	} sort_item;

int compare_a (const void *left, const void *right)
	{
	sort_item *lp = (sort_item *)left;
	sort_item *rp = (sort_item *)right;

	if (lp->a < rp->a) return -1;
	if (lp->a > rp->a) return  1;
	return 0;
	}

int compare_ba (const void *left, const void *right)
	{
	sort_item *lp = (sort_item *)left;
	sort_item *rp = (sort_item *)right;

	if (lp->b < rp->b) return -1;
	if (lp->b > rp->b) return  1;
	/* b's are equal, sort on a's */
	if (lp->a < rp->a) return -1;
	if (lp->a > rp->a) return  1;
	return 0;
	}

#define NITEMS 200
sort_item list[NITEMS];

void validate(sort_item *p)
	{
	int i;
	for (i=0; i<NITEMS; i++,p++)
		{
		if (p->a==9)
			continue;
		TEST((p->a/4)+p->b == 5000);
		TEST((p->a*p->a)/4 == p->c);
		}
	}

void sorting()
	{
	int i;
	TheTest.Next(_L("Quicksort"));
	for (i=1; i<NITEMS; i++)
		{
		list[i].a = 2*i;
		list[i].b = 5000-(i/2);
		list[i].c = i*i;
		}

	/* Sort into increasing order of a. Want item 0 to sort to position 4 */
	list[0].a = 9;
	list[0].b = 10;
	list[0].c = 11;

	qsort(list, NITEMS, sizeof(list[0]), compare_a);
	TEST(list[4].c==11);
	validate(list);

	/* Resort into increasing order of b followed by a. Want item 0 to sort to position 5 */
	list[0].a = 9;
	list[0].b = 5000-((199-4)/2);
	list[0].c = 13;

	qsort(list, NITEMS, sizeof(list[0]), compare_ba);
	TEST(list[5].c==13);
	validate(list);
	}

void searching()
	{
	int i;
	sort_item *answer;
	sort_item not_there;
	TheTest.Next(_L("Binary search for existing items"));
	for (i=0; i<NITEMS; i++)
		{
		answer = (sort_item*)bsearch(&list[i],list,NITEMS,sizeof(list[0]),compare_ba);
		TEST(answer==&list[i]);
		}
	TheTest.Next(_L("Binary search for missing items"));
	for (i=0; i<NITEMS; i++)
		{
		not_there = list[i];
		not_there.a++;
		answer = (sort_item*)bsearch(&not_there,list,NITEMS,sizeof(list[0]),compare_ba);
		if (answer!=NULL)
			TEST(!compare_ba(answer,&not_there));
		}
	for (i=0; i<NITEMS; i++)
		{
		not_there = list[i];
		not_there.a--;
		answer = (sort_item*)bsearch(&not_there,list,NITEMS,sizeof(list[0]),compare_ba);
		if (answer!=NULL)
			TEST(!compare_ba(answer,&not_there));
		}
	}

void sscanf_test()
	{
	int in[4], i;
	TheTest.Next(_L("Simple SSCANF tests"));

	in[0] = in[1] = in[2] = in[3] = 6789;
	i = sscanf("1.2.3.4", "%d.%d.%d.%d", &in[0], &in[1], &in[2], &in[3]);
	TEST (in[0]==1);
	TEST (in[1]==2);
	TEST (in[2]==3);
	TEST (in[3]==4);
	TEST (i==4);

	i = sscanf("194.223.254.9", "%d.%d.%d.%d", &in[0], &in[1], &in[2], &in[3]);
	TEST (in[0]==194);
	TEST (in[1]==223);
	TEST (in[2]==254);
	TEST (in[3]==9);
	TEST (i==4);

	i = sscanf("17.183.hello.11", "%d.%d.%d.%d", &in[0], &in[1], &in[2], &in[3]);
	TEST (in[0]==17);
	TEST (in[1]==183);
	TEST (in[2]==254);
	TEST (in[3]==9);
	TEST (i==2);
	}

/* setjmp, longjmp */

#include <setjmp.h>
#include <string.h>

jmp_buf jbufs[4];
int count=0;

static int getStackPointer(void)
	{
	static char there;
	char here;
	return &here-&there;
	}

static int getgetStackPointer(void)
	{
	int n=4;	// local variable to cause change to stack pointer
	n+=getStackPointer();
	if (n==0)
		n=getStackPointer()+37;	// this never happens - it's just to defeat the optimiser
	return (n-4);
	}

void jmp_nest(int n)
	{
	int m=n+100;	// local variable to induce a stack frame
	if (n>0)
		jmp_nest(n-1);
	longjmp(jbufs[3],m);
	}

#ifdef __VC32__
#pragma optimize( "", off )		//stop the compiler breaking this TEST
#endif

void setjmp_longjmp()
	{
	volatile int i,r,j,k,l,m; // volatile to rid us of warnings
	volatile int sp1,sp2;
	TheTest.Next(_L("Setjmp"));

	sp1=getStackPointer();
	sp2=getgetStackPointer();
	TEST(sp1!=sp2);	// call from deeper function nesting should give a different answer

	memset(jbufs[0],0,sizeof(jmp_buf));
	memset(jbufs[1],0,sizeof(jmp_buf));
	memset(jbufs[2],0,sizeof(jmp_buf));

	r=setjmp(jbufs[0]);
	TEST(r==0);

	for (i=0,j=1,k=2,l=3,m=4; i<3; i++,j*=3,k+=j,l*=m,m-=2)
		{
		r=setjmp(jbufs[i]);
		TEST(r==0);
		TEST(j>i);
		}
	r=memcmp(jbufs[0],jbufs[2], sizeof(jmp_buf));

	if (r!=0)
		{
		RDebug::Print(_L("  Test code appears to be using preserved registers (a good thing)\n"));
		RDebug::Print(_L("  buf @   %08x %08x\n"), jbufs[0], jbufs[2]);
		for (i=0;i<16;i++)
			{
			if (jbufs[0][i] != jbufs[2][i])
				RDebug::Print(_L("  buf+%02d: %08x %08x\n"), i*4, jbufs[0][i], jbufs[2][i]);
			}
		}
	else
		RDebug::Print(_L("  Test code appears not to exercise preserved registers\n"));

	r=setjmp(jbufs[0]);
	TEST(r==0);
	r=setjmp(jbufs[2]);
	TEST(r==0);
	r=memcmp(jbufs[0],jbufs[2], sizeof(jmp_buf));
	TEST(r!=0);	/* must change the return address! */

	TheTest.Next(_L("Setjmp and longjmp"));

	r=setjmp(jbufs[0]);
	if (r==0)
		{
		TEST(count==0);
		count++;
		longjmp(jbufs[0],7);
		}
	else if (r==7)
		{
		TEST(count==1);
		count++;
		longjmp(jbufs[0],3);
		}
	else if (r==3)
		{
		TEST(count==2);
		count++;
		longjmp(jbufs[0],0);	/* 0 must be turned into 1 */
		}
	else
		{
		TEST(r==1);
		TEST(count==3);
		count++;
		}

	sp1=getStackPointer();
	r=setjmp(jbufs[3]);
	if (r==0)
		{
		sp2=getStackPointer();
		TEST(sp1==sp2);
		longjmp(jbufs[3],1);		// simple setjmp/longjmp
		}
	else if (r==1)
		{
		sp2=getStackPointer();
		TEST(sp1==sp2);
		jmp_nest(20);			// more complex example
		}
	else
		{
		TEST(r==100);
		sp2=getStackPointer();
		TEST(sp1==sp2);
		}
	}

#ifdef __VC32__
#pragma optimize( "", on )	//stop the compiler breaking this TEST
#endif

/* case-insensitive comparison */

#include <string.h>
char agrave[2]= {191,0};

void casecmp()
	{
	int r;
	char *s1,*s2;
	TheTest.Next(_L("Case-insensitive string comparison"));

	s1="abcd";
	r=strcasecmp(s1,s1);
	TEST(r==0);

	s2="abcde";
	r=strcasecmp(s1,s2);
	TEST(r<0);

	r=strcasecmp(s2,s1);
	TEST(r>0);

	r=strncasecmp(s1,s2,10);
	TEST(r<0);

	r=strncasecmp(s1,s2,4);
	TEST(r==0);

	s2="ABcD";
	r=strcmp(s1,s2);
	TEST(r!=0);
	r=strcasecmp(s1,s2);
	TEST(r==0);

#if 0
	/* Need some way to set up a proper folding example */
	r=strncasecmp(s2,agrave,1);
	TEST(r==0);
#endif
	}

void arguments(int argc, char *argv[])
	{
	int i;

	TheTest.Next(_L("Command line arguments"));
	TEST(argc>0);
	TEST(argv!=0);
	printf("  argc=%d\r\n", argc);
	for (i=0; i<argc; i++)
		{
		int j;
		int length=strlen(argv[i]);
		TEST(argv[i]!=0);
		printf("  argv[%d]= ", i);
		for (j=0;j<4;j++)
			{
			printf("%02x ", ((unsigned char *)argv[i])[j]);
			if (argv[i][j]=='\0')
				break;
			}
		for (;j<3;j++)
			printf("   ");
		printf(" \"%s\" length %d\r\n", argv[i], length);
		}
	}

struct double_conv {
	char* source;
	double answer;
	int tail_offset;
	};

struct double_conv strtods[] = {
	{ "1",		1.0,	1 },
	{ "0.1",	0.1,	3 },
	{ ".1",		0.1,	2 },
	{ " 0.1",	0.1,	4 },
	{ " 1 ",	1.0,	2 },
	{ "1x",		1.0,	1 },
	{ "x",		0.0,	0 },
	{ "1.5.3",	1.5,	3 },
	{ "1.0e-1m solid red;}...",	0.1,	6 },
	{ "1.0e2blah",	100.0,	5 },
	{ "3.1415e2blah",	314.15,	8 },
	{ "0.2em solid red;}...",	0.2,	3 },
	{ "0.2e5m solid red;}...",	20000.0,	5 },
	{ 0 }
	};

void strtod_test()
	{
	double d;
	char* p;
	struct double_conv *dc;

	TheTest.Next(_L("text to double conversion"));
	for (dc=strtods; dc->source; dc++)
		{
		d=strtod(dc->source,NULL);
		TEST(d==dc->answer);
		}
	for (dc=strtods; dc->source; dc++)
		{
		p=(char*)1;
		d=strtod(dc->source,&p);
		TEST(d==dc->answer);
		TEST(p==dc->source+dc->tail_offset);
		}

	/* overflow positive number */
	d=strtod("9e9999", NULL);
	TEST(d==HUGE_VAL && errno==ERANGE);

	/* overflow negative number */
	d=strtod("-9e9999", NULL);
	TEST(d==-HUGE_VAL && errno==ERANGE);

	/* underflow number */
	d=strtod("9e-9999", NULL);
	TEST(d==0 && errno==ERANGE);
	d=strtod("-9e-9999", NULL);
	TEST(d==0 && errno==ERANGE);
}

#define TEST_8	0x88
#define TEST_16	0x1617
#define TEST_32	0x32333435

#define TEST_LIST \
	TEST_8,  TEST_16, TEST_32, \
	TEST_8,  TEST_32, TEST_16, \
	TEST_16, TEST_8,  TEST_32, \
	TEST_16, TEST_32, TEST_8,  \
	TEST_32, TEST_8,  TEST_16, \
	TEST_32, TEST_16, TEST_8

/*
   Suppressing RVCT compiler warning (for char/short/long types):
   Warning:  #1256-D: "unsigned char" would have been promoted to "int" when passed through the ellipsis parameter;
   use the latter type instead.
          b = va_arg(ap, unsigned char);
              ^
*/

#pragma diag_suppress 1256

// The above RCVT warning is a compiler error in GCC. Below is the GGC friendly version of the function.
#ifdef __GCC32__

void va_va_bwlblwwblwlblbwlwb(va_list ap)
	{
	unsigned long l;
	unsigned short w;
	unsigned char b;

	l=0; w=0; b=0;
	b = va_arg(ap, int);
	TEST(b==TEST_8);
	w = va_arg(ap, int);
	TEST(w==TEST_16);
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);

	l=0; w=0; b=0;
	b = va_arg(ap, int);
	TEST(b==TEST_8);
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);
	w = va_arg(ap, int);
	TEST(w==TEST_16);

	l=0; w=0; b=0;
	w = va_arg(ap, int);
	TEST(w==TEST_16);
	b = va_arg(ap, int);
	TEST(b==TEST_8);
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);

	l=0; w=0; b=0;
	w = va_arg(ap,int);
	TEST(w==TEST_16);
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);
	b = va_arg(ap, int);
	TEST(b==TEST_8);

	l=0; w=0; b=0;
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);
	b = va_arg(ap, int);
	TEST(b==TEST_8);
	w = va_arg(ap, int);
	TEST(w==TEST_16);

	l=0; w=0; b=0;
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);
	w = va_arg(ap, int);
	TEST(w==TEST_16);
	b = va_arg(ap, int);
	TEST(b==TEST_8);
	}


#else

void va_va_bwlblwwblwlblbwlwb(va_list ap)
	{
	unsigned long l;
	unsigned short w;
	unsigned char b;

	l=0; w=0; b=0;
	b = va_arg(ap, unsigned char);
	TEST(b==TEST_8);
	w = va_arg(ap, unsigned short);
	TEST(w==TEST_16);
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);

	l=0; w=0; b=0;
	b = va_arg(ap, unsigned char);
	TEST(b==TEST_8);
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);
	w = va_arg(ap, unsigned short);
	TEST(w==TEST_16);

	l=0; w=0; b=0;
	w = va_arg(ap, unsigned short);
	TEST(w==TEST_16);
	b = va_arg(ap, unsigned char);
	TEST(b==TEST_8);
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);

	l=0; w=0; b=0;
	w = va_arg(ap, unsigned short);
	TEST(w==TEST_16);
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);
	b = va_arg(ap, unsigned char);
	TEST(b==TEST_8);

	l=0; w=0; b=0;
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);
	b = va_arg(ap, unsigned char);
	TEST(b==TEST_8);
	w = va_arg(ap, unsigned short);
	TEST(w==TEST_16);

	l=0; w=0; b=0;
	l = va_arg(ap, unsigned long);
	TEST(l==TEST_32);
	w = va_arg(ap, unsigned short);
	TEST(w==TEST_16);
	b = va_arg(ap, unsigned char);
	TEST(b==TEST_8);
	}

#endif


#pragma diag_default 1256

void va_lbwlblwwblwlblbwlwb(unsigned long x, ...)
	{
	va_list ap;
	TEST(x==TEST_32);
	va_start(ap, x);

	va_va_bwlblwwblwlblbwlwb(ap);
	}

void va_args_test()
	{
	TheTest.Next(_L("variadic functions"));

	va_lbwlblwwblwlblbwlwb( TEST_32, TEST_LIST);
	}

void sprintf_test()
	{
	char buf[256];
	char* hw = "hello, world";

	TheTest.Next(_L("sprintf function"));

	/* WAP TOG Defect */
    sprintf(buf, "%.*f", 0, 10.1234);
	TEST(strcmp(buf, "10")==0);
	sprintf(buf, "%.0f", 10.1234);
	TEST(strcmp(buf, "10")==0);

	/* From K&R */
	sprintf(buf, ":%s:", hw);
	TEST(strcmp(buf, ":hello, world:")==0);

	sprintf(buf, ":%10s:", hw);
	TEST(strcmp(buf, ":hello, world:")==0);

	sprintf(buf, ":%.10s:", hw);
	TEST(strcmp(buf, ":hello, wor:")==0);

	sprintf(buf, ":%-10s:", hw);
	TEST(strcmp(buf, ":hello, world:")==0);

	sprintf(buf, ":%.15s:", hw);
	TEST(strcmp(buf, ":hello, world:")==0);

	sprintf(buf, ":%-15s:", hw);
	TEST(strcmp(buf, ":hello, world   :")==0);

	sprintf(buf, ":%15.10s:", hw);
	TEST(strcmp(buf, ":     hello, wor:")==0);

	sprintf(buf, ":%-15.10s:", hw);
	TEST(strcmp(buf, ":hello, wor     :")==0);
	}


static void MainL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-STDLIB-LEGACY-TMISC-0001 TMISC tests "));

	random_numbers();
	sorting();
	searching();
	setjmp_longjmp();
	casecmp();
	sscanf_test();

	int argsc = 5;
	char *argsv[]=
		{
		"tmisc", "This", "is", "a", "test.",
		};
	arguments(argsc, argsv);

	strtod_test();
	va_args_test();
	sprintf_test();
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
	CloseSTDLIB();

	__UHEAP_MARKEND;

	User::Heap().Check();
	return KErrNone;
	}
