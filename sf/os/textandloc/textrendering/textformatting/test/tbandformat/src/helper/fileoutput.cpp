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
#include "bandtestfileoutput.h"
#include "bandvalidator.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

#ifdef __WINS__
	_LIT(KTestErrorFileName, "c:\\bandtests_testfile.txt");
	_LIT(KReferenceErrorFileName, "c:\\bandtests_referencefile.txt");	
#else
	_LIT(KTestErrorFileName, "e:\\bandtests_testfile.txt");
	_LIT(KReferenceErrorFileName, "e:\\bandtests_referencefile.txt");
#endif

#ifdef __WINS__
	_LIT(KTestFileName, "c:\\bandtests.txt");	
#else
	_LIT(KTestFileName, "e:\\bandtests.txt");
#endif

_LIT8(KCR, "\r\n");

EXPORT_C CFileOutput* CFileOutput::NewL()
	{
	CFileOutput* self = new(ELeave) CFileOutput;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CFileOutput::CFileOutput()
	{
	}
	
CFileOutput::~CFileOutput()
	{
	iFile.Close();
	iTestErrorFile.Close();
	iReferenceErrorFile.Close();
	iFs.Close();
	}

void CFileOutput::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iTestErrorFile.Replace(iFs, KTestErrorFileName, EFileShareExclusive));
	User::LeaveIfError(iReferenceErrorFile.Replace(iFs, KReferenceErrorFileName, EFileShareExclusive));
	User::LeaveIfError(iFile.Replace(iFs, KTestFileName, EFileShareAny));
	}	

void CFileOutput::WriteDocumentAttributesL(const CTextView& aView)
	{
	const CTextLayout& layout = *(aView.Layout());
	const CTmTextLayout& tmLayout = layout.TagmaTextLayout();
	
	TInt sc = tmLayout.StartChar();
	TInt ec = tmLayout.EndChar();
	TInt pCount = tmLayout.Paragraphs();
	TInt lCount = tmLayout.Lines();
	TInt firstLine = 0;
	TInt lastLine = firstLine+lCount-1;

	iFile.Write(KCR);
	iFile.Write(KCR);
	iFile.Write(KCR);

	TBuf8<256> info;
	info.Format(_L8("start: %d, end: %d, pars: %d, lines: %d"), sc, ec, pCount, lCount);
	iFile.Write(info);

	iFile.Write(KCR);
	iFile.Write(KCR);
	
	TTmLineInfo firstLineInfo;
	tmLayout.LineNumberToLine(firstLine, firstLineInfo);
	
	TTmLineInfo lastLineInfo;
	tmLayout.LineNumberToLine(lastLine, lastLineInfo);

	TInt needed = 0;		
	TInt currentLine = firstLine;
	TInt parNumber = 1;

	HBufC8* lineInfoOutput = HBufC8::NewLC(2048);
	HBufC8* lineContentsOutput = HBufC8::NewLC(2048);

	TPtr8 pLineInfoOutput = lineInfoOutput->Des();
	TPtr8 pLineContentsOutput = lineContentsOutput->Des();
	

	while(currentLine < lastLine)
		{
		TTmLineInfo currentLineInfo;
		tmLayout.LineNumberToLine(currentLine, currentLineInfo);
		if(currentLineInfo.iFlags & TTmLineInfo::EParStart)
			{
			TBuf8<16> buf;
			buf.Format(_L8("Paragraph %d\r\n"), parNumber);
			pLineContentsOutput.Append(buf);
			}
		TBuf<64> lineContents;
		tmLayout.GetDisplayedTextL(currentLine, lineContents, needed);
		ASSERT(needed <= lineContents.Length());
		pLineContentsOutput.Append(lineContents.Collapse());
		pLineContentsOutput.Append(KCR);
		
		TBuf8<64> lineInfo;
		lineInfo.Format(_L8("Line number:%d, first char pos:%d, last char pos:%d"), currentLine, currentLineInfo.iStart, currentLineInfo.iEnd);
		pLineInfoOutput.Append(lineInfo);
		if(lineContents.Length() > 0)
			{
			TBuf<1> firstChar = lineContents.Left(1);
			TBuf<1> lastChar = lineContents.Right(1);
			TBuf8<1> firstChar8 = firstChar.Collapse();
			TBuf8<1> lastChar8 = lastChar.Collapse();
			
			lineInfo.Format(_L8(", first char:%S, last char:%S\r\n"), &firstChar8, &lastChar8);
			pLineInfoOutput.Append(lineInfo);
			}	
		if(currentLineInfo.iFlags & TTmLineInfo::EParEnd)
			{
			parNumber++;			
			}
		currentLine++;
		}
	iFile.Write(*lineInfoOutput);
	iFile.Write(*lineContentsOutput);
	
	CleanupStack::PopAndDestroy(2);
	
	iFile.Write(KCR);
	}
	
void CFileOutput::WriteDocumentPositionsL(const RArray<TInt>& aPositions)
	{
	iFile.Write(_L8("Document positions:\r\n"));	
	for (TInt i = 0; i < aPositions.Count(); i++)
		{
		TInt pos = aPositions[i];
		TBuf8<8> buf;
		buf.Format(_L8("%d, "), pos);
		iFile.Write(buf);
		if(((i+1)%3) == 0)
			{
			iFile.Write(KCR);	
			}
		}
	}

void CFileOutput::WriteOperationDetails(const TDesC& aDesc)
	{
	TBuf<256> des;
	TBuf8<8> cr;
	des = aDesc;
	TPtr8 des8(des.Collapse());
	cr.Format(_L8("\r\n"));
	iFile.Write(des8);
	iFile.Write(cr);
	iFile.Write(cr);
	iFile.Flush();
	}

void CFileOutput::WriteVisibleTestLinesL(const CDesCArray& aTestLines)
	{
	WriteLinesL(aTestLines, iTestErrorFile);
	iTestErrorFile.Flush();
	}

void CFileOutput::WriteVisibleReferenceLinesL(const CDesCArray& aReferenceLines)
	{
	WriteLinesL(aReferenceLines, iReferenceErrorFile);
	iReferenceErrorFile.Flush();
	}

void CFileOutput::WriteLinesL(const CDesCArray& aLines, RFile& aFile)
	{
	TBuf8<8> cr;
	TBuf<256> line;
	cr.Format(_L8("\r\n"));

	TInt count = aLines.Count();
	for (TInt i = 0; i < count; i++)
		{
		TPtrC line16 = aLines[i];
		line = line16;
		aFile.Write(line.Collapse());
		aFile.Write(cr);
		}
	aFile.Write(cr);
	}

