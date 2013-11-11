/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "T_RTPAR.H"

////////////////////////////////////////////
// CRichTextReader
////////////////////////////////////////////

CRichTextReader* CRichTextReader::NewL()
	{
	CRichTextReader* self=new(ELeave) CRichTextReader;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


CRichTextReader::CRichTextReader()
	{
	// init variables
	iParaStart = 0;
	iConsoleExists = EFalse;
	}
	 

void CRichTextReader::ConstructL()
	{ 
	// construct PML writer
	iPMLWriter = CPMLWriter::NewL();
	
	// construct RichText bits
	iThisParaFormat = CParaFormat::NewL();
	iPrevParaFormat = CParaFormat::NewL();
	}


void CRichTextReader::Destruct()
	{
	// iConsole.Destroy();
	Adt::Destroy(iPMLWriter);
	Adt::Destroy(iThisParaFormat);
	Adt::Destroy(iPrevParaFormat);
	if (iBorder)
		delete(iBorder);	// only delete if it hasn't been used (if it has RT destroys it automatically)
	}


CBufSeg* CRichTextReader::ConvertRichText(CRichText* aRichTextDoc)
// Quiet version of the RichText->PML parser
// Takes RT doc passed in, reads its global format,
// then reads the character and paragraph formats phrase by phrase
	{
	iRichTextDoc = aRichTextDoc;
	iDocLength = iRichTextDoc->DocumentLength();
	SenseGlobalFormat();

	TInt nextPhrase;
	TInt readPos = 0;
	 while (readPos < iDocLength)
	 	{
		nextPhrase = TranslatePhrase(readPos);
		readPos += nextPhrase;
		}
	iPMLWriter->Delete(1);		// remove end-of-document paragraph delimiter
	return iPMLWriter->ReturnPmlDoc();
	}


CBufSeg* CRichTextReader::ConvertRichText(CRichText* aRichTextDoc, RConsole aConsole)
// Version of the RichText->PML parser with console output (primarily for debugging)
// Takes RT doc passed in, reads its global format,
// then reads the character and paragraph formats phrase by phrase
	{
	iConsoleExists = ETrue;
	iRichTextDoc = aRichTextDoc;
	iConsole = aConsole;
	iDocLength = iRichTextDoc->DocumentLength();
	SenseGlobalFormat();

	TInt nextPhrase;
	TInt readPos = 0;
	while (readPos <= iDocLength)
	 	{
		nextPhrase = TranslatePhrase(readPos);
		readPos += nextPhrase;
		}
	iPMLWriter->Delete(1);
	iPMLWriter->Output(iConsole);	// output PML doc to screen
	return iPMLWriter->ReturnPmlDoc();
	}


void CRichTextReader::SenseGlobalFormat()
// Senses global format & writes it to PML
	{  
	// construct rich text bits
	const CParaFormatLayer* globalParaFormatLayer;	   	// initialised with factory settings
	const CCharFormatLayer* globalCharFormatLayer;
	CParaFormat* globalParaFormat = CParaFormat::NewL();
	TCharFormat globalCharFormat;

	// Sense global format
	globalParaFormatLayer = iRichTextDoc->SenseGlobalParaFormatLayer();
	globalParaFormatLayer->SenseEffectiveL(globalParaFormat);
	globalCharFormatLayer = iRichTextDoc->SenseGlobalCharFormatLayer();
	globalCharFormatLayer->SenseEffective(globalCharFormat);

	// now set it
	iPMLWriter->SetTag(EGlobal, ETagStart);		// output <G
 	CompareParaToFactory(globalParaFormat);	// step through format, adding all != factory settings
	CompareCharToFactory(globalCharFormat);	// ...
	iPMLWriter->SetTag(EGlobal, ETagEnd);		// output >\n

	// initialise iPrevXxxxFormat
	globalParaFormatLayer->SenseEffectiveL(iPrevParaFormat);
	globalCharFormatLayer->SenseEffective(iPrevCharFormat);

	// destroy rich text bits
	Adt::Destroy(globalParaFormat);
	} 


TInt CRichTextReader::TranslatePhrase(TInt aReadPos)
// Output any new formatting being applied to this phrase
// First check whether this phrase is the start of a new paragraph
// If so check the paragraph format
// Next check the character formatting of the phrase
	{
	CPicture* pic; // dummy pointer for senseChars
	TPtrC currentPhrase;
	TUint pmlWritePos;

	TInt currentParaStart = aReadPos;		

	// paragraph formatting
	if (currentParaStart != 0)			  // doesn't work for position=0 -- Duncan!!
		iRichTextDoc->ParagraphStart(currentParaStart);
	if ((currentParaStart > iParaStart)||(aReadPos == 0)) // is it a new paragraph?
		{
		iParaStart = currentParaStart;
		// delete para delimiter
		if (aReadPos > 0)
			iPMLWriter->Delete(1); // deletes 1 chars previous to the current insert pos
		// add para tag
		iPMLWriter->SetTag(EParagraph, ETagStart);
		iRichTextDoc->SenseParagraphFormatL(aReadPos,iThisParaFormat); // get para format
		CompareParaFormats(); // compare to previous paragraph to get changes
		iRichTextDoc->SenseParagraphFormatL(aReadPos,iPrevParaFormat); // set prevParaFormat to current & copy compound attributes
		iPrevParaFormat->iTopBorder=CopyBorderL(iThisParaFormat->iTopBorder,iPrevParaFormat->iTopBorder);
		iPrevParaFormat->iBottomBorder=CopyBorderL(iThisParaFormat->iBottomBorder,iPrevParaFormat->iBottomBorder);
		iPrevParaFormat->iLeftBorder=CopyBorderL(iThisParaFormat->iLeftBorder,iPrevParaFormat->iLeftBorder);
		iPrevParaFormat->iRightBorder=CopyBorderL(iThisParaFormat->iRightBorder,iPrevParaFormat->iRightBorder);
		iPrevParaFormat->iBullet=CopyBulletL(iThisParaFormat->iBullet,iPrevParaFormat->iBullet);
		iPMLWriter->SetTag(EParagraph, ETagEnd);

		}

	// character formatting
	iPMLWriter->SetTag(ECharacter, ETagStart);
	pmlWritePos = iPMLWriter->WritePos();
	iRichTextDoc->SenseChars(aReadPos,currentPhrase,iThisCharFormat,pic);	// get char format
	CompareCharFormats();	// compare to previous
	iRichTextDoc->SenseChars(aReadPos,currentPhrase,iPrevCharFormat,pic);	// get char format
	if (pmlWritePos == iPMLWriter->WritePos())
		iPMLWriter->Delete(3);		// delete tag start if tag is empty
	else
		iPMLWriter->SetTag(ECharacter, ETagEnd); // else close tag

	iPMLWriter->Insert(currentPhrase);	// output text of phrase to PML
	return currentPhrase.Length();		// return relative pos of start of next phrase
	}


TParaBorder* CRichTextReader::CopyBorderL(const TParaBorder* aFrom,TParaBorder* aTo)
// copies one paragraph border to another, creating or destroying where necessary
	{
	if (!aFrom)
		{
		if (aTo)
			delete(aTo);
		return NULL;  // No border cell in the original
		}
	else
		{
		if (!aTo)
			aTo = new(ELeave) TParaBorder;
		*aTo = *aFrom;
		return aTo;
		}
	}
	

TBullet* CRichTextReader::CopyBulletL(const TBullet* aFrom,TBullet* aTo)
// copies one bullet to another, creating or destroying where necessary
	{
	if (!aFrom)
		{
		if (aTo)
			delete(aTo);
		return NULL;  // No bullet cell in the original
		}
	else
		{
		if (!aTo)
			aTo = new(ELeave) TBullet;
		*aTo = *aFrom;
		return aTo;
		}
	}
	

void CRichTextReader::CompareParaToFactory(CParaFormat* aSensedParaFormat)
// Compares the supplied paragraph format to the factory defaults and acts on any differences
	{
	// create a new reference format with factory settings
	CParaFormat* refParaFormat = CParaFormat::NewL();
	// step through, comparing aSensedParaFormat to the reference
	// if any attributes differ, set the differences in the PML doc
	if (aSensedParaFormat->iLanguage != refParaFormat->iLanguage)
		iPMLWriter->SetFormat(EAttParaLanguage, aSensedParaFormat->iLanguage);
	if (aSensedParaFormat->iLeftMargin != refParaFormat->iLeftMargin)
		iPMLWriter->SetFormat(EAttLeftMargin, aSensedParaFormat->iLeftMargin);
	if (aSensedParaFormat->iRightMargin != refParaFormat->iRightMargin)
		iPMLWriter->SetFormat(EAttRightMargin, aSensedParaFormat->iRightMargin);
	if (aSensedParaFormat->iIndent != refParaFormat->iIndent)
		iPMLWriter->SetFormat(EAttIndent, aSensedParaFormat->iIndent);
	if (aSensedParaFormat->iAlignment != refParaFormat->iAlignment)
		iPMLWriter->SetFormat(EAttAlignment, aSensedParaFormat->iAlignment);
	if (aSensedParaFormat->iLineSpacing != refParaFormat->iLineSpacing)
		iPMLWriter->SetFormat(EAttLineSpacing, aSensedParaFormat->iLineSpacing);
	if (aSensedParaFormat->iLineSpacingControl != refParaFormat->iLineSpacingControl)
		iPMLWriter->SetFormat(EAttLineSpacingControl, aSensedParaFormat->iLineSpacingControl);
	if (aSensedParaFormat->iSpaceBefore != refParaFormat->iSpaceBefore)
		iPMLWriter->SetFormat(EAttSpaceBefore, aSensedParaFormat->iSpaceBefore);
	if (aSensedParaFormat->iSpaceAfter != refParaFormat->iSpaceAfter)
		iPMLWriter->SetFormat(EAttSpaceAfter, aSensedParaFormat->iSpaceAfter);
	if (aSensedParaFormat->iKeepTogether != refParaFormat->iKeepTogether)
		iPMLWriter->SetFormat(EAttKeepTogether, aSensedParaFormat->iKeepTogether);
	if (aSensedParaFormat->iKeepWithNext != refParaFormat->iKeepWithNext)
		iPMLWriter->SetFormat(EAttKeepWithNext, aSensedParaFormat->iKeepWithNext);
	if (aSensedParaFormat->iStartNewPage != refParaFormat->iStartNewPage)
		iPMLWriter->SetFormat(EAttStartNewPage, aSensedParaFormat->iStartNewPage);
	if (aSensedParaFormat->iWidowOrphan != refParaFormat->iWidowOrphan)
		iPMLWriter->SetFormat(EAttWidowOrphan, aSensedParaFormat->iWidowOrphan);
	if (aSensedParaFormat->iBorderMargin != refParaFormat->iBorderMargin)
		iPMLWriter->SetFormat(EAttBorderMargin, aSensedParaFormat->iBorderMargin);
	if (aSensedParaFormat->iTopBorder)
		{
		if (refParaFormat->iTopBorder == NULL)
			iPMLWriter->SetFormat(EAttTopBorder, aSensedParaFormat->iTopBorder);
		else if ((aSensedParaFormat->iTopBorder->iLineStyle != refParaFormat->iTopBorder->iLineStyle)
		||(aSensedParaFormat->iTopBorder->iAutoColor != refParaFormat->iTopBorder->iAutoColor)
		||(aSensedParaFormat->iTopBorder->iColor != refParaFormat->iTopBorder->iColor))
			{
			iPMLWriter->SetFormat(EAttTopBorder, aSensedParaFormat->iTopBorder);
			}
		}
	if (aSensedParaFormat->iBottomBorder)
		{  
		if (refParaFormat->iTopBorder == NULL)
			iPMLWriter->SetFormat(EAttBottomBorder, aSensedParaFormat->iBottomBorder);
		else if ((aSensedParaFormat->iBottomBorder->iLineStyle != refParaFormat->iBottomBorder->iLineStyle)
		||(aSensedParaFormat->iBottomBorder->iAutoColor != refParaFormat->iBottomBorder->iAutoColor)
		||(aSensedParaFormat->iBottomBorder->iColor != refParaFormat->iBottomBorder->iColor))
			{
			iPMLWriter->SetFormat(EAttBottomBorder, aSensedParaFormat->iBottomBorder);
			}
		}
	if (aSensedParaFormat->iLeftBorder)
		{  
		if (refParaFormat->iTopBorder == NULL)
			iPMLWriter->SetFormat(EAttLeftBorder, aSensedParaFormat->iLeftBorder);
		else if ((aSensedParaFormat->iLeftBorder->iLineStyle != refParaFormat->iLeftBorder->iLineStyle)
		||(aSensedParaFormat->iLeftBorder->iAutoColor != refParaFormat->iLeftBorder->iAutoColor)
		||(aSensedParaFormat->iLeftBorder->iColor != refParaFormat->iLeftBorder->iColor))
			{
			iPMLWriter->SetFormat(EAttLeftBorder, aSensedParaFormat->iLeftBorder);
			}
		}
	if (aSensedParaFormat->iRightBorder)
		{  
		if (refParaFormat->iTopBorder == NULL)
			iPMLWriter->SetFormat(EAttRightBorder, aSensedParaFormat->iRightBorder);
		else if ((aSensedParaFormat->iRightBorder->iLineStyle != refParaFormat->iRightBorder->iLineStyle)
		||(aSensedParaFormat->iRightBorder->iAutoColor != refParaFormat->iRightBorder->iAutoColor)
		||(aSensedParaFormat->iRightBorder->iColor != refParaFormat->iRightBorder->iColor))
			{
			iPMLWriter->SetFormat(EAttRightBorder, aSensedParaFormat->iRightBorder);
			}
		}
	if (aSensedParaFormat->iBullet)
		{  
		if (refParaFormat->iBullet == NULL)
			iPMLWriter->SetFormat(EAttBullet, aSensedParaFormat->iBullet);
		if ((aSensedParaFormat->iBullet->iCharacterCode != refParaFormat->iBullet->iCharacterCode)
		||(aSensedParaFormat->iBullet->iHeight != refParaFormat->iBullet->iHeight)
		||(aSensedParaFormat->iBullet->iTypeface.iName != refParaFormat->iBullet->iTypeface.iName)
		||(aSensedParaFormat->iBullet->iTypeface.iFlags != refParaFormat->iBullet->iTypeface.iFlags))
			{	
			iPMLWriter->SetFormat(EAttBullet, aSensedParaFormat->iBullet);
			}
		}
	if (aSensedParaFormat->iDefaultTabWidth != refParaFormat->iDefaultTabWidth)
		iPMLWriter->SetFormat(EAttDefaultTabWidth, aSensedParaFormat->iDefaultTabWidth);
	CheckTabList(aSensedParaFormat, refParaFormat);	// tabs are sensed separately

	// Destroy ref format
	Adt::Destroy(refParaFormat);
	}

void CRichTextReader::CompareCharToFactory(TCharFormat aSensedCharFormat)
// Compares the supplied character format to the factory defaults and acts on any differences
	{
	// create a new reference format with factory settings
	TCharFormat refCharFormat;
	// step through, comparing aSensedCharFormat to the reference
	// if any attributes differ, set the differences in the PML doc
	if (aSensedCharFormat.iLanguage != refCharFormat.iLanguage)
		iPMLWriter->SetFormat(EAttCharLanguage, aSensedCharFormat.iLanguage);
	if (aSensedCharFormat.iColor != refCharFormat.iColor)
		iPMLWriter->SetFormat(EAttColor, aSensedCharFormat.iColor.RgbToUint());
	if (aSensedCharFormat.iFontSpec.iTypeface.iName != refCharFormat.iFontSpec.iTypeface.iName)
		iPMLWriter->SetFormat(EAttFontTypefaceName, aSensedCharFormat.iFontSpec.iTypeface);
	if (aSensedCharFormat.iFontSpec.iTypeface.iFlags != refCharFormat.iFontSpec.iTypeface.iFlags)
		iPMLWriter->SetFormat(EAttFontTypefaceFlags, aSensedCharFormat.iFontSpec.iTypeface);
	if (aSensedCharFormat.iFontSpec.iHeight != refCharFormat.iFontSpec.iHeight)
		iPMLWriter->SetFormat(EAttFontHeight, aSensedCharFormat.iFontSpec.iHeight);
	if (aSensedCharFormat.iFontSpec.iPosture != refCharFormat.iFontSpec.iPosture)
		iPMLWriter->SetFormat(EAttFontPosture, aSensedCharFormat.iFontSpec.iPosture);
	if (aSensedCharFormat.iFontSpec.iStrokeWeight != refCharFormat.iFontSpec.iStrokeWeight)
		iPMLWriter->SetFormat(EAttFontStrokeWeight, aSensedCharFormat.iFontSpec.iStrokeWeight);
	if (aSensedCharFormat.iFontSpec.iPos != refCharFormat.iFontSpec.iPos)
		iPMLWriter->SetFormat(EAttFontPrintPos, aSensedCharFormat.iFontSpec.iPos);
	if (aSensedCharFormat.iFontSpec.iUnderline != refCharFormat.iFontSpec.iUnderline)
		iPMLWriter->SetFormat((TTextFormatAttribute)EAttFontUnderline, aSensedCharFormat.iFontSpec.iUnderline);
	if (aSensedCharFormat.iFontSpec.iStrikethrough != refCharFormat.iFontSpec.iStrikethrough)
		iPMLWriter->SetFormat(EAttFontStrikethrough, aSensedCharFormat.iFontSpec.iStrikethrough);
	}


void CRichTextReader::CompareParaFormats()
// compares the current para format with the format of the previous para
// any differences are output to PML
	{
	if (iThisParaFormat->iLanguage != iPrevParaFormat->iLanguage)
		iPMLWriter->SetFormat(EAttParaLanguage, iThisParaFormat->iLanguage);
	if (iThisParaFormat->iLeftMargin != iPrevParaFormat->iLeftMargin)
		iPMLWriter->SetFormat(EAttLeftMargin, iThisParaFormat->iLeftMargin);
	if (iThisParaFormat->iRightMargin != iPrevParaFormat->iRightMargin)
		iPMLWriter->SetFormat(EAttRightMargin, iThisParaFormat->iRightMargin);
	if (iThisParaFormat->iIndent != iPrevParaFormat->iIndent)
		iPMLWriter->SetFormat(EAttIndent, iThisParaFormat->iIndent);
	if (iThisParaFormat->iAlignment != iPrevParaFormat->iAlignment)
		iPMLWriter->SetFormat(EAttAlignment, iThisParaFormat->iAlignment);
	if (iThisParaFormat->iLineSpacing != iPrevParaFormat->iLineSpacing)
		iPMLWriter->SetFormat(EAttLineSpacing, iThisParaFormat->iLineSpacing);
	if (iThisParaFormat->iLineSpacingControl != iPrevParaFormat->iLineSpacingControl)
		iPMLWriter->SetFormat(EAttLineSpacingControl, iThisParaFormat->iLineSpacingControl);
	if (iThisParaFormat->iSpaceBefore != iPrevParaFormat->iSpaceBefore)
		iPMLWriter->SetFormat(EAttSpaceBefore, iThisParaFormat->iSpaceBefore);
	if (iThisParaFormat->iSpaceAfter != iPrevParaFormat->iSpaceAfter)
		iPMLWriter->SetFormat(EAttSpaceAfter, iThisParaFormat->iSpaceAfter);
	if (iThisParaFormat->iKeepTogether != iPrevParaFormat->iKeepTogether)
		iPMLWriter->SetFormat(EAttKeepTogether, iThisParaFormat->iKeepTogether);
	if (iThisParaFormat->iKeepWithNext != iPrevParaFormat->iKeepWithNext)
		iPMLWriter->SetFormat(EAttKeepWithNext, iThisParaFormat->iKeepWithNext);
	if (iThisParaFormat->iStartNewPage != iPrevParaFormat->iStartNewPage)
		iPMLWriter->SetFormat(EAttStartNewPage, iThisParaFormat->iStartNewPage);
	if (iThisParaFormat->iWidowOrphan != iPrevParaFormat->iWidowOrphan)
		iPMLWriter->SetFormat(EAttWidowOrphan, iThisParaFormat->iWidowOrphan);
	if (iThisParaFormat->iBorderMargin != iPrevParaFormat->iBorderMargin)
		iPMLWriter->SetFormat(EAttBorderMargin, iThisParaFormat->iBorderMargin);
	if (iThisParaFormat->iTopBorder)
		{
		if (iPrevParaFormat->iTopBorder == NULL)  
			iPMLWriter->SetFormat(EAttTopBorder, iThisParaFormat->iTopBorder);
		else if ((iThisParaFormat->iTopBorder->iLineStyle != iPrevParaFormat->iTopBorder->iLineStyle)
		||(iThisParaFormat->iTopBorder->iAutoColor != iPrevParaFormat->iTopBorder->iAutoColor)
		||(iThisParaFormat->iTopBorder->iColor != iPrevParaFormat->iTopBorder->iColor))
			{
			iPMLWriter->SetFormat(EAttTopBorder, iThisParaFormat->iTopBorder);
			}
		}
	if (iThisParaFormat->iBottomBorder)
		{  
		if (iPrevParaFormat->iBottomBorder == NULL)  
			iPMLWriter->SetFormat(EAttBottomBorder, iThisParaFormat->iBottomBorder);
		else if ((iThisParaFormat->iBottomBorder->iLineStyle != iPrevParaFormat->iBottomBorder->iLineStyle)
		||(iThisParaFormat->iBottomBorder->iAutoColor != iPrevParaFormat->iBottomBorder->iAutoColor)
		||(iThisParaFormat->iBottomBorder->iColor != iPrevParaFormat->iBottomBorder->iColor))
			{
			iPMLWriter->SetFormat(EAttBottomBorder, iThisParaFormat->iBottomBorder);
			}
		}
	if (iThisParaFormat->iLeftBorder)
		{  
		if (iPrevParaFormat->iLeftBorder == NULL)  
			iPMLWriter->SetFormat(EAttLeftBorder, iThisParaFormat->iLeftBorder);
		if ((iThisParaFormat->iLeftBorder->iLineStyle != iPrevParaFormat->iLeftBorder->iLineStyle)
		||(iThisParaFormat->iLeftBorder->iAutoColor != iPrevParaFormat->iLeftBorder->iAutoColor)
		||(iThisParaFormat->iLeftBorder->iColor != iPrevParaFormat->iLeftBorder->iColor))
			{
			iPMLWriter->SetFormat(EAttLeftBorder, iThisParaFormat->iLeftBorder);
			}
		}
	if (iThisParaFormat->iRightBorder)
		{  
		if (iPrevParaFormat->iRightBorder == NULL)  
			iPMLWriter->SetFormat(EAttRightBorder, iThisParaFormat->iRightBorder);
		if ((iThisParaFormat->iRightBorder->iLineStyle != iPrevParaFormat->iRightBorder->iLineStyle)
		||(iThisParaFormat->iRightBorder->iAutoColor != iPrevParaFormat->iRightBorder->iAutoColor)
		||(iThisParaFormat->iRightBorder->iColor != iPrevParaFormat->iRightBorder->iColor))
			{
			iPMLWriter->SetFormat(EAttRightBorder, iThisParaFormat->iRightBorder);
			}
		}
 	if (iThisParaFormat->iBullet)
		{  
		if (iPrevParaFormat->iBullet == NULL)  
			iPMLWriter->SetFormat(EAttBullet, iThisParaFormat->iBullet);
		else if ((iThisParaFormat->iBullet->iCharacterCode != iPrevParaFormat->iBullet->iCharacterCode)
		||(iThisParaFormat->iBullet->iHeight != iPrevParaFormat->iBullet->iHeight)
		||(iThisParaFormat->iBullet->iTypeface.iName != iPrevParaFormat->iBullet->iTypeface.iName)
		||(iThisParaFormat->iBullet->iTypeface.iFlags != iPrevParaFormat->iBullet->iTypeface.iFlags))
			{	
			iPMLWriter->SetFormat(EAttBullet, iThisParaFormat->iBullet);
			}
		}
	if (iThisParaFormat->iDefaultTabWidth != iPrevParaFormat->iDefaultTabWidth)
		iPMLWriter->SetFormat(EAttDefaultTabWidth, iThisParaFormat->iDefaultTabWidth);
	CheckTabList(iThisParaFormat, iPrevParaFormat);	// tabs are sensed separately
	}


void CRichTextReader::CheckTabList(CParaFormat* aFormatOne, CParaFormat* aFormatTwo)
// Check whether the first (current) and second formats' tablists differ
// If they do, output whole of first list to PML
	{
	TBool output = EFalse;
	if (aFormatOne->TabCount() != aFormatTwo->TabCount())
		output = ETrue;	// output to PML
	else 
		{
		// lists are same length: check that all members are the same
		TInt index = 0;
		TBool dif = EFalse;
		while ((index < aFormatOne->TabCount())&&(!dif))
			{
			if (aFormatOne->TabStop(index) != aFormatTwo->TabStop(index))
				output = ETrue;	// output to PML
			index++;
			}
		}
	if (output)
		{
		for (TInt i=0; i < aFormatOne->TabCount(); i++)
			iPMLWriter->SetFormat(EAttTabStop, aFormatOne->TabStop(i));
		}
	}


void CRichTextReader::CompareCharFormats()
	{
	if (iThisCharFormat.iLanguage != iPrevCharFormat.iLanguage)
		iPMLWriter->SetFormat(EAttCharLanguage, iThisCharFormat.iLanguage);
	if (iThisCharFormat.iColor != iPrevCharFormat.iColor)
		iPMLWriter->SetFormat(EAttColor, iThisCharFormat.iColor.RgbToUint());
	if (iThisCharFormat.iFontSpec.iTypeface.iName != iPrevCharFormat.iFontSpec.iTypeface.iName)
		iPMLWriter->SetFormat(EAttFontTypefaceName, iThisCharFormat.iFontSpec.iTypeface);
	if (iThisCharFormat.iFontSpec.iTypeface.iFlags != iPrevCharFormat.iFontSpec.iTypeface.iFlags)
		iPMLWriter->SetFormat(EAttFontTypefaceFlags, iThisCharFormat.iFontSpec.iTypeface);
	if (iThisCharFormat.iFontSpec.iHeight != iPrevCharFormat.iFontSpec.iHeight)
		iPMLWriter->SetFormat(EAttFontHeight, iThisCharFormat.iFontSpec.iHeight);
	if (iThisCharFormat.iFontSpec.iPosture != iPrevCharFormat.iFontSpec.iPosture)
		iPMLWriter->SetFormat(EAttFontPosture, iThisCharFormat.iFontSpec.iPosture);
	if (iThisCharFormat.iFontSpec.iStrokeWeight != iPrevCharFormat.iFontSpec.iStrokeWeight)
		iPMLWriter->SetFormat(EAttFontStrokeWeight, iThisCharFormat.iFontSpec.iStrokeWeight);
	if (iThisCharFormat.iFontSpec.iPos != iPrevCharFormat.iFontSpec.iPos)
		iPMLWriter->SetFormat(EAttFontPrintPos, iThisCharFormat.iFontSpec.iPos);
	if (iThisCharFormat.iFontSpec.iUnderline != iPrevCharFormat.iFontSpec.iUnderline)
		iPMLWriter->SetFormat(EAttFontUnderline, iThisCharFormat.iFontSpec.iUnderline);
	if (iThisCharFormat.iFontSpec.iStrikethrough != iPrevCharFormat.iFontSpec.iStrikethrough)
		iPMLWriter->SetFormat(EAttFontStrikethrough, iThisCharFormat.iFontSpec.iStrikethrough);
	}




//////////////////////////////////////////////////////
// CPMLWriter
//////////////////////////////////////////////////////

CPMLWriter* CPMLWriter::NewL()
	{
	CPMLWriter* self=new(ELeave) CPMLWriter;
	self->Construct();
	return self;
	}


CPMLWriter::CPMLWriter()
	{
	// init variables
	}
	 

void CPMLWriter::Construct()
	{
	iTextBuf = CBufSeg::New(64);  // create buffer with granularity 64
	}


void CPMLWriter::Destruct()
	{
	Adt::Destroy(iTextBuf);
	}


void CPMLWriter::ExpandBuf(const TDes8& aBuffer, TDes& aTarget)
	//
	// Input 8 bit buffer to be returned by ref. as an 8/16-bit version
	// Used for unicode compatability
	//
	{
	TText textPointer;
	for (TInt pos=0 ; pos<aBuffer.Length() ; pos++ )
		{
		textPointer = aBuffer[pos];
		aTarget.Append(textPointer);
		}
	}


void CPMLWriter::SquashBuf(const TDesC& aBuffer, TDes8& aTarget)
	//
	// Input 8/16 bit buffer and an 8-bit target to be copied into.
	// Used for unicode compatability
	//
	{
	TText textPointer;
	for (TInt pos=0; pos<aBuffer.Length(); pos++)
		{
		textPointer = aBuffer[pos];
		aTarget.Append(textPointer);
		}
	}


void CPMLWriter::Insert(const TDesC& aBuf)
// insert aBuf into PML doc
	{
	TUint bufLength = aBuf.Length();
	TBuf8<128> temp;
	SquashBuf(aBuf,temp);
	iTextBuf->InsertL(iInsertPos, temp, bufLength);
	iInsertPos += bufLength; 
	}


void CPMLWriter::Delete(TUint aLength)
// delete back from current insert pos aLength (characters)
	{ 
	iTextBuf->Delete(iInsertPos-aLength, aLength);
	iInsertPos -= aLength;
	}


void CPMLWriter::Output(RConsole aConsole)
// output buffer to screen
	{ 
	TBuf8<1> tempBuf;
	TInt readPos = 0;
	while (readPos < iTextBuf->Size())
		{
		iTextBuf->Read(readPos, tempBuf, 1);
		TBuf<1> wideTemp;
		ExpandBuf(tempBuf,wideTemp);
		aConsole.Write(wideTemp);
		readPos++;
		}
	}


void CPMLWriter::SetTag(TTagType aTagType, TTagStatus aStatus)
// insert a tag-open or tag-close into the PML doc
	{
	if (aStatus == ETagStart)
		{
		switch (aTagType)
			{
			case EGlobal:
				Insert(_L("\n<G "));
				break;
			case EParagraph:
				Insert(_L("\n<P "));
				break;
			case ECharacter:
				Insert(_L("<C "));
				break;
			case EControl:
				Insert(_L("<X "));
				break;
			}
		}
	if (aStatus == ETagEnd)
		{
		switch (aTagType)
			{
			case EGlobal:
				Insert(_L(">\n"));
				break;
			case EParagraph:
				Insert(_L(">\n"));
				break;
			case ECharacter:
				Insert(_L(">"));
				break;
			case EControl:
				Insert(_L(">"));
				break;
			}
		}
	}


void CPMLWriter::SetTab(TTabStop aTabStop)
// insert a tab into the PML doc
	{
	TBuf<80> tagBuf;
	tagBuf.Format(_L("Tab=,%d"),aTabStop.iPosition);	// put 2nd paramater in first
	switch (aTabStop.iType)								// insert 1st paramater after "="
		{
		case ENullTab:
			tagBuf.Insert(4,_L("Null"));				
			break;
		case ELeftTab:
			tagBuf.Insert(4,_L("Left"));
			break;
		case ECenteredTab:
			tagBuf.Insert(4,_L("Centered"));
			break;
		case ERightTab:
			tagBuf.Insert(4,_L("Right"));
			break;
		}
	Insert(tagBuf);
	}

/* The setFormat functions add the stipulated formatting to the PML doc */

void CPMLWriter::SetFormat(TTextFormatAttribute aAttribute, TInt aValue)
	{
	TBuf<80> outputBuf;
	switch (aAttribute)
		{
		// Para Formats
		case EAttParaLanguage:
			outputBuf.Format(_L("ParaLanguage=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttLeftMargin:
			outputBuf.Format(_L("LeftMargin=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttRightMargin:
			outputBuf.Format(_L("RightMargin=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttIndent:
			outputBuf.Format(_L("Indent=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttAlignment:
			ProcessAlignment(aValue);
			break;
		case EAttLineSpacing:
			outputBuf.Format(_L("LineSpacing=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttLineSpacingControl:
			ProcessLineSpacingControl(aValue);
			break;
		case EAttSpaceBefore:
			outputBuf.Format(_L("SpaceBefore=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttSpaceAfter:
			outputBuf.Format(_L("SpaceAfter=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttKeepTogether:
			ProcessBooleanAtt(aAttribute, aValue);
			break;
		case EAttKeepWithNext:
			ProcessBooleanAtt(aAttribute, aValue);
			break;
		case EAttStartNewPage:
			ProcessBooleanAtt(aAttribute, aValue);
			break;
		case EAttWidowOrphan:
			ProcessBooleanAtt(aAttribute, aValue);
			break;
		case EAttBorderMargin:
			outputBuf.Format(_L("BorderMargin=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttDefaultTabWidth:
			outputBuf.Format(_L("DefaultTabWidth=%d "),aValue);
			Insert(outputBuf);
			break;

	// Char formats
		case EAttCharLanguage:
			outputBuf.Format(_L("Language=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttColor:
			outputBuf.Format(_L("Color=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttFontHeight:
			outputBuf.Format(_L("FontHeight=%d "),aValue);
			Insert(outputBuf);
			break;
		case EAttFontPosture:
			ProcessBooleanAtt(aAttribute, aValue);
			break;
		case EAttFontStrokeWeight:
			ProcessBooleanAtt(aAttribute, aValue);
			break;
		case EAttFontPrintPos:
			ProcessFontPrintPos(aValue);
			break;
		case EAttFontUnderline:
			ProcessBooleanAtt(aAttribute, aValue);
			break;
		case EAttFontStrikethrough:
			ProcessBooleanAtt(aAttribute, aValue);
			break;
		case EAttFontTypefaceFlags:
			outputBuf.Format(_L("TypefaceFlags=%d "),aValue);
			Insert(outputBuf);
			break;
		}
	}


void CPMLWriter::SetFormat(TTextFormatAttribute aAttribute, const TTypeface &aTypeface)
	{
	TBuf<80> outputBuf;
	switch (aAttribute)
		{
		case EAttFontTypefaceName:
			outputBuf.Format(_L("TypefaceName=%S "),&aTypeface.iName);
			break;
		case EAttFontTypefaceFlags:
			outputBuf.Format(_L("TypefaceFlags=%u "),aTypeface.iFlags);
			break;
		}
	Insert(outputBuf);
	}

void CPMLWriter::SetFormat(TTextFormatAttribute aAttribute, TParaBorder* aParaBorder)
	{
	TBuf<80> outputBuf;
	TBuf<80> values;
	values.Format(_L("%d,%d,%d"),aParaBorder->iLineStyle,aParaBorder->iAutoColor,aParaBorder->iColor.RgbToUint());
	outputBuf.Insert(0,values);
	switch (aAttribute)
		{
		case EAttTopBorder:
			outputBuf.Insert(0,_L("TopBorder="));
			break;
		case EAttBottomBorder:
			outputBuf.Insert(0,_L("BottomBorder="));
			break;
		case EAttLeftBorder:
			outputBuf.Insert(0,_L("LeftBorder="));
			break;
		case EAttRightBorder:
			outputBuf.Insert(0,_L("RightBorder="));
			break;
		}
	Insert(outputBuf);
	}


void CPMLWriter::SetFormat(TTextFormatAttribute /*aAttribute*/, TBullet* aBullet)
	{
	TBuf<80> outputBuf;
	outputBuf.Format(_L("%d,%u,%u,"),aBullet->iCharacterCode,aBullet->iHeight,
															aBullet->iTypeface.iFlags);
	TBuf<128> wideTemp; // long enough??
	ExpandBuf(aBullet->iTypeface.iName,wideTemp);
	outputBuf.Append(wideTemp);
	outputBuf.Append(_L(" "));
	outputBuf.Insert(0,_L("Bullet="));
	Insert(outputBuf);
	}


void CPMLWriter::SetFormat(TTextFormatAttribute /*aAttribute*/, TTabStop aTabStop)
	{
	TBuf<80> outputBuf;
	outputBuf.Format(_L("Tabstop=%u,"),aTabStop.iPosition);
	switch (aTabStop.iType)
		{
		case ENullTab:
			outputBuf.Append(_L("Null "));
			break;
		case ELeftTab:
			outputBuf.Append(_L("Left "));
			break;
		case ERightTab:
			outputBuf.Append(_L("Right "));
			break;
		case ECenteredTab:
			outputBuf.Append(_L("Centered "));
			break;
		}
	Insert(outputBuf);
	}


void CPMLWriter::ProcessAlignment(TInt aValue)
	{
	TBuf<80> outputBuf;
	switch (aValue)
		{
		case 0:
			outputBuf.Insert(0,_L("Alignment=Left "));
			break;
		case 1:
			outputBuf.Insert(0,_L("Alignment=Center "));
			break;
		case 2:
			outputBuf.Insert(0,_L("Alignment=Right "));
			break;
		case 3:
			outputBuf.Insert(0,_L("Alignment=Justified "));
			break;
		}
	Insert(outputBuf);
	}


void CPMLWriter::ProcessLineSpacingControl(TInt aValue)
	{
	TBuf<80> outputBuf;
	switch (aValue)
		{
		case 0:
			outputBuf.Insert(0,_L("LineSpacingControl=Atleast "));
			break;
		case 1:
			outputBuf.Insert(0,_L("LineSpacingControl=Exactly "));
			break;
		}
	Insert(outputBuf);
	}


void CPMLWriter::ProcessFontPrintPos(TInt aValue)
	{
	TBuf<80> outputBuf;
	switch (aValue)
		{
		case 0:
			outputBuf.Insert(0,_L("PrintPos=Normal "));
			break;
		case 1:
			outputBuf.Insert(0,_L("PrintPos=SuperScript "));
			break;
		case 2:
			outputBuf.Insert(0,_L("PrintPos=SubScript "));
			break;
		}
	Insert(outputBuf);
	}


 void CPMLWriter::ProcessBooleanAtt(TTextFormatAttribute aAttribute, TInt aValue)
	{
	TBuf<80> outputBuf;
	switch (aAttribute)
		{
		case EAttKeepTogether:
			outputBuf.Insert(0,_L("KeepTogether "));
			break;
		case EAttKeepWithNext:
			outputBuf.Insert(0,_L("KeepWithNext "));
			break;
		case EAttStartNewPage:
			outputBuf.Insert(0,_L("StartNewPage "));
			break;
		case EAttWidowOrphan:
			outputBuf.Insert(0,_L("WidowOrphan "));
			break;
		case EAttFontPosture:
			outputBuf.Insert(0,_L("Italic "));
			break;
		case EAttFontStrokeWeight:
			outputBuf.Insert(0,_L("Bold "));
			break;
		case EAttFontUnderline:
			outputBuf.Insert(0,_L("Underline "));
			break;
		case EAttFontStrikethrough:
			outputBuf.Insert(0,_L("Strikethrough "));
			break;
		}
	if (aValue == 0)	// Boolean NOT
		outputBuf.Insert(0,_L("!"));
	Insert(outputBuf);
	}
