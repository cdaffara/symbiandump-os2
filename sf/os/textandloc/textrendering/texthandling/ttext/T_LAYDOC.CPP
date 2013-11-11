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


#include <txtlaydc.h>
#include <txtglobl.h>
#include <txtfrmat.h>
#include <txtfmlyr.h>
#include "T_LAYDOC.h"

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

LOCAL_D TPtrC defaultText(_L("This is default text"));
LOCAL_D TPtrC comp1(_L("fault text"));
LOCAL_D TPtrC comp2(_L("t"));
LOCAL_D TPtrC view(_L("x"));

GLDEF_C void CheckView(TPtrC& aView,TPtrC& aControl)
//
// Test that aView matches the contol descriptor in
// the expected metrics.
//
	{
	test(aView.Length()==(aControl.Length()+1));
	TInt index=0;
	for (index=0;index<aControl.Length();index++)
		{
		test(aView[index]==aControl[index]);
		}
	test(aView[index]==CEditableText::EParagraphDelimiter);
	}


GLDEF_C void TestDocumentLength(CEditableText::TDocumentStorage aStorage)
//
//
//
	{
	__UHEAP_MARK;
	// Make the global format layers...
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();

// Create Document
	CGlobalText* doc=CGlobalText::NewL(paraLayer,charLayer,aStorage);
	TInt length=doc->LdDocumentLength();
	test(length==0);
// Insert document content
	doc->InsertL(0,defaultText);
	length=doc->LdDocumentLength();
	test(length==20);	

	delete doc;
	delete paraLayer;
	delete charLayer;
	__UHEAP_MARKEND;
	}


GLDEF_C void TestRead(CEditableText::TDocumentStorage aStorage)
//
// Test this class' GetChars method
// functions as predicted.
//
	{
	__UHEAP_MARK;
	// Make the global format layers...
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();

// Create Document
	CGlobalText* doc=CGlobalText::NewL(paraLayer,charLayer,aStorage);
	TRAPD(ret,doc->InsertL(0,defaultText));
    UNUSED_VAR(ret);

// Create LayDoc
	TCharFormat format;
	TPtrC view;
	
	INFO_PRINTF1(_L("Sensing at start"));
	doc->GetChars(view,format,0);
	CheckView(view,defaultText);
	
	INFO_PRINTF1(_L("Sensing from char.pos.10"));
	doc->GetChars(view,format,10);
	CheckView(view,comp1);
	
	INFO_PRINTF1(_L("Sensing from end-1"));
	doc->GetChars(view,format,19);
	CheckView(view,comp2);
	
	INFO_PRINTF1(_L("Sensing from end"));
	doc->GetChars(view,format,20);
	test(*view.Ptr()==CEditableText::EParagraphDelimiter);
	
	
	delete doc;
	delete paraLayer;
	delete charLayer;
	__UHEAP_MARKEND;
	}


GLDEF_C void CheckParagraphStart(TInt aCalculated,TInt aExpected)
//
// Checks the value returned from ParagraphStart(aPos) is what
// it is expected to be.
//
	{
	test(aCalculated==aExpected);
	}


GLDEF_C void CheckCharsSkipped(TInt aCalculated,TInt aExpected)
//
// Check the number of characters skipped following a  ParagraphStart()
// is as expected.
//
	{
	test(aCalculated==aExpected);
	}


GLDEF_C void DoParagraphStart(TInt aStartPos,TInt aLength,MLayDoc* aLayout)
//
// Parametric testing of the ParagraphStart method of the
// document class hierarchy.
//
	{
	User::Heap().Check();
	TInt tempPos=0;
	TInt charsSkipped=0;
	for (TInt charPos=aStartPos;charPos<aStartPos+aLength;charPos++)
		{// Check Paragraph
		tempPos=charPos;
		charsSkipped=aLayout->LdToParagraphStart(charPos);
		// charPos is updated to paragraph start character position.
		CheckParagraphStart(charPos,aStartPos);
		charPos=tempPos;  // Reset charPos following it's update.
		CheckCharsSkipped(charsSkipped,charPos-aStartPos);
		}
	User::Heap().Check();
	}


GLDEF_C void TestParagraphStart(CEditableText::TDocumentStorage aStorage)
//
// Tests the ParagraphStart method.
//
	{
	User::Heap().Check();
	__UHEAP_MARK;
	// Make the global format layers...
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();

	TBuf<128> content;
	content.Append(_L("Paragraph one. Complete with sentence and word breaks."));
	content.Append(CEditableText::EParagraphDelimiter);
	content.Append(_L("This is paragraph two."));
	content.Append(CEditableText::EParagraphDelimiter);
	content.Append(_L("This is paragraph 3"));
	// Create document.
	CGlobalText* document=CGlobalText::NewL(paraLayer,charLayer,aStorage);
	TPtrC body(content);
	document->InsertL(0,body);
	// Now do the tests.
	INFO_PRINTF1(_L("Paragraph 1"));
	DoParagraphStart(0,55,document);  // Paragraph 1
	INFO_PRINTF1(_L("Paragraph 2"));
	DoParagraphStart(55,23,document);  // Paragraph 2
	INFO_PRINTF1(_L("Paragraph 3"));
	DoParagraphStart(78,20,document);  // Paragraph 3

	delete paraLayer;
	delete charLayer;
	delete document;
	
	__UHEAP_MARKEND;
	User::Heap().Check();
	}


GLDEF_C void TestGetParagraphFormatL(CEditableText::TDocumentStorage aStorage)
//
//
//
	{
	__UHEAP_MARK;
	User::Heap().Check();
	// Make the global format layers...
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();

	CGlobalText* globalDoc=CGlobalText::NewL(paraLayer,charLayer,aStorage);

	CParaFormat* format=CParaFormat::NewL();
	CParaFormat* control=CParaFormat::NewL();
	TParaFormatMask cMask;
	control->iLeftMarginInTwips=1440; cMask.SetAttrib(EAttLeftMargin);
	TTabStop tab1;
	tab1.iTwipsPosition=1440; tab1.iType=TTabStop::ERightTab;
	control->StoreTabL(tab1); cMask.SetAttrib(EAttTabStop);
	
	TParaBorder border;
	border.iLineStyle=TParaBorder::ESolid;
	border.iThickness=1;
	control->SetParaBorderL(CParaFormat::EParaBorderTop,border); cMask.SetAttrib(EAttTopBorder);
	control->iKeepTogether=ETrue; cMask.SetAttrib(EAttKeepTogether);
	CParaFormatLayer* layer=CParaFormatLayer::NewL(control,cMask);
	globalDoc->SetGlobalParaFormat(layer);

	globalDoc->GetParagraphFormatL(format,0);
	TInt c1=format->TabCount();
	TInt c2=control->TabCount();
	test(c1==c2);
	test(format->IsEqual(*control));

	__UHEAP_MARK;	
	delete layer;
	delete control;
	delete format;
	__UHEAP_MARKEND;
	delete globalDoc;	
	delete charLayer;
	delete paraLayer;
	User::Heap().Check();
	__UHEAP_MARKEND;
	}

GLDEF_C void TestRegister(CEditableText::TDocumentStorage aStorage)
//
// Checks all declared base class methods have been provided.
//
	{
	User::Heap().Check();
	__UHEAP_MARK;
	// Make the global format layers...
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();

// Create a para format layer
	CParaFormat* format=CParaFormat::NewL();
	TParaFormatMask mask;
	mask.SetAttrib(EAttLeftMargin);
	mask.SetAttrib(EAttRightMargin);
	CParaFormatLayer* layer=CParaFormatLayer::NewL(format,mask);
// Create global text document to layout
	CGlobalText* doc=NULL;
	TRAPD(ret,doc=CGlobalText::NewL(paraLayer,charLayer,aStorage));
    UNUSED_VAR(ret);
	TInt pos=0;
	doc->SetGlobalParaFormat(layer);

// Check constructor
	INFO_PRINTF1(_L("Constructor"));

// Check DocumentLength
	INFO_PRINTF1(_L("DocumentLength()"));
	doc->LdDocumentLength();

// Check ParagraphStart
	INFO_PRINTF1(_L("ParagraphStart()"));
	doc->LdToParagraphStart(pos);

// Check GetParagraphFormatL
	INFO_PRINTF1(_L("GetParagraphFormatL()"));
	doc->GetParagraphFormatL(format,pos);

// Check GetChars
	INFO_PRINTF1(_L("GetChars()"));
	TPtrC view;
	TCharFormat charFormat;
	doc->GetChars(view,charFormat,pos);

// check EnquirePage
	INFO_PRINTF1(_L("EnquirePageBreak()"));
	doc->EnquirePageBreak(pos,0);

	delete paraLayer;
	delete charLayer;
	delete doc;
	delete layer;
	delete format;
	
	__UHEAP_MARKEND;
	User::Heap().Check();
	}

GLDEF_C void Test()
//
// Run the tests
//
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Checking all methods present"));
	TestRegister(CEditableText::EFlatStorage);

	INFO_PRINTF1(_L("DocumentLength()"));
	TestDocumentLength(CEditableText::EFlatStorage);

	INFO_PRINTF1(_L("EnquirePage()"));
	INFO_PRINTF1(_L("Always returns 0"));
	

	INFO_PRINTF1(_L("ParagraphStart()"));
	TestParagraphStart(CEditableText::EFlatStorage);

	INFO_PRINTF1(_L("GetParagraphFormatL()"));
	TestGetParagraphFormatL(CEditableText::EFlatStorage);
		
	INFO_PRINTF1(_L("GetChars"));
	TestRead(CEditableText::EFlatStorage);
	
	__UHEAP_MARKEND;
	}
	
	 
GLDEF_C void TestSeg()
//
// Run the tests
//
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Checking all methods present"));
	TestRegister(CEditableText::ESegmentedStorage);

	INFO_PRINTF1(_L("DocumentLength()"));
	TestDocumentLength(CEditableText::ESegmentedStorage);

	INFO_PRINTF1(_L("EnquirePage()"));
	INFO_PRINTF1(_L("Always returns 0"));
	

	INFO_PRINTF1(_L("ParagraphStart()"));
	TestParagraphStart(CEditableText::ESegmentedStorage);

	INFO_PRINTF1(_L("SenseParagraphFormatL()"));
	TestGetParagraphFormatL(CEditableText::ESegmentedStorage);
		
	INFO_PRINTF1(_L("GetChars"));
	//TestRead();
	//
	INFO_PRINTF1(_L("Test not yet implemented"));
	
	__UHEAP_MARKEND;
	}

CT_LAYDOC::CT_LAYDOC()
    {
    SetTestStepName(KTestStep_T_LAYDOC);
    pTestStep = this;
    }

TVerdict CT_LAYDOC::doTestStepL()
    {
    SetTestStepResult(EFail);

    CTrapCleanup* cleanup=CTrapCleanup::New();

    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-TTEXT-LEGACY-T_LAYDOC-0001 MLayDoc - Using Flat document "));
    
    __UHEAP_MARK;
    TRAPD(ret1,Test());
    __UHEAP_MARKEND;
    
    INFO_PRINTF1(_L("MLaydoc - Using Segmented document"));
    __UHEAP_MARK;
    TRAPD(ret2,TestSeg());
    __UHEAP_MARKEND;

    delete cleanup;

    if (ret1 == KErrNone && ret2 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
