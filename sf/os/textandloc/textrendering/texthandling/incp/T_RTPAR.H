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


#ifndef __T_RTPAR_H__
#define	__T_RTPAR_H__

#include <E32TWIN.H>
#include <TXTRICH.H>

// classes defined in this header
class CRichTextReader;
class CPMLWriter;
//

enum TTagType {		   // Possible types of tag
	EGlobal, 
	EParagraph, 
	ECharacter , 
	EControl
	}; 

enum TTagStatus {	 
	ETagStart,		  // "<"
	ETagEnd			  // ">"
	};


class CRichTextReader : public CBase
	{
public:
	static CRichTextReader* NewL();
	void Destruct();

	// these functions convert a supplied RichText doc into PML 
	// which is contained in the returned buffer.
	CBufSeg* ConvertRichText(CRichText* aRichTextDoc, RConsole aConsole);
	CBufSeg* ConvertRichText(CRichText* aRichTextDoc);
protected:
	CRichTextReader();
	void ConstructL();
private:
	void SenseGlobalFormat();
	TInt TranslatePhrase(TInt aPos);	
	void CompareParaFormats();
	void CompareCharFormats();
	void CompareParaToFactory(CParaFormat* aSensed);  
	void CompareCharToFactory(TCharFormat aSensed);
	void CheckTabList(CParaFormat* aFormatOne, CParaFormat* aFormatTwo);
	TParaBorder* CopyBorderL(const TParaBorder* aFrom,TParaBorder* aTo);
	TBullet* CopyBulletL(const TBullet* aFrom,TBullet* aTo);
private:
	CRichText* iRichTextDoc;
	CParaFormat* iThisParaFormat;
	CParaFormat* iPrevParaFormat;
	TCharFormat iThisCharFormat;
	TCharFormat iPrevCharFormat;
	TParaBorder* iBorder;
				
	RConsole iConsole;
	TBool iConsoleExists;				
	CPMLWriter* iPMLWriter;
	TInt iParaStart;				// Start pos of current paragraph
	TInt iDocLength;				// length in chars of rich text doc
	};


class CPMLWriter : public CBase
	{
public:
	static CPMLWriter* NewL();
	void Destruct();

	void Insert(const TDesC& aBuf);	// inserts a string into PML doc
	void Delete(TUint aNumberOfCharsToDelete);	// deletes chars prior to current insert pos
	void Output(RConsole aConsole); // output PML doc to screen
	inline CBufSeg* ReturnPmlDoc() {return iTextBuf;}
	inline TUint WritePos() {return iInsertPos;}
	void SetTag(TTagType, TTagStatus);	// Prints tag delimiters & tag type
	void SetTab(TTabStop aTabStop);
	void SetFormat(TTextFormatAttribute aAttribute, TInt aValue);
	void SetFormat(TTextFormatAttribute aAttribute, TParaBorder* aParaBorder);
	void SetFormat(TTextFormatAttribute aAttribute, const TTypeface &aTypeface);
	void SetFormat(TTextFormatAttribute aAttribute, TBullet* aBullet);
	void SetFormat(TTextFormatAttribute aAttribute, TTabStop aTabStop);
protected:
	CPMLWriter();
	void ConstructL();
private:
	void ProcessAlignment(TInt aValue);
	void ProcessLineSpacingControl(TInt aValue);
	void ProcessFontPrintPos(TInt aValue);
	void ProcessBooleanAtt(TTextFormatAttribute aAttribute, TInt aValue);
	void ProcessBorder(TTextFormatAttribute aAttribute, TInt aValue);
	void SquashBuf(const TDesC& aBuffer, TDes8& aTarget); // squashes 16 bit buffers to 8 bit (for unicode)
	void ExpandBuf(const TDes8& aBuffer, TDes& aTarget);
private:
	CBufSeg* iTextBuf;					// Buffer for text file to be read into
	TUint iInsertPos;
	};



#endif
