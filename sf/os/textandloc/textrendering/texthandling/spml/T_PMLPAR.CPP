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


#include "../incp/T_PMLPAR.H"

#define UNUSED_VAR(a) a = a

#define UNUSED_VAR(a) a = a

////////////////////////////////////////////
// CParser
////////////////////////////////////////////

CParser* CParser::NewL()
	{
	CParser* self=new(ELeave) CParser;
	CleanupStack::PushL(self);
	self->ConstructApplicationL();
	CleanupStack::Pop();
	return self;
	}


CParser::CParser()
	{
	// init variables
	iErrorLevel = ENoError;			
	iParagraphIsOpen = EFalse;	
	iLineNo = 1;
	iReadPos = 0;
	iDocInsertPos = 0;
	iDocParaLength = 0;
	iDocPhraseLength = 0;
	iBorderUsed = EFalse;
	iBulletUsed = EFalse;
	}
	 

void CParser::ConstructApplicationL()
	{ 
	// Construct Rich Text Doc
	// Make the global format layers and associated formats and masks
	iGlobalParaFormatLayer=CParaFormatLayer::NewL();
	iGlobalCharFormatLayer=CCharFormatLayer::NewL();
	iGlobalParaFormat=CParaFormat::NewL();	   	// initialised with factory settings
	iGlobalParaFormatMask.SetAll();
	iGlobalCharFormatMask.SetAll();
	
	// Set the global layers
	iGlobalParaFormatLayer->SetL(iGlobalParaFormat,iGlobalParaFormatMask);
	iGlobalCharFormatLayer->SetL(iGlobalCharFormat, iGlobalCharFormatMask);

	// Create the rich text document
	iRichTextDoc=CRichText::NewL(iGlobalParaFormatLayer,iGlobalCharFormatLayer);

	// Initialise the paragraph and character layers
	iParaFormatLayer = CParaFormatLayer::NewL();
	iCharFormatLayer = CCharFormatLayer::NewL();
	iParaFormat = CParaFormat::NewL();

	// Create temp alias' for compound attributes
	iBorder = new TParaBorder;
	User::LeaveIfNull(iBorder);
	iBullet = new (ELeave) TBullet;
	}


CParser::~CParser()
	{
	// destroy Rich Text Document
// 	delete iRichTextDoc;
//	delete iGlobalParaFormatLayer;
//	delete iGlobalCharFormatLayer;
	delete iGlobalParaFormat;
	delete iParaFormatLayer;
	delete iCharFormatLayer;
	delete iParaFormat;
	if (!iBorderUsed)
		delete(iBorder);
	if (!iBulletUsed)
		delete(iBullet);
	}


CRichText* CParser::ParseL(CConsoleBase* aConsole)
// version for parsing with console output and interactive file dialog
// primarily for debugging purposes (you can see what's going on!)
	{
	// set console
	iConsoleExists = ETrue;
	iConsole = aConsole;

	// Construct a CFileApp & load a file
	CFileApp* myFileApp=NULL;
	TRAPD(ret, myFileApp = CFileApp::NewL());
    UNUSED_VAR(ret);
	iTextBuf = myFileApp->LoadFileL(iConsole);
	iFileName = myFileApp->ReturnFileName();

	ParseTextBufL();	   			// parse the buffer
	delete myFileApp;	// destroy file app

	// Pause before returning
	WriteNewLine();
	WriteNewLine();
	OutputToScreen(_L("Press Space to continue\n"));
	TKeyCode keystroke = EKeyNull;
	while (keystroke != EKeySpace)
		keystroke = iConsole->Getch();
	return iRichTextDoc;
	}

CRichText* CParser::ParseL(const TFileName &aFileName)
// silent version of the parser
	{
	iConsoleExists = EFalse;
	// Construct a CFileApp & load a file
	CFileApp* myFileApp=NULL;
	TRAPD(ret, myFileApp = CFileApp::NewL());
    UNUSED_VAR(ret);
	iTextBuf = myFileApp->LoadFileL(aFileName);
	iFileName = myFileApp->ReturnFileName();

	ParseTextBufL();	   			// parse the buffer
	delete myFileApp;	// destroy file app

	return iRichTextDoc;
	}


void CParser::EmitErrorMessage()
	{
	TBuf<80> errorMessage;
	switch (iErrorLevel)
		{
		case EUnknownTagType:
			errorMessage.Format(_L("    Unknown tag type: Line %d"),iLineNo);
			break;
		case EUnparagraphedText:
			errorMessage.Format(_L("    Text not contained by paragraph: Line %d"),iLineNo);
			break;
		case EUnknownAttrib:
			errorMessage.Format(_L("    Unknown tag attribute: Line %d"),iLineNo);
			break;
		case ENoAttribValue:
			errorMessage.Format(_L("    Unknown attribute or no attribute value supplied: Line %d"),iLineNo);
			break;
		case EIllegalAttribValue:
			errorMessage.Format(_L("    Illegal attribute value: Line %d"),iLineNo);
			break;
		default:
			errorMessage.Format(_L("    Error: Line %d"),iLineNo);
			break;
		}
	OutputToScreen(_L(""));
	OutputToScreen(_L("*** Error!!\n"));
	OutputToScreen(errorMessage);
	}


void CParser::OutputToScreen(const TDesC& aMessageBuffer)
	{
	if (iConsoleExists)
		iConsole->Write(aMessageBuffer);  // output line to screen
	}


TBool CParser::Validate()
// Check that document starts with <G> tag - serves as file validation
//  - Read in characters sequentially
//  - if the first alphanumeric characters encountered are not "<G" then error
	{
	TBool fileFormatError = EFalse;
	TBool fileValidated = EFalse;
	TChar charToTest;
	while ((!fileFormatError)&&(!fileValidated))
		{					
		charToTest = ReadChar();
		if (charToTest == '<')
			{
			iReadPos+=KCharLength;
			charToTest = ReadTagChar();
			if (charToTest != 'G')			 // Not a style tag - error
				fileFormatError = ETrue;
			else
				fileValidated = ETrue;
			}
		else
			{
			if (charToTest.IsAlphaDigit())	 // Char is alphanumeric - error
				fileFormatError = ETrue;	 // (File must start with style tag)
			else
				iReadPos+=KCharLength;
			}
		}
	if (fileFormatError)
		{
		OutputToScreen(_L("File format error\n"));
		}
	iReadPos = 0;		// reset after validation
	iLineNo = 1;		// ...
	return fileValidated;
	}


void CParser::ParseTextBufL()
// Parses contents of iTextBuf
// Reads in chars sequentially.
// - If a char is a tag open ("<") process the following tag
// - otherwise add the char to the text of the rich text document
// Tidy up at the end of the document 
	{ 
	TChar charToTest;
	TUint textBufSize = iTextBuf->Size();
	if (Validate())
		{
		while ((iReadPos < textBufSize)&&(iErrorLevel == ENoError))
			{
			charToTest = ReadChar();
			if (charToTest == '<')
				ProcessTagL();
			else
				ProcessTextL(charToTest);
			iReadPos+=KCharLength;						
			}
		if ((iReadPos == textBufSize)&&(iReadPos>0)&&(iErrorLevel == ENoError))
			{
			// at end of document apply any outstanding formatting (if there is text to apply it to)
			if (iDocParaLength > 0)
				{
				iParaFormatLayer->SetL(iParaFormat, iParaFormatMask);
				iRichTextDoc->ApplyParaFormatL( iParaFormat,iParaFormatMask,iDocInsertPos-iDocParaLength,iDocParaLength);
				}
			if (iDocPhraseLength > 0)
				{
				iCharFormatLayer->SetL(iCharFormat, iCharFormatMask);
				iRichTextDoc->ApplyCharFormatL(iCharFormat,iCharFormatMask,iDocInsertPos-iDocPhraseLength,iDocPhraseLength);
				}
			}
		if (iErrorLevel != ENoError)
			EmitErrorMessage();
		}
	}


TChar CParser::ReadChar()
// Reads the next character from the text buffer
	{
// 	TChar charToTest;
	TText charToTest;
	iTextBuf->Read(iReadPos,&charToTest,KCharLength);
	if (charToTest == (TText)KLineFeed)
		iLineNo++;				   	// Info used only by error messages
	return charToTest;
	}


void CParser::ProcessTextL(TChar aChar)
// 1) Check text is in a paragraph
// 2) Check for escape characters
// 3) Check for tabs
// 4) Add to paragraph
	{
	if (!iParagraphIsOpen)
		{
		if (!(aChar.IsControl()))
			iErrorLevel = EUnparagraphedText;	// Text not contained by a paragraph - error!!
		}
	else
		{
		if (aChar == '/')	// Escape character for <
			{
			TChar tempChar;
			iTextBuf->Read(iReadPos+KCharLength,&tempChar,1);	// doesn't increment line counter
			if (tempChar == '<')
				{
				AddCharToParaL(tempChar);
				iReadPos+=2*KCharLength;				  	// Skip escape character
				}
			else
				AddCharToParaL(aChar);
			}
		else if (aChar == KTabChar)
			AddCharToParaL(aChar);
		else if (!(aChar.IsControl()))		// if it's not a control char add it 
			AddCharToParaL(aChar);			//		(this includes spaces)
		}
	}


void CParser::AddCharToParaL(TChar aChar)
	{ 
	// Add char to RichText doc...
	iRichTextDoc->InsertL(iDocInsertPos, aChar);
	iDocInsertPos++;
	iDocParaLength++;
	if (iPhraseOpen)
		iDocPhraseLength++;
	// and also write it to screen
	TBuf<4> screenBuf;				// Buffer for text to be written to console
	screenBuf.Append(aChar);		
	OutputToScreen(screenBuf);
	}


void CParser::ProcessTagL()
	{
	TChar tagChar;
	iReadPos+=KCharLength;
	tagChar = ReadTagChar();	  	// Read in tag type
	ClassifyTagL(tagChar);		  	
	if (iTagType == EError)
		iErrorLevel = EUnknownTagType;
	else
		{
		ClassifyArgumentsL();
		}
	}


TChar CParser::ReadTagChar()
	{ // Returns tag character capitalised - therefore case-insensitive
 	TChar charToTest;
	charToTest = ReadChar();
	charToTest.UpperCase();
	return charToTest;
	}


void CParser::ClassifyArgumentsL()
// reads tag one argument at a time, dealing with each arg as it is read
// If and argument is followed by a value (ie a=4) it is processed in two passes.
	{
	TChar tagChar(0);
	iArgStored = EFalse;
	iArgValueExpected = EFalse;		// Initialise
	iCancelArg = EFalse;
	while ((tagChar != '>')&&(iErrorLevel == ENoError))	// ">" is end of tag
		{
		iReadPos+=KCharLength;
		tagChar = ReadTagChar();	// Read in next bit of tag
		if (iTagType != EComment)	// text of comments is ignored
			{
			if (tagChar.IsSpace())	// spaces separate args
				ProcessArgBufL();
			if (tagChar == '=')
				{
				iArgValueExpected = ETrue;
				ProcessArgBufL();
				}
			if (tagChar == '!')
				{
				iCancelArg = ETrue;
				OutputToScreen(_L("!"));
				}
			if (tagChar == ',')
				AppendToArgBuf(tagChar);
			if (tagChar.IsAlphaDigit())
				{
				AppendToArgBuf(tagChar);
				}
			}
		}
	if (tagChar == '>')				// Is it end of tag?
		{
		if (iTagType != EComment)
			{
			ProcessArgBufL();
			if ((iTagType == EControl)||(iTagType == EGlobal))	// Control & global style formatting is applied "on the spot"
				SetFormatLayerL();								// While char & paragraph are applied retrospectively
			}
		}
	}


void CParser::AppendToArgBuf(TChar aTagChar)
	{
	iArgType.Append(aTagChar);	   	// assume it'll fit
	}


void CParser::ProcessArgBufL()
	{
	if (iArgValueExpected)
		{
		if (iArgStored)
			{
			TBuf<32> tempArgBuf;				// Swap the buffers	back as an arg and its value have been stored
			tempArgBuf = iArgType;
			EmptyBuffer(iArgType);
			iArgType = iArgValue;
			EmptyBuffer(iArgValue);
			iArgValue = tempArgBuf;
			TranslateTagArgL();				// Translate the tag argument
			iArgStored = EFalse;			// Reset the flags and buffers
			iArgValueExpected = EFalse;
			EmptyBuffer(iArgType);
			EmptyBuffer(iArgValue);
			}
		else
			{
			iArgValue = iArgType;		 	// Swap the buffers ready to store the value of the arg
			EmptyBuffer(iArgType);			// Empty buffer
			iArgStored = ETrue;
			}
		}
	else
		{
		TranslateTagArgL();			// match to list
		EmptyBuffer(iArgType);
		EmptyBuffer(iArgValue);
		}
	}


void CParser::EmptyBuffer(TDes &aBuf)
	{
	aBuf.SetLength(0);
	}

	 
void CParser::ClassifyTagL(TChar aTagChar)
	{
	switch (aTagChar)
		{
		case 'G':
			iTagType = EGlobal;
			break;
		case 'P':
			{
			iTagType = EParagraph;
			if (iParagraphIsOpen)
				{
				iRichTextDoc->InsertL(iDocInsertPos,CEditableText::EParagraphDelimiter);  // insert para delimiter
				WriteNewLine();				// Write new line to console
				iDocInsertPos++;			// Paragraph delimiters occupy 1 character space
				iDocParaLength++;
				if (iPhraseOpen)
					iDocPhraseLength++;
				SetFormatLayerL();			// apply formatting to old para before starting to fill new one
				}
			else
				iParagraphIsOpen = ETrue;
			break;
			}
		case 'C':
			{
			iTagType = ECharacter;
			if (iPhraseOpen)                                                      
				SetFormatLayerL();	// apply formatting to old phrase retrospectively 			    
			else 
				iPhraseOpen = ETrue;		
			break;
			}
		case 'X':
			iTagType = EControl;
			break;
		case '!':
			iTagType = EComment;
			break;
		default:
			iTagType = EError;
			break;
		}
	}


void CParser::SetFormatLayerL()
// Apply format & mask that have been set in the tag to a Layer
// Apply the layer to the RichText doc
	{
	if (iTagType == EGlobal)
		{
		iGlobalParaFormatLayer->SetL(iGlobalParaFormat, iGlobalParaFormatMask);
		iGlobalCharFormatLayer->SetL(iGlobalCharFormat, iGlobalCharFormatMask);
		iRichTextDoc->SetGlobalParaFormat(iGlobalParaFormatLayer);
		iRichTextDoc->SetGlobalCharFormat(iGlobalCharFormatLayer);
		WriteNewLine();
		}
	if (iTagType == EParagraph)
		{
		iParaFormatLayer->SetL(iParaFormat, iParaFormatMask);
		iRichTextDoc->ApplyParaFormatL(iParaFormat,iParaFormatMask,iDocInsertPos-iDocParaLength,iDocParaLength);
		iCharFormatLayer->SetL(iCharFormat, iCharFormatMask);
		if (iDocPhraseLength > 0)
			iRichTextDoc->ApplyCharFormatL(iCharFormat,iCharFormatMask,iDocInsertPos-iDocPhraseLength,iDocPhraseLength);
		iDocParaLength = 0;			// reset ready for new paragraph
		iDocPhraseLength = 0;
		}
	if (iTagType == ECharacter)
		{
		iCharFormatLayer->SetL(iCharFormat, iCharFormatMask);
		if (iDocPhraseLength > 0)
			iRichTextDoc->ApplyCharFormatL(iCharFormat,iCharFormatMask,iDocInsertPos-iDocPhraseLength,iDocPhraseLength);
		iDocPhraseLength = 0;
		}
	}


TInt CParser::GetArgValue()
// converts numerals in iArgValue to a TInt, 
// first checking that the buffer is totally numeric in content
 	{
	TInt value = 0;
	if (BufIsNumeric(iArgValue))
		{
		TLex tmpLex(iArgValue);
		tmpLex.Val(value);
		}
	else iErrorLevel = EIllegalAttribValue;
	return value;
	}


TInt CParser::GetArgValue(const TDes &aBuf)
// converts numerals in aBuf to a TInt as above
	{ 
	TInt value = 0;
	if (BufIsNumeric(aBuf))
		{
		TLex tmpLex(aBuf);
		tmpLex.Val(value);
		}
	else iErrorLevel = EIllegalAttribValue;
	return value;
	}


TBool CParser::BufIsNumeric(const TDes &aBuffer)
// checks that aBuffer is totally numeric in content.
// checks all characters sequentially
	{
	TBool isNumeric = ETrue;
	TChar testChar;
	TUint bufLength = aBuffer.Length();
	for (TUint pos=0; ((pos<bufLength)&&(isNumeric)); pos++)
		{
		testChar = (aBuffer[pos]);
		if (!(testChar.IsDigit()&&isNumeric))
			isNumeric = EFalse;
		}
	return isNumeric;
	}

/* All the TransXxx.. functions translate arguments and their values and apply these 
	to RichText Formats and Masks */


void CParser::TranslateTagArgL()
	{
	switch (iTagType)
		{
		case EGlobal:
			TransGlobalArgL();
			break;
		case EParagraph:
			TransParagraphArgL(); 
			break;
		case ECharacter:
			TransCharArg();
			break;
		case EControl:
			TransControlArgL();
			break;
		case EError:
			break;
		}
	iCancelArg = EFalse;  // reset
	}


void CParser::TransControlArgL()
	{
	if (iArgType != _L(""))
		{
		if (iArgType == _L("TAB"))
			AddCharToParaL(KTabChar);	// TransTab();
		else
			iErrorLevel = EUnknownAttrib;
		}
	}


void CParser::TransGlobalArgL()
// Test for each possible arg in turn
// (a switch statement cannot be used)
	{
	if (iArgType != _L("")) 				// Is there an argument?
		{
		if (iArgType == _L("KEEPTOGETHER"))	
				TransParaKeepTogether();
		else if (iArgType == _L("KEEPWITHNEXT"))
				TransParaKeepWithNext();
		else if (iArgType == _L("STARTNEWPAGE"))	
				TransParaStartNewPage();
		else if (iArgType == _L("WIDOWORPHAN"))	 	
				TransParaWidowOrphan();

		if (iArgType == _L("ITALIC"))	
				TransCharPosture();
		else if (iArgType == _L("BOLD"))	
				TransCharStrokeWeight();
		else if (iArgType == _L("UNDERLINE"))	
				TransCharUnderline();
		else if (iArgType == _L("STRIKETHROUGH"))
				TransCharStrikethrough();

		else if (!iArgValueExpected)			// No argument value supplied when one is required for the
				iErrorLevel = ENoAttribValue;	// remaining options or an unknown argument supplied

		else if (iArgType == _L("PARALANGUAGE")) 
				OutputToScreen(_L("PARA LANGUAGE "));
		else if (iArgType == _L("LEFTMARGIN")) 	 	   
				TransParaArgLeftMargin();
		else if (iArgType == _L("RIGHTMARGIN"))  	
				TransParaArgRightMargin();
		else if (iArgType == _L("INDENT"))		 
				TransParaArgIndent();
		else if (iArgType == _L("ALIGNMENT"))	 
				TransParaArgAlignment();
		else if (iArgType == _L("LINESPACING"))	 
				TransParaLineSpacing();
		else if (iArgType == _L("LINESPACINGCONTROL"))   
				TransParaArgLineSpacingControl();
		else if (iArgType == _L("SPACEBEFORE"))	 	
				TransParaSpaceBefore();
		else if (iArgType == _L("SPACEAFTER"))	 
				TransParaSpaceAfter();
		else if (iArgType == _L("BORDERMARGIN"))	
				TransParaBorderMargin();
		else if (iArgType == _L("TOPBORDER"))	 	
				TransParaBorderL();
		else if (iArgType == _L("BOTTOMBORDER"))	
				TransParaBorderL();
		else if (iArgType == _L("LEFT BORDER"))	 
				TransParaBorderL();
		else if (iArgType == _L("RIGHTBORDER"))	 
				TransParaBorderL();
		else if (iArgType == _L("BULLET"))		 
				TransParaBullet();
		else if (iArgType == _L("DEFAULTTABWIDTH"))
				TransParaTabWidth();
		else if (iArgType == _L("TABSTOP"))
				TransParaTabStopL();

		else if (iArgType == _L("FONTHEIGHT"))	
				TransCharFontHeight();
		else if (iArgType == _L("PRINTPOS"))	
				TransCharPrintPos();
		else if (iArgType == _L("TYPEFACENAME"))
				TransCharTypefaceName();
		else if (iArgType == _L("TYPEFACEFLAGS"))	
				TransCharTypefaceFlags();
		else if (iArgType == _L("COLOR"))	 	 
				TransCharColor();
		else if (iArgType == _L("LANGUAGE"))	
				TransCharLanguage();
		else 
				iErrorLevel = EUnknownAttrib;
		}
	}


void CParser::TransParagraphArgL()
// Test for each possible arg in turn
// (a switch statement cannot be used)
	{
	if (iArgType != _L("")) 				// Is there an argument?
		{
		if (iArgType == _L("DEFAULT"))		
				TransParaDefault();
		else if (iArgType == _L("KEEPTOGETHER"))
				TransParaKeepTogether();
		else if (iArgType == _L("KEEPWITHNEXT"))
				TransParaKeepWithNext();
		else if (iArgType == _L("STARTNEWPAGE"))
				TransParaStartNewPage();
		else if (iArgType == _L("WIDOWORPHAN"))	 
				TransParaWidowOrphan();
		else if (!iArgValueExpected)			// No argument value supplied when one is required for the
				iErrorLevel = ENoAttribValue;	// remaining options or an unknown argument supplied
		else if (iArgType == _L("PARALANGUAGE")) 
				OutputToScreen(_L("PARA LANGUAGE "));
		else if (iArgType == _L("LEFTMARGIN")) 				   
				TransParaArgLeftMargin();
		else if (iArgType == _L("RIGHTMARGIN"))  
				TransParaArgRightMargin();
		else if (iArgType == _L("INDENT"))		 
				TransParaArgIndent();
		else if (iArgType == _L("ALIGNMENT"))	 
				TransParaArgAlignment();
		else if (iArgType == _L("LINESPACING"))	 
				TransParaLineSpacing();
		else if (iArgType == _L("LINESPACINGCONTROL"))  
				TransParaArgLineSpacingControl();
		else if (iArgType == _L("SPACEBEFORE"))	 
				TransParaSpaceBefore();
		else if (iArgType == _L("SPACEAFTER"))	 
				TransParaSpaceAfter();
		else if (iArgType == _L("BORDERMARGIN"))	
				TransParaBorderMargin();
		else if (iArgType == _L("TOPBORDER"))	 
				TransParaBorderL();
		else if (iArgType == _L("BOTTOMBORDER"))
				TransParaBorderL();
		else if (iArgType == _L("LEFT BORDER"))	 
				TransParaBorderL();
		else if (iArgType == _L("RIGHTBORDER"))	 
				TransParaBorderL();
		else if (iArgType == _L("BULLET"))		 
				TransParaBullet();
		else if (iArgType == _L("DEFAULTTABWIDTH")) 
				TransParaTabWidth();
		else if (iArgType == _L("TABSTOP"))
				TransParaTabStopL();
		else 
				iErrorLevel = EUnknownAttrib;
		}
	}


void CParser::TransParaDefault()
// turns off all applied para formatting - reverts to global
	{
	iParaFormatMask.ClearAll();
	OutputToScreen(_L("<PARA-DEFAULT>"));
	}


void CParser::TransParaArgLeftMargin()
	{
	TInt32 margin = GetArgValue();
	switch (iTagType)
		{
		case EParagraph:		 
			iParaFormat->iLeftMarginInTwips = margin;
			iParaFormatMask.SetAttrib(EAttLeftMargin);
			break;
		case EGlobal:
			iGlobalParaFormat->iLeftMarginInTwips = margin;
			iGlobalParaFormatMask.SetAttrib(EAttLeftMargin);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<LEFT MARGIN = %d>"),margin);
	OutputToScreen(outputBuf);
	}

void CParser::TransParaArgRightMargin()
	{
	TInt32 margin = GetArgValue();		
	switch (iTagType)
		{
		case EParagraph:		 
			iParaFormat->iRightMarginInTwips = margin;
			iParaFormatMask.SetAttrib(EAttRightMargin);
			break;
		case EGlobal:
			iGlobalParaFormat->iRightMarginInTwips = margin;
			iGlobalParaFormatMask.SetAttrib(EAttRightMargin);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<RIGHT MARGIN = %d>"),margin);
	OutputToScreen(outputBuf);
	}

void CParser::TransParaArgIndent()
	{
	TInt32 indent = GetArgValue();
	switch (iTagType)
		{
		case EParagraph:		 
			iParaFormat->iIndentInTwips = indent;
			iParaFormatMask.SetAttrib(EAttIndent);
			break;
		case EGlobal:
			iGlobalParaFormat->iIndentInTwips = indent;
			iGlobalParaFormatMask.SetAttrib(EAttIndent);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<INDENT = %d>"),indent);
	OutputToScreen(outputBuf);
	}

void CParser::TransParaLineSpacing()
	{
	TInt32 lineSpacing = GetArgValue();		  
	switch (iTagType)
		{
		case EParagraph:		 
			iParaFormat->iLineSpacingInTwips = lineSpacing;
			iParaFormatMask.SetAttrib(EAttLineSpacing);
			break;
		case EGlobal:
			iGlobalParaFormat->iLineSpacingInTwips = lineSpacing;
			iGlobalParaFormatMask.SetAttrib(EAttLineSpacing);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<LINESPACING = %d>"),lineSpacing);
	OutputToScreen(outputBuf);
	}

void CParser::TransParaArgLineSpacingControl()
	{
	if (iArgValue == _L("ATLEAST"))
		{
		switch (iTagType)
			{
			case EParagraph:		 
				iParaFormat->iLineSpacingControl = CParaFormat::ELineSpacingAtLeastInTwips;
				iParaFormatMask.SetAttrib(EAttLineSpacingControl);
				break;
			case EGlobal:
				iGlobalParaFormat->iLineSpacingControl = CParaFormat::ELineSpacingAtLeastInTwips;
				iGlobalParaFormatMask.SetAttrib(EAttLineSpacingControl);
				break;
			}
		OutputToScreen(_L("<Line spacing control Atleast>"));
		}
	else if (iArgValue == _L("EXACTLY"))
		{
		switch (iTagType)
			{
			case EParagraph:		 
				iParaFormat->iLineSpacingControl = CParaFormat::ELineSpacingExactlyInTwips;
				iParaFormatMask.SetAttrib(EAttLineSpacingControl);
				break;
			case EGlobal:
				iGlobalParaFormat->iLineSpacingControl = CParaFormat::ELineSpacingExactlyInTwips;
				iGlobalParaFormatMask.SetAttrib(EAttLineSpacingControl);
				break;
			}
		OutputToScreen(_L("<Line spacing control Exactly>"));
		}
	else
		iErrorLevel = EIllegalAttribValue;
	}

void CParser::TransParaSpaceBefore()
	{
	TInt32 spaceBefore = GetArgValue();		  
	switch (iTagType)
		{
		case EParagraph:		 
			iParaFormat->iSpaceBeforeInTwips = spaceBefore;
			iParaFormatMask.SetAttrib(EAttSpaceBefore);
			break;
		case EGlobal:
			iGlobalParaFormat->iSpaceBeforeInTwips = spaceBefore;
			iGlobalParaFormatMask.SetAttrib(EAttSpaceBefore);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<SPACE BEFORE = %d>"),spaceBefore);
	OutputToScreen(outputBuf);
	}

void CParser::TransParaSpaceAfter()
	{
	TInt32 spaceAfter = GetArgValue();		  
	switch (iTagType)
		{
		case EParagraph:		 
			iParaFormat->iSpaceAfterInTwips = spaceAfter;
			iParaFormatMask.SetAttrib(EAttSpaceAfter);
			break;
		case EGlobal:
			iGlobalParaFormat->iSpaceAfterInTwips = spaceAfter;
			iGlobalParaFormatMask.SetAttrib(EAttSpaceAfter);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<SPACE AFTER = %d>"),spaceAfter);
	OutputToScreen(outputBuf);
	}

void CParser::TransParaBorderMargin()
	{
	TInt32 borderMargin = GetArgValue();		  
	switch (iTagType)
		{
		case EParagraph:		 
			iParaFormat->iBorderMarginInTwips = borderMargin;
			iParaFormatMask.SetAttrib(EAttBorderMargin);
			break;
		case EGlobal:
			iGlobalParaFormat->iBorderMarginInTwips = borderMargin;
			iGlobalParaFormatMask.SetAttrib(EAttBorderMargin);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<BORDER MARGIN = %d>"),borderMargin);
	OutputToScreen(outputBuf);
	}

void CParser::TransParaKeepTogether()
	{
	switch (iTagType)
		{
		case EParagraph:		 
			if (iCancelArg)
				iParaFormat->iKeepTogether = EFalse;
			else
				iParaFormat->iKeepTogether = ETrue;
			iParaFormatMask.SetAttrib(EAttKeepTogether);
			break;
		case EGlobal:
			if (iCancelArg)
				iGlobalParaFormat->iKeepTogether = EFalse;
			else
				iGlobalParaFormat->iKeepTogether = ETrue;
			iGlobalParaFormatMask.SetAttrib(EAttKeepTogether);
			break;
		}
	// screen output
	OutputToScreen(_L("<KEEP TOGETHER>"));
	}

void CParser::TransParaKeepWithNext()
	{
	switch (iTagType)
		{
		case EParagraph:		 
			if (iCancelArg)
				iParaFormat->iKeepWithNext = EFalse;
			else
				iParaFormat->iKeepWithNext = ETrue;
			iParaFormatMask.SetAttrib(EAttKeepWithNext);
			break;
		case EGlobal:
			if (iCancelArg)
				iGlobalParaFormat->iKeepWithNext = EFalse;
			else
				iGlobalParaFormat->iKeepWithNext = ETrue;
			iGlobalParaFormatMask.SetAttrib(EAttKeepWithNext);
			break;
		}
	// screen output
	OutputToScreen(_L("<KEEP WITH NEXT>"));
	}

void CParser::TransParaStartNewPage()
	{
	switch (iTagType)
		{
		case EParagraph:		 
			if (iCancelArg)
				iParaFormat->iStartNewPage = EFalse;
			else
				iParaFormat->iStartNewPage = ETrue;
			iParaFormatMask.SetAttrib(EAttStartNewPage);
			break;
		case EGlobal:
			if (iCancelArg)
				iGlobalParaFormat->iStartNewPage = EFalse;
			else
				iGlobalParaFormat->iStartNewPage = ETrue;
			iGlobalParaFormatMask.SetAttrib(EAttStartNewPage);
			break;
		}
	// screen output
	OutputToScreen(_L("<START NEW PAGE>"));
	}

void CParser::TransParaWidowOrphan()
	{
	switch (iTagType)
		{
		case EParagraph:		 
			if (iCancelArg)
				iParaFormat->iWidowOrphan = EFalse;
			else
				iParaFormat->iWidowOrphan = ETrue;
			iParaFormatMask.SetAttrib(EAttWidowOrphan);
			break;
		case EGlobal:
			if (iCancelArg)
				iGlobalParaFormat->iWidowOrphan = EFalse;
			else
				iGlobalParaFormat->iWidowOrphan = ETrue;
			iGlobalParaFormatMask.SetAttrib(EAttWidowOrphan);
			break;
		}
	// screen output
	OutputToScreen(_L("<WIDOWS & ORPHANS>"));
	}

void CParser::TransParaArgAlignment()
	{
	// parse argValue
	if (iArgValue == _L("LEFT"))
		{
		switch (iTagType)
			{
			case EParagraph:		 
				iParaFormat->iHorizontalAlignment = CParaFormat::ELeftAlign;
				iParaFormatMask.SetAttrib(EAttAlignment);
				break;
			case EGlobal:
				iGlobalParaFormat->iHorizontalAlignment = CParaFormat::ELeftAlign;
				iGlobalParaFormatMask.SetAttrib(EAttAlignment);
				break;
			}
		OutputToScreen(_L("<ALIGNMENT LEFT>"));
		}
	else if (iArgValue == _L("RIGHT"))
		{
		switch (iTagType)
			{
			case EParagraph:		 
				iParaFormat->iHorizontalAlignment = CParaFormat::ERightAlign;
				iParaFormatMask.SetAttrib(EAttAlignment);
				break;
			case EGlobal:
				iGlobalParaFormat->iHorizontalAlignment = CParaFormat::ERightAlign;
				iGlobalParaFormatMask.SetAttrib(EAttAlignment);
				break;
			}
		OutputToScreen(_L("<ALIGNMENT RIGHT>"));
		}
	else if (iArgValue == _L("CENTER"))
		{
		switch (iTagType)
			{
			case EParagraph:		 
				iParaFormat->iHorizontalAlignment = CParaFormat::ECenterAlign;
				iParaFormatMask.SetAttrib(EAttAlignment);
				break;
			case EGlobal:
				iGlobalParaFormat->iHorizontalAlignment = CParaFormat::ECenterAlign;
				iGlobalParaFormatMask.SetAttrib(EAttAlignment);
				break;
			}
		OutputToScreen(_L("<ALIGNMENT CENTER>"));
		}
	else if (iArgValue == _L("JUSTIFIED"))
		{
		switch (iTagType)
			{
			case EParagraph:		 
				iParaFormat->iHorizontalAlignment = CParaFormat::EJustifiedAlign;
				iParaFormatMask.SetAttrib(EAttAlignment);
				break;
			case EGlobal:
				iGlobalParaFormat->iHorizontalAlignment = CParaFormat::EJustifiedAlign;
				iGlobalParaFormatMask.SetAttrib(EAttAlignment);
				break;
			}
		OutputToScreen(_L("<ALIGNMENT JUSTIFIED>"));
		}
	else
		iErrorLevel = EIllegalAttribValue;
	}


void CParser::TransParaBullet()
	{
	TInt characterCode = 0x2022;
	TUint32 heightInTwips=0;
	TUint32 flags=0;
	TBuf<KMaxTypefaceNameLength> name;
	TBuf<128> component;

	// set first component (character code)
	TInt commaPos = iArgValue.Locate(KComma);	 // find pos of first comma
	component = iArgValue.Left(commaPos);
	if (component != _L(""))					// only third arg remains
		{
		if (BufIsNumeric(component))
			{
			TLex tmpLex(component);
			// TUint value;
			tmpLex.Val(characterCode);
			}
		else 
			iErrorLevel = EIllegalAttribValue;
		}
	else
		iErrorLevel = EIllegalAttribValue;

	// set second component (bullet height)
	iArgValue.Delete(0,commaPos+1);				// delete previous component & trailing comma 
	commaPos = iArgValue.Locate(KComma);	// find pos of first comma
	component = iArgValue.Left(commaPos);
	if (component != _L(""))					// only third arg remains
		{
		if (BufIsNumeric(component))
			{
			TLex tmpLex(component);
			TUint value;
			tmpLex.Val(value);
			heightInTwips = value;
			}
		else 
			iErrorLevel = EIllegalAttribValue;
		}
	else
		iErrorLevel = EIllegalAttribValue;

	// set third component (typeface flags)
	iArgValue.Delete(0,commaPos+1);				// delete previous component & trailing comma 
	commaPos = iArgValue.Locate(KComma);		// find pos of first comma
	component = iArgValue.Left(commaPos);
	if (component != _L(""))					// only third arg remains
		{
		flags = GetArgValue(component);
		if (flags>2)
			iErrorLevel=EIllegalAttribValue;	// only values 0,1,2 valid for flag
		}
	else
		iErrorLevel = EIllegalAttribValue;

	// set fourth component (typeface name)
	iArgValue.Delete(0,commaPos+1);				// delete previous component & trailing comma 
	if (iArgValue != _L(""))					// only third arg remains
		{
		name.Copy(iArgValue);		
//		name = SquashBuf(iArgValue);
		}
	else
		iErrorLevel = EIllegalAttribValue;

	// apply
	if (iErrorLevel == ENoError)
		{
		iBulletUsed = ETrue;
		iBullet->iCharacterCode = (TText)characterCode;
		iBullet->iHeightInTwips = heightInTwips;
		if (flags>=1)
			iBullet->iTypeface.SetIsProportional(ETrue);
		if (flags>=3)
			iBullet->iTypeface.SetIsSerif(ETrue);
		iBullet->iTypeface.iName = name;
		switch (iTagType)
			{
			case EParagraph:
				iParaFormat->iBullet = iBullet;  
				iParaFormatMask.SetAttrib(EAttBullet);
				break;
			case EGlobal:
				iGlobalParaFormat->iBullet = iBullet;
				iGlobalParaFormatMask.SetAttrib(EAttBullet);
				break;
			}
		OutputToScreen(_L("<BULLET>"));
		}
	}

TBuf8<512> CParser::SquashBuf(TDes aBuffer)
	//
	// Input 8/16 bit buffer to be returned as an 8-bit version
	// Used for unicode compatability
	//
	{
	TText16 textPointer;

	TBuf8<512> returnBuf;

	for ( TInt pos=0 ; pos<aBuffer.Length() ; pos++ )
		{
		textPointer = aBuffer[pos];
#pragma warning ( disable : 4244 )
		returnBuf[pos] = textPointer; // conversion from unsigned short to unsigned char (16 bit to 8 bit) in unicode
#pragma warning ( default : 4244 )
		}
	return returnBuf;
	}



void CParser::TransParaBorderL()
	{
	TParaBorder::TLineStyle lineStyle=TParaBorder::ENullLineStyle;
	TBool autoColor=EFalse;
	TRgb color;
	TBuf<128> component;

	// set first component
	TInt commaPos = iArgValue.Locate(KComma);	 // find pos of first comma
	component = iArgValue.Left(commaPos);
	if (component != _L(""))
		{
		if (component == _L("NULL"))
			lineStyle = TParaBorder::ENullLineStyle;
		else if (component == _L("SOLID"))
			lineStyle = TParaBorder::ESolid;
		else if (component == _L("DOUBLE"))
			lineStyle = TParaBorder::EDouble;
		else if (component == _L("DOTTED"))
			lineStyle = TParaBorder::EDotted;
		else if (component == _L("DASHED"))
			lineStyle = TParaBorder::EDashed;
		else if (component == _L("DOTDASH"))
			lineStyle = TParaBorder::EDotDash;
		else if (component == _L("DOTDOTDASH"))
			lineStyle = TParaBorder::EDotDotDash;
		else
			iErrorLevel = EIllegalAttribValue; 
		}
	else
		iErrorLevel = EIllegalAttribValue;
	
	// set second component
	iArgValue.Delete(0,commaPos+1);				// delete first component & trailing comma 
	commaPos = iArgValue.Locate(KComma);	 	// find pos of first comma
	component = iArgValue.Left(commaPos);
	if (component != _L(""))
		{
		if (component == _L("ON"))
			autoColor = ETrue;
		else if (component == _L("OFF"))
			autoColor = EFalse;
		else
			iErrorLevel = EIllegalAttribValue; 
		}
	else
		iErrorLevel = EIllegalAttribValue;

	// set third component
	iArgValue.Delete(0,commaPos+1);				// delete second component & trailing comma 
	if (component != _L(""))					// only third arg remains
		{
		if (BufIsNumeric(iArgValue))
			{
			TLex tmpLex(iArgValue);
			TUint value;
			tmpLex.Val(value);
			color=TRgb((TUint32)value);
			}
		else 
			iErrorLevel = EIllegalAttribValue;
		}

	// apply
	if (iErrorLevel == ENoError)
		{
		iBorderUsed = ETrue;
		iBorder->iLineStyle = lineStyle;
		iBorder->iAutoColor = autoColor;
		iBorder->iColor = color;
		if (iArgType == _L("TOPBORDER"))
			{
			switch (iTagType)
				{
				case EParagraph:
					iParaFormat->SetParaBorderL(CParaFormat::EParaBorderTop,*iBorder);
					iParaFormatMask.SetAttrib(EAttTopBorder);
					break;
				case EGlobal:
					iGlobalParaFormat->SetParaBorderL(CParaFormat::EParaBorderTop,*iBorder);
					iGlobalParaFormatMask.SetAttrib(EAttTopBorder);
					break;
				}
			}
		if (iArgType == _L("BOTTOMBORDER"))
			{
			switch (iTagType)
				{
				case EParagraph:
					iParaFormat->SetParaBorderL(CParaFormat::EParaBorderBottom,*iBorder);
					iParaFormatMask.SetAttrib(EAttBottomBorder);
					break;
				case EGlobal:
					iGlobalParaFormat->SetParaBorderL(CParaFormat::EParaBorderBottom,*iBorder);
					iGlobalParaFormatMask.SetAttrib(EAttBottomBorder);
					break;
				}
			}
		if (iArgType == _L("LEFTBORDER"))
			{
			switch (iTagType)
				{
				case EParagraph:
					iParaFormat->SetParaBorderL(CParaFormat::EParaBorderLeft,*iBorder);
					iParaFormatMask.SetAttrib(EAttLeftBorder);
					break;
				case EGlobal:
					iGlobalParaFormat->SetParaBorderL(CParaFormat::EParaBorderLeft,*iBorder);
					iGlobalParaFormatMask.SetAttrib(EAttLeftBorder);
					break;
				}
			}
		if (iArgType == _L("RIGHTBORDER"))
			{
			switch (iTagType)
				{
				case EParagraph:
					iParaFormat->SetParaBorderL(CParaFormat::EParaBorderRight,*iBorder);
					iParaFormatMask.SetAttrib(EAttRightBorder);
					break;
				case EGlobal:
					iGlobalParaFormat->SetParaBorderL(CParaFormat::EParaBorderRight,*iBorder);
					iGlobalParaFormatMask.SetAttrib(EAttRightBorder);
					break;
				}
			}
		OutputToScreen(_L("<PARA BORDER>"));
		}
	}

void CParser::TransParaTabWidth()
	{
	TInt32 tabWidth = GetArgValue();
	switch (iTagType)
		{
		case EParagraph:		 
			iParaFormat->iDefaultTabWidthInTwips = tabWidth;
			iParaFormatMask.SetAttrib(EAttDefaultTabWidth);
			break;
		case EGlobal:
			iGlobalParaFormat->iDefaultTabWidthInTwips = tabWidth;
			iGlobalParaFormatMask.SetAttrib(EAttDefaultTabWidth);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<DEFAULT TAB WIDTH = %d>"),tabWidth);
	OutputToScreen(outputBuf);
	}


void CParser::TransParaTabStopL()
// This arg has a compound value of the form Tabstop=value,type
// It also accepts !Tabstop=value to delete an existing tabstop
	{
	TTabStop tabStop;
	TBuf<128> component;
	TBuf<6> outputBuf;

	// get first component (tab type)
	TInt commaPos = iArgValue.Locate(KComma);	 // find pos of first comma (returns -1 if no comma)
	if (commaPos > 0)	// comma & first component exist
		{
		component = iArgValue.Left(commaPos);	// take part to the left of the comma
		if (component != _L(""))				
			tabStop.iTwipsPosition = GetArgValue(component);
		else
			iErrorLevel = EIllegalAttribValue;
		}
	else if ((iCancelArg)&&(commaPos == -1))	// no comma but only one component required (position)
		{
		if (iArgValue != _L(""))				
			tabStop.iTwipsPosition = GetArgValue(iArgValue);
		else
			iErrorLevel = EIllegalAttribValue;
		}

	if (iErrorLevel == ENoError)
		{
		if (iCancelArg)
			// insert a null tab
			{
			if ((iParaFormat->LocateTab(tabStop.iTwipsPosition) != KTabNotFound)
				||(iGlobalParaFormat->LocateTab(tabStop.iTwipsPosition) != KTabNotFound))
					{
					tabStop.iType = TTabStop::ENullTab;	// null tab "deletes" existing tab
					outputBuf = _L("<!TAB>");
					}
			else
				iErrorLevel = EIllegalAttribValue;
			}
		else
			{
			// set second component (tab position in twips)
			iArgValue.Delete(0,commaPos+1);				// delete previous component & trailing comma 
			if (iArgValue != _L(""))				
				{
				outputBuf = _L("<TAB>");
				if (iArgValue == _L("NULL"))
					tabStop.iType = TTabStop::ENullTab;
				else if (iArgValue == _L("LEFT"))
					tabStop.iType = TTabStop::ELeftTab;
				else if (iArgValue == _L("CENTERED"))
					tabStop.iType = TTabStop::ECenteredTab;
				else if (iArgValue == _L("RIGHT"))
					tabStop.iType = TTabStop::ERightTab;
				else 
					iErrorLevel = EIllegalAttribValue;
				}
			else
				iErrorLevel = EIllegalAttribValue;
			}
		}

 	// Insert the tab
	if (iErrorLevel == ENoError)
		{
		switch (iTagType)
			{
			case EParagraph:
				iParaFormat->StoreTabL(tabStop);
				iParaFormatMask.SetAttrib(EAttTabStop);
				break;
			case EGlobal:
				iGlobalParaFormat->StoreTabL(tabStop);
				iGlobalParaFormatMask.SetAttrib(EAttTabStop);
				break;
			}
		// output to screen
		OutputToScreen(outputBuf);
		}
	}

 
void CParser::TransCharArg()
	{
	if (iArgType != _L("")) 				// Is there an argument?
		{
		if (iArgType == _L("DEFAULT"))
				TransCharDefault();
		else if (iArgType == _L("ITALIC"))	
				TransCharPosture();
		else if (iArgType == _L("BOLD"))	
				TransCharStrokeWeight();
		else if (iArgType == _L("UNDERLINE"))	
				TransCharUnderline();
		else if (iArgType == _L("STRIKETHROUGH"))
				TransCharStrikethrough();
		else if (!iArgValueExpected)			// No argument value supplied when one is required for the
				iErrorLevel = ENoAttribValue;	// remaining options or an unknown argument supplied
		else if (iArgType == _L("FONTHEIGHT"))	
				TransCharFontHeight();
		else if (iArgType == _L("PRINTPOS"))	
				TransCharPrintPos();
		else if (iArgType == _L("TYPEFACENAME"))
				TransCharTypefaceName();
		else if (iArgType == _L("TYPEFACEFLAGS"))	
				TransCharTypefaceFlags();
		else if (iArgType == _L("COLOR"))	 	 
				TransCharColor();
		else if (iArgType == _L("LANGUAGE"))	
				TransCharLanguage();
		else 
				iErrorLevel = EUnknownAttrib;
		}
	}


void CParser::TransCharDefault()
// turns off all applied Char formatting - reverts to global
	{
	iCharFormatMask.ClearAll();
	OutputToScreen(_L("<CHAR-DEFAULT>"));
	}


void CParser::TransCharPosture()
	{
	switch (iTagType)
		{
		case ECharacter:		 
			if (iCancelArg)
				iCharFormat.iFontSpec.iFontStyle.SetPosture(EPostureUpright);
			else
				iCharFormat.iFontSpec.iFontStyle.SetPosture(EPostureItalic);
			iCharFormatMask.SetAttrib(EAttFontPosture);
			break;
		case EGlobal:
			if (iCancelArg)
				iGlobalCharFormat.iFontSpec.iFontStyle.SetPosture(EPostureUpright);
			else
				iGlobalCharFormat.iFontSpec.iFontStyle.SetPosture(EPostureItalic);
			iGlobalCharFormatMask.SetAttrib(EAttFontPosture);
			break;
		}
	// screen output
	OutputToScreen(_L("<ITALIC>"));
	}


void CParser::TransCharStrokeWeight()
	{
	switch (iTagType)
		{
		case ECharacter:		 
			if (iCancelArg)
				iCharFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
			else
				iCharFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
			iCharFormatMask.SetAttrib(EAttFontStrokeWeight);
			break;
		case EGlobal:
			if (iCancelArg)
				iGlobalCharFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
			else
				iGlobalCharFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
			iGlobalCharFormatMask.SetAttrib(EAttFontStrokeWeight);
			break;
		}
	// screen output
	OutputToScreen(_L("<BOLD>"));
	}


void CParser::TransCharUnderline()
	{
	switch (iTagType)
		{
		case ECharacter:		 
			if (iCancelArg)
				iCharFormat.iFontPresentation.iUnderline = EUnderlineOff;
			else
				iCharFormat.iFontPresentation.iUnderline = EUnderlineOn;
			iCharFormatMask.SetAttrib(EAttFontUnderline);
			break;
		case EGlobal:
			if (iCancelArg)
				iGlobalCharFormat.iFontPresentation.iUnderline = EUnderlineOff;
			else
				iGlobalCharFormat.iFontPresentation.iUnderline = EUnderlineOn;
			iGlobalCharFormatMask.SetAttrib(EAttFontUnderline);
			break;
		}
	// screen output
	OutputToScreen(_L("<UNDERLINE>"));
	}


void CParser::TransCharStrikethrough()
	{
	switch (iTagType)
		{
		case ECharacter:		 
			if (iCancelArg)
				iCharFormat.iFontPresentation.iStrikethrough = EStrikethroughOff;
			else
				iCharFormat.iFontPresentation.iStrikethrough = EStrikethroughOn;
			iCharFormatMask.SetAttrib(EAttFontStrikethrough);
			break;
		case EGlobal:
			if (iCancelArg)
				iGlobalCharFormat.iFontPresentation.iStrikethrough = EStrikethroughOff;
			else
				iGlobalCharFormat.iFontPresentation.iStrikethrough = EStrikethroughOn;
			iGlobalCharFormatMask.SetAttrib(EAttFontStrikethrough);
			break;
		}
	// screen output
	OutputToScreen(_L("<STRIKETHROUGH>"));
	}


void CParser::TransCharFontHeight()
	{
	TInt32 fontHeight = GetArgValue();		  
	switch (iTagType)
		{
		case ECharacter:		 
			iCharFormat.iFontSpec.iHeight = fontHeight;
			iCharFormatMask.SetAttrib(EAttFontHeight);
			break;
		case EGlobal:
			iGlobalCharFormat.iFontSpec.iHeight = fontHeight;
			iGlobalCharFormatMask.SetAttrib(EAttFontHeight);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<FONTHEIGHT = %d>"),fontHeight);
	OutputToScreen(outputBuf);
	}

void CParser::TransCharPrintPos()
	{
	if (iArgValue == _L("NORMAL"))
		{
		switch (iTagType)
			{
			case ECharacter:		 
				iCharFormat.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosNormal);
				iCharFormatMask.SetAttrib(EAttFontPrintPos);
				break;
			case EGlobal:
				iGlobalCharFormat.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosNormal);
				iGlobalCharFormatMask.SetAttrib(EAttFontPrintPos);
				break;
			}
		OutputToScreen(_L("<PRINT POSITION NORMAL>"));
		}
	else if (iArgValue == _L("SUPERSCRIPT"))
		{
		switch (iTagType)
			{
			case ECharacter:		 
				iCharFormat.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSuperscript);
				iCharFormatMask.SetAttrib(EAttFontPrintPos);
				break;
			case EGlobal:
				iGlobalCharFormat.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSuperscript);
				iGlobalCharFormatMask.SetAttrib(EAttFontPrintPos);
				break;
			}
		OutputToScreen(_L("<SUPERSCRIPT>"));
		}
	else if (iArgValue == _L("SUBSCRIPT"))
		{
		switch (iTagType)
			{
			case ECharacter:		 
				iCharFormat.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSubscript);
				iCharFormatMask.SetAttrib(EAttFontPrintPos);
				break;
			case EGlobal:
				iGlobalCharFormat.iFontSpec.iFontStyle.SetPrintPosition(EPrintPosSubscript);
				iGlobalCharFormatMask.SetAttrib(EAttFontPrintPos);
				break;
			}
		OutputToScreen(_L("<SUBSCRIPT>"));
		}
	else
		iErrorLevel = EIllegalAttribValue;
	}


void CParser::TransCharTypefaceFlags()
	{
	TUint flags = GetArgValue();
	if (flags>2)
		iErrorLevel=EIllegalAttribValue;	// only values 0,1,2 valid for flag
	if (iErrorLevel==ENoError)
		{
		switch (iTagType)
			{
			case ECharacter:		 
				if (flags>=1)
					iCharFormat.iFontSpec.iTypeface.SetIsProportional(ETrue);
				if (flags>=3)
					iCharFormat.iFontSpec.iTypeface.SetIsSerif(ETrue);
				iCharFormatMask.SetAttrib(EAttFontTypeface);
				break;
			case EGlobal:
				if (flags>=1)
					iGlobalCharFormat.iFontSpec.iTypeface.SetIsProportional(ETrue);
				if (flags>=3)
					iGlobalCharFormat.iFontSpec.iTypeface.SetIsSerif(ETrue);
				iGlobalCharFormatMask.SetAttrib(EAttFontTypeface);
				break;
			}
		// screen output
		TBuf<80> outputBuf;
		outputBuf.Format(_L("<TYPEFACE FLAGS = %d>"),flags);
		OutputToScreen(outputBuf);
		}
	}


void CParser::TransCharTypefaceName()
	{
	switch (iTagType)
		{
		case ECharacter:
			iCharFormat.iFontSpec.iTypeface.iName=iArgValue;
			iCharFormatMask.SetAttrib(EAttFontTypeface);
			break;
		case EGlobal:
			iGlobalCharFormat.iFontSpec.iTypeface.iName=iArgValue;
			iGlobalCharFormatMask.SetAttrib(EAttFontTypeface);
			break;
		}
	// screen output
	TBuf<80> outputBuf;
	outputBuf.Format(_L("<TYPEFACE NAME = %S>"),&iArgValue);
	OutputToScreen(outputBuf);
	}


void CParser::TransCharColor()
	{
	TUint value = GetArgValue();
	if (iErrorLevel==ENoError)
		{
		TRgb color;
		color=TRgb((TUint32)value);
		switch (iTagType)
			{
			case ECharacter:		 
				iCharFormat.iFontPresentation.iTextColor = color;
				iCharFormatMask.SetAttrib(EAttColor);
				break;
			case EGlobal:
				iGlobalCharFormat.iFontPresentation.iTextColor = color;
				iGlobalCharFormatMask.SetAttrib(EAttColor);
				break;
			}
		// screen output
		TBuf<80> outputBuf;
		outputBuf.Format(_L("<COLOR = %d>"),value);
		OutputToScreen(outputBuf);
		}
	}


void CParser::TransCharLanguage()
// Assumes languages are integer coded - will have to change in time
	{
	TUint value = GetArgValue();
	if (iErrorLevel==ENoError)
		{
		switch (iTagType)
			{
			case ECharacter:		 
				iCharFormat.iLanguage = value;
				iCharFormatMask.SetAttrib(EAttCharLanguage);
				break;
			case EGlobal:
				iGlobalCharFormat.iLanguage = value;
				iGlobalCharFormatMask.SetAttrib(EAttCharLanguage);
				break;
			}
		// screen output
		TBuf<80> outputBuf;
		outputBuf.Format(_L("<LANGUAGE = %d>"),value);
		OutputToScreen(outputBuf);
		}
	}



////////////////////////////////
// CFileApp
////////////////////////////////


CFileApp* CFileApp::NewL()
	{
	CFileApp* self=new(ELeave) CFileApp;
	CleanupStack::PushL(self);
	self->ConstructApplicationL();
	CleanupStack::Pop();
	return self;
	}


CFileApp::CFileApp()
	{
	// init variables
	iConsoleExists = EFalse;
	iFilePos = 0;
	}
	 

void CFileApp::ConstructApplicationL()
	{
	iTextBuf = CBufSeg::NewL(64);  // Granularity of 64 bytes
	}


CFileApp::~CFileApp()
	{
	delete iTextBuf;
	}


CBufSeg* CFileApp::LoadFileL(CConsoleBase* aConsoleWin)
// Asks for file name, then loads file into text buffer.
	{ 
	iConsoleExists = ETrue;
	iConsole = aConsoleWin;			// grab console handle
	GetFileName();
	FileHandlingL();					// load file & store it
	return iTextBuf;
	}

CBufSeg* CFileApp::LoadFileL(const TFileName &aFileName)
// uses supplied FileName to load file into CBufSeg
	{ 
	iFileName = aFileName;
	FileHandlingL();					// load file & store it
	return iTextBuf;
	}


void CFileApp::OutputToScreen(const TDesC& aMessageBuffer)
	{
	if (iConsoleExists)
		iConsole->Write(aMessageBuffer);  // output line to screen
	}


TInt CFileApp::SaveFile(CBufSeg* aTextBuf, TFileName aFileName)
// saves supplied buffer to disc, making the new filename a varient of the PML source filename supplied
	{
	// set filename to be saved as
	TInt length = aFileName.Length();
	aFileName.Delete(length-1,1);
	aFileName.Append(_L("G")); // generated filenames end in G

	// create fileserver client and save.
	RFs fsClient;
	RFile theFile;
	TInt err = fsClient.Connect();
	if (err == 0)
		err = theFile.Replace(fsClient, aFileName, EFileStream|EFileRead|EFileWrite);
	if (err == 0)
		{
		TInt readWritePos = 0;
		TBuf8<80> insertBuf;
		TInt readLength=0;
		FOREVER
			{
			// read from TextBuf into insertBuf
			readLength=Min(aTextBuf->Size()-readWritePos,insertBuf.MaxLength());
			insertBuf.SetLength(0); 				// empty buffer
			aTextBuf->Read(readWritePos,insertBuf,readLength);
			// insert buf into file
			theFile.Write(readWritePos, insertBuf);
			readWritePos += insertBuf.Length();
			if (readWritePos >= aTextBuf->Size())
				break;
			}
	  	theFile.Close();
		}
	return err;
	}

void CFileApp::GetFileName()
// read FileName synchronously from console
	{ 
	TKeyCode keystroke = EKeyNull;
	iFileName.SetLength(0);

	//Debuging cheat to hardwire filename
	//iFileName=_L("d:\\etext\\incp\\dunk.pml");
	//return;

	OutputToScreen(_L("Enter file to be parsed: "));
	keystroke = iConsole->Getch();
	while (keystroke != EKeyEnter)
		{
		TBuf<1> uniBuf; // used to ease unicode build
		uniBuf.Append(keystroke);

		iConsole->Write(uniBuf);
		iFileName.Append(uniBuf);
		keystroke = iConsole->Getch();
		}
	WriteNewLine();
	}

void CFileApp::FileHandlingL()
// Open a file, read contents into buffer, then close file again
	{
	// open file
    RFile textFile;
	RFs fsClient;
	TInt err = 1;
	while (err)
		{
		err = fsClient.Connect();
		if (!err)
    		err = textFile.Open(fsClient,iFileName,EFileStream|EFileRead|EFileShareReadersOnly);
    	if (err)
			return;
/******GA
			{
    		OutputToScreen(_L("Error in file open\n\n"));
    		GetFileName();
    		}
******/
    	}   	
    
	OutputToScreen(_L("Parsing "));
	OutputToScreen(iFileName);
	WriteNewLine();
	WriteNewLine();

	// Read file into segmented buffer
	TUint insertPos = 0;

	TInt max = sizeof(TText) * KFileBufSize;

	FOREVER
		{
		iFileBuf.SetLength(0);
		ReadChunkOfFileContents(textFile);
		iTextBuf->/*Do*/InsertL(insertPos,iFileBuf.Ptr(),iFileBuf.Size());
		insertPos += iFileBuf.Size();			// in bytes
		if (iFileBuf.Size() < max)
			break;
		}

	// finish up
 	textFile.Close();
	iFilePos = 0; // reset
	}

void CFileApp::ReadChunkOfFileContents(RFile &aFile)
    {
	TBuf8<KFileBufSize> readBuf;
	aFile.Read(iFilePos,readBuf);
	// read into iFileBuf (8/16 bit); 
	TText textPointer;
	for ( TInt pos=0 ; pos<readBuf.Length() ; pos++ )
		{
		textPointer = readBuf[pos];
		iFileBuf.Append(textPointer);
		}
    //TInt ret=aFile.Read(iFilePos,iFileBuf);
	iFilePos += KFileBufSize;
    }

void CFileApp::WriteNewLine()
//
	{
	TBuf<1> buf(_L("\n"));
	OutputToScreen(buf);
	}

