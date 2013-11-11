/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <s32file.h>
#include <flddef.h>
#include <fldbase.h>
#include <fldset.h>
#include "TESTFAC.H"
#include "T_STREAM.h"

#define test(cond)											\
	{														\
	TBool __bb = (cond);									\
	TEST(__bb);												\
	if (!__bb)												\
		{													\
		ERR_PRINTF1(_L("ERROR: Test Failed"));				\
		User::Leave(1);										\
		}													\
	}

const TInt KTestCleanupStack=0x20;

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D CTextFieldSet* TheFieldSetOriginal;
LOCAL_D CTextFieldSet* TheFieldSetCopy;
LOCAL_D TTestFieldFactory* TheFactory;

_LIT(KTFieldOutputFile, "c:\\etext\\tfield.tst");
// Test Picture persistance.
void CT_STREAM::testStoreRestore(CTextFieldSet* aCopy,const CTextFieldSet* aOriginal)
    {
	// set up the store
	RFs	theFs;
	theFs.Connect();
	theFs.Delete(KTFieldOutputFile);
	theFs.MkDirAll(KTFieldOutputFile);
	CFileStore* theStore=CDirectFileStore::CreateL(theFs, KTFieldOutputFile, EFileRead | EFileWrite);
	CleanupStack::PushL(theStore);
	theStore->SetTypeL(KDirectFileStoreLayoutUid);
	//
	// store the original
	TStreamId id(0);
	TRAPD(ret,id=aOriginal->StoreL(*theStore));
		test(ret==KErrNone);
	//
	// restore into the copy
	TRAP(ret,aCopy->RestoreL(*theStore,id));
		test(ret==KErrNone);
	//
	// tidy up
	CleanupStack::PopAndDestroy();  // theStore
	theFs.Close();
    }



template <class T>
void CT_STREAM::testCopyPaste(T* aCopy, T* aOriginal,TInt aCopyPos,TInt aLen,TInt aPastePos,TInt aPasteLen)
// Copy part of anOriginal to aCopy using memory-based streams.
//
	{
	// set up the store
	RFs	theFs;
	theFs.Connect();
	CFileStore* theStore=CDirectFileStore::ReplaceL(theFs,KTFieldOutputFile,EFileRead|EFileWrite);
	CleanupStack::PushL(theStore);
	theStore->SetTypeL(KDirectFileStoreLayoutUid);
	//
	// Copy anOriginal out to the buffer
	TStreamId id(0);
	TRAPD(ret,id=aOriginal->CopyToStoreL(*theStore,aCopyPos,aLen));
		test(ret==KErrNone);
	//
	// paste into the copy
	TRAP(ret,aCopy->PasteFromStoreL(*theStore,id,aPastePos,aPasteLen));
		test(ret==KErrNone);
	//
	// tidy up
	CleanupStack::PopAndDestroy();  // theStore
	theFs.Close();

	}


TBool CT_STREAM::IsEqual(const CTextFieldSet* aCopy,const CTextFieldSet* anOriginal)
//
// Returns true if aCopy contents matches anOriginal contents.
//
	{
	// test the num chars and num fields
	TInt charCount = anOriginal->CharCount();
	TInt fieldCount = anOriginal->FieldCount();
	test(charCount==aCopy->CharCount());
	test(fieldCount==aCopy->FieldCount());

	// test each entry in the array
	for (TInt pos=0 ; pos<charCount ;)
		{
		TFindFieldInfo infoOrig,infoCopy;
		anOriginal->FindFields(infoOrig,pos,charCount-pos);
		aCopy->FindFields(infoCopy,pos,charCount-pos);
		test(infoOrig==infoCopy);
		pos += infoOrig.iFirstFieldPos+infoOrig.iFirstFieldLen;
		if (infoOrig.iFieldCountInRange==0)
			break;
		}

	return ETrue;
	}


TBool CT_STREAM::UpdateField(TInt aPos,CTextFieldSet* aSet)
	{
	// find out which field aPos is in
	TFindFieldInfo info;
	TBool ret;
	ret=aSet->FindFields(info,aPos);
		test(ret);
	// get the new value
	HBufC* buf = HBufC::NewL(5); 
	CleanupStack::PushL(buf);
 	TInt err=aSet->NewFieldValueL(buf,aPos); 
		test(err==KErrNone);
	// Notify FieldSet of update
	aSet->NotifyFieldUpdate(aPos,buf->Length());
	// tidy up
	CleanupStack::PopAndDestroy();
	return ret;
	}


void CT_STREAM::test2()
// Streams an empty field set
//
	{
	INFO_PRINTF1(_L("- streaming of a default FieldSet (no fields)"));
	testStoreRestore(TheFieldSetCopy,TheFieldSetOriginal);
	test(IsEqual(TheFieldSetCopy,TheFieldSetOriginal));
	TheFieldSetOriginal->Reset();
	TheFieldSetCopy->Reset();
	}


void CT_STREAM::test3()
// Streams a field set containing 100 chars & 1 field
//
	{
	INFO_PRINTF1(_L("- streaming with a field"));

	// insert a block of text into the original
	TheFieldSetOriginal->NotifyInsertion(0,100); // pos=0, len=100
	// insert a field into the original
	CTextField* field=TheFieldSetOriginal->NewFieldL(KDummyFieldUid);
		test(field!=NULL);
	TInt ret=TheFieldSetOriginal->InsertFieldL(10,field,KDummyFieldUid); // pos=10
		test(ret==KErrNone);
	ret=UpdateField(10,TheFieldSetOriginal);
		test(ret);

	// test streaming
	testStoreRestore(TheFieldSetCopy,TheFieldSetOriginal);
	test(IsEqual(TheFieldSetCopy,TheFieldSetOriginal));
	TheFieldSetOriginal->Reset();
	TheFieldSetCopy->Reset();
	}


void CT_STREAM::test4()
// Streams a field set into a non-empty target
//
	{
	INFO_PRINTF1(_L("- streaming into a non-empty target"));

	// insert a block of text into the original
	TheFieldSetOriginal->NotifyInsertion(0,100); // pos=0, len=100
	// insert a field into the original
	CTextField* field=TheFieldSetOriginal->NewFieldL(KDummyFieldUid);
		test(field!=NULL);
	TInt ret=TheFieldSetOriginal->InsertFieldL(10,field,KDummyFieldUid); // pos=10
		test(ret==KErrNone);
	ret=UpdateField(10,TheFieldSetOriginal);
		test(ret);

	// insert a block of text into the copy
	TheFieldSetCopy->NotifyInsertion(0,40); // pos=0, len=40
	// insert two fields into the copy
	CTextField* field2 = TheFieldSetCopy->NewFieldL(KDummyFieldUid);
	CTextField* field3 = TheFieldSetCopy->NewFieldL(KDummyFieldUid);
	ret=TheFieldSetCopy->InsertFieldL(20,field2,KDummyFieldUid); 
		test(ret==KErrNone);
	ret=TheFieldSetCopy->InsertFieldL(30,field3,KDummyFieldUid); 
		test(ret==KErrNone);
	ret=UpdateField(20,TheFieldSetCopy);
		test(ret);
	ret=UpdateField(33,TheFieldSetCopy);
		test(ret);

	// test streaming
	testStoreRestore(TheFieldSetCopy,TheFieldSetOriginal);
	test(IsEqual(TheFieldSetCopy,TheFieldSetOriginal));
	TheFieldSetOriginal->Reset();
	TheFieldSetCopy->Reset();
	}


void CT_STREAM::test5()
// Streams a field set containing an out of date field
//
	{
	INFO_PRINTF1(_L("- streaming an out of date field"));

	// insert a block of text into the original
	TheFieldSetOriginal->NotifyInsertion(0,100); // pos=0, len=100
	// insert a field into the original
	CTextField* field=TheFieldSetOriginal->NewFieldL(KDummyFieldUid);
		test(field!=NULL);
	TInt ret=TheFieldSetOriginal->InsertFieldL(10,field,KDummyFieldUid); // pos=10
		test(ret==KErrNone);
	ret=UpdateField(10,TheFieldSetOriginal);
		test(ret);
	// invalidate the field
	TheFieldSetOriginal->NotifyInsertion(11,7); // pos=11, len=7

	// test streaming
	testStoreRestore(TheFieldSetCopy,TheFieldSetOriginal);
	test(IsEqual(TheFieldSetCopy,TheFieldSetOriginal));
	TheFieldSetOriginal->Reset();
	TheFieldSetCopy->Reset();
	}


void CT_STREAM::test6()
// Tests that copy/paste methods exist
//
	{
	INFO_PRINTF1(_L("- testing copy/paste methods with empty array"));
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,0,0,0); // copyPos,Len,PastePos
	test(IsEqual(TheFieldSetCopy,TheFieldSetOriginal));
	TheFieldSetOriginal->Reset();
	TheFieldSetCopy->Reset();
	}


void CT_STREAM::test7()
// Tests copy/paste methods in detail
//
	{
	INFO_PRINTF1(_L("- testing copy/paste methods in detail"));
	// insert a block of text into the original
	TheFieldSetOriginal->NotifyInsertion(0,100); // pos=0, len=100
	// insert a field into the original
	CTextField* field=TheFieldSetOriginal->NewFieldL(KDummyFieldUid);
		test(field!=NULL);
	TInt ret=TheFieldSetOriginal->InsertFieldL(10,field,KDummyFieldUid); // pos=10
		test(ret==KErrNone);
	TBool ok=UpdateField(10,TheFieldSetOriginal);
		test(ok);
		test(TheFieldSetOriginal->CharCount()==103);
	// copy out of that into an empty array
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,5,15,0); // copyPos,Len,PastePos
	// check that the contents of the array are as expected
	TFindFieldInfo info;
	TheFieldSetCopy->FindFields(info,0,15);
		test(TheFieldSetCopy->CharCount()==15);
		test(TheFieldSetCopy->FieldCount()==1);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldPos==5);
		test(info.iFirstFieldLen==3);
	TheFieldSetCopy->Reset();
	// test copying part of a field
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,5,6,0); // copyPos,Len,PastePos
	TheFieldSetCopy->FindFields(info,0,6);
		test(TheFieldSetCopy->CharCount()==6);
		test(TheFieldSetCopy->FieldCount()==0);
		test(info.iFieldCountInRange==0);
	TheFieldSetCopy->Reset();
	// test copying exactly one field
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,10,3,0); // copyPos,Len,PastePos
	TheFieldSetCopy->FindFields(info,0,3);
		test(TheFieldSetCopy->CharCount()==3);
		test(TheFieldSetCopy->FieldCount()==1);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldPos==0);
		test(info.iFirstFieldLen==3);
	TheFieldSetCopy->Reset();
	// test pasting into text in a non-empty array
	TheFieldSetCopy->NotifyInsertion(0,50); // pos=0, len=50
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,5,15,10); // copyPos,Len,PastePos
	TheFieldSetCopy->FindFields(info,0,65);
		test(TheFieldSetCopy->CharCount()==65);
		test(TheFieldSetCopy->FieldCount()==1);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldPos==15);
		test(info.iFirstFieldLen==3);
	TheFieldSetCopy->Reset();
	// test pasting into a field
	TheFieldSetCopy->NotifyInsertion(0,50); // pos=0, len=50
	field=TheFieldSetCopy->NewFieldL(KDummyFieldUid);
		test(field!=NULL);
	ret=TheFieldSetCopy->InsertFieldL(10,field,KDummyFieldUid); // pos=10
		test(ret==KErrNone);
	ok=UpdateField(10,TheFieldSetCopy);
		test(ok);
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,5,15,11); // copyPos,Len,PastePos
	TheFieldSetCopy->FindFields(info,0,68);
		test(TheFieldSetCopy->CharCount()==68);
		test(TheFieldSetCopy->FieldCount()==1);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldPos==10);
		test(info.iFirstFieldLen==18);
	TheFieldSetCopy->Reset();
	// test pasting at the start & end of a field
	TheFieldSetCopy->NotifyInsertion(0,10); // pos=0, len=10
	field=TheFieldSetCopy->NewFieldL(KDummyFieldUid);
		test(field!=NULL);
	ret=TheFieldSetCopy->InsertFieldL(5,field,KDummyFieldUid); // pos=5
		test(ret==KErrNone);
	ok=UpdateField(5,TheFieldSetCopy);
		test(ok);
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,10,3,8); // copyPos,Len,PastePos
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,10,3,5); // copyPos,Len,PastePos
	TheFieldSetCopy->FindFields(info,0,19);
		test(TheFieldSetCopy->CharCount()==19);
		test(TheFieldSetCopy->FieldCount()==3);
		test(info.iFieldCountInRange==3);
		test(info.iFirstFieldPos==5);
		test(info.iFirstFieldLen==3);
	TheFieldSetCopy->FindFields(info,8,11);
		test(info.iFieldCountInRange==2);
		test(info.iFirstFieldPos==8);
		test(info.iFirstFieldLen==3);
	TheFieldSetCopy->FindFields(info,11,8);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldPos==11);
		test(info.iFirstFieldLen==3);
	// tidy up
	TheFieldSetOriginal->Reset();
	TheFieldSetCopy->Reset();
	}


void CT_STREAM::test8()
// Tests paste method with restricted target length
//
	{
	INFO_PRINTF1(_L("- testing paste method with restricted length"));
	// insert a block of text into the original
	TheFieldSetOriginal->NotifyInsertion(0,100); // pos=0, len=100
	// insert 2 fields into the original
	// 1...
	CTextField* field=TheFieldSetOriginal->NewFieldL(KDummyFieldUid);
		test(field!=NULL);
	TInt ret=TheFieldSetOriginal->InsertFieldL(10,field,KDummyFieldUid); // pos=10
		test(ret==KErrNone);
	TBool ok=UpdateField(10,TheFieldSetOriginal);
		test(ok);
	// 2..
	field=TheFieldSetOriginal->NewFieldL(KDummyFieldUid);
		test(field!=NULL);
	ret=TheFieldSetOriginal->InsertFieldL(20,field,KDummyFieldUid); // pos=20
		test(ret==KErrNone);
	ok=UpdateField(20,TheFieldSetOriginal);
		test(ok);
	// paste part of original into copy with length greater than required
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,10,5,0,7); // copyPos,Len,PastePos,PasteLen
	TFindFieldInfo info;
	TheFieldSetCopy->FindFields(info,0,5);
		test(TheFieldSetCopy->CharCount()==5);
		test(TheFieldSetCopy->FieldCount()==1);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldPos==0);
		test(info.iFirstFieldLen==3);
	TheFieldSetCopy->Reset();
	// paste part of original into copy with length equal to that required
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,10,5,0,5); // copyPos,Len,PastePos,PasteLen
	TheFieldSetCopy->FindFields(info,0,5);
		test(TheFieldSetCopy->CharCount()==5);
		test(TheFieldSetCopy->FieldCount()==1);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldPos==0);
		test(info.iFirstFieldLen==3);
	TheFieldSetCopy->Reset();
	// paste part of original into copy with length zero
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,10,5,0,0); // copyPos,Len,PastePos,PasteLen
		test(TheFieldSetCopy->CharCount()==0);
		test(TheFieldSetCopy->FieldCount()==0);
	TheFieldSetCopy->Reset();
	// paste part of original into copy with length st field is chopped
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,5,10,0,6); // copyPos,Len,PastePos,PasteLen
	TheFieldSetCopy->FindFields(info,0,6);
		test(TheFieldSetCopy->CharCount()==6);
		test(TheFieldSetCopy->FieldCount()==0);
		test(info.iFieldCountInRange==0);
	TheFieldSetCopy->Reset();
	// paste part of original into copy with length st one field is left off, the other pasted in
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,5,20,0,10); // copyPos,Len,PastePos,PasteLen
	TheFieldSetCopy->FindFields(info,0,10);
		test(TheFieldSetCopy->CharCount()==10);
		test(TheFieldSetCopy->FieldCount()==1);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldPos==5);
		test(info.iFirstFieldLen==3);
	// tidy up
	TheFieldSetOriginal->Reset();
	TheFieldSetCopy->Reset();
	}


void CT_STREAM::test9()
// Tests paste into set with no field factory
// Should convert all pasted fields to text...
//
	{
	INFO_PRINTF1(_L("- testing paste into set with no field factory"));
	TheFieldSetCopy->SetFieldFactory(NULL);
	// insert a block of text into the original
	TheFieldSetOriginal->NotifyInsertion(0,100); // pos=0, len=100
	// insert a field into the original
	CTextField* field=TheFieldSetOriginal->NewFieldL(KDummyFieldUid);
		test(field!=NULL);
	TInt ret=TheFieldSetOriginal->InsertFieldL(10,field,KDummyFieldUid); // pos=10
		test(ret==KErrNone);
	TBool ok=UpdateField(10,TheFieldSetOriginal);
		test(ok);
	// paste part of original into copy
	testCopyPaste(TheFieldSetCopy,TheFieldSetOriginal,5,25,0); // copyPos,Len,PastePos
	TFindFieldInfo info;
	TheFieldSetCopy->FindFields(info,0,25);
		test(TheFieldSetCopy->CharCount()==25);
		test(TheFieldSetCopy->FieldCount()==0);
	// tidy up
	TheFieldSetOriginal->Reset();
	TheFieldSetCopy->Reset();
	TheFieldSetCopy->SetFieldFactory(TheFactory);
	}


void CT_STREAM::test10()
// Streams a field set containing 100 chars & 1 field
//
	{
	INFO_PRINTF1(_L("- streaming CDateTimeField"));

	// insert a block of text into the original
	TheFieldSetOriginal->NotifyInsertion(0,100); // pos=0, len=100
	// insert a field into the original
	CTextField* field=TheFieldSetOriginal->NewFieldL(KDateTimeFieldUid);
		test(field!=NULL);
	TBuf<14> buff = _L("%-A%*I%:1%T%+A");
	((CDateTimeField*)field)->SetFormat(buff); // 01:53pm
	TInt ret=TheFieldSetOriginal->InsertFieldL(10,field,KDateTimeFieldUid); // pos=10
		test(ret==KErrNone);
	ret=UpdateField(10,TheFieldSetOriginal);
		test(ret);
	// value before
	HBufC* miniBuf = HBufC::NewL(5); 
	CleanupStack::PushL(miniBuf);
	ret=TheFieldSetOriginal->NewFieldValueL(miniBuf,10); // pos=10
		test(ret==KErrNone);
	TPtr buf = miniBuf->Des();
	INFO_PRINTF1(_L("  Value before streaming: "));
		INFO_PRINTF1(buf);
		INFO_PRINTF1(_L("\n"));
	// test streaming
	testStoreRestore(TheFieldSetCopy,TheFieldSetOriginal);
	test(IsEqual(TheFieldSetCopy,TheFieldSetOriginal));
	// value after
	ret=TheFieldSetCopy->NewFieldValueL(miniBuf,10); // pos=10
		test(ret==KErrNone);
	buf = miniBuf->Des();
	INFO_PRINTF1(_L("  Value after streaming:  "));
		INFO_PRINTF1(buf);
		INFO_PRINTF1(_L("\n"));
	CleanupStack::PopAndDestroy(); // miniBuf
	TheFieldSetOriginal->Reset();
	TheFieldSetCopy->Reset();
	}


void CT_STREAM::TestStreamingL()
//
// Test streaming of PrintSetup info.
// Stream from one copy to another, then compare
//
	{
	// create the PrintSetups
	INFO_PRINTF1(_L("Streaming CTextFieldSet"));

	// instantiate the factory and FieldSet
	TheFactory = new(ELeave) TTestFieldFactory();
	TheFieldSetOriginal = CTextFieldSet::NewL();
	TheFieldSetOriginal->SetFieldFactory(TheFactory);
	TheFieldSetCopy = CTextFieldSet::NewL();
	TheFieldSetCopy->SetFieldFactory(TheFactory);

	// Use "original" with default settings & test stream
	test2();

	// change data in original and test stream again
	test3();

	// Stream a field set into a non-empty target
	test4();

	// Stream a field set containing an out of date field
	test5();

	// Test that copy/paste methods exist
	test6();

	// Test copy/paste methods in detail
	test7();

	// Test paste with limited target area
	test8();

	// Test paste into set with no field factory
	test9();

	// Test streaming of built-in types
	test10();

	// end
	delete TheFieldSetOriginal;
	delete TheFieldSetCopy;
	delete TheFactory;
	}


void CT_STREAM::setupCleanup()
//
// Initialise the cleanup stack.
//
    {

	TheTrapCleanup=CTrapCleanup::New();
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)1);\
		test(r==KErrNone);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	}


void CT_STREAM::DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}


CT_STREAM::CT_STREAM()
    {
    SetTestStepName(KTestStep_T_STREAM);
    }

TVerdict CT_STREAM::doTestStepL()
    {
	INFO_PRINTF1(_L("T_STREAM - Fields Persistence"));
    SetTestStepResult(EFail);

    __UHEAP_MARK;

	setupCleanup();
    
    TRAPD(error1, TestStreamingL());

	delete TheTrapCleanup;

    __UHEAP_MARKEND;
	DeleteDataFile(KTFieldOutputFile);	//deletion of data files must be before call to End() - DEF047652

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
