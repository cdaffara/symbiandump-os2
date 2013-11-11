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
#include "InlineText.h"
#include "frmUtils.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TmChunkTraces.h"
#endif


TTmChunk::TTmChunk():
	iDocPos(0),
	iTextLength(0),
	iWidth(0),
	iStandardWidth(0),
	iAscent(0),
	iDescent(0),
	iMaxCharHeight(0),
	iMaxCharDepth(0),
	iFontMaxHeight(0),
	iFontMaxDepth(0),
	iBaselineOffset(0),
	iStretch(0),
	iType(ETextChunk),
	iForcedChunkEnd(EFalse),
	iForcedChunkStart(EFalse),
	iInitialInlineWidth(0),
	iFinalInlineWidth(0),
	iOverlappingChars(0),
	iContextChar(NULL),
	iContextCharInByteCode(NULL)
	{
	}

/*
Truncate to paragraph end, forced line break, picture, tab, or
change of bidirectional character type.
Corrupt surrogate is drawn according to how it maps, see MTmCustom::Map
*/
void TTmChunk::TruncateIfNeeded(CTmFormatContext& aContext, TPtrC& aText, TInfo& aInfo)
	{
	const TText* current_character = aText.Ptr();
	const TText* last_character = current_character + aText.Length();
	iTextLength = aText.Length();
	TBool split_at_word_break =
		aContext.iParFormat.iAlignment == RTmParFormat::EAlignJustify ||
		aContext.iXScale != 1000;
	TBool first_char = TRUE;
	TBool start_with_ZWJ = (current_character < last_character) ? 
					(KZeroWidthJoiner == *current_character) : EFalse;
	while (current_character < last_character)
		{
		// latin shortcut?
		if (!split_at_word_break)
			{
			if (aInfo.iBdCat == TChar::ELeftToRight)
				{
				TUint ch = (*current_character++) | 0x20;
				while ('a' <= ch && ch <= 'z' && current_character < last_character)
					{
					ch = (*current_character++) | 0x20;
					}
				--current_character;
				}
			else if (first_char)
				{
				TUint ch = (*current_character) | 0x20;
				if ('a' <= ch && ch <= 'z')
					{
					first_char = EFalse;
					aInfo.iBdCat = TChar::ELeftToRight;
					++current_character;
					continue;
					}
				}
			}

		if ( TChar::IsLowSurrogate( *current_character ) )
			{
			// skip it
			RDebug::Print(_L("Error: Should not be low surrogate. Skip corrupt low surrogate %X."), *current_character);
			current_character++;
			continue;
			}
		
		const TText *previous_character = current_character;
		TUint character_to_process = *current_character++;
		// If it's surrogate, join the high and low together
		if( TChar::IsHighSurrogate( character_to_process ) )
			{
			TInt high = character_to_process;
			if ( current_character < last_character )
			    {
	            TInt low = *current_character++;
	            if ( !TChar::IsLowSurrogate( low ) )
	                {
	                // should be low surrogate
	                // skip the high surrogate
	                RDebug::Print(_L("Error: %X should be low surrogate. Skip corrupt high surrogate %X."), low, high);
	                current_character--;
	                continue;
	                }
	            character_to_process = TChar::JoinSurrogate( high, low );			    
			    }
			}

		TBool quit = FALSE;
		TBool append_char = TRUE;

		// Break the chunk before a paragraph end, forced line break or page break,
		if (character_to_process == CEditableText::EParagraphDelimiter ||
			character_to_process == CEditableText::ELineBreak ||
			character_to_process == CEditableText::EPageBreak)
			{
			quit = TRUE;
			if (first_char)
				{
				// Paragraph delimiters, line breaks and page breaks each have
				// a chunk to themselves
				if (character_to_process == CEditableText::EParagraphDelimiter)
					aInfo.iAtParEnd = TRUE;
				else if (character_to_process == CEditableText::ELineBreak || 
						character_to_process == CEditableText::EPageBreak)
					{
					aInfo.iAtLineEnd = TRUE;
					}
				append_char = TRUE;
				}
			else
				{
				append_char = FALSE;
				}
			}

		// Deal with tabs, pictures and soft hyphens
		else if (character_to_process == CEditableText::ETabCharacter ||
				 character_to_process == CEditableText::EPictureCharacter ||
				 character_to_process == CEditableText::EPotentialHyphen)
			{
			if (first_char)	// if this is the first character in the chunk, break after the character
				{
				if (character_to_process == CEditableText::ETabCharacter)
					{
					iType = EStandardTabChunk;
					aInfo.iBdCat = TChar::ESegmentSeparator;
					}
				else if (character_to_process == CEditableText::EPictureCharacter)
					iType = EPictureChunk;
				else
					iType = ESoftHyphenChunk;
				}
			else					// else break before it
				append_char = FALSE;
			quit = TRUE;
			}

		/*
		Get the bidirectional category, or break the chunk if it has changed, or
		if the last one was a European Number Separator or Common Number Separator, because the bidirectional
		algorithm needs these to be singletons (single European separators between European numbers change
		to European numbers; single common separators between two numbers of the same type change to
		that type).

		If the current category is whitespace surrounded by two identical strong categories, change it to the strong
		category (and thus don't break). Do the counterpart of this with singleton number separators.
		This is an important optimisation (it speeds reformatting by 15%) that means an ordinary line of English
		containing N words in the same font and style is a single chunk, not N * 2 chunks.

		Change non-spacing marks to the type of the previous category unless the previous category is
		a bidirectional control character.
		*/
		if (!quit)
			{
			TChar::TBdCategory bd_cat = BdCategory(character_to_process);
			if (first_char)
				aInfo.iBdCat = bd_cat;
			else if (bd_cat == TChar::ENonSpacingMark &&
					 aInfo.iBdCat != TChar::ELeftToRightEmbedding &&
					 aInfo.iBdCat != TChar::ELeftToRightOverride &&
					 aInfo.iBdCat != TChar::ERightToLeftEmbedding &&
					 aInfo.iBdCat != TChar::ERightToLeftOverride &&
					 aInfo.iBdCat != TChar::EPopDirectionalFormat)
				bd_cat = aInfo.iBdCat;
			else
				{
				TUint next_character_to_process = *current_character;
				// If the next character is surrogate
		        if( TChar::IsHighSurrogate( next_character_to_process ) )
		            {
		            TInt high = next_character_to_process;
		            if ( current_character < last_character )
		                {
	                    TInt low = *(++current_character);
	                    
	                    if ( !TChar::IsLowSurrogate( low ) )
	                        {
	                        // should be low surrogate
	                        RDebug::Print(_L("Error: %X should be low surrogate. See TmChunk.cpp, TruncateIfNeeded()."), low);
                            // The category of the character is determined by what it's mapped
	                        next_character_to_process = aContext.iSource.Map( high );
	                        }
	                    else
	                        {
	                        next_character_to_process = TChar::JoinSurrogate( high, low );
	                        }
	                    --current_character;		                
		                }
		            }
				TChar::TBdCategory next_cat = BdCategory(next_character_to_process);				
				if (current_character < last_character && bd_cat != aInfo.iBdCat &&
					(bd_cat == TChar::EWhitespace ||
					 bd_cat == TChar::EEuropeanNumberSeparator ||
					 bd_cat == TChar::ECommonNumberSeparator ||
					 bd_cat == TChar::EBoundaryNeutral))
					{
					if (next_cat == TChar::ENonSpacingMark)
						next_cat = aInfo.iBdCat;
					if (bd_cat == TChar::EWhitespace)
						{
						if (aInfo.iBdCat == TChar::ELeftToRight && next_cat == TChar::ELeftToRight)
							bd_cat = TChar::ELeftToRight;
						else if (aInfo.iBdCat == TChar::ERightToLeft && next_cat == TChar::ERightToLeft)
							bd_cat = TChar::ERightToLeft;
						else if (aInfo.iBdCat == TChar::ERightToLeftArabic && next_cat == TChar::ERightToLeftArabic)
							bd_cat = TChar::ERightToLeftArabic;
						}
					else if (aInfo.iBdCat == TChar::EEuropeanNumber && next_cat == TChar::EEuropeanNumber)
						bd_cat = TChar::EEuropeanNumber;
					}
				if (bd_cat != aInfo.iBdCat ||
					aInfo.iBdCat == TChar::EEuropeanNumberSeparator ||
					aInfo.iBdCat == TChar::ECommonNumberSeparator)
					{
					if (start_with_ZWJ)
						{
						start_with_ZWJ = FALSE;
						//Set the bidirectionality to that of the character following the ZWJ
						aInfo.iBdCat = bd_cat;						
						}
					else
						{
						if (KZeroWidthJoiner == character_to_process)
							{							
							//If the bidirectionality of the character following
							//the ZWJ is different, then split after the ZWJ 
							//and set the iOverlappingChars flag
							if (next_cat != aInfo.iBdCat)
								{
								append_char = TRUE;
								quit = TRUE;
								if(next_cat == TChar::ELeftToRight || 
										next_cat == TChar::ERightToLeft || 
										next_cat == TChar::ERightToLeftArabic)
									{
									iOverlappingChars = 1;
									}							
								}
							}
						else
							{
							append_char = FALSE;
							quit = TRUE;							
							}
						}
					}
				}
			}

		/*
		If fully justifying or letterspacing for wysiwyg, make characters that can have space inserted after
		them separate chunks. This also makes it easy to extend underlining or strikethrough to the full
		width of a stretched character.
		*/
		if (split_at_word_break)
			{
			int stretch = aContext.iSource.Stretch(character_to_process);
			if (first_char)
				iStretch = stretch;
			else if (iStretch || stretch)
				{
				quit = TRUE;
				append_char = FALSE;
				}
			}

		first_char = FALSE;
		if (quit)
			{
			if (append_char)
				iTextLength = current_character - aText.Ptr();
			else
				iTextLength = previous_character - aText.Ptr();
			break;
			}
		}
	}

/*
Set up a newly-constructed chunk of text starting at aStartChar, with its left edge at aStartXPos (this information is
needed to determine tab sizes), truncated if necessary to prevent it exceeding aMaxChar and aMaxXPos.
*/
void TTmChunk::SetL(CTmFormatContext& aContext, TInt aStartChar,
	TInt aStartXPos, TInt aMaxChar, TInt aMaxXPos,
	TBool aAllowSpaceForFinalInlineText, TInfo& aInfo)
	{
	iDocPos = aStartChar;

	aInfo.iAtLineEnd = EFalse;
	aInfo.iAtParEnd = EFalse;
	aInfo.iTruncated = EFalse;
	aInfo.iBdCat = TChar::EOtherNeutral;
	aInfo.iEllipsisWidth = 0;
	
	iForcedChunkEnd = EFalse;
	iForcedChunkStart = EFalse;
	
	MGraphicsDeviceMap& device = aContext.Device();

	aMaxChar = Min(aMaxChar, aStartChar + KMaxTextChunkSize);

	// Get text, format and font.
	TPtrC text;
	CTmTextFontCache* font = NULL;
	User::LeaveIfError(aContext.iTextCache.GetTextL(aStartChar, aMaxChar, text, &iFormat, &font));
	CleanupClosePushL(*font);

	// Set the font ascent, descent, and maximum character height and depth.
	iAscent = font->Font().AscentInPixels();
	iDescent = font->Font().DescentInPixels();
	// Retrieve the baseline offset for the font used for this chunk of text.
	// To be used later in window redrawing when the chunks are scanned
	// to determine the overall height of the line that is to be drawn.
	// E.g. 16pt Swiss Superscript is -5, Subscript is +2, Normal is 0.
	iBaselineOffset = font->Font().BaselineOffsetInPixels();
		
	GetMaxHeightAndDepth(&font->Font(), iFontMaxHeight, iFontMaxDepth ); 

	// Check for inline text insertion and shorten aMaxChar if required
	MTmInlineTextSource* inlineTextApi = (MTmInlineTextSource*)aContext.iSource.GetExtendedInterface(KInlineTextApiExtensionUid);
	TBool inlineTextFound = EFalse;
	TInt inlineTextPos = 0;
	TTmDocPos nextDocPos;
	if (inlineTextApi != NULL)
		{
		TTmDocPos startDocPos(aStartChar, ETrue);
		TInt maxLength = aMaxChar - aStartChar;
		TInt err = inlineTextApi->GetNextInlineTextPosition(startDocPos, maxLength, nextDocPos);
		if (err == KErrNone)
			{
			if (nextDocPos.iPos == aStartChar)
				{ // first check for leading inline text
				if (nextDocPos.iLeadingEdge)
					{
					// we found leading text
					iInitialInlineWidth = font->Font().MeasureText(inlineTextApi->GetInlineText(nextDocPos));
					}
				else
					{
					// We should never get trailing edge text at the start position returned here
					// If we do then the GetNextInlineTextPosition implementation is wrong
					TmPanic(EBadReturnValue);
					}
				// Now, is there any more inline text to look out for
				startDocPos.iPos += 1;
				startDocPos.iLeadingEdge = EFalse;
				maxLength -= 1;
				err = inlineTextApi->GetNextInlineTextPosition(startDocPos, maxLength, nextDocPos);
				// if there isn't any more err will be set to KErrNotFound
				}
			// else we have already found a position greater than the start
			}
		if ((err == KErrNone) && ((aMaxChar > nextDocPos.iPos) || ((aMaxChar == nextDocPos.iPos) && !nextDocPos.iLeadingEdge)))
			{
			inlineTextPos = nextDocPos.iPos;
			inlineTextFound = ETrue;
			if (aMaxChar > inlineTextPos)
				{ // shorten aMaxChar
				aMaxChar = inlineTextPos;
				TPtrC oldText = text;
				text.Set(oldText.Left(aMaxChar - aStartChar) );
				}
			}
		}

	TruncateIfNeeded(aContext, text, aInfo);
	if (inlineTextFound && ((aStartChar + iTextLength < inlineTextPos) || nextDocPos.iLeadingEdge))
		{ // It's been truncated so no final inline text
		iFinalInlineWidth = 0;
		inlineTextPos = 0;
		inlineTextFound = EFalse;
		}
	if (iTextLength == 0)
		{
		iInitialInlineWidth = 0; // Just in case it had been set
		CleanupStack::PopAndDestroy(font); // Cleanup
		return;
		}

	if (inlineTextFound && (iDocPos + iTextLength > inlineTextPos))
		iTextLength = inlineTextPos - iDocPos;
	if (inlineTextFound && (aMaxChar == inlineTextPos) && !nextDocPos.iLeadingEdge)
		iFinalInlineWidth = font->Font().MeasureText(inlineTextApi->GetInlineText(nextDocPos));

	// Truncate to the measure and get the metrics.
	int measure = aMaxXPos - aStartXPos;
	if (aAllowSpaceForFinalInlineText && aContext.iParam.IsTruncatingWithEllipsis() && aContext.iParam.iEllipsis != 0xFFFF)
		{
		aInfo.iEllipsisWidth = font->Font().CharWidthInPixels(aContext.iParam.iEllipsis);
		measure -= aInfo.iEllipsisWidth;
		}

	if (iType == EPictureChunk)
		{
		TSize size;
		if (aContext.iSource.GetPictureSizeInTwipsL(aStartChar,size) == KErrNone)
			{
			iWidth = iStandardWidth = device.HorizontalTwipsToPixels(size.iWidth);
			iMaxCharHeight = device.VerticalTwipsToPixels(size.iHeight);
			}
		if (iWidth > measure)
			{
			iTextLength = 0;
			iWidth = 0;
			aInfo.iAtParEnd = FALSE;
			aInfo.iTruncated = TRUE;
			}
		}
	else if (iType == EStandardTabChunk)
		{
		int tabs = aContext.iParFormat.Tabs();
		int pos = 0;
		TBool found = FALSE;
		for (int i = 0; i < tabs && !found; i++)
			{
			TTmTab tab = aContext.iParFormat.Tab(i);
			int cur_pos = device.HorizontalTwipsToPixels(tab.iPosition);
			if (cur_pos > aStartXPos)
				{
				found = TRUE;
				pos = cur_pos;
				if (tab.iType == TTmTab::ECenterTab)
					iType = ECenterTabChunk;
				else if (tab.iType == TTmTab::EReverseTab)
					iType = EReverseTabChunk;
				}
			}
		if (!found)
			{
			int t = device.HorizontalTwipsToPixels(aContext.iParFormat.iTabSize);
			if (t > 0)
				{
				found = TRUE;
				int tab_size = t - aStartXPos % t;
				pos = aStartXPos + tab_size;
				}
			}
		if (found)
			{
			iWidth = iStandardWidth = pos - aStartXPos;
			if (iWidth > measure)
				{
				//If the tab character cannot fit in the remaining space in this line 
				//set the text length to 0 and set the truncated flag to indicate that 			
				//the chunk has been truncated
				iTextLength = 0;
				iWidth = 0;
				aInfo.iAtParEnd = FALSE;
				aInfo.iTruncated = TRUE;
				}
			}
		}
	else
		{
		CFbsFont::TMeasureTextOutput text_metrics;
		TBool right_to_left = aInfo.iBdCat == TChar::ERightToLeft || aInfo.iBdCat == TChar::ERightToLeftArabic;
		TUint orig_context_for_punc = iContextChar;
		// Set the context in the text cache so that it can be used in the RTmTextCache::AdvanceWidthL() function.
		aContext.iTextCache.SetContextChar(orig_context_for_punc);
		
		// First off, check if the whole thing will fit
		TInt width = aContext.iTextCache.AdvanceWidthL(aStartChar,
			aStartChar + iTextLength, right_to_left,
			measure - (iInitialInlineWidth + iFinalInlineWidth), &text_metrics);
		// Check WidthL (probably MeasureText inside it) doesn't come back
		// with a value larger than the max we specified - shouldn't happen
		if (text_metrics.iChars > iTextLength)
		    {
		    OstTrace0( TRACE_DUMP, TTMCHUNK_SETL, "EInvariant" );
		    }
		__ASSERT_DEBUG(text_metrics.iChars <= iTextLength, TmPanic(EInvariant));
		if (text_metrics.iChars < iTextLength)
			{
			// It won't all fit so find out where to chop it
			// reduce the measure by the width of any initial inline text
			// but not by the width of any final inline text - we already know it won't fit
			// and if we don't include the last character then the
			// final inline text won't be there either
			iFinalInlineWidth = 0;
			inlineTextPos = 0;
			inlineTextFound = EFalse;
			aContext.iTextCache.SetContextChar(orig_context_for_punc);
			
			width = aContext.iTextCache.AdvanceWidthL(aStartChar,
				aStartChar + iTextLength, right_to_left,
				measure - iInitialInlineWidth, &text_metrics);
			if (!(text_metrics.iChars < iTextLength) && ((aStartChar + iTextLength) > 1))
				{
				// Problem - It didn't chop any characters off the end but we know that
				// it didn't fit with all the original text plus any initial and final
				// inline text. Since any initial inline text plus all the chars fitted,
				// the only reason it didn't pass before is that there must be some final
				// inline text that won't fit so we need to forcibly chop off the last
				// char (and therefore the inline text) and then pass it through WidthL
				// again because that will detect if that character was part of the end
				// of a glyph cluster and truncate the rest of the cluster.
				aContext.iTextCache.SetContextChar(orig_context_for_punc);

				width = aContext.iTextCache.AdvanceWidthL(aStartChar,
					aStartChar + iTextLength - 1, right_to_left,
 					measure - iInitialInlineWidth, &text_metrics, 1);
				}
			if (text_metrics.iChars == 0)
				{
				// None of the text fitted, so any initial inline text won't fit either (on this line)
				iInitialInlineWidth = 0;
				}
			// It has chopped of at least the last char (and any final inline text) so all is OK
			iTextLength = text_metrics.iChars;
			aInfo.iAtParEnd = FALSE;
			aInfo.iAtLineEnd = FALSE;
			aInfo.iTruncated = TRUE;
			}

		// Add side-bearings to width
		// [Note: do not change this line's < to <= even if it "fixes" some
		// problem! This is not a fix- it merely cripples chunk amalgamation.
		// If you do this, you may prevent text shaping and measurement
		// functions from receiving combinations of characters that they can't
		// cope with- this is their problem, not Form's!]
		if (width < text_metrics.iBounds.iBr.iX)
			{
			// Right side-bearing exists
			width = text_metrics.iBounds.iBr.iX;
			// Don't want to combine with the next chunk
			// or this side-bearing will be lost and the
			// measurements will be wrong
			iForcedChunkEnd = ETrue;
			}
		if (text_metrics.iBounds.iTl.iX < 0)
			{
			// Left side-bearing exists
			width -= text_metrics.iBounds.iTl.iX;
			// Don't want to combine with the previous
			// chunk or the side-bearing will be lost and
			// the measurements will be wrong
			iForcedChunkStart = ETrue;
			}
		// Record total width
		iWidth = iStandardWidth = width + iInitialInlineWidth + iFinalInlineWidth;
		
		iMaxCharHeight = -text_metrics.iBounds.iTl.iY;
		iMaxCharDepth = text_metrics.iBounds.iBr.iY;
		//	iContextCharInByteCode is used to draw THIS chunk within this context
		//	iContextChar is only used to pass on the context to the next chunk.
		iContextChar = aContext.iTextCache.GetContextChar();
		iContextCharInByteCode = aContext.iTextCache.GetContextForByteCode();
		}
	if (iDocPos + iTextLength == aContext.iDocumentLength + 1)
		{
		aInfo.iAtParEnd = ETrue;
		iForcedChunkEnd = ETrue; // because there isn't any more text
		}
	if (inlineTextFound && (iDocPos + iTextLength == inlineTextPos))
		{
		iForcedChunkEnd = ETrue; // because the next position contains inline text
		}
	CleanupStack::PopAndDestroy(font);
	}

