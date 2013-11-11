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
* TAGMA's bytecode interpreter classes. These convert between document positions and x-y coordinates,
* draw the text, find the bytecode for some text, and supply information about lines.
*
*/


#include "TMSTD.H"
#include "TMINTERP.H"
#include "TmHighlightSource.h"
#include <s32mem.h>
#include <frmtlay.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TMINTERPTraces.h"
#endif


TTmInterpreterParam::TTmInterpreterParam(const CTmTextLayout& aTextLayout):
	iByteCode(&aTextLayout.Code()),
	iCodeStart(0),
	iCodeEnd(aTextLayout.Code().Size()),
	iTextStart(aTextLayout.StartChar()),
	iWidth(aTextLayout.LayoutWidth()),
	iDrawingInterpFlags(aTextLayout.GetDrawingInterpFlags())
	{
	}

TTmInterpreterParam::TTmInterpreterParam(const CTmCode& aByteCode):
	iByteCode(&aByteCode),
	iCodeStart(0),
	iCodeEnd(aByteCode.Size()),
	iTextStart(0),
	iWidth(0),
	iDrawingInterpFlags(0)
	{
	}

TTmInterpreter::TTmInterpreter(const TTmInterpreterParam& aParam,const TPoint* aTopLeft):
	iReader(*aParam.iByteCode,aParam.iCodeStart,aParam.iCodeEnd),
	iTextPos(aParam.iTextStart),
	iOp(0),
	iOpMod(0),
	iOpStartCode(0),
	iOpEndCode(0),
	iOpSubscript(0),
	iVisualEndOfLineIsAmbiguous(ETrue),
	iVisualStartOfLineIsAmbiguous(ETrue),
	iInlineTextFormat(0),
	iLabelType(MTmSource::ENoLabel),
	iBdState(NULL)
	{
	iOpStartChar = iOpEndChar = iTextPos;
	iTextLayoutWidth = aParam.iWidth;
	if (aTopLeft)
		iTextLayoutTopLeft = *aTopLeft;
	iNextLineTop = iTextLayoutTopLeft.iY;
	iNextLineStartChar = iTextPos;
	iLineInfo.iLineNumber = -1;
	iLineInfo.iParNumber = -1;
	iLineInfo.iLineInPar = -1;
	iLineInfo.iFlags = 0;
	if (0 == aParam.iTextStart)
		{
		iLineInfo.iFlags = TTmLineInfo::ELineEndsInForcedLineBreak;
		iVisualEndOfLineIsAmbiguous = EFalse;
		}
	}

/**
 * Returns ETrue if the document position is within the current line.
 * @internalComponent
 */
TBool TTmInterpreter::PosIsInLine(const TTmDocPos& aDocPos) const
	{
	if (aDocPos.iPos < iOpStartChar || iOpEndChar < aDocPos.iPos)
		return EFalse;
	if (iOpStartChar < aDocPos.iPos && aDocPos.iPos < iOpEndChar)
		return ETrue;
	if (aDocPos.iPos == iOpStartChar)
		return aDocPos.iLeadingEdge || !iVisualStartOfLineIsAmbiguous;
	// aDocPos.iPos == iOpEndChar
	return !aDocPos.iLeadingEdge && iVisualEndOfLineIsAmbiguous;
	}

/**
 * Read an operator EOpLine and its arguments from the bytecode, updating the
 * member variables accordingly.
 * @param aChars Number of characters in the line.
 * @return Number of bytes in the byte code for the line.
 * @internalComponent
 */
TInt TTmInterpreter::ReadOpLine(TInt aChars)
	{
	iVisualStartOfLineIsAmbiguous = iVisualEndOfLineIsAmbiguous;
	iLineInfo.iLineNumber++;
	iLineInfo.iLineInPar++;
	iLineInfo.iFlags = 0;
	if (iOpMod & EModParStart)
		{
		iVisualStartOfLineIsAmbiguous = EFalse;
		iLineInfo.iParNumber++;
		iLineInfo.iLineInPar = 0;
		iLineInfo.iParTop = iNextLineTop;
		iLineInfo.iFlags |= TTmLineInfo::EParStart;
		}
	if (iOpMod & EModParEnd)
		iLineInfo.iFlags |= TTmLineInfo::EParEnd;
	TInt flags = 0;
	if (iOpMod & EModLineFlags)
		flags = iReader.ReadNumber(); // <F> (flags)
	if (flags & ELineFlagLineEndsInForcedLineBreak)
		iLineInfo.iFlags |= TTmLineInfo::ELineEndsInForcedLineBreak;
	iVisualEndOfLineIsAmbiguous = flags & ELineFlagVisualEndOfLineIsAmbiguous;
	if (iOpMod & EModRightToLeft)
		iLineInfo.iFlags |= TTmLineInfo::EParRightToLeft;

	TInt extra_bytes = iReader.ReadNumber(); // <B> (bytecode size)

	// Set line information; read height <H> and inner rect <I> and make band-relative
	iLineInfo.iStart = iOpStartChar = iNextLineStartChar;
	iLineInfo.iEnd = iOpEndChar = iNextLineStartChar = iOpStartChar + aChars;
	iLineInfo.iOuterRect.iTl.iX = iTextLayoutTopLeft.iX;
	iLineInfo.iOuterRect.iBr.iX = iTextLayoutTopLeft.iX + iTextLayoutWidth;
	iLineInfo.iOuterRect.iTl.iY = iNextLineTop;
	iNextLineTop += iReader.ReadNumber();				// <H> (height)
	iLineInfo.iOuterRect.iBr.iY = iNextLineTop;
	iLineInfo.iInnerRect = iReader.ReadRect();			// <I> (inner rectangle)
	iLineInfo.iInnerRect.Move(iLineInfo.iOuterRect.iTl);
	iLineInfo.iBaseline = iLineInfo.iInnerRect.iTl.iY + iReader.ReadNumber();	// <A> (ascent)

	// Set the start and end pen positions
	iOpStartPen = iLineInfo.iInnerRect.iTl;
	iOpStartPen.iY = iLineInfo.iBaseline;
	iOpEndPen = iLineInfo.iInnerRect.iBr;
	iOpEndPen.iY = iLineInfo.iBaseline;

	// Read the bidirectional state immediately if it is present and is wanted (iBdState is non-null)
	//first reset the bidirectional state pointer - if the state is not present in the
	//bytecode that means that it's the default state
	if(iBdState)
		{
		iBdState->Reset();
		}
	while (iBdState && iReader.CodePos() < iReader.CodeEndPos()
		&& iReader.PeekByte() == EOpParam)
		{
		TInt end_code = iReader.CodePos() + extra_bytes;
		iReader.ReadByte(); // skip EOpParam
		TUint8 id = iReader.ReadByte();
		TInt bytes = iReader.ReadNumber();
		if (id == EParamBdState)
			{
			RBufReadStream stream(*iReader.Code().Buffer(),iReader.CodePos());
			TRAPD(error,iBdState->InternalizeL(stream));
			if (error)
				Panic(ECorrupt);
			stream.Close();
			}
		Skip(iReader.CodePos() + bytes,iOpStartChar);
		extra_bytes = end_code - iReader.CodePos();
		}

	// Set the interpreter text and pen positions.
	iPen = iOpStartPen;
	iTextPos = iOpStartChar;

	return extra_bytes;
	}

/** Read an operator and its arguments from the bytecode, updating the member
variables accordingly. Note that the current position in the byte code is
not necessarily brought up to the next operator.
@pre The current position in the byte code is at an operator.
@return EFalse if there was insufficient byte code to read.
@internalComponent */
TBool TTmInterpreter::Next()
	{
	if (iReader.CodePos() >= iReader.CodeEndPos())
		return FALSE;

	// Initialise start character, pen position and code position.
	iOpStartChar = iTextPos;
	iOpStartPen = iPen;
	iOpStartCode = iReader.CodePos();

	// Unpack the operator.
	iOp = iOpMod = iReader.ReadByte();
	iOp &= KMaskOpcode;	// top three bits
	iOpMod &= KMaskModifiers; // low five bits

	TInt chars = 1;
	iOpSubscript = 0;
	if (iOp == EOpLine)
		{
		chars = iReader.ReadNumber();
		iLineContextChar = iReader.ReadNumber();	// Read the context  for this line 
		}
		
	else
		{
		// Read optional arguments <P>, <N> and <Y>
		if (iOpMod & EModPos)
			iOpStartChar += iReader.ReadNumber();
		if (iOpMod & EModCount)
			chars = iReader.ReadNumber();
		if (iOpMod & EModSubscript)
			iOpSubscript = iReader.ReadNumber();
		}
	iOpEndChar = iOpStartChar + chars;
	iOpEndPen = iOpStartPen;

	TInt extra_bytes = 0;
	switch (iOp)
		{
		case EOpLine:
			extra_bytes = ReadOpLine(chars);
			break;

		case EOpText:
		case EOpSpecialChar:
				{
				TInt deltaX = iReader.ReadNumber();
				if (0 <= deltaX)
					iOpEndPen.iX += deltaX;
				else
					iOpStartPen.iX += deltaX;
				}
			iContextChar = iReader.ReadNumber();	// Read the context  for this chunk.
			iTextPos = iOpEndChar;
			iPen.iX = iOpEndPen.iX;
			break;

		case EOpInlineText:
			iOpEndPen.iX += iReader.ReadNumber();
			iInlineTextFormat = iReader.ReadNumber();
			{
			iInlineText.SetLength(chars + 2);
			iInlineText[0] = 0xFFFF;
			for (int i = 0; i < chars; i++)
				iInlineText[i+1] = (TText)iReader.ReadNumber();
			iInlineText[chars+1] = 0xFFFF;
			}
			iOpEndChar = iOpStartChar;
			iTextPos = iOpEndChar;
			iPen.iX = iOpEndPen.iX;
			break;

		case EOpRule:
			iBounds = iReader.ReadRect();
			iBounds.Move(iOpStartPen);
            iRuleColour = TRgb(iReader.ReadNumber());
			break;

		case EOpDoc:
			extra_bytes = iReader.ReadNumber(); // <B> (bytecode size)
			iBounds = iReader.ReadRect();		// <R> (bounds)
			iBounds.Move(iOpStartPen);
			iOpEndPen.iX = iBounds.iBr.iX;
			break;

		case EOpLabel:
			extra_bytes = iReader.ReadNumber(); // <B> (bytecode size)
			iBounds = iReader.ReadRect();		// <R> (bounds)
			iLabelType = (MTmSource::TLabelType)iReader.ReadNumber();	// <L> (label type)
			break;

		case EOpParam:
			iParamId = iReader.ReadByte();
			iOpEndChar = iOpStartChar;			// parameters consume no text
			extra_bytes = iReader.ReadNumber(); // <B> (bytecode size)
			break;			

		default:
			Panic(ECorrupt);
			break;
		}

	iOpEndCode = iReader.CodePos() + extra_bytes;

	return TRUE;
	}

/**
 * Sets the position within the byte code.
 * @param aCodePos The position within the byte code to go to.
 * @param aTextPos The document position that corresponds to aCodePos.
 * @internalComponent
 */
void TTmInterpreter::Skip(TInt aCodePos,TInt aTextPos)
	{
	iReader.SetCodePos(aCodePos);
	iTextPos = aTextPos;
	}

/** Returns the picture subscript value */
TInt TTmInterpreter::Subscript()
	{
	return iOpSubscript;
	}

/**
@internalComponent
*/
RTmGeneralInterpreter::RTmGeneralInterpreter(MTmSource& aSource,
	const TTmInterpreterParam& aParam, const TPoint* aTopLeft)
	: TTmInterpreter(aParam,aTopLeft),
	iTextCache(aSource,aSource.InterpretDevice())
	{ 
	iCopyFit = &aSource.InterpretDevice() != &aSource.FormatDevice(); 
	iTmTextDrawExt = reinterpret_cast <MTmTextDrawExt*> (Source().GetExtendedInterface(KTmTextDrawExtId));
	if(!iTmTextDrawExt)
		{
		iTmTextDrawExt = &iTmTextDrawExtDefault;
		}
	if(!iTmTextDrawExt)
	    {
        OstTrace0( TRACE_FATAL, RTMGENERALINTERPRETER_RTMGENERALINTERPRETER, "ENotImplemented" );
        __ASSERT_ALWAYS(0, Panic(ENotImplemented));
	    }
	}

/**
 * Get source text starting at aPos and not more than (aEndChar - aPos)
 * characters.
 * @param aPos The start position for the text to be returned.
 * @param aEndChar The maximum position to be returned plus one.
 * @param aText Returns a view of the text.
 * @param aFormat
 *		If non-null, returns the format of this text. All the text returned is
 *		in this format.
 * @param aFont If aFont is not null, on return contains a pointer to an opened CTmTextFontCache*
			The caller must call Close on aFont when finished with the font.
 * @internalComponent
 */
void RTmGeneralInterpreter::GetText(TInt aPos,TInt aEndChar,TPtrC& aText,TTmCharFormat* aFormat,CTmTextFontCache** aFont)
	{
	iTextCache.GetText(aPos,aEndChar,aText,aFormat,aFont);
	}

/**
 * Gets the inline text inserted by the formatter in the current position.
 * @param aText Returns the inline text. Valid until the position is moved.
 * @param aFormat If non-null, returns the format of the inline text.
 * @param aFont If aFont is not null, on return contains a pointer to an opened CTmTextFontCache*
			The caller must call Close on aFont when finished with the font.
 * @internalComponent
 */
void RTmGeneralInterpreter::GetInlineText(TPtrC& aText,TTmCharFormat* aFormat,CTmTextFontCache** aFont)
	{
	TPtrC p;
    TInt inlineTextFormatPos = InlineTextFormat() + LineInfo().iStart;
    //InlineTextFormat(): the offset position in current line
    //LineInfo().iStart : start position of current line
    //inlineTextFormatPos : position in the whole document 
    
    iTextCache.GetText(inlineTextFormatPos,KMaxTInt,p,aFormat,aFont);
	aText.Set(InlineText());
	}

/**
 * Returns the number of lines in the band.
 * @return Number of lines formatted.
 * @internalComponent
 */
TInt TTmInterpreter::Lines()
	{
	while (Next())
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, TTMINTERPRETER_LINES, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		Skip();
		}
	return iLineInfo.iLineNumber + 1;
	}

/**
 * Returns the number of paragraphs in the band.
 * @return Number of paragraphs formatted.
 * @internalComponent
 */
TInt TTmInterpreter::Paragraphs()
	{
	while (Next())
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, TTMINTERPRETER_PARAGRAPHS, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		Skip();
		}
	return iLineInfo.iParNumber + 1;
	}

/**
 * Iterates to the line containing the document position if any. Any position
 * from CTmTextLayout::StartChar() to CTmTextLayout::EndChar() inclusive can be
 * found.
 * @param aDocPos The document position to be found.
 * @return ETrue if it was found.
 * @internalComponent
 */
TBool TTmInterpreter::DocPosToLine(const TTmDocPosSpec& aDocPos)
	{
	/*
	Convert to a document position. Treat positions specified using directionality as
	trailing, then correct to the following line if necessary.
	*/
	TTmDocPos doc_pos(aDocPos.iPos,aDocPos.iType == TTmDocPosSpec::ELeading);

	if (!Next())
		return EFalse;
	if (Op() != EOpLine)
	    {
	    OstTrace0( TRACE_FATAL, TTMINTERPRETER_DOCPOSTOLINE, "ECorrupt" );
	    }
	__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
	while  (!PosIsInLine(doc_pos))
		{
		Skip();
		if (!Next())
			return EFalse;
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, DUP1_TTMINTERPRETER_DOCPOSTOLINE, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		}

	/*
	If the position is at the end of the line and was sought by direction it might not be in the line;
	if not it will be in the next line if any. Read operators and check for the position until it is
	found or we get to the next line.
	*/
	if (aDocPos.iPos == EndChar()
		&& aDocPos.iType != TTmDocPosSpec::ETrailing
		&& aDocPos.iType != TTmDocPosSpec::ELeading
		&& !AtEndOfTextLayout())
		{
		TBool rightToLeft = (aDocPos.iType == TTmDocPosSpec::ERightToLeft);
		while (Next() && Op() != EOpLine)
			{
			TBool sameDirectionality = rightToLeft?
				RightToLeft() : !RightToLeft();
			if (aDocPos.iPos == EndChar() && sameDirectionality)
				return ETrue;
			}
		}

	return ETrue;
	}

/**
 * Iterates to the line with the specified line number if any.
 * @param aLineNumber The line number to find.
 * @return ETrue if the line was found (i.e. if it is formatted).
 * @internalComponent
 */
TBool TTmInterpreter::LineNumberToLine(TInt aLineNumber)
	{
	while (Next())
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, TTMINTERPRETER_LINENUMBERTOLINE, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		if (iLineInfo.iLineNumber == aLineNumber)
			return TRUE;
		else
			Skip();
		}
	return EFalse;
	}

/**
 * Iterates to the specified line of the specified paragraph. This function
 * will always return ETrue if the paragraph is formatted. If the line number
 * is out of range it will be camped to the correct range; thus if aLineInPar
 * is KMaxTInt the last line of the paragraph is found.
 * @param aParNumber Paragraph number to be found.
 * @param aLineInPar Line number within paragraph to be found.
 * @return
 *		ETrue if the operation is successful. This will be the case if the
 *		applicable line is formatted.
 * @internalComponent
 */
TBool TTmInterpreter::ParNumberToLine(TInt aParNumber,TInt aLineInPar)
	{
	while (Next())
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, TTMINTERPRETER_PARNUMBERTOLINE, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		if (iLineInfo.iParNumber == aParNumber)
			{
			if (iLineInfo.iLineInPar >= aLineInPar || ParEnd())
				return TRUE;
			}
		Skip();
		}
	return EFalse;
	}

/**
 * Iterates to the line containing the specified Y position number if any.
 * @param aYPos The Y position to find.
 * @return ETrue if the line was found (i.e. if it is formatted).
 * @internalComponent
 */
TBool TTmInterpreter::YPosToLine(TInt aYPos)
	{
	while (Next())
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, TTMINTERPRETER_YPOSTOLINE, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		if (iLineInfo.iOuterRect.iTl.iY <= aYPos && iLineInfo.iOuterRect.iBr.iY > aYPos)
			return TRUE;
		else
			Skip();
		}
	return EFalse;
	}

/**
 * Iterates to the width of the widest line in the specified vertical range
 * that is formatted, returning its width.
 * @param aTop Line number of the top line that is to be considered.
 * @param aBottom
 *		Line number of the first line below aTop that should not be considered.
 * @return
 *		The width found, or 0 if the entire range is not formatted.
 * @internalComponent
 */
TInt TTmInterpreter::WidthOfWidestLine(TInt aTop,TInt aBottom)
	{
	int width = 0;
	while (Next())
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, TTMINTERPRETER_WIDTHOFWIDESTLINE, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		// Quit if below the range.
		if (iLineInfo.iOuterRect.iTl.iY >= aBottom)
			break;
		// If the line overlaps the range check its width.
		if (iLineInfo.iOuterRect.iBr.iY > aTop)
			{
			int cur_width = iLineInfo.iInnerRect.iBr.iX - iLineInfo.iInnerRect.iTl.iX;
			if (cur_width > width)
				width = cur_width;
			}
		Skip();
		}
	return width;
	}

/**
Finds the leftmost left bound and the rightmost right bound within the
specified y co-ordinate range
@param aTopY
	The top of the range to check.
@param aBottomY
	The bottom of the range to check.
@param aLeft
	The left most X-coordinate found. KMaxTInt if there was nothing in the
	range at all.
@param aRight
	The right most X-coordinate found. KMinTInt if there was nothing in the
	range at all.
@internalComponent
*/
void TTmInterpreter::HorizontalExtremes(TInt &aLeft, TInt &aRight,
	TInt aTopY, TInt aBottomY)
	{
	aLeft = KMaxTInt;
	aRight = KMinTInt;
	if (!Next())
		return;
	while (iLineInfo.iOuterRect.iBr.iY < aTopY)
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, TTMINTERPRETER_HORIZONTALEXTREMES, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		Skip();
		if (!Next())
			return;
		}
	while (iLineInfo.iOuterRect.iTl.iY <= aBottomY)
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, DUP1_TTMINTERPRETER_HORIZONTALEXTREMES, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		if (iLineInfo.iInnerRect.iTl.iX < aLeft)
			aLeft = iLineInfo.iInnerRect.iTl.iX;
		if (aRight < iLineInfo.iInnerRect.iBr.iX)
			aRight = iLineInfo.iInnerRect.iBr.iX;
		Skip();
		if (!Next())
			return;
		}
	}

/**
 * Finds the smallest rectangle that needs to be redrawn after reformatting
 * some text.
 * @param aNewCode Code representing the formatting of the changed text.
 * @param aStartChar The first character that has changed.
 * @param aOldLength The length of the text that was changed.
 * @param aNewLength The new length of the changed text.
 * @param aRedrawRect The rectangle that needs redrawing.
 * @internalComponent
 */
void TTmInterpreter::CalculateRedrawRect(TTmInterpreter& aNewCode,TInt aStartChar,TInt aOldLength,TInt aNewLength,
										 TRect& aRedrawRect)
	{
	/*
	Strip leading identical lines before the reformatted range and set aRedrawRect to the bounds of
	the first line to be redrawn.
	*/
	TBool have_old_line = Next();
	TBool have_new_line = aNewCode.Next();
	while (have_old_line && have_new_line)
		{
		aRedrawRect = LineInfo().iOuterRect;
		if (EndChar() > aStartChar ||
			StartChar() != aNewCode.StartChar() || EndChar() != aNewCode.EndChar())
			break;
		Skip();
		have_old_line = Next();
		aNewCode.Skip();
		have_new_line = aNewCode.Next();
		}

	// Move after the reformatted range.
	while (have_old_line && StartChar() < aStartChar + aOldLength)
		{
		Skip();
		have_old_line = Next();
		}
	while (have_new_line && aNewCode.StartChar() < aStartChar + aNewLength)
		{
		aRedrawRect.iBr.iY = aNewCode.LineInfo().iOuterRect.iBr.iY;
		aNewCode.Skip();
		have_new_line = aNewCode.Next();
		}

	// Compare lines after the reformatted range.
	int length_change = aNewLength - aOldLength;
	while (have_old_line && have_new_line)
		{
		if (StartChar() + length_change == aNewCode.StartChar() && EndChar() + length_change == aNewCode.EndChar())
			{
			/*
			These lines are identical so don't need to be redrawn, so remove the new line's rectangle from
			the redraw rectangle, and stop comparing, because subsequent lines will all be identical.
			*/
			aRedrawRect.iBr.iY = aNewCode.LineInfo().iOuterRect.iTl.iY;
			break;
			}

		// Add the current new line to the redraw rectangle.
		aRedrawRect.iBr.iY = aNewCode.LineInfo().iOuterRect.iBr.iY;
		
		// Attempt to synchronise.
		if (EndChar() + length_change < aNewCode.EndChar())
			{
			Skip();
			have_old_line = Next();
			}
		else
			{
			aNewCode.Skip();
			have_new_line = aNewCode.Next();
			}
		}

	// If the old text has run out, extend the redraw rect to cover any remaining text.
	while (have_new_line && !have_old_line)
		{
		aRedrawRect.iBr.iY = aNewCode.LineInfo().iOuterRect.iBr.iY;
		aNewCode.Skip();
		have_new_line = aNewCode.Next();
		}
	}

/**
Resets so that will return ETrue for AtEnd().
*/
void RTmGraphemeInTextChunkIterator::Reset()
	{
	iPosition.iPosInText = 0;
	iLength = 0;
	}

/**
Constructs an iterator that will return ETrue for AtEnd().
*/
RTmGraphemeInTextChunkIterator::RTmGraphemeInTextChunkIterator()
	: iFont(NULL)
	{
	Reset();
	}

void RTmGraphemeInTextChunkIterator::Close()
	{
	if (iFont)
		{
		iFont->Close();
		iFont = NULL;
		}
	
	if (iShapeInfo.IsOpen())
		iShapeInfo.Close();
	}

/**
Begins the iteration.
@param aByteCodeIterator
	The interpreter with the information about the formatting being traversed.
	Must be set to the start of the text chunk to be iterated over. The
	interpreter is not advanced by this function.
@param aCache
	A pointer to a two-byte cache in which to store the results of the
	iteration.
*/
void RTmGraphemeInTextChunkIterator::Begin(
	RTmGeneralInterpreter& aByteCodeIterator,
	TTmGraphemeEdgeInfo (*aCache)[2])
	{
	if (iShapeInfo.IsOpen())
		iShapeInfo.Close();
	iCache = aCache;
	iReverse = aByteCodeIterator.RightToLeft()?
		RTmTextCache::EVisualRightToLeft : RTmTextCache::ELeftToRight;
	iIgnorePosition = -1;
	if (!aByteCodeIterator.VisualEndOfLineIsAmbiguous())
		iIgnorePosition = aByteCodeIterator.LineInfo().iEnd;

	if (aByteCodeIterator.Op() != TTmInterpreter::EOpText)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_RTMGRAPHEMEINTEXTCHUNKITERATOR_BEGIN, "EInvariant" );
	    }
	__ASSERT_DEBUG(aByteCodeIterator.Op() == TTmInterpreter::EOpText, TmPanic(EInvariant));

	if (iFont)
		{
		iFont->Close();
		iFont = NULL;
		}
	iEndChar = aByteCodeIterator.EndChar();
	iStartChar = aByteCodeIterator.StartChar();
	iLength = iEndChar - aByteCodeIterator.StartChar();
	if (KMaxTextChunkSize < iLength)
		{
		iLength = KMaxTextChunkSize;
		iEndChar = iStartChar + iLength;
		}
	
	// If a custom interface to draw the text in context exists, then get the Displayed Text within context.
	if (aByteCodeIterator.TextCache().Source().GetExtendedInterface(KTmCustomExtensionUid))
		{
		aByteCodeIterator.TextCache().GetDisplayedText(iStartChar,iEndChar,iReverse,iBuffer,aByteCodeIterator.ContextCharChar(),0,
		&iFont);
		}
		else
			{
			aByteCodeIterator.TextCache().GetDisplayedText(iStartChar,iEndChar,iReverse,iBuffer,0,0,&iFont);
			}
	
	if (!iFont)
		{
		iPosition.iPosInText = 0;
		iLength = 0;
		return;
		}

	// start_char and end_char need to be adjusted to take account of
	// the extra context supplied by GetDisplayedText.
	--iStartChar;
	++iEndChar;
	++iLength;
	iPosition.iText.Set(iBuffer, iLength + 1);
	iPosition.iPosInText = 1;
	iPosition.iPen = aByteCodeIterator.StartPen();

	iStartPenX = iPosition.iPen.iX;
	iEndPenX = aByteCodeIterator.EndPen().iX;
	}

/** Find any protruding left side-bearing.
@return Maximum of aStartOfChunk - left bound of each glyph.
@internalComponent */
TInt LeftSideBearing(const TInt aStartOfChunk, const CFont::TPositionParam& aPos)
	{
	TInt bearing(aStartOfChunk);
	TInt glyph(aPos.iOutputGlyphs);
	while (glyph != 0)
		{
		--glyph;
		TInt left(aPos.iOutput[glyph].iBounds.iTl.iX);
		if (left < bearing)
			bearing = left;
		}
	return aStartOfChunk - bearing;
	}

/**
Advances the iteration to the next character. The aCache array as passed to
Begin will be overwritten with the results.
@return
	The number of characters found; 1 or 2.
*/
TInt RTmGraphemeInTextChunkIterator::Next()
	{
	if (AtEnd())
	    {
	    OstTrace0( TRACE_DUMP, RTMGRAPHEMEINTEXTCHUNKITERATOR_NEXT, "EBadArg" );
	    }
	__ASSERT_DEBUG(!AtEnd(), TmPanic(EBadArg));
	TInt previousPositionInBuffer = iPosition.iPosInText;
	TInt previousPosition = iReverse?
		iEndChar - iPosition.iPosInText : iStartChar + iPosition.iPosInText;
	TPoint previousPen = iPosition.iPen;
	
	//The return value of GetCharacterPosition2() is not used here
	//form and gdi has had correct behavior when GetCharacterPosition2() return EFalse
	//And that was done by changing iPosInText inside GetCharacterPosition2()
	TBool isGlyphGenerated = iFont->Font().GetCharacterPosition2(iPosition, iShapeInfo);
	
	// Move the pen (cursor position) to the right if the cluster just seen
	// has a negative side-bearing. This corrects for the fact that
	// DrawText() pushes text with left side-bearings to the right.
	iPosition.iPen.iX += LeftSideBearing(iStartPenX, iPosition);
	TInt numCodePoints = iPosition.iPosInText - previousPositionInBuffer;
	TInt currentPosition = iReverse?
		iEndChar - iPosition.iPosInText : iStartChar + iPosition.iPosInText;
	if (currentPosition == previousPosition)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_RTMGRAPHEMEINTEXTCHUNKITERATOR_NEXT, "EInvariant" );
	    }
	__ASSERT_DEBUG(currentPosition != previousPosition, TmPanic(EInvariant));

	TInt cachePos = 0;
	// Report the left hand edge of the character.
	if (iIgnorePosition != previousPosition)
		{
		(*iCache)[0].iPos.iRightToLeft = iReverse;
		(*iCache)[0].iPos.iDocPos.iLeadingEdge = !iReverse;
		(*iCache)[0].iPos.iDocPos.iPos = previousPosition;
		(*iCache)[0].iPos.iEdge = previousPen;
		(*iCache)[0].iCodePoints = numCodePoints;
		cachePos = 1;
		}
	// Report the right hand edge of the character.
	if (iIgnorePosition != currentPosition)
		{
		(*iCache)[cachePos].iPos.iRightToLeft = iReverse;
		(*iCache)[cachePos].iPos.iDocPos.iLeadingEdge = iReverse;
		(*iCache)[cachePos].iPos.iDocPos.iPos = currentPosition;
		(*iCache)[cachePos].iPos.iEdge = iPosition.iPen;
		(*iCache)[cachePos].iCodePoints = numCodePoints;
		if (iPosition.iPosInText == iLength)
			{
			// We are probably moving over a chunk, so its right edge
			// will be the end of the chunk, not where the renderer
			// thinks it is.
			if ((*iCache)[cachePos].iPos.iEdge.iX < iEndPenX)
				(*iCache)[cachePos].iPos.iEdge.iX = iEndPenX;
			}
		++cachePos;
		}
	if (0 >= cachePos)
	    {
	    OstTrace0( TRACE_DUMP, DUP2_RTMGRAPHEMEINTEXTCHUNKITERATOR_NEXT, "EInvariant" );
	    }
	__ASSERT_DEBUG(0 < cachePos, TmPanic(EInvariant));
	return cachePos;
	}

/**
Begins the iteration.
@param aByteCodeIterator
	The interpreter with the information about the formatting being traversed.
	Must be set to the start of the text chunk to be iterated over. The
	interpreter is not advanced by this function.
*/
RTmGraphemeInTextChunkIteratorNice::RTmGraphemeInTextChunkIteratorNice(
	RTmGeneralInterpreter& aByteCodeIterator)
	{
	iBase.Begin(aByteCodeIterator, &iCache);
	iCurrent = iCache;
	iEnd = iCache;
	if (!iBase.AtEnd())
		iEnd += iBase.Next();
	}

void RTmGraphemeInTextChunkIteratorNice::Close()
	{
	iBase.Close();
	}

/**
Advances the iteration to the next edge.
*/
void RTmGraphemeInTextChunkIteratorNice::Next()
	{
	if (AtEnd())
	    {
	    OstTrace0( TRACE_DUMP, RTMGRAPHEMEINTEXTCHUNKITERATORNICE_NEXT, "EBadArg" );
	    }
	__ASSERT_DEBUG(!AtEnd(), TmPanic(EBadArg));
	// Return if we have not finished moving through the cache.
	if (++iCurrent != iEnd)
		return;
	// Return if there are no more characters.
	if (iBase.AtEnd())
		return;
	iCurrent = iCache;
	iEnd = iCache + iBase.Next();
	}

/**
Returns a pointer to the current element.
@return
	The current element in the iteration. Will be undefined if AtEnd is
	returning ETrue.
*/
const TTmGraphemeEdgeInfo* RTmGraphemeInTextChunkIteratorNice::Get() const
	{
	return iCurrent;
	}

/**
Reports whether the iteration has finished.
@return
	EFalse if the iteration has not finished. If ETrue is returned, Get will
	return a defined value.
*/
TBool RTmGraphemeInTextChunkIteratorNice::AtEnd() const
	{
	return iCurrent == iEnd? ETrue : EFalse;
	}

/**
Advances to the position specified. Behaviour is undefined if the position does
not exist in the chunk. The position is considered to exist if it is within a
grapheme cluster in the chunk, even if the specific edge does not exist.
@param aDocPos
	Position to advance to.
*/
void RTmGraphemeInTextChunkIteratorNice::FindEdge(const TTmDocPos& aDocPos)
	{
	if (AtEnd())
	    {
	    OstTrace0( TRACE_DUMP, RTMGRAPHEMEINTEXTCHUNKITERATORNICE_FINDEDGE, "EBadArg" );
	    }
	__ASSERT_DEBUG(!AtEnd(), TmPanic(EBadArg));
	while (RTmGraphemeEdgeIterator::ETotalMatch !=
		RTmGraphemeEdgeIterator::DocPosMatches(aDocPos, *Get()))
		{
		Next();
		__ASSERT_DEBUG(!AtEnd(), TmPanic(EBadArg));
		}
	}

void TTmVisualDocPos::Reset()
	{
	iAmbiguity = ENotFound;
	}

TTmVisualDocPos::TTmVisualDocPos()
	{
	Reset();
	}

void TTmVisualDocPos::SetAsUnambiguous(const TTmPosInfo2& aPos)
	{
	iLeft = iRight = aPos;
	iAmbiguity = EUnambiguous;
	}

void TTmVisualDocPos::SetIfAmbiguous(
	const TTmPosInfo2& aLeft, const TTmPosInfo2& aRight)
	{
	if (aLeft.iDocPos.iPos == aRight.iDocPos.iPos
		&& aLeft.iEdge.iX == aRight.iEdge.iX)
		// These edges do meet.
		return;

	// There is an ambiguity, but which edges are involved?
	TBool leftIsRightHandEdge = aLeft.iRightToLeft?
		aLeft.iDocPos.iLeadingEdge : !aLeft.iDocPos.iLeadingEdge;
	TBool rightIsLeftHandEdge = aRight.iRightToLeft?
		!aRight.iDocPos.iLeadingEdge : aRight.iDocPos.iLeadingEdge;

	if (leftIsRightHandEdge)
		{
		iLeft = aLeft;
		if (rightIsLeftHandEdge)
			{
			iRight = aRight;
			iAmbiguity = EAmbiguous;
			}
		else
			{
			iRight = aLeft;
			iAmbiguity = ELeftOnly;
			}
		}
	else if (rightIsLeftHandEdge)
		{
		iLeft = iRight = aRight;
		iAmbiguity = ERightOnly;
		}
	}

/**
Return the position that best matches the X co-ordinate specified by aX.
@param aX X coordinate sought.
@return
	0 if no value was found. A pointer to the result if a value was found.
*/
const TTmPosInfo2* TTmVisualDocPos::NearestToX(TInt aX) const
	{
	if (iAmbiguity == ENotFound)
		return 0;
	TInt leftDistance = iLeft.iEdge.iX < aX?
		aX - iLeft.iEdge.iX : iLeft.iEdge.iX - aX;
	TInt rightDistance = iRight.iEdge.iX < aX?
		aX - iRight.iEdge.iX : iRight.iEdge.iX - aX;
	if (leftDistance < rightDistance)
		return &iLeft;
	else
		return &iRight;
	}

/** Restarts the checker for a new line. */
void TTmAmbiguityChecker::Reset()
	{
	iPrev.iEdge.iX = KMinTInt;
	iPrev.iRightToLeft = EFalse;
	iPrev.iDocPos.iPos = KMinTInt;
	iPrev.iDocPos.iLeadingEdge = ETrue;
	iLastAmbiguousPos.Reset();
	}

/** Constructs a checker ready to check a line. */
TTmAmbiguityChecker::TTmAmbiguityChecker()
	{
	Reset();
	}

/**
Checks an edge for ambiguity against the last edge added.
@param aEdge The edge to check and store.
*/
void TTmAmbiguityChecker::AddEdge(const TTmPosInfo2& aEdge)
	{
	iLastAmbiguousPos.SetIfAmbiguous(iPrev, aEdge);
	iPrev = aEdge;
	}

/**
Checks whether the last edge added is ambiguous if there are no more edges in
the line.
*/
void TTmAmbiguityChecker::EndLine()
	{
	TTmPosInfo2 dummyEdge;
	dummyEdge.iEdge.iX = KMaxTInt;
	dummyEdge.iRightToLeft = EFalse;
	dummyEdge.iDocPos.iLeadingEdge = EFalse;
	dummyEdge.iDocPos.iPos = KMaxTInt;
	AddEdge(dummyEdge);
	}

/**
Reports whether the iteration is over.
@return	ETrue if Next may no longer be called as there is no more information.
*/
TBool RTmGraphemeInTextChunkIterator::AtEnd() const
	{
	return iLength <= iPosition.iPosInText;
	}

/**
 Releases all resources
*/
void RTmGraphemeEdgeIterator::Close()
	{
	iTextChunkIterator.Close();
	}
	
/**
Advances the iteration to the next character edge. Note that this one may be in
the same place as the last.

Begin must be called before this function can be used.
@see Begin
*/
void RTmGraphemeEdgeIterator::Next()
	{
	if (AtEnd())
	    {
	    OstTrace0( TRACE_DUMP, RTMGRAPHEMEEDGEITERATOR_NEXT, "EBadArg" );
	    }
	__ASSERT_DEBUG(!AtEnd(), TmPanic(EBadArg));
	--iCacheSize;
	if (0 != iCacheSize)
		{
		++iCachePos;
		return;
		}

	iCachePos = 0;

	// more positions within this text chunk?
	if (!iTextChunkIterator.AtEnd())
		{
		iCacheSize = iTextChunkIterator.Next();
		return;
		}

	// Get the next position in this line
	while (iByteCodeIterator->CodePos() < iEndLineCodePos)
		{
		// The WHILE loop assures that there is always sufficientByteCode avaliable.
		TBool haveSufficientByteCode = iByteCodeIterator->Next();
		if (!haveSufficientByteCode)
		    {
		    OstTrace0( TRACE_DUMP, DUP1_RTMGRAPHEMEEDGEITERATOR_NEXT, "EBadArg" );
		    }
		__ASSERT_DEBUG(haveSufficientByteCode, TmPanic(EBadArg));

		iByteCodeIterator->Skip();
		TInt op = iByteCodeIterator->Op();
		if (op == TTmInterpreter::EOpSpecialChar)
			{
			TBool reverse = iByteCodeIterator->RightToLeft();
			TInt endChar = iByteCodeIterator->EndChar();
			TInt startChar = iByteCodeIterator->StartChar();
			TInt actualEndChar = reverse? startChar : endChar;		// Based on direction of text
			TInt actualStartChar = reverse? endChar : startChar;	// Based on direction of text
			iPosInfoCache[0].iPos.iDocPos.iPos = reverse? endChar : startChar;
			iPosInfoCache[0].iPos.iDocPos.iLeadingEdge = !reverse;
			iPosInfoCache[0].iPos.iRightToLeft = reverse;
			iPosInfoCache[0].iPos.iEdge = iByteCodeIterator->StartPen();
			iPosInfoCache[0].iCodePoints = 1;
			/**	If we have reached the end edge (for LTR text), do not cache the next character in the bytecode. 
				Simply return.*/
			if (actualEndChar == iIgnorePosition)
				{
				iCacheSize = 1;
				return;
				}
			iPosInfoCache[1].iPos.iDocPos.iPos = reverse? startChar : endChar;
			iPosInfoCache[1].iPos.iDocPos.iLeadingEdge = reverse;
			iPosInfoCache[1].iPos.iRightToLeft = reverse;
			iPosInfoCache[1].iPos.iEdge = iByteCodeIterator->EndPen();
			iPosInfoCache[1].iCodePoints = 1;
			iCacheSize = 2;
			/** If we have reached the start edge (for RTL text), do not cache the next character in the bytecode.
				Simply return.*/
			if (actualStartChar == iIgnorePosition)
				{
				iCacheSize = 1;
				iCachePos = 1;
				return;
				}
			// Otherwise, simply return with both cache positions set, cache size of 2, and position in cache 0
			return;
			}
		else if (op == TTmInterpreter::EOpText)
			{
			iTextChunkIterator.Begin(*iByteCodeIterator, &iPosInfoCache);
			if (!iTextChunkIterator.AtEnd())
				{
				iCacheSize = iTextChunkIterator.Next();
				return;
				}
			}
		}

	// No more positions within the line.
	if (!iTrailingEdgeOfUnambiguousLineEndAdded
		&& iByteCodeIterator->TrailingEdgeOfUnambiguousLineEnd(
		iPosInfoCache[0].iPos, ETrue))
		{
		// Got the trailing edge of the previous line's last character.
		iPosInfoCache[0].iCodePoints = 1;
		iCacheSize = 1;
		iTrailingEdgeOfUnambiguousLineEndAdded = ETrue;
		return;
		}
	}

/**
Gets the current edge, if the iteration is not at an end.

Begin must be called before this function can be used.
@see Begin
@see AtEnd

@return
	The current edge co-ordinates in formatting space, the character
	index that the position is just before and whether the edge is the trailing
	edge of the previous character or the leading edge of the next character.
	Undefined if the iteration is at an end. Call AtEnd to see if this is the
	case.
*/
const TTmGraphemeEdgeInfo& RTmGraphemeEdgeIterator::Get() const
	{
	return iPosInfoCache[iCachePos];
	}

/**
Gets the current edge, if the iteration is not at an end. A shortcut for
Get().iPos.

Begin must be called before this function can be used.
@see Begin
@see Get
@see AtEnd

@return
	The current edge co-ordinates in formatting space, the character index that
	the position is just before and whether the edge is the trailing edge of
	the previous character or the leading edge of the next character. Undefined
	if the iteration is at an end. Call AtEnd to see if this is the case.
*/
const TTmPosInfo2& RTmGraphemeEdgeIterator::GetInfo() const
	{
	return iPosInfoCache[iCachePos].iPos;
	}

/**
Determines if the iteration has finished the line or not.
@return ETrue if the iteration is at an end.
*/
TBool RTmGraphemeEdgeIterator::AtEnd() const
	{
	return iCacheSize == 0? ETrue : EFalse;
	}

/**
Begins the iteration again from the start of the line. The iterator passed in
will not be advanced beyond the current line.
@param aByteCodeIterator
	Iterator to the formatting which should be set to the start of the
	formatting to be traversed. It will be advanced during the use of this
	iterator. It should not be used by any other code while this
	RTmGraphemeEdgeIterator is being used.
 */
void RTmGraphemeEdgeIterator::Begin(
	RTmGeneralInterpreter& aByteCodeIterator)
	{
	/**	Set the position of the edge of the line to stop at when iterating through the edges */
	if (!aByteCodeIterator.VisualEndOfLineIsAmbiguous())
		iIgnorePosition = aByteCodeIterator.LineInfo().iEnd;	
	iByteCodeIterator = &aByteCodeIterator;
	iEndLineCodePos = iByteCodeIterator->EndCodePos();
	iTextChunkIterator.Reset();
	iCachePos = 0;
	iCacheSize = 1;
	// Get the first value if there is one
	if (iByteCodeIterator->TrailingEdgeOfUnambiguousLineEnd(
		iPosInfoCache[0].iPos, EFalse))
		{
		// Got the trailing edge of the previous line's last character.
		iPosInfoCache[0].iCodePoints = 1;
		iTrailingEdgeOfUnambiguousLineEndAdded = ETrue;
		return;
		}
	iTrailingEdgeOfUnambiguousLineEndAdded = EFalse;
	Next();
	}

/**
Finds the nearest edge to the x position specified. The iterator should be
initialised to the start of the line to be searched.
@param aX The X position to search for (in formatting coordinates).
@param aNearest The result.
*/
void RTmGraphemeEdgeIterator::FindXPos(TInt aX, TTmVisualDocPos& aNearest)
	{
	TTmPosInfo2 nearest;
	if (!FindEdgeByX(aX, nearest, aNearest))
		aNearest.Reset();
	else if (aNearest.Ambiguity() == TTmVisualDocPos::ENotFound)
		aNearest.SetAsUnambiguous(nearest);
	}

/**
Finds the nearest edge to the x position specified. The iterator should be
initialised to the start of the line to be searched.
@param aX The position to find (in formatting coordinates).
@param aNearest
	The nearest position to aX, if ETrue is returned.
@param aAmbiguity
	Any ambiguity visually coincident with aNearest if ETrue is returned.
@return
	ETrue if there is any position there at all. If EFalse, aNearest and
	aAmbiguity are undefined.
*/
TBool RTmGraphemeEdgeIterator::FindEdgeByX(TInt aX, TTmPosInfo2& aNearest,
	TTmVisualDocPos& aAmbiguity)
	{
	TInt lastX = KMinTInt;
	TInt distance = KMaxTInt;
	TTmAmbiguityChecker checker;
	TTmPosInfo2 found;
	// Find the last edge before aX
	while (!AtEnd() && GetInfo().iEdge.iX <= aX)
		{
		found = GetInfo();
		if (lastX != found.iEdge.iX)
			{
			lastX = found.iEdge.iX;
			distance = aX - lastX;
			checker.Reset();
			}
		checker.AddEdge(found);
		Next();
		}
	// Find the first edge after aX if it is closer.
	if (!AtEnd() && GetInfo().iEdge.iX - aX < distance)
		{
		checker.Reset();
		found = GetInfo();
		lastX = found.iEdge.iX;
		checker.AddEdge(found);
		Next();
		while (!AtEnd() && GetInfo().iEdge.iX == lastX)
			{
			checker.AddEdge(GetInfo());
			Next();
			}
		}
	if (AtEnd())
		checker.EndLine();
	aAmbiguity = checker.LastAmbiguity();
	if (lastX == KMinTInt)
		return EFalse;
	aNearest = found;
	return ETrue;
	}

/**
Determines if aDocPos matches the grapheme edge defined by aEdgeInfo.
@param aDocPos The document position specification to match.
@param aEdgeInfo The grapheme edge to match.
@return
	ENoMatch if the document position does not match the grapheme edge.
	EPositionOnly if the document position edge specification is ERightToLeft
	or ELeftToRight and the document position matches but not the
	directionality. ETotalMatch if the document position and edge
	specifications match.
*/
RTmGraphemeEdgeIterator::TGraphemeMatch
RTmGraphemeEdgeIterator::DocPosMatches(const TTmDocPosSpec& aDocPos,
	const TTmGraphemeEdgeInfo& aEdgeInfo)
	{
	TInt startOfGrapheme = aEdgeInfo.iPos.iDocPos.iPos - 
		(aEdgeInfo.iPos.iDocPos.iLeadingEdge? 0 : aEdgeInfo.iCodePoints - 1);
	if (startOfGrapheme <= aDocPos.iPos
		&& aDocPos.iPos < startOfGrapheme + aEdgeInfo.iCodePoints)
		{
		// Found a grapheme that matches the document position.
		// Does it match the rest of the spec?
		switch (aDocPos.iType)
			{
		case TTmDocPosSpec::ELeftToRight:
			return aEdgeInfo.iPos.iRightToLeft?
				EPositionOnly : ETotalMatch;
		case TTmDocPosSpec::ERightToLeft:
			return aEdgeInfo.iPos.iRightToLeft?
				ETotalMatch : EPositionOnly;
		case TTmDocPosSpec::ELeading:
			return aEdgeInfo.iPos.iDocPos.iLeadingEdge?
				ETotalMatch : ENoMatch;
		case TTmDocPosSpec::ETrailing:
			return aEdgeInfo.iPos.iDocPos.iLeadingEdge?
				ENoMatch : ETotalMatch;
		default:
			return ENoMatch;
			}
		}
	return ENoMatch;
	}

/**
Finds the edge specified. If a Right-to-Left or Left-to-Right edge is demanded,
an edge with the wrong directionality will be returned if the right one does
not exist, but one or two with the wrong one does. The iterator should be
initialised to the start of the line to be searched.
@param aDocPos The position to be found.
@param aInfo The result.
@return ETrue if a result could be found.
*/
TBool RTmGraphemeEdgeIterator::FindEdge(const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aInfo)
	{
	TInt matches = 0;
	while (!AtEnd())
		{
		TTmGraphemeEdgeInfo info = Get();
		TInt match = static_cast<TInt>(DocPosMatches(aDocPos, info));
		if (0 != match)
			{
			matches += match;
			aInfo = info.iPos;
			if (2 <= matches)
				return ETrue;
			}
		Next();
		}
	return matches;
	}

/**
Finds the rightmost edge that is to the left of aDocPos in this line.
@param aDocPos Document position that the result must be to the left of.
@param aNext Result. Undefined if 0 or 1 is returned.
@param aNearest Information about aDocPos itself. Undefined if 0 is returned.
@return
	2 if both aNearest and aNext have a result, 1 if only aNearest does and 0
	if neither were found.
*/
RTmGraphemeEdgeIterator::TEdgesFound
RTmGraphemeEdgeIterator::FindEdgeLeftwards(const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aNearest, TTmVisualDocPos& aNext)
	{
	TInt matches = 0;
	TTmPosInfo2 last;
	last = GetInfo();
	TTmAmbiguityChecker checker;
	TTmVisualDocPos lastXVisualPosition;
	while (!AtEnd() && matches < 2)
		{
		if (last.iEdge.iX != GetInfo().iEdge.iX)
			{
			lastXVisualPosition = checker.LastAmbiguity();
			if (lastXVisualPosition.Ambiguity() == TTmVisualDocPos::ENotFound)
				lastXVisualPosition.SetAsUnambiguous(last);
			checker.Reset();
			}
		last = GetInfo();
		checker.AddEdge(last);
		TInt match = static_cast<TInt>(DocPosMatches(aDocPos, Get()));
		if (match != 0)
			{
			aNearest = last;
			aNext = lastXVisualPosition;
			matches += match;
			}
		Next();
		}
	if (matches == 0)
		return ENone;
	return (aNext.Ambiguity() == TTmVisualDocPos::ENotFound)?
		ENearestOnly : ENearestAndNext;
	}

/**
Finds the leftmost edge that is to the right of aDocPos in this line.
@param aDocPos Document position that the result must be to the right of.
@param aNext Result. Undefined if 0 or 1 is returned.
@param aNearest Information about aDocPos itself. Undefined if 0 is returned.
@return
	2 if both aNearest and aNext have a result, 1 if only aNearest does and 0
	if neither were found.
*/
RTmGraphemeEdgeIterator::TEdgesFound
RTmGraphemeEdgeIterator::FindEdgeRightwards(const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aNearest, TTmVisualDocPos& aNext)
	{
	if (AtEnd())
		return ENone;

	RTmGraphemeEdgeIterator::TEdgesFound result = ENone;
	TTmAmbiguityChecker checker;
	TTmGraphemeEdgeInfo info = Get();
	TTmPosInfo2 firstOfLastX = info.iPos;
	TInt lastX = info.iPos.iEdge.iX;
	TInt nearestPositionX = KMinTInt;
	TInt nextPositionX = KMinTInt;
	TInt matches = 0;
	while (!AtEnd())
		{
		info = Get();
		TInt x = info.iPos.iEdge.iX;

		//Are we moving to a new postion?
		if (x != lastX)
			{
			if (lastX == nearestPositionX)
				{
				// Moving from nearest position to next position.
				// Need to start checking for ambiguity.
				nextPositionX = x;
				checker.Reset();
				}
			else if (lastX == nextPositionX)
				{
				// Moving off next position. Possibly we have finished.
				aNext = checker.LastAmbiguity();
				if (aNext.Ambiguity() == TTmVisualDocPos::ENotFound)
					aNext.SetAsUnambiguous(firstOfLastX);
				result = ENearestAndNext;
				if (1 < matches)
					return result;
				}
			lastX = x;
			firstOfLastX = info.iPos;
			}

		// If we are in the "next" position (one to the right of the
		// "nearest" position) we need to check for ambiguity.
		if (x == nextPositionX)
			checker.AddEdge(info.iPos);

		// Find out if we are or may be in the "nearest" position.
		TInt match = static_cast<TInt>(DocPosMatches(aDocPos, info));
		if (0 != match && matches < 2)
			{
			// We are in the nearest position, or are in a half-match
			// where the position matches but the directionality does not.
			matches += match;
			aNearest = info.iPos;
			result = ENearestOnly;
			nearestPositionX = x;
			nextPositionX = KMinTInt;
			}

		Next();
		}

	if (lastX == nextPositionX)
		{
		// We reached the end of the line at the "next" position,
		// so we need to finish the ambiguity checking.
		checker.EndLine();
		aNext = checker.LastAmbiguity();
		if (aNext.Ambiguity() == TTmVisualDocPos::ENotFound)
			aNext.SetAsUnambiguous(firstOfLastX);
		return ENearestAndNext;
		}

	return result;
	}

/**
Returns the first trailing edge beyond aDocPos not visually coincident with the
leading edge at aDocPos in this line.
@param aDocPos
	The starting document index.
@return
	The document position found, or KErrNotFound if none was found in this
	line.
*/
TInt RTmGraphemeEdgeIterator::NextPosition(TInt aDocPos)
	{
	TBool currentXIsForbidden = EFalse;
	TInt currentX = KMinTInt;
	TInt lastXBestPos = KMaxTInt;
	TInt currentXBestPos = KMaxTInt;
	while (!AtEnd())
		{
		TInt x = GetInfo().iEdge.iX;
		const TTmDocPos &docPos = GetInfo().iDocPos;
		if (x != currentX)
			{
			currentX = x;
			if (!currentXIsForbidden && currentXBestPos < lastXBestPos)
				lastXBestPos = currentXBestPos;
			currentXBestPos = KMaxTInt;
			currentXIsForbidden = EFalse;
			}
		if (docPos.iLeadingEdge)
			{
			if (docPos.iPos == aDocPos)
				currentXIsForbidden = ETrue;
			}
		else
			{
			if (aDocPos < docPos.iPos && docPos.iPos < currentXBestPos)
				currentXBestPos = docPos.iPos;
			}
		Next();
		}
	if (!currentXIsForbidden && currentXBestPos < lastXBestPos)
		lastXBestPos = currentXBestPos;
	return lastXBestPos == KMaxTInt? KErrNotFound : lastXBestPos;
	}

/**
Returns the last leading edge before aDocPos not visually coincident with the
trailing edge at aDocPos in this line.
@param aDocPos
	The starting document index.
@return
	The document position found, or KErrNotFound if none was found in this
	line.
*/
TInt RTmGraphemeEdgeIterator::PreviousPosition(TInt aDocPos)
	{
	TBool currentXIsForbidden = EFalse;
	TInt currentX = KMinTInt;
	TInt lastXBestPos = KMinTInt;
	TInt currentXBestPos = KMinTInt;
	while (!AtEnd())
		{
		TInt x = GetInfo().iEdge.iX;
		const TTmDocPos &docPos = GetInfo().iDocPos;
		if (x != currentX)
			{
			currentX = x;
			if (!currentXIsForbidden && lastXBestPos < currentXBestPos)
				lastXBestPos = currentXBestPos;
			currentXBestPos = KMinTInt;
			currentXIsForbidden = EFalse;
			}
		if (!docPos.iLeadingEdge)
			{
			if (docPos.iPos == aDocPos)
				currentXIsForbidden = ETrue;
			}
		else
			{
			if (currentXBestPos < docPos.iPos && docPos.iPos < aDocPos)
				currentXBestPos = docPos.iPos;
			}
		Next();
		}
	if (!currentXIsForbidden && lastXBestPos < currentXBestPos)
		lastXBestPos = currentXBestPos;
	return lastXBestPos == KMinTInt? KErrNotFound : lastXBestPos;
	}

/**
 * Gets a line of text as displayed, after bidirectional formatting, conversion
 * of invisible characters to visible forms, and conversion of glyphs to
 * context-dependent forms.
 * @param aLineNumber Line to be retrieved.
 * @param aText Buffer for output.
 * @param aNeeded
 *		Length of buffer required for the output. This will be greater than
 *		aText.MaxLength() if aText was not big enough.
 * @internalComponent
 */
TBool RTmGeneralInterpreter::GetDisplayedTextL(TInt aLineNumber,TDes& aText,TUint aContextChar,TInt& aNeeded)
	{
	aText.SetLength(0);
	aNeeded = 0;
	TBool found = LineNumberToLine(aLineNumber);
	if (!found)
		return FALSE;

	int i = 0;
	while (Next() && Op() != EOpLine)
		switch (Op())
			{
			case EOpText:
			case EOpSpecialChar:
				{
				TText buffer[KMaxTextChunkSize + 2];
				CTmTextFontCache* font = 0;
				RTmTextCache::TDisplayedTextDirectionality directionality =
					RightToLeft()?
						RTmTextCache::EVisualRightToLeft
						: RTmTextCache::ELeftToRight;
				User::LeaveIfError(iTextCache.GetDisplayedText(StartChar(), EndChar(),
					directionality, buffer,aContextChar, 0, &font));
				if (!font)
				    {
				    OstTrace0( TRACE_DUMP, RTMGENERALINTERPRETER_GETDISPLAYEDTEXTL, "EInvariant" );
				    }
				__ASSERT_DEBUG(font, TmPanic(EInvariant));
				CFont::TPositionParam p;
				int length = EndChar() - StartChar() + 1;
				p.iText.Set(buffer, length + 1);
				p.iPosInText = 1;
				TInt textMaxLength = aText.MaxLength();
				RShapeInfo shapeInfo;
				while (p.iPosInText < length)
					{
					if(!font->Font().GetCharacterPosition2(p, shapeInfo))
						{
						//The iPosInText changed after calling the GetCharacterPosition2()
						continue;
						}
					
					aNeeded += p.iOutputGlyphs;
					for (i = 0; i < p.iOutputGlyphs; i++)
						{
						TInt code = p.iOutput[i].iCode;
						if (code <= 0xFFFF)
							{
							// non-surrogate character
							if (aText.Length() < textMaxLength)
								aText.Append(code);
							}
						else if (code <= 0x10FFFF)
							{
							// surrogate pair
							++aNeeded;
							if (aText.Length() < textMaxLength)
								aText.Append(0xD7C0 + (code >> 10));
							if (aText.Length() < textMaxLength)
								aText.Append(0xDC00 | (code & 0x3FF));
							}
						else
							{
							// Glyph code; cannot be specified as Unicode.
							// Instead add 0xFFFD the Unicode Replacement
							// Character
							if (aText.Length() < textMaxLength)
								aText.Append(0xFFFD);
							}
						}
					}
				font->Close();
				if (shapeInfo.IsOpen())
					shapeInfo.Close();
				}
				break;

			case EOpInlineText:
				for (i = 0; i < InlineText().Length(); i++)
					{
					aNeeded++;
					if (aText.Length() < aText.MaxLength())
						aText.Append(InlineText()[i]);
					}
				break;

			default:
				Skip();
				break;
			}

	return TRUE;
	}

/**
 * Iterates past an X-Y position and returns the position information for
 * the nearest edge to that point.
 * @param aXyPos The point to find.
 * @param aInfo Returns information about the edge found.
 * @return
 *		ETrue if the operation was successful. It will succeed if the line
 *		containing aXyPos is formatted.
 * @internalComponent
 */
TBool RTmGeneralInterpreter::FindXyPos(const TPoint& aXyPos,TTmPosInfo2& aInfo)
	{
	if (!YPosToLine(aXyPos.iY))
		return EFalse;

	aInfo.iDocPos = StartDocPos();
	aInfo.iEdge = StartPen();

	RTmGraphemeEdgeIterator edgeIterator;
	edgeIterator.Begin(*this);
	TTmVisualDocPos dummy;
	if (!edgeIterator.FindEdgeByX(aXyPos.iX, aInfo, dummy))
		return EFalse;
	edgeIterator.Close();
	return ETrue;
	}

/**
 * Iterates to a document position and returns information about it.
 * @param aDocPos The position to find.
 * @param aInfo Returns information about the position found.
 * @return
 *		ETrue if the operation was successful. It will succeed if the line
 *		containing aDocPos is formatted.
 * @internalComponent
 */
TBool RTmGeneralInterpreter::FindDocPos(const TTmDocPosSpec& aDocPos,TTmPosInfo2& aInfo)
	{
	if (!DocPosToLine(aDocPos))
		return FALSE;

	aInfo.iDocPos = StartDocPos();
	aInfo.iEdge = StartPen();

	RTmGraphemeEdgeIterator edgeIterator;
	edgeIterator.Begin(*this);
	edgeIterator.FindEdge(aDocPos, aInfo);
	edgeIterator.Close();
	return TRUE;
	}

/**
Gets the trailing edge of the previous line's last character, if this position
is unambiguous and if the current position is at the visual start of the line.
Which end the visual start of the line is depends on the paragraph
directionality of the current paragraph.
@param aPosOut The position found, if any.
@param aOnRight Are we currently on the right hand extreme of the line?
@return ETrue if there is such a position at the specified end.
@internalComponent
*/
TBool RTmGeneralInterpreter::TrailingEdgeOfUnambiguousLineEnd(
	TTmPosInfo2& aPosOut, TBool aOnRight) const
	{
	if (VisualStartOfLineIsAmbiguous())
		return EFalse;
	TBool atStart = LineInfo().iFlags & TTmLineInfo::EParRightToLeft?
		aOnRight : !aOnRight;
	if (!atStart)
		return EFalse;
	aPosOut.iDocPos.iLeadingEdge = EFalse;
	aPosOut.iDocPos.iPos = LineInfo().iStart;
	aPosOut.iEdge.iY = LineInfo().iBaseline;
	aPosOut.iEdge.iX = aOnRight?
		LineInfo().iInnerRect.iBr.iX : LineInfo().iInnerRect.iTl.iX;
	aPosOut.iRightToLeft = aOnRight;
	return ETrue;
	}

RTmDrawingInterpreter::RTmDrawingInterpreter(MTmSource& aSource,CGraphicsContext& aGc,
											 const TTmInterpreterParam& aParam,const TPoint& aTopLeft,
											 const TRect& aClipRect,const TLogicalRgb* aDocBackground,
											 TUint aFlags,const TCursorSelection* aHighlight,
											 const TTmHighlightExtensions* aHighlightExtensions,
											 TBool aDrawInvertedHighlight):
	RTmGeneralInterpreter(aSource,aParam,&aTopLeft),
	iGc(aGc),
	iClipRect(aClipRect),
	iFirstLine(TRUE),
	iLabelled(FALSE),
	iLabelLeft(KMaxTInt),
	iLabelRight(KMinTInt),
	iDocBackground(aDocBackground),
	iFlags(aFlags | aParam.iDrawingInterpFlags),
	iHighlightStartPos(0),
	iHighlightEndPos(0),
	iHighlightExtensions(NULL),
	iHighlightedLineRect(),
	iHighlightedLineRgn(4), // granularity = 4
	iSource(aSource),
	iParam(aParam),
	iDrawInvertedHighlight(EFalse),
	iHighlightXorColor()
	{
	iHighlightStartPos = 0;
	iHighlightEndPos = 0;
	if (aHighlight)
		{
		if (aHighlight->iAnchorPos < aHighlight->iCursorPos)
			{
			iHighlightStartPos = aHighlight->iAnchorPos;
			iHighlightEndPos = aHighlight->iCursorPos;
			}
		else
			{
			iHighlightStartPos = aHighlight->iCursorPos;
			iHighlightEndPos = aHighlight->iAnchorPos;
			}
		}

	if (NULL != aHighlightExtensions && ! aHighlightExtensions->IsNull())
		{
		iHighlightExtensions = aHighlightExtensions;
		}

	TTmHighlightSource highlightSource(aSource);
	iDrawInvertedHighlight = aDrawInvertedHighlight || ! highlightSource.HighlightVisible();
	if (iDrawInvertedHighlight)
		{
		 // Explicit construction of the 2nd parameter for SystemColor, TRgb, ensuring opaque alpha blending.
		 // FF is opaque alpha channel value. 
		TRgb defaultColorOfBackground(static_cast<TUint32>(TLogicalRgb::ESystemBackgroundColor), 0xFF); 
		TRgb backgroundColor = aSource.SystemColor(TLogicalRgb::ESystemBackgroundIndex,defaultColorOfBackground);
    
		// Explicit construction of the 2nd parameter for SystemColor, TRgb, ensuring opaque alpha blending.
		// FF is opaque alpha channel value.
		TRgb defaultColorOfXorColor(static_cast<TUint32>(TLogicalRgb::ESystemSelectionBackgroundColor), 0xFF); 
		TRgb xorColor = aSource.SystemColor(TLogicalRgb::ESystemSelectionBackgroundIndex,defaultColorOfXorColor);

		iHighlightXorColor = backgroundColor ^ xorColor;
		}
	}

RTmDrawingInterpreter::~RTmDrawingInterpreter()
	{
	iHighlightedLineRgn.Close();
	}

TBool RTmDrawingInterpreter::GetHighlightPos(TInt& aHighlightStartPos, TInt& aHighlightEndPos) const
	{
	aHighlightStartPos = aHighlightEndPos = 0;

	if (iHighlightEndPos > iHighlightStartPos)
		{
		aHighlightStartPos = iHighlightStartPos;
		if (StartDocPos().iPos > iHighlightStartPos)
			{
			aHighlightStartPos = StartDocPos().iPos;
			}

		aHighlightEndPos = iHighlightEndPos;
		if (EndDocPos().iPos < iHighlightEndPos)
			{
			aHighlightEndPos = EndDocPos().iPos;
			}
		}

	return (aHighlightEndPos > aHighlightStartPos);
	}

TBool RTmDrawingInterpreter::GetHighlightClipRegion(const TRect& aClipRect, RRegion& aHighlightRegion) const
	{
	if (UsesAdjustedHighlight())
		{
		return GetAdjustedHighlightClipRegion(aClipRect, aHighlightRegion);
		}

	aHighlightRegion.Clear();

	TInt highlightStartPos = 0;
	TInt highlightEndPos = 0;
	if (GetHighlightPos(highlightStartPos, highlightEndPos))
		{
		TRect rect(TRect::EUninitialized);
		TTmDocPos start(highlightStartPos, ETrue);
		TTmDocPos end(highlightEndPos, EFalse);
		RTmBoundingRectInterpreter interpreter(iSource, iParam);
		TBool found = interpreter.FirstRect(start.iPos, end.iPos, rect);
		while (found)
			{
			if (!rect.IsEmpty())
				{
				rect.Move(TextLayoutTopLeft());
				aHighlightRegion.AddRect(rect);

				if (aHighlightRegion.CheckError())
					{
					break;
					}
				else
					{
					aHighlightRegion.Tidy();
					}
				}

			found = interpreter.NextRect(rect);
			}

		interpreter.Close();
		return ! aHighlightRegion.CheckError();
		}
	else
		{
		return ETrue;
		}
	}

TBool RTmDrawingInterpreter::GetFirstHighlightClipRect(RTmBoundingRectInterpreter& aInterpreter,
													   const TRect& aClipRect, TRect& aRect) const
	{
	if (UsesAdjustedHighlight())
		{
		return GetFirstAdjustedHighlightClipRect(aInterpreter, aClipRect, aRect);
		}

	TInt highlightStartPos = 0;
	TInt highlightEndPos = 0;
	if (GetHighlightPos(highlightStartPos, highlightEndPos))
		{
		TTmDocPos start(highlightStartPos, ETrue);
		TTmDocPos end(highlightEndPos, EFalse);
		TBool found = aInterpreter.FirstRect(start.iPos, end.iPos, aRect);
		while (found)
			{
			aRect.Move(TextLayoutTopLeft());
			aRect.Intersection(aClipRect);
			if (aRect.IsEmpty())
				{
				found = aInterpreter.NextRect(aRect);
				}
			else
				{
				return ETrue;
				}
			}
		}

	aRect = TRect(TRect::EUninitialized);
	return EFalse;
	}

TBool RTmDrawingInterpreter::GetNextHighlightClipRect(RTmBoundingRectInterpreter& aInterpreter,
													  const TRect& aClipRect, TRect& aRect) const
	{
	if (UsesAdjustedHighlight())
		{
		return GetNextAdjustedHighlightClipRect(aInterpreter, aClipRect, aRect);
		}

	TBool found = aInterpreter.NextRect(aRect);
	while (found)
		{
		aRect.Move(TextLayoutTopLeft());
		aRect.Intersection(aClipRect);
		if (aRect.IsEmpty())
			{
			found = aInterpreter.NextRect(aRect);
			}
		else
			{
			return ETrue;
			}
		}

	aRect = TRect(TRect::EUninitialized);
	return EFalse;
	}

TBool RTmDrawingInterpreter::GetAdjustedHighlightClipRegion(const TRect& aClipRect, RRegion& aHighlightRegion) const
	{
	if (NULL == iHighlightExtensions)
	    {
	    OstTrace0( TRACE_DUMP, RTMDRAWINGINTERPRETER_GETADJUSTEDHIGHLIGHTCLIPREGION, "EInvariant" );
	    }
	__ASSERT_DEBUG(NULL != iHighlightExtensions, TmPanic(EInvariant));

	aHighlightRegion.Clear();

	if (iHighlightEndPos > iHighlightStartPos)
		{
		RTmBoundingRectInterpreter interpreter(iSource, iParam);

		TRect rect(TRect::EUninitialized);
		TBool found = interpreter.FirstRect(iHighlightStartPos, iHighlightEndPos, rect);
		while (found)
			{
			if (! rect.IsEmpty())
				{
				rect.Move(TextLayoutTopLeft());
				iHighlightExtensions->AdjustRect(rect);
				if (rect.iBr.iY < aClipRect.iTl.iY)
					{
					// ignore it
					}
				else if (rect.iTl.iY > aClipRect.iBr.iY)
					{
					break;
					}
				else
					{
					aHighlightRegion.AddRect(rect);
					if (aHighlightRegion.CheckError())
						{
						break;
						}
					else
						{
						aHighlightRegion.Tidy();
						}
					}
				}

			found = interpreter.NextRect(rect);
			}

		interpreter.Close();

		if (aHighlightRegion.CheckError())
			{
			return EFalse;
			}
		
		aHighlightRegion.ClipRect(aClipRect);
		}

	return ETrue;
	}

TBool RTmDrawingInterpreter::GetFirstAdjustedHighlightClipRect(RTmBoundingRectInterpreter& aInterpreter,
															   const TRect& aClipRect, TRect& aRect) const
	{
	if (NULL == iHighlightExtensions)
	    {
	    OstTrace0( TRACE_DUMP, RTMDRAWINGINTERPRETER_GETFIRSTADJUSTEDHIGHLIGHTCLIPRECT, "EInvariant" );
	    }
	__ASSERT_DEBUG(NULL != iHighlightExtensions, TmPanic(EInvariant));

	if (iHighlightEndPos > iHighlightStartPos)
		{
		TBool found = aInterpreter.FirstRect(iHighlightStartPos, iHighlightEndPos, aRect);
		while (found)
			{
			if (! aRect.IsEmpty())
				{
				aRect.Move(TextLayoutTopLeft());
				iHighlightExtensions->AdjustRect(aRect);
				if (aRect.iBr.iY < aClipRect.iTl.iY)
					{
					// ignore it
					}
				else if (aRect.iTl.iY > aClipRect.iBr.iY)
					{
					break;
					}
				else
					{
					aRect.Intersection(aClipRect);
					if (! aRect.IsEmpty())
						{
						return ETrue;
						}
					}
				}

			found = aInterpreter.NextRect(aRect);
			}
		}

	aRect = TRect(TRect::EUninitialized);
	return EFalse;
	}

TBool RTmDrawingInterpreter::GetNextAdjustedHighlightClipRect(RTmBoundingRectInterpreter& aInterpreter,
															  const TRect& aClipRect, TRect& aRect) const
	{
	if (NULL == iHighlightExtensions)
	    {
	    OstTrace0( TRACE_DUMP, RTMDRAWINGINTERPRETER_GETNEXTADJUSTEDHIGHLIGHTCLIPRECT, "EInvariant" );
	    }
	__ASSERT_DEBUG(NULL != iHighlightExtensions, TmPanic(EInvariant));
	if (iHighlightEndPos <= iHighlightStartPos)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_RTMDRAWINGINTERPRETER_GETNEXTADJUSTEDHIGHLIGHTCLIPRECT, "EInvariant" );
	    }
	__ASSERT_DEBUG(iHighlightEndPos > iHighlightStartPos, TmPanic(EInvariant));

	TBool found = aInterpreter.NextRect(aRect);
	while (found)
		{
		if (! aRect.IsEmpty())
			{
			aRect.Move(TextLayoutTopLeft());
			iHighlightExtensions->AdjustRect(aRect);

			if (aRect.iBr.iY < aClipRect.iTl.iY)
				{
				// ignore it
				}
			else if (aRect.iTl.iY > aClipRect.iBr.iY)
				{
				break;
				}
			else
				{
				aRect.Intersection(aClipRect);
				if (! aRect.IsEmpty())
					{
					return ETrue;
					}
				}
			}

		found = aInterpreter.NextRect(aRect);
		}

	aRect = TRect(TRect::EUninitialized);
	return EFalse;
	}

TBool RTmDrawingInterpreter::IsCurrentOpCompletelyHighlighted() const
	{
	if (iHighlightEndPos > iHighlightStartPos)
		{
		return iHighlightStartPos <= StartDocPos().iPos &&
			iHighlightEndPos >= EndDocPos().iPos;
		}
	else
		{
		return EFalse;
		}
	}

TBool RTmDrawingInterpreter::UsesExtendedHighlight() const
	{
	return NULL != iHighlightExtensions && iHighlightExtensions->Extends();
	}

TBool RTmDrawingInterpreter::UsesAdjustedHighlight() const
	{
	return NULL != iHighlightExtensions && ! iHighlightExtensions->IsNull();
	}

void RTmDrawingInterpreter::Draw()
	{
	if (iFlags & EInvisible)
		return;

	iGc.Reset();
	InitGraphicsContext();
 	TRect drawn = iClipRect;
 	drawn.iTl.iY = drawn.iBr.iY = TextLayoutTopLeft().iY;
	int error;

	TBool done = FALSE;
	while (Next() && !done)
		{
		switch (Op())
			{
			case EOpLine:
				if (LineInfo().iOuterRect.iBr.iY < iClipRect.iTl.iY)
					{
					Skip();
					break;
					}
				if (LineInfo().iOuterRect.iTl.iY > iClipRect.iBr.iY)
					done = TRUE;
				else
					{
					TRect line_rect;
					Line(line_rect);

					// Draw background to left and right of line where it intersects the clip rectangle.
					if (!(iFlags & EIsLabel))
						{
						TRect r = iClipRect;
						r.iTl.iY = Max(r.iTl.iY,line_rect.iTl.iY);
						r.iBr.iY = Min(r.iBr.iY,line_rect.iBr.iY);
						r.iBr.iX = line_rect.iTl.iX;
						if (iDocBackground && !r.IsEmpty())
							{
							DrawBackground(r,*iDocBackground); // rectangle to left of line
							InitGraphicsContext();
							}
						r.iBr.iX = iClipRect.iBr.iX;
						r.iTl.iX = line_rect.iBr.iX;
						if (iDocBackground && !r.IsEmpty())
							{
							DrawBackground(r,*iDocBackground); // rectangle to right of line
							InitGraphicsContext();
							}
						if (drawn.IsEmpty())
							drawn.iTl.iY = line_rect.iTl.iY;
						drawn.iBr.iY = line_rect.iBr.iY;
						}
					}
				break;
			case EOpText:
				Text();
				break;
			case EOpInlineText:
				InlineText();
				break;
			case EOpSpecialChar:
				TRAP(error,SpecialCharL());
				break;
			case EOpRule:
				Rule();
				break;
			case EOpLabel:
				Label();
				break;
			default:
				Skip();	// unimplemented
				break;
			}
		}

	iParFormat.Close();

	/*
	Draw undrawn background areas outside the text. These may themselves be partially covered by
	a custom background, so each one has to be drawn by DrawBackground, which can then
	fill in any parts not custom-drawn.
	*/
	if (iDocBackground && !(iFlags & EIsLabel))
		{
		if (iLabelLeft < drawn.iTl.iX)
			drawn.iTl.iX = iLabelLeft;
		drawn.Intersection(iClipRect);
		if (drawn != iClipRect)
			{
			TRect r[4];
			SubtractRect(iClipRect,drawn,r);
			for (int i = 0; i < 4; i++)
				if (!r[i].IsEmpty())
					DrawBackground(r[i],*iDocBackground);
			}
		}

	// if the highlight is not visible (because the custom drawer defines a
	// highlighted background color identical to the highlighted foreground color)
	//  draw the selection inverting every pixel in the highlighted region.
	if (! iHighlightedLineRect.IsEmpty())
		{
		InvertHighlightedLineRgn();
		}
	}

void RTmDrawingInterpreter::InitGraphicsContext()
	{
	iGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
	iGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc.SetPenStyle(CGraphicsContext::ESolidPen);
	iGc.SetClippingRect(iClipRect);
	}

void RTmDrawingInterpreter::Line(TRect& aDrawn)
	{
	if (! iHighlightedLineRect.IsEmpty())
		{
		InvertHighlightedLineRgn();
		}

	if (iFirstLine || ParStart())
		{
		int error;
		TRAP(error,Source().GetParagraphFormatL(StartChar(),iParFormat));
		if (LineInfo().iFlags & TTmLineInfo::EParRightToLeft)
			iParFormat.iFlags |= RTmParFormat::ERightToLeft;
		else
			iParFormat.iFlags &= ~RTmParFormat::ERightToLeft;
		iLabelled = FALSE;
		MFormLabelApi* labelApi = (MFormLabelApi*)Source().GetExtendedInterface(KFormLabelApiExtensionUid);
		if ((labelApi != NULL) && (labelApi->LabelModeSelect(MTmSource::EParLabel, StartChar())))
			{
			RTmParFormat pf;
			TRAP(error,Source().GetParagraphFormatL(0,pf));
			if (!error)
				{
				iLabelled = TRUE;
				iLabelBackground = pf.iBackgroundColor;
				}
			labelApi->LabelModeCancel();
			pf.Close();
			}
		TSize label_size(0, 0);
		int margin_size = 0;
		if (labelApi != NULL)
			labelApi->LabelMetrics(MTmSource::EParLabel, label_size, margin_size);
		iLabelLeft = LineInfo().iOuterRect.iTl.iX - margin_size;
		iLabelRight = iLabelLeft + label_size.iWidth;
		}

	TRect inner_border_bounds;
	TRect outer_border_bounds(LineInfo().iOuterRect);
	TInt left_border_width = 0, right_border_width = 0, top_border_width = 0, bottom_border_width = 0;
	GetBorderInfo(inner_border_bounds,outer_border_bounds,
				  left_border_width,right_border_width,top_border_width,bottom_border_width);
	TRect r(outer_border_bounds);
	r.BoundingRect(LineInfo().iOuterRect);
	r.Intersection(iClipRect);

	if (!(iFlags & EIsLabel))
		{
		// Draw paragraph background if specified.
		if (iFlags & EDrawParBackground)
			{
			DrawBackground(r,iParFormat.iBackgroundColor);
			InitGraphicsContext();
			}
		// Otherwise use the document background if any as a paragraph background.
		else if (iDocBackground)
			{
			DrawBackground(r,*iDocBackground);
			InitGraphicsContext();
			}

		DrawLineGraphics(r);
		
		if (iLabelLeft < LineInfo().iOuterRect.iTl.iX)
			DrawLabelBackground();

		if (Source().PageBreakInRange(StartChar(),Min(EndChar(),Source().DocumentLength())))
			DrawPageBreak();
		}

	if (ParStart() && iParFormat.Bullet())
		DrawBullet();

	if (Bordered())
		DrawBorders(inner_border_bounds,outer_border_bounds,
					left_border_width,right_border_width,top_border_width,bottom_border_width);
   
   	iFirstLine = FALSE;
	aDrawn = r;
	}

void RTmDrawingInterpreter::DrawLineGraphics(const TRect& aRect)
	{
	Source().DrawLineGraphics(iGc, TextLayoutTopLeft(), aRect, LineInfo());
	InitGraphicsContext();
	}

void RTmDrawingInterpreter::DrawLabelBackground()
	{
	TRect r = LineInfo().iOuterRect;
	r.iTl.iX = iLabelLeft;
	r.iBr.iX = iLabelRight;

	// Draw the label background.
	if (iFlags & EDrawParBackground)
		DrawBackground(r,iLabelBackground);
	else if (iDocBackground)
		DrawBackground(r,*iDocBackground);		

	// Draw the gutter.
	r.iTl.iX = iLabelRight;
	r.iBr.iX = LineInfo().iOuterRect.iTl.iX;
	if (iDocBackground && !r.IsEmpty())
		DrawBackground(r,*iDocBackground);		

	InitGraphicsContext();
	}

void RTmDrawingInterpreter::DrawPageBreak()
	{
	iGc.SetPenStyle(CGraphicsContext::EDashedPen);
	TLogicalRgb color = iParFormat.iBackgroundColor;
	TUint index = color.SystemColorIndex();
	if (index)
		color = Source().SystemColor(index,color);
	iGc.SetPenColor(~(color & 0xFFFFFF));
	TPoint p = LineInfo().iOuterRect.iBr;
	p.iY--;
	TPoint q = p;
	q.iX = LineInfo().iOuterRect.iTl.iX;
	TmTextDrawExt().DrawLine(iGc, p, q);
	InitGraphicsContext();
	}

void RTmDrawingInterpreter::DrawBullet()
	{
	int left_margin = 0;
	int right_margin = 0;
	int first_line_left_margin = 0;
	int first_line_right_margin = 0;
	int bullet_margin = 0;
	int bullet_width = 0;
	int bullet_ascent = 0;
	int bullet_descent = 0;
	CFont* font = NULL;
	TRAPD(error,TmGetMarginsL(Device(),iParFormat,left_margin,right_margin,
							  first_line_left_margin,first_line_right_margin,
							  bullet_margin,bullet_width,bullet_ascent,bullet_descent,&font));
	if (!error && font)
		{
		TBuf<1> bullet;
		bullet.SetLength(1);
		bullet[0] = (TText)iParFormat.Bullet()->iCharacterCode;
		TPoint p;
		if (iParFormat.RightToLeft())
			{
			p = EndPen();
			p.iX -= bullet_margin - first_line_right_margin + bullet_width;
			}
		else
			{
			p = StartPen();
			p.iX += bullet_margin - first_line_left_margin;
			}
		iGc.UseFont(font);
		TRgb color = Source().SystemColor(iParFormat.Bullet()->iColor);
		SetPenColor(color);
		iGc.SetUnderlineStyle(EUnderlineOff);
		TmTextDrawExt().DrawText(iGc, bullet, p);
		iGc.DiscardFont();
		Device().ReleaseFont(font);
		}
	}

void RTmDrawingInterpreter::GetBorderInfo(TRect& aInnerBounds,TRect& aOuterBounds,
										  TInt& aLeftWidth,TInt& aRightWidth,TInt& aTopWidth,TInt& aBottomWidth)
	{
	const MGraphicsDeviceMap& device = Device();
	int left = 0, right = 0, top = 0, bottom = 0;
	int left_full = 0, right_full = 0, top_full = 0, bottom_full = 0;
	TmGetBorderWidth(device,iParFormat,RTmParFormat::ELeadingBorder,left,left_full);
	TmGetBorderWidth(device,iParFormat,RTmParFormat::ETrailingBorder,right,right_full);
	if (ParStart())
		TmGetBorderWidth(device,iParFormat,RTmParFormat::ETopBorder,top,top_full);
	if (ParEnd())
		TmGetBorderWidth(device,iParFormat,RTmParFormat::EBottomBorder,bottom,bottom_full);
	int h_margin = device.HorizontalTwipsToPixels(iParFormat.iBorderMargin);
	int v_margin = device.VerticalTwipsToPixels(iParFormat.iBorderMargin);

	// Establish the outer bounds of the inner border rule.
	TRect inner_bounds = LineInfo().iInnerRect;
	int left_margin = device.HorizontalTwipsToPixels(iParFormat.iLeadingMargin);
	int first_line_left_margin = left_margin + device.HorizontalTwipsToPixels(iParFormat.iFirstLineIndent);
	inner_bounds.iTl.iX = LineInfo().iOuterRect.iTl.iX + Min(left_margin,first_line_left_margin);
	inner_bounds.iBr.iX = LineInfo().iOuterRect.iBr.iX - device.HorizontalTwipsToPixels(iParFormat.iTrailingMargin);
	
	// Include paragraph labels in the border rule area.
	if (iLabelled)
		inner_bounds.iTl.iX = Min(inner_bounds.iTl.iX,iLabelLeft);
	
	if (left)
		inner_bounds.iTl.iX -= h_margin + left;
	if (right)
		inner_bounds.iBr.iX += h_margin + right;
	if (top)
		inner_bounds.iTl.iY -= v_margin + top;
	if (bottom)
		inner_bounds.iBr.iY += v_margin + bottom;

	// Establish the outer bounds of the outer border rule.
	TRect outer_bounds = inner_bounds;
	outer_bounds.iTl.iX -= left_full - left;
	outer_bounds.iBr.iX += right_full - right;
	outer_bounds.iTl.iY -= top_full - top;
	outer_bounds.iBr.iY += bottom_full - bottom;

	aInnerBounds = inner_bounds;
	aOuterBounds = outer_bounds;
	aLeftWidth = left;
	aRightWidth = right;
	aTopWidth = top;
	aBottomWidth = bottom;
	}

void RTmDrawingInterpreter::DrawBorders(const TRect& aInnerBounds,const TRect& aOuterBounds,
										TInt aLeftWidth,TInt aRightWidth,TInt aTopWidth,TInt aBottomWidth)
	{
	iGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	TRect r;
	const TTmParBorder* b = iParFormat.Border(RTmParFormat::ELeadingBorder);
	if (b && (aLeftWidth != 0))
		{
		r = aInnerBounds;
		r.iBr.iX = r.iTl.iX + aLeftWidth;
		DrawBorderRect(*b,r);
		r = aOuterBounds;
		r.iBr.iX = r.iTl.iX + aLeftWidth;
		DrawBorderRect(*b,r);
		}
	b = iParFormat.Border(RTmParFormat::ETrailingBorder);
	if (b && (aRightWidth != 0))
		{
		r = aInnerBounds;
		r.iTl.iX = r.iBr.iX - aRightWidth;
		DrawBorderRect(*b,r);
		r = aOuterBounds;
		r.iTl.iX = r.iBr.iX - aRightWidth;
		DrawBorderRect(*b,r);
		}
	b = iParFormat.Border(RTmParFormat::ETopBorder);
	if (b && (aTopWidth != 0))
		{
		r = aInnerBounds;
		r.iBr.iY = r.iTl.iY + aTopWidth;
		DrawBorderRect(*b,r);
		r = aOuterBounds;
		r.iBr.iY = r.iTl.iY + aTopWidth;
		DrawBorderRect(*b,r);
		}
	b = iParFormat.Border(RTmParFormat::EBottomBorder);
	if (b && (aBottomWidth != 0))
		{
		r = aInnerBounds;
		r.iTl.iY = r.iBr.iY - aBottomWidth;
		DrawBorderRect(*b,r);
		r = aOuterBounds;
		r.iTl.iY = r.iBr.iY - aBottomWidth;
		DrawBorderRect(*b,r);
		}
	}

void RTmDrawingInterpreter::DrawBorderRect(const TTmParBorder& aBorder,const TRect& aRect)
	{
	if (aBorder.iStyle == TTmParBorder::ESolidStyle || aBorder.iStyle == TTmParBorder::EDoubleStyle)
		{
		iGc.SetPenStyle(CGraphicsContext::ENullPen);
		SetBrushColor(aBorder.iColor);
		TmTextDrawExt().DrawRect(iGc, aRect);
		return;
		}
	switch (aBorder.iStyle)
		{
		case TTmParBorder::EDotStyle:
			iGc.SetPenStyle(CGraphicsContext::EDottedPen);
			break;
		case TTmParBorder::EDashStyle:
			iGc.SetPenStyle(CGraphicsContext::EDashedPen);
			break;
		case TTmParBorder::EDotDashStyle:
			iGc.SetPenStyle(CGraphicsContext::EDotDashPen);
			break;
		case TTmParBorder::EDotDotDashStyle:
			iGc.SetPenStyle(CGraphicsContext::EDotDotDashPen);
			break;
		default:
			return;
		}
	SetPenColor(aBorder.iColor);
	TPoint p = aRect.iTl;
	TPoint q = aRect.iBr;
	if (aRect.Width() > aRect.Height())
		{
		q.iY = p.iY;
		TmTextDrawExt().DrawLine(iGc, p, q);
		}
	else
		{
		q.iX = p.iX;
		// Set the top to the paragraph top and clip to the rectangle so that the dot or dash phase is consistent.
		p.iY = LineInfo().iParTop;
		TRect clip = aRect;
		clip.Intersection(iClipRect);
		iGc.SetClippingRect(clip);
		TmTextDrawExt().DrawLine(iGc, p, q);
		iGc.SetClippingRect(iClipRect);
		}
	iGc.SetPenStyle(CGraphicsContext::ESolidPen);
	}

/*
Draw the background in aRect, using the custom background where possible, and filling in with
the default background.
*/
void RTmDrawingInterpreter::DrawBackground(const TRect& aRect,const TLogicalRgb& aBackground)
	{
	if (aRect.IsEmpty())
		{
		return;
		}

	// 1 - first draw the background

	TRect drawn;
	Source().DrawBackground(iGc,TextLayoutTopLeft(),aRect,aBackground,drawn);


	// 2 - then draw the highlighted part of the background

	if (iDrawInvertedHighlight)
		{
		// if we are drawing an inverted selection, then we calculate the union of
		// all the highlighted rects. Since we are clipping our drawing with the current Line()
		// rect, we can assume that all the rects have the same height and their union is a
		// single rectangle.
		RTmBoundingRectInterpreter interpreter(iSource, iParam);
		TRect rect;
		TBool found = GetFirstHighlightClipRect(interpreter, aRect, rect);
		while (found)
			{
			rect.Intersection(LineInfo().iOuterRect);
			if (! rect.IsEmpty())
				{
				// iHighlightedLineRgn identifies the highlighted area in the current line.
				// note that with Right-to-left text the selection on a single line can be
				// composed by several disjoined rectangles.
				iHighlightedLineRgn.AddRect(rect); // this may fail and set the error flag
				iHighlightedLineRgn.Tidy(); // this allows us to compact the region in the smallest
											// number of rectangles, making it very unlikely to run out of memory.

				// iHighlightedLineRect keeps the bounding rect of the highlighted area in the current line.
				// if highlight extensions are not used, this rect corresponds to the highlighted rect.
				if (iHighlightedLineRect.IsEmpty())
					{
					iHighlightedLineRect = rect;
					}
				else
					{
					iHighlightedLineRect.BoundingRect(rect);
					}
				}
			found = GetNextHighlightClipRect(interpreter, aRect, rect);
			}
		interpreter.Close();
		}
	else
		{
		// the selection should not be inverted but only drawn with different colors, using TTmHighlightSource.

		RRegion highlightRgn;

		// we are using a region that can grow to contain an arbitrary number of rectangles.
		// so we must handle the case in which a rectangle allocation fails in a region operation.
		TBool outOfMemory = ! GetHighlightClipRegion(aRect, highlightRgn);
		if (outOfMemory)
			{
			// we ran out of memory allocating rectangles in the region.
			// we can still draw the highlighted background area enumerating the rectangles that
			// would compose the region and setting them as clipping rects in the graphics context.
			RTmBoundingRectInterpreter interpreter(iSource, iParam);
			TRect rect;
			TBool found = GetFirstHighlightClipRect(interpreter, aRect, rect);
			while (found)
				{
				iGc.SetClippingRect(rect);
				TTmHighlightSource highlightSource(Source());
				highlightSource.DrawBackground(iGc, TextLayoutTopLeft(), rect, aBackground, drawn);
				iGc.SetClippingRect(iClipRect);
				found = GetNextHighlightClipRect(interpreter, aRect, rect);
				}
			interpreter.Close();
			}
		else if (! highlightRgn.IsEmpty())
			{
			// here highlightRgn contains the highlighted region. We can use it to clip the graphics context
			// and draw only the highlighted background area. This is faster than drawing it rect by rect.

			highlightRgn.ClipRect(iClipRect); // ClipRect() is a safe operation, it never sets the region error flag

			if (! highlightRgn.IsEmpty())
				{
				// if the selection should not be inverted but only drawn with different colors,
				// then we clip the graphics context region and use a TTmHighlightSource to draw it.
				TBool useClippingRgn = highlightRgn.Count() > 1;
				if (useClippingRgn)
					{
					iGc.SetClippingRegion(highlightRgn);
					}
				else
					{
					iGc.SetClippingRect(highlightRgn[0]);
					}

				TRect clipRect = highlightRgn.BoundingRect();
				clipRect.Intersection(aRect);

				TTmHighlightSource highlightSource(Source());
				highlightSource.DrawBackground(iGc, TextLayoutTopLeft(), clipRect, aBackground, drawn);

				if (useClippingRgn)
					{
					iGc.CancelClippingRegion();
					}
				else
					{
					iGc.SetClippingRect(iClipRect);
					}
				}
			}

		highlightRgn.Close();
		}
	}

void RTmDrawingInterpreter::Text()
	{
	// Copy all the text into a single buffer.
	int buffer_length = EndChar() - StartChar();
	// If the buffer is overlong, we will truncate. This happens when a huge
	// number of spaces terminates a line.
	// This causes two bad effects:
	// 1) the cursor cannot be seen after the truncation position.
	// 2) finding the end of the line just finds the truncation position, not
	// the actual end of the line.
	// TPB 2/7/2001 - fix for DAS-4XVEJ7
	if (KMaxTextChunkSize < buffer_length)
		buffer_length = KMaxTextChunkSize;
	TText buffer[KMaxTextChunkSize + 2];
	TTmCharFormat format;
	CTmTextFontCache* font = NULL;
	TInt err = TextCache().GetDisplayedText(StartChar(), StartChar() + buffer_length,
		RightToLeft()?
			RTmTextCache::EVisualRightToLeft : RTmTextCache::ELeftToRight,
		buffer,ContextCharChar(), &format, &font);
	
	if (err == KErrNone)
		{
		// Draw the text
		TPtrC text(buffer, buffer_length + 2);
		if (*text.Ptr() == 0xFFFF)
			DrawText(text,0, text.Length() - 1, format,&font->Font());
		else
			DrawText(text,1, text.Length() - 1, format,&font->Font());
		font->Close();
		}
	}

void RTmDrawingInterpreter::InlineText()
	{
	TPtrC text;
	TTmCharFormat format;
	CTmTextFontCache* font = NULL;
	GetInlineText(text,&format,&font);
	if (font != NULL)
		{
		DrawText(text,0, text.Length() - 1, format,&font->Font());
		font->Close();
		}
	}

void RTmDrawingInterpreter::DrawText(const TDesC& aText,const TInt aStart, const TInt aEnd, const TTmCharFormat& aFormat,CFont* aFont)
	{
	if (!aFont)
		return;

	// Set the graphics context up for drawing the text.
	iGc.UseFont(aFont);
	iGc.SetPenStyle(CGraphicsContext::ESolidPen);
	SetPenColor(aFormat.iTextColor);
	if (aFormat.iEffects & TTmCharFormat::EUnderline)
		iGc.SetUnderlineStyle(EUnderlineOn);
	else
		iGc.SetUnderlineStyle(EUnderlineOff);
	if (aFormat.iEffects & TTmCharFormat::EStrikethrough)
		iGc.SetStrikethroughStyle(EStrikethroughOn);
	else
		iGc.SetStrikethroughStyle(EStrikethroughOff);

	// Calculate the text bounds.
	TRect text_bounds = LineInfo().iInnerRect;
	text_bounds.iTl.iX = StartPen().iX;
	text_bounds.iBr.iX = EndPen().iX;

	// Set character justification to perform copy fitting.
	int extra_pixels = 0;
	if (CopyFit() || (aFormat.iEffects & (TTmCharFormat::EUnderline | TTmCharFormat::EStrikethrough)))
		{
		CFont::TMeasureTextInput input;
		input.iFlags = CFont::TMeasureTextInput::EFVisualOrder;
		input.iStartInputChar = 1;
		extra_pixels = text_bounds.Width() - aFont->MeasureText(aText, &input);
		}

	// Draw the text.

	if (iDrawInvertedHighlight || ! IsCurrentOpCompletelyHighlighted() ||
		(NULL != iHighlightExtensions && iHighlightExtensions->Shrinks()))
		{
		// If a custom interface to draw the text in context exists, then use it to call the appropriate DrawText method.
		MTmCustomExtension* extensionInterface = reinterpret_cast <MTmCustomExtension*> (Source().GetExtendedInterface(KTmCustomExtensionUid));
		if (extensionInterface)
			{
			extensionInterface->DrawText(iGc,TextLayoutTopLeft(),text_bounds,LineInfo(),aFormat,aText, aStart, aEnd, StartPen(),extra_pixels);
			}
			else
				{
				Source().DrawText(iGc,TextLayoutTopLeft(),text_bounds,LineInfo(),aFormat,aText.Mid(aStart,aEnd - aStart + 1),StartPen(),extra_pixels);
				}
		}

	// Draw the text again if some part of it is highlighted.

	// If we are inverting the highlighted area at the end of Line(), then we should not draw highlighted text.
	if (! iDrawInvertedHighlight)
		{
		RRegion highlightRgn;

		if (! GetHighlightClipRegion(text_bounds, highlightRgn))
			{
			// If the error flag is set, there was not enough memory to allocate all the rectangles.
			// We must use a slower method: enumerate all the rectangles that compose the highlighted (and
			// maybe extended) area of the textview and set each one of these rectangles as clipping rect
			// of the graphics context. Then we draw the text.
			RTmBoundingRectInterpreter interpreter(iSource, iParam);
			TRect rect;
			TBool found = GetFirstHighlightClipRect(interpreter, iClipRect, rect);
			while (found)
				{
				iGc.SetClippingRect(rect);
				TTmHighlightSource highlightSource(Source());
				highlightSource.DrawText(iGc,TextLayoutTopLeft(),text_bounds,LineInfo(),aFormat,aText,aStart,aEnd,StartPen(),extra_pixels);
				iGc.SetClippingRect(iClipRect);
				found = GetNextHighlightClipRect(interpreter, iClipRect, rect);
				}
			interpreter.Close();
			}
		else
			{
			// If the error flag is not set, then there was enough memory to allocate the rectangles in the region.
			// We can set a single clipping region in the GC, that should be faster
			if (! highlightRgn.IsEmpty())
				{
				highlightRgn.ClipRect(iClipRect); // ClipRect() is safe (never sets the region error flag).

				if (! highlightRgn.IsEmpty())
					{
					TBool useClippingRgn = highlightRgn.Count() > 1;
					if (useClippingRgn)
						{
						iGc.SetClippingRegion(highlightRgn);
						}
					else
						{
						iGc.SetClippingRect(highlightRgn[0]); // much faster
						}

					TTmHighlightSource highlightSource(Source());
					highlightSource.DrawText(iGc,TextLayoutTopLeft(),text_bounds,LineInfo(),aFormat,aText,aStart,aEnd,StartPen(),extra_pixels);

					if (useClippingRgn)
						{
						iGc.CancelClippingRegion();
						}
					else
						{
						iGc.SetClippingRect(iClipRect);
						}
					}
				}
			}

		highlightRgn.Close();
		}

	iGc.DiscardFont();
	}

void RTmDrawingInterpreter::SpecialCharL()
	{
	TPtrC text;
	GetText(StartChar(),EndChar(),text);
	if (text[0] == CEditableText::EPictureCharacter)
		{
		ReleaseFont();
		CPicture* p = Source().PictureL(StartChar());
		if (p)
			{
			DrawPicture(p);
			}
		}
	}

void RTmDrawingInterpreter::DrawPicture(CPicture* aPicture)
	{
	TSize size;
	aPicture->GetSizeInPixels(&Device(),size);

	TPoint top_left = StartPen();
	top_left.iY -= size.iHeight - Subscript();
	TRect clip(top_left,size);
	clip.Intersection(iClipRect);

	TInt highlightStartPos = 0;
	TInt highlightEndPos = 0;
	if (! iDrawInvertedHighlight &&
		GetHighlightPos(highlightStartPos, highlightEndPos))
		{
		// if the highlighted interval [iHighlightStartPos, iHighlightEndPos] overlaps
		// the current object [StartDocPos(), EndDocPos()], then the whole picture is selected.
		TTmHighlightSource highlightSource(Source());
		highlightSource.DrawPicture(iGc, top_left, clip, Device(), *aPicture);
		}
	else
		{
		Source().DrawPicture(iGc, top_left, clip, Device(), *aPicture);
		}

	iGc.Reset();
	InitGraphicsContext();
	}

void RTmDrawingInterpreter::Rule()
	{
	SetBrushColor(RuleColour());
	TmTextDrawExt().DrawRect(iGc, Bounds());
	}

void RTmDrawingInterpreter::Label()
	{
	ReleaseFont();

	/*
	The label rectangle is a hybrid; the source interface provides x coords but the y coords come from the bytecode.
	This is so that the gutter between label and text can be adjusted at the time of drawing.
	*/
	MFormLabelApi* labelApi = (MFormLabelApi*)Source().GetExtendedInterface(KFormLabelApiExtensionUid);
	if ((labelApi != NULL) && (labelApi->LabelModeSelect(LabelType(), StartChar())))
		{
		TRect label_rect = Bounds();
		label_rect.Move(LineInfo().iOuterRect.iTl);
		label_rect.iTl.iX = iLabelLeft;
		label_rect.iBr.iX = iLabelRight;

		TTmInterpreterParam param(ByteCode());
		param.iCodeStart = CodePos();
		param.iCodeEnd = EndCodePos();
		param.iTextStart = 0;
		param.iWidth = Bounds().Width();
		TRect label_clip_rect = label_rect;
		label_clip_rect.Intersection(iClipRect);
		RTmDrawingInterpreter interpreter(Source(), iGc,param, label_rect.iTl,
			label_clip_rect, iDocBackground, iFlags | EIsLabel, NULL, NULL);
		interpreter.Draw();
		interpreter.Close();
		iGc.Reset();
		InitGraphicsContext();
		labelApi->LabelModeCancel();
		}
	Skip();
	}


void RTmDrawingInterpreter::InvertHighlightedLineRgn()
	{
	if (! iHighlightedLineRect.IsEmpty())
		{
		TBool useClippingRgn = (! iHighlightedLineRgn.IsEmpty() && ! iHighlightedLineRgn.CheckError());
		if (useClippingRgn)
			{
			if (1 == iHighlightedLineRgn.Count())
				{
				iHighlightedLineRect = iHighlightedLineRgn[0];
				useClippingRgn = EFalse;
				}
			}

		if (useClippingRgn)
			{
			iGc.SetClippingRegion(iHighlightedLineRgn);
			}
		else
			{
			iGc.SetClippingRect(iHighlightedLineRect);
			}

		iGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc.SetBrushColor(iHighlightXorColor);
		iGc.SetPenStyle(CGraphicsContext::ENullPen);
		iGc.SetDrawMode(CGraphicsContext::EDrawModeXOR);

		TTmTextDrawExt tmTextDrawExtDefault; //The default implementation of MTmTextDrawExt
		const MTmTextDrawExt* tmTextDrawExt = reinterpret_cast <const MTmTextDrawExt*>(
				(MTmSource*)(iSource.GetExtendedInterface(KTmTextDrawExtId)));
		if (NULL == tmTextDrawExt)			{

			tmTextDrawExt = &tmTextDrawExtDefault;
			}
		tmTextDrawExt->DrawRect(iGc, iHighlightedLineRect);

		iGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
		iGc.SetPenStyle(CGraphicsContext::ESolidPen);

		if (useClippingRgn)
			{
			iGc.CancelClippingRegion();
			}
		else
			{
			iGc.SetClippingRect(iClipRect);
			}

		iHighlightedLineRect = TRect(TRect::EUninitialized);
		iHighlightedLineRgn.Clear();
		}
	}


RTmBoundingRectInterpreter::RTmBoundingRectInterpreter(MTmSource& aSource,const TTmInterpreterParam& aParam):
	RTmGeneralInterpreter(aSource,aParam),
	iStartDocPos(-1,FALSE),
	iEndDocPos(-1,FALSE),
	iRangeState(EBeforeAllLines)
	{
	}

TBool RTmBoundingRectInterpreter::FirstRect(TInt aStartDocPos,TInt aEndDocPos,TRect& aRect)
	{
	iStartDocPos = TTmDocPos(aStartDocPos,TRUE);
	iEndDocPos = TTmDocPos(aEndDocPos,FALSE);
	TInt dummy = StartDocPos().iPos;
	if (aStartDocPos < StartDocPos().iPos && aEndDocPos > StartDocPos().iPos)
		{//Highlight starts before formatted band but ends inside it
		 //Do nothing (do not attempt to run DocPosToLine)
		}
	else if (!DocPosToLine(TTmDocPosSpec(iStartDocPos)))
		{
		return FALSE;
		}
	if (iStartDocPos == iEndDocPos || PosIsInLine(iEndDocPos))
		{
		iRangeState = EInLastLine;
		}
	else
		iRangeState = EInFirstLine;
	return NextRect(aRect);
	}

TBool RTmBoundingRectInterpreter::NextRect(TRect& aRect)
	{
	if (iRangeState == EAfterAllLines || !Next())
		{
		aRect.SetRect(0,0,0,0);
		return FALSE;
		}
	iRect.SetRect(0,0,0,0);
	switch (Op())
		{
		case EOpLine:
			Line();
			break;
		case EOpText:
			Text();
			break;
		case EOpSpecialChar:
			SpecialCharacter();
			break;
		default:
			Skip();
			break;
		}
	aRect = iRect;
	return TRUE;
	}

void RTmBoundingRectInterpreter::Line()
	{
	if (iRangeState == EInFirstLine)
		{
		if (PosIsInLine(iEndDocPos))
			iRangeState = EInLastLine;
		else
			iRangeState = EInMedialLine;
		}
	else if (iRangeState == EInMedialLine)
		{
		if (PosIsInLine(iEndDocPos))
			iRangeState = EInLastLine;
		}
	else if (iRangeState == EInLastLine)
		iRangeState = EAfterAllLines;
	if (iRangeState == EInMedialLine)
		{
		iRect = LineInfo().iOuterRect;
		iRect.iTl.iX = LineInfo().iInnerRect.iTl.iX;
		iRect.iBr.iX = LineInfo().iInnerRect.iBr.iX;
		Skip();
		}
	}

void RTmBoundingRectInterpreter::Text()
	{
	if (DontMeasure())
		return;

	TTmDocPos startPos = StartDocPos();
	TTmDocPos endPos = EndDocPos();

	// Return with an empty rectangle if the range doesn't overlap this section
	// of text.
	if (iEndDocPos <= startPos || endPos <= iStartDocPos)
		return;

	iRect = LineInfo().iOuterRect;
	iRect.iTl.iX = StartPen().iX;
	iRect.iBr.iX = EndPen().iX;

	// Work out if we need to find the start and end within this text chunk
	TBool startRequired = startPos.iPos < iStartDocPos.iPos? ETrue : EFalse;
	TBool endRequired = iEndDocPos.iPos < endPos.iPos? ETrue : EFalse;

	// Return with a full rectangle if the range totally covers this section of
	// text.
	if (!startRequired && !endRequired)
		return;

	// Sort the end points left to right.
	TBool leftRequired = startRequired;
	TBool rightRequired = endRequired;
	TTmDocPos leftPos = iStartDocPos;
	TTmDocPos rightPos = iEndDocPos;
	if (RightToLeft())
		{
		leftRequired = endRequired;
		rightRequired = startRequired;
		leftPos = iEndDocPos;
		rightPos = iStartDocPos;
		}

	// Truncate the rectangle as required.
	RTmGraphemeInTextChunkIteratorNice edgeIterator(*this);
	if (leftRequired)
		{
		edgeIterator.FindEdge(leftPos);
		iRect.iTl.iX = edgeIterator.Get()->iPos.iEdge.iX;
		}
	if (rightRequired)
		{
		edgeIterator.FindEdge(rightPos);
		iRect.iBr.iX = edgeIterator.Get()->iPos.iEdge.iX;
		}
	edgeIterator.Close();
	}

void RTmBoundingRectInterpreter::SpecialCharacter()
	{
	// Return with an empty rectangle if the range doesn't overlap this section
	// of text.
	if (iEndDocPos.iPos <= StartChar() || EndChar() <= iStartDocPos.iPos)
		return;

	// Otherwise return the full rectangle: special characters are indivisible.
	iRect = LineInfo().iOuterRect;
	iRect.iTl.iX = StartPen().iX;
	iRect.iBr.iX = EndPen().iX;
	}

TTmByteCodeFinder::TTmByteCodeFinder(const TTmInterpreterParam& aParam,TInt aStartDocPos,TInt aEndDocPos):
	TTmInterpreter(aParam),
	iStartDocPos(aStartDocPos),
	iEndDocPos(aEndDocPos)
	{
	}

/*
Find the bytecode for the range iStartDocPos...iEndDocPos.

If aToParStart is ETrue the bytecode is extended to the start of the paragraph, 
otherwise to one line before the one containing iStartDocPos.

If aToParEnd is ETrue the bytecode is extended to the end of the paragraph, 
otherwise to one line after the one containing iEndDocPos. However, aToParEnd 
is constrained by aMaxExtraLines, which is the maximum number of lines
allowed after the line containing iEndDocPos.

The bidirectional contexts at the start and end are placed in iStartBdState and 
iEndBdState if both pointers are non-null.
*/
TBool TTmByteCodeFinder::FindByteCode(TBool aToParStart,TBool aToParEnd,TInt aMaxExtraLines,TInfo& aInfo,
									  TBidirectionalContext* aStartBdState,TBidirectionalContext* aEndBdState)
	{
	if (!((aStartBdState && aEndBdState)
	        || (!aStartBdState && !aEndBdState)))
	    {
	    OstTrace0( TRACE_DUMP, TTMBYTECODEFINDER_FINDBYTECODE, "EBadArg" );
	    }
	__ASSERT_DEBUG((aStartBdState && aEndBdState)
		|| (!aStartBdState && !aEndBdState), TmPanic(EBadArg));
	if (aStartBdState)
		{
		aStartBdState->Reset();
		aEndBdState->Reset();
		}

	// Use aEndBdState to hold the bidirectional state temporarily, so that the following line doesn't overwrite it.
	SetBdStatePtr(aEndBdState);
	// We are looking for the context  of the previous line in the byte code, to pass it on to the next line.
	TUint contextChar = 0;
	TUint prevContextChar = 0;
	// Find iStartDocPos.
	TBool foundStart = EFalse;
	TBool get_info = EFalse;
	while (Next())
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_DUMP, DUP1_TTMBYTECODEFINDER_FINDBYTECODE, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		contextChar = LineContextCharChar();
		foundStart = iStartDocPos >= StartChar() && iStartDocPos < EndChar();
		get_info = LineInfo().iLineNumber == 0 || ParStart();
		if (!aToParStart)
			get_info = get_info || !foundStart;
		if (get_info)
			{
			aInfo.iBounds.iTl = LineInfo().iOuterRect.iTl;
			aInfo.iStartCodePos = StartCodePos();
			aInfo.iStartInfo = LineInfo();
			aInfo.iContextCharPerLine = prevContextChar;
			if (aStartBdState && aEndBdState)
				*aStartBdState = *aEndBdState;
			}
		if (foundStart)
			break;

		Skip();
		prevContextChar = contextChar;
		}
	if (!foundStart)
		return EFalse;

	// Find iEndDocPos.
	SetBdStatePtr(aEndBdState);
	if (aEndBdState)
		aEndBdState->Reset();
	TBool foundEnd = EFalse;
	TInt extra_lines = 0;
	do
		{
		// Operators must all be EOpLine because we're skipping from line to line
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, DUP3_TTMBYTECODEFINDER_FINDBYTECODE, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		aInfo.iBounds.iBr = LineInfo().iOuterRect.iBr;
		aInfo.iEndCodePos = EndCodePos();
		aInfo.iEndInfo = LineInfo();
		if (foundEnd)
			{
			extra_lines++;
			SetBdStatePtr(NULL);
			if (!aToParEnd || extra_lines >= aMaxExtraLines)
				break;
			}
		if (!foundEnd)
			foundEnd = iEndDocPos >= StartChar() && iEndDocPos < EndChar();
		if (foundEnd && ParEnd())
			break;
		Skip();
		} while (Next());

	return foundEnd;
	}
	
/*
Find the bytecode at the end of the formatted range.
The section returned must not be bigger than aMaxHeight: if aMaxHeight is sufficiently 
small that we cannot retrieve any whole lines without going over, return EFalse.

Otherwise place information about the section of bytecode in aInfo. 

The bidirectional contexts at the start and end are placed in iStartBdState and 
iEndBdState if both pointers are non-null.
*/
TBool TTmByteCodeFinder::FindByteCodeAtEnd(TInfo& aInfo, TInt aMaxHeight, TInt aTotalHeight, 
	TBidirectionalContext* aStartBdState,TBidirectionalContext* aEndBdState)
	{
	TInt heightToSkip = aTotalHeight - aMaxHeight;
	TInt cumulativeHeight = 0;
	
	if (!((aStartBdState && aEndBdState) || (!aStartBdState && !aEndBdState)))
	    {
	    OstTrace0( TRACE_DUMP, TTMBYTECODEFINDER_FINDBYTECODEATEND, "EBadArg" );
	    }
	__ASSERT_DEBUG((aStartBdState && aEndBdState) || (!aStartBdState && !aEndBdState), TmPanic(EBadArg));
	if (aStartBdState)
		{
		aStartBdState->Reset();
		aEndBdState->Reset();
		}

	// Use aEndBdState to hold the bidirectional state temporarily, so that the following line doesn't overwrite it.
	SetBdStatePtr(aEndBdState);
	// Find iStartDocPos.
	TBool foundStart = EFalse;
	while (Next())
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, DUP1_TTMBYTECODEFINDER_FINDBYTECODEATEND, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		if (cumulativeHeight > heightToSkip)
			{
			foundStart = ETrue;
			aInfo.iBounds.iTl = LineInfo().iOuterRect.iTl;
			aInfo.iStartCodePos = StartCodePos();
			aInfo.iStartInfo = LineInfo();
			if (aStartBdState && aEndBdState)
				{
				*aStartBdState = *aEndBdState;	
				}
			Skip();
			break;
			}
		cumulativeHeight += LineInfo().iOuterRect.Height();
		Skip();
		}
		
	TBool foundEnd = EFalse;	
	while (Next())
		{
		if (Op() != EOpLine)
		    {
		    OstTrace0( TRACE_FATAL, DUP2_TTMBYTECODEFINDER_FINDBYTECODEATEND, "ECorrupt" );
		    }
		__ASSERT_ALWAYS(Op() == EOpLine, Panic(ECorrupt));
		foundEnd = ETrue;
		aInfo.iBounds.iBr = LineInfo().iOuterRect.iBr;
		aInfo.iEndCodePos = EndCodePos();
		aInfo.iEndInfo = LineInfo();
		Skip();
		}
		
	SetBdStatePtr(NULL);
	return foundStart && foundEnd;
	}

