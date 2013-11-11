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


#include <e32std.h>
#include <e32base.h>

#include <gdi.h>
#include <s32file.h>

#include <txtrich.h>
#include <txtfmlyr.h>
#include <txtfrmat.h>
#include <txtstyle.h>
#include "T_STYLE.h"

LOCAL_D CTestStep *pTestStep = NULL;
#define test(cond)											\
	{														\
	TBool __bb = (cond);									\
	pTestStep->TEST(__bb);									\
	if (!__bb)												\
		{													\
		pTestStep->ERR_PRINTF1(_L("ERROR: Test Failed"));	\
		User::Leave(1);										\
		}													\
	}
#undef INFO_PRINTF1
#undef INFO_PRINTF2
// copy from tefexportconst.h
#define INFO_PRINTF1(p1)        pTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1))
#define INFO_PRINTF2(p1, p2)    pTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2))


#define UNUSED_VAR(a) a = a

const TInt KTestCleanupStack=0x40;

LOCAL_D CTrapCleanup* TheTrapCleanup;

LOCAL_D CRichText* TheText;
LOCAL_D CStyleList* TheStyleList;
LOCAL_D CParaFormatLayer* TheNormalParaLayer;
LOCAL_D CCharFormatLayer* TheNormalCharLayer;
LOCAL_D CParagraphStyle* TheStyleOne;
LOCAL_D CParagraphStyle* TheStyleTwo;


_LIT(KOutputFile, "c:\\etext\\t_style.tst");
template <class T>
void testStoreRestoreL(T& aCopy,const T& aOriginal)
// Test document persistance.
//
    {
	// set up the store
	RFs	theFs;
	theFs.Connect();
	//
	theFs.Delete(KOutputFile);
	theFs.MkDirAll(KOutputFile);
	CFileStore* theStore=CDirectFileStore::CreateL(theFs,KOutputFile,EFileRead|EFileWrite);
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


LOCAL_C TInt IsEqual(const CRichText* aCopy,const CRichText* aOriginal)
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
		for (TInt offset=0; offset<orig.Length(); offset++)
			test(copy[offset]==orig[offset]);
		lengthRead+=orig.Length();
		}
	test(lengthRead==lengthOfOriginal+1);
//
	CStyleList* origStyle=aOriginal->StyleList();
	CStyleList* copyStyle=aCopy->StyleList();
	TInt origStyleCount=origStyle->Count();
	TInt copyStyleCount=copyStyle->Count();
	test(origStyleCount==copyStyleCount);
	for (TInt ii=0;ii<origStyleCount;ii++)
		{
		RParagraphStyleInfo oInfo=origStyle->At(ii);
		RParagraphStyleInfo cInfo=copyStyle->At(ii);
		test(oInfo.iStyle->iName==cInfo.iStyle->iName);
		if (oInfo.iStyleForNextPara==NULL)
			test(cInfo.iStyleForNextPara==NULL);
		}

	return 1;
	}


LOCAL_C void ConstructEnvWithNullParaFormat()
	{
	// Create global layers
	CParaFormat* normalPara=CParaFormat::NewLC();
	TParaFormatMask paraFormatMask;
	normalPara->iHorizontalAlignment=CParaFormat::ELeftAlign;
	paraFormatMask.SetAttrib(EAttAlignment);
	TheNormalParaLayer=CParaFormatLayer::NewL(normalPara,paraFormatMask);
	CleanupStack::PopAndDestroy();  // normalPara
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	TheNormalCharLayer=CCharFormatLayer::NewL(charFormat,charFormatMask);
	//
	// Create some paragraph styles
	TheStyleOne=CParagraphStyle::NewL(*TheNormalParaLayer,*TheNormalCharLayer);
	TheStyleOne->iName=_L("Style1");
	
	//	Style two is based on style one
	TheStyleTwo=CParagraphStyle::NewL( *TheStyleOne, *(TheStyleOne->CharFormatLayer()));
	TheStyleTwo->iName=_L("Style2");
	//
	CParaFormat* styleFormat=CParaFormat::NewLC();
	TParaFormatMask styleMask;
	styleFormat->iHorizontalAlignment=CParaFormat::ECenterAlign;
	styleMask.SetAttrib(EAttAlignment);
	TheStyleOne->SetL(styleFormat,styleMask);
	//
	styleFormat->iHorizontalAlignment=CParaFormat::ERightAlign;
	//	Set paragraph format to NULL
	TheStyleTwo->SetL( NULL,styleMask);	
	CleanupStack::PopAndDestroy();  // styleFormat
	//
	// Create style table and insert styles.
	TheStyleList=CStyleList::NewL();	
	RParagraphStyleInfo info(TheStyleOne);
	TInt error=TheStyleList->AppendL(&info);
	test(error==KErrNone);
	RParagraphStyleInfo info1=TheStyleList->At(0);
	CParagraphStyle* style=info1.iStyle;
	style=NULL;

	RParagraphStyleInfo info2(TheStyleTwo,TheStyleOne);
	error=TheStyleList->AppendL(&info2);
	test(error==KErrNone);
	
	error=TheStyleList->AppendL(&info2);
	test(error==KErrAlreadyExists);
	test(TheStyleList->Count()==2);
	
	style=TheStyleList->At(1).iStyle;
	test(style->iName==_L("Style2"));
	//
	// Create the rich text with styles.
	TheText=CRichText::NewL(TheNormalParaLayer,TheNormalCharLayer,*TheStyleList);
}

LOCAL_C void ConstructEnvironment()
// Create some styles.
//
	{
	// Create global layers
	CParaFormat* normalPara=CParaFormat::NewLC();
	TParaFormatMask paraFormatMask;
	normalPara->iHorizontalAlignment=CParaFormat::ELeftAlign;
	paraFormatMask.SetAttrib(EAttAlignment);
	TheNormalParaLayer=CParaFormatLayer::NewL(normalPara,paraFormatMask);
	CleanupStack::PopAndDestroy();  // normalPara
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	TheNormalCharLayer=CCharFormatLayer::NewL(charFormat,charFormatMask);
	//
	// Create some paragraph styles
	TheStyleOne=CParagraphStyle::NewL(*TheNormalParaLayer,*TheNormalCharLayer);
	TheStyleOne->iName=_L("Style1");
	TheStyleTwo=CParagraphStyle::NewL(*TheNormalParaLayer,*TheNormalCharLayer);
	TheStyleTwo->iName=_L("Style2");
	//
	CParaFormat* styleFormat=CParaFormat::NewLC();
	TParaFormatMask styleMask;
	styleFormat->iHorizontalAlignment=CParaFormat::ECenterAlign;
	styleMask.SetAttrib(EAttAlignment);
	TheStyleOne->SetL(styleFormat,styleMask);
	//
	styleFormat->iHorizontalAlignment=CParaFormat::ERightAlign;
	TheStyleTwo->SetL(styleFormat,styleMask);
	CleanupStack::PopAndDestroy();  // styleFormat
	//
	// Create style table and insert styles.
	TheStyleList=CStyleList::NewL();	
	RParagraphStyleInfo info(TheStyleOne);
	TInt error=TheStyleList->AppendL(&info);
	test(error==KErrNone);
	RParagraphStyleInfo info1=TheStyleList->At(0);
	CParagraphStyle* style=info1.iStyle;
	style=NULL;

	RParagraphStyleInfo info2(TheStyleTwo,TheStyleOne);
	error=TheStyleList->AppendL(&info2);
	test(error==KErrNone);
	
	error=TheStyleList->AppendL(&info2);
	test(error==KErrAlreadyExists);
	test(TheStyleList->Count()==2);
	
	style=TheStyleList->At(1).iStyle;
	test(style->iName==_L("Style2"));
	//
	// Create the rich text with styles.
	TheText=CRichText::NewL(TheNormalParaLayer,TheNormalCharLayer,*TheStyleList);
	}


LOCAL_C void KillEnvironment()
// Kill everything
//
	{
	delete TheText;
	// the style table is owned by the rich text, and is destroyed there.
	delete TheNormalParaLayer;
	delete TheNormalCharLayer;
	}


LOCAL_C void TestConstruction()
// Test the construction/destruction of rich text with styles
//
	{
	__UHEAP_MARK;

	ConstructEnvironment();
	KillEnvironment();

	__UHEAP_MARKEND;
	}


LOCAL_C void TestParaWithNullParaFormat()
	{
	CParagraphStyle::TApplyParaStyleMode applyMode=CParagraphStyle::ERetainNoSpecificFormats;

	INFO_PRINTF1(_L("Apply style to paragraph with NULL para format"));
	ConstructEnvWithNullParaFormat();
	TheText->InsertL(0,_L("HEADINGBODYTEXT"));
	//
	TheText->ApplyParagraphStyleL(*TheStyleList->At(1).iStyle,0,1,applyMode);
	CParaFormat* paraFormat=CParaFormat::NewLC();
	TheText->GetParagraphFormatL(paraFormat,0);
	test(paraFormat->iHorizontalAlignment==CParaFormat::ECenterAlign);
	//
	TChar delimiter=CEditableText::EParagraphDelimiter;
	TheText->InsertL(7,delimiter);
	TheText->GetParagraphFormatL(paraFormat,6);
	test(paraFormat->iHorizontalAlignment==CParaFormat::ECenterAlign);
	TheText->GetParagraphFormatL(paraFormat,8);
	test(paraFormat->iHorizontalAlignment==CParaFormat::ECenterAlign);
	CleanupStack::PopAndDestroy();
	KillEnvironment();	
	}

LOCAL_C void TestSharedPara()
// Test
//
	{
	CParagraphStyle::TApplyParaStyleMode applyMode=CParagraphStyle::ERetainNoSpecificFormats;

	INFO_PRINTF1(_L("Apply style to shared paragraph"));
	ConstructEnvironment();
	TheText->InsertL(0,_L("HEADINGBODYTEXT"));
	//
	TheText->ApplyParagraphStyleL(*TheStyleList->At(0).iStyle,0,1,applyMode);
	CParaFormat* paraFormat=CParaFormat::NewLC();
	TheText->GetParagraphFormatL(paraFormat,0);
	test(paraFormat->iHorizontalAlignment==CParaFormat::ECenterAlign);
	//
	TChar delimiter=CEditableText::EParagraphDelimiter;
	TheText->InsertL(7,delimiter);
	TheText->GetParagraphFormatL(paraFormat,6);
	test(paraFormat->iHorizontalAlignment==CParaFormat::ECenterAlign);
	TheText->GetParagraphFormatL(paraFormat,8);
	test(paraFormat->iHorizontalAlignment==CParaFormat::ECenterAlign);
	CleanupStack::PopAndDestroy();
	KillEnvironment();
	}


LOCAL_C void TestNonSharedPara()
//
	{
	CParagraphStyle::TApplyParaStyleMode applyMode=CParagraphStyle::ERetainNoSpecificFormats;

	INFO_PRINTF1(_L("Apply style to non-shared paragraph"));
	ConstructEnvironment();
	//
	TheText->InsertL(0,_L("This is paragraph one.This is paragraph number two."));
	TChar delimiter=CEditableText::EParagraphDelimiter;
	TheText->InsertL(22,delimiter);
	//
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	charFormat.iFontPresentation.iStrikethrough=EStrikethroughOn;
	charFormatMask.SetAttrib(EAttFontStrikethrough);
	TheText->ApplyCharFormatL(charFormat,charFormatMask,0,4);
	//
	TheText->ApplyParagraphStyleL(*(TheStyleList->At(1).iStyle),0,TheText->DocumentLength(),applyMode);
	//
	CParaFormat* paraFormat=CParaFormat::NewLC();
	TheText->GetParagraphFormatL(paraFormat,0);
	test(paraFormat->iHorizontalAlignment=CParaFormat::ERightAlign);
	//
	TheText->GetParagraphFormatL(paraFormat,10);
	test(paraFormat->iHorizontalAlignment=CParaFormat::ERightAlign);
	//
	TheText->GetParagraphFormatL(paraFormat,30);
	test(paraFormat->iHorizontalAlignment=CParaFormat::ERightAlign);
	//
	CleanupStack::PopAndDestroy();  // para format
	//
	/*TEtextComponentInfo info=*/TheText->ComponentInfo();
	CRichText* theCopy=CRichText::NewL(TheNormalParaLayer,TheNormalCharLayer);
	testStoreRestoreL(*theCopy,*TheText);
	test(IsEqual(theCopy,TheText));
	//
	theCopy->ApplyParagraphStyleL(*(TheStyleList->At(0).iStyle),25,1,applyMode);
	CParagraphStyle* tempStyle = CParagraphStyle::NewL(*TheNormalParaLayer,*TheNormalCharLayer);
	theCopy->InsertL(28,delimiter);
	theCopy->InsertL(31,delimiter);
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	charFormatMask.ClearAll();
	charFormatMask.SetAttrib(EAttFontStrokeWeight);
	theCopy->ApplyCharFormatL(charFormat, charFormatMask, 33, 1);
	theCopy->NotifyStyleChangedL(tempStyle, TheStyleList->At(0).iStyle);
	//
	delete theCopy;
	delete tempStyle;
	KillEnvironment();
	}


LOCAL_C void TestStyles()
// Perform tests
//
	{
	TestSharedPara();
	TestNonSharedPara();
	}

LOCAL_C void TestStyleWithNullParaFormat()
	{
	TestParaWithNullParaFormat();
	}

LOCAL_C void TestStyleList()
	{
	__UHEAP_MARK;
	// Test 1
	// Construction under OOM
	INFO_PRINTF1(_L("Construction under OOM"));
	CStyleList* list=NULL;
	TInt nn;
	for (nn=0; ;nn++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,nn);
		__UHEAP_MARK;
		TRAPD(ret,
			list=CStyleList::NewL());
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			test(list==NULL);
			}
		else
			{
			test(list!=NULL);
			delete list;
			list=NULL;
			__UHEAP_MARKEND;
			break;
			}
		}
	__UHEAP_RESET;
	TBuf<36> answer;
	answer.Format(_L("        #allocs for full c'tion: %d\n"),nn-1);
	INFO_PRINTF1(answer);


	// Test 2
	// Populated style list, Append under OOM;
	INFO_PRINTF1(_L("AppendL() under OOM"));
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	__UHEAP_MARK;
	list=CStyleList::NewL();
	CParagraphStyle* style=NULL;
	for (TInt mm=0;mm<KMaxStyleListGranularity;mm++)
		{
		style=CParagraphStyle::NewL(*paraLayer,*charLayer);
		RParagraphStyleInfo info(style,NULL);
		TInt r=list->AppendL(&info);
		test(r==KErrNone);
		}
	test(list->Count()==KMaxStyleListGranularity);
	
	for (TInt oo=0; ;oo++)
		{
		style=CParagraphStyle::NewL(*paraLayer,*charLayer);
		RParagraphStyleInfo info(style);
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,oo);
		TInt r=KErrNone;
		TRAPD(ret,
				r=list->AppendL(&info));
		if (ret!=KErrNone)
			{
			test(r!=KErrAlreadyExists);
			test(list->Count()==KMaxStyleListGranularity);
			}
		else
			{
			test(r==KErrNone);
			test(list->Count()==KMaxStyleListGranularity+1);
			break;
			}
		__UHEAP_RESET;
		}
	delete list;
	list=NULL;
	style=NULL;
	__UHEAP_MARKEND;
	__UHEAP_RESET;


	// Test 3
	// Inserting a duplicate
	INFO_PRINTF1(_L("AppendL() a duplicate"));
	list=CStyleList::NewL();
	style=NULL;
	for (TInt pp=0;pp<KMaxStyleListGranularity;pp++)
		{
		style=CParagraphStyle::NewL(*paraLayer,*charLayer);
		RParagraphStyleInfo info(style,NULL);
		list->AppendL(&info);
		}
	test(list->Count()==KMaxStyleListGranularity);
	RParagraphStyleInfo info=list->At(0);
	TInt r=list->AppendL(&info);
	test(r==KErrAlreadyExists);
	test(list->Count()==KMaxStyleListGranularity);
	test(info.iStyle->CharFormatLayer()!=NULL);  // the duplicate style has not been deleted.
	delete list;


	// Test 4
	// Looking for a style by name that does not exist.
	INFO_PRINTF1(_L("IndexByName() where style not present"));
	list=CStyleList::NewL();
	style=NULL;
	TUint name='A';
	for (TInt qq=0;qq<KMaxStyleListGranularity;qq++)
		{
		style=CParagraphStyle::NewL(*paraLayer,*charLayer);
		style->iName.Append(name);
		name++;
		RParagraphStyleInfo info(style,NULL);
		list->AppendL(&info);
		}
	test(list->Count()==KMaxStyleListGranularity);
	TParagraphStyleName search=_L("not present");
	/*TInt index=*/list->IndexByName(search);

	delete list;

	delete paraLayer;
	delete charLayer;

	__UHEAP_MARKEND;
	
	}


LOCAL_C void TestHarness()
// Test rich text style usage.
//
    {
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-TTEXT-LEGACY-T_STYLE-0001 RichText Styles "));
	// Do the tests.
	TestConstruction();
	TestStyles();
	TestStyleWithNullParaFormat();
	INFO_PRINTF1(_L("CStyleList"));
	TestStyleList();
	//
    }


LOCAL_C void setupCleanup()
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


LOCAL_C void DeleteDataFile(const TDesC& aFullName)
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

CT_STYLE::CT_STYLE()
    {
    SetTestStepName(KTestStep_T_STYLE);
    pTestStep = this;
    }

TVerdict CT_STYLE::doTestStepL()
    {
    SetTestStepResult(EFail);

    INFO_PRINTF1(_L("Testing Paragraph Styles"));
    __UHEAP_MARK;
    setupCleanup();
    TRAPD(r,TestHarness());
    test(r == KErrNone);

    delete TheTrapCleanup;
    
    __UHEAP_MARKEND;
    
    ::DeleteDataFile(KOutputFile);      //deletion of data files must be before call to End() - DEF047652

    if (r == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
