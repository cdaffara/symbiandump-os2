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
#include <badesca.h>
#include <txtrich.h>
#include <frmtlay.h>

#include "bandvalidator.h"
#include "bandmaintainer.h"
#include "bandtestactions.h"
#include "bandtestdocument.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

EXPORT_C TDocumentSpec::TDocumentSpec(const MDesCArray& aTextContents, TInt aTopOfBand)
	:iContents(aTextContents), iTopOfBand(aTopOfBand)
	{
	__ASSERT_ALWAYS((aTopOfBand==0) || (aTopOfBand < aTextContents.MdcaCount()), User::Invariant());
	}
	
EXPORT_C void TDocumentSpec::SetupDocumentL(CRichText& aRichText, CBandMaintainer& aMaintainer/*, TBool aWholeParagraph*/)
	{
	aRichText.Reset();
	aMaintainer.InitialiseViewL();	
	TChar paragraphDelimiter(0x2029);
	for(TInt i = 0; i < iContents.MdcaCount(); i++)
		{
		aRichText.InsertL(aRichText.DocumentLength(), iContents.MdcaPoint(i));
		aRichText.InsertL(aRichText.DocumentLength(), paragraphDelimiter);	
		}
	aMaintainer.InitialiseViewL();
	const CTmTextLayout& layout = aMaintainer.TmLayoutUnderTest();	
//scroll until iTopOfBand paragraphs have been shorn off the top of the band
	TInt paragraphsToLose = iTopOfBand;
	while (paragraphsToLose > 0)
		{
		//figure out how tall the top paragraph is
		const CTextLayout& formLayout = aMaintainer.LayoutUnderTest();
		const CTmTextLayout& tmLayout = aMaintainer.TmLayoutUnderTest();

		TInt firstParHeight = 0;
		TInt lineNumber = formLayout.FirstLineInBand();
		TTmLineInfo line;
		TBool found = tmLayout.LineNumberToLine(lineNumber, line);
		__ASSERT_ALWAYS(found, User::Invariant());
		firstParHeight += line.iOuterRect.Height();
		while(!(line.iFlags & TTmLineInfo::EParEnd) && found)
			{
			lineNumber++;
			found = tmLayout.LineNumberToLine(lineNumber, line);
			if(found)
				{
				firstParHeight += line.iOuterRect.Height();
				}
			}
		//scroll down by that amount
		TInt scrollBy = -firstParHeight;
		aMaintainer.ScrollDisplayPixelsL(scrollBy);
	
		paragraphsToLose--;
		}
	}

/*
Given an array of lines that constitute a paragraph or part of a paragraph, 
this method retrieves selected lines from it that will enable us
to test the different code paths for the reformatting algorithm. 
In general, for each paragraph, lines of interest are:
(1) special cases:
	- the first line
	- the second line
	- the penultimate line
	- the last line
(2) normal cases
- at least 2 lines from the middle

So:
- if the paragraph contains 6 lines or fewer, we take them all, giving us the 4 special cases,
and two lines from the middle
- otherwise, we take the 4 special cases, and a couple from the middle

*/	
void SelectLinesL(const RArray<TTmLineInfo>& aSource, RArray<TTmLineInfo>& aLinesToTest)
	{
	//if there are 6 lines or fewer, we need them all
	if	((aSource.Count()) < 7)
		{
		for(TInt i = 0; i < aSource.Count(); i++)
			{
			User::LeaveIfError(aLinesToTest.Append(aSource[i]));
			}
		}
	else
		{
		//otherwise, get the first two
		User::LeaveIfError(aLinesToTest.Append(aSource[0]));
		User::LeaveIfError(aLinesToTest.Append(aSource[1]));
		//and the last two
		User::LeaveIfError(aLinesToTest.Append(aSource[aSource.Count()-2]));
		User::LeaveIfError(aLinesToTest.Append(aSource[aSource.Count()-1]));
		//a couple from the middle
		TInt theMiddle = aSource.Count()/2;
		User::LeaveIfError(aLinesToTest.Append(aSource[theMiddle]));
		User::LeaveIfError(aLinesToTest.Append(aSource[theMiddle+1]));	
		}		
	}
	

void SelectLinesToTestL(const RArray<TTmLineInfo>& aSource, RArray<TTmLineInfo>& aLinesToTest)
	{
	TInt lineCount = aSource.Count();
	RArray<TTmLineInfo> linesInThisParagraph; 
	CleanupClosePushL(linesInThisParagraph);
	
	for (TInt i = 0; i < lineCount; i++)
		{
		TTmLineInfo thisLine = aSource[i];
		User::LeaveIfError(linesInThisParagraph.Append(thisLine));
		if(thisLine.iFlags & TTmLineInfo::EParEnd)
			{
			SelectLinesL(linesInThisParagraph, aLinesToTest);
			linesInThisParagraph.Reset();
			}
		}
		//call it once more in case the paragraph extends beyond the end of the band
	SelectLinesL(linesInThisParagraph, aLinesToTest);
	
	CleanupStack::PopAndDestroy();
	}
	

EXPORT_C void GetVisibleLinesL(const CTextLayout& aLayout, RArray<TTmLineInfo>& aVisibleLines)
	{
	TInt startPos, endPos, numChars;
	TTmPosInfo2 ignore;
	TTmLineInfo lastLine;
	TInt lineNumber = aLayout.FirstLineInBand();
	numChars = aLayout.PosRangeInBand(startPos);
	endPos=startPos+numChars-1;
	RDebug::Print(_L("Start pos is:%d num of chars is:%d and end pos is:%d...."),startPos,numChars,endPos);
	TTmDocPos pos(endPos, ETrue);
	TTmDocPosSpec posSpec(endPos,TTmDocPosSpec::ELeading);
	TBool result = aLayout.PosInBand(pos, &lastLine);
	if(!result)
		{
	
		numChars = aLayout.PosRangeInBand(startPos);
		result = aLayout.PosInBand(pos, &lastLine);
		__ASSERT_ALWAYS(result, User::Invariant());
		}
		
	for(; lineNumber < lastLine.iLineNumber; lineNumber++)
		{
		TTmLineInfo line;
		TBool foundLine = aLayout.TagmaTextLayout().LineNumberToLine(lineNumber, line);
		__ASSERT_ALWAYS(foundLine, User::Invariant());
		User::LeaveIfError(aVisibleLines.Append(line));
		}	
	}
	
EXPORT_C void GetLinesInBandL(const CTmTextLayout& aTmLayout, RArray<TTmLineInfo>& aLinesInBand)
	{
  	TTmDocPos pos(aTmLayout.StartChar(), ETrue);
   	TTmPosInfo2 ignore;
  	TTmLineInfo firstLine;
  	TBool foundDocPos = aTmLayout.FindDocPos(pos, ignore, firstLine);
  	__ASSERT_ALWAYS(foundDocPos, User::Invariant());
  	TInt lineNumber = firstLine.iLineNumber;
  	TInt lastLineNumber = lineNumber + aTmLayout.Lines();		
  	for(; lineNumber < lastLineNumber; lineNumber++)
   		{
   		TTmLineInfo line;
  		TBool foundLine = aTmLayout.LineNumberToLine(lineNumber, line);
  		__ASSERT_ALWAYS(foundLine, User::Invariant());
   		User::LeaveIfError(aLinesInBand.Append(line));
   		}	
   	}
	
EXPORT_C void PositionsToTestL(const RArray<TTmLineInfo>& aLines, RArray<TInt>& aPositions)
	{
	RArray<TTmLineInfo> selectedLines;
	CleanupClosePushL(selectedLines);
	
	SelectLinesToTestL(aLines, selectedLines);
	
	for(TInt i = 0; i < selectedLines.Count(); i++)
		{
		TTmLineInfo lineInfo = selectedLines[i];
		TInt start = lineInfo.iStart;
		TInt end = lineInfo.iEnd == lineInfo.iStart ? lineInfo.iEnd: lineInfo.iEnd-1;
		TInt middle = lineInfo.iStart+((lineInfo.iEnd - lineInfo.iStart)/2);
		User::LeaveIfError(aPositions.InsertInOrderAllowRepeats(start));
		User::LeaveIfError(aPositions.InsertInOrderAllowRepeats(middle));
		User::LeaveIfError(aPositions.InsertInOrderAllowRepeats(end));	
		}
				
	CleanupStack::PopAndDestroy();
	}
	
EXPORT_C void ExecuteTestL(CRichText& aRichText, CBandMaintainer& aMaintainer, 
	TBandTestAction& aAction, TDocumentSpec& aDocumentSpec, RArray<TInt>& aPositions)
	{
	aDocumentSpec.SetupDocumentL(aRichText, aMaintainer);
	for (TInt i = 0; i < aPositions.Count(); i++)
		{
		TInt editPos = aPositions[i];
		for (TInt j = i; j < aPositions.Count(); j++)
			{
			TInt editLength = (aPositions[j] - editPos);
			TInt documentLength = aMaintainer.TmLayoutUnderTest().Source()->DocumentLength();
			TInt editEndPos = editLength + editPos;
			TInt lastFormattedChar = aMaintainer.TmLayoutUnderTest().EndChar();
			if ((editEndPos < documentLength) && ((editEndPos+1) < lastFormattedChar))
				{
				editLength++;
				}
			TBandTestAction::TTestResult result = aAction.ExecuteL(editPos, editLength);
			__ASSERT_ALWAYS(result == TBandTestAction::EPassed, User::Invariant());	
			aDocumentSpec.SetupDocumentL(aRichText, aMaintainer);
			}
		}
	}
	

EXPORT_C void InsertTextL(CRichText& aRichText, CBandMaintainer& aMaintainer, 
	CBandValidator& aValidator, TDocumentSpec& aDocumentSpec, const TDesC& aTextToInsert, RArray<TInt>& aPositions)
	{
	aDocumentSpec.SetupDocumentL(aRichText, aMaintainer);
	const CTextLayout& layout = aMaintainer.LayoutUnderTest();	

	for (TInt i = 0; i < aPositions.Count(); i++)
		{ 
		TInsertText insertText(aMaintainer, aRichText, aValidator, aTextToInsert);
		TBandTestAction::TTestResult result = insertText.ExecuteL(aPositions[i], aTextToInsert.Length());
		__ASSERT_ALWAYS(result == TBandTestAction::EPassed, User::Invariant());	
		aDocumentSpec.SetupDocumentL(aRichText, aMaintainer);
		}
	}

