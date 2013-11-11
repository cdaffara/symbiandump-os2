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


#include "TmLayoutImp.h"
#include <s32mem.h> 
#include "TmChunkInterp.h"
#include "InlineText.h"
#include "frmUtils.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TmLineTraces.h"
#endif

inline TBool IsSurrogate(TText a) { return 0xD800 == (a & 0xF800); }
inline TBool IsHighSurrogate(TText a) { return 0xD800 == (a & 0xFC00); }
inline TBool IsLowSurrogate(TText a) { return 0xDC00 == (a & 0xFC00); }
inline TChar JoinSurrogates(TText aHigh, TText aLow)
	{
	return ((aHigh - 0xd7f7) << 10) + aLow;
	}


inline TText16 GetHighSurrogate(TUint aChar)
	{
	return STATIC_CAST(TText16, 0xD7C0 + (aChar >> 10));
	}

inline TText16 GetLowSurrogate(TUint aChar)
	{
	return STATIC_CAST(TText16, 0xDC00 | (aChar & 0x3FF));
	}

CTmLine::CTmLine():
	iText(128),
	iTextWithoutChunkOverlaps(128),
	iChunk(32),
	iRunInfo(32),
	iCountedEndChar(0),
	iHangingEndChar(0),
	iEndChar(0),
	iNextLineStartChar(0),	
	iAtParEnd(FALSE),
	iEllipsis(0xFFFF),
	iEllipsisFormat(0),
	iEllipsisWidth(0),
	iContextChar(NULL)
	{
	}

// Format a line using the information from a format context.
void CTmLine::FormatL(CTmFormatContext& aContext)
	{
	// Reset arrays and parameters.
	iChunk.Reset();
	iText.Reset();
	iTextWithoutChunkOverlaps.Reset();
	iRunInfo.Reset();
	iCountedEndChar = iHangingEndChar = iEndChar = iNextLineStartChar = 0;
	iAtParEnd = FALSE;
	iHeight = iAscent = 0;
	iInnerBounds.SetRect(0,0,0,0);
	iEllipsis = 0xFFFF;

	// Determine the line break.
	int start_x_coord = aContext.iLineInPar == 0 ? aContext.iFirstLineLeftMargin : aContext.iLeftMargin;
	int align_x_coord = aContext.iParam.iWrapWidth -
						(aContext.iLineInPar == 0 ? aContext.iFirstLineRightMargin : aContext.iRightMargin);
	int wrap_x_coord = KMaxTInt;
	if (aContext.iParam.IsWrapping() && !(aContext.iParFormat.iFlags & RTmParFormat::ENoWrap))
		wrap_x_coord = align_x_coord;
	BreakL(aContext,start_x_coord,wrap_x_coord);

	// Calculate the ascent, descent and width.
	TInt width;
	TInt descent;
	CalculateMetrics(aContext,width,iAscent,descent);

	// Determine if the line has any border rules.
	iBordered = aContext.iLeftBorder || aContext.iRightBorder ||
				(aContext.iTopBorder && aContext.iLineInPar == 0) ||
				(aContext.iBottomBorder && iAtParEnd);
	
	// Determine the next bidirectional category and next strong bidirectional category
	aContext.iBdEndOfLine.Set(aContext.iTextCache, iNextLineStartChar);

	/*
	Calculate the bounds of the line. The outer bounds of the lines in a band completely tile
	the band, and are a series of full-width horizontal slices; these are defined completely by their heights,
	so iHeight represents the outer bounds. The inner bounds surround the actual text, not including margins,
	indents and space above and below lines and paragraphs.
	*/
	iInnerBounds.SetRect(start_x_coord,0,start_x_coord + width,KMaxTInt);
	if (aContext.iLineInPar == 0)
		{
		iInnerBounds.iTl.iY += aContext.iSpaceBefore;
		if (iBordered && aContext.iTopBorder)
			iInnerBounds.iTl.iY += aContext.iVBorderMargin + aContext.iTopBorder;
		}
	iHeight = iInnerBounds.iBr.iY = iInnerBounds.iTl.iY + iAscent + descent;
	if (iAtParEnd)
		{
		iHeight += aContext.iSpaceAfter;
		if (iBordered && aContext.iBottomBorder)
			iHeight += aContext.iVBorderMargin + aContext.iBottomBorder;
		}

	// Perform right alignment, centring or full justification.
	int measure = align_x_coord - start_x_coord;
	switch (aContext.iParFormat.iAlignment)
		{
		case RTmParFormat::EAlignNormalBidirectional:
			if (aContext.iParFormat.RightToLeft())
				{
				if(measure - width < 0)
					{
					iInnerBounds.Move(0, 0);	
					}
					else
					{
					iInnerBounds.Move(measure - width, 0);		
					}
				}
			break;
		case RTmParFormat::EAlignReverseBidirectional:
			if (!aContext.iParFormat.RightToLeft())
				iInnerBounds.Move(measure - width, 0);
			break;
		case RTmParFormat::EAlignCenter:
			iInnerBounds.Move((measure - width) / 2,0);
			break;
		case RTmParFormat::EAlignJustify:
			if (iAtParEnd || measure <= width)
				{
				// Default to normal bidirectional for the last line of
				// a paragraph or if the margins are broken.
				if (aContext.iParFormat.RightToLeft())
					iInnerBounds.Move(measure - width, 0);
				}
			else
				Justify(measure - width);
			break;
		case RTmParFormat::EAlignAbsoluteLeft:
			// default is left
			break;
		case RTmParFormat::EAlignAbsoluteRight:
			iInnerBounds.Move(measure - width, 0);
			break;
		default:
			break;
		}
	}

// Find the line break.
void CTmLine::BreakL(CTmFormatContext& aContext,TInt aStartXCoord,TInt aWrapXCoord)
	{
	aContext.iLineEndsInForcedLineBreak = EFalse;
	TInt truncate_x_coord = aWrapXCoord;
	TBool truncating_with_ellipsis = aContext.iParam.IsTruncatingWithEllipsis(); // ####################
	if (truncating_with_ellipsis)
		truncate_x_coord = aContext.iParam.iWrapWidth - aContext.iRightMargin;

	// The following character positions are all relative to the document, not the line.
	TInt start_pos = aContext.iInfo.iEndChar;	// line start
	TInt truncate_pos_with_ellipsis = KMaxTInt;	// position of truncation, allowing room for an ellipsis
	TInt truncate_pos = KMaxTInt;				// position of truncation
	TInt wrap_pos = KMaxTInt;					// position at the full measure
	TInt break_pos = KMaxTInt;					// legal break position

	CTmLine::TEllipsisInfo ellipsisInfo;		// structure used to carry info back from AppendChunk(s)L
	
	// Fill the line up to the truncation width.
	TInt cur_pos = start_pos;
	TInt cur_x_coord = aStartXCoord;

	TInt maxChar = aContext.iDocumentLength + 1;
	
	AppendChunksL(aContext, cur_pos, cur_x_coord, maxChar, truncate_x_coord, truncating_with_ellipsis, ellipsisInfo);
	iEllipsisFormat = ellipsisInfo.iEllipsisFormat;
	iEllipsisWidth = ellipsisInfo.iEllipsisWidth;
	truncate_pos_with_ellipsis = cur_pos;
	if (truncating_with_ellipsis)
		{
		AppendChunksL(aContext, cur_pos, cur_x_coord, maxChar, truncate_x_coord, EFalse, ellipsisInfo);
		}
	truncate_pos = cur_pos;

	// Fill the line up to the wrap width if it is greater than the truncation width.
	if (aWrapXCoord > truncate_x_coord)
		{
		AppendChunksL(aContext, cur_pos, cur_x_coord, maxChar, aWrapXCoord, EFalse, ellipsisInfo);
		}
	wrap_pos = cur_pos;

	// If the line has not been ended by a paragraph break or forced line end find a possible break.
	TBool line_break_found = FALSE;
	if (ellipsisInfo.iAtLineEnd || ellipsisInfo.iAtParEnd)
		{
		break_pos = wrap_pos;
		line_break_found = TRUE;
		aContext.iLineEndsInForcedLineBreak = ETrue;
		}
	TBool get_line_break_forwards = FALSE;
	TInt min_break_pos = start_pos + 1;
	TInt hanging_break_pos = break_pos;
	TInt break_pos_after_spaces = break_pos;
	while (!line_break_found)
		{
		TInt max_break_pos = cur_pos;
		TInt context_end_pos = Min(aContext.iDocumentLength + 1,
			max_break_pos + MTmSource::ELineBreakContext);
		
		// Check if context_end_pos points to surrogate high part
		// If it is, the line break cannot correct, then adjust context_end_pos
		if ( cur_pos <= aContext.iDocumentLength )
		    {
            TTmCharFormat new_format;
            TPtrC text;
            aContext.iSource.GetText( cur_pos, text, new_format );
            if ( context_end_pos > cur_pos && text.Length() > 1 &&
                    IsHighSurrogate( text[ text.Length() - 1 ] ) )
                {
                --context_end_pos;
                }
		    }
		
		// Append line breaking context.
		AppendChunksL(aContext, cur_pos, cur_x_coord, context_end_pos, KMaxTInt, EFalse, ellipsisInfo);
		TInt chars_appended = cur_pos - max_break_pos;
		TPtrC p(iTextWithoutChunkOverlaps.Ptr(),iTextWithoutChunkOverlaps.Length());
		if (aContext.iSource.GetLineBreakL(p,start_pos,min_break_pos - start_pos,max_break_pos - start_pos,
										   get_line_break_forwards,break_pos,hanging_break_pos,break_pos_after_spaces))	
			{
			if (hanging_break_pos > iTextWithoutChunkOverlaps.Length() || break_pos_after_spaces > iTextWithoutChunkOverlaps.Length())
				TmPanic(EBadLineBreak);
			break_pos += start_pos;
			hanging_break_pos += start_pos;
			break_pos_after_spaces += start_pos;
			if (break_pos < min_break_pos || break_pos > max_break_pos)
				TmPanic(EBadLineBreak);
			if (hanging_break_pos < break_pos || break_pos_after_spaces < hanging_break_pos)
				TmPanic(EBadLineBreak);
			line_break_found = TRUE;
			}
		// If no break has been found and no more characters could be added, break here.
		else if (chars_appended == 0)
			{
			break_pos = hanging_break_pos = break_pos_after_spaces = max_break_pos;
			line_break_found = TRUE;
			}

		/*
		If no line break has been found, and if we prefer to break at an illegal position rather
		than exceeding the measure, break at the truncation position but make sure the line
		contains at least one character.
		*/
		if (!line_break_found && !aContext.iParam.LegalLineBreaksOnly())
			{
			break_pos = hanging_break_pos = break_pos_after_spaces = Max(wrap_pos,min_break_pos);
			line_break_found = TRUE;
			}

		/*
		After the first iteration, switch to searching forwards for a line break, because we know that the
		text is now too wide and we want the shortest possible overlength line.
		*/
		get_line_break_forwards = TRUE;
		min_break_pos = max_break_pos;
		}

	if (truncating_with_ellipsis && break_pos > truncate_pos)
		{
		iCountedEndChar = iHangingEndChar = iEndChar = truncate_pos_with_ellipsis;
		iEllipsis = aContext.iParam.iEllipsis;
		}
	else
		{
		iCountedEndChar = break_pos;
		iHangingEndChar = hanging_break_pos;
		iEndChar = break_pos_after_spaces;
		}

	// Determine whether the line ends at the end of the paragraph.
	iAtParEnd = break_pos_after_spaces == ellipsisInfo.iParEndPos;
	if (iAtParEnd)
		aContext.iLineEndsInForcedLineBreak = ETrue;

	iNextLineStartChar = break_pos_after_spaces;

	/*
	Truncate the text after the displayed characters, ensuring there is a break of chunk at this point.
	This makes it easier to justify the line and write the bytecode.
	*/
	if (iCountedEndChar < cur_pos)
		{
		int chunk_start_pos = start_pos;
		int chunk_end_pos = start_pos;
		int chunk_index = 0;
		cur_x_coord = aStartXCoord;
		// find the chunk that contains the end of the text,(iCountedEndChar)
		while (chunk_index < iChunk.Length())
			{
			chunk_end_pos += (iChunk[chunk_index].iTextLength - iChunk[chunk_index].iOverlappingChars);
			if (chunk_end_pos > iCountedEndChar)
				break;
			cur_x_coord += iChunk[chunk_index].iWidth;
			chunk_start_pos = chunk_end_pos;
			chunk_index++;
			}
		//  remove the chunk that contains the last bit of text and any after
		iChunk.Truncate(chunk_index);
		iRunInfo.Truncate(chunk_index);
		iText.Truncate(chunk_start_pos - start_pos);
		cur_pos = chunk_start_pos;

		// add a chunks so there is just enough text to reach the end
		while (cur_pos < iCountedEndChar)
			{
			if (!AppendChunkL(aContext,cur_pos,cur_x_coord,iCountedEndChar,KMaxTInt,FALSE,ellipsisInfo))
				break;
			}
		while (cur_pos < iHangingEndChar)
			{
			if (!AppendChunkL(aContext,cur_pos,cur_x_coord,iHangingEndChar,KMaxTInt,FALSE,ellipsisInfo))
				break;
			}
			
		// If the last chunk is a soft hyphen restore its full width.		
		if (iChunk.Length())
			{
			TTmChunk& last_chunk = iChunk[iChunk.Length() - 1];
			if (last_chunk.iType == TTmChunk::ESoftHyphenChunk)
				last_chunk.iWidth = last_chunk.iStandardWidth;
			}

		}
	}

// Determine the ascent, descent and width of a line.
void CTmLine::CalculateMetrics(CTmFormatContext& aContext,TInt& aWidth,TInt& aAscent,TInt& aDescent) const
	{
	aWidth = 0;
	aAscent = 0;
	aDescent = 0;
	MTmSource::TLineHeightParam p;
	p.iFontMaxAscent = aContext.iBulletAscent;
	p.iFontMaxDescent = aContext.iBulletDescent;
	int i = 0;
	const TTmChunk* c = NULL;
	for (i = 0, c = iChunk.Ptr(); i < iChunk.Length(); i++, c++)
		{
		if (c->iType == TTmChunk::EPictureChunk)
			{
			switch (c->iFormat.iPictureAlignment)
				{
				case TTmCharFormat::EPictureAlignTop:
					if (c->iMaxCharHeight > p.iMaxTopPictureHeight)
						p.iMaxTopPictureHeight = c->iMaxCharHeight;
					break;
				case TTmCharFormat::EPictureAlignBottom:
					if (c->iMaxCharHeight > p.iMaxBottomPictureHeight)
						p.iMaxBottomPictureHeight = c->iMaxCharHeight;
					break;
				case TTmCharFormat::EPictureAlignCenter:
					if (c->iMaxCharHeight > p.iMaxCenterPictureHeight)
						p.iMaxCenterPictureHeight = c->iMaxCharHeight;
					break;
				case TTmCharFormat::EPictureAlignBaseline:
					if (c->iMaxCharHeight > p.iMaxCharHeight)
						p.iMaxCharHeight = c->iMaxCharHeight;
					break;
				}
			}
		else
			{
			// Need to include the baseline offset in the Ascent/Decent 
			// calculations in case the font for this chunk is a 
			// superscript/subscript font. Offset will be 0 for a normal font.
			// Greatest Ascent = iAscent+ABS(iBaselineOffset)
			// Greatest Descent = MAX(p.iFontMaxDescent, c->iBaselineOffset)
			// See defect INC000926.
			if ((c->iAscent-c->iBaselineOffset) > p.iFontMaxAscent)
				p.iFontMaxAscent = c->iAscent - c->iBaselineOffset;
			if ((c->iDescent > p.iFontMaxDescent) || (c->iBaselineOffset > p.iFontMaxDescent))
				p.iFontMaxDescent = Max(c->iDescent, c->iBaselineOffset);

			if (c->iMaxCharHeight > p.iMaxCharHeight)
				p.iMaxCharHeight = c->iMaxCharHeight;
			if (c->iMaxCharDepth > p.iMaxCharDepth)
				p.iMaxCharDepth = c->iMaxCharDepth;
			if (c->iFontMaxHeight > p.iFontMaxCharHeight)
				p.iFontMaxCharHeight = c->iFontMaxHeight;
			if (c->iFontMaxDepth > p.iFontMaxCharDepth)
				p.iFontMaxCharDepth = c->iFontMaxDepth;
			}
		aWidth += c->iWidth;
		}
	if (iEllipsis != 0xFFFF)
		aWidth += iEllipsisWidth;
	
	// If the line has no characters set the ascent and descent to those of the current font.
	if (!iChunk.Length() || !(p.iFontMaxAscent || p.iFontMaxDescent))
		{
		TPtrC text;
		CTmTextFontCache* font = NULL;
		aContext.iTextCache.GetText(aContext.iInfo.iEndChar,KMaxTInt,text,NULL,&font);
		if (font)
			{
			// Changes for defects INC000922 & DEF000925. 
			// If a bullet point is present at the start of an empty paragraph
			// which is bigger than the text font size then we need to preserve
			// the bullet point sizes set above over the font sizes. This is
			// why IFs surround the two assignments below. 
			if (font->Font().AscentInPixels() > p.iFontMaxAscent)
				p.iFontMaxAscent = font->Font().AscentInPixels();
			if (font->Font().DescentInPixels() > p.iFontMaxDescent)
				p.iFontMaxDescent = font->Font().DescentInPixels();
			/* get the MaxCharHeight and MaxCharDepth */
			GetMaxHeightAndDepth(&font->Font(), p.iFontMaxCharHeight, p.iFontMaxCharDepth ); 
			font->Close();
			}
		}

	// Set the line's ascent and descent to take account of desired minimum line height, overlarge characters, etc.
	p.iDesiredLineHeight = aContext.iLineSpacing;
	p.iExactLineHeight = aContext.iExactLineSpacing;
	aContext.iSource.SetLineHeight(p,aAscent,aDescent);
	}

void CTmLine::Justify(TInt aExtraSpace)
	{
	/*
	Find the range to stretch: from the chunk after the last tab to the penultimate chunk;
	but don't stretch if the last tab was not a standard tab.
	*/
	int last = iChunk.Length() - 1;
	int first = last;
	int i;
	TBool non_standard_tab = FALSE;
	for (i = last - 1; i >= 0; i--)
		{
		if (iChunk[i].iType & TTmChunk::ETabFlag)
			{
			if (iChunk[i].iType != TTmChunk::EStandardTabChunk)
				non_standard_tab = TRUE;
			break;
			}
		first = i;
		}
	if (first >= last || non_standard_tab)
		return;

	// Total the stretchability.
	int stretch = 0;
	TTmChunk* c;
	for (i = first, c = iChunk.Ptr() + first; i < last; i++, c++)
		stretch += c->iStretch;

	// Do nothing if there's no stretchability.
	if (stretch == 0)
		return;

	// Adjust the line bounds.
	iInnerBounds.iBr.iX += aExtraSpace;

	// Add space to the stretchable chunks in proportion to their stretchability.
	while (aExtraSpace > 0)
		{
		int dividend = aExtraSpace;
		for (i = first, c = iChunk.Ptr() + first; i < last && aExtraSpace > 0; i++, c++)
			if (c->iStretch > 0)
				{
				c->iType = TTmChunk::EStretchedChunk;
				int increment = (dividend * c->iStretch) / stretch;
				if (increment == 0)
					increment = 1;
				if (increment > aExtraSpace)
					increment = aExtraSpace;
				c->iWidth += increment;
				aExtraSpace -= increment;
				}
		}
	}


void CTmLine::AppendChunksL(CTmFormatContext& aContext,TInt& aStartChar,TInt& aStartXPos,TInt aMaxChar,TInt aMaxXPos,
						      TBool aAllowSpaceForFinalInlineText,CTmLine::TEllipsisInfo& aEllipsisInfo)
	{
	while (aStartChar < aMaxChar && !aEllipsisInfo.iAtLineEnd && !aEllipsisInfo.iAtParEnd)
		{
		aEllipsisInfo.iEllipsisFormat = aStartChar;
		if (!AppendChunkL(aContext, aStartChar, aStartXPos, aMaxChar, aMaxXPos, aAllowSpaceForFinalInlineText, aEllipsisInfo))
			break;
		}
	if (aEllipsisInfo.iAtParEnd)
		aEllipsisInfo.iParEndPos = aStartChar;
	}


TBool CTmLine::AppendChunkL(CTmFormatContext& aContext,TInt& aStartChar,TInt& aStartXPos,TInt aMaxChar,TInt aMaxXPos,
						    TBool aAllowSpaceForFinalInlineText,CTmLine::TEllipsisInfo& aEllipsisInfo)
	{
	/*
	Find any previous non-standard tab, so that we can adjust the measure to allow for characters pushed left.
	Accumulate the segment width (width of chunks between tabs), which will be used to
	adjust the width of the previous tab if necessary.

	Return TRUE if it was possible to append a new chunk of non-zero length. It will be impossible if
	aMaxChar is <= aStartChar or the chunk consists of a ligation that ends after aMaxChar, or the smallest thing
	that can be appended is wider than aMaxXPos - aStartXPos.
	*/
	int segment_width = 0;
	TTmChunk* prev_tab = NULL;

	if (iChunk.Length() > 0)
		{
		int i = iChunk.Length() - 1;
		TTmChunk* c = &iChunk[i];
		while (i >= 0 && !prev_tab)
			{
			if (c->iType & TTmChunk::ETabFlag)
				{
				if (c->iType != TTmChunk::EStandardTabChunk)
					{
					prev_tab = c;
					int avail = aMaxXPos - aStartXPos;
					if (prev_tab->iType == TTmChunk::ECenterTabChunk)
						aMaxXPos += Min(avail,prev_tab->iWidth);
					else
						aMaxXPos += prev_tab->iWidth;
					}
				break;
				}
			else
				segment_width += c->iWidth;
			c--;
			i--;
			}
		}

	//+ strip trailing whitespace when measuring centred segment?

	// Prepare to zero the metrics of the preceding chunk if it was a soft hyphen.
	int prev_soft_hyphen_width = 0;
	if (iChunk.Length() && iChunk[iChunk.Length() - 1].iType == TTmChunk::ESoftHyphenChunk)
		prev_soft_hyphen_width = iChunk[iChunk.Length() - 1].iWidth;

	// Create the new chunk.
	TTmChunk chunk;
	TTmChunk::TInfo chunkInfo;
	// If this is the first chunk of the line, then take the context  from the line. Otherwise, take
	// it form the previous chunk in this line.
	if (iChunk.Length() == 0)
		chunk.iContextChar = iContextChar;
	else
		chunk.iContextChar = iChunk[iChunk.Length() - 1].iContextChar;
	
	chunk.SetL(aContext,aStartChar,aStartXPos - prev_soft_hyphen_width,aMaxChar,aMaxXPos,aAllowSpaceForFinalInlineText,chunkInfo);
	// copy over info to be passed back
	aEllipsisInfo.iEllipsisWidth = chunkInfo.iEllipsisWidth;
	aEllipsisInfo.iAtLineEnd = chunkInfo.iAtLineEnd;
	aEllipsisInfo.iAtParEnd = chunkInfo.iAtParEnd;
	TBidirectionalState::TRunInfo run_info;
	run_info.iCategory = chunkInfo.iBdCat;
	run_info.iIndex = iChunk.Length();
	
	// Add the chunk and the bidirectional information to the arrays.
	if (chunk.iTextLength > 0)
		{
		if (prev_soft_hyphen_width)
			{
			TTmChunk& c = iChunk[iChunk.Length() - 1];
			c.iWidth = 0;
			c.iAscent = 0;
			c.iDescent = 0;
			c.iMaxCharHeight = 0;
			c.iMaxCharDepth = 0;
			aStartXPos -= prev_soft_hyphen_width;
			}

		iChunk.AppendL(chunk);
		int start = chunk.iDocPos;
		int end = chunk.iDocPos + chunk.iTextLength;
		TPtrC text;
		while (start < end)
			{
			aContext.iTextCache.GetText(start,end,text);
			iText.AppendL(text.Ptr(),text.Length());
			iTextWithoutChunkOverlaps.AppendL(text.Ptr(),text.Length() - chunk.iOverlappingChars);
			start += text.Length();
			}

		iRunInfo.AppendL(run_info);

		aStartChar += chunk.iTextLength;
		if (chunk.iOverlappingChars > 0 && ! chunkInfo.iAtLineEnd && ! chunkInfo.iAtParEnd)
			{
			aStartChar -= chunk.iOverlappingChars;
			if (aStartChar < 0)
			    {
			    OstTrace0( TRACE_DUMP, CTMLINE_APPENDCHUNKL, "EInvariant" );
			    }
			__ASSERT_DEBUG(aStartChar >= 0, TmPanic(EInvariant));
			}

		aStartXPos += chunk.iWidth;

		// Adjust the width of any previous centring or right-aligning tab unless the new chunk is itself a tab
		if (prev_tab && !(chunk.iType & TTmChunk::ETabFlag))
			{
			segment_width += chunk.iWidth;
			int old_chunk_width = prev_tab->iWidth;
			if (prev_tab->iType == TTmChunk::ECenterTabChunk)
				prev_tab->iWidth = Max(0,prev_tab->iStandardWidth - segment_width / 2);
			else
				prev_tab->iWidth = Max(0,prev_tab->iStandardWidth - segment_width);
			aStartXPos += prev_tab->iWidth - old_chunk_width;
			}
		if (chunkInfo.iTruncated)	// move test for truncation to here from loop around AppendChunkL
			return EFalse;
		return ETrue;
		}
	else
		return EFalse;
	}

void CTmLine::WriteCodeL(CTmFormatContext& aContext)
	{
	TInt line_start_bytecode_pos = aContext.iCode.Size();
	CTmCode& code = aContext.iCode;
	code.CreateBufferL();
	TInt doc_pos = aContext.iInfo.iEndChar;
	
	// Context for the line. From logic end chunk in this line
	TUint lineContextChar = iChunk[iChunk.Length()-1].iContextChar;

	// Write the bidirectional state in force at the start of the line.
	if (!aContext.iBdState.IsDefault())
		WriteBdStateL(aContext);

	// Reorder the line. This changes the bidirectional state so has to be done after that is written.
	TBool visualEndIsAmbiguous;
	aContext.iBdState.ReorderLine(iRunInfo.Ptr(),iRunInfo.Length(),
		aContext.iLineInPar == 0, AtParEnd(), aContext.iParFormat.RightToLeft(),
		aContext.iBdEndOfLine.FirstCategory(),
		aContext.iBdEndOfLine.FirstStrongCategory(),
		visualEndIsAmbiguous);

	// Create and write the paragraph label if any. Trap the operation so that label mode can be cancelled.
	if (aContext.iLineInPar == 0)
		{
		MFormLabelApi* labelApi = (MFormLabelApi*)aContext.iSource.GetExtendedInterface(KFormLabelApiExtensionUid);
		if ((labelApi != NULL) && (labelApi->LabelModeSelect(MTmSource::EParLabel, aContext.iInfo.iEndChar)))
			{
			TRAPD(error, WriteLabelCodeL(aContext,MTmSource::EParLabel));
			labelApi->LabelModeCancel();
			if (error)
				User::Leave(error);
			}
		}

	// Write out the termination characters if they are on the left because
	// the paragraph is right to left.
	TInt termination_chars = iEndChar - iHangingEndChar;
	if (termination_chars > 0 && aContext.iParFormat.RightToLeft())
		{
		TUint8 op = static_cast<TUint8>(TTmInterpreter::EOpText
			| TTmInterpreter::EModDontMeasure
			| TTmInterpreter::EModRightToLeft);
		if (iHangingEndChar != doc_pos)
			op |= TTmInterpreter::EModPos;
		if (termination_chars != 1)
			op |= TTmInterpreter::EModCount;
		code.AppendByteL(op);
		if (op & TTmInterpreter::EModPos)
			code.AppendNumberL(iHangingEndChar - doc_pos);
		if (op & TTmInterpreter::EModCount)
			code.AppendNumberL(termination_chars);
		TInt width = aContext.iTextCache.TotalWidthL(iHangingEndChar, iEndChar, ETrue);
		code.AppendNumberL(-width);
		// Append context , which is NULL in this case
		code.AppendNumberL(0);
		doc_pos = iEndChar;
		}
	/*
	Write the line in the order defined by iRunInfo.
	Amalgamate adjacent text chunks with the same direction. This is a very worthwhile
	optimisation to the size of the generated bytecode.
	Chunks of type ETextChunk are pieces of text to be displayed at their natural widths, so can be concatenated.
	*/
	TInt group_start = 0;
	TInt group_left = 0;
	TInt group_right = 0;
	TUint contextChar = 0;
    TInt line_start_position = 0;
    //Record current line start position which will be used to get line offset position of inlineText
    if (iRunInfo.Length() > 0)
        {
        TInt first_group_start_index = iRunInfo[0].iIndex;
        TTmChunk* first_group_start_chunk = &iChunk[first_group_start_index];
        line_start_position = first_group_start_chunk->iDocPos;
        }
    
	while (group_start < iRunInfo.Length())
		{
		TInt group_end = group_start + 1;

		// Decide the base operator.
		TInt group_start_index = iRunInfo[group_start].iIndex;
		TInt direction = iRunInfo[group_start].iDirection;
		TTmChunk* group_start_chunk = &iChunk[group_start_index];
		TInt pos = group_start_chunk->iDocPos;
		TInt length = group_start_chunk->iTextLength;
		TInt initialInlineWidth = group_start_chunk->iInitialInlineWidth;
		TInt finalInlineWidth = group_start_chunk->iFinalInlineWidth;
		TBool forcedChunkEnd = group_start_chunk->iForcedChunkEnd;
		contextChar = group_start_chunk->iContextCharInByteCode;
		
		TInt inlineFormat = pos >= line_start_position ?
		    pos - line_start_position :
            line_start_position - pos;//Position offset in current line
		group_right += group_start_chunk->iWidth;
		TInt subscript = 0;
		TUint8 op = 0;
		TBool skip_this_chunk = EFalse;
		switch (group_start_chunk->iType)
			{
			case TTmChunk::ETextChunk:
			case TTmChunk::EStretchedChunk:
				op = TTmInterpreter::EOpText;
				break;
			case TTmChunk::ESoftHyphenChunk:
				op = TTmInterpreter::EOpText;
				if (group_start < iRunInfo.Length() - 1)
					skip_this_chunk = ETrue;
				break;
			case TTmChunk::EStandardTabChunk:
			case TTmChunk::ECenterTabChunk:
			case TTmChunk::EReverseTabChunk:
				op = TTmInterpreter::EOpText;
				break;
			case TTmChunk::EPictureChunk:
				op = TTmInterpreter::EOpSpecialChar | TTmInterpreter::EModSubscript;
				switch (group_start_chunk->iFormat.iPictureAlignment)
					{
					case TTmCharFormat::EPictureAlignTop:
						subscript = group_start_chunk->iMaxCharHeight - iAscent;
						break;
					case TTmCharFormat::EPictureAlignBottom:
						subscript = iInnerBounds.Height() - iAscent;
						break;
					case TTmCharFormat::EPictureAlignCenter:
						subscript = (iInnerBounds.Height() + group_start_chunk->iMaxCharHeight) / 2 - iAscent;
						break;
					case TTmCharFormat::EPictureAlignBaseline:
						subscript = 0;
						break;
					}
				break;
			default:
				TmPanic(EBadChunkType);
				break;
			}

		TBool ellipsis_needed = EFalse;
		TInt overlappingCharacters = group_start_chunk->iOverlappingChars;

		if (!skip_this_chunk)
			{

			// Amalgamate text chunks if possible.
			if (aContext.iXScale == 1000 && group_start_chunk->iType == TTmChunk::ETextChunk)
				while (group_end < iRunInfo.Length())
					{
					TInt group_end_index = iRunInfo[group_end].iIndex;
					TTmChunk* group_end_chunk = &iChunk[group_end_index];
					TInt index_difference = group_end - group_start;
					if (direction == 1)
						index_difference = -index_difference;
					
					// Check to see if the chunk cannot be amalgamated
					if (group_end_index - group_start_index != index_difference ||		// test for adjacency,
						iRunInfo[group_end].iDirection != direction ||					// same direction,
						group_end_chunk->iType != TTmChunk::ETextChunk ||				// text type
						group_end_chunk->iFormat != group_start_chunk->iFormat ||		// same format
						// inline text insertion or side-bearings supress amalgamation
						forcedChunkEnd || group_end_chunk->iForcedChunkStart ||
						length + group_end_chunk->iTextLength > KMaxTextChunkSize)		// within maximum chunk length
							{
							break;
							}
						
					if (direction == 0)
						{
						if (finalInlineWidth + group_end_chunk->iInitialInlineWidth != 0)
							break;
						}
					else
						{
						if (initialInlineWidth + group_end_chunk->iFinalInlineWidth != 0)
							break;
						}
					if (direction == 1)
						pos = group_end_chunk->iDocPos;
					
					//Handle the length of chunks with overlapping characters ensuring
					//that when the chunks are amalgamated, the length is corrected.
					length += group_end_chunk->iTextLength - overlappingCharacters;
					overlappingCharacters = group_end_chunk->iOverlappingChars;
					
					group_right += group_end_chunk->iWidth;
					initialInlineWidth += group_end_chunk->iInitialInlineWidth;
					finalInlineWidth += group_end_chunk->iFinalInlineWidth;
					forcedChunkEnd = group_end_chunk->iForcedChunkEnd;
					// If chunks are amalgamated, the context from the first chunk will provide context for the
					// amalgamated chunks.
					contextChar = group_start_chunk->iContextCharInByteCode;

					group_end++;
					}

			// Scale the width and subscript if necessary.
			TInt width = group_right - group_left;
			if (aContext.iXScale != 1000)
				{
				width = (group_right * aContext.iXScale / 1000) -
						(group_left * aContext.iXScale / 1000);
				}
			width -= (initialInlineWidth + finalInlineWidth);

			if (aContext.iYScale != 1000)
				subscript = subscript * aContext.iYScale / 1000;

			// Write a leading ellipsis if this is the content-final chunk and is right-to-left.
			ellipsis_needed = iEllipsis != 0xFFFF && iRunInfo[group_end - 1].iIndex == iChunk.Length() - 1;
			if (ellipsis_needed && direction == 1)
				{
				code.AppendByteL(TTmInterpreter::EOpInlineText);
				code.AppendNumberL(iEllipsisWidth);
				if (iEllipsisFormat >= line_start_position)
					code.AppendNumberL(iEllipsisFormat - line_start_position);
				else
					code.AppendNumberL(line_start_position - iEllipsisFormat);
				code.AppendNumberL(iEllipsis);
				}

			// Is there any inline text to go here - before the chunk starts
			if ((initialInlineWidth > 0) && (direction == 0))
				{ // yes - go fetch it
				WriteInlineTextL(aContext, code, pos, initialInlineWidth, ETrue, inlineFormat);
				}
			else if ((finalInlineWidth > 0) && (direction == 1))
				{ // yes - go fetch it
				WriteInlineTextL(aContext, code, pos + length, finalInlineWidth, EFalse, inlineFormat);
				}

			// Decide which optional arguments are needed and write the operator and its arguments.
			if (pos != doc_pos)
				op |= TTmInterpreter::EModPos;
			if (length > 1)
				op |= TTmInterpreter::EModCount;
			if (direction == 1)
				op |= TTmInterpreter::EModRightToLeft;
			code.AppendByteL(op);
			if (op & TTmInterpreter::EModPos)
				code.AppendNumberL(pos - doc_pos);
			if (op & TTmInterpreter::EModCount)
				code.AppendNumberL(length);
			if (op & TTmInterpreter::EModSubscript) // at present used only for pictures
				code.AppendNumberL(subscript);
			code.AppendNumberL(width);
			code.AppendNumberL(contextChar);	// Append the context  for this chunk
			
			// Is there any inline text to go here - after the end of the chunk
			if ((finalInlineWidth > 0) && (direction == 0))
				{ // yes - go fetch it
				WriteInlineTextL(aContext, code, pos + length, finalInlineWidth, EFalse, inlineFormat);
				}
			else if ((initialInlineWidth > 0) && (direction == 1))
				{ // yes - go fetch it
				WriteInlineTextL(aContext, code, pos, initialInlineWidth, ETrue, inlineFormat);
				}

			// Write a trailing ellipsis if this is the content-final chunk and is left_to_right
			if (ellipsis_needed && direction == 0)
				{
				code.AppendByteL(TTmInterpreter::EOpInlineText);
				code.AppendNumberL(iEllipsisWidth);
				if (iEllipsisFormat >= line_start_position)
					code.AppendNumberL(iEllipsisFormat - line_start_position);
				else
					code.AppendNumberL(line_start_position - iEllipsisFormat);
				code.AppendNumberL(iEllipsis);
				}

			doc_pos = pos + length;
			}

		group_left = group_right;
		if (ellipsis_needed)
			group_left += iEllipsisWidth;
		group_start = group_end;
		}

	// Write bytecode representing any control characters at the right end of
	// the line, if the paragraph is left-to-right.
	if (termination_chars > 0 && !aContext.iParFormat.RightToLeft())
		{
		TUint8 op = (TUint8)(TTmInterpreter::EOpText | TTmInterpreter::EModDontMeasure);
		if (iHangingEndChar != doc_pos)
			op |= TTmInterpreter::EModPos;
		if (termination_chars != 1)
			op |= TTmInterpreter::EModCount;
		code.AppendByteL(op);
		if (op & TTmInterpreter::EModPos)
			code.AppendNumberL(iHangingEndChar - doc_pos);
		if (op & TTmInterpreter::EModCount)
			code.AppendNumberL(termination_chars);
		code.AppendNumberL(0);
		// Append context , which is NULL in this case
		code.AppendNumberL(0);

		}

	// Scale the line dimensions if necessary.
	TRect inner_bounds = iInnerBounds;
	TInt ascent = iAscent;
	TInt height = iHeight;
	if (aContext.iXScale != 1000 || aContext.iYScale != 1000)
		{
		inner_bounds.iTl.iX = inner_bounds.iTl.iX * aContext.iXScale / 1000;
		inner_bounds.iBr.iX = inner_bounds.iBr.iX * aContext.iXScale / 1000;
		inner_bounds.iTl.iY = inner_bounds.iTl.iY * aContext.iYScale / 1000;
		inner_bounds.iBr.iY = inner_bounds.iBr.iY * aContext.iYScale / 1000;
		ascent = ascent * aContext.iYScale / 1000;

		// Treat height specially to avoid cumulative rounding errors in the height of a document.
		TInt line_top = aContext.iInfo.iHeight * aContext.iYScale / 1000;
		TInt line_bottom = (aContext.iInfo.iHeight + iHeight) * aContext.iYScale / 1000;
		height = line_bottom - line_top;
		}

	// Write the bytecode giving the line information.
	TInt lineFlags = 0;
	if (aContext.iLineEndsInForcedLineBreak)
		lineFlags |= TTmInterpreter::ELineFlagLineEndsInForcedLineBreak;
	else if (visualEndIsAmbiguous)
		// line end cannot be ambiguous if there is a line break, even
		// if the heuristics say it might be.
		lineFlags |= TTmInterpreter::ELineFlagVisualEndOfLineIsAmbiguous;
	TInt bytecode_size = code.Size() - line_start_bytecode_pos;
	TUint8 op = TTmInterpreter::EOpLine;
	if (aContext.iParFormat.RightToLeft())
		op |= TTmInterpreter::EModRightToLeft;
	if (iBordered)
		op |= TTmInterpreter::EModBordered;
	if (aContext.iLineInPar == 0)
		op |= TTmInterpreter::EModParStart;
	if (iAtParEnd)
		op |= TTmInterpreter::EModParEnd;
	if (lineFlags != 0)
		op |= TTmInterpreter::EModLineFlags;
	// Line info is inserted back at the beginning of this stretch of code
	code.InsertByteL(op,line_start_bytecode_pos);
	TInt pos = line_start_bytecode_pos + 1;
	// <N> Number of characters in the line
	pos = code.InsertNumberL(iNextLineStartChar - aContext.iInfo.iEndChar, pos);
	// <C> Context  for the line
	pos = code.InsertNumberL(lineContextChar, pos);
	// <F> Line flags
	if (lineFlags != 0)
		pos = code.InsertNumberL(lineFlags, pos);
	// <B> Size of the bytecode
	pos = code.InsertNumberL(bytecode_size, pos);
	// <H> Height of the line
	pos = code.InsertNumberL(height, pos);
	// <I> The inner bounds of the line, relative to the line's top-left corner
	pos = code.InsertRectL(inner_bounds, pos);
	// <A> The ascent: the distance between the baseline and the top of the line
	pos = code.InsertNumberL(ascent, pos);
	}

void CTmLine::WriteInlineTextL(CTmFormatContext& aContext, CTmCode& aCode, TInt aPos, TInt aInlineWidth, TBool aLeadingEdge, TInt aInlineFormat)
	{
	MTmInlineTextSource* inlineTextApi = (MTmInlineTextSource*)aContext.iSource.GetExtendedInterface(KInlineTextApiExtensionUid);
	if (inlineTextApi==0)
	    {
	    OstTrace0( TRACE_DUMP, CTMLINE_WRITEINLINETEXTL, "EInvariant" );
	    }
	__ASSERT_DEBUG(inlineTextApi!=0,TmPanic(EInvariant));

	TTmDocPos startDocPos(aPos, aLeadingEdge);
	TPtrC inlineText = inlineTextApi->GetInlineText(startDocPos);
	TUint8 op2 = TTmInterpreter::EOpInlineText;
	TInt inlineLength = inlineText.Length();
	if (inlineLength > TTmInterpreter::EMaxInlineChars)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTMLINE_WRITEINLINETEXTL, "EInvariant" );
	    }
	__ASSERT_DEBUG(inlineLength <= TTmInterpreter::EMaxInlineChars,TmPanic(EInvariant));
	if (inlineLength > 1)
		op2 |= TTmInterpreter::EModCount;
	aCode.AppendByteL(op2);
	if (inlineLength > 1)
		aCode.AppendNumberL(inlineLength);
	aCode.AppendNumberL(aInlineWidth);
	aCode.AppendNumberL(aInlineFormat);
	for (TInt i = 0; i < inlineLength; i++)
		{
		aCode.AppendNumberL(inlineText[i]);
		}
	}

void CTmLine::WriteLabelCodeL(CTmFormatContext& aContext,MTmSource::TLabelType aLabelType)
	{
	// Format the label.
	CTmCode *label_code = new(ELeave) CTmCode;
	CleanupStack::PushL(label_code);
	TTmFormatParam param;
	param.iMaxLines = 1;	// labels have only one line
	TSize label_size(0, 0);
	TInt margin_size = 0;
	MFormLabelApi* labelApi = (MFormLabelApi*)aContext.iSource.GetExtendedInterface(KFormLabelApiExtensionUid);
	if (labelApi != NULL)
		labelApi->LabelMetrics(aLabelType, label_size, margin_size);
	param.iWrapWidth = label_size.iWidth;
	CTmFormatContext::TInfo info;
	info.iContextCharPerLine = NULL;
	CTmFormatContext::FormatL(aContext.iSource,param,*label_code,info, aContext.iTextLayout);

	// Make sure the main line's height and ascent are big enough to accommodate the label.
	TTmInterpreterParam interpreter_param(*label_code);
	interpreter_param.iWidth = info.iWidth;
	TTmInterpreter interpreter(interpreter_param);
	interpreter.LineNumberToLine(0);
	int label_ascent = interpreter.LineInfo().iBaseline - interpreter.LineInfo().iOuterRect.iTl.iY;
	int label_descent = interpreter.LineInfo().iOuterRect.Height() - label_ascent;
	int line_descent = iHeight - iAscent;
	iAscent = Max(iAscent,label_ascent);
	iHeight = iAscent + Max(line_descent,label_descent);

	// Insert the bytecode describing the label.
	CTmCode& code = aContext.iCode;
	code.AppendByteL(TTmInterpreter::EOpLabel | TTmInterpreter::EModCount);
	code.AppendNumberL(0);	// a label consumes no bytes from the main text source
	code.AppendNumberL(label_code->Size());
	TRect r;
	r.iTl.iX = -margin_size;
	r.iBr.iX = r.iTl.iX + info.iWidth;
	r.iBr.iY = iAscent + label_descent;
	r.iTl.iY = r.iBr.iY - info.iHeight;
	code.AppendRectL(r);
	code.AppendNumberL(aLabelType);

	// Insert the label bytecode.
	code.ChangeL(code.Size(),code.Size(),*label_code);

	// Clean up.
	CleanupStack::PopAndDestroy();	// label_code
	}

void CTmLine::WriteBdStateL(CTmFormatContext& aContext)
	{
	int start_bytecode_pos = aContext.iCode.Size();
	CTmCode& code = aContext.iCode;
	RBufWriteStream stream(*code.Buffer(),start_bytecode_pos);
	aContext.iBdState.ExternalizeL(stream);
	aContext.iBdEndOfLine.ExternalizeL(stream);
	stream.CommitL();
	stream.Close();
	int bytecode_size = code.Size() - start_bytecode_pos;
	int pos = start_bytecode_pos;
	code.InsertByteL(TTmInterpreter::EOpParam,pos++);
	code.InsertByteL(TTmInterpreter::EParamBdState,pos++);
	code.InsertNumberL(bytecode_size,pos);
	}
void CTmLine::SetContextChar(TUint aContext, TBool aFromChunk)
	{
	// If the last chunk in the line has context, then take it from there, else take it from the one provided.
	if (aFromChunk && iChunk.Length() > 0)
		iContextChar = iChunk[iChunk.Length() - 1].iContextChar;
	else
		iContextChar = aContext;
	}
