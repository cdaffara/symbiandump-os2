/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Header for the classes that import and export plain text into and out of ETEXT.
*
*/


#ifndef __TXTPLAIN__
#define __TXTPLAIN__ 1

#include <e32std.h>
#include <s32mem.h>
#include <txtetext.h>
#include "TxtWriter.h"

class CCnvCharacterSetConverter;
class TPlainTextIOState;

NONSHARABLE_CLASS(CPlainTextConverter) : public CBase
/**
@internalComponent
*/
	{
	public:
	static CPlainTextConverter* NewLC();
	~CPlainTextConverter();
	void PrepareToConvertL(TPlainTextIOState& aState,const TDesC8* aSample);

	enum
		{
		EConversionBufferSize = 1024
		};

	CCnvCharacterSetConverter* iConverter;	// use this converter to translate input or output
	TUint8* iConversionBuffer;				// the buffer used by iConverter for non-Unicode text
	TInt iConversionBufferLength;			// current number of characters in the conversion buffer
	};

class TPlainTextIOState
/**
@internalComponent
*/
	{
	public:
	TPlainTextIOState(const CPlainText::TImportExportParam& aParam,CPlainText::TImportExportResult& aResult,
					  RWriteStream& aOutput,RReadStream& aInput);
	TBool Finished() const
		{ return iResult.iOutputChars >= iParam.iMaxOutputChars || iResult.iInputChars >= iParam.iMaxInputChars; }
	TText ReadRawCharL();
	void WriteRawCharL(TText aChar);

	const CPlainText::TImportExportParam& iParam;
	CPlainText::TImportExportResult& iResult;
	RWriteStream& iOutput;
	RReadStream& iInput;
	CPlainTextConverter* iConverter;// if non-null, use CHARCONV, via this object, to convert a
									// foreign encoding to or from Unicode
	TBool iSwapInput;				// input needs to be byte-swapped
	TBool iCheckByteOrder;			// flip iSwapInput if the next input character is 0xFFFE (byte-swapped byte order mark)
	};

class TPlainTextWriter: public TPlainTextIOState
/**
@internalComponent
*/
	{
	public:
	static void TranslateL(const CPlainText::TImportExportParam& aParam,CPlainText::TImportExportResult& aResult,
						   RWriteStream& aOutput,RReadStream& aInput);

	private:
	TPlainTextWriter(const CPlainText::TImportExportParam& aParam,CPlainText::TImportExportResult& aResult,
					 RWriteStream& aOutput,RReadStream& aInput);
	void TranslateHelperL();

	enum
		{
		EDefaultLineBufferSize = CPlainTextConverter::EConversionBufferSize * 2
		};

	void TranslateToEofL();
	void FlushL();
	void WriteCharL(TText aChar);
	void WriteNewLineL();
	void WriteAndConvertL(const TText* aText,TInt aLength);

	RBufReadStream iBufReadStream;	// a stream over the input buffer
	TInt iMaxLineLength;			// wrap lines automatically if they exceed this length
	TInt iLineLength;				// length of the current line
	TText* iLineBuffer;				// if non-null, this buffer contains the current line, or Unicode text
									// to be converted to a foreign encoding
	TInt iMaxLineBufferLength;		// maximum number of characters in iLineBuffer
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(TPlainTextReader) : public TPlainTextIOState, public MOutputChar
	{
	public:
	static void TranslateL(const CPlainText::TImportExportParam& aParam,CPlainText::TImportExportResult& aResult,
						   RWriteStream& aOutput,RReadStream& aInput);

	private:
	TPlainTextReader(const CPlainText::TImportExportParam& aParam,CPlainText::TImportExportResult& aResult,
					 RWriteStream& aOutput,RReadStream& aInput);
	void TranslateHelperL(TSLBTransaltor& aSLBTranslator);

	enum
		{
		EInputBufferSize = CPlainTextConverter::EConversionBufferSize * 2
		};

	void TranslateToEofL(TSLBTransaltor& aSLBTranslator);
	TText ReadAndConvertL();
	virtual void OutputCharL(TText aChar);

	TText* iInputBuffer;				// if non-null, this buffer contains Unicode text
										// converted from a foreign encoding
	TInt iInputLength;					// number of characters currently stored in iInputBuffer
	TInt iInputPos;						// current position in iInputBuffer;
	TInt iConversionState;				// state used by iConverter when converting to Unicode
	};

#endif // __TXTPLAIN__
