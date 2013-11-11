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


#ifndef __T_PMLPAR_H__
#define __T_PMLPAR_H__

#include <e32cons.h>
#include <f32file.h>
#include <txtrich.h>
#include <txtfrmat.h>
#include <txtfmlyr.h>

#pragma warning ( disable : 4705 ) // turns off "statement has no effect" warning

const TInt KCharLength = 2;

// Clases defined in this file
class CFileApp;
class CParser;

const TChar KTabChar=TChar(9);
const TChar KLineFeed(10);
const TChar KComma=TChar(44);
const TInt	KFileBufSize=128;

const TBuf<1> NEWLINE(_L("\n"));

class CFileApp : public CBase
	{ // Reads a text file into a segmented buffer
public:
	static CFileApp* NewL();
	~CFileApp();

	CBufSeg* LoadFileL(CConsoleBase* aConsole);
	CBufSeg* LoadFileL(const TFileName &aFileName);
	TInt SaveFile(CBufSeg* aTextBuf, TFileName aFileName);	  // returns error level
	inline TFileName ReturnFileName() {return iFileName;}
protected:
	CFileApp();
	void ConstructApplicationL();
private:
	void OutputToScreen(const TDesC& aMessageBuffer);
	void FileHandlingL();
	void GetFileName();
	void ReadChunkOfFileContents(RFile &aTextFile);
	void WriteNewLine();
private:
    CConsoleBase* iConsole;				// Pointer to the console window
	TBool iConsoleExists;
    TFileName iFileName;				// Hmmm...
	TUint iFilePos;			   			// Current position within file
	CBufSeg * iTextBuf;					// Buffer for text file to be read into
	TBuf<KFileBufSize> iFileBuf;		// Buffer for reading file in
	};


class CParser : public CBase
	{ // Loads a segmented buffer full of text and parses it as PML, converting it into RichText
public:
	static CParser* NewL();
	~CParser();

	CRichText* ParseL(const TFileName &aFileName);	// no GDI - Filename supplied
	CRichText* ParseL(CConsoleBase* aConsole);	// with E32 console interface
protected:
	CParser();
	void ConstructApplicationL();
private:
	void ParseTextBufL();
	TBool Validate();					
	TChar ReadChar();
	void ProcessTextL(TChar aChar);
	void AddCharToParaL(TChar aChar);

	TChar ReadTagChar();					// Tag methods
	void ProcessTagL();						//
	void ClassifyTagL(TChar aTagChar);		//
	void AppendToArgBuf(TChar aTagChar);	//
	void ProcessArgBufL();					//
	void ClassifyArgumentsL();				//
	void TranslateTagArgL();					//
	void TransGlobalArgL();					//
	void TransParagraphArgL();			   	//
	void TransCharArg();					//
	void TransControlArgL();				//
	TInt GetArgValue();						//
	TInt GetArgValue(const TDes &aBuf);		//
	TBool BufIsNumeric(const TDes &aBuffer);//

	void TransParaDefault();				// Argument translators for paragraph
	void TransParaArgAlignment();			// 
	void TransParaArgLeftMargin();			//
	void TransParaArgRightMargin();			//
	void TransParaArgIndent();				//
	void TransParaLineSpacing();			//
	void TransParaArgLineSpacingControl();	//
	void TransParaSpaceBefore();			//
	void TransParaSpaceAfter();				//
	void TransParaKeepTogether();			//
	void TransParaKeepWithNext();			//
	void TransParaStartNewPage();			//
	void TransParaWidowOrphan();			//
	void TransParaBorderMargin();			//
	void TransParaBorderL();					//
	void TransParaTabWidth();				//
	void TransParaTabStopL();				// 
	void TransParaBullet();					//
	void TransCharDefault();				// ... and character
	void TransCharPosture();				//
	void TransCharStrokeWeight();			//
	void TransCharUnderline();				//
	void TransCharStrikethrough();			//
	void TransCharFontHeight();				//
	void TransCharPrintPos();				//
	void TransCharTypefaceName();			//
	void TransCharTypefaceFlags();			//
	void TransCharColor();					//
	void TransCharLanguage();				//

	void SetFormatLayerL();					// RT doc methods

	void EmptyBuffer(TDes &aBuf);
	void OutputToScreen(const TDesC& aMessageBuffer);
	void WriteNewLine() {OutputToScreen(NEWLINE);}
	void EmitErrorMessage();
public:
	TFileName iFileName;	   
private:
	TBuf8<512> SquashBuf(TDes aBuffer);
	//
	enum {						// Error types
		ENoError,
		EUnparagraphedText,
		EUnknownTagType,
		EUnknownAttrib,
		ENoAttribValue,
		EIllegalAttribValue
		};
	enum { 						// Possible types of tag
		EGlobal, 
		EParagraph, 
		ECharacter, 
		EControl,
		EComment, 
		EError};
	TUint iErrorLevel;
    CConsoleBase* iConsole;			// Pointer to the console window
	TBool iConsoleExists;
	CBufSeg * iTextBuf;					// Buffer for the text file to be parsed
	TUint iReadPos;						// Current read position in iTextBuf
	TUint iLineNo;						// Current line no. in iTextBuf (for error messages)
	TBool iParagraphIsOpen;				// Self explanatory?
	TBool iPhraseOpen;					// Character formatting is being applied
	TBool iArgValueExpected;			// Set if a tag argument requires a value eg =120
	TBool iArgStored;					// Set if there is an argument in temp storage
	TBool iCancelArg;					// Set if the arg has been canceled - ie turns formatting off
	TBool iBorderUsed;
	TBool iBulletUsed;					
	TInt iTagType;						// The type of tag: see enum above
	TBuf<32> iArgType;					// Buffer for tag arguments. Whole arg if they have no value 
	TBuf<32> iArgValue;					// Buffer for arg values... (32 bytes)  eg. <P ArgType=ArgValue>

	// Rich Text document variables
	CRichText* iRichTextDoc;			// The RT doc
	TInt iDocInsertPos;					// Cursor position in RT doc
	TUint iDocParaLength;				// Length of current paragraph
	TUint iDocPhraseLength; 			// ...
	CParaFormatLayer* iGlobalParaFormatLayer;
	TParaFormatMask iGlobalParaFormatMask;
	CParaFormat* iGlobalParaFormat;
	CCharFormatLayer* iGlobalCharFormatLayer;
	TCharFormatMask iGlobalCharFormatMask;
	TCharFormat iGlobalCharFormat;
	CParaFormatLayer* iParaFormatLayer;
	TParaFormatMask iParaFormatMask;
	CParaFormat* iParaFormat; 
	CCharFormatLayer* iCharFormatLayer;
	TCharFormatMask iCharFormatMask;
	TCharFormat iCharFormat;
	TParaBorder* iBorder;
	TBullet* iBullet;
	};

#endif
