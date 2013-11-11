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
#include "T_CPLAIN.h"

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

const TBool KPictureIsDelimiter=EFalse;
const TBool KPunctuationIsDelimiter=ETrue;

template<class S,class T,CEditableText::TDocumentStorage D>
class TestCPlainText : public CPlainText
	{
public:
	static TestCPlainText* NewL();
	void TestL();  // Controls testing of the Flat storage usage.
	void TestSegmentedL();  // Controls testing of the segmented storage usage.
	void CheckDocLength(TInt aLength);
	void CheckDocLength(CPlainText* aDocument,TInt aLength);
	void CheckContent(const TDesC& aOriginal,TPtrC& aControl);
	void CheckContent(const TDesC& aOriginal,TDes& aControl);
	void CheckContent(CPlainText* aDocument,TPtrC& aControl);
	void CheckParagraphStart(TInt aCalculated,TInt aExpected);
	void CheckCharsSkipped(TInt aCalculated,TInt aExpected);
	void DoParagraphStart(TInt aStartPos,TInt aLength,CPlainText* aDocument);
	void InsertL(TInt aPos,const TDesC& aBuf);
	void CPlainTest0L();  // Checks all methods are present.
	void CPlainTest1L();  // Test construction under different memory conditions.
	void CPlainTest2L();  // Read method tests
	void CPlainTest3L();
	void CPlainTest4L();  
	void CPlainTest5L();
	void CPlainTest5aL();  // Added to test the word info method.
	void CPlainTest5bL();  // Added to test the CharPosOfParagraph method.
	void CPlainTest5cL();  // Added to test the ParagraphNumberForPos method.
	void DoCharPosOfParagraph(CPlainText* aText,TInt aParaN,TInt aPosCheck,TInt aLengthCheck);
	void DoParagraphNumberForPos(CPlainText* aText,TInt aPos,TInt aParaCheck,TInt aPosCheck);
	void CPlainTest6L();
	void CPlainTest7L();
	void CPlainTest8L();
	void DefectsL();
	void INC070807L();
private:
	TestCPlainText();
	TestCPlainText(const TestCPlainText& aTestCPlainText);
	TestCPlainText& operator=(const TestCPlainText& aTestCPlainText);
private:
	static TPtrC iDefaultDoc;
	static TPtrC iInBuf;
	static TPtrC iOutBuf;
	static TPtrC iOutBufComp;
	static TPtrC iInBuf2;
	static TPtrC iOutBuf2;
	static TPtrC iDummy;
	static TPtrC iComp1;
	static TPtrC iComp2;
	static TPtrC iComp3;
	static TPtrC iComp4;
	static TPtrC iComp5;
	static TPtrC iInsertBuf;
	static TPtrC iTestBuf1;
	static TPtrC iTestBuf2;
	static TPtrC iTestBuf2a;	
	static TPtrC iTestBuf3;
	};

template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iDefaultDoc(_L("A default constructor has been used to build this text in a new document that is newly created."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iComp1(_L("A default constructor has been used to build this text in a new document that is newly created."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iComp2(_L("this text in a new document that is newly created."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iComp3(_L("d."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iComp4(_L("."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iComp5(_L(""));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iDummy(_L("a"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iInBuf(_L("AAAAAAAAAA"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iOutBuf(_L(""));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iInBuf2(_L("OOOO"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iOutBufComp(_L("AAAAAAAAAA"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iOutBuf2(_L(""));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iTestBuf1(_L("abcdefINLINE_TEXTghijklmnopqrstuvwxyz"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iInsertBuf(_L("abcdefghijklmnopqrstuvwxyz"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iTestBuf2(_L("klmnopqrstuvwxyz"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iTestBuf2a(_L("klmnopqrst"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>::iTestBuf3(_L("efghi"));

template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iDefaultDoc(_L("A default constructor has been used to build this text in a new document that is newly created."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iComp1(_L("A default constructor has been used to build this text in a new document that is newly created."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iComp2(_L("this text in a new document that is newly created."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iComp3(_L("d."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iComp4(_L("."));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iComp5(_L(""));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iDummy(_L("a"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iInBuf(_L("AAAAAAAAAA"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iOutBuf(_L(""));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iInBuf2(_L("OOOO"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iOutBufComp(_L("AAAAAAAAAA"));
template<> TPtrC TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>::iOutBuf2(_L(""));

#if ((defined (__GCC32__)) && (!defined (__X86GCC__)) )
template class TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage>;
template class TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage>;
#endif


template<class S, class T,CEditableText::TDocumentStorage D>
TestCPlainText<S,T,D>* TestCPlainText<S,T,D>::NewL()
//
//
//
	{
	TestCPlainText<S,T,D>* tmp=new(ELeave)TestCPlainText<S,T,D>;
	tmp->ConstructL(D);
	return tmp;
	}


template<class S, class T,CEditableText::TDocumentStorage D>
TestCPlainText<S,T,D>::TestCPlainText()
//
// Default constructor.
//
	{}


template<class S, class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CheckDocLength(TInt aLength)
//
// Test the current document length == aLength
//
	{
	TInt docLength=DocumentLength();
	test(docLength==aLength);
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CheckDocLength(CPlainText* aDocument,TInt aLength)
//
// Test that the length of document aDocument==aLength
//
	{
	test(aDocument->DocumentLength()==aLength);	
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CheckContent(const TDesC& aOriginal,TPtrC& aControl)
//
// Check that document content is what you expect
//
	{
	test(aOriginal.Length()==aControl.Length());
	for (TInt offset=0;offset<aOriginal.Length();offset++)
		{
		test(aOriginal[offset]==aControl[offset]);
		}
	}
 

template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CheckContent(const TDesC& aOriginal,TDes& aControl)
//
// Check that document content is what you expect
//
	{
	test(aOriginal.Length()==aControl.Length());
	for (TInt offset=0;offset<aOriginal.Length();offset++)
		{
		test(*aOriginal.Ptr()==*aControl.Ptr());
		}
	}



template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CheckContent(CPlainText* aDocument,TPtrC& aControl)
//
// Check that document content is what you expect
//
	{
	test(aDocument->DocumentLength()==aControl.Length());
	TPtrC view(iDummy);
	for (TInt offset=0;offset<aDocument->DocumentLength();offset++)
		{
		view.Set(aDocument->Read(offset).Ptr(),aDocument->Read(offset).Length());
		test(*view.Ptr()==aControl[offset]);
		}
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CheckParagraphStart(TInt aCalculated,TInt aExpected)
//
// Checks the value returned from ParagraphStart(aPos) is what
// it is expected to be.
//
	{
	test(aCalculated==aExpected);
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CheckCharsSkipped(TInt aCalculated,TInt aExpected)
//
// Check the number of characters skipped following a  ParagraphStart()
// is as expected.
//
	{
	test(aCalculated==aExpected);
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::InsertL(TInt aPos,const TDesC& aBuf)
//
	{CPlainText::InsertL(aPos,aBuf);}



template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::DoParagraphStart(TInt aStartPos,TInt aLength,CPlainText* aDocument)
//
// Parametric testing of the ParagraphStart method of the
// document class hierarchy.
//
	{
	TInt tempPos=0;
	TInt charsSkipped=0;
	for (TInt charPos=aStartPos;charPos<aStartPos+aLength;charPos++)
		{// Check Paragraph
		tempPos=charPos;
		charsSkipped=aDocument->ToParagraphStart(charPos);
		// charPos is updated to paragraph start character position.
		CheckParagraphStart(charPos,aStartPos);
		charPos=tempPos;  // Reset charPos following it's update.
		CheckCharsSkipped(charsSkipped,charPos-aStartPos);
		}
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest0L()
//
// Tests that all published methods exist.
//
	{
	__UHEAP_MARK;
	CPlainText* doc=CPlainText::NewL(D);
	doc->DocumentLength();
	doc->InsertL(0,iDefaultDoc);
	TChar character('X');
	doc->InsertL(0,character);
	TInt pos=3;
	doc->ToParagraphStart(pos);
	doc->DeleteL(3,1);
	doc->Read(2);
	doc->Read(2,2);
	TBuf<128> buf;
	doc->Extract(buf);
	TInt startPos,length;
	doc->GetWordInfo(13,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	doc->WordCount();
	doc->ParagraphCount();
	doc->Reset();
	delete doc;
	__UHEAP_MARKEND;
	}


template<class S, class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest1L()
//
// PlainText document construction.
//
	{
	__UHEAP_MARK;
	test(iByteStore!=NULL);
	CheckDocLength(0);
	
   	INFO_PRINTF1(_L("NewL()"));
  	CPlainText* document=CPlainText::NewL(D);
	document->InsertL(0,iDefaultDoc);
	test(document->DocumentLength()==95);
	delete document;
#ifdef _DEBUG
  	INFO_PRINTF1(_L("NewL() failing on OOM"));
	TInt flag=0;
	__UHEAP_FAILNEXT(1);
	TRAPD(ret, (void)CPlainText::NewL(D));
	if (ret!=KErrNone)
		{
		flag++;
		}
	// seems __UHEAP_FAILNEXT does not work well in platsim.
	// below test does not pass in platsim.
//	test(flag!=0);
#endif

	__UHEAP_MARKEND;
	}


template<class S, class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest2L()
//
// Tests the Read method.
// Does alter the state of the object.
//
	{
	__UHEAP_MARK;
	CPlainText* document=CPlainText::NewL(D);
	document->InsertL(0,iDefaultDoc);
	
	INFO_PRINTF1(_L("sense from start"));
	/*TPtrC dummy=*/document->Read(0);
	CheckDocLength(document,95);
	CheckContent(document,iComp1);

	INFO_PRINTF1(_L("sense from n"));
	TPtrC dummy2=document->Read(45);
	//iDummy.Set(document->Read(45).Ptr(),document->Read(45).Length());
	CPlainText* doc2=CPlainText::NewL(D);
	doc2->InsertL(0,dummy2);
	doc2->DeleteL(50,1);  // We need a debug copy constructor here.  The origianl
		// document has an oed character, and when we construct a new doucment
		// based on the original, we get our own eod character plus the one that is
		// part of the original, = 2 eod characters.
		// This breaks the following tests.  So in the short term I have elected to delete this
		// trailing, unwanted eod character.
	CheckDocLength(doc2,50);
	CheckContent(doc2,iComp2);
	delete doc2;
	
	INFO_PRINTF1(_L("sense from last-1"));
	iDummy.Set(document->Read(93).Ptr(),document->Read(93).Length());
	CPlainText* doc3=CPlainText::NewL(D);
	doc3->InsertL(0,iDummy);
	doc3->DeleteL(2,1);
	CheckDocLength(doc3,2);
	CheckContent(doc3,iComp3);
	delete doc3;
	
	INFO_PRINTF1(_L("sense from last"));
	iDummy.Set(document->Read(94).Ptr(),document->Read(94).Length());
	CPlainText* doc4=CPlainText::NewL(D);
	doc4->InsertL(0,iDummy);
	doc4->DeleteL(1,1);
	CheckDocLength(doc4,1);
	CheckContent(doc4,iComp4);
	delete doc4;

	INFO_PRINTF1(_L("sense from last+1"));
	iDummy.Set(document->Read(95).Ptr(),document->Read(95).Length());
	CPlainText* doc5=CPlainText::NewL(D);
	doc5->InsertL(0,iDummy);
	doc5->DeleteL(0,1);
	CheckDocLength(doc5,0);
	CheckContent(doc5,iComp5);
	delete doc5;
	
	delete document;
	__UHEAP_MARKEND;
	}


template<class S, class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest3L()
//
// Tests correct execution of insert and delete
// on default document content.  The document
// content should be left unchanged.
//
	{
	__UHEAP_MARK;
	// Create document with 'AAAAAAAAAA'
	TChar character('Y');
	CPlainText* document=CPlainText::NewL(D);
	document->InsertL(0,iInBuf);
	iOutBuf.Set(document->Read(0));
	CheckDocLength(document,10);
	CheckContent(document,iOutBufComp);

	INFO_PRINTF1(_L("Inverting at start"));
	document->InsertL(0,iInBuf2);
	document->InsertL(0,character);
	document->DeleteL(0,1);
	CheckDocLength(document,14);
	document->DeleteL(0,4);
 	iOutBuf2.Set(document->Read(0));
	CheckDocLength(document,10);
	CheckContent(document,iOutBufComp);

	INFO_PRINTF1(_L("Inverting in middle"));
	document->InsertL(4,iInBuf2);
	document->InsertL(5,character);
	document->DeleteL(5,1);
	CheckDocLength(document,14);
	document->DeleteL(4,4);
	iOutBuf2.Set(document->Read(0));
	CheckDocLength(document,10);
	CheckContent(document,iOutBufComp);

	INFO_PRINTF1(_L("Inverting at end"));	
	document->InsertL(10,iInBuf2);
	document->InsertL(11,character);
	document->DeleteL(11,1);
	CheckDocLength(document,14);
	document->DeleteL(10,4);
	iOutBuf2.Set(document->Read(0));
	CheckDocLength(document,10);
	CheckContent(document,iOutBufComp);

	document->DeleteL(0,10);
	CheckDocLength(0);
	delete document;

	__UHEAP_MARKEND;
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest4L()
//
// Test scan method combinations
//
	{
	// Create document content for test.
	TBuf<128> content;
	content.Append(_L("Paragraph one. Complete with sentence and word breaks."));
	content.Append(EParagraphDelimiter);
	// Para 1 is 55 characters incl. delimiter.
	content.Append(_L("This is paragraph two."));
	content.Append(EParagraphDelimiter);
	// Para 2 is 23 characters incl. delimiter.
	content.Append(_L("This is paragraph 3"));
	content.Append(EParagraphDelimiter);
	// Para 3 is 20 characters incl. delimiter.
	content.Append(EParagraphDelimiter);
	// Para 4 is 1 character incl. delimiter.
	content.Append(EParagraphDelimiter);
	content.Append(EParagraphDelimiter);
	// Create document.
	CPlainText* document=CPlainText::NewL(D);
	TPtrC body(content);
	document->InsertL(0,body);
	// Now do the tests.
	INFO_PRINTF1(_L("Paragraph 1"));
	DoParagraphStart(0,55,document);  // Paragraph 1
	INFO_PRINTF1(_L("Paragraph 2"));
	DoParagraphStart(55,23,document);  // Paragraph 2
	INFO_PRINTF1(_L("Paragraph 3"));
	DoParagraphStart(78,20,document);  // Paragraph 3
	INFO_PRINTF1(_L("Paragraph 4->2 consecutive delimiters, posshould not change"));
	DoParagraphStart(99,1,document);  // Paragraph 6

	delete document;
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest5L()
//
// Tests the extract method.
//
	{
	__UHEAP_MARK;
	CPlainText* doc=CPlainText::NewL(D);
	doc->InsertL(0,iDefaultDoc);

	INFO_PRINTF1(_L("Extract(buf)"));
	TBuf<128> buf;
	doc->Extract(buf);
	CheckContent(iDefaultDoc,buf);

	INFO_PRINTF1(_L("Extract(buf,pos)"));
	TInt pos=45;
	doc->Extract(buf,pos);
	CheckContent(iComp2,buf);

	INFO_PRINTF1(_L("Extract(buf) from multiple segments"));
	TBuf<256> bigBuf(_L("abcdefghijklmnopqrstuvwxyzABCEDFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCEDFGHIJKLMNOPQRSTUVWXYZ"));
	CPlainText* segDoc=CPlainText::NewL(ESegmentedStorage);
	segDoc->InsertL(0,bigBuf);
	segDoc->Extract(buf);
	CheckContent(bigBuf,buf);
	delete segDoc;

	delete doc;
	__UHEAP_MARKEND;
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest5aL()
//
// Tests the WordInfo() method.
//
	{
	__UHEAP_MARK;
	TPtrC content(_L("   some   testing  a  texty     content"));

	CPlainText* doc=CPlainText::NewL(D);
	doc->InsertL(0,content);

	INFO_PRINTF1(_L("WordInfo()"));
	TInt currentPos,startPos,length;
	currentPos=0;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==0);	test(length==0);
	currentPos=1;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==1);	test(length==0);
	currentPos=2;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==2);	test(length==0);

	currentPos=3;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==3);	test(length==4);

	currentPos=5;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==3);	test(length==4);

	currentPos=7;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==3);	test(length==4);

	currentPos=8;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==8);	test(length==0);
	
	currentPos=9;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==9);	test(length==0);

	currentPos=10;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==10);	test(length==7);

	currentPos=18;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==18);	test(length==0);

	currentPos=19;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==19);	test(length==1);

	currentPos=20;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==19);	test(length==1);

	currentPos=21;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos==21);	test(length==0);

	currentPos=38;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos=32);	test(length==7);

	currentPos=39;
	doc->GetWordInfo(currentPos,startPos,length,KPictureIsDelimiter,KPunctuationIsDelimiter);
	test(startPos=32);	test(length==7);

	delete doc;
	__UHEAP_MARKEND;
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest5bL()
//
// Tests the CharPosOfParagraph() method.
//
	{
	__UHEAP_MARK;
	TPtrC para1(_L("para1"));
	TPtrC para2(_L("para22"));
	TPtrC para3(_L("para333"));
	TPtrC para4(_L("para4444"));
	CPlainText* doc=CPlainText::NewL(D);
	doc->InsertL(0,para1);
	TChar delim(EParagraphDelimiter);
	doc->InsertL(5,delim);
	doc->InsertL(6,para2);
	doc->InsertL(12,delim);
	doc->InsertL(13,para3);
	doc->InsertL(20,delim);
	doc->InsertL(21,para4);
	//
	TInt paraN=0;
	//
	DoCharPosOfParagraph(doc,paraN,0,6);
	paraN++;
	DoCharPosOfParagraph(doc,paraN,6,7);
	paraN++;
	DoCharPosOfParagraph(doc,paraN,13,8);
	paraN++;
	DoCharPosOfParagraph(doc,paraN,21,9);
	paraN++;
	DoCharPosOfParagraph(doc,paraN,CPlainText::EScanEndOfData,-1);
	//
	doc->InsertL(0,delim);
	doc->InsertL(0,delim);
	paraN=0;
	DoCharPosOfParagraph(doc,paraN,0,1);
	doc->InsertL(doc->DocumentLength(),delim);
	// There are now 6 paras.
	TInt paraCount=doc->ParagraphCount();
	test(paraCount==7);
	paraN=5;
	DoCharPosOfParagraph(doc,paraN,23,9);
	paraN++;
	DoCharPosOfParagraph(doc,paraN,32,1);
	delete doc;
	__UHEAP_MARKEND;
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::DoCharPosOfParagraph(CPlainText* aText,TInt aParaN,TInt aPosCheck,TInt aLengthCheck)
//
//
//
	{
	TInt length=0;
	TInt pos=-1;
	pos=aText->CharPosOfParagraph(length,aParaN);
	test(pos==aPosCheck);
	if (aLengthCheck!=-1)
		test(length==aLengthCheck);
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest5cL()
//
// Tests the ParagraphNumberForPos() method.
//
	{
	__UHEAP_MARK;
	TPtrC para1(_L("para1"));
	TPtrC para2(_L("para22"));
	TPtrC para3(_L("para333"));
	TPtrC para4(_L("para4444"));
	CPlainText* doc=CPlainText::NewL(D);
	doc->InsertL(0,para1);
	TChar delim(EParagraphDelimiter);
	doc->InsertL(5,delim);
	doc->InsertL(6,para2);
	doc->InsertL(12,delim);
	doc->InsertL(13,para3);
	doc->InsertL(20,delim);
	doc->InsertL(21,para4);
	//
	TInt pos=0;
	//
	for (;pos<6;pos++)
		DoParagraphNumberForPos(doc,pos,0,0);
	for (pos=6;pos<13;pos++)
		DoParagraphNumberForPos(doc,pos,1,6);
	for (pos=13;pos<21;pos++)
		DoParagraphNumberForPos(doc,pos,2,13);
	for (pos=21;pos<30;pos++)
		DoParagraphNumberForPos(doc,pos,3,21);
	//
	doc->InsertL(0,delim);
	doc->InsertL(0,delim);
	pos=0;
	DoParagraphNumberForPos(doc,pos,0,0);
	pos=1;
	DoParagraphNumberForPos(doc,pos,1,1);
	doc->InsertL(doc->DocumentLength(),delim);
	// There are now 7 paras.
	TInt paraCount=doc->ParagraphCount();
	test(paraCount==7);
	pos=32;
	DoParagraphNumberForPos(doc,pos,6,32);
	delete doc;
	__UHEAP_MARKEND;
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::DoParagraphNumberForPos(CPlainText* aText,TInt aPos,TInt aParaCheck,TInt aPosCheck)
//
//
//
	{
	TInt para=-1;
	para=aText->ParagraphNumberForPos(aPos);
	test(para==aParaCheck);
	test(aPos==aPosCheck);
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest6L()
//
// Tests the word count method.
//
	{
	__UHEAP_MARK;
	CPlainText* doc=CPlainText::NewL(D);
	TInt count=doc->WordCount();
	test(count==0);
	
	TPtrC buf(_L("This has four words"));
	doc->InsertL(0,buf);
	count=doc->WordCount();
	test(count==4);
	delete doc;
	__UHEAP_MARKEND;
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest7L()
//
// Tests the paragraph count method.
//
	{
	__UHEAP_MARK;
	CPlainText* doc=CPlainText::NewL(D);
	TPtrC buf(_L("This has four words"));
	doc->InsertL(0,buf);
	TInt count=doc->ParagraphCount();
	test(count==1);
	delete doc;
	__UHEAP_MARKEND;
	}


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::CPlainTest8L()
//
// Tests the reset method behave as specified.
//
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("1st Reset"));
	CPlainText* doc=CPlainText::NewL(D);
	doc->InsertL(0,iDefaultDoc);
	test(doc->DocumentLength()==95);
	doc->Reset();
	test(doc->DocumentLength()==0);

	// Test multiple resets work.
	// This defect was highlighted by DavidW 13.10.95.
	// It arises because I have added an assert length>0 in the delete method.
	// Clearly on a second reset the length is zero, hence the failure.
	// Defect fixed 13.10.95 by DuncanS and included in the tests below.
	INFO_PRINTF1(_L("2nd Reset - defect - DavidW 13.10.95"));
	doc->Reset();
	
	delete doc;
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ETEXT-UT-1574
@SYMTestCaseDesc	    Tests for the removal of inline text when inline text removal flag is set.
@SYMTestPriority 	    High
@SYMTestActions  	    Inserts some inline text into a section of text, tests to make sure it was inserts
						then it removes the inline text and tests to make sure it was all removed correctly.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 INC070807
*/	


template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::INC070807L()
//
// Tests whether inline text is removed when the EExcludeInlineEditedText flag is set.
//
	{
	__UHEAP_MARK;
	
	TInt textLengthFifty = 50;
	TInt textLengthTen = 10;
	TInt startingPositionZero = 0;
	TInt startingPositionTen = 10;	
	TInt NumberDeleted, NumberInserted, PositionOfInsertion, NewPositionOfInsertion = 0;
	TBool HasChangedFormat = NULL;
	TInt NumberToHide = 0; 	
	MFepInlineTextFormatRetriever* InlineTextFormatRetriever = NULL;
	TBufC<20> InitialInlineText =_L("INLINE_TEXT");  // inline text to be inserted	
	TBuf<50> outputBuf;
			
	INFO_PRINTF1(_L("Return all the text"));
	CPlainText* doc=CPlainText::NewL(D);
	doc->DocumentLength();
	doc->InsertL(0,iInsertBuf); // insert "abcdefghijklmnopqrstuvwxyz"

	
	TInt PosOfInlineText = 6;
	
	doc->StartFepInlineEditL(HasChangedFormat,NumberDeleted,NumberInserted,PositionOfInsertion,NewPositionOfInsertion,InitialInlineText,PosOfInlineText,NumberToHide,*InlineTextFormatRetriever);

	doc->Extract(outputBuf, startingPositionZero, textLengthFifty);  // Returns all the text
	test (iTestBuf1 == outputBuf);  // testing outputBuf, making sure it contains the inline text - "abcdefINLINE_TEXTghijklmnopqrstuvwxyz"
	
	INFO_PRINTF1(_L("Extract and discard only the inline text, from Pos0 returning the rest"));
	doc->ExtractSelectively(outputBuf, startingPositionZero, textLengthFifty, EExcludeInlineEditedText); // Returns only non-inline text
	test (iInsertBuf == outputBuf); // testing outputBuf, making sure the inline text has been extracted correctly - "abcdefghijklmnopqrstuvwxyz"	
	delete doc;	


	CPlainText* doc2=CPlainText::NewL(D);	
	doc2->DocumentLength();
	doc2->InsertL(0,iInsertBuf); // insert "abcdefghijklmnopqrstuvwxyz"
	
	INFO_PRINTF1(_L("Extract upto 50 character, from Pos10, within the range (no inline editting set)"));
	// Returns upto 50 characters starting from Pos10
	doc2->ExtractSelectively(outputBuf, startingPositionTen, textLengthFifty, EExcludeInlineEditedText);
	test (iTestBuf2 == outputBuf);  // "klmnopqrstuvwxyz"		
	
	INFO_PRINTF1(_L("Extract upto 10 characters, from Pos10, within the range (no inline editting set)"));
	// Returns upto 10 characters starting from Pos10
	doc2->ExtractSelectively(outputBuf, startingPositionTen, textLengthTen, EExcludeInlineEditedText); 
	test (iTestBuf2a == outputBuf); // "klmnopqrst"		

	INFO_PRINTF1(_L("Extract upto 50 character, from Pos0, 'EExtractAll' flag selected"));	
	// Returns upto 50 characters starting from Pos0.
	doc2->ExtractSelectively(outputBuf, startingPositionZero, textLengthFifty, EExtractAll); 
	test (iInsertBuf == outputBuf); // "abcdefghijklmnopqrstuvwxyz"
	delete doc2;


	CPlainText* doc3=CPlainText::NewL(D);	
	doc3->DocumentLength();
	doc3->InsertL(0,iInsertBuf); // insert "abcdefghijklmnopqrstuvwxyz"
 
	PosOfInlineText = 4;
	
	doc3->StartFepInlineEditL(HasChangedFormat,NumberDeleted,NumberInserted,PositionOfInsertion,NewPositionOfInsertion,InitialInlineText,PosOfInlineText,NumberToHide,*InlineTextFormatRetriever);

	INFO_PRINTF1(_L("Extract all the non-inline text, from Pos10, within the range (inline editting on)"));
	doc3->ExtractSelectively(outputBuf, startingPositionTen, textLengthTen, EExcludeInlineEditedText); // Returns only non-inline text.
	test (iTestBuf3 == outputBuf); // testing outputBuf, making sure the inline text has been extracted correctly	- "efghi".	
	delete doc3;	

	__UHEAP_MARKEND;
	}
	  

template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::TestSegmentedL()
//
// Controls testing of the segmented storage case.
//
	{
    INFO_PRINTF1(_L("All methods"));
	CPlainTest0L();
	INFO_PRINTF1(_L("Construction"));
	CPlainTest1L();
	INFO_PRINTF1(_L("Read"));
	INFO_PRINTF1(_L("Not yet implemented"));

 	INFO_PRINTF1(_L("Inverse Testing using InsertL&Delete: content left intact"));
	CPlainTest3L();
 	INFO_PRINTF1(_L("Paragraph Start"));
	CPlainTest4L();
 	INFO_PRINTF1(_L("Extract"));
	CPlainTest5L();
	INFO_PRINTF1(_L("WordInfo"));
	CPlainTest5aL();
	INFO_PRINTF1(_L("CharPosOfParagraph"));
	CPlainTest5bL();
	INFO_PRINTF1(_L("ParagraphNumberForPos"));
	CPlainTest5cL();
 	INFO_PRINTF1(_L("CountWords"));
	CPlainTest6L();
	INFO_PRINTF1(_L("CountParas"));
	CPlainTest7L();
 	INFO_PRINTF1(_L("Reset"));
	CPlainTest8L();
	}



template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::TestL()
//
// Test the CPlainText methods
//
	{
	INFO_PRINTF1(_L("All methods"));
	CPlainTest0L();
	INFO_PRINTF1(_L("Construction"));
	CPlainTest1L();
	INFO_PRINTF1(_L("Read"));
	CPlainTest2L();
	INFO_PRINTF1(_L("Inverse Testing using InsertL&Delete: content left intact"));
	CPlainTest3L();
	INFO_PRINTF1(_L("Paragraph Start"));
	CPlainTest4L();
	INFO_PRINTF1(_L("Extract"));
	CPlainTest5L();
	INFO_PRINTF1(_L("WordInfo"));
	CPlainTest5aL();
	INFO_PRINTF1(_L("CharPosOfParagraph"));
	CPlainTest5bL();
	INFO_PRINTF1(_L("ParagraphNumberForPos"));
	CPlainTest5cL();
	INFO_PRINTF1(_L("CountWords"));
	CPlainTest6L();
	INFO_PRINTF1(_L("CountParas"));
	CPlainTest7L();
	INFO_PRINTF1(_L("Reset"));
	CPlainTest8L();
	}



template<class S,class T,CEditableText::TDocumentStorage D>
void TestCPlainText<S,T,D>::DefectsL()
//
// Defect Fixes
//
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-UT-1574 INC070807 "));
	INC070807L();
	}

CT_CPLAIN::CT_CPLAIN()
    {
    SetTestStepName(KTestStep_T_CPLAIN);
    pTestStep = this;
    }

TVerdict CT_CPLAIN::doTestStepL()
    {
    SetTestStepResult(EFail);

    CTrapCleanup* cleanup=CTrapCleanup::New();
    
    __UHEAP_MARK;
    
    typedef TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage> instance1;
    typedef TestCPlainText<TText,TPtrC,CEditableText::ESegmentedStorage> instance2;
    typedef TestCPlainText<TText,TPtrC,CEditableText::EFlatStorage> instance3;

    instance1* doctest=NULL;

    TRAPD(ret1, doctest=instance1::NewL());

    INFO_PRINTF1(_L("CPlainText - Flat"));
    TRAPD(ret2, doctest->TestL());
    delete doctest;

    INFO_PRINTF1(_L("CPlainText - Segmented"));
    instance2* doctest1=NULL;
    TRAPD(ret3, doctest1=instance2::NewL());
    TRAPD(ret4, doctest1->TestSegmentedL());
    delete doctest1;

    INFO_PRINTF1(_L("Defect..."));
    instance3* doctest2=NULL;
    TRAPD(ret5, doctest2=instance3::NewL());
    TRAPD(ret6, doctest2->DefectsL());
    delete doctest2;

    __UHEAP_MARKEND;
    
    delete cleanup;

    if (ret1 == KErrNone && ret2 == KErrNone && ret3 == KErrNone && ret4 == KErrNone && ret5 == KErrNone && ret6 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
