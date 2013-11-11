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
// Overview:
// Test methods of the LString16, LString8, LString template class.
// API Information:
// LString16, LString8, LString 
// Details :
// - Create some 16 bit modifiable descriptors, 8 bit modifiable descriptors
// of fixed length, Build-independent modifiable descriptors, initialize 
// with different strings and check for
// - Comparison operators,
// - Property access methods,
// - Fill & swap methods,
// - Conversion methods,
// - Comparison methods,
// - Pattern Matching methods,
// - Pattern Locating methods,
// - Copying methods, 
// - Find, FindC, FindF methods,
// - Repeat, Trim, TrimLeft, TrimRight, Insert, Delete, Left,
// Right, Mid methods,
// - Formatting methods,
// - Replace methods are as expected.
// - Construct some descriptors with buffer length, string and buffer reference and
// verify that they are created successfully.
// - Test assignment operators and comparison operators for different descriptors.
// - Initialize some descriptors and check descriptors' maximum length, length and 
// size are as expected. 
// - Check Fill and Swap methods are as expected.
// - Test Fold, Collate, LowerCase, UpperCase methods are as expected.
// - Test Comparison methods are as expected.
// - Test pattern matching for simple string, wild cards with collated comparison. Verify that the 
// return value is KErrNotFound when pattern doesn't match.
// - Check Locating methods by searching character in forward and backward direction and 
// verify the return value is KErrNotFound when unavailable character is searched.
// - Check copying strings and converting those into lower and upper case strings are
// as expected.
// - Check Find methods by searching string and verify the return value is KErrNotFound when 
// unavailable string is searched.
// - Check Repeat, Trim, Insert and Delete methods are as expected.
// - Check the formatting operations are as expected.
// - Check integer to decimal character representation is as expected.
// - Check integer to character representation with different number system is as expected.
// - Check string formatting with variable parameter list is as expected
// - Check Replace method by replacing string at different places in a string is as expected.
// - Check the conversion of real numbers, extended precision real numbers into string
// format is as expected.
// - Check Format and FormatList methods are as expected.
// - Check Format of TReal is as expected.
// - Check the non-leaving and leaving descriptors overflow handlers are as expected. 
// Platforms/Drives/Compatibility:
// All 
// Assumptions/Requirement/Pre-requisites:
// Failures and causes:
// Base Port information:
// 
//

#include <e32test.h>
#include <e32math.h>
#include <hal.h>
#include <hal_data.h>
#include <hal_data.h>
#include <e32svr.h>
#include <estring.h>

#ifdef __VC32__
    // Solve compilation problem caused by non-English locale
    #pragma setlocale("english")
#endif

LOCAL_D RTest test(_L("T_LSFORMAT"));

#pragma warning(disable: 4127) // disabling warning "conditional expression is constant"
#pragma warning(disable: 4310) // disabling warning "cast truncates constant value"

#undef _TL
#define _TL(a) DESTEMPLATE((S*)RTest::String(sizeof(S),(TText8*)a,(TText16*)L ## a)) 
#undef _TS
#define _TS(a) ((const S*)RTest::String(sizeof(S),(TText8*)a,(TText16*)L ## a)) 

template<class T,class S,class DESTEMPLATE>	
class TestLString
	{
public:
	TestLString(TInt aLength); // Test class constructor.
	void Test1L();   // Tests all functions of the class with auto extension
	void Test2();   // Tests all constructors.
	void Test3();	// Tests all assignment operators
	void Test4();	// Tests all comparison operators
	void Test7();	// Tests all conversion 
	void Test8();	// Tests all comparison
	void Test9();	// Tests all matching
	void Test10();	// Tests all locating
	void Test11L();	// Tests all Copying
	void Test12();	// Tests all finding
	void Test13L();	// Tests all basic like ops
	void Test14L();  // Tests all formating with auto extension 
	void Test15L();  // Tests all replacing with auto extension 
	void test_TBufL(); // Test all classes
protected:
	void Test14_ReorderedParameterFormatting(TInt aDummyParameter, ...);
	void Test14_ReorderedParameterFormattingL(TInt aDummyParameter, ...);
private:
	TInt iMaxBufLength;
	};

template<class T>
void TestEq(const T& a, const T& b, TInt aLine)
	{
	if (a!=b)
		{
		LString16 buf(256);
		test.Printf(_L("LINE %d:\n"),aLine);
		buf.CopyL(a);
		test.Printf(_L("a=%S\n"),&buf);
		buf.CopyL(b);
		test.Printf(_L("b=%S\n"),&buf);
		test(0);
		}
	}


#define TESTEQ(a,b)	TestEq<T>((a),(b),__LINE__)

template <class T,class S,class DESTEMPLATE>
GLDEF_C TestLString<T,S,DESTEMPLATE>::TestLString(TInt aLength)
// Constructor.
	: iMaxBufLength(aLength)
	{}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4011
@SYMTestCaseDesc Tests LString Auto-extending functions 
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the auto-extending LString 
				functions to manipulate the strings.
				Verifies that the LStrings are auto-extended as expected.
@SYMTestExpectedResults The LString objects should be auto-extended as required to
				contain any new data.
@SYMREQ	10372-3
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test1L()
	{
	
	test.Start(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4011"));
	
	test.Next(_L("Extending Methods"));

	T a(1);
	T b(1);

	test(a.Length()==0);
	test(a.MaxLength() >=1);

	test.Next(_L("Setting a bigger length "));
	a.SetLengthL(10);
	test(a.Length()== 10);
	test(a.MaxLength() >= 10);

	test.Next(_L("operator+ with memory extension "));
	test(b.MaxLength() >=1);
	b += a;
	test(b.Length()== 10);
	test(b.MaxLength() >= 10);
	
	TChar c('A');
	test(b.MaxLength() >=1);
	b += c;
	test(b.Length()== 11);
	test(b.MaxLength() >= 10);

	test.Next(_L("Filling past current size "));
	a.FillL(' ', 20);
	test(a.Length()== 20);
	test(a.MaxLength() >= 20);

	test.Next(_L("Append filling past current size "));
	a.AppendFillL(' ', 10);
	test(a.Length()== 30);
	test(a.MaxLength() >= 30);

	test.Next(_L("Zeroing past current size "));
	a.FillZL(40);
	test(a.Length()== 40);
	test(a.MaxLength() >= 40);

	test.Next(_L("Entended Copying"));
	a.CopyL(b);
	test(a == b);

	a.CopyL(_TL("AB"));
	test(a == _TL("AB"));

	a.CopyFL(b);
	
	T d(b);
	d.Fold();
	test(a == d);

	a.CopyFL(_TL("AB"));
	test(a == _TL("ab"));

	a.CopyCL(b);

	d = b;
	d.Collate();
	test(a == d);

	a.CopyCL(_TL("AB"));
	test(a == _TL("ab"));

	a.CopyLCL(b);

	d = b;
	d.LowerCase();
	test(a == d);

	a.CopyLCL(_TL("AB"));
	test(a == _TL("ab"));

	a.CopyUCL(b);

	d = b;
	d.UpperCase();
	test(a == d);

	a.CopyUCL(_TL("AB"));
	test(a == _TL("AB"));

	a.CopyCPL(b);

	d = b;
	d.Capitalize();
	test(a == d);	
	
	a.CopyCPL(_TL("AB"));
	test(a == _TL("Ab"));

	test.Next(_L("Basic like ops"));

	a.Zero();
	b=_TL("AA");
	a.InsertL(0,b);
	test(a == b);

	test.Next(_L("Formating"));
	a.JustifyL(_TL("AB"),10,ELeft,' ');
	test(a == _TL("AB        "));
	
	a.JustifyL(b,10,ELeft,' ');
	test(a == _TL("AA        "));

	b.FillL('A',2);
	test(b == _TL("AA"));

	a.Zero();
	a.AppendJustifyL(_TL("AB"),10,ELeft,' ');
	test(a == _TL("AB        "));
	
	a.AppendJustifyL(b,10,ELeft,' ');
	test(a == _TL("AB        AA        "));
		
	DESTEMPLATE des(b);
	a.Zero();
	a.AppendJustifyL(des,2,10,ELeft,' ');
	test(a == _TL("AA        "));
	
	a.AppendJustifyL(des,2,KDefaultJustifyWidth ,ELeft,' ');
	test(a == _TL("AA        AA"));
	
	a.Zero();
	a.AppendJustifyL(des,10,ELeft,' ');
	test(a == _TL("AA        "));
	
	a.AppendJustifyL(des,KDefaultJustifyWidth ,ELeft,' ');
	test(a == _TL("AA        AA"));
	
	a.Zero();
	a.AppendJustifyL(b.PtrZL(),10,ELeft,' ');
	test(a == _TL("AA        "));
	
	a.AppendJustifyL(b.PtrZL(),KDefaultJustifyWidth,ELeft,' ');
	test(a == _TL("AA        AA"));
	
	a.AppendJustifyL(b.PtrZL(),2,KDefaultJustifyWidth,ELeft,' ');
	test(a == _TL("AA        AAAA"));
	
	a.Zero();
	a.AppendJustifyL(b.PtrZL(),2,10,ELeft,' ');
	test(a == _TL("AA        "));
	
	a.AppendJustifyL(b.PtrZL(),2,5,ELeft,' ');
	test(a == _TL("AA        AA   "));

	TInt v1=10;
	a.NumL(v1);
	test(a == _TL("10"));
	a.AppendNumL(v1);
	test(a == _TL("1010"));

	TInt v2=10;
	a.NumL((TUint)v2,EHex);
	test(a == _TL("a"));
	a.AppendNumL((TUint)v2,EHex);
	test(a == _TL("aa"));

	a.NumUCL((TUint)v2,EHex);
	test(a == _TL("A"));
	a.AppendNumUCL((TUint)v2,EHex);
	test(a == _TL("AA"));
	
	a.NumFixedWidthL((TUint)v2,EHex,1);
	test(a == _TL("a"));
	a.AppendNumFixedWidthL((TUint)v2,EHex,1);
	test(a == _TL("aa"));
	
	a.NumFixedWidthUCL((TUint)v2,EHex,1);
	test(a == _TL("A"));
	a.AppendNumFixedWidthUCL((TUint)v2,EHex,1);
	test(a == _TL("AA"));

	TReal v3=10.0;
	TRealFormat ff;
	ff.iType=KRealFormatFixed;
	ff.iWidth=10;
	ff.iPlaces=2;
	ff.iPoint='.';
	ff.iTriad=',';
	ff.iTriLen=3;
	a.NumL(v3,ff);
	test(a == _TL("10.00"));
	
	a.AppendNumL(v3,ff);
	test(a == _TL("10.0010.00"));
	
	a.FormatL(_TL("%d"),12);
	test(a == _TL("12"));
	
	a.AppendFormatL(_TL("%d"),12);
	test(a == _TL("1212"));

	b=_TL("%d");
	a.FormatL(b,12);
	test(a == _TL("12"));
	a.AppendFormatL(b,12);
	test(a == _TL("1212"));
	//*/
	
	test.Next(_L("Replacing"));
	a=_TL("AAC");
	b=_TL("B");
	a.ReplaceL(1,1,b);
	test(a == _TL("ABC"));
	
	test.End();
	}



/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4012
@SYMTestCaseDesc Tests all LString Constructors
@SYMTestPriority High
@SYMTestActions Creates LString objects using the different constructors.
				Verifies that length and contents are as expected
@SYMTestExpectedResults LString objects should be created with the correct 
				Length and contents
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test2()
// Tests all constructors.
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4012"));
	
	test.Next(_L("Default"));
	
	T a(iMaxBufLength);
	test(a.MaxLength() >=iMaxBufLength);
	test(a.Length()==0);

	test.Next(_L("By string"));
	T c(iMaxBufLength);
	c = _TL("AB");
	test(c.Length()==2);
	test(c[0]=='A');
	test(c[1]=='B');

	test.Next(_L("By buffer reference"));
	T d(iMaxBufLength);
	d = c;
	test(d.Length()==2);
	test(d[0]=='A');
	test(d[1]=='B');
	
	test.End();
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4013
@SYMTestCaseDesc Tests all LString Assignment operators
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the different assignment 
				operators to change the string contents
				Verifies that length and contents are as expected
@SYMTestExpectedResults LString data should be assigned as expected
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test3()
// Tests all assignment operators
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4013"));
	
	test.Next(_L("By String"));
	
	T a;
	a=_TL("AB");
	a+=_TL("CD");
	test(a.Length()==4);
	test(a==_TL("ABCD"));

	test.Next(_L("By buffer"));
	T b;
	b=a;
	b+=a;
	test(b.Length()==8);
	test(b==_TL("ABCDABCD"));
//	
	test.End();
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4014
@SYMTestCaseDesc Tests all LString comparison operators
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the different comparison 
				operators to compare string contents against other strings 
				Verifies that comparison operators work as expected
@SYMTestExpectedResults LString data comparison should work as expected
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test4()
// Test all comparison operators
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4014"));
	
	test.Next(_L("By NULL string"));
	
	T a;
	test(a==_TL(""));		// NULL strings
	test(!(a!=_TL("")));
	test(a<=_TL(""));
	test(a>=_TL(""));
	test(!(a<_TL("")));
	test(!(a>_TL("")));
	test(_TL("")==a);
	test(!(_TL("")!=a));
	test(_TL("")<=a);
	test(_TL("")>=a);
	test(!(_TL("")<a));
	test(!(_TL("")>a));

	test.Next(_L("By string or buffer"));
	a=_TL("abc");
	test(a==_TL("abc"));		// ==
	test(!(a==_TL("xyz")));
	test(!(a==_TL("aa")));
	test(_TL("abc")==a);
	test(!(_TL("xyz")==a));
	test(!(_TL("aa")==a));
	test(a!=_TL("xyz"));		// !=
	test(!(a!=_TL("abc")));
	test(a!=_TL("aa"));
	test(_TL("xyz")!=a);
	test(!(_TL("abc")!=a));
	test(_TL("aa")!=a);
	test(a<_TL("x"));			// <
	test(!(a<_TL("abc")));
	test(!(a<_TL("aa")));
	test(_TL("aa")<a);
	test(!(_TL("abc")<a));
	test(!(_TL("xyz")<a));
	test(a>_TL("aa"));			// >
	test(!(a>_TL("abc")));
	test(!(a>_TL("xyz")));
	test(_TL("xyz")>a);
	test(!(_TL("abc")>a));
	test(!(_TL("aa")>a));
	test(a>=_TL("abc"));		// >=
	test(!(a>=_TL("xyz")));
	test(a>=_TL("aa"));
	test(_TL("abc")>=a);
	test(!(_TL("aaa")>=a));
	test(_TL("xyz")>=a);
	test(a<=_TL("abc"));		// <=
	test(!(a<=_TL("aa")));
	test(a<=_TL("xyz"));
	test(_TL("abc")<=a);
	test(!(_TL("xyz")<=a));
	test(_TL("aa")<=a);

	test.Next(_L("By special characters"));
	a=_TL("!@#$%^&*()");
	test(a==_TL("!@#$%^&*()"));
	
	test.End();	
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4015
@SYMTestCaseDesc Tests all LString conversion methods
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the different conversion 
				methods to fold, collate and change case.
				Verifies that conversion methods work as expected.
@SYMTestExpectedResults LString data conversion methods should work as expected
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test7()
// Conversion operators
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4015"));
	
	test.Next(_L("Fold, collate ..."));
	
	T a;
	T b;
	a=_TL("abc AbC");
	b=_TL("ABC ABC");
	a.Fold();
	b.Fold();
	test(a==b);
	a=_TL("abc AbC");
	b=_TL("ABC ABC");
	a.Collate();
	b.Collate();
	test(a==b);
	a.LowerCase();
	test(a==_TL("abc abc"));
	a.Capitalize();
	test(a==_TL("Abc abc"));
	a.UpperCase();
	test(a==_TL("ABC ABC"));
	
	test.End();
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4016
@SYMTestCaseDesc Tests LString Compare() method
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the Compare()
				method to compare string contents against other strings 
				Verifies that comparison method works as expected
@SYMTestExpectedResults LString Compare() method should return a positive 
				value if this LString is greater than the specified descriptor. 
				Negative if this LString is less than the specified descriptor. 
				Zero if both descriptors have the same length and the their 
				contents are the same.
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test8()
// Comparison
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4016"));
	
	test.Next(_L("By string"));
	
	T a(iMaxBufLength);
	a=_TL("abc AbC");
	test(a.Compare(_TL("abc AbC"))==0);
	test(a.CompareF(_TL("ABC aBc"))==0);
	test(a.Compare(_TL("xyz"))!=0);
	test(a.CompareC(_TL("xyz"))!=0);
	test(a.CompareF(_TL("xyz"))!=0);

	test.Next(_L("By buffer"));
	T b;
	T c;
	a=_TL("abc AbC");
	b=_TL("abc AbC");
	c=_TL("xyz");
	test(a.Compare(b)==0);
	test(a.Compare(c)!=0);
	b=_TL("ABC aBc");
	test(a.CompareC(c)!=0);
	test(a.CompareF(b)==0);
	test(a.CompareF(c)!=0);

	test.End();
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4017
@SYMTestCaseDesc Tests LString MatchX() methods
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the MatchX()
				methods to compare string contents against other strings 
				Verifies that MatchX methods work as expected
@SYMTestExpectedResults LString MatchX() methods should compare string 
				contents and return the index of the first match, 
				or KErrNotFound if there is no match.
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test9()
// Matching (need to test explicit result as error KErrNotFound = KMaxTUint
// 			 and so registers as TRUE. (test parameter is TUint) )
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4017"));
	
	test.Next(_L("By string"));
	
	T a(iMaxBufLength);
	a=_TL("abc AbC");
	test(a.Match(_TL("abc AbC"))==0);
	test(a.MatchC(_TL("ABC aBc"))==0);
	test(a.MatchF(_TL("ABC aBc"))==0);
	test(a.Match(_TL("xyz"))==KErrNotFound);
	test(a.MatchC(_TL("xyz"))==KErrNotFound);
	test(a.MatchF(_TL("xyz"))==KErrNotFound);

	test.Next(_L("By buffer"));
	T b;
	T c;
	a=_TL("abc AbC");
	b=_TL("abc AbC");
	c=_TL("xyz");
	test(a.Match(b)==0);
	test(a.Match(c)==KErrNotFound);
	b=_TL("ABC aBc");
	test(a.MatchC(b)==0);
	test(a.MatchC(c)==KErrNotFound);
	test(a.MatchF(b)==0);
	test(a.MatchF(c)==KErrNotFound);

	test.Next(_L("Wildcards"));
	a=_TL("abcxyz");
	test(a.Match(_TL("abc*"))==0);
	test(a.Match(_TL("abw*"))==KErrNotFound);
	a=_TL("abcdefg");
	test(a.Match(_TL("a*fg"))==0);
	test(a.Match(_TL("a*f"))==KErrNotFound);
	test(a.Match(_TL("abc*fgh"))==KErrNotFound);
	a=_TL("abcdef");
	test(a.Match(_TL("abc?ef"))==0);
	test(a.Match(_TL("abc?xf"))==KErrNotFound);

	a=_TL("a(01)");
	test(a.Match(_TL("*(01)"))==1);
	test(a.Match(_TL("?(01)"))==0);
	test(a.Match(_TL("?(*)"))==0);
	test(a.Match(_TL("?(**)"))==0);

	test(a.Match(_TL("?(\?\?)"))==0);
	test(a.Match(_TL("*(*)"))>=0);
	test(a.Match(_TL("*(0?)"))>=0);
	test(a.Match(_TL("a(\?\?)"))==0);
	test(a.Match(_TL("*(\?\?)"))>=0);

	test.Next(_L("wild cards with collated comparison"));
	a = _TL("abcdefghijkl");
	test(a.MatchC(_TL("abc*")) == 0);
	test(a.MatchC(_TL("abc")) == KErrNotFound);
	test(a.MatchC(_TL("xyz")) == KErrNotFound);
	test(a.MatchC(_TL("*def")) == KErrNotFound);
	test(a.MatchC(_TL("*def*")) == 3);
	test(a.MatchC(_TL("*d?f*")) == 3);
	test(a.MatchC(_TL("a*kl")) == 0);
	test(a.MatchC(_TL("*e*?l")) == 4);
	test(a.MatchC(_TL("abc*dEf*")) == 0);
	
	T candidate(_TL(""));
	T search(_TL("**"));
	
	test(candidate.MatchC(search) == 0);
	
	candidate = _TL("");
	search = _TL("*");
	test(candidate.MatchC(search) == 0);
	
	candidate = _TL("abcd");
	search = _TL("*abc*cd");
	test(candidate.MatchC(search) == KErrNotFound);
   	
	if (sizeof(S) == 2)
		{
		test.Next(_L("Unicode MatchC and FindC treat base+accent as equal to composed character"));
		LString16 p = _L("te\x302te");
		test(p.MatchC(_L("t\xeate")) == 0);
		test(p.FindC(_L("t\xeate")) == 0);
		}

	test.End();
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4018
@SYMTestCaseDesc Tests LString Locate() methods
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the LocateX()
				methods to search for a character within the LString
				Verifies that LocateX() methods works as expected
@SYMTestExpectedResults LString LocateX() method should return offset 
				of the character position from the beginning of the data or 
				KErrNotFound if no matching character can be found.
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test10()
// Locating
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4018"));
	
	T a(iMaxBufLength);
	TChar b;

	test.Next(_L("First Char"));
	
	b='a';
	a=_TL("axaxa");
	test(a.Locate(b)==0);
	test(a.LocateF(b)==0);
	test(a.LocateReverse(b)==4);
	test(a.LocateReverseF(b)==4);

	test.Next(_L("Middle Char"));
	a=_TL("xaxa");
	test(a.Locate(b)==1);
	test(a.LocateF(b)==1);
	a=_TL("axax");
	test(a.LocateReverse(b)==2);
	test(a.LocateReverseF(b)==2);

	test.Next(_L("Last Char"));
	a=_TL("xxa");
	test(a.Locate(b)==2);
	test(a.LocateF(b)==2);
	a=_TL("axx");
	test(a.LocateReverse(b)==0);
	test(a.LocateReverseF(b)==0);

	test.Next(_L("Test for failure of locate"));
	a=_TL("xxx");
	test(a.Locate(b)==KErrNotFound);
	test(a.LocateF(b)==KErrNotFound);
	test(a.LocateReverse(b)==KErrNotFound);
	test(a.LocateReverseF(b)==KErrNotFound);

	test.End();
	}
	
	
/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4019
@SYMTestCaseDesc Tests LString CopyX() methods
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the CopyX()
				methods to copy data into the strings.
				Verifies that the length and contents of the LString 
				are as expefcted
@SYMTestExpectedResults LString CopyX() methods should copy data into 
				LString as expected
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test11L()
// Copying
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4019"));
	
	test.Next(_L("By String"));
	
	T a(iMaxBufLength);
	T b(iMaxBufLength);
		
	a.CopyL(_TL("abc"));
	test(a.Length()==3);
	test(a==_TL("abc"));
	a.CopyFL(_TL("abc"));
	test(a.Length()==3);
	b.CopyFL(_TL("ABC"));
	test(a==b);
	a.CopyLCL(_TL("AbC"));
	test(a==_TL("abc"));
	test(a.Length()==3);
	a.CopyCL(_TL("abc"));
	b.CopyCL(_TL("ABC"));
	test(a==b);
	test(a.Length()==3);
	a.CopyCPL(_TL("abc"));
	test(a==_TL("Abc"));
	test(a.Length()==3);
	a.CopyUCL(_TL("aBc"));
	test(a==_TL("ABC"));
	test(a.Length()==3);

	test.Next(_L("By buffer"));
	b=_TL("abc");
	a.CopyL(b);
	test(a==_TL("abc"));
	test(a.Length()==3);
	a=_TL("");
	a.CopyFL(b);
	b.CopyFL(_TL("ABC"));
	test(a==b);
	test(a.Length()==3);
	a=_TL("");
	b=_TL("AbC");
	a.CopyLCL(b);
	test(a==_TL("abc"));
	test(a.Length()==3);
	a=_TL("");
	b=_TL("abC");
	a.CopyCL(b);
	b.CopyCL(_TL("ABC"));
	test(a==b);
	test(a.Length()==3);
	a=_TL("");
	b=_TL("abC");
	a.CopyCPL(b);
	test(a==_TL("Abc"));
	test(a.Length()==3);
	a=_TL("");
	b=_TL("aBc");
	a.CopyUCL(b);
	test(a.Length()==3);
	test(a==_TL("ABC"));

	test.End();
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4020
@SYMTestCaseDesc Tests LString FindX() methods
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the FindX()
				methods to find substrings.
				Verifies that the index of the substring returned is as expected
@SYMTestExpectedResults LString FindX() methods should return the index of the first 
				character of the substring if it exists in the LString otherwise
				return KErrNotFound.
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test12()
// Finding
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4020"));
	
	test.Next(_L("By String"));
	
	T a(iMaxBufLength);
	T b(iMaxBufLength);
	
	a=_TL("abccef");
	test(a.Find(_TL(""))==0);
	test(a.Find(_TL("abc"))==0);
	test(a.Find(_TL("cce"))==2);
	test(a.Find(_TL("cef"))==3);
	test(a.Find(_TL("efg"))==KErrNotFound);
	test(a.Find(_TL("xxx"))==KErrNotFound);
	test(a.FindF(_TL(""))==0);
	test(a.FindF(_TL("AbC"))==0);
	test(a.FindF(_TL("CcE"))==2);
	test(a.FindF(_TL("CeF"))==3);
	test(a.FindF(_TL("efg"))==KErrNotFound);
	test(a.FindF(_TL("xxx"))==KErrNotFound);
	test(a.FindC(_TL(""))==0);
	test(a.FindC(_TL("aBc"))==0);
	test(a.FindC(_TL("cce"))==2);
	test(a.FindC(_TL("cEf"))==3);
	test(a.FindC(_TL("efg"))==KErrNotFound);
	test(a.FindC(_TL("xxx"))==KErrNotFound);

	test.Next(_L("By buffer"));
	test(a.Find(b)==0);
	test(a.FindF(b)==0);
	test(a.FindC(b)==0);
	b=_TL("xxx");
	test(a.Find(b)==KErrNotFound);
	test(a.FindF(b)==KErrNotFound);
	test(a.FindC(b)==KErrNotFound);
	b=_TL("efg");
	test(a.Find(b)==KErrNotFound);
	test(a.FindF(b)==KErrNotFound);
	test(a.FindC(b)==KErrNotFound);
	b=_TL("abc");
	test(a.Find(b)==0);
	b=_TL("cce");
	test(a.Find(b)==2);
	b=_TL("cef");
	test(a.Find(b)==3);
	b=_TL("AbC");
	test(a.FindF(b)==0);
	b=_TL("CcE");
	test(a.FindF(b)==2);
	b=_TL("CeF");
	test(a.FindF(b)==3);
	b=_TL("aBc");
	test(a.FindC(b)==0);
	b=_TL("cCe");
	test(a.FindC(b)==2);
	b=_TL("cEf");
	test(a.FindC(b)==3);

	test.End();
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4021
@SYMTestCaseDesc Tests LString modification methods
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the Repeat(),
				TrimX(), InsertL(), Delete(), Left(), Right() and Mid() methods
				to modify the string.
				Verifies that the LString is modified as expected
@SYMTestExpectedResults LString modification methods should modify the LString 
				data as expected.
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test13L()
// Basic like ops
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4021"));
	
	test.Next(_L("Repeat, trim, insert and delete"));
	
	T a(iMaxBufLength);
	T b(iMaxBufLength);
	TInt max=a.MaxLength(); 
	b=_TL("abc");
	a.Repeat(_TL("abc"));
	test(a==_TL(""));
	a.Repeat(b);
	test(a==_TL(""));
	for (TInt j=1;j<max;j++) // ?? Cannot SetLength = MaxLength
		{
		a.SetLengthL(j);
		a.Repeat(_TL("abc"));
        TInt i;
		for (i=0;i<j;i++)
			test(a[i]==b[i%3]);
		a=_TL("");
		a.SetLengthL(j);
		a.Repeat(b);
		for (i=0;i<j;i++)
			test(a[i]==b[i%3]);
		a=_TL("");
		}
	a=_TL("\t\n  ab \t\n ");
	a.TrimLeft();
	test(a==_TL("ab \t\n "));
	test(a.Length()==6);
	a=_TL("\t\n  ab \t\n ");
	a.TrimRight();
	test(a==_TL("\t\n  ab"));
	test(a.Length()==6);
	a=_TL(" \t\n ab \t \n");
	a.Trim();
	test(a==_TL("ab"));
	a.Trim();
	test(a==_TL("ab"));
	a=_TL("abc");
	b=_TL("123");
	a.InsertL(1,b);
	test(a==_TL("a123bc"));
	test(a.Length()==6);
	b=_TL("");
	a.InsertL(4,b);
	test(a==_TL("a123bc"));
	test(a.Length()==6);
	a.InsertL(0,b);
	test(a==_TL("a123bc"));
	test(a.Length()==6);
	a.Delete(1,3);
	test(a==_TL("abc"));
	test(a.Length()==3);
	a.Delete(0,3);
	test(a==_TL(""));
	test(a.Length()==0);

	test.Next(_L("TrimAll"));
	a=_TL("");
	a.TrimAll();
	test(a==_TL(""));
	a=_TL(" ");
	a.TrimAll();
	test(a==_TL(""));
	a=_TL("   ");
	a.TrimAll();
	test(a==_TL(""));
	a=_TL("    ab cd  ef    g");
	a.TrimAll();
	test(a==_TL("ab cd ef g"));
	a=_TL("abcdef");
	a.TrimAll();
	test(a==_TL("abcdef"));
	a=_TL("a  b\t cd\t\tef");
	a.TrimAll();
	test(a==_TL("a b\tcd\tef"));
	a=_TL("abcdef \t ghijk");
	a.TrimAll();
	test(a==_TL("abcdef ghijk"));
	a=_TL("abcdef g");
	a.TrimAll();
	test(a==_TL("abcdef g"));
	a=_TL("ab  cd  ef  gh  ij");
	a.TrimAll();
	test(a==_TL("ab cd ef gh ij"));
	a=_TL("a        b          c     defg h     i  jk l     mno pqr stu  vw   xyz");
	a.TrimAll();
	test(a==_TL("a b c defg h i jk l mno pqr stu vw xyz"));

	test.Next(_L("Right, Left and Mid"));
	a=_TL("abcdef");
	b = a.Left(3);
	test(b==_TL("abc"));
	test(b.Length()==3);
	b = a.Right(3);
	test(b==_TL("def"));
	b = a.Mid(2);
	test(b==_TL("cdef"));
	test(b.Length()==4);
	b = a.Left(2);
	test(b==_TL("ab"));
	test(b.Length()==2);
	b = a.Right(2);
	test(b==_TL("ef"));
	b = a.Mid(2,1);
	test(b==_TL("c"));
	test(b.Length()==1);
	b = a.Left(6);
	test(b==_TL("abcdef"));
	test(b.Length()==6);
	b=_TL("");
	b.SetLengthL(4);
	b = a.Right(6);
	test(b==_TL("abcdef"));
	test(b.Length()==6);
	a = a.Left(6);
	test(a==_TL("abcdef"));
	b=_TL("");
	b = a.Mid(0,6);
	test(b==_TL("abcdef")); 
	test.End();
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4022
@SYMTestCaseDesc Tests LString formatting methods
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the LString formatting methods
				to format the string.
				Verifies that the LString is formatted as expected
@SYMTestExpectedResults LString formatting methods should fromta the LString as expected
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test14L()
// Formating operations
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4022"));
	
	test.Next(_L("Justify"));
	//---------------------------Apepend+Justify-----------------------------------
	T a(1);
	T b(1);
	T d(1);

	TInt aWidth;
	TChar c;
	a=_TL("wxyz");
	b=_TL("abc");
	d=_TL("linearisation");
	const S* pD=_TS("299792458");
	c='x';
	aWidth=KDefaultJustifyWidth; // Left justified, Default width
	a.JustifyL(b,aWidth,ELeft,c);
	test(a==b);
	test(a.Length()==3);
	a.AppendJustifyL(b,aWidth,ELeft,c);
	test(a==_TL("abcabc"));
	test(a.Length()==6);
	aWidth=1;	// Width < String length
	a.JustifyL(b,aWidth,ELeft,c);
	test(a==_TL("a"));
	test(a.Length()==1);
	a.AppendJustifyL(b,aWidth,ELeft,c);
	test(a==_TL("aa"));
	test(a.Length()==2);
	aWidth=5; // Width > String length
	a.JustifyL(b,aWidth,ELeft,c);
	test(a==_TL("abcxx"));
	test(a.Length()==5);
	a.AppendJustifyL(b,aWidth,ELeft,c);
	test(a==_TL("abcxxabcxx"));
	test(a.Length()==10);

	aWidth=KDefaultJustifyWidth; // Right justified, Default width
	a.JustifyL(b,aWidth,ERight,c);
	test(a==b);
	test(a.Length()==3);
	a.AppendJustifyL(b,aWidth,ERight,c);
	test(a==_TL("abcabc"));
	test(a.Length()==6);

	aWidth=1; // Right justified, Width < String length
	a.JustifyL(b,aWidth,ERight,c);
	test(a==_TL("a"));
	test(a.Length()==1);
	a.AppendJustifyL(b,aWidth,ERight,c);
	test(a==_TL("aa"));
	test(a.Length()==2);

	aWidth=5; // Right justified, width > String length
	a.JustifyL(b,aWidth,ERight,c);
	test(a==_TL("xxabc"));
	test(a.Length()==5);
	a.AppendJustifyL(b,aWidth,ERight,c);
	test(a==_TL("xxabcxxabc"));
	test(a.Length()==10);

	aWidth=KDefaultJustifyWidth; // Center justified, Default width
	a.JustifyL(b,aWidth,ECenter,c);
	test(a==b);
	test(a.Length()==3);
	a.AppendJustifyL(b,aWidth,ECenter,c);
	test(a==_TL("abcabc"));
	test(a.Length()==6);

	aWidth=1; // Centre justified, width < String length
	a.JustifyL(b,aWidth,ECenter,c);
	test(a==_TL("a"));
	test(a.Length()==1);
	a.AppendJustifyL(b,aWidth,ECenter,c);
	test(a==_TL("aa"));
	test(a.Length()==2);

	aWidth=5; // Centre justified, width > String length
	a.JustifyL(b,aWidth,ECenter,c);
	test(a==_TL("xabcx"));
	test(a.Length()==5);
	a.AppendJustifyL(b,aWidth,ECenter,c);
	test(a==_TL("xabcxxabcx"));
	test(a.Length()==10);

	//----------------------Num----------------------------------------------------
	test.Next(_L("Num"));
	TInt j=-2147483647-1; 
	a.NumL(j);
	test(a==_TL("-2147483648"));
	test(a.Length()==11);
	TUint i=2147483648u;
	a.NumL(i);
	test(a==_TL("2147483648"));
	test(a.Length()==10);
	if (a.MaxLength()>31)
		{
		a.NumL(i,EBinary);
		test(a==_TL("10000000000000000000000000000000"));
		test(a.Length()==32);
		a=_TL("");
		a.NumUCL(i,EBinary);
		test(a==_TL("10000000000000000000000000000000"));
		test(a.Length()==32);
		}
	i=31;
	a.NumL(i,EBinary);
	test(a==_TL("11111"));
	test(a.Length()==5);
	a=_TL("");
	a.NumUCL(i,EBinary);
	test(a==_TL("11111"));
	test(a.Length()==5);
	i=2147483648u;
	a.NumL(i,EOctal);
	test(a==_TL("20000000000"));
	test(a.Length()==11);
	a=_TL("");
	a.NumUCL(i,EOctal);
	test(a==_TL("20000000000"));
	test(a.Length()==11);
	a.NumL(i,EDecimal);
	test(a==_TL("2147483648"));
	test(a.Length()==10);
	a=_TL("");
	a.NumUCL(i,EDecimal);
	test(a==_TL("2147483648"));
	test(a.Length()==10);
	a.NumL(i,EHex);
	test(a==_TL("80000000"));
	test(a.Length()==8);
	a=_TL("");
	a.NumUCL(i,EHex);
	test(a==_TL("80000000"));
	test(a.Length()==8);
	i=0;
	a.NumL(i);
	test(a==_TL("0"));
	test(a.Length()==1);
	a=_TL("abc");
	a.NumL(i,EBinary);
	test(a==_TL("0"));
	test(a.Length()==1);
	a=_TL("abc");
	a.NumUCL(i,EBinary);
	test(a==_TL("0"));
	test(a.Length()==1);
	a=_TL("abc");
	a.NumL(i,EOctal);
	test(a==_TL("0"));
	test(a.Length()==1);
	a=_TL("");
	a.NumUCL(i,EOctal);
	test(a==_TL("0"));
	test(a.Length()==1);
	a=_TL("abc");
	a.NumL(i,EDecimal);
	test(a==_TL("0"));
	test(a.Length()==1);
	a=_TL("");
	a.NumUCL(i,EDecimal);
	test(a==_TL("0"));
	test(a.Length()==1);
	a=_TL("abc");
	a.NumL(i,EHex);
	test(a==_TL("0"));
	test(a.Length()==1);
	a=_TL("");
	a.NumUCL(i,EHex);
	test(a==_TL("0"));
	test(a.Length()==1);

	//	TInt i=a.Num(6.2,format); NOT IMPLEMENTED
	//----------------------AppendNum----------------------------------------------------
	test.Next(_L("AppendNum"));
	a=_TL("0");
	a.AppendNumL(j);
	test(a==_TL("0-2147483648"));
	test(a.Length()==12);

	a=_TL("abc");
	i=4294967295u;
	a.AppendNumL(i);
	test(a==_TL("abc4294967295"));
	test(a.Length()==13);

	j=2147483647;
	a=_TL("abc");
	a.AppendNumL(j);
	test(a==_TL("abc2147483647"));
	test(a.Length()==13);

	a=_TL("a");
	i=180150000;
	if (a.MaxLength()>28)
		{
		a.AppendNumL(i,EBinary);
		test(a==_TL("a1010101111001101111011110000"));
		test(a.Length()==29);
		}
	a=_TL("a");
	a.AppendNumL(15,EBinary);
	test(a==_TL("a1111"));
	test(a.Length()==5);

	a=_TL("a");
	a.AppendNumL(i,EDecimal);
	test(a==_TL("a180150000"));
	test(a.Length()==10);

	a=_TL("a");
	a.AppendNumL(i,EOctal);
	test(a==_TL("a1257157360"));
	test(a.Length()==11);

	a=_TL("a");
	a.AppendNumUCL(i,EHex);
	test(a==_TL("aABCDEF0"));
	test(a.Length()==8);

	
	a=_TL("a");
	a.AppendNumFixedWidthL(i,EHex,3);
	test(a==_TL("aabc"));
	test(a.Length()==4);
	
	a=_TL("a");
	a.AppendNumFixedWidthUCL(i,EHex,4);
	test(a==_TL("aABCD"));
	test(a.Length()==5);
	

	//----------------------Format----------------------------------------------------
	test.Next(_L("Format"));
	a=_TL("");
	b=_TL("cde");
	a.FormatL(_TL("%S"),&b);
	test(a==b);
	test(a.Length()==3);

    DESTEMPLATE xyz=_TL("xyzwpq");
	a.FormatL(_TL("%S"),&xyz);
	test(a==_TL("xyzwpq"));
	test(a.Length()==6);

    DESTEMPLATE cde=_TL("cde");
	a.FormatL(_TL("ab %-x5S"),&cde);
	test(a==_TL("ab cdexx"));
	test(a.Length()==8);

	a.FormatL(_TL("ab %=x5S"),&cde);
	test(a==_TL("ab xcdex"));
	test(a.Length()==8);

	a.FormatL(_TL("ab %+x5S"),&cde);
	test(a==_TL("ab xxcde"));
	test(a.Length()==8);

	a.FormatL(_TL("ab %5S"),&cde);
	test(a==_TL("ab   cde"));
	test(a.Length()==8);

	a.FormatL(_TL("ab %-**S"),'x',5,&cde);
	test(a==_TL("ab cdexx"));
	test(a.Length()==8);

	a.FormatL(_TL("ab %*S"),5,&cde);
	test(a==_TL("ab   cde"));
	test(a.Length()==8);

	a=_TL("xyz");
	a.FormatL(_TL("ab %-x5S"),&b);
	test(a==_TL("ab cdexx"));
	test(a.Length()==8);

	a=_TL("xyz");
	a.FormatL(_TL("ab %-**S"),'x',5,&b);
	test(a==_TL("ab cdexx"));
	test(a.Length()==8);

	a=_TL("xyz");
	a.FormatL(_TL("ab %*S"),5,&b);
	test(a==_TL("ab   cde"));
	test(a.Length()==8);

	DESTEMPLATE fred=_TL("fred");
	a.FormatL(_TL("%+0*S"),10,&fred);
	test(a==_TL("000000fred"));
	test(a.Length()==10);

	a.FormatL(_TL("%-0*S"),7,&fred);
	test(a==_TL("fred000"));
	test(a.Length()==7);

	a.FormatL(_TL("%0*S"),11,&fred);
	test(a==_TL("0000000fred"));
	test(a.Length()==11);

	a.FormatL(_TL("c=%s"),pD);
	TESTEQ(a,_TL("c=299792458"));

	a.FormatL(_TL("c=%10.6s"),pD);
	test(a==_TL("c=    299792"));

	a.FormatL(_TL("c=%*.*s"),5,4,pD);
	test(a==_TL("c= 2997"));

	a.FormatL(_TL("%S"),&d);
	test(a==_TL("linearisation"));

	a.FormatL(_TL("%10.6S"),&d);
	test(a==_TL("    linear"));

	a.FormatL(_TL("%*.*S"),5,4,&d);
	test(a==_TL(" line"));

	a.FormatL(_TL("%*.*Sed"),10,8,&d);
	test(a==_TL("  linearised"));

	a.FormatL(_TL("%*.*S"),14,20,&d);
	test(a==_TL(" linearisation"));

	a.FormatL(_TL("ab %-,5b"),7);
	test(a==_TL("ab 111,,"));
	test(a.Length()==8);

	a.FormatL(_TL("ab %=,5O"),31);
	test(a==_TL("ab ,37,,"));
	test(a.Length()==8);

	a.FormatL(_TL("ab %+xlx"),TInt64(171));
	test(a==_TL("ab ab"));
	test(a.Length()==5);

	a.FormatL(_TL("ab %+xlX %+xlx"),TInt64(171),TInt64(171));
	TESTEQ(a,_TL("ab AB ab"));
	test(a.Length()==8);

	a.FormatL(_TL("ab %lu"),MAKE_TINT64((TUint)(KMinTInt),0));
	test(a==_TL("ab 9223372036854775808"));
	test(a.Length()==22);

	a.FormatL(_TL("ab %ld"),MAKE_TINT64((TUint)(KMinTInt),1));
	test(a==_TL("ab -9223372036854775807"));
	test(a.Length()==23);

	a.FormatL(_TL("ab %ld"),MAKE_TINT64((TUint)(KMinTInt),0));
	test(a==_TL("ab -9223372036854775808"));
	test(a.Length()==23);

	a.FormatL(_TL("ab %ld"),MAKE_TINT64((TUint)(KMaxTInt),KMaxTUint));
	test(a==_TL("ab 9223372036854775807"));
	test(a.Length()==22);

	a.FormatL(_TL("ab %ld"),MAKE_TINT64(KMaxTUint,KMaxTUint));
	test(a==_TL("ab -1"));
	test(a.Length()==5);

	a.FormatL(_TL("ab %lu"),MAKE_TINT64(KMaxTUint,KMaxTUint));
	test(a==_TL("ab 18446744073709551615"));
	test(a.Length()==23);

	a.FormatL(_TL("ab %ld"),TInt64(0));
	test(a==_TL("ab 0"));
	test(a.Length()==4);

	a.FormatL(_TL("ab %lb"),TInt64(0));
	test(a==_TL("ab 0"));
	test(a.Length()==4);

	a.FormatL(_TL("ab %lx"),TInt64(0));
	test(a==_TL("ab 0"));
	test(a.Length()==4);

	a.FormatL(_TL("ab %lo"),TInt64(0));
	test(a==_TL("ab 0"));
	test(a.Length()==4);

	a.FormatL(_TL("ab %lu"),TInt64(0));
	test(a==_TL("ab 0"));
	test(a.Length()==4);

	a.FormatL(_TL("ab %lb"),MAKE_TINT64((TUint)(KMaxTInt),KMaxTUint));
	test(a==_TL("ab 111111111111111111111111111111111111111111111111111111111111111"));
	test(a.Length()==66);

	a.FormatL(_TL("ab %lb"),MAKE_TINT64(KMaxTUint,KMaxTUint));
	test(a==_TL("ab 1111111111111111111111111111111111111111111111111111111111111111"));
	test(a.Length()==67);

	a.FormatL(_TL("ab %lx"),MAKE_TINT64((TUint)(KMaxTInt),KMaxTUint));
	test(a==_TL("ab 7fffffffffffffff"));
	test(a.Length()==19);

	a.FormatL(_TL("ab %lx"),MAKE_TINT64(KMaxTUint,KMaxTUint));
	test(a==_TL("ab ffffffffffffffff"));
	test(a.Length()==19);

	a.FormatL(_TL("ab %lo"),MAKE_TINT64((TUint)(KMaxTInt),KMaxTUint));
	test(a==_TL("ab 777777777777777777777"));
	test(a.Length()==24);

	// tests which excercise any 8 byte alignment requirement on 64bit integers
	a.FormatL(_TL("%li%S"),MAKE_TINT64(1,2),&fred);
	test(a==_TL("4294967298fred"));

	a.FormatL(_TL("%S%li%S"),&fred,MAKE_TINT64(1,2),&fred);
	test(a==_TL("fred4294967298fred"));

	a.FormatL(_TL("%lu%S"),MAKE_TINT64(1,2),&fred);
	test(a==_TL("4294967298fred"));

	a.FormatL(_TL("%S%lu%S"),&fred,MAKE_TINT64(1,2),&fred);
	test(a==_TL("fred4294967298fred"));

	a.FormatL(_TL("ab %U"),233);
	test(a==_TL("ab 233"));
	test(a.Length()==6);

	a.FormatL(_TL("ab %*d"),5,-131);
	test(a==_TL("ab  -131"));
	test(a.Length()==8);

	a.FormatL(_TL("ab%c"),'x');
	test(a==_TL("abx"));
	test(a.Length()==3);

	a.FormatL(_TL("%W"),-131);
	test(*(TInt32*)a.Ptr()==-131);

	a.FormatL(_TL("%M"),-131);
	test(*(TInt32*)a.Ptr()==2113929215);

	a.FormatL(_TL("%w"),-131);
	test(*(TInt16*)a.Ptr()==-131);

	a.FormatL(_TL("%m"),-131);
	test(*(TInt16*)a.Ptr()==32255);

	//----------------------AppendFormat----------------------------------------------------
	a=_TL("xyz");
	a.AppendFormatL(_TL("ab %+xlx"),TInt64(171));
	test(a==_TL("xyzab ab"));
	test(a.Length()==8);

	a=_TL("xyz");
	a.AppendFormatL(_TL("ab %5S"),&b);
	test(a==_TL("xyzab   cde"));
	test(a.Length()==11);

	a=_TL("xyz");
	a.AppendFormatL(_TL("%W"),-131);
//	test(*(TInt32*)(a.Ptr()+3)==-131); // Alignment-safe version:
    TInt val;
    Mem::Copy(&val,&a[3],4);
	test(val==-131);

// Cannot do this on GCCE or ARMv5 because of 
// "Cannot pass objects of non-POD type through '...'. 
// Call will abort at runtime".

#ifdef __WINSCW__
	const TAny* const zeroTerminatedString=(sizeof(S)==2)? (const TAny*)_S16(":-)E"): (const TAny*)_S8(":-)E");
	const TInt dummyParameter=0;
	b=_TL("ebdb");
	Test14_ReorderedParameterFormattingL(dummyParameter, 0x20ac, 11, 3, 13.89543, zeroTerminatedString, '!', TInt64(199), 2, &b, 6, 30005, TRealX(0.125), 0x8bdd);
#endif

	test.End();
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4024
@SYMTestCaseDesc Tests LString leaving variant FormatListL() method
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the LString FormatListL() method
				to format the string.
				Verifies that the LString is formatted as expected
@SYMTestExpectedResults LString FormatListL() method should work as expected
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test14_ReorderedParameterFormattingL(TInt aDummyParameter, ...)
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4024"));
	
	VA_LIST parameterList;
	T generated(1);
	T expected(1);

	VA_START(parameterList, aDummyParameter);
	generated.FormatListL(_TL("\t%-**.*fqq%.3swww%+*5Ldeeee%.*Srrrrr%0*xtttttt%.3Fyyyyyyy%c"), parameterList);
	test(generated.Length()==61);
	expected.FormatL(_TL("\t13.895%c%c%c%c%cqq:-)www!!199eeeeebrrrrr007535tttttt0.125yyyyyyy"), (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac);
	test(generated.Left(generated.Length()-1)==expected);
	test(generated[generated.Length()-1]==(S)0x8bdd);

	VA_START(parameterList, aDummyParameter);
	generated.FormatListL(_TL("\t%$1$-**.*fqq%.3swww%+*5Ldeeee%.*Srrrrr%0*xtttttt%$6$.3Fyyyyyyy%c"), parameterList);
	test(generated.Length()==61);
	expected.FormatL(_TL("\t13.895%c%c%c%c%cqq:-)www!!199eeeeebrrrrr007535tttttt0.125yyyyyyy"), (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac);
	test(generated.Left(generated.Length()-1)==expected);
	test(generated[generated.Length()-1]==(S)0x8bdd);

	VA_START(parameterList, aDummyParameter);
	generated.FormatListL(_TL("\t%$6$.3Fqq%.3swww%+*5Ldeeee%.*Srrrrr%0*xtttttt%$1$-**.*fyyyyyyy%c"), parameterList);
	test(generated.Length()==61);
	expected.FormatL(_TL("\t0.125qq:-)www!!199eeeeebrrrrr007535tttttt13.895%c%c%c%c%cyyyyyyy"), (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac);
	test(generated.Left(generated.Length()-1)==expected);
	test(generated[generated.Length()-1]==(S)0x8bdd);

	VA_START(parameterList, aDummyParameter);
	generated.FormatListL(_TL("\t%-**.*fqq%.3swww%$5$0*xeeee%.*Srrrrr%$3$+*5Ldtttttt%.3Fyyyyyyy%c"), parameterList);
	test(generated.Length()==61);
	expected.FormatL(_TL("\t13.895%c%c%c%c%cqq:-)www007535eeeeebrrrrr!!199tttttt0.125yyyyyyy"), (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac);
	test(generated.Left(generated.Length()-1)==expected);
	test(generated[generated.Length()-1]==(S)0x8bdd);

	VA_START(parameterList, aDummyParameter);
	generated.FormatListL(_TL("\t%-**.*fqq%$4$.*Swww%+*5Ldeeee%$2$.3srrrrr%0*xtttttt%.3Fyyyyyyy%c"), parameterList);
	test(generated.Length()==61);
	expected.FormatL(_TL("\t13.895%c%c%c%c%cqqebwww!!199eeee:-)rrrrr007535tttttt0.125yyyyyyy"), (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac);
	test(generated.Left(generated.Length()-1)==expected);
	test(generated[generated.Length()-1]==(S)0x8bdd);

	VA_START(parameterList, aDummyParameter);
	generated.FormatListL(_TL("\t%-**.*fqq%.3swww%+*5Ldeeee%$7$crrrrr%0*xtttttt%.3Fyyyyyyy%$4$.*S"), parameterList);
	test(generated.Length()==61);
	expected.FormatL(_TL("\t13.895%c%c%c%c%cqq:-)www!!199eeee"), (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac);
	test(generated.Left(29)==expected);
	test(generated[29]==(S)0x8bdd);
	test(generated.Mid(29+1)==_TL("rrrrr007535tttttt0.125yyyyyyyeb"));

	VA_START(parameterList, aDummyParameter);
	generated.FormatListL(_TL("\t%$4$.*Sqq%.3swww%+*5Ldeeee%$6$.3Frrrrr%0*xtttttt%$1$-**.*fyyyyyyy%c"), parameterList);
	test(generated.Length()==61);
	expected.FormatL(_TL("\tebqq:-)www!!199eeee0.125rrrrr007535tttttt13.895%c%c%c%c%cyyyyyyy"), (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac);
	test(generated.Left(generated.Length()-1)==expected);
	test(generated[generated.Length()-1]==(S)0x8bdd);

	VA_START(parameterList, aDummyParameter);
	generated.FormatListL(_TL("\t%$7$cqq%$6$.3Fwww%$5$0*xeeee%.*Srrrrr%$3$+*5Ldtttttt%$2$.3syyyyyyy%$1$-**.*f"), parameterList);
	test(generated.Length()==61);
	test(generated.Left(1)==_TL("\t"));
	test(generated[1]==(S)0x8bdd);
	expected.FormatL(_TL("qq0.125www007535eeeeebrrrrr!!199tttttt:-)yyyyyyy13.895%c%c%c%c%c"), (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac);
	test(generated.Mid(2)==expected);

	VA_START(parameterList, aDummyParameter);
	generated.FormatListL(_TL("\t%$7$cqq%$6$.3Fwww%$5$0*xeeee%$4$.*Srrrrr%$3$+*5Ldtttttt%$2$.3syyyyyyy%$1$-**.*f"), parameterList);
	test(generated.Length()==61);
	test(generated.Left(1)==_TL("\t"));
	test(generated[1]==(S)0x8bdd);
	expected.FormatL(_TL("qq0.125www007535eeeeebrrrrr!!199tttttt:-)yyyyyyy13.895%c%c%c%c%c"), (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac, (S)0x20ac);
	test(generated.Mid(2)==expected);
	
	test.End();
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4025
@SYMTestCaseDesc Tests LString ReplaceL() method
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the ReplaceL() method
				to replace the string data.
				Verifies that the LString data is replaced as expected
@SYMTestExpectedResults LString ReplaceL() method should replace data 
				in the LString as expected.
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::Test15L()
// Replacing
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4025"));
	
	test.Next(_L("Replace"));
	T a(1);
	T b(1);
	test(a.MaxLength()>=1);
	a=_TL("abccccc");
	b=_TL("def");
	a.ReplaceL(3,4,b); // Replace with smaller in middle (insert and delete)
	test(a==_TL("abcdef"));
	a.ReplaceL(1,1,b); // Replace with larger in middle (insert and delete)
	test(a==_TL("adefcdef"));
	a.ReplaceL(0,8,_TL("")); // Replace complete string (delete)
	test(a==_TL(""));
	a.ReplaceL(0,0,b); // Replace at beginning (insert)
	test(a==b);
	a.ReplaceL(3,0,_TL("xyz")); // Replace at end (append)
	test(a==_TL("defxyz"));
	a.ReplaceL(0,0,_TL("")); // Replace nothing at beginning (do nothing)
	test(a==_TL("defxyz"));
	a.ReplaceL(6,0,_TL("")); // Replace nothing at end (do nothing)
	test(a==_TL("defxyz"));
	test.End();
	}


template<class T,class S,class DESTEMPLATE>
GLDEF_C void TestLString<T,S,DESTEMPLATE>::test_TBufL()
// Test the TBuf class.
	{
	test.Start(_L("All operations"));
	Test1L();
	test.Next(_L("Constructors"));
	Test2();
	test.Next(_L("Additional tests"));
	Test3();
	test.Next(_L("Comparison operators"));
	Test4();
	test.Next(_L("Conversion operators"));
	Test7();
	test.Next(_L("Comparison"));
	Test8();
	test.Next(_L("Matching"));
	Test9();
	test.Next(_L("Locating"));
	Test10();
	test.Next(_L("Copying"));
	Test11L();
	test.Next(_L("Finding"));
	Test12();
	test.Next(_L("Basic like ops"));
	Test13L();
	test.Next(_L("Leaving variants of Formating"));
	Test14L();
	
	test.Next(_L("Leaving variants of Replacing"));
	Test15L();
	test.End();
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4026
@SYMTestCaseDesc Tests LString FormatL() method
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the FormatL() method
				to format the string data.
				Verifies that the LString data is formatted as expected
@SYMTestExpectedResults LString FormatL() method should format data 
				in the LString as expected.
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
LOCAL_C void testFormatL()
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4026"));
	
	//TBuf<0x100> aa;
	T aa(1);
	aa.FormatL(_TL("x%- 5fx"), 6.2345678);
	test(aa==_TL("x6.234568x"));
	aa.FormatL(_TL("x%+ 5fx"), 6.2345678);
	test(aa==_TL("x6.234568x"));
	aa.FormatL(_TL("x% 5fx"), 6.2345678);
	test(aa==_TL("x6.234568x"));
	aa.FormatL(_TL("x%= 5fx"), 6.2345678);
	test(aa==_TL("x6.234568x"));
	aa.FormatL(_TL("x%- 10fx"), 6.2345);
	test(aa==_TL("x6.234500  x"));
	aa.FormatL(_TL("x%+ 10fx"), 6.2345);
	test(aa==_TL("x  6.234500x"));
	aa.FormatL(_TL("x% 10fx"), 6.2345);
	test(aa==_TL("x  6.234500x"));
	aa.FormatL(_TL("x%= 10fx"), 6.2345);
	test(aa==_TL("x 6.234500 x"));
	aa.FormatL(_TL("x%10fx"), 12345352342.545);
	test(aa==_TL("x12,345,352,342.545000x"));
	aa.FormatL(_TL("x%20.9fx"), 1.0123456789);
	test(aa==_TL("x         1.012345679x"));
	aa.FormatL(_TL("x%5.1fx"), 1.99);
	test(aa==_TL("x  2.0x"));

// Cannot do these on GCCE or ARMv5 because of 
// "Cannot pass objects of non-POD type through '...'. 
// Call will abort at runtime".
#ifdef __WINSCW__
	aa.FormatL(_TL("x%- 5Fx"), TRealX(6.2345678));
	test(aa==_TL("x6.234568x"));
	aa.FormatL(_TL("x%+ 5Fx"), TRealX(6.2345678));
	test(aa==_TL("x6.234568x"));
	aa.FormatL(_TL("x% 5Fx"), TRealX(6.2345678));
	test(aa==_TL("x6.234568x"));
	aa.FormatL(_TL("x%= 5Fx"), TRealX(6.2345678));
	test(aa==_TL("x6.234568x"));
	aa.FormatL(_TL("x%- 10Fx"), TRealX(6.2345));
	test(aa==_TL("x6.234500  x"));
	aa.FormatL(_TL("x%+ 10Fx"), TRealX(6.2345));
	test(aa==_TL("x  6.234500x"));
	aa.FormatL(_TL("x% 10Fx"), TRealX(6.2345));
	test(aa==_TL("x  6.234500x"));
	aa.FormatL(_TL("x%+010Fx"), TRealX(6.2345));
	test(aa==_TL("x006.234500x"));
	aa.FormatL(_TL("x%+10Fx"), TRealX(6.2345));
	test(aa==_TL("x  6.234500x"));
	aa.FormatL(_TL("x%10Fx"), TRealX(6.2345));
	test(aa==_TL("x  6.234500x"));
	aa.FormatL(_TL("x%010Fx"), TRealX(6.2345));
	test(aa==_TL("x006.234500x"));
	aa.FormatL(_TL("x%= 10Fx"), TRealX(6.2345));
	test(aa==_TL("x 6.234500 x"));
	aa.FormatL(_TL("x%10Fx"), TRealX(12345352342.545));
	test(aa==_TL("x12,345,352,342.545000x"));
	aa.FormatL(_TL("x%20.9Fx"), TRealX(1.0123456789));
	test(aa==_TL("x         1.012345679x"));
	aa.FormatL(_TL("x%5.1Fx"), TRealX(1.99));
	test(aa==_TL("x  2.0x"));
#endif

	aa.FormatL(_TL("x%- 5ex"), 6.2345678);
	test(aa==_TL("x6.234568E+00x"));
	aa.FormatL(_TL("x%+ 5ex"), 6.2345678);
	test(aa==_TL("x6.234568E+00x"));
	aa.FormatL(_TL("x% 5ex"), 6.2345678);
	test(aa==_TL("x6.234568E+00x"));
	aa.FormatL(_TL("x%= 5ex"), 6.2345678);
	test(aa==_TL("x6.234568E+00x"));
	aa.FormatL(_TL("x%- 14ex"), 6.2345);
	test(aa==_TL("x6.234500E+00  x"));
	aa.FormatL(_TL("x%+ 14ex"), 6.2345);
	test(aa==_TL("x  6.234500E+00x"));
	aa.FormatL(_TL("x% 14ex"), 6.2345);
	test(aa==_TL("x  6.234500E+00x"));
	aa.FormatL(_TL("x%= 14ex"), 6.2345);
	test(aa==_TL("x 6.234500E+00 x"));
	aa.FormatL(_TL("x%10ex"), 12345352342.545);
	test(aa==_TL("x1.234535E+10x"));
	aa.FormatL(_TL("x%20.9ex"), 1.0123456789);
	test(aa==_TL("x     1.012345679E+00x"));
	aa.FormatL(_TL("x%5.1ex"), 1.99);
	test(aa==_TL("x2.0E+00x"));
	
	test.End();
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4027
@SYMTestCaseDesc Tests LString AppendFormatL() method
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses the AppendFormatL() method
				to format the string data with and without an overflow handler.
				Verifies that the LString data is formatted as expected when using an
				overflow handler.
@SYMTestExpectedResults LString AppendFormatL() method should format data 
				in the LString as expected with and without an overflow handler
@SYMREQ	10372
*/
template <class T,class S,class DESTEMPLATE>
void testAppendFormatIgnoreOverflowL()
	{
	
	test.Start (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4027"));
	
	test.Next(_L("Test no overflow"));
		
	T buf(1);
	buf.AppendFormatL(_TL("A Descriptor"));	
	test(buf==_TL("A Descriptor"));
	
	test.Next(_L("Test overflow with no conversions"));
	buf.AppendFormatL(_TL("123456789"));
	test(buf==_TL("A Descriptor123456789"));
	
	test.Next(_L("Force overflow with descriptor conversion"));
	buf = _TL("Symbian OS");
	buf.Compress();
	
	T buf2;
	buf2 = _TL(" - This descriptor should cause overflow");
	buf.AppendFormatL(_TL("%S"),&buf2);
	test(buf==_TL("Symbian OS - This descriptor should cause overflow"));

	test.End();

	}
	
#ifndef _DEBUG
#pragma warning( disable : 4702) //Unreachable code
#pragma warning( disable : 4710) //Function not expanded
#endif


void RunTestsL()
	{
	test.Next(_L("class LString8"));
	TestLString<LString8, TText8, TPtrC8> a(0x50);
	a.test_TBufL();

	test.Next(_L("TReal formating with auto LString8 extension "));
	testFormatL<LString8, TText8, TPtrC8>();
		
	test.Next(_L("class LString16"));
	TestLString<LString16, TText,TPtrC> b(0x50);
	b.test_TBufL();

	test.Next(_L("TReal formating with auto LString16 extension "));
	testFormatL<LString16, TText,TPtrC>();
	
	testAppendFormatIgnoreOverflowL<LString8,TText8,TPtrC8>();
	testAppendFormatIgnoreOverflowL<LString16,TText,TPtrC>();
	}

GLDEF_C TInt E32Main()
// Test the TBuf type.
    {
      
	CTrapCleanup* trapHandler=CTrapCleanup::New();
	test(trapHandler!=NULL);
	
	
	test.Title();
 	test.Start(_L("Test Formating Functions"));

 	TRAPD(err, RunTestsL());
 	
 	test.End();
 	
 	delete trapHandler;

    return(err);
    
    }

//#pragma warning( default : 4702)
//#pragma warning( default : 4710)


