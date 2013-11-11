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
* Class to test the line paginator.
*
*/


#include <basched.h>
#include <frmpage.h>
#include <frmconst.h>
#include <bautils.h>
#include <e32test.h>
#include "tlinepag.h"

namespace LocalToTLinePag
{
_LIT(KTLinePag, "TLinePag");
CTLinePagStep*  TestStep = NULL;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)
}
using namespace LocalToTLinePag;
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions

LOCAL_C void CheckL(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
        TBuf<256> buf;
        buf.AppendFormat(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
        TestStep->print(buf,(TText8*)__FILE__,aLine);
		TestStep->testpoint(EFalse,(TText8*)__FILE__,aLine);
		}
	}

class CLinePaginatorTest : public CBase
	{
public:
	static CLinePaginatorTest* NewL();
	~CLinePaginatorTest();
	void OpenTestFileL();
	void StartPaginateL();
	TBool CompareArrays();
private:
	void ConstructL();
	CLinePaginatorTest();
	TBool PaginateSectionL();
	TBool ReadTestFile(TPageLine& aline);
	void WriteOutputFile(TPageLine aLine);
	void StorePageBreakL(TInt aDocPos);
private:
	CArrayFixFlat<TInt>* iCharsPerPage;
	TLinePaginator iPaginator;
	CArrayFixFlat<TInt>* iTestChars;
	TInt iNumPages;
	RFile iTestFile;
	RFs  ifsClient;
	TInt iFilePos;
	TBool iTestPageBreak;
	};


#define TEST_FILE _L("z:\\test\\app-framework\\form\\input\\page.txt")

const TInt KGranularity = 10;

CLinePaginatorTest* CLinePaginatorTest::NewL()
	{
	CLinePaginatorTest* self=new(ELeave) CLinePaginatorTest();
	self->ConstructL();
	return self;
	}

CLinePaginatorTest::CLinePaginatorTest()
	{}

CLinePaginatorTest::~CLinePaginatorTest()
	{
	iTestFile.Close();
	ifsClient.Close();
	delete iTestChars;
	delete iCharsPerPage;
	}

void CLinePaginatorTest::ConstructL()
	{
	iCharsPerPage=new(ELeave) CArrayFixFlat<TInt>(KGranularity);
	iTestChars=new(ELeave) CArrayFixFlat<TInt>(KGranularity);

	iFilePos=0;
	iNumPages=0;

	iPaginator.SetArray(iCharsPerPage);
	}


void CLinePaginatorTest::StartPaginateL(/*parameters*/)
	{
	// do initialisation stuff
	while(PaginateSectionL())
		{
		}
	TBool success=CompareArrays();
	CheckL(success, ETrue,__LINE__);
	}


TBool CLinePaginatorTest::PaginateSectionL()
	{
	TPageLine line;

	while (ReadTestFile(line))
		{
		iPaginator.AppendLineL(line);
		if (iTestPageBreak)
			StorePageBreakL(line.iDocPos);
		WriteOutputFile(line);
		}

	iPaginator.FlushL(line.iDocPos);
	StorePageBreakL(line.iDocPos);

	return(EFalse);
	}

void CLinePaginatorTest::OpenTestFileL()
	{
	TBuf8<128> text;
	TInt err=ifsClient.Connect();
	err=iTestFile.Open(ifsClient,TEST_FILE,EFileStream|EFileRead|EFileShareReadersOnly);


	iTestFile.Read(0,text,128);
	TInt startNum=text.Locate('<')+1;
	text.Delete(0,startNum);
	TLex8 lex=text;
	TInt pageHeight;
	lex.Val(pageHeight);
	iPaginator.SetPageHeight(pageHeight);

	TBuf<254> buf;
	buf.AppendFormat(_L("DocPos\tHeight\tKeep\tStart\tHeight of Pages = <%d>\n"), pageHeight);
	TESTPRINT(buf);
	}

TBool CLinePaginatorTest::ReadTestFile(TPageLine& aLine)
	{
	TLex8 lex;
	TBuf8<128> textBuffer;
	TBuf8<128> numBuffer;
	TInt startNum;

	iTestFile.Read(iFilePos,textBuffer,128);
	if (textBuffer.Locate('X') != KErrNotFound && textBuffer.Locate('X') < textBuffer.Locate('\r'))
		{
		startNum=textBuffer.Locate('\n')+1;
  		textBuffer.Delete(0,startNum);
		iFilePos+=startNum;
		lex=textBuffer;
		lex.Val(aLine.iDocPos);
		TBuf<254> buf;
		buf.AppendFormat(_L("%d\tX\n"), aLine.iDocPos);
		TESTPRINT(buf);
		return EFalse;
		}

	startNum=textBuffer.Locate('\n')+1;
	textBuffer.Delete(0,startNum);
	iFilePos+=startNum;
	lex=textBuffer;
	lex.Val(aLine.iDocPos);

	startNum=textBuffer.Locate('\t')+1;
	textBuffer.Delete(0,startNum);
	iFilePos+=startNum;
	lex=textBuffer;
	lex.Val(aLine.iLineHeight);

	startNum=textBuffer.Locate('\t')+1;
	textBuffer.Delete(0,startNum);
	iFilePos+=startNum;
	lex=textBuffer;
	lex.Val(aLine.iKeepWithNext);

	startNum=textBuffer.Locate('\t')+1;
	textBuffer.Delete(0,startNum);
	iFilePos+=startNum;
	lex=textBuffer;
	lex.Val(aLine.iStartNewPage);

	if (textBuffer.Locate('\t')<textBuffer.Locate('\r')
		&& textBuffer.Locate('B')!=KErrNotFound
		&& textBuffer.Locate('B')<textBuffer.Locate('\r'))
		iTestPageBreak=ETrue;
	else
		iTestPageBreak=EFalse;

	iFilePos+=textBuffer.Locate('\r')+1;
	return ETrue;
	}

void CLinePaginatorTest::WriteOutputFile(TPageLine aLine)
	{
    TBuf<256> buf;
	if (iTestPageBreak)
		{
		buf.AppendFormat(_L("%d\t%d\t%d\t%d\tBREAK\n"), aLine.iDocPos, aLine.iLineHeight, aLine.iKeepWithNext, aLine.iStartNewPage);
		TESTPRINT(buf);		
		}
	else
		{
        buf.Zero();        
		buf.AppendFormat(_L("%d\t%d\t%d\t%d\n"), aLine.iDocPos, aLine.iLineHeight, aLine.iKeepWithNext, aLine.iStartNewPage);
		TESTPRINT(buf);
		}

	}

void CLinePaginatorTest::StorePageBreakL(TInt aDocPos)
	{
	iNumPages++;
	iTestChars->AppendL(aDocPos);
	}

//#pragma warning( disable : 4701 )	//local variable 'docPosError' may be used without having been initialized
TBool CLinePaginatorTest::CompareArrays()
	{
	TInt numPages=iCharsPerPage->Count();
	TInt numTestPages=iTestChars->Count();
	TInt numChars;
	TInt numTestChars;
	TBool pagesSame=EFalse;
	TBuf8<128> text;
	TInt prevDocPos=0;
	TInt docPosError=0;

	TBuf<256> buf;
	if (numPages==numTestPages)
		{
 		buf.AppendFormat(_L("Correct Number of Pages = %d\n"), numPages);
 		TESTPRINT(buf);
		pagesSame=ETrue;
		for (TInt i=0; i<numPages; i++)
			{
			numChars=(*iCharsPerPage)[i];
			numTestChars=(*iTestChars)[i]-prevDocPos;
			if(numChars!=numTestChars)
				{
				if (pagesSame)
					docPosError=(*iTestChars)[i];
				pagesSame=EFalse;
				}
			prevDocPos=(*iTestChars)[i];
			}
		if (pagesSame)
			{            
 			TESTPRINT(_L("Correct Number of Characters on Pages -- PASSED\n"));
			}
		else
			{
            TESTPRINT(_L("Incorrect Number of Characters on Pages -- FAILED\n"));            
            buf.Zero();
			buf.AppendFormat(_L("First Error Occurs at Position = %d"), docPosError);
			TESTPRINT(buf);
			}
		}
	else
		{
        TESTPRINT(_L("Incorrect Number of Pages  -- FAILED\n"));
        buf.Zero();
		buf.AppendFormat(_L("Number Calculated	by LinePaginator = %d\n"), numPages);		
		buf.AppendFormat(_L("Number Contained in File = %d\n"), numTestPages);
		TESTPRINT(buf);
		}

	iTestFile.Close();
	return pagesSame;
	}


void LinePaginateL()
	{
	CLinePaginatorTest* linePaginate=CLinePaginatorTest::NewL();
	linePaginate->OpenTestFileL();
	linePaginate->StartPaginateL();
	delete linePaginate;
	}

TVerdict CTLinePagStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
	TESTPRINT(KTLinePag);
	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-LEGACY-LINEPAG-0001 CLinePaginatorTest tests "));
	TRAPD(error, LinePaginateL());
	TESTPOINT(error == KErrNone);
	return TestStepResult();
	}

void FormPanic(TFormPanic aPanic)
	{
	User::Panic(_L("Form"),aPanic);
	}

