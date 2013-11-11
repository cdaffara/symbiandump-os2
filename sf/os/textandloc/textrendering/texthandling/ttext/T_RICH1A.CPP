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
#include <gdi.h>
#include <conpics.h>
#include <s32stor.h>
#include "../incp/T_PMLPAR.H"
#include "T_RICH1A.h"

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

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D const TInt KTestCleanupStack=0x200;

class MAtomicTest
	{
public:
	/** Test that the object is correctly set at stage aStage, then advance it
	to stage aStage+1 with an atomic (with respect to leaving) operation.

	The test at each stage should test that the previous stage really happened
	and that no part of the new stage has happened.

	RunAtomicTest will test that each of these operations really is atomic.
	@param aStage The stage to test this object is at.
	@return
		ETrue if this object was successfully advanced to the next stage.
		EFalse if there were no more stages to be advanced to.
	*/
	virtual TBool TestAndRunStageL(TInt aStage) = 0;
	/** Tests that the leave reported in aError is suitable as an error code
	from aStage. Panics if not.

	@param aError Code that TestAndRunStageL(aStage) left with.
	@param aStage Stage at which the leave occurred. */
	virtual void TestErrorCondition(TInt aStage, TInt aError) = 0;
	void RunAtomicTest();
	};

/** Runs TestAndRunStage repeatedly with different out-of-memory test
conditions.

It checks that the operations defined at each stage either leave and leave this
object in the same condition, or return successfully and leave the object in
the next stage. */
void MAtomicTest::RunAtomicTest()
	{
	TInt stage = 0;
	TInt failRate = 1;
	TBool more = ETrue;
	while (more)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, failRate);
		TRAPD(err, more = TestAndRunStageL(stage));
		if (err == KErrNone)
			{
			failRate = 1;
			++stage;
			}
		else
			{
			TestErrorCondition(stage, err);
			++failRate;
			}
		}
	__UHEAP_RESET;
	}

template<class S,class T,CEditableText::TDocumentStorage D>
class CRichTest : public CRichText, public MAtomicTest
	{
public:
	static CRichTest* NewL(CParaFormatLayer* aPara,CCharFormatLayer* aChar);

	void DoFlatTests();
	void RegisterMethodsL();
	void InsertWithDelimsL();
	void TestResetL();
	void TestDelete1L();
	void TestDelete2L();
    void TestApplyRemoveCharFormat();
	void TestRemSpecParaFmtL();
	void DoPML();
	void TestForDefectTET5DHEWWL();
	void TestForDefectINC010183L();
	void TestForDefectINC064162L();
	void TestForDefectINC109323L();
	// Tests SetInsertCharFormat and friends for atomicity.
	TBool TestAndRunStageL(TInt aStage);
	void TestErrorCondition(TInt aStage, TInt aError);
protected:
	CRichTest(CParaFormatLayer* aPara,CCharFormatLayer* aChar);
	};

template<class S, class T,CEditableText::TDocumentStorage D>
CRichTest<S,T,D>* CRichTest<S,T,D>::NewL(CParaFormatLayer* aPara,CCharFormatLayer* aChar)
	{
	CRichTest<S,T,D>* tmp=new(ELeave)CRichTest<S,T,D>(aPara,aChar);
	tmp->ConstructL(D,EDefaultTextGranularity,EMultiPara);
	return tmp;
	}


template<class S, class T,CEditableText::TDocumentStorage D>
CRichTest<S,T,D>::CRichTest(CParaFormatLayer* aPara,CCharFormatLayer* aChar)
	:CRichText(aPara,aChar)
	{}


template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::DoFlatTests()
	{
	INFO_PRINTF1(_L("Registering all methods"));
	RegisterMethodsL();
	INFO_PRINTF1(_L("Inserting with embedded paragraph delimiters"));
	InsertWithDelimsL();
	
	}


template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::InsertWithDelimsL()
	{
	INFO_PRINTF1(_L("Inserting into shared para"));

	Reset();
	TBuf<512> testbuf(_L("a"));
	for (TInt ii=0;ii<8;ii++)
		{
		testbuf.Append('a');
		testbuf.Append(CEditableText::EParagraphDelimiter);
		}
	InsertL(0,testbuf);
	test(DocumentLength()==17);
	test(ParagraphCount()==9);

	Reset();
	TBufC<512> bufC;
	InsertL(0,bufC);
		
	Reset();
	TBuf<512> buf(_L("Herewith"));
	InsertL(0,buf);

	InsertL(4,CEditableText::EParagraphDelimiter);
	// THE ABOVE IS TEMPORARY ONLY _ REMOVE IT AS SOON AS YOUVE DONE.
	
	Reset();
//	TBuf<512> buf(_L("Herewith"));
	buf.Append(EParagraphDelimiter);
	InsertL(0,buf);

	buf.Append(_L("Is para one"));
	buf.Append(EParagraphDelimiter);
	InsertL(4,buf);

	buf.Append(_L(" trailing text"));
	InsertL(DocumentLength()-1,buf);
	//////////////////////////////////
	// Pathalogical case (1)
	// Inserting text with delimiters between 2 adjacent pictures.
	INFO_PRINTF1(_L("Inserting text with delimiters between 2 adjacent pictures."));
	//
	Reset();
	CXzePicture* picA=CXzePicture::NewL('Z');
	TPictureHeader hdrA;
	hdrA.iPictureType=KUidXzePictureType;
	hdrA.iPicture=picA;
	//
	CXzePicture* picB=CXzePicture::NewL('X');
	TPictureHeader hdrB;
	hdrB.iPictureType=KUidXzePictureType;
	hdrB.iPicture=picB;
	//
	InsertL(0,hdrA);
	InsertL(DocumentLength(),hdrB);
	buf.SetLength(0);
	buf.Append(_L("some"));
	buf.Append(CEditableText::EParagraphDelimiter);
	buf.Append(_L("trailing text"));
	InsertL(1,buf);
	//
	Reset();  // Destroys all pictures.
	picA=CXzePicture::NewL('Z');
	hdrA.iPictureType=KUidXzePictureType;
	hdrA.iPicture=picA;
	picB=CXzePicture::NewL('X');
	hdrB.iPictureType=KUidXzePictureType;
	hdrB.iPicture=picB;
	InsertL(0,hdrA);
	InsertL(DocumentLength(),hdrB);
	buf.SetLength(5);  // A single para delimiter, with no trailing text.
	InsertL(1,buf);
	////////////////////////////////
	// Pathalogical case (2)
	// Insert text with delimiters after a picture
	INFO_PRINTF1(_L("Insert text with delimiters after a picture"));
	//
	Reset();  // Destroys all pictures.
	picA=CXzePicture::NewL('Z');
	hdrA.iPictureType=KUidXzePictureType;
	hdrA.iPicture=picA;
	InsertL(0,hdrA);
	buf.SetLength(7);
	InsertL(1,buf);

	////////////////////////////////
	// Pathalogical case (3)
	// Insert text with delimiters before a picture
	INFO_PRINTF1(_L("Insert text with delimiters before a picture"));
	//
	Reset();  // Destroys all pictures.
	picA=CXzePicture::NewL('Z');
	hdrA.iPictureType=KUidXzePictureType;
	hdrA.iPicture=picA;
	InsertL(0,hdrA);
	buf.SetLength(7);
	InsertL(0,buf);
	//	
	Reset();
	
	}


template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::RegisterMethodsL()
//
// 1st part of testing, involves all methods being called to ensure that all
// methods exist and run without panicking.
//
	{
	INFO_PRINTF1(_L("CRichText::NewL()"));

	// InsertL()
	INFO_PRINTF1(_L("InsertL()"));
	TPtrC buf(_L("Herewith"));
	InsertL(0,buf);

//	11.3.97 DavidA defect test 
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	TInt lastChar=DocumentLength();
	ApplyCharFormatL(charFormat,charFormatMask,lastChar,1);
//	end of addition.


	TPtrC buf2(_L("Hello"));
	InsertL(0,buf2);

	DeleteL(3,7);

	// SetInsertCharFormatL()
	INFO_PRINTF1(_L("SetInsertCharFormatL()"));
	TCharFormat format; TCharFormatMask mask;
	format.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	mask.SetAttrib(EAttFontStrokeWeight);
	SetInsertCharFormatL(format,mask,3);
	// Test for alloc heaven when doing multiple simultaneous SetInsertCharFormat()s.
	format.iFontSpec.iFontStyle.SetPosture(EPostureItalic);
	mask.SetAttrib(EAttFontPosture);
	SetInsertCharFormatL(format,mask,3);
	//
	TPtrC buf3(_L(" is bold."));
	InsertL(3,buf3);
	CancelInsertCharFormat();

	// Inserting new paragraph
	INFO_PRINTF1(_L("Inserting paragraph delimiter (Insert(TChar))"));
	InsertL(4,CEditableText::EParagraphDelimiter);

	// Delete()
	INFO_PRINTF1(_L("DeleteL()"));
	DeleteL(5,1);

	// Sensing character format over a specified range.
	INFO_PRINTF1(_L("GetCharFormat()"));
	TCharFormat charFormat1; TCharFormatMask undeterminedMask;
	GetCharFormat(charFormat1,undeterminedMask,0,4);

	// Sensing paragraph format over a specified range.
	INFO_PRINTF1(_L("SenseParagraphFormatL()"));
	CParaFormat* pParaFormat=NULL;
	TRAPD(r,pParaFormat=CParaFormat::NewL());
	test(r==KErrNone);
	TParaFormatMask undeterminedParaMask;
	GetParaFormatL(pParaFormat,undeterminedParaMask,0,DocumentLength());
	delete pParaFormat;
	
	// DelSetInsertCharFormatL()
	INFO_PRINTF1(_L("DelSetInsertCharFormatL()"));
	TPtrC buf4(_L("This is para 2."));
	InsertL(0,buf4);
	SetInsertCharFormatL(format,mask,15);
	TPtrC buf5(_L("In italic."));
	InsertL(15,buf5);
	DelSetInsertCharFormatL(4,5);
	
	// CancelInsertCharFormat()
	INFO_PRINTF1(_L("CancelInsertCharFormat()"));
	CancelInsertCharFormat();
	
	// ApplyParaFormatL()
	INFO_PRINTF1(_L("ApplyParaFormatL()"));
	CParaFormat* paraFormat=CParaFormat::NewL(); TParaFormatMask paraMask;
	paraFormat->iHorizontalAlignment=CParaFormat::ECenterAlign;
	paraMask.SetAttrib(EAttAlignment);
	ApplyParaFormatL(paraFormat,paraMask,0,DocumentLength());

	// ApplyCharFormatL()
	INFO_PRINTF1(_L("ApplyCharFormatL()"));
	format.iFontSpec.iFontStyle.SetPosture(EPostureItalic); mask.SetAttrib(EAttFontPosture);
	format.iFontPresentation.iStrikethrough=EStrikethroughOn; mask.SetAttrib(EAttFontStrikethrough);
	ApplyCharFormatL(format,mask,0,DocumentLength());

	// SenseParaFormatL()
	INFO_PRINTF1(_L("SenseParaFormatL()"));
	GetParagraphFormatL(paraFormat,DocumentLength()-1);

	// CountParas()
	INFO_PRINTF1(_L("ParagraphCount()"));
	ParagraphCount();

	// ParagraphStart()
	INFO_PRINTF1(_L("ParagraphStart()"));
	TInt aPos=0;
	ToParagraphStart(aPos);

	// Reset
	Reset();

	delete paraFormat;

	
	}

/**
@SYMTestCaseID 			SYSLIB-ETEXT-UT-3548
@SYMTestCaseDesc  		Testing behaviour of function ApplyCharFormatL() when called at end of document
						that contains plain text. Applied formatting should work on subsequent text
@SYMTestPriority  		High
@SYMTestActions 		1. Insert text1
						2. Switch on bold formatting
						3. Insert text2
						4. Test that first char of text1 is not bold and a character of text 2 is bold. 
@SYMTestExpectedResults	ApplyCharFormatL() applies formatting from end of doc to subsequent character.
@SYMDEF					INC109323
*/
template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestForDefectINC109323L()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-UT-3548 Test ApplyCharFormatL() for bold at end positions of text "));	
	_LIT(KText1, "Testing Bold format");
	_LIT(KText2, "bold");	

	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);
	CRichText* doc = CRichText::NewL(paraLayer,charLayer,D);
	CleanupStack::PushL(doc);
	TCharFormat format1;
	TCharFormat fmtCheck1;
	format1.iFontPresentation.iUnderline = EUnderlineOff;
	// initilizing format attributes
	TCharFormatMask mask1;
	TCharFormatMask mask2;	
	fmtCheck1.iFontPresentation.iUnderline = EUnderlineOff;	
	//Test document containing test string and cursor positioned at end of doc.
	//Turn on bold formatting and subsequent characters are in bold format.
	doc->InsertL(0, KText1);
	format1.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	mask1.SetAttrib(EAttFontStrokeWeight);//Set Bold
	doc->ApplyCharFormatL(format1, mask1, doc->DocumentLength(),0);//Apply Bold	
	doc->InsertL(19, KText2);//These should be in bold
	mask1.ClearAttrib(EAttFontStrokeWeight);//Remove bold formatting
	doc->GetCharFormat(format1, mask1, 0, 1);//Get format of characters at position 0
	doc->GetCharFormat(fmtCheck1, mask2, 22, 1);//Get format of characters at position 22
	test(format1.iFontSpec.iFontStyle.StrokeWeight()!=EStrokeWeightBold);
	test(fmtCheck1.iFontSpec.iFontStyle.StrokeWeight()==EStrokeWeightBold);	
	//tidyup
	CleanupStack::PopAndDestroy(doc);
	CleanupStack::PopAndDestroy(charLayer);
	CleanupStack::PopAndDestroy(paraLayer);	
	__UHEAP_MARKEND;
	}

template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestRemSpecParaFmtL()
//
// Test the RemoveSpecificParaFormatL() method of CRichText.
//
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("RemoveSpecificParaFormatL()"));
	CParaFormat* globalParaFormat=CParaFormat::NewLC();
	TParaFormatMask globalParaMask;
	globalParaFormat->iHorizontalAlignment=CParaFormat::ECenterAlign;
	globalParaMask.SetAttrib(EAttAlignment);
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL(globalParaFormat,globalParaMask);
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	CRichText* doc=NULL;
	TRAPD(ret,doc=CRichText::NewL(paraLayer,charLayer,D));
	if (ret!=KErrNone)
		{
		test(doc==NULL);
		User::Leave(ret);
		}
	TPtrC buf1(_L("Here is paragraph one text."));
	doc->InsertL(0,buf1);
	//
	// Apply specific paragraph format
	CParaFormat* format=CParaFormat::NewLC();
	TParaFormatMask mask;
	format->iHorizontalAlignment=CParaFormat::ERightAlign;
	mask.SetAttrib(EAttAlignment);
	doc->ApplyParaFormatL(format,mask,1,1);
	CParaFormat* sensedFormat=CParaFormat::NewLC();
	doc->GetParagraphFormatL(sensedFormat,0);
	test(sensedFormat->iHorizontalAlignment==CParaFormat::ERightAlign);
	//
	// Remove specific para format and test
	doc->RemoveSpecificParaFormatL(0,1);
	doc->GetParagraphFormatL(sensedFormat,0);
	test(sensedFormat->iHorizontalAlignment==CParaFormat::ECenterAlign);
	//
	CleanupStack::PopAndDestroy(3); // the 2 para formats.
	delete doc;
	delete paraLayer;
	delete charLayer;

	__UHEAP_MARKEND;
	}


template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestDelete1L()
//
// Test the DeleteL() method of CRichText.
//
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("DeleteL()"));
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	CRichText* doc=NULL;
	TRAPD(ret,doc=CRichText::NewL(paraLayer,charLayer,D));
	if (ret!=KErrNone)
		{
		test(doc==NULL);
		User::Leave(ret);
		}
	TPtrC buf1(_L("Here is paragraph one text."));
	TPtrC buf2(_L("Here is paragraph one text."));
	doc->InsertL(0,buf1);
	doc->InsertL(doc->DocumentLength(),EParagraphDelimiter);
	doc->InsertL(doc->DocumentLength(),buf2);
	//
	// Apply formatting
	TCharFormat applyFormat;
	TCharFormatMask applyMask;
	applyFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	applyMask.SetAttrib(EAttFontStrokeWeight);
	doc->ApplyCharFormatL(applyFormat,applyMask,8,28);
	//
	// Now try the delete - should end up with a single shared paragraph!!!
	doc->DeleteL(8,28);
	TInt documentLength=doc->DocumentLength();
	test(documentLength==27);
	//
	delete doc;
	delete paraLayer;
	delete charLayer;

	__UHEAP_MARKEND;
	}


template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestDelete2L()
//
// Test the DeleteL() method of CRichText.
// Deleting the paragraph delimiter between 2 paras of constant character format, but of varying
// paragraph formats.
//
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("DeleteL()"));
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	CRichText* doc=NULL;
	TRAPD(ret,doc=CRichText::NewL(paraLayer,charLayer,D));
	if (ret!=KErrNone)
		{
		test(doc==NULL);
		User::Leave(ret);
		}
	TPtrC buf1(_L("A"));
	TPtrC buf2(_L("B"));
	TPtrC buf3(_L("C"));
	doc->InsertL(0,buf1);
	doc->InsertL(doc->DocumentLength(),EParagraphDelimiter);
	doc->InsertL(doc->DocumentLength(),buf2);
	doc->InsertL(doc->DocumentLength(),EParagraphDelimiter);
	doc->InsertL(doc->DocumentLength(),buf3);
	//
	// Apply formatting
	CParaFormat* applyFormat=CParaFormat::NewLC();
	TParaFormatMask applyMask;
	applyFormat->iHorizontalAlignment=CParaFormat::ERightAlign;
	applyMask.SetAttrib(EAttAlignment);
	//
	// Make 1st & 3rd para the same para format, different to para 2.
	
	doc->ApplyParaFormatL(applyFormat,applyMask,1,1);
	doc->ApplyParaFormatL(applyFormat,applyMask,5,1);
	CleanupStack::PopAndDestroy();  // para format
	//
	// Now try the delete - should end up with a single shared paragraph!!!
	/*TBool parasMerged=*/doc->DeleteL(3,1);  // delete para 2 delimiter
	TInt documentLength=doc->DocumentLength();
	test(documentLength==4);
	//
	delete doc;
	delete paraLayer;
	delete charLayer;

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID             SYSLIB-ETEXT-UT-3431
@SYMTestCaseDesc          Testing behaviour of functions ApplyCharFormatL() and RemoveSpecificFormatL();
    formatting of text including end of document character
@SYMTestPriority          High
@SYMTestActions         1. format text and compare attribute values for equality
                        2. testing for fix of INC097216 whether EOD also formatted
                        3. remove formatting of text and compare attribute values for equality
                        4. testing for fix of DEF104149 whether EOD also has formatting removed
@SYMTestExpectedResults    ApplyCharFormatL() and RemoveSpecificFormatL() apply and remove formatting from
    specified range of text including end of document character
@SYMDEF                    DEF104149
*/
template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestApplyRemoveCharFormat()
    {
    __UHEAP_MARK;
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-UT-3431 Test ApplyCharFormatL() & RemoveSpecificCharFormat() "));	
    CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
    CleanupStack::PushL(paraLayer);
    CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
    CleanupStack::PushL(charLayer);
    CRichText* doc = CRichText::NewL(paraLayer,charLayer,D);
    CleanupStack::PushL(doc);
    _LIT(KText1, "Hello there!");
    doc->InsertL(0, KText1);
    test(doc->DocumentLength()==12);
   
    // initilizing format (ie.underline) values
    TCharFormat formatCheck;
    formatCheck.iFontPresentation.iUnderline = EUnderlineOff;
    TCharFormat fmatCheck;
    formatCheck.iFontPresentation.iUnderline = EUnderlineOff;
    TCharFormat format;
    format.iFontPresentation.iUnderline = EUnderlineOn;
    TCharFormat formatNone;
    formatCheck.iFontPresentation.iUnderline = EUnderlineOff;

    // initilizing format attributes
    TCharFormatMask maskCheck;   
    TCharFormatMask mask;
    mask.SetAttrib(EAttFontUnderline);
   
    // underline KText1 and compare its attribute values with EUnderlineOn for equality
    doc->ApplyCharFormatL(format, mask, 0, 12);   
    doc->GetCharFormat(formatCheck, maskCheck, 0, 12);
    // testing for fix of INC097216 whether EOD also formatted
    doc->GetCharFormat(fmatCheck, maskCheck, 12, 1);
    test(format.IsEqual(formatCheck));
    test(format.IsEqual(fmatCheck));

    // remove underlining of KText1 and compare its attribute values with EUnderlineOff for equality
    doc->RemoveSpecificCharFormatL(0, 12);
    doc->GetCharFormat(formatCheck, maskCheck, 0, 12);
    // testing for fix of INC104149 whether EOD also has formatting removed
    doc->GetCharFormat(fmatCheck, maskCheck, 12, 1);
    test(formatCheck.IsEqual(formatNone));
    test(fmatCheck.IsEqual(formatNone));
   
    CleanupStack::PopAndDestroy(doc);
    CleanupStack::PopAndDestroy(charLayer);
    CleanupStack::PopAndDestroy(paraLayer);   
    __UHEAP_MARKEND;
    }


template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestResetL()
//
// Test the Reset() method of CRichText.
//
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Reset()"));
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	CRichText* doc=NULL;
	TRAPD(ret,doc=CRichText::NewL(paraLayer,charLayer,D));
	if (ret!=KErrNone)
		{
		test(doc==NULL);
		User::Leave(ret);
		}
	TPtrC buf1(_L("Here is paragraph one text."));
	TPtrC buf2(_L("And here is that of the second paragraph"));
	doc->InsertL(0,buf1);
	doc->InsertL(doc->DocumentLength(),EParagraphDelimiter);
	doc->InsertL(doc->DocumentLength(),buf2);
	//
	doc->Reset();
	test(doc->ParagraphCount()==1);
	doc->Reset();
	test(doc->ParagraphCount()==1);
	doc->Reset();
	test(doc->ParagraphCount()==1);
	delete doc;
	delete paraLayer;
	delete charLayer;

	__UHEAP_MARKEND;
	}
	

template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::DoPML()
//
// Use PML source translator to produce a RichText component.
//
	{
 	__UHEAP_MARK;
	INFO_PRINTF1(_L("Checking PML sourced RichText Component"));
 	// set filename
	TFileName theFileName=_L("z:\\test\\app-framework\\etext\\t_rich1a.pml");
	// Parse PML
	CParser* myParser=NULL;
	CRichText* richTextDoc=NULL;
	TRAPD(ret, myParser=CParser::NewL());
	CleanupStack::PushL(myParser);
	TRAP(ret, richTextDoc=myParser->ParseL(theFileName));
	CleanupStack::PushL(richTextDoc);
	CParaFormatLayer* pl = const_cast<CParaFormatLayer*>(
		richTextDoc->GlobalParaFormatLayer());
	CleanupStack::PushL(pl);
	CCharFormatLayer* cl = const_cast<CCharFormatLayer*>(
		richTextDoc->GlobalCharFormatLayer());
	CleanupStack::PushL(cl);
	
	TInt paraCount=richTextDoc->ParagraphCount();
	test(paraCount==2);
	TBool hasMarkupData=richTextDoc->HasMarkupData();
	test(hasMarkupData==EFalse);

	// Testing overloaded senseChars - looking at the indeterminate mask.
//	TCharFormat charFormat;	TCharFormatMask charFormatMask;
//	richTextDoc->GetCharFormat(0,17,charFormat,charFormatMask);
//	test(charFormatMask==0);

//	richTextDoc->GetCharFormat(17,10,charFormat,charFormatMask);

	CParaFormat* paraFormat = 0;
	paraFormat=CParaFormat::NewLC();
	TParaFormatMask paraFormatMask;
	richTextDoc->GetParaFormatL(paraFormat,paraFormatMask,0,2);
	richTextDoc->GetParaFormatL(paraFormat,paraFormatMask,17,3);  // All para 2 and just para1

	CleanupStack::PopAndDestroy(paraFormat);
	CleanupStack::PopAndDestroy(cl);
	CleanupStack::PopAndDestroy(pl);
	CleanupStack::PopAndDestroy(richTextDoc);
	CleanupStack::PopAndDestroy(myParser);
	__UHEAP_MARKEND;
	}

template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestForDefectTET5DHEWWL()
//
// Test the CRichTextIndex::InsertL() for defect TET-5DHEWW which 
// should not be present from 15/10/02.
//
	{
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Testing for the presence of defect TET-5DHEWW"));

	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();

	CRichText* doc = NULL;
	TRAPD(ret, doc = CRichText::NewL(paraLayer,charLayer,D));
	if (ret!=KErrNone)
		{
		test(doc==NULL);
		User::Leave(ret);
		}

	TPtrC buf1(_L("sometext "));
	doc->InsertL(0, buf1);

	TCharFormat charFormat12(_L("Times New Roman"), 240);
	TCharFormatMask charMask12;
	charMask12.SetAttrib(EAttFontHeight);
	doc->SetInsertCharFormatL(charFormat12, charMask12, doc->DocumentLength());

	TPtrC buf2(_L("sometext "));
	doc->InsertL(doc->DocumentLength(), buf2);

	doc->SetInsertCharFormatL(charFormat12, charMask12, doc->DocumentLength());
	doc->InsertL(doc->DocumentLength(), EParagraphDelimiter);

	// Defect present in CRichTextIndex if test executable crashes/panics on the 
	// previous line on a debug build. Should this test method return normally then 
	// the EText library used does not contain the defect.

	TInt docLen = doc->DocumentLength();
	test(docLen==19);

	INFO_PRINTF1(_L("RTEST:                        Test PASSED - defect not present!\n"));

	delete doc;
	delete paraLayer;
	delete charLayer;

	__UHEAP_MARKEND;
	}

// CRichTextIndex::GetParaFormatL doesn't set TParaFormatMask correctly
template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestForDefectINC010183L()
	{
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Testing for the presence of defect INC010183"));

	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);

	CRichText* doc = CRichText::NewL(paraLayer,charLayer,D);
	CleanupStack::PushL(doc);

	TPtrC buf1(_L("para1\x2029para2\x2029para3"));
	doc->InsertL(0, buf1);

	CParaFormat* pFormat = CParaFormat::NewLC();
	pFormat->iBullet = new(ELeave) TBullet;
	pFormat->iBullet->iHeightInTwips = 1;
	TParaFormatMask pMask;
	pMask.SetAttrib(EAttBullet);

	doc->ApplyParaFormatL(pFormat, pMask, 8, 1);
	doc->GetParaFormatL(pFormat, pMask, 0, 14);

	// test that the bullet's "varies" flag is set.
	test(pMask.AttribIsSet(EAttBullet));

	doc->GetParaFormatL(pFormat, pMask, 7, 6);

	// test that the bullet's "varies" flag is set.
	test(pMask.AttribIsSet(EAttBullet));

	CleanupStack::PopAndDestroy(pFormat);
	CleanupStack::PopAndDestroy(doc);
	CleanupStack::PopAndDestroy(charLayer);
	CleanupStack::PopAndDestroy(paraLayer);

	__UHEAP_MARKEND;
	}


// CRichTextIndex::InsertL, Insertion of zero-length text should not cancel the pending new TCharFormat.
// This test sets the Strikethrough option on and applies it to a zero length section of text before checking
// the format has been applied correctly. Then it inputs 3 characters and checks to see if the format of these
// characters also has the character format applied to them.  
template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestForDefectINC064162L()
	{
	__UHEAP_MARK;
	
	INFO_PRINTF1(_L("INC064162 - Testing insertion of zero-length text doesn't cancel the pending TCharFormat"));
	
	TInt length = 3;	
	TPtrC bufPtrLetterNone(_L(""));
	TPtrC bufPtrLettersABC(_L("ABC"));
	
	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);

	CRichText* doc = CRichText::NewL(paraLayer,charLayer,D);
	CleanupStack::PushL(doc);

	TCharFormat cFormatCheck;
	cFormatCheck.iFontPresentation.iStrikethrough = EStrikethroughOff;
	
	TCharFormat cFormat;
	cFormat.iFontPresentation.iStrikethrough = EStrikethroughOn;

	TCharFormatMask cMaskCheck;
	
	TCharFormatMask cMask;
	cMask.SetAttrib(EAttFontStrikethrough);
	
	// Applying the strikethrough format on a zero length section of text.
	doc->ApplyCharFormatL(cFormat,cMask,0,0);	

	doc->InsertL(0, bufPtrLetterNone);
	
	doc->GetCharFormat(cFormatCheck, cMaskCheck,0,0);

	// testing the format of the empty buf
	test(cFormat.IsEqual(cFormatCheck));

	doc->InsertL(0, bufPtrLettersABC);	
	doc->GetCharFormat(cFormatCheck, cMaskCheck,0,length);
	
	// testing the format of the 3 characters
	test(cFormat.IsEqual(cFormatCheck));
 
	CleanupStack::PopAndDestroy(doc);
	CleanupStack::PopAndDestroy(charLayer);
	CleanupStack::PopAndDestroy(paraLayer);

	__UHEAP_MARKEND;
	}



template<class S, class T,CEditableText::TDocumentStorage D>
TBool CRichTest<S,T,D>::TestAndRunStageL(TInt aStage)
	{
	_LIT(KSomeText, "Text\x2029par ");
	_LIT(KSomeTextPlus3, "Text\x2029par \x2029z ");
	TCharFormat format;
	TCharFormatMask mask;
	TCharFormatMask varies;
	switch (aStage)
		{
	case 0:
		Reset();
		return ETrue;
	case 1:
		test(DocumentLength() == 0);
		InsertL(0, KSomeText);
		return ETrue;
	case 2:
			{
			TBuf<100> buf;
			test(DocumentLength() == KSomeText().Length());
			Extract(buf, 0, KSomeText().Length());
			test(0 == buf.Compare(KSomeText));
			TCharFormat format0;
			GetCharFormat(format, varies, 0, KSomeText().Length());
			test(varies.IsNull());		// format should not vary
			test(format.IsEqual(format0));
			}
		// Set an insert char format for font height 100
		mask.SetAttrib(EAttFontHeight);
		format.iFontSpec.iHeight = 100;
		SetInsertCharFormatL(format, mask, 9);
		return ETrue;
	case 3:
		test(DocumentLength() == KSomeText().Length());
		// Insert a carriage return and cancel the insert char format.
		// This stands in place of moving the cursor away from the
		// insertion point and back again.
		InsertL(9, 0x2029);
		return ETrue;
	case 4:
		test(DocumentLength() == KSomeText().Length() + 1);
		CancelInsertCharFormat();
		return ETrue;
	case 5:
		test(DocumentLength() == KSomeText().Length() + 1);
		GetCharFormat(format, varies, 0, 9);
			{
			TCharFormat format0;
			test(format.IsEqual(format0));
			test(varies.IsNull());
			}
		GetCharFormat(format, varies, 9, 1);
		test(varies.IsNull());
		test(format.iFontSpec.iHeight == 100);
		// Insert a character after the new carriage return. It should
		// have height 100. This is what the defect INC038479 is
		// complaining about. In fact it is a long-standing problem,
		// long assumed to be too difficult to solve in a reasonable
		// time.
		InsertL(10, 'z');
		return ETrue;
	case 6:
		test(DocumentLength() == KSomeText().Length() + 2);
		GetCharFormat(format, varies, 9, 2);
		test(varies.IsNull());
		test(format.iFontSpec.iHeight == 100);
		// Insert a space. This prepares us for checking for a regression
		// that was introduced then fixed in the fix for INC038479.
		InsertL(11, ' ');
		return ETrue;
	case 7:
			{
			TBuf<100> buf;
			test(DocumentLength() == KSomeTextPlus3().Length());
			Extract(buf, 0, KSomeTextPlus3().Length());
			test(0 == buf.Compare(KSomeTextPlus3));
			TCharFormat format0;
			GetCharFormat(format, varies, 0, KSomeText().Length());
			test(varies.IsNull());		// format should not vary across the original bit
			test(format.IsEqual(format0));
			GetCharFormat(format, varies, KSomeText().Length(), 3);
			test(format.iFontSpec.iHeight == 100);
			test(varies.IsNull());
			}
		// Set a new insert character format for bold
		mask.SetAttrib(EAttFontStrokeWeight);
		format.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
		SetInsertCharFormatL(format, mask, 9);
		return ETrue;
	case 8:
		// and unset the insert character format again
		mask.SetAttrib(EAttFontStrokeWeight);
		format.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
		SetInsertCharFormatL(format, mask, 9);
		return ETrue;
	case 9:
		test(DocumentLength() == KSomeTextPlus3().Length());
		// Add a carriage return.
		InsertL(12, 0x2029);
		return ETrue;
	case 10:
		test(DocumentLength() == KSomeTextPlus3().Length() + 1);
		GetCharFormat(format, varies, KSomeTextPlus3().Length(), 1);
		test(varies.IsNull());
		test(format.iFontSpec.iFontStyle.StrokeWeight() == EStrokeWeightNormal);
		return EFalse;
	default:
		return ETrue;
		}
	}

template<class S, class T,CEditableText::TDocumentStorage D>
void CRichTest<S,T,D>::TestErrorCondition(TInt aStage, TInt aError)
	{
	// No leaves possible except NoMemory.
	test(aError == KErrNoMemory);
	// Stage 4 should not leave at all.
	test(aStage != 4);
	}


template<class S, class T, CEditableText::TDocumentStorage D>
void TestClassesL()
	{
	__UHEAP_MARK;

	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);
	CRichTest<S, T, D>* doc = CRichTest<S, T, D>::NewL(paraLayer, charLayer);
	CleanupStack::PushL(doc);

	doc->DoFlatTests();
	doc->TestResetL();
	doc->TestDelete1L();
	doc->TestDelete2L();
	doc->TestApplyRemoveCharFormat();
	doc->TestRemSpecParaFmtL();
	doc->DoPML();
	doc->TestForDefectTET5DHEWWL();
	doc->TestForDefectINC010183L();
	doc->TestForDefectINC064162L();
	doc->TestForDefectINC109323L();
	INFO_PRINTF1(_L("INC038479 - Email editor: font settings made at doc end can be lost and returned to default"));
	// Also tests SetInsetCharFormat more thoroughly.
	doc->RunAtomicTest();

	CleanupStack::PopAndDestroy(doc);
	CleanupStack::PopAndDestroy(charLayer);
	CleanupStack::PopAndDestroy(paraLayer);

	__UHEAP_MARKEND;
	}

void DoTestsL()
	{
	INFO_PRINTF1(_L("CRichText - Flat"));
	TestClassesL<TText,TPtrC,CEditableText::EFlatStorage>();
	INFO_PRINTF1(_L("CRichText - Segmented"));
	TestClassesL<TText,TPtrC,CEditableText::ESegmentedStorage>();
	
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

CT_RICH1A::CT_RICH1A()
    {
    SetTestStepName(KTestStep_T_RICH1A);
    pTestStep = this;
    }

TVerdict CT_RICH1A::doTestStepL()
    {
    SetTestStepResult(EFail);

    setupCleanup();
    INFO_PRINTF1(_L("CRichText Document"));
    __UHEAP_MARK;
    
    TRAPD(ret,DoTestsL());
    
    __UHEAP_MARKEND;
    delete TheTrapCleanup;
    
    if (ret == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
