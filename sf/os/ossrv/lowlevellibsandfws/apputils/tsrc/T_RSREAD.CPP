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
// Tests resource reader
// NB, prior to version 050 of BAFL, the resource compiler did not generate
// unicode text and so this code used to make a special 8/16 bit text
// comparison where appropriate and the resource binary file could be common
// between the 8 and 16-bit builds.  This is now no-longer the case, which means
// that the resource binary used for this text MUST be compiled as Unicode
// text for the unicode build and 8-bit text for the 8-bit build.
// - PNJ, January 1997.
// 
//

#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <f32file.h>
#include <barsc.h>
#include <barsread.h>
#include <barsread2.h>
#include <badesca.h>
#include <trsc.rsg>
#include "T_RSC.H"

RTest test(_L("T_RSREAD"));
RFs TheFs;
RResourceFile TheResourceFile;

_LIT(KPanicThread1,"panicThread1");
_LIT(KPanicThread2,"panicThread2");
_LIT(KPanicThread3,"panicThread3");
_LIT(KPanicThread4,"panicThread4");

class TRsReadTester
	{
public:
	void TestButtonL();
	void TestArrayL();
	void TestFlPtEdL();
	void TestMenuBarL();
    void TestAlignment1L();
    void TestAlignment2L();
    void TimingTestsL();
    TInt PanicTests();
private:
    void DumpBytes(const TAny* aPtr,TInt aLen);
    TUint8 DumpByte(TUint aVal);
    void CreateResourceReaderLC(TResourceReader& aReader,TInt aResourceId) const;
    void PanicTResImp1();
    void PanicTResImp2();
    void PanicTResImp3();
    void PanicTResImp4();
	};

void TRsReadTester::CreateResourceReaderLC(TResourceReader& aReader,TInt aResourceId) const
    {
    HBufC8* resource=TheResourceFile.AllocReadLC(aResourceId);
    aReader.SetBuffer(resource);
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0433
@SYMTestCaseDesc        Tests for TResourceReader::ReadInt16(),TResourceReader::ReadUint16() functions
@SYMTestPriority        Medium
@SYMTestActions         Attempt to read a BUTTON resource
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestButtonL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0433 Test reading BUTTON resource "));
    TResourceReader reader;
    CreateResourceReaderLC(reader,SYS_BUTTON_ONE);
    test(reader.ReadInt16()==3);
    test(reader.ReadUint16()==5);
    TPtrC ptr=reader.ReadTPtrC();
    test(ptr==_L("Text"));
    ptr.Set(reader.ReadTPtrC());
    test(ptr.Length()==0);
    HBufC* hBuf=reader.ReadHBufCL();
    test(*hBuf==_L("Bitmap placeholder"));
    delete(hBuf);
    CleanupStack::PopAndDestroy();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0434
@SYMTestCaseDesc        Tests for TResourceReader::ReadDesCArrayL() function
@SYMTestPriority        Medium
@SYMTestActions         Attempt to read an array resource
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestArrayL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0434 Test reading ARRAY resource "));
    TResourceReader reader;
    CreateResourceReaderLC(reader,SYS_ARRAY_ONE);
    CDesCArray* desArray=reader.ReadDesCArrayL();
    CleanupStack::PopAndDestroy();
    test(desArray->Count()==5);

    test((*desArray)[0]==_L("Esc"));
    test((*desArray)[1]==_L("Enter"));
    test((*desArray)[2]==_L("Tab"));
    test((*desArray)[3]==_L("Del"));
    test((*desArray)[4]==_L("Space"));

    delete(desArray);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0435
@SYMTestCaseDesc        Tests for TResourceReader::ReadInt16(),TResourceReader::ReadInt64() function
@SYMTestPriority        High
@SYMTestActions         Attempt for reading FLPTED resource
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestFlPtEdL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0435 Test reading FLPTED resource "));
    TResourceReader reader;
    CreateResourceReaderLC(reader,SYS_FLPTED_ONE);
    test(reader.ReadInt16()==18);
    TReal little=reader.ReadReal64();
    test(little==0.0);
    TReal big=reader.ReadReal64();
    test(big>9.89e99);
    test(big<9.91e99);
    CleanupStack::PopAndDestroy(1);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0436
@SYMTestCaseDesc        TResourceReader class functionality test
						Test for TResourceReader::ReadInt32 (),TResourceReader::ReadTPtrC() functions
@SYMTestPriority        High
@SYMTestActions         Attempt for reading MENU_BAR resource
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestMenuBarL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0436 Test reading MENU_BAR resource "));
    TResourceReader reader;
    CreateResourceReaderLC(reader,SYS_MENUBAR_ONE);
    test(reader.ReadInt16()==8); // how many items following
    TPtrC ref=_L("abcdefgh");
    for (TInt ii=1; ii<=8; ii++)
        {
        test(reader.ReadInt32()==ii);
        test(reader.ReadTPtrC()==ref.Left(ii));
        }
    CleanupStack::PopAndDestroy(1);
	}

TInt horrors[] = {
	SYS_ALIGNMENT_HORROR0, SYS_ALIGNMENT_HORROR1,
	SYS_ALIGNMENT_HORROR2, SYS_ALIGNMENT_HORROR3,
	0
	};

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0437
@SYMTestCaseDesc        Reading ALIGNMENT_HORROR resources test
                        Test for TResourceReader::ReadTPtrC8(),TResourceReader::ReadTPtrC16() function
@SYMTestPriority        High
@SYMTestActions         Tests for reading descriptors and checks for alignment
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestAlignment1L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0437 Test reading ALIGNMENT_HORROR resources "));
	TPtrC8  ref8 =_L8("xyz");
	TPtrC16 ref16=_L16("xyz");
    TResourceReader reader;
	for (TInt rr=0; horrors[rr]!=0; rr++)
		{
		CreateResourceReaderLC(reader,horrors[rr]);
		test(reader.ReadTPtrC8() ==ref8.Left(rr));
		test(reader.ReadTPtrC16()==ref16.Left(rr));
		CleanupStack::PopAndDestroy(1);
		}
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0438
@SYMTestCaseDesc        Reading ALIGNMENT_HORROR_ARRAY resource test
@SYMTestPriority        High
@SYMTestActions         Tests for reading descriptors and checks for alignment
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestAlignment2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0438 Test reading ALIGNMENT_HORROR_ARRAY resource "));
    TResourceReader reader;
    CreateResourceReaderLC(reader,SYS_ALIGNMENT_HORROR_ARRAY);
    test(reader.ReadInt16()==7); // how many items following
    TPtrC8  ref8 =_L8("abcdef");
    TPtrC16 ref16=_L16("abcdef");
    for (TInt ii=0; ii<=6; ii++)
        {
        test(reader.ReadTPtrC8() ==ref8.Left(ii));
        test(reader.ReadTPtrC16()==ref16.Mid(ii));
        }
    CleanupStack::PopAndDestroy(1);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0439
@SYMTestCaseDesc        Timing Tests
@SYMTestPriority        High
@SYMTestActions         Check for the time loads needed to repeatedly load a resource
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TimingTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0439 Timing tests "));
	test.Next(_L("BUTTON and FLPTED"));
	TUint time = User::TickCount();
	for (TInt ii=0; ii<100; ii++)
		{
    	TResourceReader reader;
    	CreateResourceReaderLC(reader,SYS_BUTTON_ONE);
	    CleanupStack::PopAndDestroy();
	    CreateResourceReaderLC(reader,SYS_FLPTED_ONE);
    	CleanupStack::PopAndDestroy();
		}
	time = User::TickCount() - time;
    test.Printf(_L("Time to load 100 times: %d\n"),time);

	test.Next(_L("BUTTON and ARRAY"));
	time = User::TickCount();
	for (TInt jj=0; jj<100; jj++)
		{
    	TResourceReader reader;
    	CreateResourceReaderLC(reader,SYS_BUTTON_ONE);
	    CleanupStack::PopAndDestroy();
	    CreateResourceReaderLC(reader,SYS_ARRAY_ONE);
    	CleanupStack::PopAndDestroy();
		}
	time = User::TickCount() - time;
    test.Printf(_L("Time to load 100 times: %d\n"),time);
	}


TUint8 TRsReadTester::DumpByte(TUint aVal)
    {
    return(aVal>9? TUint8(aVal-10+'a'): TUint8(aVal+'0'));
    }

void TRsReadTester::DumpBytes(const TAny* aPtr,TInt aLen)
    {
    TUint8* src=(TUint8*)aPtr;
    TBuf<100> whole;
    TUint8* tar=(TUint8*)whole.Ptr();
    TInt len=0;
    while (aLen--)
        {
        TUint val=(*src++);
        TUint top=val/16;
        TUint bottom=val%16;
        *tar++=DumpByte(top);
        *tar++=DumpByte(bottom);
        *tar++=' ';
        len+=3;
        }
    whole.SetLength(len);
    test.Printf(whole);
    test.Printf(_L("\r\n"));
    }

TInt ThreadFunc1(TAny*)
  	{
  	CTrapCleanup* cleanup=CTrapCleanup::New();
  	RResourceReader reader;
  	reader.Close();// iCurrentPtr=NULL
  	TRAPD(err,reader.AdvanceL(0));//calls TResourceReaderImpl::AdvanceL()
  	delete cleanup;
  	return err;
  	}

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-1791
@SYMTestCaseDesc        Testing a panic on TResourceReaderImpl class(JustInTimeDebug is disabled)
@SYMTestPriority        Low
@SYMTestActions         Test that panics, when the condition inside __ASSERT is made false, by calling AdvanceL, after iCurrentPtr is made NULL using RResourceReader::close()
@SYMTestExpectedResults Test must panic
@SYMREQ                 REQ0000
*/

void TRsReadTester::PanicTResImp1()
  	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-1791 "));
  	TRequestStatus threadStatus;
  	RThread thread1;
  	TInt rc;
  	rc = thread1.Create(KPanicThread1, ThreadFunc1,
  		KDefaultStackSize, KMinHeapSize, KMinHeapSize*4,NULL);
  	test(KErrNone == rc);
  	thread1.Logon(threadStatus);
  	thread1.Resume();
  	User::WaitForRequest(threadStatus);

  	test(thread1.ExitType() == EExitPanic);
  	thread1.Close();
  	}

TInt ThreadFunc2(TAny*)
	{
  	CTrapCleanup* cleanup=CTrapCleanup::New();
  	RResourceReader reader;
  	reader.Close();// iCurrentPtr=NULL
  	TRAPD(err,reader.ReadInt8L());//calls TResourceReaderImpl::ReadInt8L()
  	delete cleanup;
  	return err;
  	}

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-1792
@SYMTestCaseDesc        Testing a panic on TResourceReaderImpl class(JustInTimeDebug is disabled)
@SYMTestPriority        Low
@SYMTestActions         Test that panics, when the condition inside __ASSERT is made false, by calling ReadInt8L, after iCurrentPtr is made NULL using RResourceReader::close()
@SYMTestExpectedResults Test must panic
@SYMREQ                 REQ0000
*/
void TRsReadTester::PanicTResImp2()
 	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-1792 "));
  	TRequestStatus threadStatus;
  	RThread thread2;
  	TInt rc;
  	rc = thread2.Create(KPanicThread2, ThreadFunc2,
  		KDefaultStackSize, KMinHeapSize, KMinHeapSize*4,NULL);
 	test(KErrNone == rc);
  	thread2.Logon(threadStatus);
  	thread2.Resume();
  	User::WaitForRequest(threadStatus);

  	test(thread2.ExitType() == EExitPanic);
  	thread2.Close();
  	}

TInt ThreadFunc3(TAny*)
  	{
  	CTrapCleanup* cleanup=CTrapCleanup::New();
  	RResourceReader reader;
  	reader.Close();
  	TRAPD(err,reader.ReadTPtrC8L());//calls TResourceReaderImpl::ReadTPtrC8L()
  	delete cleanup;
  	return err;
  	}

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-1793
@SYMTestCaseDesc        Testing a panic on TResourceReaderImpl class(JustInTimeDebug is disabled)
@SYMTestPriority        Low
@SYMTestActions         Test that panics, when the condition inside __ASSERT is made false, by calling ReadTPtrC8L, after iCurrentPtr is made NULL using RResourceReader::close()
@SYMTestExpectedResults Test must panic
@SYMREQ                 REQ0000
*/
void TRsReadTester::PanicTResImp3()
 	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-1793 "));
  	TRequestStatus threadStatus;
  	RThread thread3;
  	TInt rc;
  	rc = thread3.Create(KPanicThread3, ThreadFunc3,
  		KDefaultStackSize, KMinHeapSize, KMinHeapSize*4,NULL);
  	test(KErrNone == rc);
  	thread3.Logon(threadStatus);
  	thread3.Resume();
  	User::WaitForRequest(threadStatus);

  	test(thread3.ExitType() == EExitPanic);
  	thread3.Close();
  	}

TInt ThreadFunc4(TAny*)
  	{
  	CTrapCleanup* cleanup=CTrapCleanup::New();
  	RResourceReader reader;
  	reader.Close();
  	TRAPD(err,reader.ReadUint8L());//calls TResourceReaderImpl::ReadUint8L()
  	delete cleanup;
  	return err;
  	}

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-1794
@SYMTestCaseDesc        Testing a panic on TResourceReaderImpl class(JustInTimeDebug is disabled)
@SYMTestPriority        Low
@SYMTestActions         Test that panics, when the condition inside __ASSERT is made false, by calling ReadUint8L, after iCurrentPtr is made NULL using RResourceReader::close()
@SYMTestExpectedResults Test must panic
@SYMREQ                 REQ0000
*/
void TRsReadTester::PanicTResImp4()
 	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-1794 "));
 	TRequestStatus threadStatus;
  	RThread thread4;
  	TInt rc;
  	rc = thread4.Create(KPanicThread4, ThreadFunc4,
  		KDefaultStackSize, KMinHeapSize, KMinHeapSize*4,NULL);
  	test(KErrNone == rc);
  	thread4.Logon(threadStatus);
  	thread4.Resume();
  	User::WaitForRequest(threadStatus);

  	test(thread4.ExitType() == EExitPanic);
  	thread4.Close();
  	}

TInt TRsReadTester::PanicTests()
  	{
  	TBool jit;
  	jit = User::JustInTime();
  	User::SetJustInTime(EFalse);

  	//Tests for panics in TResourceReaderImpl class, through RResourceReader class
  	PanicTResImp1();
  	PanicTResImp2();
  	PanicTResImp3();
  	PanicTResImp4();

  	User::SetJustInTime(jit);
  	return KErrNone;
  	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0440
@SYMTestCaseDesc        Resource reader test
@SYMTestPriority        High
@SYMTestActions         Executes all the tests related to resource read
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void RunTests()
    {
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0440 Resource reader "));
    TheFs.Connect();
    TheResourceFile.OpenL(TheFs,KRomResourceFile);
	TRsReadTester lt;
	TRAPD(errCode, lt.TestButtonL());
	test(errCode==KErrNone);
	TRAP(errCode, lt.TestArrayL());
	test(errCode==KErrNone);
	TRAP(errCode, lt.TestFlPtEdL());
	test(errCode==KErrNone);
	TRAP(errCode, lt.TestMenuBarL());
	test(errCode==KErrNone);
	TRAP(errCode, lt.TestAlignment1L());
	test(errCode==KErrNone);
	TRAP(errCode, lt.TestAlignment2L());
	test(errCode==KErrNone);
    TRAP(errCode, lt.TimingTestsL());
	test(errCode==KErrNone);
    TheResourceFile.Close();
    //Test that raises panics
	test(KErrNone==lt.PanicTests());

    TheFs.Close();
	test.End();
	test.Close();
    }

TInt E32Main()
	{
	__UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	test.Title();
    TRAPD(err,RunTests());
	test(!err);
	test.Close();
    delete(cleanup);
	__UHEAP_MARKEND;
    return(0);
	}
