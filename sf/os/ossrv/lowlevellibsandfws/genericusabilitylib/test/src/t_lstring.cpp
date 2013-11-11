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
// The test cases below are template based allowing the same code to 
// be used for testing both LString16 and LString8 classes.
// Appropriate LString and buffer types are passed as the template
// parameters in the call to each test case.
// API Information:
// LString16, LString8, LString.
// Details:
// For LString8, LString16 and LString objects:
// - Test the Create and CreateMax methods by verifying the return value of
// KErrNone, the initial length and max length. Perform basic write and read
// operations and verify the results.
// - Test the CreateL and CreateMaxL methods by verifying the return value of 
// KErrNone. Also force a heap error and verify return value of KErrNoMemory.
// - Test the Create(const TDesC_& aDesc) and Create(const TDesCX_ aDesc, 
// TInt aMaxLength) methods by verifying the return value of KErrNone. Verify
// initial length, max length and initialisation.
// - Test the CreateL(const TDesC_& aDesc) and CreateMaxL(const TDesCX_ aDesc, 
// TInt aMaxLength) methods by verifying the return value of KErrNone. Also 
// force a heap error and verify return value of KErrNoMemory.
// - Test the Swap method by creating two initialised objects, calling Swap 
// and confirming the results as expected.
// - Test the Assign method by performing an assign from a variety of sources
// and verifying the results are as expected.
// - Test the ReAlloc method in a variety of scenarios that decrease memory, 
// increase memory and zero-length memory. Verify that the results are as
// expected.
// - Test the ReAllocL by verifying the return value of KErrNone. Also force 
// a heap error and verify return value of KErrNoMemory. Verify that the
// object is the same as before the failed ReAllocL call.
// - Test the CleanupClosePushL method via CleanupStack::PopAndDestroy().
// - Force the CleanupClosePushL to leave to check cleanup of LString.
// Platforms/Drives/Compatibility:
// All 
// Assumptions/Requirement/Pre-requisites:
// Failures and causes:
// Base Port information:
// 
//

#include <e32base.h>
#include <e32math.h>
#include <e32test.h>
#include <euserhl.h>
#include <u32std.h>
#include <f32file.h>
#include <s32mem.h>

LOCAL_D RTest test(_L("T_LSTRING"));

/* This macro generates a TTEXT* from the string passed in.  
 * The TTEXT type is a template parameter defined for the calling function
 * and an appropriate 8 or 16 bit string variant is created by this macro
 */
#undef _TS
#define _TS(a) ((const TTEXT*)RTest::String(sizeof(TTEXT),(TText8*)a,(TText16*)L ## a))

/* This macro generates a CHAR* from the string passed in.  
 * The CHAR type is a template parameter defined for the calling function
 * and an appropriate 8(char)or 16(wchar_t) bit string variant is created by this macro
 */
#undef _CS
#define _CS(a) ((const CHAR*)RTest::String(sizeof(TTEXT),(TText8*)a,(TText16*)L ## a))

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4001
@SYMTestCaseDesc Tests constructors of LString classes
@SYMTestPriority High
@SYMTestActions Creates LString objects using the different 
				constructors. 
@SYMTestExpectedResults All constructors create object object as expected
@SYMREQ	10372
*/
template<class LSTRING, class TBUF, class TTEXT, class HBUF, class CHAR >
LOCAL_C void TestConstructors(LSTRING*)
{
	
	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4001"));
	
	test.Next(_L("LString_(const TDesC& aDes) constructor"));
	const TBUF des (_TS("123456"));
	const LSTRING lStr(des);
	test(lStr.Length() == 6);
	test(lStr.MaxLength() >= 6);
	
	test.Next(_L("LString_(const LString16& aDes) constructor"));
	LSTRING lStr1(lStr);
	test(lStr1.Length() == 6);
	test(lStr1.MaxLength() >= 6);
	
	test.Next(_L("LString_(const TUInt16* aString) constructor"));
	LSTRING lStr2(lStr1.PtrZL());
	test(lStr2.Length() == 6);
	test(lStr2.MaxLength() >= 6);
	
	test.Next(_L("LString_(HBufC_* aHBuf) constructor"));

	HBUF* hBuf = HBUF::NewMax(12);	//Create LString as EBufCPtr
	LSTRING lStr3(hBuf);
	test(lStr3.Length() == 12);
	test(lStr3.MaxLength() >= 12);

	hBuf = HBUF::NewMax(0);
	LSTRING lStr4(hBuf);			//The length of aHBuf is zero
	test(lStr4.Length() == 0);

	hBuf = NULL;				//aHBuf is NULL
	LSTRING lStr5((HBUF*)hBuf);
	test(lStr5.Length() == 0);
	test(lStr5.MaxLength() == 0);
	
	test.Next(_L("LString_(TUInt16* aHeapCell, TInt aLength, TInt aMaxLength) constructor"));
	TTEXT* heap = NULL;

	heap = (TTEXT*)User::Alloc(24*(TInt)sizeof(TTEXT)); //Allocate 48 bytes for 24 long LString16
	LSTRING lStr6(heap, 12,24);
	test(lStr6.Length() == 12);		
	test(lStr6.MaxLength() >= 24);		

	test.Next(_L("LString_(TUint* aHeapCell, TInt aMaxLength ) method"));
	
	heap = (TTEXT*)User::Alloc(24*(TInt)sizeof(TTEXT)); //Allocate 48 bytes for 24 long LString16
	LSTRING lStr7(heap, 24);
	test(lStr7.Length() == 0);		
	test(lStr7.MaxLength() >= 24);		
	
	test.Next(_L("LString_(char/wchar_t * aCharStr) constructor"));
	
	LSTRING lStr8(_CS("0123456789"));
	test(lStr8.Length() == 10);		
	test(lStr8.MaxLength() >= 10);
	
	LSTRING lStr9(_CS("01234567890"));
	test(lStr9.Length() == 11);		
	test(lStr9.MaxLength() >= 11);
}



/**
Tests the following methods:
 - TInt Assign(const LString_& lStr);
 - TInt Assign(TUint* aHeapCell, TInt aMaxLength);
 - TInt Assign(TUint* aHeapCell, TInt aLength, TInt aMaxLength);
 - TInt Assign(HBufC& aHBuf);
 - LString(HBufC_&) constructor.
*/
/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4002
@SYMTestCaseDesc Tests Assign methods of LString
@SYMTestPriority High
@SYMTestActions Creates LString objects and assigns data to the LString using 
				the LString::Assign overloads.
				Checks that the data is assigned as expected.
				Creates LString objects from HBufC* and verifies that object
				is created as expected
@SYMTestExpectedResults LString objects are created and assigned as expected
@SYMREQ	10372
*/
template<class LSTRING, class TBUF, class TTEXT, class HBUF, class RBUF>	
LOCAL_C void TestAssign(LSTRING*)
{

	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4002"));
	
	TBUF des (_TS("123456"));
	LSTRING lStr2(des);
	TTEXT* heap = NULL;
	RBUF buf;

	test.Next(_L("Assign(const LString_& aLString) method"));

	LSTRING lStr;
	lStr.Assign(lStr2);
	// the assignment clears lStr2 so the two strings should be unequal
	test(lStr != lStr2);

	test.Next(_L("Assign(TUint* aHeapCell, TInt aLength, TInt aMaxLength ) method"));

	heap = (TTEXT*)User::Alloc(24*(TInt)sizeof(TTEXT)); //Allocate 48 bytes for 24 long LString16
	lStr.Assign(heap, 12,24);
	test(lStr.Length() == 12);		
	test(lStr.MaxLength() >= 24);		

	heap = NULL;
	lStr.Assign(heap, 0,0);
	test(lStr.Length() == 0);		
	test(lStr.MaxLength() == 0);		
	
	test.Next(_L("Assign(TUint* aHeapCell, TInt aMaxLength ) method"));
	
	heap = (TTEXT*)User::Alloc(24*(TInt)sizeof(TTEXT)); //Allocate 48 bytes for 24 long LString16
	lStr.Assign(heap, 24);
	test(lStr.Length() == 0);		
	test(lStr.MaxLength() >= 24);		

	test.Next(_L("Assign(HBufC_* aHBuf) method"));

	HBUF* hBuf = HBUF::NewMax(11);
	lStr.Assign(hBuf);			//Create LString as EBufCPtr type
	test(lStr.Length() == 11);
	test(lStr.MaxLength() >= 11); //There could me more allocated memory - see HBufC8::Des()
	
	HBUF* hBuf2 = HBUF::NewMax(5);
	lStr = hBuf2;			//Create LString as EBufCPtr type
	test(lStr.Length() == 5);
	test(lStr.MaxLength() >= 5); //There could me more allocated memory - see HBufC8::Des()

	test.Next(_L("Assign(const RBuf_& aRBuf) method"));

	buf.Create(des);
	lStr.Assign(buf);
	
	// the assignment trasnfers buf so the two strings should be equal
	test(lStr == des);
	test(lStr != buf);
	
}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4003
@SYMTestCaseDesc Tests AppendL methods of LString
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses AppendL methods to append 
				data to the LString.
				Checks that the data is appended as expected.
@SYMTestExpectedResults LString objects are created and data appended as 
				expected with the strings grouwing as necessary to accomodate 
				the new data.
@SYMREQ	10372
*/
template<class LSTRING, class TBUF, class TTEXT, class CHAR>
LOCAL_C void TestAppendL(LSTRING*)
{
	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4003"));

	LSTRING lStr;
	LSTRING lStr2;
	TBUF des(_TS("123456"));
	TBUF des2(_TS("123456A"));
	

	test.Next(_L("AppendL(const TDesc_& aDes) method"));

	lStr.AppendL(des);
	test(lStr == des);
	
	test.Next(_L("AppendL(const TUint16* aBuf, TInt aLength) method"));

	lStr2 = des;
	lStr.Reset();
	lStr.AppendL(lStr2.PtrZL(),lStr2.Length());
	test(lStr == des);
	
	test.Next(_L("AppendL(TChar aChar) method"));

	TChar c = 'A';
	lStr = des;
	lStr.AppendL(c);
	test(lStr == des2);
	
	test.Next(_L("ZeroTerminateL() method"));

	lStr = des;
	lStr.ZeroTerminateL();
	test(lStr == des);
	
	test.Next(_L("AppendL(char/wchar_t* aCharStr) method"));
	
	LSTRING lStr3(_CS("0123456789"));
	test(lStr3.Length() == 10);		
	test(lStr3.MaxLength() >= 10);
	
	LSTRING lStr4(_CS("01234567890"));
	test(lStr4.Length() == 11);		
	test(lStr4.MaxLength() >= 11);

	lStr3.AppendL(_CS("0"),1);
	test(lStr3 == lStr4);
	
	lStr4.AppendL(_CS("0123456789"),10);
	test(lStr4.Length() == 21);		
	test(lStr4.MaxLength() >= 21);
	
	test.Next(_L("operator + (char/wchar_t* aCharStr) method"));
	
	LSTRING lStr5(_CS("0123456789"));
	test(lStr5.Length() == 10);		
	test(lStr5.MaxLength() >= 10);
	
	LSTRING lStr6(_CS("01234567890"));
	test(lStr6.Length() == 11);		
	test(lStr6.MaxLength() >= 11);

	lStr5+=_CS("0");
	test(lStr5 == lStr6);
	
	lStr6 +=_CS("0123456789");
	test(lStr6.Length() == 21);		
	test(lStr6.MaxLength() >= 21);
	
}

/**
Tests the following methods. 
 - TInt ReAlloc(TInt aMaxLength);
*/
/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4004 
@SYMTestCaseDesc Tests ReAlloc methods of LString
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses ReAlloc to decrease,
				increase and zero memory. Validates that length and 
				maxlength are changed as expected 
@SYMTestExpectedResults ReAlloc should change the length and maxLength 
				of the LString data as expected
@SYMREQ	10372
*/
template<class LSTRING, class TBUF, class TTEXT, class HBUF>	
LOCAL_C void TestReAllocL(LSTRING*)
{
	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4004"));

	test.Next(_L("ReAlloc(TInt aMaxLength) method"));

	TBUF des (_TS("0123456"));
	
	//reallocate EPtr type - decrease memory
	LSTRING lStr(des);
	lStr.SetLengthL(3);
	test(lStr.ReAlloc(3)==KErrNone);					//ReAlloc to EPtr
	test(lStr.MaxLength()>=3);
	test(lStr.Length()==3);
	test(lStr[0] == (TTEXT)('0'));
	test(lStr[2] == (TTEXT)('2'));

	//reallocate EBufCPtr type - decrease memory
	HBUF* hBuf = HBUF::NewMax(9);
	*hBuf = _TS("012345678");
	lStr.Assign(hBuf);						//Create as EBufCPtr
	lStr.SetLengthL(5);
	test(lStr.ReAlloc(5)==KErrNone);		//ReAlloc to EBufCPtr
	test(lStr.MaxLength()>=5);//There could be more allocated memory - see HBufC8::Des()
	test(lStr.Length()==5);
	test(lStr[0] == (TTEXT)('0'));
	test(lStr[4] == (TTEXT)('4'));

	//reallocate EBufCPtr type - increase memory
	hBuf = HBUF::NewMax(9);
	*hBuf = _TS("012345678");
	lStr.Assign(hBuf);						//Create as EBufCPtr
	test(lStr.ReAlloc(15)==KErrNone);		//ReAlloc to EBufCPtr
	test(lStr.MaxLength()>=15);//There could be more allocated memory - see HBufC8::Des()
	test(lStr.Length()==9);
	test(lStr[0] == (TTEXT)('0'));
	test(lStr[8] == (TTEXT)('8'));

	//reallocate EPtr type - to zero-length
	lStr = des;
	lStr.SetLengthL(0);
	test(lStr.ReAlloc(0)==KErrNone);		//ReAlloc to EPtr
	test(lStr.MaxLength()==0);
	test(lStr.Length()==0);

	//reallocate EBufCPtr type to zero-length
	hBuf = HBUF::NewMax(9);
	*hBuf = _TS("012345678");
	lStr.Assign(hBuf);						//Create as EBufCPtr
	lStr.SetLengthL(0);
	test(lStr.ReAlloc(0)==KErrNone);		//ReAlloc to EPtr
	test(lStr.MaxLength()==0);
	test(lStr.Length()==0);

	//reallocate from zero-length
	lStr.Reset();						
	test(lStr.ReAlloc(9)==KErrNone);		//ReAlloc to EPtr
	test(lStr.MaxLength() >=9);
	test(lStr.Length()==0);

	//reallocate from zero-length to zero-length
	lStr.Reset();							
	test(lStr.ReAlloc(0)==KErrNone);		//ReAlloc to EPtr
	test(lStr.Length() == 0);		
	test(lStr.MaxLength() == 0);		
}

/**
Tests the following methods. 
 - TInt ReAllocL(TInt aMaxLength);
*/
/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4005
@SYMTestCaseDesc Tests Leaving variant ReAllocL of LString
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses ReAllocL to 
				increase memory under OOM conditions
				Verifies that ReAllocL leaves with KErrNoMemory
@SYMTestExpectedResults Calls to ReAllocL that increase memory should 
				leave with KErrNoMemory under OOM conditions
@SYMREQ	10372
*/
template<class LSTRING, class TBUF, class TTEXT>	
LOCAL_C void TestReAllocLeaving(LSTRING*)
{

	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4005"));
	
	test.Next(_L("ReAllocL(TInt aMaxLength) method"));
	TBUF des(_TS("01"));

	LSTRING lStr(des);
	TRAPD(ret, lStr.ReAllocL(6));	//ReAlloc buffer
	test(KErrNone == ret);

#if defined(_DEBUG)
	__UHEAP_FAILNEXT(1);
	TRAP(ret, lStr.ReAllocL(100));	//Realloc buffer. This should fail.
	test(KErrNoMemory == ret);
#endif //(_DEBUG)

	test(lStr.MaxLength() >=6);		//Check LString is the same as before ... 
	test(lStr.Length()==2);			//... ReAlloc that failed.
	test(lStr[0] == (TTEXT)('0'));
	test(lStr[1] == (TTEXT)('1'));
}


/**
Tests the following methods. 
 - void SwapL(LString& aBuf);
 - void SwapL(TDes& aBuf);
*/
/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4006
@SYMTestCaseDesc Tests SwapL methods of LString
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses SwapL to 
				swap string contents.
				Verifies that contents are swapped and memory 
				reallocated where necessary
@SYMTestExpectedResults Calls to SwapL should swap string contents 
				and buffer should be automatically reallocated to fit the new data.
@SYMREQ	10372
*/
template<class LSTRING, class TBUF, class TTEXT>
LOCAL_C void TestSwapL(LSTRING*)
{
	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4006"));

	LSTRING lStr1, lStr2;
	TBUF des1(_TS("12"));
	TBUF des2 (_TS("345678"));
	TBUF des3 (_TS("12345678"));
	TBUF swap_des = des2;

	test.Next(_L("Swap(LString_& aLString) method"));

	// assignment operation; implies deep copying
	lStr1 = des1;
	lStr2 = des2;

	// swap LStrings with TDes
	// lStr1 should grow to accommodate swap_des
	lStr1.SwapL(swap_des);
	test(lStr1==des2);
	test(swap_des==des1);

	// swap two LStrings
	// lStr2 should grow to accommodate lStr1
	lStr1 = des3;
	lStr1.SwapL(lStr2);

	test(lStr1==des2);
	test(lStr2==des3);

}


/**
Test assignemnt operator.
*/
/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4007
@SYMTestCaseDesc Tests Assignment operator for LString
@SYMTestPriority High
@SYMTestActions Creates LString objects and uses assignment to 
				change string contents.
				Verifies that contents are swapped and memory 
				reallocated where necessary
@SYMTestExpectedResults Assignment operator should change string contents
				and buffer should be automatically reallocated to fit the new data.
@SYMREQ	10372
*/
template<class LSTRING, class TBUF, class TBUFC, class TTEXT, class CHAR>
LOCAL_C void TestAssignmentOperatorL()
{
	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4007"));	

	test.Next(_L("Assignment operator"));

	TBUF tdes(_TS("Modifiable descriptor"));
	TBUFC tdesc(_TS("Non-modifiable descriptor"));

	LSTRING lStr(32);
	LSTRING lStr2(32);
	lStr2.CopyL(_TS("Buffer descriptor"), 17);

	lStr = tdesc;	test(lStr == tdesc);
	lStr = tdes;	test(lStr == tdes);
	lStr = lStr2;	test(lStr == lStr2);
	
	LSTRING lStr3(tdes);
	lStr = lStr3.PtrZL(); test(lStr == tdes);
	
	test.Next(_L("operator=(char/wchar_t* aCharStr) method"));
	LSTRING lStr4(_CS("123456"));
	LSTRING lStr5 = _CS("123456");
	test(lStr4 == lStr5);
	
	LSTRING lStr6;
	lStr6 =  _CS("123456");
	test(lStr4 == lStr6);
	}

/** 
Test Capacity growth and compression
*/
/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4008
@SYMTestCaseDesc Tests capacity growth and compression for LString
@SYMTestPriority High
@SYMTestActions Creates an LString object then calls SetMaxLength and Compress
				to increase or decrease underlying buffer size.
				Tests that Length and Maxlength are modified as expected 
@SYMTestExpectedResults SetMaxLength and Compress should  resize the underlying
				buffer as expected.
@SYMREQ	10372
*/
template<class LSTRING, class TBUF, class TTEXT> 
LOCAL_C void TestCapacityChangesL(LSTRING*)
{
	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4008"));
	
	test.Next(_L("Test capacity growth and compression on LString"));

	LSTRING lStr(_TS("0123456"));
	test(lStr.Length()==7);
	test(lStr.MaxLength() >=7);

 	lStr.SetMaxLengthL(10);
	test(lStr.Length()==7);
	test(lStr.MaxLength() >=10);

 	lStr.SetMaxLengthL(6);
	test(lStr.Length()==6);
	test(lStr.MaxLength() >=6);
 	
 	lStr.SetMaxLengthL(10);
	test(lStr.Length()==6);
	test(lStr.MaxLength() >=10);
	
	//Call the same thing again to check the condition
	//that required length is already set
 	lStr.SetMaxLengthL(10);
	test(lStr.Length()==6);
	test(lStr.MaxLength() >=10);

 	lStr.Compress();
 	test(lStr.Length()==6);
	test(lStr.MaxLength() >= 6);
	
	//Call the same thing again to check the condition
	//that the string is already compressed
 	lStr.Compress();
 	test(lStr.Length()==6);
	test(lStr.MaxLength() >= 6);
	
	lStr.ReserveFreeCapacityL(15);
 	test(lStr.Length()==6);
	test(lStr.MaxLength() >= 32);
	
	lStr.Reset();
 	test(lStr.Length()==0);
	test(lStr.MaxLength() == 0);

}


/** 
Test copying from 16bit to 8bit and vice versa
*/
/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4009
@SYMTestCaseDesc Tests Copying between 8 and 16 bit LString variants
@SYMTestPriority High
@SYMTestActions Creates an LString8 and LString16 object and uses the cross-variant 
				Copy functions to copy data between the variants.
@SYMTestExpectedResults Data is successfully copied between the variants.
@SYMREQ	10372
*/
LOCAL_C void TestCrossCopyingL()
	{
	
	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4009"));
	
	test.Next(_L("Test cross copying on LString"));
	
	LString8 lStr8("0123");
	LString16 lStr16(L"01234567");

	LString8 targetStr8;
	LString16 targetStr16;
	
	targetStr8.CopyL(lStr16);
	targetStr16.CopyL(lStr8);
	}	


/** 
Test creating an LString from a stream
*/
/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4010
@SYMTestCaseDesc Tests Creating an LString from a stream
@SYMTestPriority High
@SYMTestActions Creates a in-memory stream and writes some data to it.
				Creates an LString from a readstream and verifies that the
				contents of the LString match the data written to the stream.
@SYMTestExpectedResults The LString contents should match the data written 
				to the stream.
@SYMREQ	10372
*/
template<class LSTRING, class TTEXT>
LOCAL_C void TestReadFromStreamL()
	{
	test.Next (_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4010"));
	
	test.Next(_L("Test creating LString from a stream"));
	
	RFs fs;
	fs.Connect() OR_LEAVE;
	
	LSTRING outString = _TS("This is a test string written to a stream");
	
	//Create a buffer to contain the stream
	CBufFlat* buf = CBufFlat::NewL(outString.MaxLength());
	
	//Create a write stream
	RBufWriteStream outStream;
	outStream.Open(*buf);
	
	//write some data to the stream
	outStream << outString;
	outStream.CommitL();
	
	//Open a readstream
	RBufReadStream inStream;	
	inStream.Open(*buf);
	
	//Create an LString from the stream
	LSTRING inString;
	inString.CreateL(inStream,outString.Length());	
	test(inString == outString);
	
	delete buf;	
	}

/** 
Test support for [wide]character strings.
APIs that modify data.
*/
/**
@SYMTestCaseID BASESRVCS-EUSERHL-UT-4068
@SYMTestCaseDesc Test the APIs provided to support wchar_t and char strings
@SYMTestPriority High
@SYMTestActions 1)Construct LString object from the supplied null terminated 
				character string.
				2)Assign new string to the constructed LString object
				3)Appends data onto the end of this LString object's data.
				The length of this descriptor is incremented to reflect the new content.
				4)Copy new data into the LString object, replacing any existing
				data, and expanding its heap buffer to accommodate if necessary.
				5)Insert contents into the LString
				6)Replace data to the end of the LString object and justify it.
				7)Append data of specified length, to the end of the LString object.
				8)Justify data, to the end of the LString object.
				9)Appends data onto the end of this descriptor's data and justifies it.
@SYMTestExpectedResults The LString contents should match the data expected after the operation.
@SYMREQ	10372
*/

template<class LSTRING, class TTEXT, class CHAR>
LOCAL_C void TestCharacterStringSupport_Modifiers(LSTRING*)
	{
	test.Next (_L ("@SYMTestCaseID:BASESRVCS-EUSERHL-UT-4068"));
	
	// 1. test Constructor 
	// Constructs LString object from the supplied null terminated 
	// character string
	test.Next(_L("LString_(char/wchar_t * aCharStr) constructor"));
	LSTRING lStr(_CS("0123456789"));
	test(lStr.Length() == 10);		
	test(lStr.MaxLength() >= 10);
	test(lStr.Compare(_CS("0123456789")) == 0 );
	// try strings ending with 0
	LSTRING lStr1(_CS("01234567890"));
	test(lStr1.Length() == 11);		
	test(lStr1.MaxLength() >= 11);
	test(lStr1.Compare(_CS("01234567890")) == 0 );
	
	// 2. test '=' operator
	LSTRING lTestStr;
	// Assign new string to the constructed LString object
	test.Next(_L("LString_ operator '=' "));
	lTestStr = _CS("Try a New String");
	test(lTestStr.Compare(_CS("Try a New String")) == 0 );
	test(lTestStr.Length() == 16);		
	test(lTestStr.MaxLength() >= 16);
	
	// 3. test '+=' operator
	// Appends data onto the end of this LString object's data.
	// The length of this descriptor is incremented to reflect the new content.
	test.Next(_L("LString_ operator '+=' "));
	lTestStr += _CS("!!!");
	test(lTestStr.Compare(_CS("Try a New String!!!")) == 0 );
	test(lTestStr.Length() == 19);		
	test(lTestStr.MaxLength() >= 19);
		
	// 4.Test "Copy()" Variants
	LSTRING lTestStr1;
	LSTRING lTestStr2;
	// Copy new data into the LString object, replacing any existing
	// data, and expanding its heap buffer to accommodate if necessary.
	test.Next(_L("LString_ CopyL "));
	lTestStr1.CopyL(_TS("Try a New String"));
	test(lTestStr1.Compare(_TS("Try a New String")) == 0 );
	test(lTestStr1.Length() == 16);		
	test(lTestStr1.MaxLength() >= 16);
	
	// Copy folded(normalized) content
	test.Next(_L("LString_ CopyFL "));
	lTestStr1.CopyFL(_CS("Some RaNDom STRING"));
	lTestStr2.CopyFL(_CS("SOME RaNDom string"));
	test(lTestStr1.Compare(lTestStr2) == 0);
	
	// Copy contents in Lower case
	test.Next(_L("LString_ CopyLCL "));
	lTestStr1.CopyLCL(_CS("SOME STRING IN UPPER CASE"));
	test(lTestStr1 == _CS("some string in upper case"));

	// Copy contents in Upper case
	test.Next(_L("LString_ CopyUCL "));
	lTestStr1.CopyUCL(_CS("some string in lower case"));
	test(lTestStr1 == _CS("SOME STRING IN LOWER CASE"));
		
	// Copy Capitalized contents
	test.Next(_L("LString_ CopyCPL "));
	lTestStr1.CopyCPL(_CS("some string in lower case"));
	test(lTestStr1 == _CS("Some string in lower case"));
			
	// 5. Test Insert()
	LSTRING lTestStr3;
	// Insert contents into a string
	test.Next(_L("LString_ InsertL "));
	lTestStr3 = _CS("Some Content Can Be Into This String");
	lTestStr3.InsertL(20,_CS("Inserted "));
	test(lTestStr3 == _CS("Some Content Can Be Inserted Into This String"));
	
	// 6. Test Replace()
	LSTRING lTestStr4;
	// Replace contents form the string
	test.Next(_L("LString_ ReplaceL "));
	lTestStr4 = _CS("Some Content Can Be Decalper");
	lTestStr4.ReplaceL(20,8,_CS("Replaced"));
	test(lTestStr4 == _CS("Some Content Can Be Replaced"));
	
	// 7. Test Append()
	LSTRING lTestStr5;
	//Append data of specified length, to the end of the LString object.	
	test.Next(_L("LString_ AppendL(src,length)"));
	lTestStr5.CopyL( _CS("Try appending "));
	lTestStr5.AppendL(_CS("Try appending some more"),3);
	test(lTestStr5 == _CS("Try appending Try"));
		
	//Append data , to the end of the LString object.	
	test.Next(_L("LString_ AppendL(src)"));
	lTestStr5.CopyL( _CS("Try appending "));
	lTestStr5.AppendL(_CS("Try appending some more"));
	test(lTestStr5 == _CS("Try appending Try appending some more"));
		
	// 8. Test Justify()
	LSTRING lTestStr6;
	//Copy data into this descriptor and justifies it, replacing any existing data	
	test.Next(_L("LString_ JustifyL "));
	lTestStr6.CopyL(_CS("Justified"));
	lTestStr6.JustifyL(_CS("Just"),9,ERight,*(_TS("x")));
	test(lTestStr6 == _CS("xxxxxJust"));
	
	// 9. Test AppendJustify variants
	LSTRING lTestStr7;
	// Append data to the end of the LString object and justify it.
	test.Next(_L("LString_ AppendJustifyL(const char*,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill)"));
	lTestStr7.CopyL(_CS("One "));
	lTestStr7.AppendJustifyL(_CS("Two "),KDefaultJustifyWidth,ERight,*(_TS("x")));
	test(lTestStr7 == _TS("One Two "));
	
	lTestStr7.CopyL(_CS("One "));
	lTestStr7.AppendJustifyL(_CS("Two Three"),3,7,ERight,*(_TS("x")));
	test(lTestStr7 == _CS("One xxxxTwo") );
	
	// Append data to the end of the LString object and justify it.
	test.Next(_L("LString_ AppendJustifyL(const char* aCharStr,TInt aWidth,TAlign anAlignment,TChar aFill)"));
	lTestStr7.CopyL(_CS("One "));
	lTestStr7.AppendJustifyL(_CS("Two Three"),KDefaultJustifyWidth,ERight,*(_TS("x")));
	test(lTestStr7 == _TS("One Two Three"));
	
	lTestStr7.CopyL(_CS("One "));
	lTestStr7.AppendJustifyL(_CS("Two Three"),13,ERight,*(_TS("x")));
	test(lTestStr7 == _CS("One xxxxTwo Three") );
	}

/** 
Test support for [wide]character strings.
APIs that do not modify any data.
*/
/**
@SYMTestCaseID BASESRVCS-EUSERHL-UT-4069
@SYMTestCaseDesc Test the APIs provided to support wchar_t and char strings
@SYMTestPriority High
@SYMTestActions 1)Determine whether this descriptor's data is equal to the specified string's data.
				2)Determine whether this descriptor's data is less than the specified string's data.
				3)Determine whether this descriptor's data is less than or equal to the specified string's data.
				4)Determine whether this descriptor's data is greater than the specified string's data.
				5)Determine whether this descriptor's data is greater than or equal to the specified string's data.
				6)Determine whether this descriptor's data is not equal to the specified string's data.
				7)Compare this descriptor's data with the specified string's data.
				8)Search this descriptor's data for a match with the match pattern supplied in the specified string's
				9)Searches for the first occurrence of the specified data sequence within this descriptor
@SYMTestExpectedResults The operation/comparision  must result in the desired output
@SYMREQ	10372
*/
template<class LSTRING,class TTEXT, class CHAR>
LOCAL_C void TestCharacterStringSupport_NonModifiers(LSTRING*)
	{
	test.Next (_L ("@SYMTestCaseID:BASESRVCS-EUSERHL-UT-4069"));
	
	// 1.test '==' operator
	LSTRING lTestStr1;
	lTestStr1.CopyL(_CS("Are they equal?? "));
	test(lTestStr1 == _CS("Are they equal?? "));
	lTestStr1.CopyL(_CS("12345670"));
	test(lTestStr1 == _CS("12345670"));
	
	// 2.test "<" operator
	LSTRING lTestStr2;
	lTestStr2.CopyL(_CS("ABCDEFGH"));
	test(lTestStr2 < _CS("abcdefgh"));
	lTestStr2.CopyL(_CS(" Is this is smaller"));
	test(lTestStr2 < _CS("No, larger of the string is greater than the smaller one"));
	
	// 3.test "<=" operator
	LSTRING lTestStr3;
	lTestStr3.CopyL(_CS("ABCDEFGH"));
	test(lTestStr3 <= _CS("abcdefgh"));
	lTestStr3.CopyL(_CS("equals"));
	test(lTestStr3 <= _CS("equals"));
	
	// 4.test ">" operator
	LSTRING lTestStr4;
	lTestStr4.CopyL(_CS("abcdefgh"));
	test(lTestStr4 > _CS("ABCDEFGH"));
	lTestStr4.CopyL(_CS("No, larger of the string is greater than the smaller one"));
	test(lTestStr4 > _CS("Is this smaller??"));
	
	// 5.test ">=" operator
	LSTRING lTestStr5;
	lTestStr5.CopyL(_CS("abcdefgh"));
	test(lTestStr5 >= _CS("ABCDEFGH"));
	lTestStr5.CopyL(_CS("equals"));
	test(lTestStr5 >= _CS("equals"));
	
	// 6.test "!="
	LSTRING lTestStr6;
	lTestStr6.CopyL(_CS("abcdefgh"));
	test(lTestStr6 != _CS("ABCDEFGH"));
	lTestStr6.CopyL(_CS("equals"));
	test(!(lTestStr6 != _CS("equals")));
	
	// 7.test Compare variants
	LSTRING lTestStr7;
	// Compare strict
	lTestStr7.CopyL(_CS("abcdefgh"));
	test(lTestStr7.Compare(_CS("ABCDEFGH")) > 0);	
	lTestStr7.CopyL(_CS("ABCDEFGH"));
	test(lTestStr7.Compare(_CS("abcdefgh")) < 0);
	lTestStr5.CopyL(_CS("equals"));
	test(lTestStr5.Compare( _CS("equals")) == 0);
	// Compare foalded
	lTestStr7.CopyL(_CS("abcdefgh"));
	test(lTestStr7.CompareF(_CS("ABcDeFgH")) == 0);	
	
	// 8.test Match variants
	LSTRING lTestStr8;
	// Match strict
	lTestStr8.CopyL(_CS("abcdefghijklmnopqrstuvwxyz"));
	test(lTestStr8.Match(_CS("*ijk*"))== 8);
	test(lTestStr8.Match(_CS("*i?k*"))== 8);
	test(lTestStr8.Match(_CS("ijk*"))== KErrNotFound);
	// Match Folded
	test(lTestStr8.MatchF(_CS("*IjK*"))== 8);
	test(lTestStr8.MatchF(_CS("*I?k*"))== 8);
	test(lTestStr8.MatchF(_CS("ijK*"))== KErrNotFound);
	
	// 9.test Find variants
	LSTRING lTestStr9;
	// Find strict
	lTestStr9.CopyL(_CS("abcdefghijklmnopqrstuvwxyz"));
	test(lTestStr9.Find(_CS("abcde")) == 0);
	test(lTestStr9.Find(_CS("cde")) == 2);
	test(lTestStr9.Find(_CS("efg22")) == KErrNotFound);
	test(lTestStr9.Find(_CS("efg22"),3) == 4);
	// Find folded
	test(lTestStr9.FindF(_CS("aBcDe")) == 0);
	test(lTestStr9.FindF(_CS("cDe")) == 2);
	test(lTestStr9.FindF(_CS("eFg22")) == KErrNotFound);
	test(lTestStr9.FindF(_CS("efG22"),3) == 4);
	}

void RunTestsL()
	{
	
	LString8* r8=0;
	LString16* r16=0;

	// LString8 Tests
	TestConstructors<LString8,TBuf8<11>,TText8, HBufC8, char>(r8);
	TestSwapL<LString8,TBuf8<11>,TText8>(r8);
	TestAssign<LString8,TBuf8<11>,TText8,HBufC8,RBuf8>(r8);
	TestAppendL<LString8,TBuf8<11>,TText8, char>(r8);
	TestReAllocL<LString8,TBuf8<11>,TText8,HBufC8>(r8);
	TestReAllocLeaving<LString8,TBuf8<11>,TText8>(r8);
	TestAssignmentOperatorL<LString8,TBuf8<32>,TBufC8<32>,TText8, char>();
	TestCapacityChangesL<LString8,TBuf8<11>,TText8>(r8);
	TestReadFromStreamL<LString8,TText8>();
	TestCharacterStringSupport_Modifiers<LString8,TText8, char>(r8);
	TestCharacterStringSupport_NonModifiers<LString8,TText8, char>(r8);
	
	// LString16 Tests
	TestConstructors<LString16,TBuf16<11>,TText16,HBufC16, wchar_t>(r16);
	TestSwapL<LString16,TBuf16<11>,TText16>(r16);
	TestAssign<LString16,TBuf16<11>,TText16,HBufC16,RBuf16>(r16);
	TestAppendL<LString16,TBuf16<11>, TText16,wchar_t>(r16);
	TestReAllocL<LString16,TBuf16<11>,TText16,HBufC16>(r16);
	TestReAllocLeaving<LString16,TBuf16<11>,TText16>(r16);
	TestAssignmentOperatorL<LString16,TBuf16<32>,TBufC16<32>,TText16,wchar_t>();
	TestCapacityChangesL<LString16,TBuf16<11>,TText16>(r16);
	TestReadFromStreamL<LString16,TText16>();
	TestCharacterStringSupport_Modifiers<LString16,TText16, wchar_t>(r16);
	TestCharacterStringSupport_NonModifiers<LString16,TText16, wchar_t>(r16);
	TestCrossCopyingL();
	}




/** 
Test arithmetric operator overloads
*/


GLDEF_C TInt E32Main()
    {

	CTrapCleanup* trapHandler=CTrapCleanup::New();
	test(trapHandler!=NULL);

	test.Title();
	test.Start(_L("Testing LString8 & LString16 classes"));

	__UHEAP_MARK;

	TRAPD(err, RunTestsL());
	
	__UHEAP_MARKEND;

	test.End();

	delete trapHandler;
	return(err);
    }

