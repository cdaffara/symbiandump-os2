/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Base class functions for TAGMA's text source interface classes.
*
*/



#include <txtfrmat.h>
#include <txtetext.h>
#include "TAGMA.H"
#include "TMSTD.H"
#include <linebreak.h>

inline TBool IsSurrogate(TText a) { return 0xD800 == (a & 0xF800); }

/**
Convert a system colour index to a system colour, using or modifying the supplied default colour if desired.
*/
EXPORT_C TRgb MTmCustom::SystemColor(TUint /*aColorIndex*/,TRgb aDefaultColor) const
	{
	return aDefaultColor;
	}

/**
Return the level of stretchability; the relative desirability of inserting space after a character
when fully justifying. A value of zero means that the character cannot be stretched. Higher values
mean greater stretchability.
*/
EXPORT_C TInt MTmCustom::Stretch(TUint aChar) const
	{
	// Ordinary spaces and non-break spaces are most stretchable.
	if (aChar == 0x0020 || aChar == 0x00A0)
		return 3;
	// Nothing else in General Scripts, Symbols, Surrogates, Private Use, Compatibility, and Specials is stretchable.
	if (aChar < 0x3000 || aChar >= 0xD800)
		return 0;
	// Ideographs, Hangul, Bopomofo, Kanbun, etc., are stretchable if nothing else is available.
	if (aChar <= 0x303F || aChar >= 0x3100)
		return 1;
	// Katakana and hiragana are more stretchable than ideographs but less so than ordinary spaces.
	return 2;
	}

/**
The default function to map a character to the character drawn, or to 0xFFFF if the character is not to be drawn.
*/
EXPORT_C TUint MTmCustom::Map(TUint aChar) const
	{
	// Deal with some easy cases without calling TChar::GetCategory.
	if (aChar >= 0x0020 && aChar <= 0x007E) // ascii
		return aChar;
	
	if (aChar >= 0x00A1 && aChar <= 0x021F) // latin supplement & contiguous latin extended
		{
		if (aChar == CEditableText::EPotentialHyphen) //0x00AD
			return '-'; //0x002D
		return aChar;
		}
		
	if (aChar >= 0xE000 && aChar <= 0xF8FF) // private use area
		return aChar;

	// All special hyphens are represented as an ordinary  hyphen.
	if (aChar == CEditableText::ENonBreakingHyphen)
		return '-';

	// Picture characters are turned into 0x25A3 (black square inside white square).
	if (aChar == CEditableText::EPictureCharacter)
		return 0x25A3;

	// Zero-width space not drawn
	if (aChar == 0x200B)
		return 0xFFFF;
	// Hard space rendered as normal space
	if (aChar == 0xA0)
		return 0x20;
	// don't show corrupt surrogate
	if (IsSurrogate(aChar))
		{
		//This is an informationl message. Commented out to avoid annoyance
		//RDebug::Print(_L("Error: Do not show corrupt surrogate %X."), aChar);
		return 0xFFFF;
		}
		
	TChar::TCategory cat = TChar(aChar).GetCategory();
		
	// Control characters and unassigned characters are usually not understood
	// by CFont::DrawText. Some are and must be present to get the correct
	// result.
	if (cat > TChar::EZsCategory
		&& aChar != KZeroWidthJoiner
		&& aChar != KZeroWidthNonJoiner)
		return 0xFFFF;

	// Other characters are left alone.
	return aChar;
	}

/** 
The default function to set the line height.
*/
EXPORT_C void MTmCustom::SetLineHeight(const TLineHeightParam& aParam,TInt& aAscent,TInt& aDescent) const
	{
	// Set the ascent and descent to the maximum font ascent and descent.
	aAscent = aParam.iFontMaxAscent;
	aDescent = aParam.iFontMaxDescent;

	// Adjust to take account of desired line height.
	if (aAscent + aDescent < aParam.iDesiredLineHeight)
		aDescent = aParam.iDesiredLineHeight - aAscent;
	else if (aAscent + aDescent > aParam.iDesiredLineHeight && aParam.iExactLineHeight)
		{
		aDescent = Max(0,(aParam.iDesiredLineHeight - aAscent));
		aAscent = aParam.iDesiredLineHeight - aDescent;
		}

	/*
	Try to accommodate the maximum font height by increasing the ascent and reducing the descent by the same amount.
	Using the maximum font height rather than the maximum character height preserves line spacing where possible.
	*/
	int excess = Max(0,aParam.iFontMaxCharHeight - aAscent);
	int descent_available = Max(0,aDescent - aParam.iFontMaxCharDepth);
	int ascent_increase = Min(excess,descent_available);
	aAscent += ascent_increase;
	aDescent -= ascent_increase;

	/*
	If that is not possible, adjust the height and depth to accommodate the highest and deepest characters
	and the tallest vertically floating picture. Don't do this, of course, if the line spacing was specified
	as an exact requirement.
	*/
	if (!aParam.iExactLineHeight)
		{
		if (aParam.iMaxCharHeight > aAscent)
			aAscent = aParam.iMaxCharHeight;
		if (aParam.iMaxCharDepth > aDescent)
			aDescent = aParam.iMaxCharDepth;

		/*
		If there are only top-aligned overheight pictures any excess goes into the descent,
		and if there are only bottom-aligned overheight pictures any excess goes into the ascent,
		otherwise any excess is distributed evenly between ascent and descent.
		*/
		int top_excess = aParam.iMaxTopPictureHeight - (aAscent + aDescent);
		int bottom_excess = aParam.iMaxBottomPictureHeight - (aAscent + aDescent);
		int centre_excess = aParam.iMaxCenterPictureHeight - (aAscent + aDescent);
		if (top_excess > 0 && bottom_excess <= 0 && centre_excess <= 0)
			aDescent += top_excess;
		else if (top_excess <= 0 && bottom_excess > 0 && centre_excess <= 0)
			aAscent += bottom_excess;
		else
			{
			centre_excess = Max(centre_excess,top_excess);
			centre_excess = Max(centre_excess,bottom_excess);
			if (centre_excess > 0)
				{
				aAscent += centre_excess / 2;
				aDescent += centre_excess - centre_excess / 2;
				}
			}
		}
	}

/**
The default function to drawn the background. It fills the supplied rectangle with the background colour.
*/
EXPORT_C void MTmCustom::DrawBackground(CGraphicsContext& aGc,const TPoint& /*aTextLayoutTopLeft*/,const TRect& aRect,
										const TLogicalRgb& aBackground,TRect& aRectDrawn) const
	{
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	SetBrushColor(aGc,aBackground);
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.DrawRect(aRect);
	aRectDrawn = aRect;
	}

/**
By default no line graphics are drawn.
*/
EXPORT_C void MTmCustom::DrawLineGraphics(CGraphicsContext& /*aGc*/,const TPoint& /*aTextLayoutTopLeft*/,
										  const TRect& /*aRect*/,const TTmLineInfo& /*aLineInfo*/) const
	{
	}

/**
Draw text and its highlit background if any. The text should be drawn with its origin
at aTextOrigin after optionally drawing the background in aRect; and the text should be expanded
in width by aExtraPixels, normally by using letterspacing. The default function
draws the text with no special effects and supports standard and rounded highlighting only.
The font, colour, and text style, which are specified in aFormat, have already been selected into the graphics context.
*/
EXPORT_C void MTmCustom::DrawText(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
								  const TTmLineInfo& aLineInfo,const TTmCharFormat& aFormat,
								  const TDesC& aText,const TPoint& aTextOrigin,TInt aExtraPixels) const
	{
	MTmCustomExtension c;
	c.DrawText(aGc,aTextLayoutTopLeft,aRect,aLineInfo,aFormat,aText,-1,-1,aTextOrigin,aExtraPixels);
	}

/** Draw the picture onto the graphics context.
@param aGc The graphics context.
@param aTopLeft Coordinates of the top left corner pixel of the picture.
@param aRect A clipping rectangle that defines the area to draw the picture.
@param aDevice The device map for the graphics device. It provides
the scaling to apply to the picture.
@param aPicture The drawable object.

@see CPicture::Draw
*/
EXPORT_C void MTmCustom::DrawPicture(CGraphicsContext& aGc,
	const TPoint& aTopLeft, const TRect& aRect,
	MGraphicsDeviceMap& aDevice, const CPicture& aPicture) const
	{
	aGc.SetClippingRect(aRect);
	aPicture.Draw(aGc, aTopLeft, aRect, &aDevice);
	}

/** This function translates logical colours specified in FORM objects into
real colours.
It extracts the logical colour index stored in TLogicalRgb and invokes
<code>MtmCustom::SystemColor(TUint, TRgb)</code> to convert the logical colour to
real colour.
@param aColor The logical colour to translate.
@return The real colour represesented by the logical colour.
*/
EXPORT_C TRgb MTmCustom::SystemColor(TLogicalRgb aColor) const
	{
	MTmCustomExtension c;
	return c.SystemColor(aColor);
	}

/** Sets the pen colour.
The pen is used to draw lines, the outlines of filled shapes, and text.

@param aColor The logical colour for the pen. 
@see CGraphicsContext::SetPenColor()
*/
EXPORT_C void MTmCustom::SetPenColor(CGraphicsContext& aGc,TLogicalRgb aColor) const
	{	
	MTmCustomExtension c;
	TUint index = aColor.SystemColorIndex();
	if (index)
	   aColor = SystemColor(index,aColor) & 0xFFFFFF;
	c.SetPenColor(aGc,aColor);
	}

/** Sets the brush colour.
The brush is used for filling shapes and the background of text boxes. The 
brush has colour, style, pattern and pattern origin parameters.
If no brush colour has been set, it defaults to white. However the default 
brush style is null, so when drawing to a window the default appears to be 
the window's background colour.

@param aColor The logical colour for the brush. 
@see CGraphicsContext::SetDrawMode()
*/
EXPORT_C void MTmCustom::SetBrushColor(CGraphicsContext& aGc,TLogicalRgb aColor) const
	{
	MTmCustomExtension c;
	c.SetBrushColor(aGc,aColor);
	}
	
/**************************MTmCustomExtension*************************************************/

/**
Convert a system colour index to a system colour, using or modifying the supplied default colour if desired.
*/
EXPORT_C TRgb MTmCustomExtension::SystemColor(TUint /*aColorIndex*/,TRgb aDefaultColor) const
	{
	return aDefaultColor;
	}

/**
Draw text and its highlit background if any. The text should be drawn with its origin
at aTextOrigin after optionally drawing the background in aRect; and the text should be expanded
in width by aExtraPixels, normally by using letterspacing. The default function
draws the text with no special effects and supports standard and rounded highlighting only.
The font, colour, and text style, which are specified in aFormat, have already been selected into the graphics context.
*/
EXPORT_C void MTmCustomExtension::DrawText(CGraphicsContext& aGc,const TPoint& /*aTextLayoutTopLeft*/,const TRect& aRect,
								  const TTmLineInfo& /*aLineInfo*/,const TTmCharFormat& aFormat,
								  const TDesC& aText,const TInt aStart, const TInt aEnd, const TPoint& aTextOrigin,TInt aExtraPixels) const
	{
	TUint effects = aFormat.iEffects;
	CGraphicsContext::TTextParameters contextParam;
	contextParam.iStart = aStart;
	contextParam.iEnd = aEnd;
	
	// Draw the highlight.
	if (effects)
		{
		if (effects & TTmCharFormat::EBackground)
			{
			// Draw the background.
			if (!(effects & TTmCharFormat::ERounded))
				{
				SetBrushColor(aGc,aFormat.iBackgroundColor);
				aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
				aGc.SetPenStyle(CGraphicsContext::ENullPen);
				aGc.DrawRect(aRect);
				aGc.SetPenStyle(CGraphicsContext::ESolidPen);
				}

			// Draw the background with rounded corners.
			else
				{
				/*
				The pen is made solid, not null, and the pen colour is set, to
				work around some bugs in DrawRoundRect: see ER5U defect EDNGASR-487K64.
				*/
				SetBrushColor(aGc,aFormat.iBackgroundColor);
				aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
				aGc.SetPenStyle(CGraphicsContext::ESolidPen);
				SetPenColor(aGc,aFormat.iBackgroundColor);
				TSize corner(Min(aRect.Height(),aRect.Width()) / 3,aRect.Height() / 3);
				aGc.DrawRoundRect(aRect,corner);
				//+ fill in flat side if needed; draw ordinary rectangle if both sides are flat.
				SetPenColor(aGc,aFormat.iTextColor);
				}
			}

		// Draw the text offset towards the bottom-right in the highlight colour before drawing in the text colour.
		else if (effects & TTmCharFormat::EShadow)
			{
			SetPenColor(aGc,aFormat.iBackgroundColor);
			if (aExtraPixels)
				aGc.SetCharJustification(aExtraPixels,aText.Length());
			TPoint p(aTextOrigin);
			p.iX += 2;
			p.iY += 2;
			if (aStart < 0 || aEnd < 0)
				aGc.DrawText(aText,p);
			else
				aGc.DrawText(aText,&contextParam,p);
			SetPenColor(aGc,aFormat.iTextColor);
			}
		}

	// Draw the text, with letterspacing if needed.
	if (aExtraPixels)
		aGc.SetCharJustification(aExtraPixels,aText.Length());
	if (aStart < 0 || aEnd < 0)
		aGc.DrawText(aText,aTextOrigin);
	else	
		aGc.DrawText(aText,&contextParam,aTextOrigin);
	}


/** This function translates logical colours specified in FORM objects into
real colours.
It extracts the logical colour index stored in TLogicalRgb and invokes
<code>MtmCustom::SystemColor(TUint, TRgb)</code> to convert the logical colour to
real colour.
@param aColor The logical colour to translate.
@return The real colour represesented by the logical colour.
*/
EXPORT_C TRgb MTmCustomExtension::SystemColor(TLogicalRgb aColor) const
	{
	TUint index = aColor.SystemColorIndex();
	if (index)
		return SystemColor(index,aColor) & 0xFFFFFF;
	return aColor;
	}

/** Sets the pen colour.
The pen is used to draw lines, the outlines of filled shapes, and text.

@param aColor The logical colour for the pen. 
@see CGraphicsContext::SetPenColor()
*/
EXPORT_C void MTmCustomExtension::SetPenColor(CGraphicsContext& aGc,TLogicalRgb aColor) const
	{
	aGc.SetPenColor(SystemColor(aColor));
	}

/** Sets the brush colour.
The brush is used for filling shapes and the background of text boxes. The 
brush has colour, style, pattern and pattern origin parameters.
If no brush colour has been set, it defaults to white. However the default 
brush style is null, so when drawing to a window the default appears to be 
the window's background colour.

@param aColor The logical colour for the brush. 
@see CGraphicsContext::SetDrawMode()
*/
EXPORT_C void MTmCustomExtension::SetBrushColor(CGraphicsContext& aGc,TLogicalRgb aColor) const
	{
	aGc.SetBrushColor(SystemColor(aColor));
	}

/***************************MTmCustomExtension End********************************************/

/**
Return the end of the paragraph - after the paragraph separator if any.
*/
EXPORT_C TInt MTmSource::ParagraphEnd(TInt aPos) const
	{
	TInt doc_end = DocumentLength();
	TPtrC text;
	TTmCharFormat format;
	while (aPos < doc_end)
		{
		GetText(aPos,text,format);
		const TText *p = text.Ptr();
		const TText *q = p + text.Length();
		while (p < q)
			if (*p == CEditableText::EParagraphDelimiter)
				return aPos + 1;
			else
				{
				p++;
				aPos++;
				}
		}
	return doc_end + 1;
	}

/**
The default function to get a picture; override this if there are pictures in the document.
*/
EXPORT_C CPicture* MTmSource::PictureL(TInt /*aPos*/) const
	{
	return NULL;
	}

// The default function to get a picture's size; override this if there are pictures in the document.
EXPORT_C TInt MTmSource::GetPictureSizeInTwipsL(TInt /*aPos*/,TSize& /*aSize*/) const
	{
	return KErrNotFound;
	}

/**
The default function to select label mode for a paragraph containing a particular position.
If a label of the selected type exists for the paragraph, the mode is selected and the source
returns the text of the label, not the main text, until the mode is cancelled. The return value
is ETrue if a label exists, EFalse otherwise. This means that FALSE must be returned if label mode
has already been selected.
*/
EXPORT_C TBool MFormLabelApi::LabelModeSelect(MTmSource::TLabelType /*aType*/, TInt /*aPos*/)
	{
	return EFalse;
	}

/**
Cancel any label mode that has been selected.
*/
EXPORT_C void MFormLabelApi::LabelModeCancel()
	{
	}

/**
Get the size and margin size for a label. The maximum width and height for the label text
are placed in aLabelSize and the width or height of the margin containing the label in aMarginSize.
Whether the label is placed left, right, above or below the text and whether the label is
centred in the margin or placed flush with one of its edges depends on the label type.
*/
EXPORT_C void MFormLabelApi::LabelMetrics(MTmSource::TLabelType /*aType*/, TSize& aLabelSize, TInt& aMarginSize) const
	{
	aLabelSize.iWidth = 0;
	aLabelSize.iHeight = 0;
	aMarginSize = 0;
	}



EXPORT_C TAny* MTmSource::GetExtendedInterface(const TUid& /*aInterfaceId*/)
	{
	return NULL;
	}

/**
@internalAll
@released
*/
EXPORT_C void MTmSource::MTmSource_Reserved_1()
	{
	}
/**
@internalAll
@released
*/
EXPORT_C void MTmSource::MTmSource_Reserved_2()
	{
	}


/** Returns whether line break is possible between two characters.
@param aPrevClass The class before the potential line break.
@param aNextClass The class after the potential line break.
@param aHaveSpaces Whether spaces separate the characters.
@return ETrue if a line break is possible between characters with the two line
break classes, EFalse if not.

@see MLineBreaker::LineBreakPossible()
*/
EXPORT_C TBool MTmCustom::LineBreakPossible(TUint aPrevClass,TUint aNextClass,TBool aHaveSpaces) const
	{
	MLineBreaker b;
	return b.LineBreakPossible(aPrevClass, aNextClass, aHaveSpaces);
	}

/** Converts Unicode character into line breaking class.
@param aCode Input Unicode value.
@param aRangeStart Returns the start of the contiguous range of characters 
including aCode that have the returned line breaking class.
@param aRangeEnd Returns the end of the contiguous range of characters including 
aCode that have the returned line breaking class.
@return The line breaking class for aCode.

@see MLineBreaker::LineBreakClass()
*/
EXPORT_C TUint MTmCustom::LineBreakClass(TUint aCode,TUint& aRangeStart,TUint& aRangeEnd) const
	{
	MLineBreaker b;
	return b.LineBreakClass(aCode, aRangeStart, aRangeEnd);
	}

/** 
Gets the position of the first or last possible line break position in a
text string.
This function is called instead of LineBreakPossible() for runs of characters
of class ESaLineBreakClass. It is used for languages like Thai, Lao
and Khmer that have no spaces between words, so that line breaks must be
calculated using dictionary lookup or a linguistic algorithm.
Only the Thai language is supported in this implementation, i.e. this
function will return false for other languages.
@see MLineBreaker::GetLineBreakInContext()

@param aText A string containing characters of class ESaLineBreakClass.
@param aMinBreakPos A position within aText at which to begin searching for a
possible line break position.
@param aMaxBreakPos A position within aText at which to stop searching for a
possible line break position.
@param aForwards If ETrue, the function gets the first possible line break
position (searches forwards from aMinBreakPos); if EFalse, gets the last one
(searches backwards from aMaxBreakPos).
@param aBreakPos On return, the position of the first or last possible line
break within aText. This must be greater than zero and less than aText.Length()
- 1, and must also be in the range aMinBreakPos to aMaxBreakPos.
@return ETrue if a possible line break position is found, EFalse if not.

*/
EXPORT_C TBool MTmCustom::GetLineBreakInContext(const TDesC& aText,
	TInt aMinBreakPos, TInt aMaxBreakPos, TBool aForwards, TInt& aBreakPos) const
	{
	MLineBreaker b;
	return b.GetLineBreakInContext(aText, aMinBreakPos, aMaxBreakPos,
		aForwards, aBreakPos);
	}

/** Tests whether a character may be positioned outside the right margin.
This function is provided to allow customisation of the line breaking
behaviour for closing punctuation in Japanese. Any characters for which this function returns
ETrue are allowed to overhang the right margin. The rest will be moved to the
next line.

The default implementation of this function just returns false.
@see MLineBreaker::IsHangingCharacter()

@param aChar The Unicode character code of interest.
@return ETrue if the character specified can overhang the right margin, EFalse if
not. */
EXPORT_C TBool MTmCustom::IsHangingCharacter(TUint aChar) const
	{
	MLineBreaker b;
	return b.IsHangingCharacter(aChar);
	}

/*
A minimal MTmSource class used by the static function MTmSource::GetStandardLineBreak. It provides
stubs for the pure virtual functions.
*/
class TMinimalSource: public MTmSource
	{
	// We need to be able to instantiate this class for a single minimal purpose that does not
	// involve the functions FormatDevice or InterpretDevice. So the following 3 lines use a
	// syntax trick to avoid compiler warnings about returning a NULL reference
	MGraphicsDeviceMap* NoDeviceMap() const { return 0; }
	MGraphicsDeviceMap& FormatDevice() const { return *NoDeviceMap(); }
	MGraphicsDeviceMap& InterpretDevice() const { return *NoDeviceMap(); }
	TInt DocumentLength() const { return 0; }
	void GetText(TInt,TPtrC&,TTmCharFormat&) const { }
	void GetParagraphFormatL(TInt,RTmParFormat&) const { }
	TInt ParagraphStart(TInt) const { return 0; }
	};

/**
A static function to expose the standard line breaking system for use on plain text, mainly so that UIKON can
use it for messages.

It finds the last feasible line break in the range aMinBreakPos...aMaxBreakPos within aText, and the start of
the next line, which may be after aMaxBreakPos (because aMaxBreakPos may be in a run of spaces or before
a line or paragraph separator).

The text passed in is the maximum that will fit the required line width; in other words, it has already been measured.
The best way to do this is to use CFont::MeasureText and specify a TMeasureTextInput parameter with
TMeasureTextInput::iMaxAdvance to the line width; TMeasureTextOutput::iChars gives the maximum length that
will fit the line.

Bidirectional reformatting should be done after the break position is found.

Limitations:

Forced line breaks, which may be caused by CR, LF, FF, combinations of these, U+2028, and U+2029, are ignored,
the purpose of the function being to break some text that is already known to be too wide for a certain measure.
Dictionary-based line breaking for Thai is not available. Use zero-width spaces to allow line breaks in Thai.

This function cannot leave because MTmSource::GetLineBreakL cannot leave unless the implementation of PictureL can
leave, and here the default MTmSource::PictureL, which does not leave, is called.
*/
EXPORT_C void MTmSource::GetStandardLineBreak(const TDesC& aText,TInt aMinBreakPos,TInt aMaxBreakPos,
											  TInt& aBreakPos,TInt& aNextLineStart)
	{
	TMinimalSource source;
	TInt hanging_break_pos;
	TBool found = source.GetLineBreakL(aText,0,aMinBreakPos,aMaxBreakPos,FALSE,
									   aBreakPos,hanging_break_pos,aNextLineStart); // cannot leave
	if (!found)
		aBreakPos = aNextLineStart = aMaxBreakPos;
	}

	
class TSourcePictureBreaker : public MContingentLineBreakerL
	{
public:
	TSourcePictureBreaker(const MTmSource& aSource, TInt aDocPos)
		: iSource(&aSource), iDocPos(aDocPos) {}
	TBool IsLegalBreakBeforeL(TInt aTextPosition, TInt aPreceedingClass,
		TBool aHasSpaces)
		{
		CPicture* pic = iSource->PictureL(iDocPos + aTextPosition);
		if (pic)
			return pic->LineBreakPossible(aPreceedingClass, ETrue, aHasSpaces);
		return ETrue;
		}
	TBool IsLegalBreakAfterL(TInt aTextPosition, TInt aFollowingClass,
		TBool aHasSpaces)
		{
		CPicture* pic = iSource->PictureL(iDocPos + aTextPosition);
		if (pic)
			return pic->LineBreakPossible(aFollowingClass, EFalse, aHasSpaces);
		return ETrue;
		}
private:
	const MTmSource* iSource;
	TInt iDocPos;
	};

/*
Get the first or last possible line break in that part of aText between aMinBreakPos and aMaxBreakPos inclusive.
Return this in aBreakPos, which must be in the range aMinBreakPos...aMaxBreakPos. The absolute document
position of the start of the text is supplied in aDocPos; this is needed for determining the line break
characteristics of embedded pictures.

Two more positions are returned:
aHangingBreakPos is the break position after any hanging characters,
and aBreakPosAfterSpaces is the position after any spaces; aHangingBreakPos is
greater than or equal to aBreakPos and aBreakPosAfterSpaces is greater than or equal to aHangingBreakPos.

Return TRUE if a break is found, FALSE if not. aForwards determines the direction of search.

This function cannot leave unless MTmSource::PictureL can leave.
*/
EXPORT_C TBool MTmSource::GetLineBreakL(const TDesC& aText,TInt aDocPos,TInt aMinBreakPos,TInt aMaxBreakPos,
										TBool aForwards,TInt& aBreakPos,TInt& aHangingBreakPos,
										TInt& aBreakPosAfterSpaces) const
	{
	TSourcePictureBreaker picBreaker(*this, aDocPos);
	if(!MLineBreaker::GetLineBreakL(aText, aMinBreakPos, aMaxBreakPos, 
									aForwards, &picBreaker, aHangingBreakPos, 
									aBreakPosAfterSpaces))
		{
		return EFalse;
		}
	aBreakPos = aHangingBreakPos <= aMaxBreakPos? aHangingBreakPos : aMaxBreakPos;
	return ETrue;
	}

// The default function to state whether there is a page break in the given range.
EXPORT_C TBool MTmSource::PageBreakInRange(TInt /*aStartPos*/,TInt /*aEndPos*/) const
	{
	return FALSE;
	}
