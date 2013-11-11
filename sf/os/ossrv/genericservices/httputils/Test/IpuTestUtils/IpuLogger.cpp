// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// inulogger.cpp
// 
//

#include <f32file.h> // for TParse, in Flogger stuff


#include "InuLogger.h"

#define KLogsDir				_L("c:\\logs\\")
#define KInuLogsDirName			_L("InetUtils")
#define KTestHeader				_L("Inet Utils Log")
#define KTestCommentPrepend		_L("\t")

const TInt KMaxLogLineLength = 128;

EXPORT_C
TInuLogger::~TInuLogger()
	{
#if defined (_DEBUG)
	iLogger.Close();
#endif
	}

EXPORT_C
#if defined (_DEBUG)
void TInuLogger::CreateFlogger(const TDesC& aFileName, TInt aShowDate, TInt aShowTime)
//
//	Create log file in directory KLogsdir\KWapLogsDirName - Note: ingore Drive and Path of aFilename
	{
	iLogger.Connect();
	TParse p;
	p.Set(aFileName, NULL, NULL);
	iLogger.CreateLog(KInuLogsDirName, p.NameAndExt(), EFileLoggingModeOverwrite);
	iLogger.SetDateAndTime(aShowDate, aShowTime);
	iLogger.Write(KTestHeader);
	}
#else if !defined (_DEBUG)
void TInuLogger::CreateFlogger(const TDesC& , TInt , TInt ) {}
#endif

EXPORT_C
#if defined (_DEBUG)
void TInuLogger::LogIt(TRefByValue<const TDesC> aFmt, ...)
//
//	Messages to the front end emulator and to the log file
	{
	VA_LIST list;
	VA_START(list,aFmt);

	TBuf<KMaxFileName> buf;
	buf.Append(KTestCommentPrepend);
	buf.AppendFormatList(aFmt,list);
	VA_END(list);

	WriteComment(buf);
	}
#else if !defined (_DEBUG)
void TInuLogger::LogIt(TRefByValue<const TDesC> , ...) {}
#endif



#if defined (_DEBUG)
EXPORT_C void TInuLogger::WriteComment(const TDesC& aComment)
//
//	Writes aComment to test log file, logging file and test harness
	{
	TPtrC line;
	line.Set(aComment);

	while (line.Length() > KMaxLogLineLength)
		{
		iLogger.Write(line.Left(KMaxLogLineLength));
		line.Set(line.Right(line.Length() - KMaxLogLineLength));
		}
	
	iLogger.Write(line.Left(line.Length()));
	
	}

#else if !defined (_DEBUG)
EXPORT_C void TInuLogger::WriteComment(const TDesC& ) {}
#endif



EXPORT_C
#if defined (_DEBUG)
void TInuLogger::DumpIt(const TDesC8& aData)
//Do a formatted dump of binary data
	{
	// Iterate the supplied block of data in blocks of cols=80 bytes
	const TInt cols=16;
	TInt pos = 0;
	TBuf<KMaxLogLineLength> logLine;
	TBuf<KMaxLogLineLength> anEntry;
	while (pos < aData.Length())
		{
		//start-line exadecimal( a 4 digit number)
		anEntry.Format(TRefByValue<const TDesC>_L("%04x : "), pos);
		logLine.Append(anEntry.Left(KMaxLogLineLength));

		// Hex output
		TInt offset;
		for (offset = 0; offset < cols; offset++)
			{
			if (pos + offset < aData.Length())
				{
				TInt nextByte = aData[pos + offset];
				anEntry.Format(TRefByValue<const TDesC>_L("%02x "), nextByte);
				logLine.Append(anEntry);
				}
			else
				{
				//fill the remaining spaces with blanks untill the cols-th Hex number 
				anEntry.Format(TRefByValue<const TDesC>_L("   "));
				logLine.Append(anEntry);
				}
			}
			anEntry.Format(TRefByValue<const TDesC>_L(": "));
			logLine.Append(anEntry);

		// Char output
		for (offset = 0; offset < cols; offset++)
			{
			if (pos + offset < aData.Length())
				{
				TInt nextByte = aData[pos + offset];
				if ((nextByte >= 32) && (nextByte <= 127))
					{
					anEntry.Format(TRefByValue<const TDesC>_L("%c"), nextByte);
					logLine.Append(anEntry);
					}
				else
					{
					anEntry.Format(TRefByValue<const TDesC>_L("."));
					logLine.Append(anEntry);
					}
				}
			else
				{
				anEntry.Format(TRefByValue<const TDesC>_L(" "));
				logLine.Append(anEntry);
				}
			}
			LogIt(TRefByValue<const TDesC>_L("%S\n"), &logLine);	
			logLine.Zero();

		// Advance to next  byte segment (1 seg= cols)
		pos += cols;
		}
	}
#else if !defined (_DEBUG)
void TInuLogger::DumpIt(const TDesC8& ) {}
#endif

EXPORT_C
#if defined (_DEBUG)
void TInuLogger::WriteComment(const TDesC8& aData)
//Do a write of the supplied data, literally where possible
	{
	TPtrC8 line;
	line.Set(aData);
	while (line.Length() > KMaxLogLineLength)
		{
		iLogger.Write(line.Left(KMaxLogLineLength));
		line.Set(line.Right(line.Length() - KMaxLogLineLength));
		}
	
	iLogger.Write(line.Left(line.Length()));
	}
#else if !defined (_DEBUG)
void TInuLogger::WriteComment(const TDesC8& ) {}
#endif

