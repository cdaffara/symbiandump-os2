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
* Implementation of the classes that import and export plain text.
*
*/


#include "TXTSTD.H"
#include "TXTPLAIN.H"
#include "charconv.h"

TPlainTextIOState::	TPlainTextIOState(const CPlainText::TImportExportParam& aParam,
									  CPlainText::TImportExportResult& aResult,
									  RWriteStream& aOutput,RReadStream& aInput):
	iParam(aParam),
	iResult(aResult),
	iOutput(aOutput),
	iInput(aInput),
	iConverter(NULL),
	iSwapInput(FALSE),
	iCheckByteOrder(FALSE)
	{
	aResult = CPlainText::TImportExportResult();  // zero output counters; aResult may be re-used.
	}

TText TPlainTextIOState::ReadRawCharL()
	{
	TText c;
	if (iParam.iInputInternal)
		iInput.ReadL((TUint8*)&c,sizeof(TText));
	else
		c = iInput.ReadUint16L();
	if (iSwapInput)
		c = (TText)(((c << 8) & 0xFF00) | ((c >> 8) & 0xFF));
	if (iCheckByteOrder)
		{
		if (c == CEditableText::EReversedByteOrderMark)
			{
			c = CEditableText::EByteOrderMark;
			iSwapInput = !iSwapInput;
			}
		iCheckByteOrder = FALSE;
		}
	iResult.iInputChars++;
	return c;
	}

void TPlainTextIOState::WriteRawCharL(TText aChar)
	{
	if (iResult.iOutputChars < iParam.iMaxOutputChars)
		{
		if (iParam.iOutputInternal)
			iOutput.WriteL((TUint8*)&aChar,sizeof(TText));
		else
			iOutput.WriteUint16L(aChar);
		iResult.iOutputChars++;
		}
	}

CPlainTextConverter* CPlainTextConverter::NewLC()
	{
	CPlainTextConverter* c = new(ELeave) CPlainTextConverter;
	CleanupStack::PushL(c);
	c->iConversionBuffer = new(ELeave) TUint8[EConversionBufferSize];
	return c;
	}

CPlainTextConverter::~CPlainTextConverter()
	{
	delete iConverter;
	delete [] iConversionBuffer;
	}

/*
Prepare to convert between Unicode and a foreign encoding.
If aSample is non-null it can be used to guess the foreign encoding, but only if iParam.iGuessForeignEncoding is true.
*/
void CPlainTextConverter::PrepareToConvertL(TPlainTextIOState& aState,const TDesC8* aSample)
	{
	RFs rfs;

	iConverter = CCnvCharacterSetConverter::NewL();
	if (aState.iParam.iFileSession == NULL)
		{
		TInt error = rfs.Connect();
		User::LeaveIfError(error);
				
		CleanupClosePushL(rfs);
		}
	else
		rfs = *aState.iParam.iFileSession;

	TUint foreign_encoding = aState.iParam.iForeignEncoding;
	
	// Try to guess the foreign encoding.
	if (aSample && aState.iParam.iGuessForeignEncoding)
		{
		CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* charsets =
			CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(rfs);
		TInt confidence = 0;
		CCnvCharacterSetConverter::AutoDetectCharacterSetL(confidence,foreign_encoding,*charsets,*aSample);
		CleanupStack::PopAndDestroy(charsets);
		if (confidence < 50)
			User::Leave(KErrNotSupported);
		}

	if (iConverter->PrepareToConvertToOrFromL(foreign_encoding,rfs) != CCnvCharacterSetConverter::EAvailable)
		User::Leave(KErrNotSupported);
	aState.iResult.iForeignEncoding = foreign_encoding;
	if (aState.iParam.iFileSession == NULL)
		{
		CleanupStack::Pop(); // rfs
		rfs.Close();
		}
	}

void TPlainTextWriter::TranslateL(const CPlainText::TImportExportParam& aParam,CPlainText::TImportExportResult& aResult,
								  RWriteStream& aOutput,RReadStream& aInput)
	{
	TPlainTextWriter writer(aParam,aResult,aOutput,aInput);
	writer.TranslateHelperL();
	}

TPlainTextWriter::TPlainTextWriter(const CPlainText::TImportExportParam& aParam,CPlainText::TImportExportResult& aResult,
								   RWriteStream& aOutput,RReadStream& aInput):
	TPlainTextIOState(aParam,aResult,aOutput,aInput),
	iLineLength(0),
	iLineBuffer(NULL),
	iMaxLineBufferLength(0)
	{
	}

void TPlainTextWriter::TranslateHelperL()
	{
	if (iParam.iForeignEncoding)
		{
		iConverter = CPlainTextConverter::NewLC();
		iConverter->PrepareToConvertL(*this,NULL);
		}

	if (iParam.iOrganisation == CPlainText::EOrganiseByLine)
		iMaxLineLength = iParam.iMaxLineLength;
	else
		iMaxLineLength = KMaxTInt; // when exporting by paragraph, the wrapping width has no effect
	if (iMaxLineLength <= 0)
		iMaxLineLength = KMaxTInt;
	iLineLength = 0;
	if (iMaxLineLength < KMaxTInt)
		iMaxLineBufferLength = iMaxLineLength;
	else if (iParam.iForeignEncoding)
		iMaxLineBufferLength = EDefaultLineBufferSize;
	if (iMaxLineBufferLength)
		iLineBuffer = new(ELeave) TText[iMaxLineBufferLength];
	else
		iLineBuffer = NULL;
	CleanupStack::PushL(iLineBuffer);
	TRAPD(error,TranslateToEofL());
	if (error == KErrEof)
		error = KErrNone;
	if (error == KErrNone)
		{
		FlushL();
		iOutput.CommitL();
		}
	CleanupStack::Pop(iLineBuffer);
	delete [] iLineBuffer;
	if (iConverter)
		CleanupStack::PopAndDestroy(iConverter);
	User::LeaveIfError(error);
	}

void TPlainTextWriter::TranslateToEofL()
	{
	while (!Finished())
		{
		TText c = ReadRawCharL();
		switch (c)
			{
			// Write a CR-LF at a forced line break if organising by line.
			case CEditableText::ELineBreak:
				if (iParam.iOrganisation == CPlainText::EOrganiseByLine)
					{
					FlushL();
					WriteNewLineL();
					}
				else
					WriteCharL(c);
				break;

			// Write a CR-LF at the end of the paragraph, then an extra one if lines are split by CR-LFs.
			case CEditableText::EParagraphDelimiter:
				FlushL();
				WriteNewLineL();
				if (iParam.iOrganisation == CPlainText::EOrganiseByLine)
					WriteNewLineL();
				break;

			default:
				WriteCharL(c);
			}
		}
	}

void TPlainTextWriter::FlushL()
	{
	if (iLineBuffer)
		WriteAndConvertL(iLineBuffer,iLineLength);
	iLineLength = 0;
	}

void TPlainTextWriter::WriteCharL(TText aChar)
	{
	if (iLineBuffer)
		{
		if (iLineLength >= iMaxLineBufferLength)
			{
			int linebreak = iMaxLineBufferLength;
			int stripped_linebreak = iMaxLineBufferLength;

			if (iLineLength >= iMaxLineLength)
				{
				for (linebreak = iMaxLineLength; linebreak > 0; linebreak--)
					if (iLineBuffer[linebreak - 1] == ' ')
						break;
				if (linebreak == 0)
					linebreak = iMaxLineLength;

				// Strip a single trailing space if any; it is added when text is imported.
				stripped_linebreak = linebreak;
				if (iLineBuffer[linebreak - 1] == ' ')
					stripped_linebreak = linebreak - 1;
				}

			WriteAndConvertL(iLineBuffer,stripped_linebreak);
			if (iLineLength >= iMaxLineLength)
				WriteNewLineL();
			int i = linebreak;
			int j = 0;
			while (i < iMaxLineBufferLength)
				iLineBuffer[j++] = iLineBuffer[i++];
			iLineLength = j;
			}
		iLineBuffer[iLineLength++] = aChar;
		}
	else
		WriteRawCharL(aChar);
	}

void TPlainTextWriter::WriteNewLineL()
	{
	WriteAndConvertL(_S("\x0d\x0a"),2);
	}

void TPlainTextWriter::WriteAndConvertL(const TText* aText,TInt aLength)
	{
	if (iConverter)
		{
		while (aLength > 0)
			{
			TPtrC source(aText,aLength);
			TPtr8 dest(iConverter->iConversionBuffer,CPlainTextConverter::EConversionBufferSize);
			int remainder = iConverter->iConverter->ConvertFromUnicode(dest,source);
			if (remainder < 0)
				User::Leave(KErrCorrupt);
			int available = iParam.iMaxOutputChars - iResult.iOutputChars;
			if (available < dest.Length())
				dest.SetLength(available);
			if (dest.Length() > 0)
				{
				iOutput.WriteL(dest);
				iResult.iOutputChars += dest.Length();
				}
			int converted = aLength - remainder;
			aText += converted;
			aLength -= converted;
			}
		}
	else
		{
		while (aLength-- > 0)
			WriteRawCharL(*aText++);
		}
	}

TPlainTextReader::TPlainTextReader(const CPlainText::TImportExportParam& aParam,CPlainText::TImportExportResult& aResult,
								   RWriteStream& aOutput,RReadStream& aInput):
	TPlainTextIOState(aParam,aResult,aOutput,aInput),
	iInputBuffer(NULL),
	iInputLength(0),
	iInputPos(0),
	iConversionState(CCnvCharacterSetConverter::KStateDefault)
	{
	iCheckByteOrder = TRUE;
	}

void TPlainTextReader::TranslateL(const CPlainText::TImportExportParam& aParam,CPlainText::TImportExportResult& aResult,
								  RWriteStream& aOutput,RReadStream& aInput)
	{
	TPlainTextReader reader(aParam,aResult,aOutput,aInput);
	if(reader.iParam.iOrganisation == CPlainText::EOrganiseByLine)
		{
		TLineTextWriter txtWriter(reader);
		TSLBTransaltor slbTranslator(txtWriter);
		reader.TranslateHelperL(slbTranslator);
		}
	else
		{
		TParagraphTextWriter txtWriter(reader);
		TSLBTransaltor slbTranslator(txtWriter);
		reader.TranslateHelperL(slbTranslator);
		}
	}

void TPlainTextReader::TranslateHelperL(TSLBTransaltor& aSLBTranslator)
	{
	if (iParam.iForeignEncoding || iParam.iGuessForeignEncoding)
		{
		iConverter = CPlainTextConverter::NewLC();
		iInputBuffer = new(ELeave) TText[EInputBufferSize];
		CleanupStack::PushL(iInputBuffer);
		}
	else
		iInputBuffer = NULL;
	TRAPD(error,TranslateToEofL(aSLBTranslator));
	if (error == KErrEof)
		error = KErrNone;
	if (error == KErrNone)
		iOutput.CommitL();
	if (iConverter)
		{
		CleanupStack::Pop(iInputBuffer);
		delete [] iInputBuffer;
		CleanupStack::PopAndDestroy(iConverter);
		}
	User::LeaveIfError(error);
	}

void TPlainTextReader::TranslateToEofL(TSLBTransaltor& aSLBTranslator)
	{
	while(!Finished())
		{
		TText c = ReadAndConvertL();
		aSLBTranslator.ProcessL(c);
		}
	aSLBTranslator.FlushL();
	}

TText TPlainTextReader::ReadAndConvertL()
	{
	// Read EConversionBufferSize bytes into a conversion buffer (iConversionBuffer).
	// Using CharConv convert this into unicode and place in a destination buffer (dest).
	// This may result in some bytes that cannot be converted (remainder) as that
	// character encoding is truncated.
	// This remainder is then moved to the begining of the conversion buffer and more
	// data read in after it, in effect untruncating that last character.
	// Before this next read takes place the next converted unicode character is returned
	// until the destination buffer positional pointers reach the end where more data is
	// required for processing.
	//  
	if (iConverter && iInputBuffer)
		{
		if (iInputPos >= iInputLength)
			{
			/*
			Attempt to read more foreign characters if there are less than 20,
			which is the current maximum length of a multibyte character sequence for CHARCONV.
			Use MStreamBuf::ReadL, which doesn't leave on EOF, rather than RReadStream::ReadL,
			which does, and doesn't tell you how much was actually read.
			*/
			if (iConverter->iConversionBufferLength < 20)
				iConverter->iConversionBufferLength +=
					iInput.Source()->ReadL(iConverter->iConversionBuffer + iConverter->iConversionBufferLength,
										   CPlainTextConverter::EConversionBufferSize - iConverter->iConversionBufferLength);

			// Create the converter late so we have a sample of foreign text for auto-detection of the encoding.
			if (!iConverter->iConverter)
				{
				TPtrC8 sample(iConverter->iConversionBuffer,iConverter->iConversionBufferLength);
				iConverter->PrepareToConvertL(*this,&sample);
				}

			// Translate from the foreign encoding to Unicode.
			TPtr dest(iInputBuffer,0,EInputBufferSize);
			TPtrC8 source(iConverter->iConversionBuffer,iConverter->iConversionBufferLength);
			int remainder = iConverter->iConverter->ConvertToUnicode(dest,source,iConversionState);
			if (remainder < 0)
				User::Leave(KErrCorrupt);

			// Move the remaining foreign characters if any to the start of the buffer
			// so that on the next read it can be joined with its truncated part.
			for (int i = 0, j = iConverter->iConversionBufferLength - remainder; i < remainder; ++i, ++j)
				iConverter->iConversionBuffer[i] = iConverter->iConversionBuffer[j];
			iConverter->iConversionBufferLength = remainder;

			iInputPos = 0;
			iInputLength = dest.Length();
			if (iInputLength == 0)
				User::Leave(KErrEof);
			}
		iResult.iInputChars++;
		return iInputBuffer[iInputPos++];
		}
	else
		return ReadRawCharL();
	}

/**
The method processes the imput characters, writing them to the output, but skipping 
the picture characters (CEditableText::EPictureCharacter).
The method is not called directly and should not be called. It implements 
MOutputChar::OutputCharL(TChar aChar) and is called from TParagraphTextWriter and 
TLineTextWriter implementations.
@param aChar Character to be processed.
*/
void TPlainTextReader::OutputCharL(TText aChar)
	{
	switch(aChar)
		{
		case CEditableText::EByteOrderMark :
			// leading byte order marks are ignored
			if(iResult.iInputChars > 1)
				{
				WriteRawCharL(aChar);
				}
			break;
		case CEditableText::EPictureCharacter:
			//Picture characters are ignored because they would cause ETEXT to panic when it attempted to find
			//the picture corresponding to the character.
			break;
		default:
			WriteRawCharL(aChar);
			break;
		}
	}

