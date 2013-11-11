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
// Started by DWW, October 1996
// Tests cliboard
// 
//

#include <e32test.h>
#include <baclipb.h>
#include <bautils.h>
#include <f32file.h>
#include <s32strm.h>
#include <s32stor.h>
#include <s32file.h>

LOCAL_D RTest test(_L("T_CLIPB"));

const TUid KClipboardFileUid={268435515};
const TUid KUidNameClipboardType={77001};
const TUid KUidAddressClipboardType={77002};
const TUid KUidNumberClipboardType={77003};
const TUid KUidTestType={77004};

const TPtrC KClipboardFileCDrive=_L("C:\\System\\Data\\ClpBoard.cbd");
const TPtrC KClipboardFileDDrive=_L("D:\\System\\Data\\ClpBoard.cbd");
const TPtrC KClipboardFileEDrive=_L("E:\\System\\Data\\ClpBoard.cbd");
/**
 * NOTE :- On order to run this test in WINS, there must exist a mapping for the drives
 * used above. So in Epoc32\Data add the following lines to epoc.ini...
 *
 * _epoc_drive_d \epoc32\wins\d
 * _epoc_drive_e \epoc32\wins\e
 *
 * only needed for drive D & E as there is a mapping for drive C.
 * Plus make sure that the the above directories (\epoc32\wins\d & e) exist.
 */

class TClBase
	{
public:
	virtual void CopyToClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary) const=0;
	virtual TBool PasteFromClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary)=0;
	void CopyL();
	TBool PasteL();
	TBool PasteWithoutClipboardL();
private:
	void DoCopyL(RFs& aFsSession);
	TBool DoPasteL(RFs& aFsSession);
	};

void TClBase::CopyL()
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	TRAPD(err,DoCopyL(fsSession));
	fsSession.Close();
	User::LeaveIfError(err);
	}

void TClBase::DoCopyL(RFs& aFsSession)
	{
	CClipboard* cb=CClipboard::NewForWritingLC(aFsSession);
	CopyToClipboardL(cb->Store(),cb->StreamDictionary());
	cb->CommitL();
	CleanupStack::PopAndDestroy();
	}

TBool TClBase::PasteL()
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	TBool res=EFalse;
	TRAPD(err,res=DoPasteL(fsSession));
	fsSession.Close();
	User::LeaveIfError(err);
	return(res);
	}

TBool TClBase::DoPasteL(RFs& aFsSession)
	{
	CClipboard* cb=CClipboard::NewForReadingLC(aFsSession);
	TBool res=PasteFromClipboardL(cb->Store(),cb->StreamDictionary());
	CleanupStack::PopAndDestroy();
	return(res);
	}

class TClName : public TClBase
	{
public:
	void CopyToClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary) const;
	TBool PasteFromClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary);
public:
	TBuf<4> iName;
	};

class TClNameWithAddress : public TClName
	{
	void CopyToClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary) const;
	TBool PasteFromClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary);
public:
	TBuf<10> iAddress;
	};

class TClNameWithNumber : public TClName
	{
	void CopyToClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary) const;
	TBool PasteFromClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary);
public:
	TInt32 iNumber;
	};

class TClInteger : public TClBase
	{
public:
	void CopyToClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary) const;
	TBool PasteFromClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary);
public:
	TInt32 iInteger;
	};

void TClName::CopyToClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary) const
	{
	RStoreWriteStream stream;
	TStreamId streamId=stream.CreateLC(aStore);
	stream<<iName;
	CleanupStack::PopAndDestroy();
	aDictionary.AssignL(KUidNameClipboardType,streamId);
	}

TBool TClName::PasteFromClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary)
	{
	TStreamId streamId=aDictionary.At(KUidNameClipboardType);
	if (streamId==KNullStreamId)
		return(EFalse);
	RStoreReadStream stream;
	stream.OpenLC(aStore,streamId);
	stream>>iName;
	CleanupStack::PopAndDestroy();
	return(ETrue);
	}

void TClNameWithAddress::CopyToClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary) const
	{
	TClName::CopyToClipboardL(aStore,aDictionary);
	RStoreWriteStream stream;
	TStreamId streamId=stream.CreateLC(aStore);
	stream<<iAddress;
	CleanupStack::PopAndDestroy();
	aDictionary.AssignL(KUidAddressClipboardType,streamId);
	}

TBool TClNameWithAddress::PasteFromClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary)
	{
	TBool namePasted=TClName::PasteFromClipboardL(aStore,aDictionary);
	TStreamId streamId=aDictionary.At(KUidAddressClipboardType);
	if (streamId==KNullStreamId)
		return(namePasted);
	RStoreReadStream stream;
	stream.OpenLC(aStore,streamId);
	stream>>iAddress;
	CleanupStack::PopAndDestroy();
	return(ETrue);
	}

void TClNameWithNumber::CopyToClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary) const
	{
	TClName::CopyToClipboardL(aStore,aDictionary);
	RStoreWriteStream stream;
	TStreamId streamId=stream.CreateLC(aStore);
	stream<<iNumber;
	CleanupStack::PopAndDestroy();
	aDictionary.AssignL(KUidNumberClipboardType,streamId);
	}

TBool TClNameWithNumber::PasteFromClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary)
	{
	TBool namePasted=TClName::PasteFromClipboardL(aStore,aDictionary);
	TStreamId streamId=aDictionary.At(KUidNumberClipboardType);
	if (streamId==KNullStreamId)
		return(namePasted);
	RStoreReadStream stream;
	stream.OpenLC(aStore,streamId);
	stream>>iNumber;
	CleanupStack::PopAndDestroy();
	return(ETrue);
	}

void TClInteger::CopyToClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary) const
	{
	RStoreWriteStream stream;
	TStreamId streamId=stream.CreateLC(aStore);
	stream<<iInteger;
	CleanupStack::PopAndDestroy();
	aDictionary.AssignL(KUidNumberClipboardType,streamId);
	}

TBool TClInteger::PasteFromClipboardL(CStreamStore& aStore,CStreamDictionary& aDictionary)
	{
	TStreamId streamId=aDictionary.At(KUidNumberClipboardType);
	if (streamId==KNullStreamId)
		return(EFalse);
	RStoreReadStream stream;
	stream.OpenLC(aStore,streamId);
	stream>>iInteger;
	CleanupStack::PopAndDestroy();
	return(ETrue);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0404
@SYMTestCaseDesc        Tests the  CClipboard::Store(),StreamDictionary functions
@SYMTestPriority        High
@SYMTestActions         Tests the clipboard's file store with various cases(no file,file in use,corrupt file,non-store file)
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestErrorHandling(const TDesC& aTestPath)
	{

	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0404 "));
	TPtrC KClipboardFile(aTestPath);
	RFs fs;
	test (fs.Connect()==KErrNone);
//
	test.Next(_L("No clipboard file"));
	fs.Delete(KClipboardFile);
	test (CClipboard::Clear(fs)==KErrNone);
	CClipboard* cb=NULL;
	TRAPD(err,cb=CClipboard::NewForReadingL(fs));
	test (err==KErrNone);
	test (&cb->Store()==NULL);
	test (&cb->StreamDictionary()!=NULL);
	test (cb->StreamDictionary().IsNull());
	delete cb;
//
	test.Next(_L("In use"));
	RFile file;
	test (file.Create(fs,KClipboardFile,EFileRead|EFileWrite)==KErrNone);
	TRAP(err,cb=CClipboard::NewForReadingL(fs));
	test (err==KErrNone);
	test (&cb->Store()==NULL);
	test (&cb->StreamDictionary()!=NULL);
	test (cb->StreamDictionary().IsNull());
	delete cb;
//
	test.Next(_L("Clear while in use"));
	test (CClipboard::Clear(fs)==KErrInUse);
//
	test.Next(_L("Bogus clipboard file"));
	file.Close();
	TRAP(err,cb=CClipboard::NewForReadingL(fs));
	test (err==KErrNone);
	test (&cb->Store()==NULL);
	test (&cb->StreamDictionary()!=NULL);
	test (cb->StreamDictionary().IsNull());
	delete cb;
//
	test.Next(_L("Non-store file"));
	test (file.Replace(fs,KClipboardFile,EFileRead|EFileWrite)==KErrNone);
	test (file.Write(_L8("some data which does not make this a file store"))==KErrNone);
	file.Close();
	TRAP(err,cb=CClipboard::NewForReadingL(fs));
	test (err==KErrNone);
	test (&cb->Store()==NULL);
	test (&cb->StreamDictionary()!=NULL);
	test (cb->StreamDictionary().IsNull());
	delete cb;
//
	test.Next(_L("Wrong type file"));
	CFileStore* store=CDirectFileStore::ReplaceLC(fs,KClipboardFile,EFileRead|EFileWrite);
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid,KUidTestType));
	store->CommitL();
	CleanupStack::PopAndDestroy();
	TRAP(err,cb=CClipboard::NewForReadingL(fs));
	test (err==KErrNone);
	test (&cb->Store()==NULL);
	test (&cb->StreamDictionary()!=NULL);
	test (cb->StreamDictionary().IsNull());
	delete cb;
//
	test.Next(_L("Corrupted clipboard"));
	store=CDirectFileStore::ReplaceLC(fs,KClipboardFile,EFileRead|EFileWrite);
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid,KClipboardFileUid));
	store->CommitL();
	CleanupStack::PopAndDestroy();
	TRAP(err,cb=CClipboard::NewForReadingL(fs));
	test (err==KErrNone);
	test (&cb->Store()==NULL);
	test (&cb->StreamDictionary()!=NULL);
	test (cb->StreamDictionary().IsNull());
	delete cb;
//
	test.Next(_L("Clear clipboard"));
	test (CClipboard::Clear(fs)==KErrNone);
	TRAP(err,cb=CClipboard::NewForReadingL(fs));
	test (err==KErrNone);
	test (&cb->Store()==NULL);
	test (&cb->StreamDictionary()!=NULL);
	test (cb->StreamDictionary().IsNull());
	delete cb;
//
#if defined(_DEBUG)
	test.Next(_L("Out of memory failure"));
//
	cb=CClipboard::NewForWritingLC(fs);
	RStoreWriteStream stream;
	TStreamId testId=stream.CreateLC(cb->Store());
	stream.WriteInt32L(0);
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	cb->StreamDictionary().AssignL(KUidTestType,testId);
	cb->CommitL();
	CleanupStack::PopAndDestroy();
//
	__UHEAP_MARK;
	for (TInt ii=0;;++ii)
		{
		__UHEAP_FAILNEXT(ii);
		TRAP(err,cb=CClipboard::NewForReadingL(fs));
		if (err==KErrNone)
			break;
		test(err==KErrNoMemory);
		__UHEAP_CHECK(0);
		}
	__UHEAP_RESET;
	test (&cb->Store()!=NULL);
	test (cb->StreamDictionary().At(KUidTestType)==testId);
	delete cb;
	__UHEAP_MARKEND;
//
#endif
//
	test.Next(_L("Fail to commit clipboard"));
	TRAP(err,cb=CClipboard::NewForWritingLC(fs);CleanupStack::Pop());
	test (err==KErrNone);
	delete cb;
	test (!BaflUtils::FileExists(fs,KClipboardFile));
//
	fs.Close();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0405
@SYMTestCaseDesc        Tests the CClipboard::NewForWritingLC,NewForReadingL
@SYMTestPriority        High
@SYMTestActions         Tests for copy and paste operations on TClName,TClNameWithAddress,TClInteger,TClNameWithNumber
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void DoTestsOnDrive(const TDesC& aDrivePath)
    {
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0405 Test single paste "));
	TClName n1,n2;
	n1.iName=_L("Fred");
	n2.iName=_L("Dino");
	n1.CopyL();
	TClNameWithAddress na1,na2;
	test(na1.PasteL());
	n2.CopyL();
	test(na2.PasteL());
	test(na1.iName==_L("Fred"));
	test(na2.iName==_L("Dino"));
	na1.iAddress=_L("Bedrock");
	test.Next(_L("Test double paste"));
	na1.CopyL();
	test(na2.PasteL());
	test(na2.iAddress==_L("Bedrock"));
	test(na2.iName==_L("Fred"));
	test.Next(_L("Test paste failure if type not present"));
	TClInteger i1,i2;
	test(!i1.PasteL());
	test.Next(_L("Test selecting the right data type"));
	TClNameWithNumber nn1,nn2;
	nn1.iNumber=36363;
	test(nn1.PasteL());
	test(nn1.iNumber==36363);
	test(nn1.iName==_L("Fred"));
	nn1.iName=_L("Wilm");
	nn1.CopyL();
	test(i2.PasteL());
	test(i2.iInteger==36363);
	test(n1.PasteL());
	test(n1.iName==_L("Wilm"));
	test(nn2.PasteL());
	test(nn2.iName==_L("Wilm"));
	test(nn2.iNumber==36363);
//
	TestErrorHandling(aDrivePath);
	test.End();
    }

void DoTests()
	{
	// Tests for the HAL attribute ClipboardDrive set in the device.
	// C Drive
	DoTestsOnDrive(KClipboardFileCDrive);
	}

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	test.Title();
	test.Start(_L("Testing CClipboard "));
    TRAPD(err,DoTests());
    test(err==KErrNone);
	test.End();
    test.Close();
    delete cleanup;
    __UHEAP_MARKEND;
	return(0);
    }
