/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "T_FIELD1.h"
#include "TESTFAC.H"
#include <fldset.h>
#include <flddef.h>

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

#define UNUSED_VAR(a) a = a

const TInt KTestCleanupStack=0x40;

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D CTextFieldSet* TheFieldSet;
LOCAL_D TTestFieldFactory* TheFactory;


TBool CT_FIELD1::UpdateField(TInt aPos)
	{
	// find out which field aPos is in
	TFindFieldInfo info;
	TBool inField=TheFieldSet->FindFields(info,aPos);
		test(inField);
	// get the new value
	HBufC* buf = HBufC::NewLC(5); 
 	TInt ret=TheFieldSet->NewFieldValueL(buf,aPos); 
	CleanupStack::Pop();
	CleanupStack::PushL(buf);
		test(ret==KErrNone);
	// Notify FieldSet of update
	TheFieldSet->NotifyFieldUpdate(aPos,buf->Length());
	// tidy up
	CleanupStack::PopAndDestroy();
	return ret;
	}


void CT_FIELD1::test1()
// Tests that all CTextFieldSet methods exist as specced
//
	{
	INFO_PRINTF1(_L("- Testing that all CTextFieldSet methods exist"));
	// inform TheFieldSet about an imaginary insertion, then an imaginary deletion
	TheFieldSet->NotifyInsertion(0,10); // pos=0, len=10
	TheFieldSet->NotifyDeletion(2,5); // pos=2, len=5
	// insert a field & get its initial value
	CTextField* field = TheFieldSet->NewFieldL(KDummyFieldUid);
	TInt ret = TheFieldSet->InsertFieldL(1,field,KDummyFieldUid); // pos=1
		test(ret==KErrNone);
	HBufC* buf = HBufC::NewLC(20); // max length 20
	TheFieldSet->NewFieldValueL(buf,1); // pos=1
	CleanupStack::Pop();
	CleanupStack::PushL(buf);
	TheFieldSet->NotifyFieldUpdate(1,buf->Length());
	CleanupStack::PopAndDestroy(); // buf
	// count number of fields & characters
	TheFieldSet->FieldCount();
	TheFieldSet->CharCount();
	// find the inserted field
	TheFieldSet->FindFields(0); // pos=0
	TFindFieldInfo info;
	TheFieldSet->FindFields(info,0,3); // pos=0, range=3
	// remove the field
	TheFieldSet->RemoveField(2);
	// reset the field array
	TheFieldSet->Reset();
	}


void CT_FIELD1::test2()
// Tests inserting, updating and removing a field
//
	{
	INFO_PRINTF1(_L("- Testing field insertion, update and removal"));
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	// insert a field
	CTextField* field = TheFieldSet->NewFieldL(KDummyFieldUid);
	TInt ret=TheFieldSet->InsertFieldL(0,field,KDummyFieldUid); // pos=0
		test(ret==KErrNone);
	test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==1);
	// get its initial value
	ret=UpdateField(0);
		test(ret==KErrNone);
		test(TheFieldSet->CharCount()==3);
		test(TheFieldSet->FieldCount()==1);
	// get its value in a buffer of insufficient size
	HBufC* miniBuf = HBufC::NewLC(0); 
	ret=TheFieldSet->NewFieldValueL(miniBuf,0); // pos=0
	CleanupStack::Pop();
	CleanupStack::PushL(miniBuf);
		test(ret==KErrNone);
	// delete part of the field & update
	TheFieldSet->NotifyDeletion(1,1); // pos=1, len=1
		test(TheFieldSet->CharCount()==2);
		test(TheFieldSet->FieldCount()==1);
	ret=UpdateField(0);
		test(TheFieldSet->CharCount()==3);
		test(TheFieldSet->FieldCount()==1);
	// delete over a zero length
	TheFieldSet->NotifyDeletion(1,0); // pos=1, len=0
		test(TheFieldSet->CharCount()==3);
		test(TheFieldSet->FieldCount()==1);
	TFindFieldInfo info;
	TBool inField=TheFieldSet->FindFields(info,0);
		test(inField);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==3);
		test(info.iFirstFieldPos==0);
	// delete all the contents of the field & update
	TheFieldSet->NotifyDeletion(0,3); // pos=0, len=3
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
/*	ret=UpdateField(0);
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	ret=TheFieldSet->RemoveField(0); // pos=0
		test(ret);
		test(TheFieldSet->CharCount()==3);
		test(TheFieldSet->FieldCount()==0);
*/	// reset the field array
	TheFieldSet->Reset();
	CleanupStack::PopAndDestroy(); // miniBuf
	}


void CT_FIELD1::test3()
// Tests InField() in "all" the oddball situations
//
	{
	TFindFieldInfo info;
	INFO_PRINTF1(_L("- Testing InField() calls"));
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	// Zero length doc
	TBool inField=TheFieldSet->FindFields(info,0);
		test(!inField);
		test(info.iFieldCountInRange==0);
		test(info.iFirstFieldLen==0);
		test(info.iFirstFieldPos==0);
	// Insert field (zero length) & test before
	CTextField* field = TheFieldSet->NewFieldL(KDummyFieldUid);
	TInt ret=TheFieldSet->InsertFieldL(0,field,KDummyFieldUid); // pos=0
		test(ret==KErrNone);
	inField=TheFieldSet->FindFields(info,0);
		test(inField);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==0);
		test(info.iFirstFieldPos==0);
	// Insert another field (zero length) & test before
	CTextField* field2 = TheFieldSet->NewFieldL(KDummyFieldUid);
	ret=TheFieldSet->InsertFieldL(0,field2,KDummyFieldUid); // pos=0
		test(ret==KErrNone);
	inField=TheFieldSet->FindFields(info,0);
		test(inField);
		test(info.iFieldCountInRange==2);
		test(info.iFirstFieldLen==0);
		test(info.iFirstFieldPos==0);
	// Get the value of the first field, test before & test between them
	ret=UpdateField(0);
		test(ret==KErrNone);
	inField=TheFieldSet->FindFields(info,0);
		test(inField);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==3);
		test(info.iFirstFieldPos==0);
	inField=TheFieldSet->FindFields(info,3);
		test(inField);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==0);
		test(info.iFirstFieldPos==3);
	// Get the value of the second field, test between them
	ret=UpdateField(3);
		test(ret==KErrNone);
	inField=TheFieldSet->FindFields(info,3);
		test(inField);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==3);
		test(info.iFirstFieldPos==3);
	// end of doc - after field
	inField=TheFieldSet->FindFields(info,6);
		test(!inField);
		test(info.iFieldCountInRange==0);
		test(info.iFirstFieldLen==0);
		test(info.iFirstFieldPos==0);
	// In field
	inField=TheFieldSet->FindFields(info,2);
		test(inField);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==3);
		test(info.iFirstFieldPos==0);
	// Other method at same pos
	inField=TheFieldSet->FindFields(2);
		test(inField);
	// Pretend to insert some text between the fields
	TheFieldSet->NotifyInsertion(3,5); // pos=3, len=5
		test(TheFieldSet->CharCount()==11);
		test(TheFieldSet->FieldCount()==2);
	// Test in text
	inField=TheFieldSet->FindFields(info,6);
		test(info.iFieldCountInRange==0);
		test(info.iFirstFieldLen==0);
		test(info.iFirstFieldPos==0);
	// Test from field to field over the intervening text
	inField=TheFieldSet->FindFields(info,1,9);
		test(info.iFieldCountInRange==2);
		test(info.iFirstFieldLen==3);
		test(info.iFirstFieldPos==0);
	// Test from field 1 up to field 2
	inField=TheFieldSet->FindFields(info,0,8);
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==3);
		test(info.iFirstFieldPos==0);
	TheFieldSet->Reset();
	}


void CT_FIELD1::test4()
// Tests Insertion and deletion of text around and into fields
//
	{
	TFindFieldInfo info;
	INFO_PRINTF1(_L("- Testing insertion and deletion of text around and into fields"));
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	// Insert some text
	TheFieldSet->NotifyInsertion(0,4); // pos=0, len=4
		test(TheFieldSet->CharCount()==4);
		test(TheFieldSet->FieldCount()==0);
	// Insert some text into the text
	TheFieldSet->NotifyInsertion(2,6); // pos=2, len=6
		test(TheFieldSet->CharCount()==10);
		test(TheFieldSet->FieldCount()==0);
	// Insert a field into the text
	CTextField* field = TheFieldSet->NewFieldL(KDummyFieldUid);
	TInt ret=TheFieldSet->InsertFieldL(4,field,KDummyFieldUid); // pos=4
	if(ret != KErrNone && field !=NULL)
		{
		delete field;
		field = NULL;
		}
		test(ret==KErrNone);
	ret=UpdateField(4);
		test(ret==KErrNone);
		test(TheFieldSet->CharCount()==13);
		test(TheFieldSet->FieldCount()==1);
	// Insert some text directly before the field (check it hasn't inserted into the field)
	TheFieldSet->NotifyInsertion(4,2); // pos=4, len=2
		test(TheFieldSet->CharCount()==15);
		test(TheFieldSet->FieldCount()==1);
	TBool inField=TheFieldSet->FindFields(info,7); // pos=7
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==3);
		test(info.iFirstFieldPos==6);
	// Insert some text directly after the field
	TheFieldSet->NotifyInsertion(9,1); // pos=9, len=1
		test(TheFieldSet->CharCount()==16);
		test(TheFieldSet->FieldCount()==1);
	inField=TheFieldSet->FindFields(info,7); // pos=7
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==3);
		test(info.iFirstFieldPos==6);
	// Insert some text into the field
	TheFieldSet->NotifyInsertion(7,4); // pos=9, len=4
		test(TheFieldSet->CharCount()==20);
		test(TheFieldSet->FieldCount()==1);
	inField=TheFieldSet->FindFields(info,7); // pos=7
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==7);
		test(info.iFirstFieldPos==6);
	// Delete some text directly after the field
	TheFieldSet->NotifyDeletion(13,1); // pos=13, len=1
		test(TheFieldSet->CharCount()==19);
		test(TheFieldSet->FieldCount()==1);
	inField=TheFieldSet->FindFields(info,9); // pos=9
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==7);
		test(info.iFirstFieldPos==6);
	// Delete some text before the field
	TheFieldSet->NotifyDeletion(4,2); // pos=4, len=2
		test(TheFieldSet->CharCount()==17);
		test(TheFieldSet->FieldCount()==1);
	inField=TheFieldSet->FindFields(info,7); // pos=7
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==7);
		test(info.iFirstFieldPos==4);
	// Delete some text overlapping into the field
	TheFieldSet->NotifyDeletion(1,5); // pos=1, len=5
		test(TheFieldSet->CharCount()==12);
		test(TheFieldSet->FieldCount()==1);
	inField=TheFieldSet->FindFields(info,3); // pos=3
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==5);
		test(info.iFirstFieldPos==1);
	// Delete some text overlapping out of the field
	TheFieldSet->NotifyDeletion(3,4); // pos=3, len=4
		test(TheFieldSet->CharCount()==8);
		test(TheFieldSet->FieldCount()==1);
	inField=TheFieldSet->FindFields(info,2); // pos=2
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==2);
		test(info.iFirstFieldPos==1);
	// Delete all text, inc field
	TheFieldSet->NotifyDeletion(0,8); // pos=0, len=8
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
		
	//Adding some character & field at the end & then deleting all
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	// Insert some text
	TheFieldSet->NotifyInsertion(0,4); // pos=0, len=4
		test(TheFieldSet->CharCount()==4);
		test(TheFieldSet->FieldCount()==0);
	// Insert some text into the text
	TheFieldSet->NotifyInsertion(2,7); // pos=2, len=7
		test(TheFieldSet->CharCount()==11);
		test(TheFieldSet->FieldCount()==0);
	//Insert field at the last position
	CTextField* field1 = TheFieldSet->NewFieldL(KDummyFieldUid);
	ret=TheFieldSet->InsertFieldL(5,field1,KDummyFieldUid); // pos=5
	if(ret != KErrNone && field1 !=NULL)
		{
		delete field1;
		field1 = NULL;
		}
		test(ret==KErrNone);
	ret=UpdateField(5);
	//Delete last character
	TheFieldSet->NotifyDeletion(10,1); // pos=10, len=1
		test(TheFieldSet->CharCount()==13);
		test(TheFieldSet->FieldCount()==1);
	//Insert characters at the field position so field moves to 8th pos
	TheFieldSet->NotifyInsertion(5,3); // pos=5, len=3
		test(TheFieldSet->CharCount()==16);
		test(TheFieldSet->FieldCount()==1);
	inField=TheFieldSet->FindFields(info,8); // pos=8
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==3);
		test(info.iFirstFieldPos==8);
	//Delete last character
	TheFieldSet->NotifyDeletion(15,1); // pos=15, len=1
		test(TheFieldSet->CharCount()==15);
		test(TheFieldSet->FieldCount()==1);
	//Delete out of range character that doesnt exist..
	TheFieldSet->NotifyDeletion(15,1); // pos=15, len=1
		test(TheFieldSet->CharCount()==15);
		test(TheFieldSet->FieldCount()==1);
	//Delete all the characters after the field
	TheFieldSet->NotifyDeletion(11, 4); // pos=11, len=4
		test(TheFieldSet->CharCount()==11);
		test(TheFieldSet->FieldCount()==1);
	//Shorten the field by deleting the last character of the field
	TheFieldSet->NotifyDeletion(10, 1); // pos=10, len=1
		test(TheFieldSet->CharCount()==10);
		test(TheFieldSet->FieldCount()==1);
	inField=TheFieldSet->FindFields(info,8); // pos=0
		test(info.iFieldCountInRange==1);
		test(info.iFirstFieldLen==2);
		test(info.iFirstFieldPos==8);
	//length to be removed is one more than the existing data..DEF095911
	TheFieldSet->NotifyDeletion(0,11); // pos=0, len=11
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	inField=TheFieldSet->FindFields(info,0); // pos=0
		test(info.iFieldCountInRange==0);
		test(info.iFirstFieldLen==0);
		test(info.iFirstFieldPos==0);

	// Finish up
	TheFieldSet->Reset();
	}


void CT_FIELD1::test5()
// Tests inserting, updating and removing a CDateTimeField
//
	{
	INFO_PRINTF1(_L("- Testing CDateTimeField"));
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	// insert a field with default format
	CTextField* field = TheFieldSet->NewFieldL(KDateTimeFieldUid);
	TInt ret=TheFieldSet->InsertFieldL(0,field,KDateTimeFieldUid); // pos=0
		test(ret==KErrNone);
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==1);
	// get its initial value
	ret=UpdateField(0);
		test(ret==KErrNone);
		test(TheFieldSet->FieldCount()==1);
	// get its value and display it
	HBufC* miniBuf = HBufC::NewLC(5); 
	ret=TheFieldSet->NewFieldValueL(miniBuf,0); // pos=0
	CleanupStack::Pop();
	CleanupStack::PushL(miniBuf);
		test(ret==KErrNone);
	TPtr buf = miniBuf->Des();
	INFO_PRINTF1(_L("  The field value is: "));
	INFO_PRINTF1(buf);
	INFO_PRINTF1(_L("\n"));
	// reset the field array
	TheFieldSet->Reset();
	CleanupStack::PopAndDestroy(); // miniBuf
	}


void CT_FIELD1::test6()
// Tests CDateTimeFields with non-default formatting
//
	{
	INFO_PRINTF1(_L("- Testing CDateTimeField with non-default formatting"));
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	// create a field and set format
	CTextField* field = TheFieldSet->NewFieldL(KDateTimeFieldUid);
	((CDateTimeField*)field)->SetFormat(_L("%-A%*I%:1%T%+A")); //
	// insert field
	TInt ret=TheFieldSet->InsertFieldL(0,field,KDateTimeFieldUid); // pos=0
		test(ret==KErrNone);
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==1);
	// get its initial value
	ret=UpdateField(0);
		test(ret==KErrNone);
//		test(TheFieldSet->CharCount()==5);
		test(TheFieldSet->FieldCount()==1);
	// get its value and display it
	HBufC* miniBuf = HBufC::NewLC(5); 
	ret=TheFieldSet->NewFieldValueL(miniBuf,0); // pos=0
	CleanupStack::Pop();
	CleanupStack::PushL(miniBuf);
		test(ret==KErrNone);
	TPtr buf = miniBuf->Des();
	INFO_PRINTF1(_L("  The field value is: "));
	INFO_PRINTF1(buf);
	INFO_PRINTF1(_L("\n"));
	// reset the field array
	TheFieldSet->Reset();
	CleanupStack::PopAndDestroy(); // miniBuf
	}


void CT_FIELD1::test7()
// Tests inserting, updating and removing a CPageNumberField
//
	{
	INFO_PRINTF1(_L("- Testing CPageNumberField"));
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	// insert a field with default format
	CTextField* field = TheFieldSet->NewFieldL(KPageNumberFieldUid);
	TInt ret=TheFieldSet->InsertFieldL(0,field,KPageNumberFieldUid); // pos=0
		test(ret==KErrNone);
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==1);
	// get its initial value
	ret=UpdateField(0);
		test(ret==KErrNone);
		test(TheFieldSet->FieldCount()==1);
	// get its value and display it
	HBufC* miniBuf = HBufC::NewLC(5); 
	ret=TheFieldSet->NewFieldValueL(miniBuf,0); // pos=0
	CleanupStack::Pop();
	CleanupStack::PushL(miniBuf);
		test(ret==KErrNone);
	TPtr buf = miniBuf->Des();
	INFO_PRINTF1(_L("  The field value is: "));
	INFO_PRINTF1(buf);
	INFO_PRINTF1(_L("\n"));
	// reset the field array
	TheFieldSet->Reset();
	CleanupStack::PopAndDestroy(); // miniBuf
	}


void CT_FIELD1::test8()
// Tests inserting, updating and removing a CPageNumberField
//
	{
	INFO_PRINTF1(_L("- Testing CFileNameField"));
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==0);
	// insert a field with default format
	CTextField* field = TheFieldSet->NewFieldL(KFileNameFieldUid);
	TInt ret=TheFieldSet->InsertFieldL(0,field,KFileNameFieldUid); // pos=0
		test(ret==KErrNone);
		test(TheFieldSet->CharCount()==0);
		test(TheFieldSet->FieldCount()==1);
	// get its initial value
	ret=UpdateField(0);
		test(ret==KErrNone);
		test(TheFieldSet->FieldCount()==1);
	// get its value and display it
	HBufC* miniBuf = HBufC::NewLC(5); 
	ret=TheFieldSet->NewFieldValueL(miniBuf,0); // pos=0
	CleanupStack::Pop();
	CleanupStack::PushL(miniBuf);
		test(ret==KErrNone);
	TPtr buf = miniBuf->Des();
	INFO_PRINTF1(_L("  The field value is: "));
	INFO_PRINTF1(buf);
	INFO_PRINTF1(_L("\n"));
	// reset the field array
	TheFieldSet->Reset();
	CleanupStack::PopAndDestroy(); // miniBuf
	}


/*
Added to test the fix to 'EDNHARN-4NVDHC: Text fields are not removed correctly in edwin'. Deletion
of two fields failed to delete the second one.
*/
void CT_FIELD1::test_multiple_fields()
	{
	INFO_PRINTF1(_L(" - testing multiple fields"));
	test(TheFieldSet->CharCount()==0);
	test(TheFieldSet->FieldCount()==0);

	/*
	Insert two dummy fields and some imaginary text. Dummy fields have the value 'XXX'.
	If the text is represented by 'x's the text will be: "xxXXXxXXXxxxx".
	*/
	CTextField* field1 = TheFieldSet->NewFieldL(KDummyFieldUid);
	TInt ret = TheFieldSet->InsertFieldL(0,field1,KDummyFieldUid);
	test(ret == KErrNone);
	test(TheFieldSet->FieldCount() == 1);
	ret = UpdateField(0);
	test(ret == KErrNone);
	CTextField* field2 = TheFieldSet->NewFieldL(KDummyFieldUid);
	ret = TheFieldSet->InsertFieldL(3,field2,KDummyFieldUid);
	test(ret == KErrNone);
	test(TheFieldSet->FieldCount() == 2);
	ret = UpdateField(3);
	test(ret == KErrNone);
	TheFieldSet->NotifyInsertion(0,2);
	TheFieldSet->NotifyInsertion(5,1);
	TheFieldSet->NotifyInsertion(9,4);
	test(TheFieldSet->CharCount() == 13);

	// Delete the two fields and the character between them.
	TheFieldSet->NotifyDeletion(2,7);
	test(TheFieldSet->FieldCount() == 0);
	test(TheFieldSet->CharCount() == 6);

	TheFieldSet->Reset();
	}


void CT_FIELD1::testFields()
// Test the fields dll.
//
    {
	INFO_PRINTF1(_L("Testing Fields"));
	
	// instantiate the  FieldSet and use the default factory
	TheFactory = new(ELeave) TTestFieldFactory();
	TheFieldSet = CTextFieldSet::NewL();
	TheFieldSet->SetFieldFactory(TheFactory);
	
	// the first test just checks that all methods exist (a la Duncan)
	test1();
	// Tests inserting, updating and removing a field
	test2();
	// Tests FindFields() & InField() in "all" the oddball situations
	test3();
	// Tests Insertion and deletion of text around and into fields
	test4();
	// Tests the CDateTimeField class
	test5();
	// Tests the CDateTimeField class with non-default formatting
	test6();
	// Test CPageNumberField
	test7();
	// Test CFileNameField
	test8();
	// Test multiple field insertion and deletion.
	test_multiple_fields();
	
	// clean up
	delete TheFieldSet;
	delete TheFactory;
	//CleanupStack::PopAndDestroy(); 
    }


void CT_FIELD1::setupCleanup()
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

CT_FIELD1::CT_FIELD1()
    {
    SetTestStepName(KTestStep_T_FIELD1);
    }

TVerdict CT_FIELD1::doTestStepL()
    {
	INFO_PRINTF1(_L("Testing Fields"));
    SetTestStepResult(EFail);

    __UHEAP_MARK;

	setupCleanup();
    
    TRAPD(error1, testFields());

	delete TheTrapCleanup;

    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
