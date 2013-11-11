// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 
//

#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <f32file.h>
#include <barsc2.h>
#include <barsread2.h>
#include <badesca.h>
#include <trsc.rsg>
#include "T_RSC.H"

RTest TheTest(_L("T_RSREAD2"));
RFs TheFs;
CResourceFile* TheResourceFile = NULL;

//
//
//Test macroses and functions
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
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
//

//Opens aReader using RResourceReader::OpenLC(CResourceFile*, TInt)
static void OpenResourceReaderLC(RResourceReader& aReader, TInt aResourceId)
    {
    TEST(TheResourceFile != NULL);
    aReader.OpenLC(TheResourceFile, aResourceId);
    }

//Opens aReader using RResourceReader::OpenL(const TDesC8&)
static void OpenResourceReader2LC(RResourceReader& aReader, TInt aResourceId)
    {
    TEST(TheResourceFile != NULL);
    HBufC8* rscData = TheResourceFile->AllocReadLC(aResourceId);
    aReader.OpenL(rscData->Des());
    CleanupStack::PopAndDestroy(rscData);
    CleanupClosePushL(aReader);
    }

//This enum describes how to obtain the resource buffer for
//RResourceReader objects: from the resource file or from a buffer
enum TRscBufAllocation
    {
    EFromRscFile,
    EFromRscBuf,
    ERscBufLast
    };

typedef void (*FTOpenResourceReaderLC)(RResourceReader& aReader, TInt aResourceId);

static FTOpenResourceReaderLC OpenResourceReaderFuncLC[ERscBufLast] =
    {
    &OpenResourceReaderLC,
    &OpenResourceReader2LC
    };

//Opens aReader either from a resource file ot from a resource buffer
static void OpenResourceReaderLC(RResourceReader& aReader, TInt aResourceId, TRscBufAllocation aRscBufAllocation)
    {
    OpenResourceReaderFuncLC[aRscBufAllocation](aReader, aResourceId);
    }

class TRsReadTester
	{
public:
	void TestButtonL(RResourceReader& aReader);
	void TestArrayL(RResourceReader& aReader);
	void TestFlPtEdL(RResourceReader& aReader);
	void TestMenuBarL(RResourceReader& aReader);
    void TestAlignment1L(RResourceReader& aReader, TInt aIndex);
    void TestAlignment2L(RResourceReader& aReader);
    void TimingTestsL(TRscBufAllocation aRscBufAllocation);
private:
    void DumpBytes(const TAny* aPtr,TInt aLen);
    TUint8 DumpByte(TUint aVal);
	};

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1289
@SYMTestCaseDesc        Tests for reading data from a BUTTON resource file
@SYMTestPriority        High
@SYMTestActions         Tests for RResourceReader::ReadInt16L(),RResourceReader::ReadUInt16L(),
                        RResourceReader::ReadTPtrCL() functions
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestButtonL(RResourceReader& aReader)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1289 Test reading BUTTON resource "));
    TEST(aReader.ReadInt16L()==3);
    TEST(aReader.ReadUint16L()==5);
    TPtrC ptr=aReader.ReadTPtrCL();
    TEST(ptr==_L("Text"));
    ptr.Set(aReader.ReadTPtrCL());
    TEST(ptr.Length()==0);
    HBufC* hBuf=aReader.ReadHBufCL();
    TEST(*hBuf==_L("Bitmap placeholder"));
    delete(hBuf);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1290
@SYMTestCaseDesc        Tests for RResourceReader::ReadDesCArrayL() function
@SYMTestPriority        High
@SYMTestActions         Attempt to interpret data from an ARRAY resource file
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestArrayL(RResourceReader& aReader)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1290 Test reading ARRAY resource "));
    CDesCArray* desArray=aReader.ReadDesCArrayL();
    TEST(desArray->Count()==5);

    TEST((*desArray)[0]==_L("Esc"));
    TEST((*desArray)[1]==_L("Enter"));
    TEST((*desArray)[2]==_L("Tab"));
    TEST((*desArray)[3]==_L("Del"));
    TEST((*desArray)[4]==_L("Space"));

    delete(desArray);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1291
@SYMTestCaseDesc        Tests for reading FLPTED resource
@SYMTestPriority        High
@SYMTestActions         Tests for RResourceReader::ReadReal64L() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestFlPtEdL(RResourceReader& aReader)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1291 Test reading FLPTED resource "));
    TEST(aReader.ReadInt16L()==18);
    TReal little=aReader.ReadReal64L();
    TEST(little==0.0);
    TReal big=aReader.ReadReal64L();
    TEST(big>9.89e99);
    TEST(big<9.91e99);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1292
@SYMTestCaseDesc        Tests for reading MENU_BAR resource
@SYMTestPriority        High
@SYMTestActions         Tests for RResourceReader::ReadInt16L() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestMenuBarL(RResourceReader& aReader)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1292 Test reading MENU_BAR resource "));
    TEST(aReader.ReadInt16L()==8); // how many items following
    TPtrC ref=_L("abcdefgh");
    for (TInt ii=1; ii<=8; ii++)
        {
        TEST(aReader.ReadInt32L()==ii);
        TEST(aReader.ReadTPtrCL()==ref.Left(ii));
        }
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1293
@SYMTestCaseDesc        Tests for reading ALIGNMENT_HORROR resources
@SYMTestPriority        High
@SYMTestActions         Tests for RResourceReader::ReadTPtrC8L(),RResourceReader::ReadTPtrC16L() functions
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestAlignment1L(RResourceReader& aReader, TInt aIndex)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1293 Test reading ALIGNMENT_HORROR resources "));
	TPtrC8  ref8 =_L8("xyz");
	TPtrC16 ref16=_L16("xyz");
	TEST(aReader.ReadTPtrC8L() == ref8.Left(aIndex));
	TEST(aReader.ReadTPtrC16L()== ref16.Left(aIndex));
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1294
@SYMTestCaseDesc        Tests for reading ALIGNMENT_HORROR_ARRAY resource
@SYMTestPriority        High
@SYMTestActions         Tests for RResourceReader::ReadTPtrC8L(),RResourceReader::ReadTPtrC16L() functions
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TestAlignment2L(RResourceReader& aReader)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1294 Test reading ALIGNMENT_HORROR_ARRAY resource "));
    TEST(aReader.ReadInt16L()==7); // how many items following
    TPtrC8  ref8 =_L8("abcdef");
    TPtrC16 ref16=_L16("abcdef");
    for (TInt ii=0; ii<=6; ii++)
        {
        TEST(aReader.ReadTPtrC8L() ==ref8.Left(ii));
        TEST(aReader.ReadTPtrC16L()==ref16.Mid(ii));
        }
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1295
@SYMTestCaseDesc        Tests for time required to load a resource file around 100 times
@SYMTestPriority        High
@SYMTestActions         Tests for time taken to load resource files repeatedly
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TRsReadTester::TimingTestsL(TRscBufAllocation aRscBufAllocation)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1295 Timing tests "));
	TheTest.Next(_L("BUTTON and FLPTED"));
	TUint time = User::TickCount();
	for (TInt ii=0; ii<100; ii++)
		{
    	RResourceReader reader;
        ::OpenResourceReaderLC(reader, SYS_BUTTON_ONE, aRscBufAllocation);
	    CleanupStack::PopAndDestroy(&reader);
        ::OpenResourceReaderLC(reader, SYS_FLPTED_ONE, aRscBufAllocation);
    	CleanupStack::PopAndDestroy(&reader);
		}
	time = User::TickCount() - time;
	TheTest.Printf(_L("Time to load 100 times: %d\n"),time);

	TheTest.Next(_L("BUTTON and ARRAY"));
	time = User::TickCount();
	for (TInt jj=0; jj<100; jj++)
		{
    	RResourceReader reader;
        ::OpenResourceReaderLC(reader, SYS_BUTTON_ONE, aRscBufAllocation);
	    CleanupStack::PopAndDestroy(&reader);
        ::OpenResourceReaderLC(reader, SYS_ARRAY_ONE, aRscBufAllocation);
    	CleanupStack::PopAndDestroy(&reader);
		}
	time = User::TickCount() - time;
	TheTest.Printf(_L("Time to load 100 times: %d\n"),time);
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
    TheTest.Printf(whole);
    TheTest.Printf(_L("\r\n"));
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0474
@SYMTestCaseDesc        Tests for the functionality of RResourceReader class
@SYMTestPriority        High
@SYMTestActions         Open a resource file and test for the reading the data
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
static void TestButtonL(TRsReadTester& aTest, TRscBufAllocation aRscBufAllocation)
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0474 "));
    RResourceReader reader;
    ::OpenResourceReaderLC(reader, SYS_BUTTON_ONE, aRscBufAllocation);
	TRAPD(errCode, aTest.TestButtonL(reader));
    CleanupStack::PopAndDestroy(&reader);
	TEST2(errCode, KErrNone);
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0475
@SYMTestCaseDesc        Tests for the functionality of RResourceReader class
@SYMTestPriority        High
@SYMTestActions         Tests for reading descriptor array.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
static void TestArrayL(TRsReadTester& aTest, TRscBufAllocation aRscBufAllocation)
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0475 "));
    RResourceReader reader;
    ::OpenResourceReaderLC(reader, SYS_ARRAY_ONE, aRscBufAllocation);
	TRAPD(errCode, aTest.TestArrayL(reader));
    CleanupStack::PopAndDestroy(&reader);
	TEST2(errCode, KErrNone);
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0476
@SYMTestCaseDesc        Tests for the functionality of RResourceReader class
@SYMTestPriority        High
@SYMTestActions         Tests for reading real values
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
static void TestFlPtEdL(TRsReadTester& aTest, TRscBufAllocation aRscBufAllocation)
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0476 "));
    RResourceReader reader;
    ::OpenResourceReaderLC(reader, SYS_FLPTED_ONE, aRscBufAllocation);
	TRAPD(errCode, aTest.TestFlPtEdL(reader));
    CleanupStack::PopAndDestroy(&reader);
	TEST2(errCode, KErrNone);
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0477
@SYMTestCaseDesc        Tests for the functionality of RResourceReader class
@SYMTestPriority        High
@SYMTestActions         Tests for reading menu_bar resource
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
static void TestMenuBarL(TRsReadTester& aTest, TRscBufAllocation aRscBufAllocation)
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0477 "));
    RResourceReader reader;
    ::OpenResourceReaderLC(reader, SYS_MENUBAR_ONE, aRscBufAllocation);
	TRAPD(errCode, aTest.TestMenuBarL(reader));
    CleanupStack::PopAndDestroy(&reader);
	TEST2(errCode, KErrNone);
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0478
@SYMTestCaseDesc        Tests for the functionality of RResourceReader class
@SYMTestPriority        High
@SYMTestActions         Tests for reading descriptors and checks for alignment
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
static void TestAlignment1L(TRsReadTester& aTest, TRscBufAllocation aRscBufAllocation)
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0478 "));
    const TInt TheHorrors[] =
        {
	    SYS_ALIGNMENT_HORROR0, SYS_ALIGNMENT_HORROR1,
	    SYS_ALIGNMENT_HORROR2, SYS_ALIGNMENT_HORROR3,
	    0
	    };

	for(TInt i=0;TheHorrors[i]!=0;++i)
        {
        RResourceReader reader;
        ::OpenResourceReaderLC(reader, TheHorrors[i], aRscBufAllocation);
	    TRAPD(errCode, aTest.TestAlignment1L(reader, i));
        CleanupStack::PopAndDestroy(&reader);
	    TEST2(errCode, KErrNone);
        }
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0479
@SYMTestCaseDesc        Tests for the functionality of RResourceReader class
@SYMTestPriority        High
@SYMTestActions         Tests for reading descriptors and checks for alignment
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
static void TestAlignment2L(TRsReadTester& aTest, TRscBufAllocation aRscBufAllocation)
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0479 "));
    RResourceReader reader;
    ::OpenResourceReaderLC(reader, SYS_ALIGNMENT_HORROR_ARRAY, aRscBufAllocation);
	TRAPD(errCode, aTest.TestAlignment2L(reader));
    CleanupStack::PopAndDestroy(&reader);
	TEST2(errCode, KErrNone);
    }

static void RunTestsL()
    {
	TheTest.Start(_L("Resource reader"));
	CleanupClosePushL(TheFs);
	User::LeaveIfError(TheFs.Connect());
    TheResourceFile = CResourceFile::NewLC(TheFs,KRomResourceFile, 0, 0);

	TRsReadTester lt;

    ::TestButtonL(lt, EFromRscFile);
    ::TestButtonL(lt, EFromRscBuf);

    ::TestArrayL(lt, EFromRscFile);
    ::TestArrayL(lt, EFromRscBuf);

    ::TestFlPtEdL(lt, EFromRscFile);
    ::TestFlPtEdL(lt, EFromRscBuf);

    ::TestMenuBarL(lt, EFromRscFile);
    ::TestMenuBarL(lt, EFromRscBuf);

    ::TestAlignment1L(lt, EFromRscFile);
    ::TestAlignment1L(lt, EFromRscBuf);

    ::TestAlignment2L(lt, EFromRscFile);
    ::TestAlignment2L(lt, EFromRscBuf);

    TRAPD(errCode, lt.TimingTestsL(EFromRscFile));
	TEST2(errCode, KErrNone);

    TRAP(errCode, lt.TimingTestsL(EFromRscBuf));
	TEST2(errCode, KErrNone);

	CleanupStack::PopAndDestroy(TheResourceFile);
	CleanupStack::PopAndDestroy(1, &TheFs);
    }

TInt E32Main()
	{
	__UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	TheTest.Title();
    TRAPD(err,::RunTestsL());
	TEST2(err, KErrNone);
	TheTest.Next(_L("/n"));
	TheTest.End();
	TheTest.Close();
    delete(cleanup);
	__UHEAP_MARKEND;
    return(0);
	}
