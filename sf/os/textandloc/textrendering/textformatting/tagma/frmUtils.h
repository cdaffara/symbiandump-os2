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




#ifndef FRMUTILS_H
#define FRMUTILS_H


static TChar::TBdCategory BdCategory(TUint aCode)
	{
	// Optimise for very common cases.
	if (aCode == 0x0020)
		return TChar::EWhitespace;
	if (aCode >= 0x0061 && aCode <= 0x007A || // a..z
		aCode >= 0x0041 && aCode <= 0x005A || // A..Z
		aCode >= 0x00C0 && aCode <= 0x0233 || // Latin-1, Latin Extended A and B
		aCode >= 0x0388 && aCode <= 0x03F3 || // most Greek letters
		aCode >= 0x0401 && aCode <= 0x0481) // most Cyrillic letters
		return TChar::ELeftToRight;
	if (aCode == 0x002C || aCode == 0x002E)
		return TChar::ECommonNumberSeparator;
	if (aCode == 0x002D)
		return TChar::EEuropeanNumberTerminator;
	if (aCode >= '0' && aCode <= '9')
		return TChar::EEuropeanNumber;
	return TChar(aCode).GetBdCategory();
	}

static TChar::TBdCategory FirstStrongCategoryInParagraph(TInt aStartPosOfNextLine, RTmTextCache& aText, TInt &aPositionOfLastStrongCategory, TInt aStartPosOfThisLine)
	{
	TUint theChar = aText.Char(aStartPosOfNextLine);
	TChar::TBdCategory cat = BdCategory(theChar);

	TInt pos = aStartPosOfNextLine + 1;
	
	//	Surrogate
	if ( theChar > 0xFFFF )
		++pos;
	
	// While category still weak, keep looking
	while (TChar::EPopDirectionalFormat < cat
		&& cat != TChar::EParagraphSeparator)
		{
		__ASSERT_DEBUG(pos <= aText.Source().DocumentLength(),
			TmPanic(EInvariant));
		// If we have reached the part of text that has already been traversed, without finding
		// a strong category, then just return the category at aPositionOfLastStrongCategory as
		// that is the first strong category in the remaining paragraph.
		// This is a possibility when aStartPosOfNextLine is less than aStartPosOfPrevLine
		if (pos == aStartPosOfThisLine)
			{
			return BdCategory(aText.Char(aPositionOfLastStrongCategory));
			}
		theChar = aText.Char(pos);

		cat = BdCategory( theChar );	
		++pos;
		
		if ( theChar > 0xFFFF )
			{
			++pos;
			}
		}
	aPositionOfLastStrongCategory = pos;
	return cat;
	}

/**
Get values for aMaxHeight, aMaxDepth
*/
static void GetMaxHeightAndDepth( CFont * aFont, TInt& aMaxHeight,TInt& aMaxDepth)
	{
	CFbsFont* open_font = NULL;
	
	if (aFont->TypeUid() == KCFbsFontUid)
		{
		open_font = (CFbsFont*)aFont;
		if (!open_font->IsOpenFont())
			open_font = NULL;
		}

	if (open_font)
		{
		TOpenFontMetrics font_metrics;
		open_font->GetFontMetrics(font_metrics);
		aMaxHeight = font_metrics.MaxHeight();
		aMaxDepth = font_metrics.MaxDepth();
		}
	else
		{
		aMaxHeight = aFont->AscentInPixels();
		aMaxDepth = aFont->DescentInPixels();
		}	
	}
	
static void CalculateScale(const MTmSource& aSource,TInt& aXScale,TInt& aYScale)
	{
	if (&aSource.FormatDevice() == &aSource.InterpretDevice())
		aXScale = aYScale = 1000;
	else
		{
		int p = aSource.FormatDevice().HorizontalPixelsToTwips(1000);
		int q = aSource.InterpretDevice().HorizontalPixelsToTwips(1000);
		aXScale = p * 1000 / q;
		p = aSource.FormatDevice().VerticalPixelsToTwips(1000);
		q = aSource.InterpretDevice().VerticalPixelsToTwips(1000);
		aYScale = p * 1000 / q;
		}
	}

#endif //FRMUTILS_H
