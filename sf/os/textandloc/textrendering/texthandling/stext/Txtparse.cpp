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


#include "TXTRICH.H"
#include "TXTSTD.H"
#include "ParseLst.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TxtparseTraces.h"
#endif



// Install and activate a particular parser, app provides instance
EXPORT_C void CRichText::ActivateParserL(MParser* aParser)
	{
	CParserList* activeParserList = (CParserList*)Dll::Tls();
	if (!activeParserList)
		{
		CreateParserETextTLSL();
		activeParserList = (CParserList*)Dll::Tls();
		}
	activeParserList->ActivateParserL(aParser);
	}


// Deactivate and deinstall a particular parser, identified by ptr to instance
EXPORT_C void CRichText::DeactivateParser(MParser* aParser)
	{
	CParserList* activeParserList = (CParserList*)Dll::Tls();
	if (!activeParserList)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXT_DEACTIVATEPARSER, "EParserListNotInitialized" );
	    }
	__ASSERT_DEBUG(activeParserList, Panic(EParserListNotInitialized));
	activeParserList->DeactivateParser(aParser);
	if ((activeParserList->iRefCount == 0) && (activeParserList->iNumberInList == 0))
		{
		Dll::FreeTls();
		delete activeParserList;
		}
	}


// Install and activate a parser in the default set
EXPORT_C void CRichText::ActivateDefaultParserL(MParser* aParser)
	{
	CParserList* activeParserList = (CParserList*)Dll::Tls();
	if (!activeParserList)
		{
		CreateParserETextTLSL();
		activeParserList = (CParserList*)Dll::Tls();
		}
	activeParserList->ActivateDefaultParserL(aParser);
	}


// Deactivate and deinstall the standard set of default parsers
EXPORT_C void CRichText::DeactivateParserDefaults()
	{
	CParserList* activeParserList = (CParserList*)Dll::Tls();
	if (activeParserList)
		{
		activeParserList->DeactivateParserDefaults();
		if ((activeParserList->iRefCount == 0) && (activeParserList->iNumberInList == 0))
			{
			Dll::FreeTls();
			delete activeParserList;
			}
		}
	}


// Create ParserLst instance and retain ownership of it but pass address to EText TLS
void CRichText::CreateParserETextTLSL()
	{
	if (Dll::Tls() != NULL)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXT_CREATEPARSERETEXTTLSL, "EParserListAlreadyExists" );
	    }
	__ASSERT_DEBUG(Dll::Tls() == NULL, Panic(EParserListAlreadyExists));
	CParserList* activeParserList = new (ELeave) CParserList;
	CleanupStack::PushL(activeParserList);
	TInt err = Dll::SetTls(activeParserList);
	User::LeaveIfError(err);
	CleanupStack::Pop(activeParserList);
	}


// Set observer callback to tell whenever the object has been edited.
// If set set then the parser system is active for this instance, otherwise not.

 

EXPORT_C void CRichText::SetEditObserver(MEditObserver* aEditObserver)
/** Sets the rich text object's edit observer. The observer's EditObserver() function 
is called by the rich text object each time the object's text content is edited 
(e.g. after a paste, insert, delete, reset etc.).

@param aEditObserver Pointer to the edit observer. */
	{
	iParserData->iEditObserver = aEditObserver;
	}

	
EXPORT_C TBool CRichText::ParseText(TInt& aStartOfTags, TInt& aLength, TBool aForceScanAllText)
	{
	if (!iIndex.IsPtr())
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXT_PARSETEXT, "EParserListTextIndexNotInitialized" );
	    }
	__ASSERT_ALWAYS(iIndex.IsPtr(),Panic(EParserListTextIndexNotInitialized));
	TBool foundSomething = EFalse;
	if (iParserData->iActiveParserList && iParserData->iEditObserver)
		{
		if (aForceScanAllText)
			foundSomething = iParserData->iActiveParserList->ParseThisText(*this,0,DocumentLength(),
																		   aStartOfTags,aLength);
		else if (iParserData->HaveRange())
			foundSomething = iParserData->iActiveParserList->ParseThisText(*this,iParserData->StartParse(),
									iParserData->EndParse() - iParserData->StartParse(),
									aStartOfTags,aLength);
		// All parsers have scanned the area
		iParserData->KillRange();
		}
	return foundSomething;
	}


// Given a cursor position, is there a tag under it and, if so, give me details	
TBool CRichText::DoCursorOverTag(TInt aPos, MParser*& aParser, TInt& aTagStart, TInt& aLength) const
	{
	TCharFormatX format;
	TCharFormatXMask varies;
	TBool success = EFalse;
	TBuf<1> buf;

	if (!iParserData->iActiveParserList)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXT_DOCURSOROVERTAG, "EParserListNotInitialized" );
	    }
	__ASSERT_DEBUG(iParserData->iActiveParserList, Panic(EParserListNotInitialized));
	if (!iParserData->iEditObserver)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXT_DOCURSOROVERTAG, "EParserListNotActive" );
	    }
	__ASSERT_DEBUG(iParserData->iEditObserver, Panic(EParserListNotActive));
	GetExtendedCharFormat(format, varies, aPos, 1);
	Extract(buf, aPos, 1);
	if ((format.iParserTag) && (buf[0] != 0x2029))
		{
		aParser = iParserData->iActiveParserList->ParserWithThisTag(format.iParserTag);
		if (aParser == NULL)
			{ // Parser has been deactivated
			return EFalse;
			}
		// Get extent of tag (or set of contiguous tags for same parser)
		TInt pos = aPos;
		TInt startScan;
		TInt scanLength;
		// need to check backwards
		TUint tag = format.iParserTag;	// Stash tag before overwriting it
		for (; pos > 0; pos--)
			{
			GetExtendedCharFormat(format, varies, pos - 1, 1);
			Extract(buf, aPos, 1);
			if ((format.iParserTag != tag) || (buf[0] == 0x2029))
				break;
			}
		startScan = pos;
		// Now forwards
		TInt len = DocumentLength();
		while (pos < len)
			{
			TPtrC ptr;
			GetTextAndExtendedFormat(ptr, format, pos);
			if (format.iParserTag != tag)
				break;
			pos += ptr.Length();
			}
		if (pos > len)
			pos = len;
		scanLength = pos - startScan;
		// Now use the parser that found it originally to isolate the exact range
		// of the tag from the range that could contain several
		for (;;)
			{
			TInt result = aParser->ParseThisText(*this, EFalse, startScan, scanLength, aTagStart, aLength);
			// Check we haven't gone past it (failed to find it this time)
			if (!result || (aTagStart > aPos))
				break;
			if ((aPos >= aTagStart) && (aPos < aTagStart + aLength))
				{
				// We've found it
				success = ETrue;
				break;
				}
			// Not yet, skip over that one
			startScan += aLength;
			scanLength -= aLength;
			if (scanLength < 0)
				break;
			}
	
		}
	return success;
	}


EXPORT_C TBool CRichText::CursorOverTag(TInt aPos, MParser*& aParser, TInt& aTagStart, TInt& aLength) const
	{
	TBool over = EFalse;

	if (iParserData->iActiveParserList && iParserData->iEditObserver)
		{
		iParserData->iLastKnownCursor = aPos;
		if (DoCursorOverTag(aPos, aParser, aTagStart, aLength))
			{
			over = ETrue;
			}
		if (aPos && DoCursorOverTag(aPos - 1, aParser, aTagStart, aLength))
			{
			over = ETrue;
			}
		}
	return over && 
		aParser->ConfirmCursorOverTag(*this, aTagStart, aLength, aPos);
	}


EXPORT_C TInt CRichText::PositionOfNextTag(TInt aPos, const MParser * aParser) const
	{
	if (iParserData->iActiveParserList && iParserData->iEditObserver)
		{
		MParser* parser;
		TInt tagStart;
		TInt length;
		TInt newPos = aPos;
		TUint tag = 0;
		if (aParser)
			tag = iParserData->iActiveParserList->TagForThisParser(aParser);
		if (DoCursorOverTag(newPos, parser, tagStart, length))
			newPos = tagStart + length;	// To get past the current tag
		TPtrC ptr;
		TCharFormatX format;
		while (newPos < DocumentLength())
			{
			GetTextAndExtendedFormat(ptr, format, newPos);
			if (format.iParserTag &&(!aParser || format.iParserTag == tag))
				return newPos;
			newPos += ptr.Length();
			}
		}

	return KErrNotFound;
	}


EXPORT_C TInt CRichText::PositionOfNextTag(TInt aPos) const
	{
	return PositionOfNextTag(aPos, NULL);
	}


EXPORT_C TInt CRichText::PositionOfPrevTag(TInt aPos, const MParser * aParser) const
	{
	if (iParserData->iActiveParserList && iParserData->iEditObserver)
		{
		MParser* parser;
		TInt tagStart;
		TInt length;
		TInt newPos = aPos;
		TUint tag = 0;
		if (aParser)
			tag = iParserData->iActiveParserList->TagForThisParser(aParser);
		if (DoCursorOverTag(newPos, parser, tagStart, length))
			newPos = tagStart;	// To get past the current tag
		TCharFormatX format;
		TCharFormatXMask varies;
		
		for (; newPos > 0; newPos--)
			{
			GetExtendedCharFormat(format, varies, newPos - 1, 1);
			if (format.iParserTag &&(!aParser || format.iParserTag == tag))
				{
				if (DoCursorOverTag(newPos - 1, parser, tagStart, length))
					return tagStart;
				}
			}
		}

	return KErrNotFound;
	}


EXPORT_C TInt CRichText::PositionOfPrevTag(TInt aPos) const
	{
	return PositionOfPrevTag(aPos, NULL);
	}


void CRichText::OverrideFormatForParsersIfApplicable(TPtrC& aText, TCharFormatX& aFormat, TInt aStartPos) const
	{
	if (aFormat.iParserTag && iParserData->iActiveParserList && iParserData->iEditObserver)
		{
		// Replace format
		TInt start = -1;
		TInt length = 0;
		TInt curPos = iParserData->iLastKnownCursor;
		if (curPos > DocumentLength())
			curPos = DocumentLength();  // This shouldn't be neccesary but it makes it more
										// bulletproof if the calls from outside are made in
										// the wrong order
		if (curPos != -1)
			{
			TCharFormatX format;
			TCharFormatXMask varies;
			MParser* parser;

			GetExtendedCharFormat(format, varies, curPos, 1);
			// If char at curpos has a tag then cursor is over that tag, get extents of tag
			if (CParserList::ReformatOnRollover(format.iParserTag))
				DoCursorOverTag(curPos, parser, start, length);
			else if (curPos)
				{
				GetExtendedCharFormat(format, varies, curPos - 1, 1);
				// Try the char "before" curpos
				if (CParserList::ReformatOnRollover(format.iParserTag))
					DoCursorOverTag(curPos - 1, parser, start, length);
				}
			}

		MParser* parser = iParserData->iActiveParserList->ParserWithThisTag(aFormat.iParserTag);
		
		if (length && (aStartPos >= start) && (aStartPos < start + length))
			{
			if (start + length < aStartPos + aText.Length())
				aText.Set(aText.Left(start + length - aStartPos));
			if (parser != NULL)
				{
				// Only accept the rollover format if the parser agrees 
				// with the framework match of a tag
				if (parser->ConfirmCursorOverTag(*this, start, length, curPos))
					parser->GetRolloverFormat(aFormat.iCharFormat);
				else 
					// Reset format to recognised format if parser disagrees
					// with the framework match as the tag is still in view
					// and must be formatted as in the else clause below.
					parser->GetRecogniseFormat(aFormat.iCharFormat);
				}
			}
		else
			{
			if (length && (start > aStartPos))
				aText.Set(aText.Left(start - aStartPos));
			if (parser != NULL)
				parser->GetRecogniseFormat(aFormat.iCharFormat);
			}
		}
	}


void CRichText::CallEditObserver(TInt aStart, TInt aExtent) const
	{
	if (iParserData->iEditObserver)
		iParserData->iEditObserver->EditObserver(aStart, aExtent);
	}
