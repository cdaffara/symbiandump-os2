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
* TestLayout.cpp test jig for CTmTextLayout
*
*/


#include "TestLayout.h"
#include "TGraphicsContext.h"
#include "TMINTERP.H"

#include <txtrich.h>

#include <e32test.h>
/**
Constructor.
@param aRichText
	Text to be formatted.
@param aDeviceMap
	Formatting and drawing device.
@internalComponent
*/
CETextSource::CETextSource(const CRichText& aRichText,
	MGraphicsDeviceMap* aDeviceMap)
	: iRichText(&aRichText), iDeviceMap(aDeviceMap)
	{
	}

/**
Gets the length of the document.
@return
	The number of Unicode UTF16 codes in the document.
@internalComponent
*/
TInt CETextSource::DocumentLength() const
	{
	return iRichText->DocumentLength();
	}

/**
Gets a run of text of constant formatting.
@param aPos
	Start of the text to get.
@param aText
	Segment of text to return.
@param aFormat
	Format of the segment of text.
@internalComponent
*/
void CETextSource::GetText(TInt aPos, TPtrC& aText, TTmCharFormat& aFormat) const
	{
	TCharFormat characterFormat;
	iRichText->GetChars(aText, characterFormat, aPos);
	aFormat = characterFormat;
	}

/**
Gets the format of the paragraph at aPos.
@param aPos
	Character index.
@param aFormat
	Returns the format.
@internalComponent
*/
void CETextSource::GetParagraphFormatL(TInt aPos, RTmParFormat& aFormat) const
	{
	CParaFormat* format = CParaFormat::NewLC();
	iRichText->GetParagraphFormatL(format, aPos);
	aFormat.CopyL(*format);
	CleanupStack::PopAndDestroy(format);
	}

/**
Returns the start of the paragraph containing aPos.
@param aPos
	Character index.
@return
	Character index of the start of the paragraph.
@internalComponent
*/
TInt CETextSource::ParagraphStart(TInt aPos) const
	{
	iRichText->ParagraphNumberForPos(aPos);
	return aPos;
	}

/**
Gets a pointer to the picture at aPos.
@param aPos
	Character index.
@return
	A pointer to the picture at aPos if there is one. 0 otherwise.
@internalComponent
*/
CPicture* CETextSource::PictureL(TInt aPos) const
	{
	return iRichText->PictureHandleL(aPos);
	}

/**
Creates a new CTestTmTextLayout.
@param aText
	Text to format.
@param aWrapWidth
	Width to format at.
@return
	The newly constructed object.
@internalComponent
*/
CTestTmTextLayout* CTestTmTextLayout::NewLC(
	const CRichText& aText, TInt aWrapWidth)
	{
	CTestTmTextLayout* r = new CTestTmTextLayout;
	CleanupStack::PushL(r);
	r->ConstructL(aText, aWrapWidth);
	return r;
	}

/**
Creates a new CTestTmTextLayout.
@param aText
	Text to format.
@param aWrapWidth
	Width to format at.
@param aTransliterate
	Function to transform aText with if required. 0 if no transformation is to
	take place.
@return
	The newly constructed object.
@internalComponent
*/
CTestTmTextLayout* CTestTmTextLayout::NewLC(
	const TDesC& aText, TInt aWrapWidth, TransliterationFn* aTransliterate)
	{
	CTestTmTextLayout* r = new CTestTmTextLayout;
	CleanupStack::PushL(r);
	r->iParagraphFormatLayer = CParaFormatLayer::NewL();
	r->iCharacterFormatLayer = CCharFormatLayer::NewL();
	HBufC* buf = HBufC::NewLC(aText.Length());
	TPtr des = buf->Des();
	if (aTransliterate)
		aTransliterate(aText, des);
	else
		des.Copy(aText);
	r->iRichText = CRichText::NewL(
		r->iParagraphFormatLayer, r->iCharacterFormatLayer);
	r->iRichText->InsertL(0, *buf);
	CleanupStack::PopAndDestroy(buf);
	r->ConstructL(*r->iRichText, aWrapWidth);
	return r;
	}

/**
Destructor.
@internalComponent
*/
CTestTmTextLayout::~CTestTmTextLayout()
	{
	delete iLayout;
	delete iSource;
	delete iDevice;
	delete iRichText;
	delete iCharacterFormatLayer;
	delete iParagraphFormatLayer;
	}

/**
Constructs a new CTestTmTextLayout.
@param aText
	Text to format.
@param aWrapWidth
	Width to format at.
@return
	The newly constructed object.
@internalComponent
*/
void CTestTmTextLayout::ConstructL(const CRichText& aText, TInt aWrapWidth)
	{
	iLayout = new (ELeave) CTmTextLayout;
	TSize screen(aWrapWidth, 100);
	iDevice = CTestGraphicsDevice::NewL(screen, 0);
	iSource = new (ELeave) CETextSource(aText, iDevice);
	iFormatParam.iWrapWidth = aWrapWidth;
	iFormatParam.iFlags = TTmFormatParamBase::EWrap;
	iLayout->SetTextL(*iSource, iFormatParam);
	}

/**
Reformats the text.
@param aIn
	Reformatting parameters.
@param aOut
	Reformatting result.
@internalComponent
*/
void CTestTmTextLayout::FormatL(const TTmReformatParam& aIn, TTmReformatResult& aOut)
	{
	iLayout->FormatL(iFormatParam, aIn, aOut);
	}
	
/**
@SYMTestCaseID          SYSLIB-FORM-UT-1887
@SYMTestCaseDesc        Testing CTmTextLayout::FindAdjacentChunks() API
@SYMTestPriority        Low
@SYMTestActions         Tests by finding the text chunks adjoining a given document position. When pos=-1,FindAdjacentChunks() returns false
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void CTestTmTextLayout::TestAdjacentChunks()
	{
	RTest test(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-1887  FindAdjacentChunks"));
	for (TInt pos = -1; pos != 4; ++pos)
		{
		for (TInt type = 0; type != 4; ++type)
			{
			TTmDocPosSpec posSpec(pos,static_cast<TTmDocPosSpec::TType>(type));
			CTmTextLayout::TTmChunkDescription left;
			CTmTextLayout::TTmChunkDescription right;
			TBool result = iLayout->FindAdjacentChunks(posSpec,left,right);
			if(pos==-1)
			test(result==0);
			}
		}
		test.Close();
	}
