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


#include <txtrich.h>
#include <txtfmlyr.h>
#include <s32mem.h>
#include <s32file.h>
#include <flddef.h>
#include <fldbltin.h>
#include "../incp/T_PMLPAR.H"
#include "T_CONVRT.h"

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

const TInt KTestCleanupStack=0x20;

LOCAL_D CTrapCleanup* TheTrapCleanup;
//
LOCAL_D CRichText* TheText=NULL;
LOCAL_D CParaFormatLayer* TheGlobalParaLayer=NULL;
LOCAL_D CCharFormatLayer* TheGlobalCharLayer=NULL;


////////////////////////////////////////////////////////////////////////////////////////////
class TTestFieldFactoryCONVRT : public MTextFieldFactory
	{
public:
	// from MTextFieldFactory
	virtual CTextField* NewFieldL(TUid aFieldType); 
	// Creates a field of the type specified
	// Returns NULL if it does not recognise/support the field type
	};

CTextField* TTestFieldFactoryCONVRT::NewFieldL(TUid aFieldType)
// Creates a field (in aHeader) of the type specified in aHeader
// 
	{
	CTextField* field=NULL;
	if (aFieldType==KDateTimeFieldUid)
		field = (CTextField*)new(ELeave) CDateTimeField();
	return field;
	}
/////////////////////////////////////////////////////////////////////////////////////////////

_LIT(KOutputFile, "c:\\etext\\t_convrt.tst");
template <class T>
void CT_CONVRT::testStoreRestoreL(T& aCopy,const T& aOriginal)
// Test document persistance.
//
    {
	// set up the store
	RFs	theFs;
	theFs.Connect();
	//
	theFs.Delete(KOutputFile);
	theFs.MkDirAll(KOutputFile);
	CFileStore* theStore = CDirectFileStore::CreateL(theFs, KOutputFile, EFileRead | EFileWrite);
	CleanupStack::PushL(theStore);
	theStore->SetTypeL(KDirectFileStoreLayoutUid);
	//
	// store the original
	TStreamId id(0);
	TRAPD(ret,id=aOriginal.StoreL(*theStore));
		test(ret==KErrNone);
	//
	// restore into the copy
	TRAP(ret,aCopy.RestoreL(*theStore,id));
		test(ret==KErrNone);
	//
	// tidy up
	CleanupStack::PopAndDestroy();  // theStore
	theFs.Close();
    }


TInt CT_CONVRT::IsEqual(const CEditableText* aCopy,const CEditableText* aOriginal)
//
// Returns true if aCopy contents matches aOriginal contents.
// Takes account of multiple segments of a segmented text component.
//
	{
	TInt lengthOfOriginal=aOriginal->DocumentLength();
	TInt lengthOfCopy=aCopy->DocumentLength();
	test(lengthOfOriginal==lengthOfCopy);
//
	TPtrC copy,orig;
//
	TInt lengthRead=0;
	while(lengthRead<=lengthOfOriginal)
		{
		copy.Set((aCopy->Read(lengthRead)));
		orig.Set((aOriginal->Read(lengthRead)));
		for (TInt offset=0; offset<copy.Length(); offset++)
			test(copy[offset]==orig[offset]);
		lengthRead+=copy.Length();
		}
	test(lengthRead==lengthOfOriginal+1);
	return 1;
	}


void CT_CONVRT::DoTestRichTextL()
//
// Test streaming CRichText.
//
	{// Create the global text components.
	const TInt KSmallestTextBuffer=1;
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	//	
	CRichText* theCopy=NULL;
	//
	TInt biggest;
	TInt size=User::Available(biggest); 
	//
	TRAPD(ret,
	theCopy=CRichText::NewL(paraLayer,charLayer,CEditableText::ESegmentedStorage,KSmallestTextBuffer));
	//
	TInt newsize=User::Available(biggest);
	TInt footprint=size-newsize;
	//
	size=User::Available(biggest);
	CGlobalText* gText=NULL;
	TRAP(ret,
	gText=CGlobalText::NewL(paraLayer,charLayer,CEditableText::ESegmentedStorage,KSmallestTextBuffer));
	newsize=User::Available(biggest);
	TInt globalPrint=size-newsize;
	TBuf<50> buf;
	buf.Format(_L("Empty rich text takes: %d bytes\n"),footprint);
	INFO_PRINTF1(buf);
	buf.Format(_L("Empty global text takes: %d bytes\n"),globalPrint);
	INFO_PRINTF1(buf);
//	test.Getch();
	delete gText;
	//
	// Now add a text field to this.
	TTestFieldFactoryCONVRT factory;
	TheText->SetFieldFactory(&factory);
	theCopy->SetFieldFactory(&factory);
	CTextField* field=NULL;
	TRAP(ret,
	field=factory.NewFieldL(KDateTimeFieldUid));
	test(ret==KErrNone);
	TRAP(ret,
	TheText->InsertFieldL(0,field,KDateTimeFieldUid));
	test(ret==KErrNone);
	TRAP(ret,
	TheText->UpdateFieldL(0));
	test(ret==KErrNone);
	//
	// Do the store/restore and test
	testStoreRestoreL(*theCopy,*TheText);
	test(IsEqual(theCopy,TheText));
	theCopy->Reset();  // lets me see inside the invariant;

	delete theCopy;
	delete paraLayer;
	delete charLayer;
	}


void CT_CONVRT::LoadIntoText(TFileName& aFileName)
//
	{
	CParser* myParser=NULL;
	TRAPD(ret,
	myParser=CParser::NewL());
	test(ret == KErrNone);
	TRAP(ret,
	TheText=myParser->ParseL(aFileName));
	test(ret == KErrNone);
	TheGlobalParaLayer=(CParaFormatLayer*)TheText->GlobalParaFormatLayer();
	TheGlobalCharLayer=(CCharFormatLayer*)TheText->GlobalCharFormatLayer();
	delete myParser;
	}


void CT_CONVRT::KillText()
//
	{
	delete TheText;
	}


void CT_CONVRT::KillLayers()
	{
	delete TheGlobalParaLayer;
	delete TheGlobalCharLayer;
	}


void CT_CONVRT::Reset()
//
	{
	KillText();
	KillLayers();
	}


void CT_CONVRT::GoL()
//
	{
	INFO_PRINTF1(_L("Rich Text of Shared Para Formats Only"));
	TFileName fileName=_L("z:\\test\\app-framework\\etext\\shared.pml");
	LoadIntoText(fileName);
	TBool hasMarkupData=TheText->HasMarkupData();
	test(hasMarkupData);
	DoTestRichTextL();
	Reset();
	//
	INFO_PRINTF1(_L("Rich Text with specific character formatting"));
	fileName=_L("z:\\test\\app-framework\\etext\\test1.pml");
	LoadIntoText(fileName);
	hasMarkupData=TheText->HasMarkupData();
	test(hasMarkupData);
	DoTestRichTextL();
	Reset();
	}


void CT_CONVRT::setupCleanup()
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


void CT_CONVRT::DeleteDataFile(const TDesC& aFullName)
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

CT_CONVRT::CT_CONVRT()
    {
    SetTestStepName(KTestStep_T_CONVRT);
    }

TVerdict CT_CONVRT::doTestStepL()
    {
    SetTestStepResult(EFail);

    setupCleanup();
    __UHEAP_MARK;

    INFO_PRINTF1(_L("T_CONVRT - Rich Text Persistence"));
    INFO_PRINTF1(_L("Persisting Rich Text"));
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-LEGACY-T_CONVRT-0001 "));
    TRAPD(error1, GoL());

    __UHEAP_MARKEND;
    delete TheTrapCleanup;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
