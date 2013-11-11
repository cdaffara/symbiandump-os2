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


#include <txtfmlyr.h>
#include <txtrich.h>
#include <txtfrmat.h>
#include <gdi.h>
#include "T_FMT.h"

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

template<class S>
class TestFormat
	{
public:
	void CheckAllClassesL();
	void CheckTTabStop();
	void CheckTParaBorder();
	void CheckTBullet();
	void CheckCParaFormatL();
	void CheckCParaFormatSpecialL();
	void CheckCParaFormatTabsEqual(TTabStop& aChecl,TTabStop& aControl);
	void CheckTParaFormatMask();
	void CheckCParaFormatLayerL();
	void CheckCParaFormatLayerRestL();
	void CheckTCharFormat();
	void CheckTCharFormatMask();
	void CheckCCharFormatLayerL();
	void CheckFormatsEqual(CParaFormat* aControl,CParaFormat* aCheck);
	void CheckFormatsEqual(TCharFormat& aControl,TCharFormatMask& aControlMask,TCharFormat& aCheck,TCharFormatMask& aMask);
	void CheckFormatsEqual(TParaFormatMask& aControl,TParaFormatMask& aCheck);
	void CheckFormatsEqual(TCharFormatMask& aControl,TCharFormatMask& aCheck);
	};


template<class S>
void TestFormat<S>::CheckTTabStop()
//
// Checks TTabStop construction and methods.
//
	{
	INFO_PRINTF1(_L("Checking all methods"));
	
	// Default constructor.
	TTabStop tab1;
	// Assignment operator.
	TTabStop tab3;
	tab3=tab1;

	INFO_PRINTF1(_L("Default constructor"));
	test(tab1.iTwipsPosition==0);
	if (tab1.iType==TTabStop::ELeftTab)
	    INFO_PRINTF2(_L("\nleft tab - %d\n"),tab1.iType);
	else if (tab1.iType==TTabStop::ECenteredTab)
	    INFO_PRINTF2(_L("\ncentered tab - %d\n"),tab1.iType);
	else if (tab1.iType==TTabStop::ERightTab)
	    INFO_PRINTF2(_L("\nright tab - %d\n"),tab1.iType);
	else if (tab1.iType==TTabStop::ENullTab)
	    INFO_PRINTF2(_L("\nnull tab - %d\n"),tab1.iType);
	else
	    INFO_PRINTF2(_L("\nsomething completely different - %d \n"),tab1.iType);
	test(tab1.iType==TTabStop::ELeftTab);
//	test.Getch();

	INFO_PRINTF1(_L("Copy constructor"));
	TTabStop tab4;
	tab4.iTwipsPosition=1440;
	tab4.iType=TTabStop::ERightTab;
	TTabStop tab5(tab4);
	test(tab5.iTwipsPosition==tab4.iTwipsPosition);
	test(tab5.iType==tab4.iType);

	INFO_PRINTF1(_L("Assignment operator"));
	tab1=tab5;;
	test(tab1.iTwipsPosition==tab5.iTwipsPosition);
	test(tab1.iType==tab5.iType);

	INFO_PRINTF1(_L("Equality operator"));
	test(tab1==tab5);

	INFO_PRINTF1(_L("Inequality operator"));
	tab1.iTwipsPosition=2;
	test(tab1!=tab5);
	}


template<class S>
void TestFormat<S>::CheckTParaBorder()
//
// Checks TParaBorder construction.
//
	{
	INFO_PRINTF1(_L("Checking all methods"));
	// Default Constructor.
	TParaBorder border1;

	INFO_PRINTF1(_L("Default constructor"));
	test(border1.iLineStyle==TParaBorder::ENullLineStyle);
	test(border1.iAutoColor);
	TLogicalRgb c(TLogicalRgb::ESystemForegroundColor);
	test(border1.iColor == c);

	TParaBorder border2;
	INFO_PRINTF1(_L("Equality operator"));
	test(border2==border1);

	INFO_PRINTF1(_L("Inequality operator"));
	border2.iLineStyle=TParaBorder::ESolid;
	border2.iThickness=2;
	test(border2!=border1);

	border1.iLineStyle=TParaBorder::ESolid;
	border1.iThickness=2;
	test(border2==border1);
	}


template<class S>
void TestFormat<S>::CheckTBullet()
//
// Checks TBullet construction.
//
	{
	INFO_PRINTF1(_L("Checking all methods"));
	// Default constructor.
	TBullet bullet;

	INFO_PRINTF1(_L("Default constructor"));
	test(0x2022==bullet.iCharacterCode);
	test(bullet.iHeightInTwips==0);

	INFO_PRINTF1(_L("==/!="));
	TBullet bullet2;
	test(bullet==bullet2);
	test(!(bullet!=bullet2));

	TBullet bullet3;
	bullet3.iCharacterCode=45;
	test(bullet!=bullet3);
	test(!(bullet==bullet3));
	}


template<class S>
void TestFormat<S>::CheckCParaFormatTabsEqual(TTabStop& aCheck,TTabStop& aControl)
//
// Check the 2 TTabStop structs are equal.
//
	{test(aCheck==aControl);}


template<class S>
void TestFormat<S>::CheckCParaFormatL()
//
//	Checks CParaFormat construction and methods.
//
	{
	INFO_PRINTF1(_L("Checking all methods"));
	CheckCParaFormatSpecialL();
	__UHEAP_MARK;

	TInt failRate;
	CParaFormat* pp=NULL;
	for (failRate=1;;failRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		TRAPD(ret,pp=CParaFormat::NewL());
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			test(pp==NULL);
			}
		else
			{
			test(pp!=NULL);
			delete pp;
			__UHEAP_MARKEND;
			break;
			}
		}
	__UHEAP_RESET;

	
	
	
	CParaFormat* format=CParaFormat::NewL();

	INFO_PRINTF1(_L("Tab methods"));
	TTabStop control[5];
	control[4].iTwipsPosition=KMaxTUint32;
	control[3].iTwipsPosition=8640;
	control[2].iTwipsPosition=5760;
	control[1].iTwipsPosition=2880;
 	control[0].iTwipsPosition=1;

	control[0].iType=TTabStop::ERightTab;
	control[1].iType=TTabStop::ECenteredTab;
	
// Store the tabs.	
	test(format->TabCount()==0);
	format->StoreTabL(control[4]);
	test(format->TabCount()==1);
	format->StoreTabL(control[2]);
	test(format->TabCount()==2);
	format->StoreTabL(control[1]);
	test(format->TabCount()==3);
	format->StoreTabL(control[3]);
	test(format->TabCount()==4);
	format->StoreTabL(control[0]);
	test(format->TabCount()==5);

// Read the tabs.
	TTabStop buf;
	TInt tc1=format->TabCount();
	for (TInt count=0;count<tc1;count++)
		{
		buf=format->TabStop(count);
		CheckCParaFormatTabsEqual(buf,control[count]);
		}

// RemoveAllTabs
	format->RemoveAllTabs();
	TInt tabCount=format->TabCount();
	test(tabCount==0);

// Remove the tabs.
	format->RemoveTab(5760);
	test(format->TabCount()==0);
	format->RemoveTab(2880);
	test(format->TabCount()==0);
	format->RemoveTab(8640);
	test(format->TabCount()==0);
	format->RemoveTab(1);
	test(format->TabCount()==0);
	format->RemoveTab(KMaxTUint32);
	test(format->TabCount()==0);

	delete format;
	format=NULL;
	__UHEAP_MARKEND;
	}


template<class S>
void TestFormat<S>::CheckCParaFormatSpecialL()
//
// Checks CParaFormat construction and methods.
//
	{
	__UHEAP_MARK;
	CParaFormat* format=CParaFormat::NewLC();
	CParaFormat* newFormat=CParaFormat::NewLC();
//	Tab functions
	format->TabCount();
	TTabStop tab;
	format->StoreTabL(tab);
	format->TabStop(0);
	format->RemoveTab(0);
	format->RemoveAllTabs();
//	Border functions
	TParaBorder testBorder;
	format->RemoveAllBorders();
	format->BordersPresent();
	format->AllBordersEqual(*newFormat);
	format->SetParaBorderL(CParaFormat::EParaBorderTop,testBorder);
	format->IsBorderEqual(CParaFormat::EParaBorderBottom,*newFormat);
	CleanupStack::PopAndDestroy(2);
	format=NULL;
	__UHEAP_MARKEND;
	TInt ret;
	TInt check=0;
#ifdef _DEBUG
	__UHEAP_MARK;
	// Construction.
	INFO_PRINTF1(_L("Construction failing on OOM")); 
	__UHEAP_FAILNEXT(1);
	TRAP(ret,format=CParaFormat::NewL());
	if (ret!=KErrNone)
		check=1;
    // seems __UHEAP_FAILNEXT does not work well in platsim.
    // below test does not pass in platsim.
//	test(check==1);
#endif
	INFO_PRINTF1(_L("Construction succeeding"));
	check=0;
	TRAP(ret,format=CParaFormat::NewL());
	if (ret!=KErrNone)
		check++;
	test(check==0);
	//
	TLogicalRgb fillColor(TLogicalRgb::ESystemBackgroundColor);
	test(format->iFillColor==fillColor);
	test(format->iLanguage==0);
	test(format->iLeftMarginInTwips==0);
	test(format->iRightMarginInTwips==0);
	test(format->iIndentInTwips==0);
	test(format->iBorderMarginInTwips==0);
	test(format->iSpaceBeforeInTwips==0);
	test(format->iSpaceAfterInTwips==0);
	test(format->iHorizontalAlignment==CParaFormat::ELeftAlign);
	test(format->iVerticalAlignment==CParaFormat::EUnspecifiedAlign);
	test(format->iKeepTogether==EFalse);
	test(format->iKeepWithNext==EFalse);
	test(format->iStartNewPage==EFalse);
	test(format->iWidowOrphan==EFalse);
	test(format->iWrap);
	test(!format->BordersPresent());
	test(format->iBullet==NULL);
	test(format->iLineSpacingInTwips==200);
	test(format->iLineSpacingControl==CParaFormat::ELineSpacingAtLeastInTwips);
	test(format->iDefaultTabWidthInTwips==360);

	INFO_PRINTF1(_L("Equality operator"));
	CParaFormat* two=CParaFormat::NewL();
	test(two->IsEqual(*format));
	delete two;

	INFO_PRINTF1(_L("Copy constructor"));
	CParaFormat* three=CParaFormat::NewL(*two);
	test(three->IsEqual(*two));
	delete three;


	// Destroy()
	INFO_PRINTF1(_L("Destroy()"));	
	delete format;
	format=NULL;
	__UHEAP_MARKEND;
	}


template<class S>
void TestFormat<S>::CheckTParaFormatMask()
//
// Checks TParaFormatMask construction and methods.
//
	{
	__UHEAP_MARK;
	TInt count=0;
// All methods.
	INFO_PRINTF1(_L("Checking all methods"));
	TParaFormatMask mask;
	mask.SetAttrib(EAttLeftMargin);	
	mask.AttribIsSet(EAttLeftMargin);
	mask.ClearAttrib(EAttLeftMargin);
	TParaFormatMask maskTemp;
	test(maskTemp==mask);
	test(!(maskTemp!=mask));

// Construction.
	INFO_PRINTF1(_L("Construction"));
	TParaFormatMask mask1;
	for (count=EAttParaLanguage;count<ETextFormatAttributeCount;count++)
		{
		test(mask1.AttribIsSet((TTextFormatAttribute)count)==EFalse);
		}
	

// SetAttrib()
	INFO_PRINTF1(_L("SetAttrib()"));
	for (count=EAttParaLanguage;count<ETextFormatAttributeCount;count++)
		{
		mask1.SetAttrib((TTextFormatAttribute)count);
		}

// ClearAttrib()
	INFO_PRINTF1(_L("ClearAttrib()"));
	for (count=EAttParaLanguage;count<ETextFormatAttributeCount;count++)
		{
		mask1.ClearAttrib((TTextFormatAttribute)count);
		}
	for (count=EAttParaLanguage;count<ETextFormatAttributeCount;count++)
		{
		test(mask1.AttribIsSet((TTextFormatAttribute)count)==EFalse);
		}
// AttribIsSet()
	INFO_PRINTF1(_L("AttribIsSet()"));
	// Already tested in the above.

	INFO_PRINTF1(_L("SetAll()"));
	TParaFormatMask mask2;
	mask2.SetAll();  // sets border container but not individual borders.
 	for (count=EAttParaLanguage;count<EAttTabStop;count++)
		{
		test(mask2.AttribIsSet((TTextFormatAttribute)count));
		}

	INFO_PRINTF1(_L("ClearAll()"));
	mask2.ClearAll();
 	for (count=EAttParaLanguage;count<EAttTabStop;count++)
		{
		test(mask2.AttribIsSet((TTextFormatAttribute)count)==EFalse);
		}
	mask2.SetAttrib(EAttLeftMargin);
	test(mask2.AttribIsSet(EAttLeftMargin));
	
	__UHEAP_MARKEND;
	}


template<class S>
void TestFormat<S>::CheckFormatsEqual(TCharFormat& aControl,TCharFormatMask& aControlMask,TCharFormat& aCheck,TCharFormatMask& aMask)
//
// Checks that 2 TCharFormats are exactly equal.
//
	{
	test(aControlMask==aMask);
	test(aControl.IsEqual(aCheck,aMask));	
	}


template<class S>
void TestFormat<S>::CheckFormatsEqual(CParaFormat* aControl,CParaFormat* aCheck)
//
// Checks 2 CParaFormats are exactly equal.
//
	{test(aControl->IsEqual(*aCheck));}


template<class S>
void TestFormat<S>::CheckFormatsEqual(TParaFormatMask& aControl,TParaFormatMask& aCheck)
//
// Checks the guards are exactly the same.
//
	{test(aControl==aCheck);}


template<class S>
void TestFormat<S>::CheckCCharFormatLayerL()
//
// Checks CCharFormatLayer constructor and methods.
//
	{
	__UHEAP_MARK;

	TParaFormatMask mmm;
	CParaFormatLayer* pL=CParaFormatLayer::NewL();
	CParaFormatLayer* pL1=CParaFormatLayer::NewL(NULL,mmm);
	mmm.SetAttrib(EAttLeftMargin);
	CParaFormat ppp;
	ppp.iLeftMarginInTwips=1000;
	CParaFormatLayer* pL2=CParaFormatLayer::NewL(&ppp,mmm);

	test(pL->IsEmpty());
	test(pL1->IsEmpty());
	test(!(pL2->IsEmpty()));

	delete pL2;
	delete pL1;
	delete pL;
	
	TCharFormat format1,format2,format3,formatCmp;
	TCharFormatMask f1,f2,f3,fCmp;

	TRgb Color1(10,10,10);
	TRgb Color2(50,50,50);
	TRgb hlColor1(3,4,5);

	// Setup layer 1 values
	format1.iFontPresentation.iTextColor=Color1;	f1.SetAttrib(EAttColor);
	format1.iFontSpec.iHeight=1400;	f1.SetAttrib(EAttFontHeight);
	format1.iFontSpec.iFontStyle.SetPosture(EPostureItalic);	f1.SetAttrib(EAttFontPosture);
	format1.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSubscript);	f1.SetAttrib(EAttFontPrintPos);
	// Setup layer 2 values
	format2.iFontPresentation.iHighlightColor=hlColor1; f2.SetAttrib(EAttFontHighlightColor);
	format2.iFontPresentation.iHighlightStyle=TFontPresentation::EFontHighlightRounded;  f2.SetAttrib(EAttFontHighlightStyle);
	format2.iFontPresentation.iStrikethrough=EStrikethroughOn;	f2.SetAttrib(EAttFontStrikethrough);
	format2.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);	f2.SetAttrib(EAttFontStrokeWeight);
	format2.iFontPresentation.iUnderline=EUnderlineOn;	f2.SetAttrib(EAttFontUnderline);
	format2.iFontSpec.iTypeface.iName=(_L("Moffat"));
	format2.iFontSpec.iTypeface.SetIsProportional(ETrue);
	format2.iFontSpec.iTypeface.SetIsSerif(ETrue);
    format2.iFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	f2.SetAttrib(EAttFontTypeface);
	// Setup layer 3 values - should be ignored cos they're already present.
	format3.iFontPresentation.iTextColor=Color2;	f3.SetAttrib(EAttColor);
	format3.iFontPresentation.iHighlightStyle=TFontPresentation::EFontHighlightNormal;  f3.SetAttrib(EAttFontHighlightStyle);
	format3.iFontSpec.iHeight=2800;	f3.SetAttrib(EAttFontHeight);
	format3.iFontSpec.iFontStyle.SetPosture(EPostureUpright);	f3.SetAttrib(EAttFontPosture);
	format3.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSuperscript);	f3.SetAttrib(EAttFontPrintPos);
	format3.iFontPresentation.iStrikethrough=EStrikethroughOff;	f3.SetAttrib(EAttFontStrikethrough);
	format3.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);	f3.SetAttrib(EAttFontStrokeWeight);
	format3.iFontPresentation.iUnderline=EUnderlineOff;	f3.SetAttrib(EAttFontUnderline);
	format3.iFontSpec.iTypeface.iName=(_L("Moon"));
	format3.iFontSpec.iTypeface.SetIsProportional(EFalse);
	format3.iFontSpec.iTypeface.SetIsSerif(EFalse);
	f3.SetAttrib(EAttFontTypeface);
	// Setup formatCmp values - the expected result of format layering.
	formatCmp.iFontPresentation.iTextColor=Color1;	fCmp.SetAttrib(EAttColor);
	formatCmp.iFontPresentation.iHighlightColor=hlColor1; fCmp.SetAttrib(EAttFontHighlightColor);
	formatCmp.iFontPresentation.iHighlightStyle=TFontPresentation::EFontHighlightRounded;  fCmp.SetAttrib(EAttFontHighlightStyle);
	formatCmp.iFontSpec.iHeight=1400;	fCmp.SetAttrib(EAttFontHeight);
	formatCmp.iFontSpec.iFontStyle.SetPosture(EPostureItalic);	fCmp.SetAttrib(EAttFontPosture);
	formatCmp.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSubscript);	fCmp.SetAttrib(EAttFontPrintPos);
	formatCmp.iFontPresentation.iStrikethrough=EStrikethroughOn;	fCmp.SetAttrib(EAttFontStrikethrough);
	formatCmp.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);	fCmp.SetAttrib(EAttFontStrokeWeight);
	formatCmp.iFontPresentation.iUnderline=EUnderlineOn;	fCmp.SetAttrib(EAttFontUnderline);
	formatCmp.iFontSpec.iTypeface.iName=(_L("Moffat"));
	formatCmp.iFontSpec.iTypeface.SetIsProportional(ETrue);
	formatCmp.iFontSpec.iTypeface.SetIsSerif(ETrue);
    formatCmp.iFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	fCmp.SetAttrib(EAttFontTypeface);
	// Store the formats in char format layers
	CCharFormatLayer* formatLayer3=CCharFormatLayer::NewL(format3,f3);
	CCharFormatLayer* formatLayer2=CCharFormatLayer::NewL(format2,f2);
	CCharFormatLayer* formatLayer1=CCharFormatLayer::NewL(format1,f1);
	formatLayer1->SetBase(formatLayer2);
	INFO_PRINTF1(_L("ChainCount()"));
	test(formatLayer1->ChainCount()==2);
	// Now read them in and compare them:
	// First just the layers.
	INFO_PRINTF1(_L("SenseL() - Sensing this layer only"));
	TCharFormat result1;
	TCharFormatMask result1Mask;
	formatLayer1->Sense(result1,result1Mask);
	CheckFormatsEqual(format1,f1,result1,result1Mask);
		
	TCharFormat result2;
	TCharFormatMask result2Mask;
	formatLayer2->Sense(result2,result2Mask);
	CheckFormatsEqual(format2,f2,result2,result2Mask);
	test(result2.iFontSpec.iFontStyle.BitmapType() == format2.iFontSpec.iFontStyle.BitmapType());
	test(result2.iFontSpec.iFontStyle.BitmapType() == EAntiAliasedGlyphBitmap);
		
	// Now check the effective formats are correct
	INFO_PRINTF1(_L("SenseEffectiveL() - utilising basedOn"));
	TCharFormatMask dummy;
	TCharFormat result3;
	formatLayer1->SenseEffective(result3);
	CheckFormatsEqual(format3,dummy,result3,dummy);
		
	// Now check the effective formats are correct
	// The result should be the same as above,
	// since all these values are present in the resultant TCharFormat.
	// Ie, checking that overlapping attributes in a lower layer are not taken.
	INFO_PRINTF1(_L("SenseEffectiveL() - checking overlapping attributes are ignored"));
	// Add another layer of formatting by implementing the next based on link.
	formatLayer2->SetBase(formatLayer3);
		INFO_PRINTF1(_L("ChainCount()"));
		test(formatLayer1->ChainCount()==3);
		test(formatLayer2->ChainCount()==2);
		test(formatLayer3->ChainCount()==1);
	TCharFormat result4;
	formatLayer1->SenseEffective(result4);
	CheckFormatsEqual(format3,dummy,result4,dummy);

	delete formatLayer1;
	delete formatLayer2;
	delete formatLayer3;
	__UHEAP_MARKEND;
	}


template<class S>
void TestFormat<S>::CheckCParaFormatLayerRestL()
//
// Checks combinations of set and sense with format layering.
//
	{
	__UHEAP_MARK;
	CParaFormat* format1=CParaFormat::NewL();
	TParaFormatMask format1Mask;

	CParaFormat* format2=CParaFormat::NewL();
	TParaFormatMask format2Mask;
	
	CParaFormat* format3=CParaFormat::NewL();
	
	CParaFormat* format4=CParaFormat::NewL();
	TParaFormatMask format4Mask;

	TRgb Color1(10,10,10);
	TRgb Color2(50,50,50);
	TRgb Color3(100,100,100);
	TRgb Color4(150,150,150);

	TTabStop tab1layer1,tab2layer1,tab3layer1;
	tab1layer1.iTwipsPosition=2520; tab1layer1.iType=TTabStop::ECenteredTab;
	tab2layer1.iTwipsPosition=3960; tab2layer1.iType=TTabStop::ERightTab;
	tab3layer1.iTwipsPosition=11160;tab3layer1.iType=TTabStop::ELeftTab;
	TTabStop tab1layer2,tab2layer2,tab3layer2;
	tab1layer2.iTwipsPosition=2160; tab1layer2.iType=TTabStop::ENullTab;
	tab2layer2.iTwipsPosition=3600; tab2layer2.iType=TTabStop::ENullTab;
	tab3layer2.iTwipsPosition=10080;tab3layer2.iType=TTabStop::ENullTab;
	TTabStop tab1layer3,tab2layer3,tab3layer3;
	tab1layer3.iTwipsPosition=2160; tab1layer3.iType=TTabStop::ECenteredTab;
	tab2layer3.iTwipsPosition=3600; tab2layer3.iType=TTabStop::ERightTab;
	tab3layer3.iTwipsPosition=10080;tab3layer3.iType=TTabStop::ELeftTab;
	// Setup layer 1 values.
	format1->iFillColor=Color1; format1Mask.SetAttrib(EAttFillColor);
	format1->iLeftMarginInTwips=1440; format1Mask.SetAttrib(EAttLeftMargin);
	format1->iRightMarginInTwips=7000; format1Mask.SetAttrib(EAttRightMargin);
	format1->iIndentInTwips=1500; format1Mask.SetAttrib(EAttIndent);
	format1->iHorizontalAlignment=CParaFormat::ERightAlign; format1Mask.SetAttrib(EAttAlignment);
	format1->iVerticalAlignment=CParaFormat::EBottomAlign; format1Mask.SetAttrib(EAttVerticalAlignment);
	format1->iLineSpacingInTwips=12; format1Mask.SetAttrib(EAttLineSpacing);
	format1->iSpaceBeforeInTwips=6; format1Mask.SetAttrib(EAttSpaceBefore);
	format1->iSpaceAfterInTwips=7; format1Mask.SetAttrib(EAttSpaceAfter);
	format1->iKeepTogether=ETrue; format1Mask.SetAttrib(EAttKeepTogether);
	format1->iKeepWithNext=ETrue; format1Mask.SetAttrib(EAttKeepWithNext);
	format1->iStartNewPage=ETrue; format1Mask.SetAttrib(EAttStartNewPage);
	format1->iWidowOrphan=ETrue; format1Mask.SetAttrib(EAttWidowOrphan);
	format1->iWrap=EFalse; format1Mask.SetAttrib(EAttWrap);
	format1->iDefaultTabWidthInTwips=2880; format1Mask.SetAttrib(EAttDefaultTabWidth);
	format1->StoreTabL(tab1layer1); format1->StoreTabL(tab2layer1); format1->StoreTabL(tab3layer1);
	format1Mask.SetAttrib(EAttTabStop);
	// Setup format 2 values.
	TParaBorder topBorder;
	topBorder.iLineStyle=TParaBorder::ESolid;
	topBorder.iThickness=1;
	topBorder.iAutoColor=ETrue;
	topBorder.iColor=Color1;
	format2->SetParaBorderL(CParaFormat::EParaBorderTop,topBorder);
	format2Mask.SetAttrib(EAttTopBorder);
	//
	TParaBorder bottomBorder;
	bottomBorder.iLineStyle=TParaBorder::ESolid;
	bottomBorder.iThickness=1;
	bottomBorder.iAutoColor=ETrue;
	bottomBorder.iColor=Color2;
	format2->SetParaBorderL(CParaFormat::EParaBorderBottom,bottomBorder);
	format2Mask.SetAttrib(EAttBottomBorder);
	//
	TParaBorder leftBorder;
	leftBorder.iLineStyle=TParaBorder::ESolid;
	leftBorder.iThickness=1;
	leftBorder.iAutoColor=ETrue;
	leftBorder.iColor=Color3;
	format2->SetParaBorderL(CParaFormat::EParaBorderLeft,leftBorder);
	format2Mask.SetAttrib(EAttLeftBorder);
	//
	TParaBorder rightBorder;
	rightBorder.iLineStyle=TParaBorder::ESolid;
	rightBorder.iThickness=1;
	rightBorder.iAutoColor=ETrue;
	rightBorder.iColor=Color4;
	format2->SetParaBorderL(CParaFormat::EParaBorderRight,rightBorder);
	format2Mask.SetAttrib(EAttRightBorder);
	//
	format2->StoreTabL(tab1layer2); format2->StoreTabL(tab2layer2); format2->StoreTabL(tab3layer2);
	format2Mask.SetAttrib(EAttTabStop);
	format2->iBullet=new(ELeave)TBullet;	format2Mask.SetAttrib(EAttBullet);
	format2->iBullet->iHeightInTwips=200;
	format2->iBullet->iCharacterCode=202;
	format2->iBullet->iTypeface.iName=(_L("Symbol"));
	format2->iBullet->iTypeface.SetIsProportional(ETrue);
	format2->iBullet->iTypeface.SetIsSerif(ETrue);
	// Setup format 3 values - The resulting format after hunting based on links.
	format3->iFillColor=Color1;
	format3->iLeftMarginInTwips=1440;
	format3->iRightMarginInTwips=7000;
	format3->iIndentInTwips=1500;
	format3->iHorizontalAlignment=CParaFormat::ERightAlign;
	format3->iVerticalAlignment=CParaFormat::EBottomAlign;
	format3->iLineSpacingInTwips=12;
	format3->iSpaceBeforeInTwips=6;
	format3->iSpaceAfterInTwips=7;
	format3->iKeepTogether=ETrue;
	format3->iKeepWithNext=ETrue;
	format3->iStartNewPage=ETrue;
	format3->iWidowOrphan=ETrue;
	format3->iWrap=EFalse;
	format3->iDefaultTabWidthInTwips=2880;
  	TParaBorder top;
	top.iLineStyle=TParaBorder::ESolid;
	top.iThickness=1;
	top.iAutoColor=ETrue;
	top.iColor=Color1;
	TParaBorder bottom;
	bottom.iLineStyle=TParaBorder::ESolid;
	bottom.iThickness=1;
	bottom.iAutoColor=ETrue;
	bottom.iColor=Color2;
	TParaBorder left;
	left.iLineStyle=TParaBorder::ESolid;
	left.iThickness=1;
	left.iAutoColor=ETrue;
	left.iColor=Color3;
	TParaBorder right;
	right.iThickness=1;
	right.iLineStyle=TParaBorder::ESolid;
	right.iAutoColor=ETrue;
	right.iColor=Color4;
	format3->SetParaBorderL(CParaFormat::EParaBorderTop,top);
	format3->SetParaBorderL(CParaFormat::EParaBorderBottom,bottom);
	format3->SetParaBorderL(CParaFormat::EParaBorderLeft,left);
	format3->SetParaBorderL(CParaFormat::EParaBorderRight,right);
	//
	format3->StoreTabL(tab1layer1); format3->StoreTabL(tab2layer1); format3->StoreTabL(tab3layer1);
//	format3->StoreTabL(tab1layer2); format3->StoreTabL(tab2layer2); format3->StoreTabL(tab3layer2);
	format3->iBullet=new(ELeave)TBullet;
	format3->iBullet->iHeightInTwips=200;
	format3->iBullet->iCharacterCode=202;
	format3->iBullet->iTypeface.iName=(_L("Symbol"));
	format3->iBullet->iTypeface.SetIsProportional(ETrue);
	format3->iBullet->iTypeface.SetIsSerif(ETrue);
	// Setup format 4 values - The resulting format after hunting based on links.
	// and ignoring all these attributes since they are already set in the CParaFormat.
 	format4->iLeftMarginInTwips=6666; format4Mask.SetAttrib(EAttLeftMargin);
	format4->iRightMarginInTwips=6666; format4Mask.SetAttrib(EAttRightMargin);
	format4->iIndentInTwips=6666; format4Mask.SetAttrib(EAttIndent);
	format4->iHorizontalAlignment=CParaFormat::ERightAlign; format4Mask.SetAttrib(EAttAlignment);
	format4->iLineSpacingInTwips=6666; format4Mask.SetAttrib(EAttLineSpacing);
	format4->iSpaceBeforeInTwips=6666; format4Mask.SetAttrib(EAttSpaceBefore);
	format4->iSpaceAfterInTwips=6666; format4Mask.SetAttrib(EAttSpaceAfter);
	format4->iKeepTogether=EFalse; format4Mask.SetAttrib(EAttKeepTogether);
	format4->iKeepWithNext=EFalse; format4Mask.SetAttrib(EAttKeepWithNext);
	format4->iStartNewPage=EFalse; format4Mask.SetAttrib(EAttStartNewPage);
	format4->iWidowOrphan=EFalse; format4Mask.SetAttrib(EAttWidowOrphan);
	format4->iDefaultTabWidthInTwips=6666; format4Mask.SetAttrib(EAttDefaultTabWidth);
	TParaBorder zTop;
	zTop.iLineStyle=TParaBorder::EDashed;
	format4Mask.SetAttrib(EAttTopBorder);
	zTop.iAutoColor=EFalse;
	TParaBorder zBottom;
	zBottom.iLineStyle=TParaBorder::EDashed;
	format4Mask.SetAttrib(EAttBottomBorder);
	zBottom.iAutoColor=EFalse;
	TParaBorder zLeft;
	zLeft.iLineStyle=TParaBorder::EDashed;
	format4Mask.SetAttrib(EAttLeftBorder);
	zLeft.iAutoColor=EFalse;
	TParaBorder zRight;
	zRight.iLineStyle=TParaBorder::EDashed;
	format4Mask.SetAttrib(EAttRightBorder);
	zRight.iAutoColor=EFalse;
	format4->SetParaBorderL(CParaFormat::EParaBorderTop,zTop);
	format4->SetParaBorderL(CParaFormat::EParaBorderBottom,zBottom);
	format4->SetParaBorderL(CParaFormat::EParaBorderLeft,zLeft);
	format4->SetParaBorderL(CParaFormat::EParaBorderRight,zRight);
	//
	format4->StoreTabL(tab1layer3); format4->StoreTabL(tab2layer3); format4->StoreTabL(tab3layer3);
	format4Mask.SetAttrib(EAttTabStop);
	format4->iBullet=new(ELeave)TBullet; format4Mask.SetAttrib(EAttBullet);
	format4->iBullet->iHeightInTwips=240;
	format4->iBullet->iCharacterCode=201;
	format4->iBullet->iTypeface.iName=(_L("Windings"));
	format4->iBullet->iTypeface.SetIsSerif(ETrue);
	format4->iBullet->iTypeface.SetIsProportional(EFalse);
	// Store the formats in para format layers
	CParaFormatLayer* formatLayer4=CParaFormatLayer::NewL(format4,format4Mask);

	CParaFormatLayer* formatLayer2=CParaFormatLayer::NewL(format2,format2Mask);
	
	CParaFormatLayer* formatLayer=CParaFormatLayer::NewL(format1,format1Mask);
	formatLayer->SetBase(formatLayer2);

	// Now read them in and compare them:
	// First just the layers.
	INFO_PRINTF1(_L("SenseL() - Sensing this layer only"));
	CParaFormat* formatResult1=CParaFormat::NewL();
	TParaFormatMask formatResult1Mask;
	formatLayer->SenseL(formatResult1,formatResult1Mask);
	CheckFormatsEqual(format1,formatResult1);
	CheckFormatsEqual(format1Mask,formatResult1Mask);
	delete formatResult1;

	formatResult1=CParaFormat::NewL();
	formatResult1Mask.ClearAll();
	formatLayer2->SenseL(formatResult1,formatResult1Mask);
	CheckFormatsEqual(format2,formatResult1);
	CheckFormatsEqual(format2Mask,formatResult1Mask);
	delete formatResult1;

	// Now check the effective formats are correct
	INFO_PRINTF1(_L("SenseEffectiveL() - utilising basedOn"));
	CParaFormat* formatResult2=CParaFormat::NewL();
	formatLayer->SenseEffectiveL(formatResult2);
	CheckFormatsEqual(format3,formatResult2);
	delete formatResult2;
	
	// Now check the effective formats are correct
	// The result should be the same as above,
	// since all these values are present in the resultant CParaFormat.
	// Ie, checking that overlapping attributes in a lower layer are not taken.
	INFO_PRINTF1(_L("SenseEffectiveL() - checking overlapping attributes are ignored"));
	// Add another layer of formatting by implementing the next based on link.
	formatLayer2->SetBase(formatLayer4);
	formatResult2=CParaFormat::NewL();
	formatLayer->SenseEffectiveL(formatResult2);
	CheckFormatsEqual(format3,formatResult2);
	delete formatResult2;
	
	// Test ChainCount() method
	INFO_PRINTF1(_L("ChainCount()"));
	test(formatLayer4->ChainCount()==1);
	test(formatLayer2->ChainCount()==2);
	test(formatLayer->ChainCount()==3);

	// Now clean up.
	delete formatLayer;
	delete formatLayer2;
	delete formatLayer4;
	//formatLayer3 does not exist, so each can use it's own same numbered format.
	delete format1;
	delete format2;
	delete format3;
	delete format4;
	__UHEAP_MARKEND;
	}


LOCAL_C void CheckBulletInheritance()
//
// Checks correct inheritance of bullets.
//
	{
	INFO_PRINTF1(_L("Testing bullet inheritance"));
	__UHEAP_MARK;
	
	CParaFormatLayer* baseLayer=CParaFormatLayer::NewL();
	CParaFormatLayer* specificLayer=CParaFormatLayer::NewL();
	specificLayer->SetBase(baseLayer);
	CParaFormat* paraFormat=CParaFormat::NewLC();
	paraFormat->iBullet=new(ELeave) TBullet;
	paraFormat->iBullet->iHeightInTwips=200;
	paraFormat->iBullet->iCharacterCode=202;
	TParaFormatMask paraMask;
	paraMask.SetAttrib(EAttBullet);
	specificLayer->SetL(paraFormat,paraMask);
	//
	// specific bullet over null inherited
	INFO_PRINTF1(_L("Specific bullet over null inherited"));
	CParaFormat* sensed=CParaFormat::NewLC();
	specificLayer->SenseEffectiveL(sensed);
	test(sensed->iBullet!=NULL);
	test(sensed->iBullet->iHeightInTwips==200);
	CleanupStack::PopAndDestroy();  // sensed
	//
	// null bullet over inherited
	INFO_PRINTF1(_L("Null bullet over inherited"));
	baseLayer->Reset();
	specificLayer->Reset();
	baseLayer->SetL(paraFormat,paraMask);
	CParaFormat* empty=CParaFormat::NewLC();
	TParaFormatMask temp;
	temp.SetAttrib(EAttBullet);
	specificLayer->SetL(empty,temp);
	CleanupStack::PopAndDestroy();  // empty
	sensed=CParaFormat::NewLC();
	specificLayer->SenseEffectiveL(sensed);
	CleanupStack::PopAndDestroy();  // sensed
//	test(sensed->iBullet==NULL);
	//
	// non-null bullet over inherited bullet
	INFO_PRINTF1(_L("Non-Null bullet over inherited"));
	specificLayer->Reset();
	paraFormat->iBullet->iHeightInTwips=1000;
	specificLayer->SetL(paraFormat,paraMask);
	sensed=CParaFormat::NewLC();
	specificLayer->SenseEffectiveL(sensed);
	test(sensed->iBullet!=NULL);
	test(sensed->iBullet->iHeightInTwips==1000);
	CleanupStack::PopAndDestroy();  // sensed
	
	CleanupStack::PopAndDestroy();  // paraFormat.
	delete specificLayer;
	delete baseLayer;

	__UHEAP_MARKEND;
	}
	
	
template<class S>
void TestFormat<S>::CheckCParaFormatLayerL()
//
// Checks CParaFormatLayer construction and methods.
//
	{
	__UHEAP_MARK;

	
	TInt ret=0;
	TInt check=0;
	CParaFormatLayer* layer0=NULL;

	INFO_PRINTF1(_L("Constructor"));
#ifdef _DEBUG
	INFO_PRINTF1(_L("Failing on OOM"));
	__UHEAP_FAILNEXT(1);
	TRAP(ret,layer0=CParaFormatLayer::NewL());
	if (ret!=KErrNone)
		check++;
    // seems __UHEAP_FAILNEXT does not work well in platsim.
    // below test does not pass in platsim.
//	test(check>0);
#endif

	INFO_PRINTF1(_L("Succeeding"));
	check=0;
	TRAP(ret,layer0=CParaFormatLayer::NewL());
	if (ret!=KErrNone)
		check++;
	test(check==0);
	delete layer0;

// Set/Sense Default Para Format.	
	INFO_PRINTF1(_L("Set/Sense Default ParaFormat"));
	CParaFormat* defaultFormat=CParaFormat::NewL();
	
	//to test EAttParaLanguageX
	defaultFormat->iLanguage |= 0x100;
	
	//set iBullet as a valid bullet to test EAttBullet and EAttBulletX
	defaultFormat->iBullet=new(ELeave)TBullet;
	defaultFormat->iBullet->iStyle = TBullet::EBulletStyle; 
	defaultFormat->iBullet->iHeightInTwips = 1;
	
	TParaFormatMask defaultGuard;
	defaultGuard.SetAttrib(EAttParaLanguage);
	defaultGuard.SetAttrib(EAttBullet);
	defaultGuard.SetAttrib(EAttLeftMargin);
	defaultGuard.SetAttrib(EAttRightMargin);
	defaultGuard.SetAttrib(EAttIndent);
	defaultGuard.SetAttrib(EAttAlignment);
	defaultGuard.SetAttrib(EAttLineSpacing);
	defaultGuard.SetAttrib(EAttLineSpacingControl);
	defaultGuard.SetAttrib(EAttSpaceBefore);
	defaultGuard.SetAttrib(EAttSpaceAfter);
	defaultGuard.SetAttrib(EAttKeepTogether);
	defaultGuard.SetAttrib(EAttKeepWithNext);
	defaultGuard.SetAttrib(EAttStartNewPage);
	defaultGuard.SetAttrib(EAttWidowOrphan);
	defaultGuard.SetAttrib(EAttBorderMargin);
	defaultGuard.SetAttrib(EAttDefaultTabWidth);

	CParaFormat* buffer1=CParaFormat::NewL();
	TParaFormatMask bufferGuard1;
	CParaFormat* buffer2=CParaFormat::NewL();
	
	CParaFormatLayer* testing1=CParaFormatLayer::NewL();
	testing1->SetL(defaultFormat,defaultGuard);
	testing1->SenseL(buffer1,bufferGuard1);
	testing1->SenseEffectiveL(buffer2);
	
	test(defaultFormat->IsEqual(*buffer1));
	test(defaultFormat->IsEqual(*buffer2));
	CheckFormatsEqual(defaultGuard,bufferGuard1);
	
	delete testing1;

	delete defaultFormat;
	delete buffer1;
	delete buffer2;

	CheckCParaFormatLayerRestL();
	__UHEAP_MARKEND;
	}


template<class S>
void TestFormat<S>::CheckTCharFormat()
//
// Checks the TCharFormat construction.
//
	{
	__UHEAP_MARK;
// All methods
	INFO_PRINTF1(_L("Constructor"));
	TCharFormat format;
	test(format.iLanguage==0);

	INFO_PRINTF1(_L("Constructor with arguments"));
	TInt height=240;
	TBuf<32> name=_S("arial");
	TCharFormat format1(name,height);
	format1.iFontSpec.iTypeface.SetAttributes(TTypeface::EProportional|TTypeface::ESerif);
	TCharFormat control;
	control.iFontSpec.iHeight=240;
	control.iFontSpec.iTypeface.iName=_S("arial");
	test(format1.IsEqual(control));

	__UHEAP_MARKEND;
	}
	

template<class S>
void TestFormat<S>::CheckTCharFormatMask()
//
// Checks TCharFormatMask construction and methods.
//
	{
	__UHEAP_MARK;
// All methods.
	TInt count=0;
	INFO_PRINTF1(_L("Checking all methods"));
	TCharFormatMask mask;
	mask.SetAttrib(EAttFontHeight);	
	mask.AttribIsSet(EAttFontHeight);
	mask.ClearAttrib(EAttFontHeight);
// Construction.
	INFO_PRINTF1(_L("Construction"));
	TCharFormatMask mask1;
	for (count=EAttCharLanguage;count<ETextFormatAttributeCount;count++)
		{
		test(mask1.AttribIsSet((TTextFormatAttribute)count)==EFalse);
		}
// SetAttrib()
	INFO_PRINTF1(_L("SetAttrib()"));
	for (count=EAttCharLanguage;count<ETextFormatAttributeCount;count++)
		{
		mask1.SetAttrib((TTextFormatAttribute)count);
		}
	for (count=EAttCharLanguage;count<ETextFormatAttributeCount;count++)
		{
		test(mask1.AttribIsSet((TTextFormatAttribute)count));
		}
// ClearAttrib()
	INFO_PRINTF1(_L("ClearAttrib()"));
	for (count=EAttCharLanguage;count<ETextFormatAttributeCount;count++)
		{
		mask1.ClearAttrib((TTextFormatAttribute)count);
		}
	for (count=EAttCharLanguage;count<ETextFormatAttributeCount;count++)
		{
		test(mask1.AttribIsSet((TTextFormatAttribute)count)==EFalse);
		}
// AttribIsSet()
	INFO_PRINTF1(_L("AttribIsSet()"));
	// Already tested in the above.
	INFO_PRINTF1(_L("SetAll()"));
	TCharFormatMask mask2;
	mask2.SetAll();
	for (count=EAttCharLanguage;count<ETextFormatAttributeCount;count++)
		{
		test(mask2.AttribIsSet((TTextFormatAttribute)count));
		}
	INFO_PRINTF1(_L("ClearAll()"));
	mask2.ClearAll();
	for (count=EAttCharLanguage;count<ETextFormatAttributeCount;count++)
		{
		test(mask2.AttribIsSet((TTextFormatAttribute)count)==EFalse);
		}
	__UHEAP_MARKEND;
	}


template<class S>
void TestFormat<S>::CheckAllClassesL()
//
// Check all classes and structs exist.
//
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-LEGACY-T_FMT-0001 TTabStop "));
	CheckTTabStop();

	INFO_PRINTF1(_L("TParaBorder"));
	CheckTParaBorder();

	INFO_PRINTF1(_L("TBullet"));
	CheckTBullet();

	INFO_PRINTF1(_L("CParaFormat"));
	CheckCParaFormatL();

	INFO_PRINTF1(_L("TParaFormatMask"));
	CheckTParaFormatMask();
    
	INFO_PRINTF1(_L("CParaFormatLayer"));
	CheckCParaFormatLayerL();
	
	INFO_PRINTF1(_L("TCharFormat"));
	CheckTCharFormat();

	INFO_PRINTF1(_L("TCharFormatMask"));
	CheckTCharFormatMask();

	INFO_PRINTF1(_L("CCharFormatLayer"));
	CheckCCharFormatLayerL();

	}


LOCAL_C void TestSettingNullTabsL()
// Tests setting null tabs into a para format with tab stops.
//
	{
	INFO_PRINTF1(_L("Setting Null Tabs"));

	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	//
	CRichText* text=CRichText::NewL(paraLayer,charLayer);
	//
	// Set tabs in the first paragraph.
	CParaFormat* paraFormat=CParaFormat::NewLC();
	TTabStop tabA;
		tabA.iTwipsPosition=100;
		tabA.iType=TTabStop::ELeftTab;
	TTabStop tabB;
		tabB.iTwipsPosition=200;
		tabB.iType=TTabStop::ELeftTab;
	TTabStop tabC;
		tabC.iTwipsPosition=300;
		tabC.iType=TTabStop::ERightTab;
	paraFormat->StoreTabL(tabA);
	paraFormat->StoreTabL(tabB);
	paraFormat->StoreTabL(tabC);
	TParaFormatMask paraMask;
	paraMask.SetAttrib(EAttTabStop);
	text->ApplyParaFormatL(paraFormat,paraMask,0,0);
	CleanupStack::PopAndDestroy();  // paraFormat
	//
	// Test that the tabs have been stored.
	CParaFormat* result=CParaFormat::NewLC();
	TParaFormatMask resultMask;
	text->GetParagraphFormatL(result,0);
	test(result->TabCount()==3);
	//
	// Now set zero tabs 
	text->InsertL(0,CEditableText::EParagraphDelimiter);
	CParaFormat* newFormat=CParaFormat::NewLC();
	TParaFormatMask newMask;
	newMask.SetAttrib(EAttTabStop);
	text->ApplyParaFormatL(newFormat,newMask,1,0);
	CleanupStack::PopAndDestroy();  // newFormat
	//
	// Test how many tabs we have in the new paragraph
	CleanupStack::PopAndDestroy();  // result
	result=CParaFormat::NewLC();
	resultMask.ClearAll();
	text->GetParagraphFormatL(result,1);
	test(result->TabCount()==0);
	CleanupStack::PopAndDestroy();  // result
	//
	// Cleanup
	delete text;
	delete paraLayer;
	delete charLayer;
	}


LOCAL_C void TestFormatLayerResetL()
// Test CFormatLayer::Reset();
//
	{
	INFO_PRINTF1(_L("CFormatLayer::Reset()"));

	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	charFormat.iFontSpec.iFontStyle.SetPosture(EPostureItalic);
	charFormat.iFontPresentation.iStrikethrough=EStrikethroughOn;
	charFormat.iFontPresentation.iUnderline=EUnderlineOn;
	charFormatMask.SetAll();
	charLayer->SetL(charFormat,charFormatMask);
	//
	TCharFormat formatOne;
	TCharFormatMask formatOneMask;
	charLayer->Sense(formatOne,formatOneMask);
	test(formatOne.iFontSpec.iFontStyle.StrokeWeight()==EStrokeWeightBold);
	//
	charLayer->Reset();
	//
	TCharFormat result;
	TCharFormatMask resultMask;
	charLayer->Sense(result,resultMask);
	//
	TCharFormat comparator;
	test(result.IsEqual(comparator));
	//
	delete charLayer;
	}

CT_FMT::CT_FMT()
    {
    SetTestStepName(KTestStep_T_FMT);
    pTestStep = this;
    }

TVerdict CT_FMT::doTestStepL()
    {
    SetTestStepResult(EFail);

    CTrapCleanup* cleanup=CTrapCleanup::New();
    INFO_PRINTF1(_L("TFormat Test Code"));
    TestFormat<TText>* fmt=new(ELeave) TestFormat<TText>;
    TRAPD(ret1, fmt->CheckAllClassesL());
    TRAPD(ret2, TestFormatLayerResetL());
    TRAPD(ret3, TestSettingNullTabsL());
    TRAPD(ret4, CheckBulletInheritance());
        
    __UHEAP_MARK;
    delete(fmt);
    fmt=NULL;
    __UHEAP_MARKEND;

    delete cleanup;

    if (ret1 == KErrNone && ret2 == KErrNone && ret3 == KErrNone && ret4 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
