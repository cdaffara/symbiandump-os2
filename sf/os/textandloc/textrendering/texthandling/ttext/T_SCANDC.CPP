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
#include "T_SCANDC.h"

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

template<class S,CEditableText::TDocumentStorage D>
class CTestScan : public CPlainText
	{
public:
	static CTestScan* NewL();
	void Destruct();
	void Test1L();  // Tests all operations of the class.
	void Test2L();  // Test public methods of class.
	void Test2aL();  // Tests for CountWords
	void Test2bL();  // Tests for ScanWord
	void Test2cL();  // Tests for CountParagraphs
	void Test2dL();  // Tests for ScanParagraph
	void TestHeapSpace();  // Test for memory leakage
	void StoreTextL(TBuf<64>&);
	void DoCountWords(TInt);
	void DoCountParas(TInt);
	void DoScanWords(TInt aPos,TUint aFlags,TInt aEndPos,TInt aCharCheck);
	void DoScanParas(TInt aPos,TUint aFlags,TInt aEndPos,TInt aCharCheck);
	void InsertL(TInt aPos,const TDesC& aBuf);
public:
	CPlainText *iDocument;
	TInt iUnitCount;
	TInt iCharsSkipped;
	TChar iDelimiter;
	TInt startAvail,endAvail,temp;
private:
 CTestScan();  // Default constructor
 CTestScan(const CTestScan& aCTestScan);
 CTestScan& operator=(const CTestScan& aCTestScan);
	};


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C CTestScan<S,D>* CTestScan<S,D>::NewL()
//
// constructs a new instance of this class.
//
	{
	CTestScan<S,D>* tmp=new(ELeave) CTestScan<S,D>;
	tmp->ConstructL(D);
	return tmp;
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C CTestScan<S,D>::CTestScan()
//
// Default constructor
//
	:iDelimiter(EParagraphDelimiter)
	{
	TRAPD(ret,iDocument=CPlainText::NewL(D));
    UNUSED_VAR(ret);
	}


template<class S,CEditableText::TDocumentStorage D>
void CTestScan<S,D>::Destruct()
//
//
//
	{
	delete iDocument;
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::InsertL(TInt aPos,const TDesC& aBuf)
//
	{CPlainText::InsertL(aPos,aBuf);}



template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::StoreTextL(TBuf<64>& aBuf)
//
// Insert from aBuf into document storage at position 0.
//
	{
	TInt length=iDocument->DocumentLength();
	if (length>0)
		{
		iDocument->DeleteL(0,(length));
		}
	TPtrC buf(aBuf);
	iDocument->InsertL(0,buf);
	iUnitCount=0;
	}			


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::DoCountWords(TInt aCount)
//
// Calls CountWords, and checks that number of words
// counted, matches the known good value 'count'.
//
	{
	iUnitCount=iDocument->WordCount();
	test(aCount==iUnitCount);
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::DoCountParas(TInt aCount)
//
// Calls CountParagraphs, and checks that number
// counted, matches the known good value 'count'.
//
	{
	iUnitCount=iDocument->ParagraphCount();
	test(aCount==iUnitCount);
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::DoScanWords(TInt aPos,TUint aFlags,TInt aEndPos,TInt aCharCheck)
//
// Calls ScanWords and tests results are as expected.
//
	{
	iCharsSkipped=iDocument->ScanWords(aPos,aFlags);
	test(iCharsSkipped==aCharCheck);
	test(aPos==aEndPos);
	iCharsSkipped=0;
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::DoScanParas(TInt aPos,TUint aFlags,TInt aEndPos,TInt aCharCheck)
//
// Calls ScanParas and checks results are as expected.
//
	{
	iCharsSkipped=iDocument->ScanParas(aPos,aFlags);
	test(iCharsSkipped==aCharCheck);
	test(aPos==aEndPos);
	iCharsSkipped=0;
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::Test1L()
//
// Tests all operations of the class.
//
	{
	TBuf<64> temp1;
	temp1.Append(_L("The world"));	//	Create a descriptor over some text
	temp1.Append(iDelimiter);
	temp1.Append(iDelimiter);
	temp1.Append(_L("is a"));
	StoreTextL(temp1);
	iDocument->WordCount();
	TInt pos=5;
	TUint flags=(CPlainText::EScanToUnitStart|CPlainText::EScanJoinDelimiters|CPlainText::EScanStayIfBoundary);
	((CTestScan*)iDocument)->ScanWords(pos,flags);
	TBuf<64> temp2(_L("This is a paragraph"));
	temp2.Append(iDelimiter);
	iDocument->DeleteL(0,temp1.Length());
	StoreTextL(temp2);
	pos=0;
	flags=(CPlainText::EScanToUnitStart|CPlainText::EScanJoinDelimiters);
	((CTestScan*)iDocument)->ScanParas(pos,flags);
	iDocument->ParagraphCount();
	}

template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::Test2aL()
//
//	Tests the CountWords method with different cases
//
	{
	TBuf<64> temp;
	
	//INFO_PRINTF1(_L("0 words"));
	//iDocument->InsertL(0,buf.Ptr(),buf.Size());
	//DoCountWords(0);
	// This test has been removed since it is no longer feasible.
	// The design currently inserts an end of document character
	// as soon as the document is created, hence, there can not
	// be 0 words.
	
	INFO_PRINTF1(_L("0 words, all delimiters"));
	temp.Append(_L("   "));
	temp.Append(iDelimiter);
	temp.Append(_L("   "));
	StoreTextL(temp);
	DoCountWords(0);
	
	INFO_PRINTF1(_L("1 word, no delimiter"));
	temp.Append(_L("This"));
	StoreTextL(temp);
	DoCountWords(1);

	INFO_PRINTF1(_L("1 word, at start, no delimiter"));
	temp.Delete(0,temp.Size());
	temp.Append(_L("THIS"));
	StoreTextL(temp);
	DoCountWords(1);

	INFO_PRINTF1(_L("1 word, with delimiter"));
	temp.Append(' ');
	StoreTextL(temp);
	DoCountWords(1);

	INFO_PRINTF1(_L("Many words, no delimiter"));
	temp.Append(_L("  is a wonderful world"));
	StoreTextL(temp);
	DoCountWords(5);

	INFO_PRINTF1(_L("Many words with delimiter"));
	temp.Append(' ');
	StoreTextL(temp);
	DoCountWords(5);

	
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::Test2bL()
//
//	Tests the ScanWord method with different flag settings
//
	{
	TBuf<64> buf;
	buf.Append(_L(" This  tEsT  TeXT"));
	StoreTextL(buf);  

	iCharsSkipped=iUnitCount=0;
	INFO_PRINTF1(_L("Forward scan"));

	INFO_PRINTF1(_L("Stay at boundary:to unit start"));
	TUint flags=(CPlainText::EScanStayIfBoundary | CPlainText::EScanToUnitStart);
	DoScanWords(8,flags,12,4);
	DoScanWords(7,flags,7,0);
	DoScanWords(6,flags,6,0);
	DoScanWords(5,flags,6,1);
		
	INFO_PRINTF1(_L("Stay at boundary:to unit end"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanWords(8,flags,11,3);
	DoScanWords(7,flags,11,4);
	DoScanWords(6,flags,6,0);
	DoScanWords(5,flags,5,0);

	INFO_PRINTF1(_L("Stay at boundary:to unit start:join delimiters"));
	flags=(CPlainText::EScanStayIfBoundary|CPlainText::EScanToUnitStart|CPlainText::EScanJoinDelimiters);
	DoScanWords(8,flags,13,5);
	DoScanWords(7,flags,7,0);
	DoScanWords(6,flags,7,1);
	DoScanWords(5,flags,7,2);
	
	INFO_PRINTF1(_L("Stay at boundary:to unit end:join delimiters"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanWords(8,flags,11,3);
	DoScanWords(7,flags,11,4);
	DoScanWords(6,flags,11,5);
	DoScanWords(5,flags,5,0);

	INFO_PRINTF1(_L("To unit start"));
	flags=CPlainText::EScanToUnitStart;
	DoScanWords(8,flags,12,4);
	DoScanWords(7,flags,12,5);
	DoScanWords(6,flags,7,1);
	DoScanWords(5,flags,6,1);
	
	INFO_PRINTF1(_L("To unit end"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanWords(8,flags,11,3);
	DoScanWords(7,flags,11,4);
	DoScanWords(6,flags,11,5);
	DoScanWords(5,flags,6,1);

	INFO_PRINTF1(_L("To unit start:join delimiters"));
	flags=(CPlainText::EScanToUnitStart|CPlainText::EScanJoinDelimiters);
	DoScanWords(8,flags,13,5);
	DoScanWords(7,flags,13,6);
	DoScanWords(6,flags,7,1);
	DoScanWords(5,flags,7,2);

	INFO_PRINTF1(_L("To unit end:join delimiters"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanWords(8,flags,11,3);
	DoScanWords(7,flags,11,4);
	DoScanWords(6,flags,11,5);
	DoScanWords(5,flags,11,6);
	
	INFO_PRINTF1(_L("Backward scan"));
	INFO_PRINTF1(_L("Stay at boundary:to unit start"));
	flags=(CPlainText::EScanBackwards|CPlainText::EScanStayIfBoundary|CPlainText::EScanToUnitStart);
	DoScanWords(9,flags,7,2);
	DoScanWords(10,flags,7,3);
	DoScanWords(11,flags,7,4);
	DoScanWords(12,flags,12,0);
	DoScanWords(13,flags,13,0);
	
	INFO_PRINTF1(_L("Stay at boundary:to unit end"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanWords(9,flags,6,3);
	DoScanWords(10,flags,6,4);
	DoScanWords(11,flags,11,0);
	DoScanWords(12,flags,12,0);
	DoScanWords(13,flags,12,1);

	INFO_PRINTF1(_L("Stay at boundary:to unit start:join delimiters"));
	flags=CPlainText::EScanBackwards|CPlainText::EScanStayIfBoundary|CPlainText::EScanToUnitStart|CPlainText::EScanJoinDelimiters;
	DoScanWords(9,flags,7,2);
	DoScanWords(10,flags,7,3);
	DoScanWords(11,flags,7,4);
	DoScanWords(12,flags,7,5);
	DoScanWords(13,flags,13,0);

	INFO_PRINTF1(_L("Stay at boundary:to unit end:join delimiters"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanWords(9,flags,5,4);
	DoScanWords(10,flags,5,5);
	DoScanWords(11,flags,11,0);
	DoScanWords(12,flags,11,1);
	DoScanWords(13,flags,11,2);
	
	INFO_PRINTF1(_L("To unit start"));
	flags=CPlainText::EScanBackwards|CPlainText::EScanToUnitStart;
	DoScanWords(9,flags,7,2);
	DoScanWords(10,flags,7,3);
	DoScanWords(11,flags,7,4);
	DoScanWords(12,flags,7,5);
	DoScanWords(13,flags,12,1);
	
	INFO_PRINTF1(_L("To unit end"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanWords(9,flags,6,3);
	DoScanWords(10,flags,6,4);
	DoScanWords(11,flags,6,5);
	DoScanWords(12,flags,11,1);
	DoScanWords(13,flags,12,1);

	INFO_PRINTF1(_L("To unit start:join delimiters"));
	flags=CPlainText::EScanBackwards|CPlainText::EScanToUnitStart|CPlainText::EScanJoinDelimiters;
	DoScanWords(9,flags,7,2);
	DoScanWords(10,flags,7,3);
	DoScanWords(11,flags,7,4);
	DoScanWords(12,flags,7,5);
	DoScanWords(13,flags,7,6);

	INFO_PRINTF1(_L("To unit end:join delimiters"));
	flags=CPlainText::EScanToUnitEnd|CPlainText::EScanJoinDelimiters|CPlainText::EScanBackwards;
	DoScanWords(9,flags,5,4);
	DoScanWords(10,flags,5,5);
	DoScanWords(11,flags,5,6);
	DoScanWords(12,flags,11,1);
	DoScanWords(13,flags,11,2);
	
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::Test2cL()
//
//	Tests the CountParagraphs method with different cases
//
	{
	TBuf<64> buf;
	
	//INFO_PRINTF1(_L("0 paras"));
	//iDocument->InsertL(0,buf.Ptr(),buf.Size());
	//DoCountParagraphs(0);
	// This test no longer applies, since documents are created with
	// an end of document character.  ie, The document can not have
	// 0 words.
	
	INFO_PRINTF1(_L("Empty text source"));
//	buf.Append(iDelimiter);
//	buf.Append(iDelimiter);
	StoreTextL(buf);
	DoCountParas(1);
	
	INFO_PRINTF1(_L("1 para. no delimiter"));
	buf.Append(_L("This"));
	StoreTextL(buf);
	DoCountParas(1);

	INFO_PRINTF1(_L("1 para. at start, no delimiter"));
	buf.Delete(0,buf.Size());
	buf.Append(_L(" THIS   "));
	StoreTextL(buf);
	DoCountParas(1);

	INFO_PRINTF1(_L("1 para, with delimiter"));
	buf.Append(iDelimiter);
	StoreTextL(buf);
	DoCountParas(2);

	INFO_PRINTF1(_L("2 paras, no delimiter"));
	buf.Append(' ');
	StoreTextL(buf);
	DoCountParas(2);

	INFO_PRINTF1(_L("2 paras with delimiter"));
	buf.Append(iDelimiter);
	StoreTextL(buf);
	DoCountParas(3);

	
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::Test2dL()
//
//	Tests the ScanParagraph method with different flag settings
//
	{
	TBuf<64> buf;
	buf.Append(_L(" This"));
	buf.Append(iDelimiter);
	buf.Append(iDelimiter);
	buf.Append(_L("tEsT"));
	buf.Append(iDelimiter);
	buf.Append(iDelimiter);
	buf.Append(_L("TeXT"));
	StoreTextL(buf);  

	iCharsSkipped=iUnitCount=0;
	INFO_PRINTF1(_L("Forward scan"));

	INFO_PRINTF1(_L("Stay at boundary:to unit start"));
	TUint flags=(CPlainText::EScanStayIfBoundary|CPlainText::EScanToUnitStart);
	DoScanParas(8,flags,12,4);
	DoScanParas(7,flags,7,0);
	DoScanParas(6,flags,6,0);
	DoScanParas(5,flags,6,1);
		
	INFO_PRINTF1(_L("Stay at boundary:to unit end"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanParas(8,flags,11,3);
	DoScanParas(7,flags,11,4);
	DoScanParas(6,flags,6,0);
	DoScanParas(5,flags,5,0);

	INFO_PRINTF1(_L("Stay at boundary:to unit start:join delimiters"));
	flags=(CPlainText::EScanStayIfBoundary | CPlainText::EScanToUnitStart | CPlainText::EScanJoinDelimiters);
	DoScanParas(8,flags,13,5);
	DoScanParas(7,flags,7,0);
	DoScanParas(6,flags,7,1);
	DoScanParas(5,flags,7,2);
	
	INFO_PRINTF1(_L("Stay at boundary:to unit end:join delimiters"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanParas(8,flags,11,3);
	DoScanParas(7,flags,11,4);
	DoScanParas(6,flags,11,5);
	DoScanParas(5,flags,5,0);

	INFO_PRINTF1(_L("To unit start"));
	flags=CPlainText::EScanToUnitStart;
	DoScanParas(8,flags,12,4);
	DoScanParas(7,flags,12,5);
	DoScanParas(6,flags,7,1);
	DoScanParas(5,flags,6,1);
	
	INFO_PRINTF1(_L("To unit end"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanParas(8,flags,11,3);
	DoScanParas(7,flags,11,4);
	DoScanParas(6,flags,11,5);
	DoScanParas(5,flags,6,1);

	INFO_PRINTF1(_L("To unit start:join delimiters"));
	flags=(CPlainText::EScanToUnitStart | CPlainText::EScanJoinDelimiters);
	DoScanParas(8,flags,13,5);
	DoScanParas(7,flags,13,6);
	DoScanParas(6,flags,7,1);
	DoScanParas(5,flags,7,2);

	INFO_PRINTF1(_L("To unit end:join delimiters"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanParas(8,flags,11,3);
	DoScanParas(7,flags,11,4);
	DoScanParas(6,flags,11,5);
	DoScanParas(5,flags,11,6);
	
	INFO_PRINTF1(_L("Backward scan"));
	INFO_PRINTF1(_L("Stay at boundary:to unit start"));
	flags=(CPlainText::EScanBackwards|CPlainText::EScanStayIfBoundary|CPlainText::EScanToUnitStart);
	DoScanParas(9,flags,7,2);
	DoScanParas(10,flags,7,3);
	DoScanParas(11,flags,7,4);
	DoScanParas(12,flags,12,0);
	DoScanParas(13,flags,13,0);
	DoScanParas(7,flags,7,0);
	DoScanParas(1,flags,0,1);
	DoScanParas(5,flags,0,5);
	DoScanParas(4,flags,0,4);
	DoScanParas(3,flags,0,3);
	DoScanParas(2,flags,0,2);
	DoScanParas(1,flags,0,1);
	INFO_PRINTF1(_L("Testing at buffer boundaries."));
	DoScanParas(0,flags,0,0);
	DoScanParas(16,flags,13,3);
	

	INFO_PRINTF1(_L("Stay at boundary:to unit end"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanParas(9,flags,6,3);
	DoScanParas(10,flags,6,4);
	DoScanParas(11,flags,11,0);
	DoScanParas(12,flags,12,0);
	DoScanParas(13,flags,12,1);

	INFO_PRINTF1(_L("Stay at boundary:to unit start:join delimiters"));
	flags=CPlainText::EScanBackwards|CPlainText::EScanStayIfBoundary|CPlainText::EScanToUnitStart|CPlainText::EScanJoinDelimiters;
	DoScanParas(9,flags,7,2);
	DoScanParas(10,flags,7,3);
	DoScanParas(11,flags,7,4);
	DoScanParas(12,flags,7,5);
	DoScanParas(13,flags,13,0);

	INFO_PRINTF1(_L("Stay at boundary:to unit end:join delimiters"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanParas(9,flags,5,4);
	DoScanParas(10,flags,5,5);
	DoScanParas(11,flags,11,0);
	DoScanParas(12,flags,11,1);
	DoScanParas(13,flags,11,2);
	
	INFO_PRINTF1(_L("To unit start"));
	flags=CPlainText::EScanBackwards|CPlainText::EScanToUnitStart;
	DoScanParas(9,flags,7,2);
	DoScanParas(10,flags,7,3);
	DoScanParas(11,flags,7,4);
	DoScanParas(12,flags,7,5);
	DoScanParas(13,flags,12,1);
	
	INFO_PRINTF1(_L("To unit end"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanParas(9,flags,6,3);
	DoScanParas(10,flags,6,4);
	DoScanParas(11,flags,6,5);
	DoScanParas(12,flags,11,1);
	DoScanParas(13,flags,12,1);

	INFO_PRINTF1(_L("To unit start:join delimiters"));
	flags=CPlainText::EScanBackwards|CPlainText::EScanToUnitStart|CPlainText::EScanJoinDelimiters;
	DoScanParas(9,flags,7,2);
	DoScanParas(10,flags,7,3);
	DoScanParas(11,flags,7,4);
	DoScanParas(12,flags,7,5);
	DoScanParas(13,flags,7,6);

	INFO_PRINTF1(_L("To unit end:join delimiters"));
	flags |= CPlainText::EScanToUnitEnd;
	flags &=~CPlainText::EScanToUnitStart;
	DoScanParas(9,flags,5,4);
	DoScanParas(10,flags,5,5);
	DoScanParas(11,flags,5,6);
	DoScanParas(12,flags,11,1);
	DoScanParas(13,flags,11,2);
	
	
	}


template<class S,CEditableText::TDocumentStorage D>
GLDEF_C void CTestScan<S,D>::Test2L()
//
// Test all the methods of the class
//
	{
	INFO_PRINTF1(_L("CountWords"));
	Test2aL();

	INFO_PRINTF1(_L("ScanWord"));
	Test2bL();

	INFO_PRINTF1(_L("Count Paragraphs"));
	Test2cL();

	INFO_PRINTF1(_L("Scan Paragraph"));
	Test2dL();

	
	}

CT_SCANDC::CT_SCANDC()
    {
    SetTestStepName(KTestStep_T_SCANDC);
    pTestStep = this;
    }

TVerdict CT_SCANDC::doTestStepL()
    {
    SetTestStepResult(EFail);

    TheTrapCleanup=CTrapCleanup::New();

    INFO_PRINTF1(_L("SCANNING OF TEXT DOCUMENTS"));
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-TTEXT-LEGACY-T_SCANDC-0001 CPlainText - Flat ")); 
    typedef CTestScan<TText,CEditableText::EFlatStorage> instance1;
    typedef CTestScan<TText,CEditableText::ESegmentedStorage> instance2;
    
    instance1* aa=NULL;
    TRAPD(ret1, aa=instance1::NewL());

    INFO_PRINTF1(_L("Registering all methods"));

    TRAPD(ret2,aa->Test1L());

    INFO_PRINTF1(_L("Testing all methods"));

    TRAPD(ret3,aa->Test2L());

    __UHEAP_MARK;
    delete aa;
    __UHEAP_MARKEND;

    INFO_PRINTF1(_L("CPlainText - Segmented"));

    instance2* bb=NULL;
    TRAPD(ret4, bb=instance2::NewL());

    __UHEAP_MARK;
    INFO_PRINTF1(_L("Registering all methods"));
    TRAPD(ret5,bb->Test1L());
    INFO_PRINTF1(_L("Testing all methods"));
    TRAPD(ret6,bb->Test2L());
    delete bb;
    __UHEAP_MARKEND;
    
    delete TheTrapCleanup;

    if (ret1 == KErrNone && ret2 == KErrNone && ret3 == KErrNone && ret4 == KErrNone && ret5 == KErrNone && ret6 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
