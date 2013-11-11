/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent 
*/
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include <txtglobl.h>
#include <frmtview.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <frmtlay.h>

#include <coemain.h>
#include <coeaui.h>

#include <e32test.h>
#include "bandtestfileoutput.h"
#include "bandvalidator.h"
#include "testenvironment.h"
#include "richcontrol.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

EXPORT_C CBandValidator* CBandValidator::NewLC(const CTextView& aTestView, const CTextView& aReferenceView, CFileOutput& aOutput)
	{
	CBandValidator* self = new(ELeave) CBandValidator(aTestView, aReferenceView, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBandValidator::CBandValidator(const CTextView& aTestView, const CTextView& aReferenceView, CFileOutput& aOutput)
	:iFileOutput(aOutput), iTestView(aTestView), iReferenceView(aReferenceView)
	{
	}
	
void CBandValidator::ConstructL()
	{
	iFailLineNumberInPara = -1;
	iFailLineNumber = -1;
	}

EXPORT_C CBandValidator::~CBandValidator()
	{
	}

TBool CBandValidator::LineInfosAreEqualL( TTmLineInfo& aLineUnderTest,  TTmLineInfo& aLineReference)
	{
	return (aLineUnderTest.iFlags == aLineReference.iFlags);
	}
	
EXPORT_C TBool CBandValidator::ValidateL(const TDes& aDesc)
   	{
	iFailLineNumberInPara = -1;
	iFailLineNumber = -1;
   //get line info and text contents for the test view
   	RArray<TTmLineInfo> infoUnderTest;
   	CleanupClosePushL(infoUnderTest);
   	if(!(GetVisibleLineInfosL(iTestView, infoUnderTest)))
   		{
RDebug::Printf("fail: can't find test lines");

   	  	iFileOutput.WriteOperationDetails(aDesc);
  		CleanupStack::PopAndDestroy();
   		return EFalse;
   		}
   	
   	CDesCArrayFlat* contentsUnderTest = new(ELeave) CDesCArrayFlat (10);
   	CleanupStack::PushL(contentsUnderTest);
   	GetLineContentsL(iTestView, infoUnderTest, *contentsUnderTest);
  	
  //get line info and text contents for the reference view	
  	RArray<TTmLineInfo> infoReference;
  	CleanupClosePushL(infoReference);
  	if(!(GetReferenceLineInfosL(infoUnderTest, infoReference)))
  		{
RDebug::Printf("fail: can't find matching reference lines infos");
  	  	iFileOutput.WriteOperationDetails(aDesc);
  		CleanupStack::PopAndDestroy(3);
  		return EFalse;
  		}
   
  	CDesCArrayFlat* contentsReference = new(ELeave) CDesCArrayFlat (10);
  	CleanupStack::PushL(contentsReference);
  	GetLineContentsL(iReferenceView, infoReference, *contentsReference);

  	TBool lineCountDiff = EFalse;
	TInt lineCountForTest = infoUnderTest.Count()<=infoReference.Count()? infoUnderTest.Count(): infoReference.Count();
//now validate the line infos
	if(!(infoUnderTest.Count() == infoReference.Count()))
		{
		lineCountDiff = ETrue;
		RDebug::Printf("fail: test line count differs from reference line count");
		}	
  	TBool lineInfoDiff = EFalse;
	for (TInt i = 0; i < lineCountForTest; i++)
		{
		if(!(LineInfosAreEqualL(infoUnderTest[i], infoReference[i])))
			{
			lineInfoDiff = ETrue;
RDebug::Printf("fail: flag mismatch for line number %d: test flags:%d, reference flags %d", i, infoUnderTest[i].iFlags, infoReference[i].iFlags);
		  	//write out the test text contents, and the reference text contents
  			iFileOutput.WriteOperationDetails(aDesc);
  			iFileOutput.WriteVisibleTestLinesL(*contentsUnderTest);
  			iFileOutput.WriteVisibleReferenceLinesL(*contentsReference);
  			break;
			}
		}
	for (TInt i = 0; i < lineCountForTest; i++)
		{
		TPtrC lineUnderTest = (*contentsUnderTest)[i];
		TPtrC lineReference = (*contentsReference)[i];
		if(!(lineUnderTest==lineReference))
			{
			iFailLineNumberInPara = infoUnderTest[i].iLineInPar;
			iFailLineNumber = infoUnderTest[i].iLineNumber;
RDebug::Printf("fail:line contents mismatch: test line: %S, reference line: %S", &lineUnderTest, &lineReference);
		  	//write out the test text contents, and the reference text contents
  			iFileOutput.WriteOperationDetails(aDesc);
  			iFileOutput.WriteVisibleTestLinesL(*contentsUnderTest);
  			iFileOutput.WriteVisibleReferenceLinesL(*contentsReference);
			CleanupStack::PopAndDestroy(4);
			return EFalse;
			}
		}
	CleanupStack::PopAndDestroy(4);
	if (lineCountDiff || lineInfoDiff)
		{
		return EFalse;
		}
	return ETrue;
	}
	
TBool CBandValidator::GetVisibleLineInfosL(const CTextView& aView, RArray<TTmLineInfo>& aVisibleLines)
	{
    CleanupClosePushL(aVisibleLines);
	const CTextLayout* layout = aView.Layout();
	__ASSERT_ALWAYS(layout, User::Invariant());
	const CTmTextLayout& tmLayout = layout->TagmaTextLayout();
//get first visible line
//this function usefully returns 0 if the line is not found...that might be a problem
	TInt firstLineNumber = 0;
	TTmLineInfo startLine;	
	if(!(tmLayout.LineNumberToLine(firstLineNumber, startLine)))
		{
        CleanupStack::Pop(&aVisibleLines);
		return EFalse;
		}
//get last visible line
	TPoint bottom(0, (aView.ViewRect().Height()-1));//-1 is a not nice.
	TTmPosInfo2 endPosInfo;
	TTmLineInfo endLine;	
	if(!(layout->FindXyPos(bottom, endPosInfo, &endLine)))
		{
		//if we can't find any formatting at the bottom of the view rectangle,
		//this may be because we're at the end of the document. 
		//Figure out if this is so
		if(tmLayout.EndChar() >= tmLayout.Source()->DocumentLength())
			{
			//it is, so just get the last formatted line
			if(!(tmLayout.LineNumberToLine((tmLayout.Lines())-1, endLine)))
				{
                CleanupStack::Pop(&aVisibleLines);
				return EFalse;
				}
			}
		else
			{
            CleanupStack::Pop(&aVisibleLines);
			return EFalse;
			}
		}
//add first, last, and everything in between
	User::LeaveIfError(aVisibleLines.Append(startLine));
	TInt lineNumber = startLine.iLineNumber+1;
	TInt lastLineNumber = endLine.iLineNumber;
	while (lineNumber <= lastLineNumber)
		{
		TTmLineInfo lineInfo;
		if(!(tmLayout.LineNumberToLine(lineNumber, lineInfo)))
			{
            CleanupStack::Pop(&aVisibleLines);
			return EFalse;
			}
		User::LeaveIfError(aVisibleLines.Append(lineInfo));
		lineNumber++;
		}
	
	CleanupStack::Pop(&aVisibleLines);
	return ETrue;	
	}
	
TBool CBandValidator::GetReferenceLineInfosL(const RArray<TTmLineInfo>& aVisibleLines, RArray<TTmLineInfo>& aReferenceLineInfos)
	{
	aReferenceLineInfos.Reset();
	TInt lineCount = aVisibleLines.Count();
	const CTmTextLayout& tmLayoutReference = iReferenceView.Layout()->TagmaTextLayout();	
	for (TInt i = 0; i < lineCount; i++)
		{
		TInt lineStartPos = aVisibleLines[i].iStart;
		TTmDocPos pos(lineStartPos, ETrue);
		TTmPosInfo2 ignore;
		TTmLineInfo referenceLine;
		if(!(tmLayoutReference.FindDocPos(pos, ignore, referenceLine)))
			{
			return EFalse;
			}
		User::LeaveIfError(aReferenceLineInfos.Append(referenceLine));
		}
	return ETrue;
	}
	
void CBandValidator::GetLineContentsL(const CTextView& aView, const RArray<TTmLineInfo>& aLineInfos, CDesCArray& aLineContents)
	{
//get text contents of test line
	const CTextLayout* layout = aView.Layout();
	__ASSERT_ALWAYS(layout, User::Invariant());
	const CTmTextLayout& tmTestLayout = layout->TagmaTextLayout();
	
	TInt lineCount = aLineInfos.Count();	
	for (TInt i = 0; i < lineCount; i++)
		{
		TBuf<256> lineContent;
		TInt needed = 0;		
		tmTestLayout.GetDisplayedTextL(aLineInfos[i].iLineNumber, lineContent, needed);
		__ASSERT_ALWAYS(needed <= lineContent.Length(), User::Invariant());
		aLineContents.AppendL(lineContent);
		}	
	}
	

