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
*
*/


#include <mparser.h>


// Given the text object, scan the specified range of the text buffer
// (from aStartScan to one before aEndScan) and show the first match, if
// any, that you would like tagged by setting the range (from aStartTag
// to one before aEndTag). Return ETrue if you find anything, EFalse
// otherwise. This function must be defined. If aAllowBack is ETrue then
// the parser is allowed to recognise tags that start before but
// continue into the range specified, otherwise it is not allowed to do
// so (and, if it does, will assert in debug mode).
// virtual TBool ParseThisText(const CRichText& aTextObj,
//							   TBool aAllowBack,
//							   TInt aStartScan,
//							   TInt aScanLength,
//							   TInt& aStartTag,
//							   TInt& aTagLength) = 0;


// The text cursor is over the text string in aTextObj (aLength
// characters starting at aStartText). Return a const ptr to the string
// that should be displayed for a "Do It!" button. (This can be
// constant, such as a _LIT, or change according to the tagged text.)
// The parser retains ownership of this string and, if necessary, should
// delete it when this function is next called or when the parser is
// deleted. This function will normally be invoked directly by UIKON and
// must be defined.
// virtual const TDesC& CreateDoItText(const CRichText& aTextObj,
//									   TInt aStartText,
//									   TInt aLength) = 0;


// The text cursor is over the text string in aTextObj (aLength
// characters starting at aStartText) and the "Do It!" button has been
// pushed (or some other activation method has been triggered). Do
// whatever should happen (such as launching a browser with the
// indicated URL). If the parser requires a copy of the text to pass on,
// it is up to it to take one before doing anything (such as handing it
// to a separate thread) and to dispose of it later. This function will
// normally be invoked directly by UIKON and must be defined.
// virtual void ActivateThisTextL(const CRichText& aTextObj,
//							      TInt aStartText,
//							      TInt aLength) = 0;


// Whether this parser makes any formatting changes to text that is
// recognised and tagged as soon as it has been recognised. If it does
// then this function should be overridden with one that returns ETrue
// and the GetRecogniseFormat() function should be overridden
// appropriately - otherwise do nothing.
EXPORT_C TBool MParser::ReformatOnRecognise() const
	{
	return EFalse;
	}


// Whether this parser makes any formatting changes to text that is
// recognised and tagged when the text cursor moves onto it. If it does
// then this function should be overridden with one that returns ETrue
// and the GetRolloverFormat() function should be overridden
// appropriately otherwise do nothing.
EXPORT_C TBool MParser::ReformatOnRollover() const
	{
	return EFalse;
	}


// Given an existing format, modify it to reflect the highlighting
// changes that apply immediately some text is recognised and tagged by
// this parser. If this parser does not make any highlighting changes at
// recognise time then this function should not be overridden.
EXPORT_C void MParser::GetRecogniseFormat(TCharFormat& /*aFormat*/)
	{
	}


// Given an existing format, modify it to reflect the highlighting 
// changes that apply when the text cursor moves onto text that has been
// recognised and tagged by this parser. This should include all changes
// that you wish to make to the base format as this will *not* be
// applied cumulatively to the recognise highlighting. If this parser
// does not make any highlighting changes at rollover time then this
// function should not be overridden and any recognise highlighting will
// remain.
EXPORT_C void MParser::GetRolloverFormat(TCharFormat& aFormat)
	{
	GetRecogniseFormat(aFormat);
	}


EXPORT_C TBool MParser::ConfirmCursorOverTag(const CRichText& /*aTextObj*/,
	TInt /*aTagStart*/, TInt /*aTagLen*/, TInt /*aCurPos*/)
/**
 This confirmation routine is called by the parsing framework when
 it's logic determines the cursor is over a tag. It gives the parser 
 a chance to override the default cursor-tag rollover matching. This
 is not necessary if the default behaviour is acceptable.
 It is called from the CRichText::CursorOverTag()
 when the framework tests for format redraw when over a tag and 
 CRichText::OverrideFormatForParsersIfApplicable() when Tagma
 is formatting text.
@publishedPartner
@released
@param aTextObj
 A reference to the text document tag is within.
@param aTagStart
 The document position of the first tag character.
@param aTagLen
 The number of characters in the tag.
@param aCurPos
 The cursor position used in calculation of calling routines.
@return TBool
 ETrue if parser agrees cursor over a tag, EFalse otherwise.
*/
	{
	return ETrue;
	}


EXPORT_C void MParser::MParser_Reserved_2() {}
