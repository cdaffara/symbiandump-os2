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


#include <e32svr.h>

#include <s32mem.h>

#include <txtrich.h>
#include <txtglobl.h>

#include "../incp/T_PMLPAR.H"
#include "TTIMES1.h"

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

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D CParaFormatLayer* TheGlobalParaFormatLayer;
LOCAL_D CCharFormatLayer* TheGlobalCharFormatLayer;
LOCAL_D CParser* TheParser;

LOCAL_D const TInt KTestCleanupStack=0x200;
LOCAL_D const TInt KCreateRichTextCount=1000;

LOCAL_D CRichText* TheText[KCreateRichTextCount];

void CT_TTIMES1::GenerateGlobalLayersL()
// Provides the base layers for globl text and below
//
	{
	TParaFormatMask paraMask;
	TheGlobalParaFormatLayer=CParaFormatLayer::NewL((CParaFormat*)NULL,paraMask);
	TCharFormat charFormat;
	TCharFormatMask charMask;
	TheGlobalCharFormatLayer=CCharFormatLayer::NewL(charFormat,charMask);
	}


void CT_TTIMES1::KillGlobalLayers()
// Destroy the base layers for global text and below
//
	{
	delete TheGlobalParaFormatLayer;
	delete TheGlobalCharFormatLayer;
	TheGlobalParaFormatLayer=NULL;
	TheGlobalCharFormatLayer=NULL;
	}

void CT_TTIMES1::KillText()
	{
	for (TInt ii=0; ii<KCreateRichTextCount;ii++)
		{
		delete TheText[ii];
		TheText[ii]=NULL;
		}
	}


struct TNow : public TTime 
	{
	TNow() { HomeTime(); }
	};

void CT_TTIMES1::CreationTestsL()
//
	{
	GenerateGlobalLayersL();

	TNow start;
	TInt ii = 0;
	for (ii=0; ii<KCreateRichTextCount;ii++)
		{
		TheText[ii]=CRichText::NewL(TheGlobalParaFormatLayer,TheGlobalCharFormatLayer);			
		}
	TNow finish;
	KillText();
	test(ii==KCreateRichTextCount);
	//
	// Display the metric
	TBuf<60> context;
	context.Format(_L("        Create rich text %d times: %d\n"),KCreateRichTextCount,finish.MicroSecondsFrom(start).Int64());
	INFO_PRINTF1(context);
	//
	// Tidy up
	KillText();
	KillGlobalLayers();
	}


CRichText* CT_TTIMES1::LoadIntoTextL(TFileName& aFileName)
//
	{
	TRAPD(ret,
	TheParser=CParser::NewL());
	CRichText* text=NULL;
	TRAP(ret,
	text=TheParser->ParseL(aFileName));
	TheGlobalParaFormatLayer=(CParaFormatLayer*)text->GlobalParaFormatLayer();
	TheGlobalCharFormatLayer=(CCharFormatLayer*)text->GlobalCharFormatLayer();
	delete TheParser;
	TheParser=NULL;
	return text;
	}


void CT_TTIMES1::GetAppendTestTimeL(const CRichText* aSource,TInt aTestLoopCount)
	{
	CRichText* target=CRichText::NewL(TheGlobalParaFormatLayer,TheGlobalCharFormatLayer);
	test(target->DocumentLength()==0);
	test(!target->HasMarkupData());

	TNow start;
	TInt ii = 0;
	for (ii=0;ii<aTestLoopCount;ii++)
		{
		target->AppendTakingSolePictureOwnershipL(*aSource);
		}
	TNow finish;
	TInt charSum=aTestLoopCount*(aSource->DocumentLength()+1)-1;
	test(target->DocumentLength()==charSum);
	//
	// Calculate and display result
	test(ii==aTestLoopCount);
	TBuf<60> context;
	context.Format(_L("        Append %d rich text's: %d\n"),aTestLoopCount,finish.MicroSecondsFrom(start).Int64());
	INFO_PRINTF1(context);
	delete target;
	}


void CT_TTIMES1::GetBenchmarkAppendTestTimeL(const CRichText* aSource,TInt aTestLoopCount)
	{
	CRichText** target=new CRichText*[aTestLoopCount];
	CBufStore** store=new CBufStore*[aTestLoopCount];	
	TInt ii = 0;
	for (ii=0;ii<aTestLoopCount;ii++)
		{
		target[ii]=CRichText::NewL(TheGlobalParaFormatLayer,TheGlobalCharFormatLayer);
		store[ii]=CBufStore::NewL(128);	
		test(target[ii]->DocumentLength()==0);
		test(!target[ii]->HasMarkupData());
		}
	TNow start;
	for (ii=0;ii<aTestLoopCount;ii++)
		{
		TStreamId id=aSource->StoreL(*store[ii]);
		target[ii]->RestoreL(*store[ii],id);
		}
	TNow finish;

	for (ii=0;ii<aTestLoopCount;ii++)
		{
		delete target[ii];
		delete store[ii];
		}
	delete []target;
	delete []store;
	//
	// Calculate and display result
	test(ii==aTestLoopCount);
	TBuf<70> context2;
	context2.Format(_L("        Benchmark              : %d\n"),finish.MicroSecondsFrom(start).Int64());
	INFO_PRINTF1(context2);
	}


void CT_TTIMES1::AppendTest1L()
//
	{
	TInt testLoopCount=900;
	CRichText* source=CRichText::NewL(TheGlobalParaFormatLayer,TheGlobalCharFormatLayer);
	source->InsertL(source->DocumentLength(),_L("A bit of text that pretends to be an entry's title"));
	test(!source->HasMarkupData());
	GetAppendTestTimeL(source,testLoopCount);
	//
	// Now the benchmark against which we should measure.
	GetBenchmarkAppendTestTimeL(source,testLoopCount);
	//
	// tidy up
	delete source;
	}


void CT_TTIMES1::AppendTest2L()
//
	{
	KillGlobalLayers();  // they are replaced by the ones read in from the PML file.
	TInt testLoopCount=266;
	TFileName file=_L("z:\\test\\app-framework\\etext\\ttimes1.pml");
	CRichText* source=LoadIntoTextL(file);
	test(source->HasMarkupData());
	GetAppendTestTimeL(source,testLoopCount);
	//
	// Now the benchmark against which we should measure.
	GetBenchmarkAppendTestTimeL(source,testLoopCount);
	//
	// tidy up
	delete source;
	}


void CT_TTIMES1::AppendTestsL()
//
	{
	GenerateGlobalLayersL();
	//
	INFO_PRINTF1(_L("Plain Text Component Only"));
	TRAPD(ret,
	AppendTest1L());
	test(ret==KErrNone);
	//
	INFO_PRINTF1(_L("Plain Text with limited Markup - no pictures"));
	TRAP(ret,
	AppendTest2L());
	test(ret==KErrNone);
	//
	KillGlobalLayers();
	}
	
	
void CT_TTIMES1::DoTestsL()
//
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-TTEXT-LEGACY-T_TTIMES1-0001 Creation Tests "));
	CreationTestsL();
	INFO_PRINTF1(_L("Appending Tests"));
	AppendTestsL();
	//
	}


void CT_TTIMES1::setupCleanup()
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

CT_TTIMES1::CT_TTIMES1()
    {
    SetTestStepName(KTestStep_T_TTIMES1);
    }

TVerdict CT_TTIMES1::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    setupCleanup();
    
    INFO_PRINTF1(_L("CRichText Document"));
    TRAPD(error1, DoTestsL());

    delete TheTrapCleanup;

    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
