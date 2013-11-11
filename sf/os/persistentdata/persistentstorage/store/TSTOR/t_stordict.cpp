// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32mem.h>

//
// CTestStreamDictionary
// The only reason this class to be used here is to 
// get an access to CStreamDictionary::iArray (private data member). 
//
class CTestStreamDictionary : public CStreamDictionary
	{
public:
	struct TEntry 
	    {
	    TUid uid;
	    TStreamId id;
	    };
public:
	static CTestStreamDictionary* NewL();
	CTestStreamDictionary();
	TUid Uid(TInt aInt) 
	    { 
	    return (*iCheat)[aInt].uid; 
	    }
	TStreamId StreamId(TInt aInt) 
	    { 
	    return (*iCheat)[aInt].id; 
	    }
	TInt Count() 
	    { 
	    return iCheat->Count(); 
	    }
private:
	CArrayFixSeg<TEntry>* iCheat;
	};

CTestStreamDictionary* CTestStreamDictionary::NewL()
	{
	CTestStreamDictionary* thing=new(ELeave) CTestStreamDictionary();
	return thing;
	}

CTestStreamDictionary::CTestStreamDictionary()
	: iCheat((CArrayFixSeg<TEntry>*)&iCheat-1) //Now iCheat points to the base class' private data member: 
	                                           //CStreamDictionary::iArray.
                                               //This way it is possible to call iArray's methods (even though it is private).
	{
	}


//
// Test code
//

const TInt KTestExpandSize=0x20;

static RTest TheTest(_L("t_stordict"));

// some uid-stream pairs to use for testing
const TUid testUid1={1};
static TStreamId testStreamId1=TStreamId(1);
//
const TUid testUid2={57};
static TStreamId testStreamId2=TStreamId(57);
//
const TUid testUid3={99999};
static TStreamId testStreamId3=TStreamId(425);
//

//Put test data files to be deleted at the end here!
void DeleteDataFiles()
    {
    }

//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
    {
    if(!aValue)
        {
        DeleteDataFiles();
        TheTest(EFalse, aLine);
        }
    }
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
    {
    if(aValue != aExpected)
        {
        RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
        DeleteDataFiles();
        TheTest(EFalse, aLine);
        }
    }
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1201
@SYMTestCaseDesc	    Tests for copy operations on dictionary files
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for copying two classes using memory based streams.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
template <class T1,class T2>
void testCopyL(T1& aCopy,const T2& anOriginal)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1201 "));
	CBufSeg* buf=0;
	TRAPD(r,buf=CBufSeg::NewL(KTestExpandSize));
    TEST2(r, KErrNone);
//
// Write anOriginal out to the buffer.
//
	RBufWriteStream out;
	out.Append(*buf);
	TRAP(r,out<<anOriginal);
	TEST2(r, KErrNone);
	TRAP(r,out.CommitL());
    TEST2(r, KErrNone);
//
// Read anOriginal in from the buffer.
//
	RBufReadStream in(*buf);
	TRAP(r,in>>aCopy);
    TEST2(r, KErrNone);
//
// See if it's consumed the lot.
//
	TUint8 b;
	TEST2(in.Source()->ReadL(&b,1), 0);
//
	delete buf;
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1202
@SYMTestCaseDesc	    Tests if two dictionary files are equal
@SYMTestPriority 	    High
@SYMTestActions  	    Tests if count of entries,UID and streamID's are equal
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void testIsEqual(CTestStreamDictionary* aCopy,CTestStreamDictionary* aOrig)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1202 "));
	TInt origCount=aOrig->Count();
	TEST(origCount==aCopy->Count());
	//
	for (TInt i=0 ; i<origCount ; i++)
		{
		TEST(aOrig->Uid(i)==aCopy->Uid(i));
		TEST(aOrig->StreamId(i)==aCopy->StreamId(i));
		}
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1203
@SYMTestCaseDesc	    Tests for simple operations on a dictionary file
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for assign,re-assigning,removing entries from the file
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void simpleTestsL()
	{
	CTestStreamDictionary* dic=CTestStreamDictionary::NewL();
	// attempt finding and removing with an empty dictionary
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1203 Manipulating an empty dictionary "));
	TEST2(dic->Count(), 0);
	TEST(dic->At(testUid1)==KNullStreamId);
	dic->Remove(testUid1);
	TEST2(dic->Count(), 0);
	TEST(dic->IsNull());
	//
	// assign an entry
	TheTest.Next(_L("Assigning entries and manipulating them"));
	TRAPD(ret,dic->AssignL(testUid1,testStreamId1));
	TEST2(ret, KErrNone);
	TEST2(dic->Count(), 1);
	TEST(!dic->IsNull());
	TEST(dic->At(testUid1)==testStreamId1);
	//
	// assign another entry
	TRAP(ret,dic->AssignL(testUid2,testStreamId2));
	TEST2(ret, KErrNone);
	TEST2(dic->Count(), 2);
	TEST(dic->At(testUid2)==testStreamId2);
	//
	// re-assign uid1
	TRAP(ret,dic->AssignL(testUid1,testStreamId3));
	TEST2(ret, KErrNone);
	TEST2(dic->Count(), 2);
	TEST(dic->At(testUid1)==testStreamId3);
	//
	// test finding and removing a non-existant entry from a non-empty dictionary
	TEST(dic->At(testUid3)==KNullStreamId);
	dic->Remove(testUid3);
	TEST2(dic->Count(), 2);
	//
	// test removing an entry
	dic->Remove(testUid1);
	TEST2(dic->Count(), 1);
	TEST(dic->At(testUid1)==KNullStreamId);
	TEST(dic->At(testUid2)==testStreamId2);
	TEST(!dic->IsNull());
	//
	// test removing the other entry
	dic->Remove(testUid2);
	TEST2(dic->Count(), 0);
	TEST(dic->IsNull());
	TEST(dic->At(testUid1)==KNullStreamId);
	TEST(dic->At(testUid2)==KNullStreamId);
	//
	delete dic;
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1204
@SYMTestCaseDesc	    Streaming dictionary files tests
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for copying an empty dictionary and dictionary containing different sets of entries
                        Tests for equality of two dictionary files and test the copied file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void streamingTestsL()
	{
	CTestStreamDictionary* orig=CTestStreamDictionary::NewL();
	CTestStreamDictionary* copy=CTestStreamDictionary::NewL();
	//
	// copy an empty dictionary
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1204 Streaming an empty dictionary "));
	TEST(orig->IsNull());
	testCopyL(*copy,*orig);
	TEST(copy->IsNull());
	//
	// copy a dictionary containing a range of entries
	TheTest.Next(_L("Streaming a dictionary containing entries"));
	TRAPD(ret,orig->AssignL(testUid1,testStreamId1));
	TRAP(ret,orig->AssignL(testUid2,testStreamId2));
	TRAP(ret,orig->AssignL(testUid3,testStreamId3));
	testCopyL(*copy,*orig);
	testIsEqual(copy,orig);
	TEST(!copy->IsNull());
	//
	delete orig;
	delete copy;
	}

void DoTestL()
    {
    simpleTestsL();
    streamingTestsL();
    }

TInt E32Main()
	{
    __UHEAP_MARK;
	
    TheTest.Title();
	
    CTrapCleanup* trapCleanup = CTrapCleanup::New();
    TheTest(trapCleanup != NULL);

	TheTest.Start(_L("Testing CStreamDictionary..."));
	
	TRAPD(err, DoTestL());
    TEST2(err, KErrNone);

    DeleteDataFiles();
    
    TheTest.End();
    TheTest.Close();

    delete trapCleanup;
    
    __UHEAP_MARKEND;
    
	return KErrNone;
	}

