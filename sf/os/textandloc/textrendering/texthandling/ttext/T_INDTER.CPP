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

#include <txtfrmat.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <gdi.h>

#include "../incp/T_PMLPAR.H"
#include "T_INDTER.h"

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

LOCAL_D CTrapCleanup* TheTrapCleanup=NULL;
LOCAL_D CRichText* TheText=NULL;
LOCAL_D CParaFormatLayer* TheGlobalParaLayer=NULL;
LOCAL_D CCharFormatLayer* TheGlobalCharLayer=NULL;
LOCAL_D CParser* TheParser;


LOCAL_C void SetRichTextL(TFileName& aFileName)
//
	{
	delete TheText;
	delete TheGlobalParaLayer;
	delete TheGlobalCharLayer;
	TheParser=CParser::NewL();
	CleanupStack::PushL(TheParser);
	TheText=TheParser->ParseL(aFileName);
	CleanupStack::PopAndDestroy();
	TheGlobalParaLayer=(CParaFormatLayer*)TheText->GlobalParaFormatLayer();
	TheGlobalCharLayer=(CCharFormatLayer*)TheText->GlobalCharFormatLayer();
	}


LOCAL_C void CreateRichTextL()
//
	{
	TParaFormatMask paraMask;
	TheGlobalParaLayer=CParaFormatLayer::NewL((CParaFormat*)NULL,paraMask);
	CleanupStack::PushL(TheGlobalParaLayer);
	TCharFormat charFormat;
	TCharFormatMask charMask;
	TheGlobalCharLayer=CCharFormatLayer::NewL(charFormat,charMask);
	CleanupStack::PushL(TheGlobalCharLayer);
	TheText=CRichText::NewL(TheGlobalParaLayer,TheGlobalCharLayer);
	CleanupStack::Pop(2);
	}


LOCAL_C void DestroyRichText()
//
	{
	delete TheText;
	delete TheGlobalParaLayer;
	delete TheGlobalCharLayer;
	}


LOCAL_C void ResetTextL()
// Initialise the rich text.
//
	{
	TheText->Reset();
	TBuf<512> buf(_L("This is paragraph one"));
	buf.Append(CEditableText::EParagraphDelimiter);
	buf.Append(_L("This is paragraph two"));
	buf.Append(CEditableText::EParagraphDelimiter);
	buf.Append(_L("This is paragraph 333"));
	// Editable text already has the terminating paragraph delimter.
	TheText->InsertL(0,buf);
	}


LOCAL_C TParaBorder BorderValues(TParaBorder::TLineStyle aLineStyle,TInt aThickness)
//
	{
	TParaBorder border;
	border.iLineStyle=aLineStyle;
	border.iThickness=aThickness;
	return border;
	}


LOCAL_C void PerformTestParaBordersL(CParaFormat& aApplyFormat,TParaFormatMask& aApplyMask,
									 TTextFormatAttribute anAttribute,CParaFormat::TParaBorderSide aSide,
									 TParaBorder& aBorder,
									 CParaFormat& aSensedFormat,TParaFormatMask& aSensedMask)
//
	{
	aApplyMask.SetAttrib(anAttribute);
	aApplyFormat.SetParaBorderL(aSide,aBorder);
	TheText->ApplyParaFormatL(&aApplyFormat,aApplyMask,0,1);
	TheText->GetParaFormatL(&aSensedFormat,aSensedMask,0,TheText->DocumentLength());
	}


LOCAL_C void TestParaBordersL()
// Tests the indeterminate state of paragraph borders.
//
	{
	ResetTextL();
	CParaFormat* sensedFormat=CParaFormat::NewLC();
	TParaFormatMask undeterminedMask;
	TheText->GetParaFormatL(sensedFormat,undeterminedMask,0,TheText->DocumentLength());
	test(undeterminedMask.IsNull());
	//
	CParaFormat* applyFormat=CParaFormat::NewLC();
	TParaFormatMask applyMask;
	//
	TParaBorder border=BorderValues(TParaBorder::ESolid,1);
	//
	PerformTestParaBordersL(*applyFormat,applyMask,EAttTopBorder,CParaFormat::EParaBorderTop,border,*sensedFormat,undeterminedMask);
	test(undeterminedMask.AttribIsSet(EAttTopBorder));
	test(!(undeterminedMask.AttribIsSet(EAttBottomBorder)));
	test(!(undeterminedMask.AttribIsSet(EAttLeftBorder)));
	test(!(undeterminedMask.AttribIsSet(EAttRightBorder)));
	//
	PerformTestParaBordersL(*applyFormat,applyMask,EAttBottomBorder,CParaFormat::EParaBorderBottom,border,*sensedFormat,undeterminedMask);	
	test(undeterminedMask.AttribIsSet(EAttTopBorder));
	test(undeterminedMask.AttribIsSet(EAttBottomBorder));
	test(!(undeterminedMask.AttribIsSet(EAttLeftBorder)));
	test(!(undeterminedMask.AttribIsSet(EAttRightBorder)));
	//
	PerformTestParaBordersL(*applyFormat,applyMask,EAttLeftBorder,CParaFormat::EParaBorderLeft,border,*sensedFormat,undeterminedMask);
	test(undeterminedMask.AttribIsSet(EAttTopBorder));
	test(undeterminedMask.AttribIsSet(EAttBottomBorder));
	test(undeterminedMask.AttribIsSet(EAttLeftBorder));
	test(!(undeterminedMask.AttribIsSet(EAttRightBorder)));
	//
	PerformTestParaBordersL(*applyFormat,applyMask,EAttRightBorder,CParaFormat::EParaBorderRight,border,*sensedFormat,undeterminedMask);	
	test(undeterminedMask.AttribIsSet(EAttTopBorder));
	test(undeterminedMask.AttribIsSet(EAttBottomBorder));
	test(undeterminedMask.AttribIsSet(EAttLeftBorder));
	test(undeterminedMask.AttribIsSet(EAttRightBorder));
	//
	CleanupStack::PopAndDestroy(2);  // applyFormat & sensedFormat
	}


LOCAL_C void TestParaFormatL()
// Test the indeterminate state of paragraph format attributes.
//
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-TTEXT-LEGACY-T_INDTER-0001 Paragraph format attributes "));
	TestParaBordersL();
	}


LOCAL_C void TestCharFormatL()
// Test the indeterminate state of character format attributes.
//
	{
	INFO_PRINTF1(_L("Character format attributes"));
	//
	ResetTextL();
	TCharFormat applyFormat;
	TCharFormatMask applyMask;
	//
	TCharFormat sensedFormat;
	TCharFormatMask sensedMask;
	// Make para 2 bold
	applyFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	applyMask.SetAttrib(EAttFontStrokeWeight);
	TheText->ApplyCharFormatL(applyFormat,applyMask,22,22);
	// Make para 3 italic
	applyFormat.iFontSpec.iFontStyle.SetPosture(EPostureItalic);
	applyMask.ClearAll();  applyMask.SetAttrib(EAttFontPosture);
	TheText->ApplyCharFormatL(applyFormat,applyMask,44,21);
	//	
	TheText->GetCharFormat(sensedFormat,sensedMask,0,TheText->DocumentLength());
	test(sensedMask.AttribIsSet(EAttFontStrokeWeight));
	test(sensedMask.AttribIsSet(EAttFontPosture));
	//
	TheText->GetCharFormat(sensedFormat,sensedMask,50,1);
	test(sensedMask.IsNull());
	//
	TheText->GetCharFormat(sensedFormat,sensedMask,44,21);
	test(sensedMask.IsNull());
	//
	TFileName file=_L("z:\\test\\app-framework\\etext\\t_indter.pml");
	SetRichTextL(file);

	TheText->GetCharFormat(sensedFormat,sensedMask,4,1);
	//
	TheText->GetCharFormat(sensedFormat,sensedMask,0,TheText->DocumentLength());
	test(sensedMask.AttribIsSet(EAttFontStrokeWeight));
	test(sensedMask.AttribIsSet(EAttFontPosture));
	test(sensedMask.AttribIsSet(EAttFontUnderline));
	test(sensedMask.AttribIsSet(EAttFontStrikethrough));
	test(sensedMask.AttribIsSet(EAttFontPrintPos));
	}


LOCAL_C void DoTestL()
// Main routine
//
    {
	CreateRichTextL();
	TestParaFormatL();
	TestCharFormatL();
	
	DestroyRichText();
    }


LOCAL_C void setupCleanup()
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

CT_INDTER::CT_INDTER()
    {
    SetTestStepName(KTestStep_T_INDTER);
    pTestStep = this;
    }

TVerdict CT_INDTER::doTestStepL()
    {
    SetTestStepResult(EFail);

    INFO_PRINTF1(_L("Rich Text Format attribute Indeterminate State tests"));
    __UHEAP_MARK;
    setupCleanup();
    TRAPD(r, DoTestL());
    test(r == KErrNone);

    delete TheTrapCleanup;
    
    __UHEAP_MARKEND;
    
    if (r == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
