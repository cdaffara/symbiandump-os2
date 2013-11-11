// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <e32def.h>
#include <f32file.h>

#include <ecom/test_bed/datalogger.h>

// KLogBufferSize is the maximum line length allowed by flogger
const TInt KMaxTBLogEntrySize = KLogBufferSize;

// Common string literals for all output formats
_LIT(KTimeFormatStr,"%J%:1%T%:2%S%.%*C3");
// Above string will give time in format HH:MM:SS.SSS therefore max length = 12
const TInt KTimeBufLength = 12;

// Define some string literals for HTML formatting
_LIT(KHTMLDocumentStart,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"><HTML><HEAD><META NAME=\"robots\" CONTENT=\"noindex\"><META NAME=\"author\" CONTENT=\"Symbian RTestBed log generator\"><TITLE>");
// Insert title string here
_LIT(KHTMLContentStart,"</TITLE></HEAD><BODY><P><FONT=3>");
// Insert time string here
_LIT(KHTMLCommentStart,"<P>");
// Insert content here
_LIT(KHTMLCommentEnd,"</P>");
_LIT(KHTMLDocumentEnd,"</FONT></P></BODY></HTML>");

//___________________________________________________________________________
// Define the overflow handling classes for any log formatting methods
// Simply record the overflow...
//

NONSHARABLE_CLASS(TLogMessageOverflow) : public TDes16Overflow
	{
	public:
	
		TLogMessageOverflow();
	
		void Overflow(TDes16&);	
	
		TInt iError;
	};

TLogMessageOverflow::TLogMessageOverflow()
: TDes16Overflow(),
iError(KErrNone)
	{
	}

void TLogMessageOverflow::Overflow(TDes16&)
	{
	__DEBUGGER();
	iError = KErrOverflow;
	}


NONSHARABLE_CLASS(TLogMessageOverflow8) : public TDes8Overflow
	{
	public:
	
		TLogMessageOverflow8();
	
		void Overflow(TDes8&);	
	
		TInt iError;
	};

TLogMessageOverflow8::TLogMessageOverflow8()
: TDes8Overflow(),
iError(KErrNone)
	{
	}

void TLogMessageOverflow8::Overflow(TDes8&)
	{
	__DEBUGGER();
	iError = KErrOverflow;
	}

//___________________________________________________________________________

//___________________________________________________________________________

CDataLogger::~CDataLogger()
	{
	if(iLogOutput != NULL)
		{
		if(iLogStyle != EText && iLogFormat.iDocumentEnd)
			Log(iLogOutput, *(iLogFormat.iDocumentEnd));
		iLogOutput->Close();
		}

	if(iReportOutput != NULL)
		{
		if(iLogStyle != EText && iLogFormat.iDocumentEnd)
			Log(iReportOutput, *(iLogFormat.iDocumentEnd));
		iReportOutput->Close();
		}

	delete iFormatBuf;
	delete iDebug;
	delete iDefaultLogOutput;
	delete iDefaultReportOutput;
	}


CDataLogger::CDataLogger()
	{
	}

CDataLogger* CDataLogger::NewLC(TLoggingInfo* aLogInfo)
	{
	CDataLogger* self = new (ELeave) CDataLogger();
	CleanupStack::PushL(self);
	self->ConstructL(aLogInfo);
	return self;
	}

CDataLogger* CDataLogger::NewL(TLoggingInfo* aLogInfo)
	{
	CDataLogger* self = NewLC(aLogInfo);
	CleanupStack::Pop();
	return self;
	}


void CDataLogger::ConstructL(TLoggingInfo* aLogInfo)
	{
	iFormatBuf = HBufC::NewL(KMaxTBLogEntrySize);

	SetupLoggingL(aLogInfo);

	iLogOutput->OpenL();
	iReportOutput->OpenL();

	// Record additional information : Time
	TTime time;
	time.UniversalTime();
	TBuf<KTimeBufLength> timeBuf;
	time.FormatL(timeBuf,KTimeFormatStr);
	if(iLogStyle != EText)
		{
		// Use the log format
		Log(iLogOutput, *(iLogFormat.iDocumentStart));
		if(aLogInfo->iTitle)
			Log(iLogOutput, *(aLogInfo->iTitle));
		Log(iLogOutput, *(iLogFormat.iContentStart));
		Log(iLogOutput, timeBuf);

		Log(iReportOutput, *(iLogFormat.iDocumentStart));
		if(aLogInfo->iTitle)
			Log(iReportOutput, *(aLogInfo->iTitle));
		Log(iReportOutput, *(iLogFormat.iContentStart));
		Log(iReportOutput, timeBuf);
		}
	else
		{
		if(aLogInfo && aLogInfo->iTitle)
			{
			Log(iLogOutput, *(aLogInfo->iTitle));
			Log(iReportOutput, *(aLogInfo->iTitle));
			}
		Log(iLogOutput, timeBuf);
		Log(iReportOutput, timeBuf);
		}
	}


EXPORT_C void CDataLogger::DumpMemoryBlock(const TUint8* aAddress, TInt aLength)
	{
	const TInt KBytesPerRow = 16;
	const TInt KLowestAsciiPrint = 32;
	const TInt KHighestAsciiPrint = 127;

	// The required descriptors for outputting the data
	_LIT(KDumpLineStart, "%04x : ");
	_LIT(KDumpHexOutput, "%02x ");
	_LIT(KDumpHexBlank, "   ");
	_LIT(KDumpHexEnd, ": ");
	_LIT(KDumpCharOutput, "%c");
	_LIT(KDumpCharUnknown, ".");
	_LIT(KDumpCharBlank, " ");

	TPtrC8 theData(aAddress, aLength);

	// Iterate the supplied block of data in blocks of 16 bytes
	TInt pos = 0;
	TBuf<KMaxTBLogEntrySize> logLine;
	TBuf<KMaxTBLogEntrySize> anEntry;
	while (pos < theData.Length())
		{
		TInt offset = 0;

		anEntry.Format(KDumpLineStart, pos);
		logLine.Append(anEntry);

		// Hex output
		for (offset = 0; offset < KBytesPerRow; ++offset)
			{
			if (pos + offset < theData.Length())
				{
				TInt nextByte = theData[pos + offset];
				anEntry.Format(KDumpHexOutput, nextByte);
				logLine.Append(anEntry);
				}
			else
				{
				anEntry.Format(KDumpHexBlank);
				logLine.Append(anEntry);
				}
			}
			anEntry.Format(KDumpHexEnd);
			logLine.Append(anEntry);

		// Char output
		for (offset = 0; offset < KBytesPerRow; ++offset)
			{
			if (pos + offset < theData.Length())
				{
				TInt nextByte = theData[pos + offset];
				if ((nextByte >= KLowestAsciiPrint) && (nextByte <= KHighestAsciiPrint))
					{
					anEntry.Format(KDumpCharOutput, nextByte);
					logLine.Append(anEntry);
					}
				else
					{
					anEntry.Format(KDumpCharUnknown);
					logLine.Append(anEntry);
					}
				}
			else
				{
				anEntry.Format(KDumpCharBlank);
				logLine.Append(anEntry);
				}
			}

		//Log this line to the file
		if(iLogStyle != EText)
			{
			Log(iLogOutput, *(iLogFormat.iCommentStart));
			Log(iLogOutput, logLine);
			Log(iLogOutput, *(iLogFormat.iCommentEnd));
			}
		else
			Log(iLogOutput, logLine);

		logLine.Zero();

		// Advance to next segment of size 'KBytesPerRow'
		pos += KBytesPerRow;
		}

	}


EXPORT_C void CDataLogger::LogInformation(const TDesC16& aComment)
	{
	if(iLogStyle != EText)
		{
		Log(iLogOutput, *(iLogFormat.iCommentStart));
		Log(iLogOutput, aComment);
		Log(iLogOutput, *(iLogFormat.iCommentEnd));
		}
	else
		Log(iLogOutput, aComment);
	iDebug->Print(aComment);
	}

EXPORT_C void CDataLogger::LogInformation(const TDesC8& aComment)
	{
	// Create a wide descriptor to copy aComment into
	HBufC* message = HBufC::NewMax(aComment.Length());

	// If the allocation failed then do nothing
	if(message != NULL)
		{
		TPtr message16 = message->Des();

		message16.Copy(aComment);
		LogInformation(message16);
		delete message;
		}
	}

EXPORT_C void CDataLogger::LogInformationWithParameters(TRefByValue<const TDesC16> aFormat, ...)
	{
	// Prepare the message
	// coverity [var_decl]
       // VA_LIST is initialized in VA_START
	VA_LIST list;
	VA_START(list,aFormat);
	
	TPtr message = iFormatBuf->Des();

	// Catch the overflow if formatting
	TLogMessageOverflow overflowHandler;
	message.AppendFormatList(aFormat,list,&overflowHandler);
	VA_END(list);
	if(overflowHandler.iError == KErrNone)
		{
		// Ok formatted correctly so...
		// Wrap the logging level as the first parameter
		if(iLogStyle != EText)
			{
			Log(iLogOutput, *(iLogFormat.iCommentStart));
			Log(iLogOutput, message);
			Log(iLogOutput, *(iLogFormat.iCommentEnd));
			}
		else
			Log(iLogOutput, message);
		iDebug->Print(message);
		}

	// Clear the message buffer
	message.Zero();
	}

EXPORT_C void CDataLogger::LogInformationWithParameters(TRefByValue<const TDesC8> aFormat, ...)
	{
	// Create an 8 bit descriptor to copy aFormat into
	HBufC8* message8 = HBufC8::New(KMaxTBLogEntrySize);
	if(message8)
		{
		// Prepare the message
		// coverity [var_decl]
	       // VA_LIST is initialized in VA_START
		VA_LIST list;
		VA_START(list,aFormat);
		TPtr8 messagePtr8 = message8->Des();

		// Catch the overflow if formatting
		TLogMessageOverflow8 overflowHandler;
		messagePtr8.AppendFormatList(aFormat,list,&overflowHandler);
		VA_END(list);
		if(overflowHandler.iError == KErrNone)
			{
			TPtr message = iFormatBuf->Des();
			// Copy over the fromatted message into the 16 bit descriptor
			message.Copy(messagePtr8);

			// Ok formatted correctly so...
			// Wrap the logging level as the first parameter
			if(iLogStyle != EText)
				{
				Log(iLogOutput, *(iLogFormat.iCommentStart));
				Log(iLogOutput, message);
				Log(iLogOutput, *(iLogFormat.iCommentEnd));
				}
			else
				Log(iLogOutput, message);
			iDebug->Print(message);

			// Clear the message buffer
			message.Zero();
			}
		delete message8;
		}
	}

EXPORT_C void CDataLogger::ReportInformation(const TDesC& aComment)
	{
	if(iLogStyle != EText)
		{
		Log(iReportOutput, *(iLogFormat.iCommentStart));
		Log(iReportOutput, aComment);
		Log(iReportOutput, *(iLogFormat.iCommentEnd));
		}
	else
		Log(iReportOutput, aComment);
	}

EXPORT_C void CDataLogger::ReportInformationWithParameters(TRefByValue<const TDesC> aFormat, ...)
	{
	// Prepare the message
	// coverity [var_decl]
       // VA_LIST is initialized in VA_START
	VA_LIST list;
	VA_START(list,aFormat);
	
	TPtr message = iFormatBuf->Des();

	// Catch the overflow if formatting
	TLogMessageOverflow overflowHandler;
	message.AppendFormatList(aFormat,list,&overflowHandler);
	VA_END(list);
	if(overflowHandler.iError == KErrNone)
		{
		// Ok formatted correctly so...
		// Wrap the logging level as the first parameter
		if(iLogStyle != EText)
			{
			Log(iReportOutput, *(iLogFormat.iCommentStart));
			Log(iReportOutput, message);
			Log(iReportOutput, *(iLogFormat.iCommentEnd));
			}
		else
			Log(iReportOutput, message);
		}

	// Clear the message buffer
	message.Zero();
	}


void CDataLogger::SetupRDebugL(TBool aRequest) 
	{
	delete iDebug;
	iDebug = 0;
	
	if(aRequest) 
		iDebug = new(ELeave) TDebugPrint;		// Print to RDebug
	else 
		iDebug = new(ELeave) TNullDebugPrint;	// Ignore prints
	}

void CDataLogger::TDebugPrint::Print(const TDesC& aMessage) 
	{
	_LIT(KRDebugFormatStr,"%S");
	RDebug::Print(KRDebugFormatStr, &aMessage);
	}

void CDataLogger::Log(MLogOutput* aLogOutput, const TDesC16& aMessage)
	{
	// If the message is short enough then log it in one go
	if(aMessage.Length() < KMaxTBLogEntrySize)
		aLogOutput->Write(aMessage);
	else
		{
		// Start at the beginning and log out short blocks until finished
		TInt outIndex = 0;
		while(outIndex < aMessage.Length())
			{
			if((outIndex+KMaxTBLogEntrySize) > aMessage.Length())
				{
				aLogOutput->Write(aMessage.Right(aMessage.Length() - outIndex));
				outIndex = aMessage.Length();
				}
			else
				{
				// The -1 is required to ensure that the submessage is not too long
				TPtrC subMessage = aMessage.Mid(outIndex, KMaxTBLogEntrySize - 1);
				// Find the space nearest the end for a convenient break point
				TInt spaceLoc = subMessage.LocateReverse(TChar(' '));
				if(spaceLoc != KErrNotFound)
					outIndex = spaceLoc;
				else
					outIndex = KMaxTBLogEntrySize - 1;
				aLogOutput->Write(subMessage.Left(++outIndex));
				}
			}
		}
	}

void CDataLogger::SetupLoggingL(TLoggingInfo* aLogInfo)
	{
	// The possible log filenames
	_LIT(KTestBedLogName, "RTestBed.log");
	_LIT(KTestBedHtmlLogName, "TestBedLog.html");
	// The possible report file names
	_LIT(KTestBedReportName, "RTestBed.rep");
	_LIT(KTestBedHtmlReportName, "TestBedReport.html");

	if(aLogInfo)
		{
		iLogStyle = aLogInfo->iStyle;
		
		if(aLogInfo->iLogOutput)
			iLogOutput = aLogInfo->iLogOutput;
		else
			{
			if(iLogStyle==EHtml)
				iDefaultLogOutput = new(ELeave) CDefaultLogOutput(KTestBedHtmlLogName);
			else
				iDefaultLogOutput = new(ELeave) CDefaultLogOutput(KTestBedLogName);

			iLogOutput = iDefaultLogOutput;
			}

		if(aLogInfo->iReportOutput)
			iReportOutput = aLogInfo->iReportOutput;
		else
			{
			if(iLogStyle==EHtml)
				iDefaultReportOutput = new(ELeave) CDefaultLogOutput(KTestBedHtmlReportName);
			else
				iDefaultReportOutput = new(ELeave) CDefaultLogOutput(KTestBedReportName);

			iReportOutput = iDefaultReportOutput;
			}

		SetupRDebugL(aLogInfo->iUseRDebug);
		}
	else
		{
		iLogStyle = EText;
		iDefaultLogOutput = new(ELeave) CDefaultLogOutput(KTestBedLogName);
		iLogOutput = iDefaultLogOutput;
		iDefaultReportOutput = new(ELeave) CDefaultLogOutput(KTestBedReportName);
		iReportOutput = iDefaultReportOutput;

		SetupRDebugL(ETrue);
		}

	// If the user has specified a custom logging style then use their LogFormat
	if(iLogStyle == ECustom)
		iLogFormat = *(aLogInfo->iLogFormat);
	else if(iLogStyle == EHtml)
		{
		// Output as HTML
		iLogFormat.iDocumentStart	= &(KHTMLDocumentStart());
		iLogFormat.iContentStart	= &(KHTMLContentStart());
		iLogFormat.iCommentStart	= &(KHTMLCommentStart());
		iLogFormat.iCommentEnd		= &(KHTMLCommentEnd());
		iLogFormat.iDocumentEnd		= &(KHTMLDocumentEnd());
		}
	}
