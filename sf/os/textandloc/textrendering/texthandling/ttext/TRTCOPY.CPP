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
#include <txtstyle.h>
#include <flddef.h>
#include <fldbltin.h>
#include <gdi.h>
#include <conpics.h>											   						 
#include <s32mem.h>
#include <s32file.h>
#include "../incp/T_PMLPAR.H"
#include "TRTCOPY.h"

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

LOCAL_D RFs theFs;
LOCAL_D CRichText* ComponentText;
LOCAL_D CParaFormatLayer* GlobalParaFormatLayer;
LOCAL_D CCharFormatLayer* GlobalCharFormatLayer;
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D CParser* TheParser;

const TInt KTestCleanupStack=0x500;


CRichText* CT_TRTCOPY::LoadIntoTextL(TFileName& aFileName)
//
	{
	TRAPD(ret,
	TheParser=CParser::NewL());
	CRichText* text=NULL;
	TRAP(ret,
	text=TheParser->ParseL(aFileName));
	GlobalParaFormatLayer=(CParaFormatLayer*)text->GlobalParaFormatLayer();
	GlobalCharFormatLayer=(CCharFormatLayer*)text->GlobalCharFormatLayer();
	delete TheParser;
	TheParser=NULL;
	return text;
	}

/*
CStyleList* CT_TRTCOPY::CreatePopulatedStyleList()
//
	{
	//
	// Create style aswell.
	CStyleList* list=CStyleList::NewL();
	CParagraphStyle* style1=CParagraphStyle::NewL(*GlobalParaFormatLayer,*GlobalCharFormatLayer);
	CParagraphStyle* style2=CParagraphStyle::NewL(*GlobalParaFormatLayer,*GlobalCharFormatLayer);
	CParagraphStyle* style3=CParagraphStyle::NewL(*GlobalParaFormatLayer,*GlobalCharFormatLayer);
	list->AppendL(*style1);
	list->AppendL(*style2);
	list->AppendL(*style3);
	return list;
	}
*/

void CT_TRTCOPY::CreateAggregateTextL()
//
	{
	TFileName theFileName=_L("z:\\test\\app-framework\\etext\\aggregat.pml");
//	AggregateText=LoadIntoTextL(theFileName);
	//
	theFileName=_L("z:\\test\\app-framework\\etext\\componnt.pml");
	ComponentText=LoadIntoTextL(theFileName);
	// Insert a picture or two
	CXzePicture* pic1=CXzePicture::NewL('x');
	CleanupStack::PushL(pic1);
	CXzePicture* pic2=CXzePicture::NewL('z');
	CleanupStack::PushL(pic2);
	// Create the picture headers
	TPictureHeader hdr1;
	TPictureHeader hdr2;
	hdr1.iPictureType=KUidXzePictureType;
	hdr1.iPicture=pic1;
	hdr2.iPictureType=KUidXzePictureType;
	hdr2.iPicture=pic2;
	ComponentText->InsertL(0,hdr1);
	ComponentText->InsertL(ComponentText->DocumentLength(),hdr2);
	CleanupStack::Pop(2);  // pic1,pic2
	//
	// Now do the insert.
//	AggregateText->InsertL(10,ComponentText);
//	delete AggregateText;
	delete ComponentText;
	delete GlobalParaFormatLayer;
	delete GlobalCharFormatLayer;
	}


void CT_TRTCOPY::doMainL()
	{
	CreateAggregateTextL();
	}


void CT_TRTCOPY::setupCleanup()
//
// Initialise the cleanup stack.
//
    {
	TheTrapCleanup=CTrapCleanup::New();
	test(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	test(r==KErrNone);
	}

CT_TRTCOPY::CT_TRTCOPY()
    {
    SetTestStepName(KTestStep_T_TRTCOPY);
    }

TVerdict CT_TRTCOPY::doTestStepL()
    {
    SetTestStepResult(EFail);

    INFO_PRINTF1(_L("TRTCOPY"));
    setupCleanup();
    __UHEAP_MARK;

    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-TTEXT-LEGACY-T_TRTCOPY-0001 RichText Copy Testing "));
    TRAPD(error1, doMainL());

    __UHEAP_MARKEND;
    delete TheTrapCleanup;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
