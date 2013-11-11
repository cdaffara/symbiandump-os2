/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TxtWriter.h"
#include <txtetext.h>
#include "TXTPLAIN.H"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TxtWriterTraces.h"
#endif


static const TText KLineBreak = 0x0A;//Used by TSLBTransaltor class - 0x0A, 0x0D, {0x0D, 0x0A}
                              //character aequences found in the input stream will be translated 
                              //to KLineBreak character.

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// TSLBTransaltor class

/**
The method processes the input characters and translates 0x0D, 0x0A and 0x0D 0x0A characters
to line breaks. When the next output character is ready, the method calls WriteL() method of
the controlled MTextWriter interface (iTextWriter data member) with the output character 
as an argument. 
Note: Identified line breaks will be translated to 0x0A character. 0x0A character code is very
      appropriate for use because all 0x0A, 0x0D or {0x0D, 0x0A} character sequences are filtered
	  from the input stream and it is guaranteed that the output stream can't have any of them.
Note: The output character stream should not contain 0x0D character.
@param aChar Input character to be processed.
*/
void TSLBTransaltor::ProcessL(TText aChar)
	{
	if(aChar == 0x0A)
		{
		//Output a line break. It does not matter what is the previous character.
		//If it is 0x0D - line break should be the output (0x0D 0x0A). If it is something else - 
		//(i\x0A) again the output is a line break.
		iTextWriter.WriteL(KLineBreak);
		}
	else
		{
		if(iPrevChar == 0x0D)
			{
			//Previous character is 0x0D and current character is not 0x0A - then we have to output
			//a line break - the previous character was stored one call before and now has to be 
			//processed, if there is no 0x0A character.
			iTextWriter.WriteL(KLineBreak);
			}
		if(aChar != 0x0D)
			{
			//If current character is 0x0D, then it will be stored for further processing (in 
			//case if the next character is 0x0A). If current character is not 0x0D and not 
			//0x0A - then output it immediately.
			iTextWriter.WriteL(aChar);
			}
		}
	iPrevChar = aChar;
	}

/**
The method immediately sends to the output any characters, left for further processing. 
The method shoud be called by the TSLBTransaltor's client after the processing of all 
input characters.
*/
void TSLBTransaltor::FlushL()
	{
	if(iPrevChar == 0x0D)
		{
		//The last input character is 0x0D and there won't be any more characters, so there is
		//not a chanse to find a matching 0x0A character. Output it as a single line break.
		iTextWriter.WriteL(KLineBreak);
		}
	//iTextWriter has an internal state too. Flush it.
	iTextWriter.FlushL();
	iPrevChar = 0;
	}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// TParagraphTextWriter class

/**
This method should be called only from TSLBTransaltor implementation. It gets the characters 
processed by TSLBTransaltor instance as an input and writes them to the output (using MOutputChar
interface - iOutputChar data member), translating line breaks to paragraph delimiters.
@param aChar Input character to be processed. It can't be 0x0D, but it could be 0x0A - identified
             line break.
*/
void TParagraphTextWriter::WriteL(TText aChar)
	{
	if (aChar == 0x0D)
	    {
	    OstTrace0( TRACE_DUMP, TPARAGRAPHTEXTWRITER_WRITEL, "Invariant" );
	    }
	__ASSERT_DEBUG(aChar != 0x0D, User::Invariant());
	if(aChar == KLineBreak)
		{
		iOutputChar.OutputCharL(CEditableText::EParagraphDelimiter);
		}
	else
		{
		iOutputChar.OutputCharL(aChar);
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// TLineTextWriter class

/**
This method should be called only from TSLBTransaltor implementation. It gets the characters 
processed by TSLBTransaltor instance as an input and writes them to the output (using MOutputChar
interface - iOutputChar data member), translating line breaks to paragraph delimiters or spaces.
The translation rules are:
 - single line break - space;
 - double line break - paragraph delimiter;
@param aChar Input character to be processed. It can't be 0x0D, but it could be 0x0A - identified
             line break.
*/
void TLineTextWriter::WriteL(TText aChar)
	{
	if (aChar == 0x0D)
	    {
	    OstTrace0( TRACE_DUMP, TLINETEXTWRITER_WRITEL, "Invariant" );
	    }
	__ASSERT_DEBUG(aChar != 0x0D, User::Invariant());
	TText prevChar = iPrevChar;
	iPrevChar = aChar;
	if(aChar != KLineBreak)
		{
		if(prevChar == KLineBreak)
			{
			//Current character is not a line break, but the previous one is.
			//Then, it is a single line break - output a space.
			iOutputChar.OutputCharL(' ');
			}
		//Current character is not a line break - then just output it.
		iOutputChar.OutputCharL(aChar);
		}
	else
		{
		if(prevChar == KLineBreak)
			{
			//Current character is a line break, previous character is a line break too.
			//Double line break - output a paragraph delimiter.
			//Both characters are consumed, so iPrevChar is set to 0.
			iPrevChar = 0;
			iOutputChar.OutputCharL(CEditableText::EParagraphDelimiter);
			}
		}
	}

/**
The method immediately sends to the output any characters, left for further processing. 
This method should be called only from TSLBTransaltor implementation.
*/
void TLineTextWriter::FlushL()
	{
	if(iPrevChar == KLineBreak)
		{
		//There is no more input characters and the last input charactes is a line break.
		//Then, treat it as a single line break and output a space.
		iOutputChar.OutputCharL(' ');
		}
	iPrevChar = 0;
	}
