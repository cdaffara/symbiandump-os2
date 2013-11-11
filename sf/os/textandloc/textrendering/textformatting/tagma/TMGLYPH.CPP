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
* Text caching for TAGMA: cacheing of text and formats extracted from a text source,
*
*/


#include "TMSTD.H"
#include <bidi.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TMGLYPHTraces.h"
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

TUint RTmTextCache::Char(TInt aPos)
	{
	if (aPos < 0 || aPos > iDocumentLength)
		return 0xFFFF;
	if (aPos == iDocumentLength)
		return CEditableText::EParagraphDelimiter;
	if (aPos < iTextStart || aPos >= iTextStart + iTextLength)
		{
		TPtrC p;
		GetText(aPos,KMaxTInt,p);
		}
	if ( IsHighSurrogate( iText[aPos-iTextStart] ) ) 
		{
		if (iTextStart+iTextLength-aPos > 1)
			{
			TText high = iText[aPos-iTextStart];
			TText low = iText[aPos-iTextStart+1];
			if ( IsLowSurrogate( low ) )
			    return JoinSurrogates( high, low );
			else
			    // If the corrupt surrogate, it is treated as what it is mapped
			    return iSource.Map( high );
			}
		else
			{
			// If the corrupt surrogate, it is treated as what it is mapped
			return iSource.Map( iText[aPos-iTextStart] );
			}
		}
	else
		{
		return iText[aPos - iTextStart];
		}
	}

/**
Measure the width of some text, substituting glyphs where necessary.

@param aStart Start position of text in document
@param aEnd (Exclusive) end position of text in document
@param aRightToLeft True if the text is right-to-left
@param aMaxAdvance Measurement will stop if this advance is exceeded
@param aOutput Output form text measurement function
@param aExtraChar
	Fetch this much context beyond aEnd. This helps when the text is to be
	truncated, and aEnd is not certain to be at a cluster boundary.
@return Advance width. Side-bearings are not included. */
TInt RTmTextCache::AdvanceWidthL(TInt aStart, TInt aEnd, TBool aRightToLeft,
						  TInt aMaxAdvance, CFbsFont::TMeasureTextOutput* aOutput, TInt aExtraChar)
	{
	// Get the displayed text, up to the copy fit width if necessary.
	TInt length = aEnd - aStart;
	TText buffer[KMaxTextChunkSize + 2 + 1];
	if (length > KMaxTextChunkSize)
		TmPanic(ETextWidthBufferOverflow);
	
	CTmTextFontCache *font = NULL;

	// Get the text in logical order. If a custom interface to draw (in later stages)
	// the text in context exists, then measure the text with context.
	TInt error;
	if (Source().GetExtendedInterface(KTmCustomExtensionUid))
		{
		error = GetDisplayedText(aStart, aEnd + aExtraChar,
								  aRightToLeft ? ELogicalRightToLeft : ELeftToRight,
								  buffer,iContextCharPerChunk, 0, &font);
		}
		else
			{
			error = GetDisplayedText(aStart, aEnd + aExtraChar,
								  aRightToLeft ? ELogicalRightToLeft : ELeftToRight,
								  buffer,0, 0, &font);
			}
	
	// Don't push font onto cleanup stack because this method only leaves below.
	// If there was an error in GetDisplayedText, font will have been closed
	if (error)
		User::Leave(error);

	CFont::TMeasureTextInput input;
 	input.iFlags = aRightToLeft?
 		CFont::TMeasureTextInput::EFVisualOrderRightToLeft
 		: CFont::TMeasureTextInput::EFVisualOrder;
   
   	// Create a new buffer which will have the context  as well
   	TPtrC inputText(buffer, length + 2 + aExtraChar);
   	input.iStartInputChar = 1;
   	input.iEndInputChar = input.iStartInputChar + length;
   	input.iMaxBounds = aMaxAdvance;
   	input.iFlags |= CFont::TMeasureTextInput::EFIncludePenPositionInBoundsCheck;
 	CFbsFont::TMeasureTextOutput output;
 	CFbsFont::TMeasureTextOutput* pOutput = aOutput? aOutput : &output;
 	// Measure this text in the context supplied. Any punctuations in this chunk
 	// will now be measured based on this context.
 	TInt width = font->Font().MeasureText(inputText, &input, pOutput);
 	// Do not report initial Zero-Width Joiner or 0xFFFF
 	if (0 != pOutput->iChars)
 		--pOutput->iChars;
	
	// Find the new context at the end of this chunk. This will be used to pass on to the next chunk.
	// If the context hasn't changed, pass on the previous context.
	TUint last;
	TInt textLength = input.iStartInputChar + pOutput->iChars;
	TChar::TBdCategory cat = TChar::ELeftToRight;
	while (textLength != 0)
		{
		TUint charSize = 1;
		last = *(inputText.Ptr() + textLength);
		if ( IsSurrogate( last ) && textLength > 1 ) 
			{
			TText low = *(inputText.Ptr() + textLength);
			TText high = *(inputText.Ptr() + textLength - 1);
			if ( IsLowSurrogate( low ) && IsHighSurrogate( high ) )
				{
				charSize = 2;
				last = JoinSurrogates( high, low );
				}
			else
			    {
			    // If the corrupt surrogate, it is treated as what it is mapped
			    last = iSource.Map( last );
			    }
			}
		
		cat = TChar(last).GetBdCategory();
		if (last != 65535 && textLength !=0 &&
			(cat == TChar::ELeftToRight ||
			 cat == TChar::ERightToLeft ||
			 cat == TChar::ERightToLeftArabic))
			{
			iContextCharPerChunk = last;
			break;	
			}
		textLength -= charSize;
		}
	
	// Check if this chunk is a punctuation chunk. If it is not, the context in the byte code for this chunk
	// will be NULL. Otherwise it will just be the previous context.
	if (cat == TChar::ELeftToRight ||
		 cat == TChar::ERightToLeft ||
		 cat == TChar::ERightToLeftArabic ||
		 cat == TChar::EWhitespace)
		{
		iContextCharInByteCode = NULL;
		}
	else
		iContextCharInByteCode = iContextCharPerChunk;

	font->Close();

	return width;
	}

/** Measure the width of some text, substituting glyphs where necessary.
@param aStart Start position of text in document
@param aEnd (Exclusive) end position of text in document
@param aRightToLeft True if the text is right-to-left
@return width including side-bearings */
TInt RTmTextCache::TotalWidthL(TInt aStart, TInt aEnd,
	TBool aRightToLeft)
	{
 	CFbsFont::TMeasureTextOutput output;
 	TInt width = AdvanceWidthL(aStart, aEnd, aRightToLeft, KMaxTInt, &output);
 	// Add on any protruding side-bearings
 	if (width < output.iBounds.iBr.iX)
 		width = output.iBounds.iBr.iX;
 	if (output.iBounds.iTl.iX < 0)
 		width -= output.iBounds.iTl.iX;
 	return width;
	}


/*
Get source text starting at aPos and not more than (aMaxEndChar - aPos) characters.
If aFormat is non-null get the format.
@param aFont If aFont is not null, on return contains a pointer to an opened CTmTextFontCache*
		The caller must call Close on aFont when finished with the font.
@return Failure condition from getting the font, if any.
*/
TInt RTmTextCache::GetText(TInt aPos,TInt aMaxEndChar,TPtrC& aText,TTmCharFormat* aFormat,CTmTextFontCache** aFont)
	{
	TInt error = KErrNone;
	if (aPos < iTextStart || aPos >= iTextStart + iTextLength)
		{
		iTextStart = aPos;
		iTextBuffer.Zero();
		TPtrC p;
		TTmCharFormat new_format;
		iSource.GetText(iTextStart,p,new_format);
		if (!(new_format.iFontSpec == iFormat.iFontSpec))
			{
			ReleaseFont();
			}
		iFormat = new_format;
		iText = p.Ptr();
		iTextLength = p.Length();
		if (iTextLength == 0)
			TmPanic(EZeroLengthTextSupplied);
		}
	int offset = aPos - iTextStart;
	aText.Set(iText + offset,iTextLength - offset);
	if (aText.Length() > aMaxEndChar - aPos)
		{
		const TText* p = aText.Ptr();
		aText.Set(p,aMaxEndChar - aPos);
		}
	if (aFormat)
		*aFormat = iFormat;
	if (aFont)
		{
		if (iFont == NULL)
			{
			TFontSpec fs;
			iFormat.iFontSpec.GetTFontSpec(fs);
			CFont *font = NULL;
			error = iDevice.GetNearestFontInTwips(font,fs);
			if (font != NULL)
				{
				iFont = CTmTextFontCache::New(iDevice, *font);
				if (iFont == NULL)
					{
					iDevice.ReleaseFont(font);
					error = KErrNoMemory;
					}
				}
			}
		if (iFont && error == KErrNone)
			{
			*aFont = iFont;
			iFont->Open();
			}
		}
	return error;
	}

/** Finds the extent of the run of characters that have the same format as
aFormat following aPos, up to aMaxEndChar.
@internalComponent */
TInt FollowOnTextEnd(TInt aPos, TInt aMaxEndChar,
	const TTmCharFormat& aFormat, MTmSource& aSource, TBool forcedExtend = EFalse)
	{
	TTmCharFormat format;
	TPtrC text;
	while (aPos < aMaxEndChar)
		{
		aSource.GetText(aPos, text, format);
		// The forcedExtend flag is used for only one case that to prevent single ZWJ being 
		// returned by RTmTextCache::GetTextL. Please see RTmTextCache::GetTextL.
		if (format != aFormat && !forcedExtend)
			return aPos;
		TInt length = text.Length();
		if (length == 0)
			return aPos;
		if (text[0] == CEditableText::EPictureCharacter)
			{
			return aPos;
			}
		aPos += length;
		}
	return aMaxEndChar;
	}

/** Copys the specified text from the source into the buffer.
@internalComponent */
void AppendTextToBuffer(TDes& aBuffer,
	TInt aStart, TInt aEnd, MTmSource& aSource)
	{
	TTmCharFormat dummy;
	while (aStart != aEnd)
		{
		TPtrC text;
		aSource.GetText(aStart, text, dummy);
		TInt textEnd = aStart + text.Length();
		if (aStart >= textEnd)
		    {
		    OstTrace0( TRACE_FATAL, DUP1__APPENDTEXTTOBUFFER, "EZeroLengthTextSupplied" );
		    }
		__ASSERT_ALWAYS(aStart < textEnd, TmPanic(EZeroLengthTextSupplied));
		if (aEnd < textEnd)
			{
			// This run is overlong, so just append all that will fit
			aBuffer.Append(&text[0], aEnd - aStart);
			return;
			}
		aBuffer.Append(text);
		aStart = textEnd;
		}
	}

/** Same as GetText but will join text together if it has the same format.
@param
	aText the text returned. Is valid until the next call of GetText, GetTextL
	or Close.
@param aFont If aFont is not null, on return contains a pointer to an opened CTmTextFontCache*
		The caller must call Close on aFont when finished with the font.
@internalComponent */
TInt RTmTextCache::GetTextL(TInt aPos, TInt aMaxEndChar, TPtrC& aText,
	TTmCharFormat* aFormat, CTmTextFontCache** aFont)
	{
	TInt bufferEnd = iTextBufferStart + iTextBuffer.Length();
	if ( iTextBufferStart <= aPos && aPos < bufferEnd
		&& (aMaxEndChar < bufferEnd || iTextBufferEndsInFormatChange) )
		{
		TInt textEnd = Min(bufferEnd, aMaxEndChar);
		aText.Set(&iTextBuffer[aPos - iTextBufferStart], textEnd - aPos);
		if (aFormat)
			*aFormat = iFormat;
		if (aFont)
			{
			*aFont = iFont;
			iFont->Open();
			}
			
		return KErrNone;
		}

	TInt error =  GetText(aPos, aMaxEndChar, aText, aFormat, aFont);
	TBool forcedExtend = EFalse;
	const TText* ch = aText.Ptr();
	// When the text is just a ZWJ, the following text should be forced extended, because the ZWJ should
	// never be returned as a single text chunk.
	if (aText.Length() == 1 && *ch == KZeroWidthJoiner)
		forcedExtend = ETrue;
	if (error == KErrNone)
		{
		CleanupClosePushL(**aFont);
		TInt endOfAText = aPos + aText.Length();

		TInt end = FollowOnTextEnd(endOfAText, aMaxEndChar, iFormat, iSource, forcedExtend);
		if (end != endOfAText)
			{
			iTextBuffer.Zero();
			if (iTextBuffer.MaxLength() < end - aPos)
				iTextBuffer.ReAllocL(end - aPos);

			iTextBuffer.Append(aText);
			AppendTextToBuffer(iTextBuffer, endOfAText, end, iSource);
			iTextBufferStart = aPos;
			iTextBufferEndsInFormatChange = end < aMaxEndChar;
			aText.Set(iTextBuffer);
			}
		CleanupStack::Pop(*aFont);
		}
	return error;
	}

TBool RTmTextCache::IsArabicPoint(TInt aChar)
	{
	return 0x64B <= aChar && aChar < 0x671
		&& !(0x656 <= aChar && aChar < 0x670);
	}

TUint SupplementaryCharMap(MTmSource *aSource, TText aHigh, TText aLow)
    {
    TChar c(aHigh);
    if ( IsHighSurrogate( aHigh ) && IsLowSurrogate( aLow ))
        c = JoinSurrogates( aHigh, aLow );
    return aSource->Map(c);
    }

/**
Gets all the displayed text in the range aStart...aEnd and puts it into a
buffer that must be at least aEnd - aStart + 2 characters in length. If
aFormat is non-null gets the format of the first section of text. If aFont
is non-null gets the font for the first section of text. If aDirectionality
is EVisualRightToLeft reverses the text and mirrors appropriate characters.
Adds a zero-width joiner to the start and/or end of the text returned if
these are necessary for the correct contextual glyph choice. Adds a
0xFFFF to each end if this is not required.
@param aFont If aFont is not null, on return contains a pointer to an opened CTmTextFontCache*
		The caller must call Close on aFont when finished with the font.
@return Failure condition on getting font, if any.
@internalComponent
*/
TInt RTmTextCache::GetDisplayedText(TInt aStart, TInt aEnd,
									TDisplayedTextDirectionality aDirectionality, TText* aBuffer,TUint aContextChar,
									TTmCharFormat* aFormat, CTmTextFontCache** aFont)
	{
	if (aStart > aEnd)
	    {
	    OstTrace0( TRACE_DUMP, RTMTEXTCACHE_GETDISPLAYEDTEXT, "EBadArg" );
	    }
	__ASSERT_DEBUG(aStart <= aEnd, TmPanic(EBadArg));
	if (aContextChar == 0)
		aBuffer[0] = 0xFFFF;
	else
		aBuffer[0] = aContextChar;
	if (aEnd == aStart)
		return KErrGeneral;
	TPtrC text;
	TText* output = aBuffer;
	TTmCharFormat format;

	// might the (logically) first character join with the previous one?
	GetText(aStart, aStart + 1, text, &format);	
    if (aFormat)
        *aFormat = format;
	TInt c = iSource.Map(text[0]);
	if (IsHighSurrogate(text[0]))
		{
		GetText(aStart, aStart + 2, text, &format);
		if (aFormat)
			*aFormat = format;
		if (text.Length() >= 2)
			{
			if (IsLowSurrogate(text[1]))
				{
				c = SupplementaryCharMap(&iSource, text[0], text[1]);
				}
			else
				{
				c = iSource.Map(text[0]);	
				}
			}
		}
	
	if (0 < aStart && CFont::CharactersJoin(
		aDirectionality == ELeftToRight? KZeroWidthJoiner : c,
		aDirectionality == ELeftToRight? c : KZeroWidthJoiner))
		{
		TInt prev = aStart - 1;
		TTmCharFormat prevFormat;
		GetText(prev, aStart, text, &prevFormat);
		TInt prevChar = iSource.Map(text[0]);
		while (0 < prev && IsArabicPoint(prevChar))
			{
			GetText(--prev, aStart, text, &prevFormat);
			prevChar = iSource.Map(text[0]);
			}
		if (CFont::CharactersJoin(
			aDirectionality == ELeftToRight? prevChar : c,
			aDirectionality == ELeftToRight? c : prevChar)
			&& format == prevFormat)
			// Characters join at the beginning.
			*output = KZeroWidthJoiner;
		}

	output = aBuffer + 1;
	TInt error = KErrNone;
	while (aStart < aEnd)
		{
		TInt currentError = GetText(aStart, aEnd, text, &format, aFont);
		// do not have to push aFont onto clean-up stack because this is a non-leaving method
		if (aFont)
			error = currentError;
		aFont = NULL;
		TInt length = text.Length();
		aStart += length;
		const TText* input = text.Ptr();
		const TText* end = input + length;
		while (input < end)
			{
			if (IsHighSurrogate(*input))
				{
				if (input+1 < end && IsLowSurrogate(input[1]))
					{
					c = SupplementaryCharMap(&iSource, input[0], input[1]);
					input++;
					input++;
					*output++ = GetHighSurrogate( c );
					*output++ = GetLowSurrogate( c );
					}
				else
					{
					c = iSource.Map(input[0]);
					input++;
					*output++ = static_cast<TText>(c);
					}
				}
			else if (IsLowSurrogate(*input))
				{
				c = iSource.Map(input[0]);
				input++;
				*output++ = static_cast<TText>(c);
				}
			else
				{
				c = iSource.Map(*input++);
				*output++ = static_cast<TText>(c);
				}
			}
		}

	*output = 0xFFFF;
	// might the (logically) last character join with the next one?
	if (aStart < iSource.DocumentLength())
		{
		const TText* prev = output;
		while (--prev != aBuffer && IsArabicPoint(*prev)) {}
		if (CFont::CharactersJoin(
			aDirectionality == ELeftToRight? *prev : KZeroWidthJoiner,
			aDirectionality == ELeftToRight? KZeroWidthJoiner : *prev))
			{
			TTmCharFormat nextFormat;
			GetText(aStart, aStart + 1, text, &nextFormat);
			TInt nextChar = iSource.Map(text[0]);
			if (CFont::CharactersJoin(
				aDirectionality == ELeftToRight? *prev : nextChar,
				aDirectionality == ELeftToRight? nextChar : *prev)
				&& format == nextFormat)
				// Characters join at the end.
				*output = KZeroWidthJoiner;
			}
		}

	if (aDirectionality == EVisualRightToLeft)
		TBidirectionalState::ReverseGroups(aBuffer, output + 1 - aBuffer);
	
	return error;
	}

void RTmTextCache::SetContextChar(TUint aContextChar)
	{
	iContextCharPerChunk = aContextChar;
	}

