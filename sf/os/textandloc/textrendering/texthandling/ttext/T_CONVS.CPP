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


#include <txtetext.h>
#include <txtglobl.h>
#include <txtfmlyr.h>
#include <s32mem.h>
#include <s32file.h>
#include <fldbase.h>
#include <fldbltin.h>
#include <flddef.h>
#include "T_CONVS.h"

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
const TInt KTestExpandSize=0x20;

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D	TPtrC bigBuf(_L("This is a very big buffer indeed, containing text and special characters,\
 big enough to fill a segment of an editable text component that employs segmented storage"));

////////////////////////////////////////////////////////////////////////////////////////////
class TTestFieldFactoryCONVS : public MTextFieldFactory
	{
public:
	// from MTextFieldFactory
	virtual CTextField* NewFieldL(TUid aFieldType); 
	// Creates a field of the type specified
	// Returns NULL if it does not recognise/support the field type
	};

CTextField* TTestFieldFactoryCONVS::NewFieldL(TUid aFieldType)
// Creates a field (in aHeader) of the type specified in aHeader
// 
	{
	CTextField* field=NULL;
	if (aFieldType==KDateTimeFieldUid)
		field = (CTextField*)new(ELeave) CDateTimeField();
	return field;
	}
/////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CT_CONVS::testCopy(T &aCopy,const T &anOriginal)
//
// Copy anOriginal to aCopy using memory-based streams.
//
	{
	CBufSeg *buf=CBufSeg::NewL(KTestExpandSize);
	if (buf==NULL)
		User::Panic(_L("Allocating buffer"), 1234);
	
//	Write anOriginal out to the buffer.
	RBufWriteStream out(*buf);
	TRAPD(r,out<<anOriginal);
	test(r==KErrNone);
	TRAP(r,out.CommitL());
	if (r!=KErrNone)
	    User::Panic(_L("Committing write stream"), 1234);

//	Read anOriginal in from the buffer.
	RBufReadStream in(*buf);
	TRAP(r,in>>aCopy);
	test(r==KErrNone);

//	See if it's consumed the lot.
	TRAP(r,in.ReadUint8L());
	test(r==KErrEof);
//
	delete buf;
	}

_LIT(KOutputFile, "c:\\etext\\t_convs.tst");
template <class T>
void CT_CONVS::testStoreRestoreL(T& aCopy,const T& aOriginal)
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


template <class T>
void CT_CONVS::testCopyChain(T &aCopy,const T &anOriginal,TInt aExcludeCount,const CFormatLayer* aBase)
//
// Copy anOriginal to aCopy using memory-based streams.
//
	{
	CBufSeg *buf=CBufSeg::NewL(KTestExpandSize);
	if (buf==NULL)
	    User::Panic(_L("Allocating buffer"), 1234);
	
//	Write anOriginal out to the buffer.
	RBufWriteStream out(*buf);
	TRAPD(r,anOriginal.ExternalizeChainL(out,aExcludeCount));
	test(r==KErrNone);
	TRAP(r,out.CommitL());
	if (r!=KErrNone)
	    User::Panic(_L("Committing write stream"), 1234);

//	Read anOriginal in from the buffer.
	RBufReadStream in(*buf);
	TRAP(r,aCopy.InternalizeChainL(in,aBase));
	test(r==KErrNone);

//	See if it's consumed the lot.
	TRAP(r,in.ReadUint8L());
	test(r!=KErrNone);
//
	delete buf;
	}


TInt CT_CONVS::IsEqual(const CPlainText* aCopy,const CPlainText* aOriginal)
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
	test(aCopy->FieldCount()==aOriginal->FieldCount());
	return 1;
	}


void CT_CONVS::testPlainTextL(CEditableText::TDocumentStorage aStorage)
//
// Test streaming CPlainText.
//
	{// Create the plain text components.
	INFO_PRINTF1(_L("Streaming CPlainText"));
	CPlainText* copy=CPlainText::NewL(aStorage);
	CPlainText* testDoc=CPlainText::NewL(aStorage);
	//
	// Set the original - empty
	INFO_PRINTF1(_L("empty."));
	testStoreRestoreL(*copy,*testDoc);
	test(IsEqual(copy,testDoc));
	//	
	INFO_PRINTF1(_L("paragraph delimiter"));
	TRAPD(r,testDoc->InsertL(0,CEditableText::EParagraphDelimiter));
	test(r==KErrNone);
	testStoreRestoreL(*copy,*testDoc);	
	test(IsEqual(copy,testDoc));
	//
	// Next test with tons of text guaranteed to force segment break when using segmented storage.	
	INFO_PRINTF1(_L("big text component"));
	testDoc->InsertL(0,bigBuf);
	testStoreRestoreL(*copy,*testDoc);
	test(IsEqual(copy,testDoc));
	//
	// Now test with field components.
	INFO_PRINTF1(_L("big text doc with field components."));
	TTestFieldFactoryCONVS factory;
	testDoc->SetFieldFactory(&factory);
	copy->SetFieldFactory(&factory);
	CTextField* field=NULL;
	TRAPD(ret,
	field=factory.NewFieldL(KDateTimeFieldUid));
	test(ret==KErrNone);
	TRAP(ret,
	testDoc->InsertFieldL(0,field,KDateTimeFieldUid));
	test(ret==KErrNone);
	testStoreRestoreL(*copy,*testDoc);
	test(IsEqual(copy,testDoc));
	//
	//
	delete copy;
	delete testDoc;
	}


void CT_CONVS::testGlobalTextL(CEditableText::TDocumentStorage aStorage)
//
// Test streaming CGlobalText.
//
	{// Create the plain text components.
	INFO_PRINTF1(_L("Streaming CGlobalText"));
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	// Set something interesting in the layers:
	CParaFormat* paraFormat1=CParaFormat::NewL(); TParaFormatMask paraMask1;
	TCharFormat charFormat1; TCharFormatMask charMask1;
	paraFormat1->iHorizontalAlignment=CParaFormat::ECenterAlign; paraMask1.SetAttrib(EAttAlignment);
	paraFormat1->iLeftMarginInTwips=4000; paraMask1.SetAttrib(EAttLeftMargin);
	paraLayer->SetL(paraFormat1,paraMask1);
	charFormat1.iFontSpec.iFontStyle.SetPosture(EPostureItalic); charMask1.SetAttrib(EAttFontPosture);
	charFormat1.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold); charMask1.SetAttrib(EAttFontStrokeWeight);
	charLayer->SetL(charFormat1,charMask1);
	//	
	CGlobalText* copy=CGlobalText::NewL(paraLayer,charLayer,aStorage);
	CGlobalText* testDoc=CGlobalText::NewL(paraLayer,charLayer,aStorage);

//	Set the original - empty
	INFO_PRINTF1(_L("empty."));
	testStoreRestoreL(*copy,*testDoc);
	test(IsEqual(copy,testDoc));
//	
	INFO_PRINTF1(_L("paragraph delimiter"));
	TRAPD(r,testDoc->InsertL(0,CEditableText::EParagraphDelimiter));
	test(r==KErrNone);
	testStoreRestoreL(*copy,*testDoc);
	test(IsEqual(copy,testDoc));

//	Next test with tons of text guaranteed to force segment break when using segmented storage.	
	INFO_PRINTF1(_L("big text component"));
	testDoc->InsertL(0,bigBuf);
	testStoreRestoreL(*copy,*testDoc);
	test(IsEqual(copy,testDoc));
		
	delete copy;
	delete testDoc;
	delete paraLayer;
	delete charLayer;
	delete paraFormat1;
	}


TInt CT_CONVS::LayerIsEqual(const CParaFormatLayer* aRestored,const CParaFormatLayer* aOriginal)
//
// Returns true if aRestored contents matches aOriginal contents.
//
	{
	CParaFormat* restored=NULL;  TParaFormatMask rm;
	CParaFormat* original=NULL;  TParaFormatMask om;
	TRAPD(r,restored=CParaFormat::NewL());     test(r==KErrNone);
	TRAP(r,original=CParaFormat::NewL());     test(r==KErrNone);

	aOriginal->SenseL(original,om);
	aRestored->SenseL(restored,rm);

	test(original->IsEqual(*restored));

	delete restored;
	delete original;
	return 1;
	}


TInt CT_CONVS::LayerIsEqual(const CCharFormatLayer* aRestored,const CCharFormatLayer* aOriginal)
//
// Returns true if aRestored contents matches aOriginal contents.
//
	{
	TCharFormat restored;  TCharFormatMask rm;
	TCharFormat original;  TCharFormatMask om;

	aOriginal->Sense(original,om);
	aRestored->Sense(restored,rm);

	test(original.IsEqual(restored));
	
	return 1;
	}


void CT_CONVS::testFmtLayerStoreL()
//
// Test the format layer StoreL().
//
	{
	INFO_PRINTF1(_L("CParaFormatLayer"));
//	Create test layers.
	CParaFormatLayer* pfl1=NULL;
	CParaFormatLayer* restored=NULL;
	CParaFormat* pf1=NULL;
	TRAPD(r,restored=CParaFormatLayer::NewL());     test(r==KErrNone);
	// Force *restored* to allocate storage for iteself by storing a null layer.
	TParaFormatMask rm;     rm.ClearAll(); CParaFormat* rpf=NULL;
	restored->SetL(rpf,rm);
	TRAP(r,pfl1=CParaFormatLayer::NewL());     test(r==KErrNone);
	TRAP(r,pf1=CParaFormat::NewL());     test(r==KErrNone);
	TParaFormatMask pm1;
	pm1.SetAll();  // Sets all but the compound attributes.
//	TEST ONE DEFAULT CASES
	INFO_PRINTF1(_L("Default paragraph format values."));
	TRAP(r,pfl1->SetL(pf1,pm1));     test(r==KErrNone);
	testCopy(*restored,*pfl1);
	test(LayerIsEqual(restored,pfl1));
	test(restored->SenseBase()==pfl1->SenseBase());  // Both should default to based on NULL
//	TEST TWO 
	INFO_PRINTF1(_L("Setting all attributes"));
	pf1->iLeftMarginInTwips=5000; pm1.ClearAll(); pm1.SetAttrib(EAttLeftMargin);
	pf1->iRightMarginInTwips=5001; pm1.SetAttrib(EAttRightMargin);
	pf1->iIndentInTwips=5002;pm1.SetAttrib(EAttIndent);
	pf1->iHorizontalAlignment=CParaFormat::ERightAlign; pm1.SetAttrib(EAttAlignment);
	pf1->iVerticalAlignment=CParaFormat::ECenterAlign; pm1.SetAttrib(EAttVerticalAlignment);
	pf1->iLineSpacingInTwips=5003; pm1.SetAttrib(EAttLineSpacing);
	pf1->iLineSpacingControl=CParaFormat::ELineSpacingAtLeastInTwips; pm1.SetAttrib(EAttLineSpacingControl);
	pf1->iSpaceBeforeInTwips=5004; pm1.SetAttrib(EAttSpaceBefore);
	pf1->iSpaceAfterInTwips=5005; pm1.SetAttrib(EAttSpaceAfter);
	pf1->iKeepTogether=ETrue; pm1.SetAttrib(EAttKeepTogether);
	pf1->iKeepWithNext=ETrue; pm1.SetAttrib(EAttKeepWithNext);
	pf1->iStartNewPage=ETrue; pm1.SetAttrib(EAttStartNewPage);
	pf1->iWidowOrphan=ETrue; pm1.SetAttrib(EAttWidowOrphan);
	pf1->iWrap=EFalse; pm1.SetAttrib(EAttWrap);
	pf1->iBorderMarginInTwips=5006; pm1.SetAttrib(EAttBorderMargin);
	pf1->iDefaultTabWidthInTwips=5007; pm1.SetAttrib(EAttDefaultTabWidth);
	// TopBorder
	TParaBorder top;
	top.iLineStyle=TParaBorder::ESolid;
	top.iThickness=4;
	top.iAutoColor=ETrue;
	pf1->SetParaBorderL(CParaFormat::EParaBorderTop,top);
	pm1.SetAttrib(EAttTopBorder);
	// BottomBorder
	TParaBorder bottom;	
	bottom.iLineStyle=TParaBorder::ESolid;
	bottom.iThickness=4;
	bottom.iAutoColor=ETrue;
	pf1->SetParaBorderL(CParaFormat::EParaBorderBottom,bottom);
	pm1.SetAttrib(EAttBottomBorder);
	// LeftBorder
	TParaBorder left;
	left.iLineStyle=TParaBorder::ESolid;
	left.iThickness=4;
	left.iAutoColor=ETrue;
	pf1->SetParaBorderL(CParaFormat::EParaBorderLeft,left);
	pm1.SetAttrib(EAttLeftBorder);
	// RightBorder
	TParaBorder right;
	right.iLineStyle=TParaBorder::ESolid;
	right.iThickness=4;
	top.iAutoColor=ETrue;
	pf1->SetParaBorderL(CParaFormat::EParaBorderRight,right);
	pm1.SetAttrib(EAttRightBorder);
	// Bullet
	pf1->iBullet=new(ELeave)TBullet;
	TUint charCode=5008;
	pf1->iBullet->iCharacterCode=(TUint8)charCode;
	pf1->iBullet->iHeightInTwips=5009;
	pf1->iBullet->iTypeface.iName=_L("Duncan");
	pf1->iBullet->iTypeface.SetIsProportional(EFalse);
	pf1->iBullet->iTypeface.SetIsSerif(EFalse);
	pm1.SetAttrib(EAttBullet);
	// 2 Tab Stops.
	TTabStop tab1,tab2;
	tab1.iTwipsPosition=5010; tab1.iType=TTabStop::ERightTab;
	tab2.iTwipsPosition=5011; tab2.iType=TTabStop::ECenteredTab;
	pf1->StoreTabL(tab1);
	pf1->StoreTabL(tab2);
	pm1.SetAttrib(EAttTabStop);
//
	TRAP(r,pfl1->SetL(pf1,pm1));
	testCopy(*restored,*pfl1);
	test(LayerIsEqual(restored,pfl1));
	test(restored->SenseBase()==pfl1->SenseBase());  // Both should default to based on NULL
//
	delete pf1;
	delete pfl1;
	delete restored;
//
//	Now the CCharFormatLayer Store/Restore
//
	INFO_PRINTF1(_L("CCharFormatLayer"));
// 	
	INFO_PRINTF1(_L("Setting all attributes"));
//	Create test layers.
	CCharFormatLayer* cfl1=NULL;
	CCharFormatLayer* cRestored=NULL;
	TCharFormat cf1; TCharFormatMask cm1;
//
	TRAP(r,cRestored=CCharFormatLayer::NewL());     test(r==KErrNone);
	// Force *restored* to allocate storage for iteself by storing a null layer.
	TCharFormatMask rcm;     rcm.ClearAll(); TCharFormat rcf;
	cRestored->SetL(rcf,rcm);
//
	TRAP(r,cfl1=CCharFormatLayer::NewL());     test(r==KErrNone);
//	
	TRgb color(20,20,20);
	cf1.iFontPresentation.iTextColor=color;	cm1.SetAttrib(EAttColor);
	cf1.iFontSpec.iTypeface.iName=_L("DUNCANXZE");
	cf1.iFontSpec.iTypeface.SetIsProportional(ETrue);	cm1.SetAttrib(EAttFontTypeface);
	cf1.iFontSpec.iTypeface.SetIsSerif(EFalse);
    cf1.iFontSpec.iFontStyle.SetBitmapType(EMonochromeGlyphBitmap);

	cf1.iFontSpec.iHeight=6000;	cm1.SetAttrib(EAttFontHeight);
	cf1.iFontSpec.iFontStyle.SetPosture(EPostureItalic);	cm1.SetAttrib(EAttFontPosture);
	cf1.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);	cm1.SetAttrib(EAttFontStrokeWeight);
	cf1.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSuperscript);	cm1.SetAttrib(EAttFontPrintPos);
	cf1.iFontPresentation.iUnderline=EUnderlineOn;	cm1.SetAttrib(EAttFontUnderline);
	cf1.iFontPresentation.iStrikethrough=EStrikethroughOn;	cm1.SetAttrib(EAttFontStrikethrough);
	cf1.iFontPresentation.iHighlightColor=color;  cm1.SetAttrib(EAttFontHighlightColor);
	cf1.iFontPresentation.iHighlightStyle=TFontPresentation::EFontHighlightNormal;  cm1.SetAttrib(EAttFontHighlightStyle);
//
	TRAP(r,cfl1->SetL(cf1,cm1));
	test(r==KErrNone);
	testCopy(*cRestored,*cfl1);
	test(LayerIsEqual(cRestored,cfl1));

	TCharFormat rfmt;  
    TCharFormatMask rmask;
	cRestored->Sense(rfmt, rmask);
    test(rfmt.iFontSpec.iFontStyle.BitmapType() == EMonochromeGlyphBitmap);

//
	delete cfl1;
	delete cRestored;
	}


TInt CT_CONVS::ChainIsEqual(const CParaFormatLayer* aCopy,const CParaFormatLayer* aOriginal)
//
// Tests that the restored chain is identical to the original chain.
//
	{
	TInt origChainCount=aOriginal->ChainCount();
	/*TInt copyChainCount=*/aCopy->ChainCount();
//	Check the chain heads are equal.
	test(LayerIsEqual(aCopy,aOriginal));
	TInt descendantCount=origChainCount-1;
	
	const CFormatLayer* nextCopyLayer=aCopy->SenseBase();
	const CFormatLayer* nextOrigLayer=aOriginal->SenseBase();
	for (TInt loop=0;loop<descendantCount;loop++)
		{
		test(LayerIsEqual((CParaFormatLayer*)nextCopyLayer,(CParaFormatLayer*)nextOrigLayer));

		nextCopyLayer=nextCopyLayer->SenseBase();
		nextOrigLayer=nextOrigLayer->SenseBase();
		}
	return 1;
	}


TInt CT_CONVS::ChainIsEqual(const CCharFormatLayer* aCopy,const CCharFormatLayer* aOriginal)
//
// Tests that the restored chain is identical to the original chain.
//
	{
	TInt origChainCount=aOriginal->ChainCount();
	/*TInt copyChainCount=*/aCopy->ChainCount();
//	Check the chain heads are equal.
	test(LayerIsEqual(aCopy,aOriginal));
	TInt descendantCount=origChainCount-1;
	
	const CFormatLayer* nextCopyLayer=aCopy->SenseBase();
	const CFormatLayer* nextOrigLayer=aOriginal->SenseBase();
	for (TInt loop=0;loop<descendantCount;loop++)
		{
		test(LayerIsEqual((CCharFormatLayer*)nextCopyLayer,(CCharFormatLayer*)nextOrigLayer));

		nextCopyLayer=nextCopyLayer->SenseBase();
		nextOrigLayer=nextOrigLayer->SenseBase();
		}
	return 1;
	}


void CT_CONVS::DoParaChainL()
//
// Tests the streaming of a chain of format layers
//
	{
	INFO_PRINTF1(_L("Re/StoreChainL()"));
	INFO_PRINTF1(_L("CParaFormatLayer"));
//	Create the chain of para format layers.
	CParaFormatLayer* l1=CParaFormatLayer::NewL();
	CParaFormatLayer* l2=CParaFormatLayer::NewL();
	CParaFormatLayer* l3=CParaFormatLayer::NewL();
	CParaFormatLayer* l4=CParaFormatLayer::NewL();
//	Chain together.
	l1->SetBase(l2);
	l2->SetBase(l3);
	l3->SetBase(l4);
//	Create head of restored format stream, and force it to get storage.	
	CParaFormatLayer* restoredChainHead=CParaFormatLayer::NewL();
	CParaFormat* restoredParaFormat=CParaFormat::NewL();
	TParaFormatMask restoredParaMask;
	restoredParaMask.ClearAll();
	restoredChainHead->SetL(restoredParaFormat,restoredParaMask);
//	General paraformat and its mask.
	CParaFormat* paraFormat=CParaFormat::NewL();
	TParaFormatMask paraMask;
	paraMask.ClearAll();
//	Set layer one stuff
	TTabStop tab1,tab2;
	tab1.iTwipsPosition=5000;	tab2.iTwipsPosition=5001;
	tab1.iType=TTabStop::ERightTab;	tab2.iType=TTabStop::ECenteredTab;
	paraFormat->StoreTabL(tab1);
	paraFormat->StoreTabL(tab2);
	paraMask.SetAttrib(EAttTabStop);
	l1->SetL(paraFormat,paraMask);
	paraMask.ClearAll();
//	Set layer two stuff
	TParaBorder top1;
	top1.iLineStyle=TParaBorder::ESolid;
	top1.iThickness=3;
	top1.iAutoColor=ETrue;
	paraFormat->SetParaBorderL(CParaFormat::EParaBorderTop,top1);
	paraMask.SetAttrib(EAttTopBorder);
	l2->SetL(paraFormat,paraMask);
	paraMask.ClearAll();
//	Set the layer 3 stuff.
	paraFormat->iBullet=new(ELeave)TBullet;
	paraFormat->iBullet->iTypeface.iName=_L("SKELTON");
	paraFormat->iBullet->iTypeface.SetIsProportional(EFalse);
	paraFormat->iBullet->iTypeface.SetIsSerif(EFalse);
	paraFormat->iBullet->iHeightInTwips=3003;
	paraFormat->iBullet->iCharacterCode=32;
	paraMask.SetAttrib(EAttBullet);
	l3->SetL(paraFormat,paraMask);
	paraMask.ClearAll();
// Set the layer 4 stuff.
	paraFormat->iHorizontalAlignment=CParaFormat::EJustifiedAlign;	paraMask.SetAttrib(EAttAlignment);
	paraFormat->iSpaceAfterInTwips=6000;			paraMask.SetAttrib(EAttSpaceAfter);
	paraFormat->iKeepTogether=ETrue;			paraMask.SetAttrib(EAttKeepTogether);
	l4->SetL(paraFormat,paraMask);
// NOW DO IT
	testCopyChain(*restoredChainHead,*l1,0,(const CFormatLayer*)NULL);
	TInt restoredChainCount=restoredChainHead->ChainCount();
	test(ChainIsEqual(restoredChainHead,l1));
//	DESTROY STUFF
	CParaFormatLayer* current=restoredChainHead;
	CParaFormatLayer* next=(CParaFormatLayer*)restoredChainHead->SenseBase();
	delete current;
	for (TInt loop=0;loop<restoredChainCount-1;loop++)	
		{
		current=next;
		next=(CParaFormatLayer*)current->SenseBase();
		delete current;
		}
	delete l1;
	delete l2;
	delete l3;
	delete l4;
	delete paraFormat;
	delete restoredParaFormat;
	}


void CT_CONVS::DoCharChainL()
//
//
//
	{
	INFO_PRINTF1(_L("CCharFormatLayer"));
//	Create the chain of character format layers.
	CCharFormatLayer* cl1=CCharFormatLayer::NewL();
	CCharFormatLayer* cl2=CCharFormatLayer::NewL();
	CCharFormatLayer* cl3=CCharFormatLayer::NewL();
	CCharFormatLayer* cl4=CCharFormatLayer::NewL();
//	Chain together.
	cl1->SetBase(cl2);
	cl2->SetBase(cl3);
	cl3->SetBase(cl4);
//	Create head of restored format stream, and force it to get storage.	
	CCharFormatLayer* rChar=CCharFormatLayer::NewL();
	TCharFormat restoredCharFormat;
	TCharFormatMask restoredCharMask;
	restoredCharMask.ClearAll();
	rChar->SetL(restoredCharFormat,restoredCharMask);
//	General charformat and its mask.
	TCharFormat charFormat;	TCharFormatMask charMask;
	charMask.ClearAll();
//	Set layer one stuff
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);	charMask.SetAttrib(EAttFontStrokeWeight);
	charFormat.iFontSpec.iFontStyle.SetPosture(EPostureItalic);	charMask.SetAttrib(EAttFontPosture);
	charFormat.iFontPresentation.iUnderline=EUnderlineOn;	charMask.SetAttrib(EAttFontUnderline);
	cl1->SetL(charFormat,charMask);
	charMask.ClearAll();
//	Set layer two stuff
	charFormat.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSubscript);	charMask.SetAttrib(EAttFontPrintPos);
	cl2->SetL(charFormat,charMask);
	charMask.ClearAll();
//	Set the layer 3 stuff.
	charFormat.iFontPresentation.iStrikethrough=EStrikethroughOn;	charMask.SetAttrib(EAttFontStrikethrough);
	cl3->SetL(charFormat,charMask);
	charMask.ClearAll();
//	Set the layer 4 stuff.
	charFormat.iFontSpec.iTypeface.iName=_L("Arial");
	charFormat.iFontSpec.iHeight=200;
	charMask.SetAttrib(EAttFontHeight);
	charMask.SetAttrib(EAttFontTypeface);
	cl4->SetL(charFormat,charMask);
//	NOW DO IT
	INFO_PRINTF1(_L("Chain 4 layers deep, terminating on a based on NULL"));
	testCopyChain(*rChar,*cl1,0,(const CFormatLayer*)NULL);
	TInt restoredChainCount=rChar->ChainCount();
	test(ChainIsEqual(rChar,cl1));
//	DESTROY STUFF
	CCharFormatLayer* chCurrent=rChar;
	CCharFormatLayer* chNext=(CCharFormatLayer*)rChar->SenseBase();
	delete chCurrent;
	for (TInt loop=0;loop<restoredChainCount-1;loop++)	
		{
		chCurrent=chNext;
		chNext=(CCharFormatLayer*)chCurrent->SenseBase();
		delete chCurrent;
		}
	delete cl1;
	delete cl2;
	delete cl3;
	delete cl4;
	}
		

void CT_CONVS::DoCharChainVariant1()
//
// Case 2: Where the chain does not terminate at a NULL link.
//
	{
//	Create the chain of character format layers.
	CCharFormatLayer* cl1=CCharFormatLayer::NewL();
	CCharFormatLayer* cl2=CCharFormatLayer::NewL();
	CCharFormatLayer* cl3=CCharFormatLayer::NewL();
	CCharFormatLayer* cl4=CCharFormatLayer::NewL();
//	Chain together.
	cl1->SetBase(cl2);
	cl2->SetBase(cl3);
	cl3->SetBase(cl4);
//	Create head of restored format stream, and force it to get storage.	
	CCharFormatLayer* rChar=CCharFormatLayer::NewL();
	TCharFormat restoredCharFormat;
	TCharFormatMask restoredCharMask;
	restoredCharMask.ClearAll();
	rChar->SetL(restoredCharFormat,restoredCharMask);
//	General charformat and its mask.
	TCharFormat charFormat;	TCharFormatMask charMask;
	charMask.ClearAll();
//	Set layer one stuff
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);	charMask.SetAttrib(EAttFontStrokeWeight);
	charFormat.iFontSpec.iFontStyle.SetPosture(EPostureItalic);	charMask.SetAttrib(EAttFontPosture);
	charFormat.iFontPresentation.iUnderline=EUnderlineOn;	charMask.SetAttrib(EAttFontUnderline);
	cl1->SetL(charFormat,charMask);
	charMask.ClearAll();
//	Set layer two stuff
	charFormat.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSubscript);	charMask.SetAttrib(EAttFontPrintPos);
	cl2->SetL(charFormat,charMask);
	charMask.ClearAll();
//	Set the layer 3 stuff.
	charFormat.iFontPresentation.iStrikethrough=EStrikethroughOn;	charMask.SetAttrib(EAttFontStrikethrough);
	cl3->SetL(charFormat,charMask);
	charMask.ClearAll();
//	Set the layer 4 stuff.
	charFormat.iFontSpec.iTypeface.iName=_L("Arial");
	charFormat.iFontSpec.iHeight=200;
	charMask.SetAttrib(EAttFontHeight);
	charMask.SetAttrib(EAttFontTypeface);
	cl4->SetL(charFormat,charMask);
//	NOW DO IT
	INFO_PRINTF1(_L("Chain 3 layers deep, terminating on a non-NULL based-on"));
	testCopyChain(*rChar,*cl1,1,(const CFormatLayer*)cl4);
	TInt restoredChainCount=rChar->ChainCount();
	test(ChainIsEqual(rChar,cl1));
//	DESTROY STUFF
	CCharFormatLayer* chCurrent=rChar;
	CCharFormatLayer* chNext=(CCharFormatLayer*)rChar->SenseBase();
	delete chCurrent;
	for (TInt loop=0;loop<restoredChainCount-2;loop++)	
		{
		chCurrent=chNext;
		chNext=(CCharFormatLayer*)chCurrent->SenseBase();
		delete chCurrent;
		}
	delete cl1;
	delete cl2;
	delete cl3;
	delete cl4;
	}


void CT_CONVS::testFmtLayerStoreChainL()
//
// Controls the testing of the chainig stuff.
//
	{
	DoParaChainL();
	DoCharChainL();
	DoCharChainVariant1();
//	DoCharChainVariant2();  TO BE IMPLEMENTED
//	doCharChainVariant3();	TO BE IMPLEMENTED
	}


void CT_CONVS::testFmtLayerL()
//
// Tests the streaming of format layers.
//
	{
	testFmtLayerStoreL();
	testFmtLayerStoreChainL();
	}


void CT_CONVS::setupCleanup()
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


void CT_CONVS::DeleteDataFile(const TDesC& aFullName)
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

CT_CONVS::CT_CONVS()
    {
    SetTestStepName(KTestStep_T_CONVS);
    }

TVerdict CT_CONVS::doTestStepL()
    {
    SetTestStepResult(EFail);

    INFO_PRINTF1(_L("T_CONVS - EditableText Persistence"));
    setupCleanup();
    __UHEAP_MARK;

    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-LEGACY-T_CONVS-0001 EText components using Flat Storage "));
    TRAPD(error1, testPlainTextL(CEditableText::EFlatStorage));
    TRAPD(error2, testGlobalTextL(CEditableText::EFlatStorage));
    
    INFO_PRINTF1(_L("EText components using Segmented storage"));
    TRAPD(error3, testPlainTextL(CEditableText::ESegmentedStorage));
    TRAPD(error4, testGlobalTextL(CEditableText::ESegmentedStorage));

    INFO_PRINTF1(_L("Format Layer components"));
    TRAPD(error5, testFmtLayerL());

    __UHEAP_MARKEND;
    DeleteDataFile(KOutputFile);  //deletion of data files must be before call to End() - DEF047652
    delete TheTrapCleanup;

    if(error1 == KErrNone && error2 == KErrNone && error3 == KErrNone && error4 == KErrNone && error5 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
