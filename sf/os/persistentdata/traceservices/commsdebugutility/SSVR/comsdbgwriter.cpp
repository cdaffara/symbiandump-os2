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
// Implements the Flogger server side data writing
// 
//

/**
 @file
 @internalComponent
*/

#include "comsdbgwriter.h"
#include "comsdbgmessages.h"


const TInt KHeapBufSize = 50000;   // Maximum that the file buffer can grow to limit its impact somewhat


// RDebug will truncate strings longer than the limit below
const TInt KRDebugLimit = 0x100;


// source strings, lengths and offsets
_LIT8(KLogStartedString, "#Logging started on dd/mm/yyyy. Output version 2\r\n");	///< Format specifier for the very first line written. The output version is printed so tools processing the log output can determine the expected format of the output.
_LIT8(KDateChangedString,"#      Date is now: dd/mm/yyyy.\r\n");	///< Format specifier for subsequent date changes
const TInt KLogStartedStringLength = 50;
const TInt KLogStartedDayOffset = 20;
const TInt KLogStartedMonthOffset = 23;
const TInt KLogStartedYearOffset = 26;

_LIT8(KUnableToLog, "#Logs lost since log file couldn't be written to");

_LIT8(KClearLogString, "#Clear Log called by ");
const TInt KClearLogStringLength = 21;

_LIT8(KEolCharacters, "\r\n");
const TInt KEolCharactersLength = 2;

_LIT8(KLogPathTree,"\\");	
_LIT8(KLogFileExt,".");

//Below const is... log string + 2 tags + 8 for the thread id + 3 tabs + EOL characters.
const TInt KMaxFinalLogStringLength = KLogBufferSize+2*KMaxTagLength+8+3+KEolCharactersLength;

_LIT8(KBadMediaString, "#Bad media setting in ini file.\r\n");
_LIT8(KUnableToUpdateMedia, "#Unable to update logging media. Err code: %d\r\n");
_LIT8(KUnableToLoadSerialDevices, "#Unable to load the LDD or PDD required for serial. Err code: %d\r\n");
_LIT8(KUnableToOpenSerial, "#Unable to open the serial port. Err code: %d\r\n");
_LIT8(KUnableToSetSerialConfig, "#Could not set serial port configuration after opening port. Err code: %d\r\n");

// source characters
_LIT8(KTabCharacter, "\t");
_LIT8(KTypeIdentifierAscii, "a");
_LIT8(KTypeIdentifierBinary, "b");

// serial specs
const TBps KFloggerSerialRate = EBps115200;
const TInt KSerialRetryCount = 50;
const TInt KSerialTimeoutInMicroSecs = 100000;

// serial ports
const TInt KSerialPort1 = 0;
#if defined (__WINS__)
const TInt KSerialPort2OnEmulator = 1;
#else
const TInt KSerialPort2OnTarget = 2;
#endif

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV.PDD")
#define LDD_NAME _L("ECOMM.LDD")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif


// log file specs
_LIT(KLogFileName, "log.txt");
_LIT(KLogDefaultFilePath, "C:\\logs\\");


const TUint KZeroDate = 0;  ///< The day to count forward from when determining date roll.
const TUint KSecondsToWriteTimestampOnNoActivity = 5;

#if defined (__WINS__)
const TInt Win32DisplayStringLengthMax = 1024; 
#endif




CLogManager* CLogManager::NewL(MLogArrayAccess& aArrayAccess)
	{
	CLogManager* self = new(ELeave) CLogManager(aArrayAccess);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CLogManager::CLogManager(MLogArrayAccess& aArrayAccess)
: iArrayAccess(aArrayAccess), iTimeString(KTimeFormat)
{}

CLogManager::~CLogManager()
	{
	delete iLogger;
	}

void CLogManager::ConstructL()
/*
 * Set the initial media to file and write there until the server has told us
 * which media is in the ini file.
 * Write the log started and date message. 
 */
	{
	iLogger = CFileWriter::NewL();	//File writer by default
	iCurrentMediaSetting = KFileMedia;
	iLoggingEnabled = EFalse;
	}

TInt CLogManager::ThreadEntryPoint(TAny* aPtr)
	{
	MLogArrayAccess* arrayAccess = static_cast<MLogArrayAccess*> (aPtr);

	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (cleanupStack==NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err, CLogManager::DoRunThreadL(*arrayAccess));

	delete cleanupStack;

	return err;
	}

void CLogManager::DoRunThreadL(MLogArrayAccess& aArrayAccess)
	{
	CLogManager* self = CLogManager::NewL(aArrayAccess);
	self->DoStart();
	delete self;
	}

void CLogManager::DoStart()
/**
 * Second/consumer/slave/draining/dequeuer thread main loop.
 * @note Continuously takes the log queue top message and processes it. Blocks on the "Get" if no messages.
 * @note Completes when a shutdown message is processed from the queue. This message sets "iShutDown".
 */
	{
	CLogMessageBase * message = NULL;

	TInt ret = KErrNone;
	RThread::Rendezvous(ret);

	FOREVER
		{
		iArrayAccess.GetFirstMessageAndTakeOwnership(message);
		__ASSERT_ALWAYS(message, User::Panic(KFloggerServerPanic, ENullMessageInArray));
		message->Invoke(*this);
		delete message;
        // GetFirstMessage waits on the request semaphore, so to balance these each 
		// time a message is processed we must then signal the completion semaphore
		// when in synchronous mode
		iArrayAccess.SignalCompletionSemaphore();
		if (iShutDown)
			{
			break;
			}
		}
	}

void CLogManager::WriteDateIntoLog(TBool firstTime)
/**
 * Write the date straight to the log output
 * @param firstTime If ETrue then write a "log started" otherwise write a "date change" msg
 */
	{
	if (!iLoggingEnabled) 
	   {
	   return;
	   }
	//Put a date stamp for when we've started logging
	//Can't use TTime::FormatL since it gives unicode and we want narrow.
	TTime time;
	TBuf8<KLogStartedStringLength> logDateString;
	time.HomeTime();
	TDateTime dateTime(time.DateTime());
	if (firstTime)
		{
		logDateString.Copy(KLogStartedString);
		}
	else
		{
		logDateString.Copy(KDateChangedString);
		}
	TBuf8<2> holdingBuf;
	holdingBuf.NumFixedWidth(dateTime.Day()+1, EDecimal, 2);
	logDateString.Replace(KLogStartedDayOffset,2,holdingBuf);
	holdingBuf.NumFixedWidth(dateTime.Month()+1, EDecimal, 2);
	logDateString.Replace(KLogStartedMonthOffset,2,holdingBuf);
	TBuf8<4> holdingBuf2;
	holdingBuf2.NumFixedWidth(dateTime.Year(), EDecimal, 4);
	logDateString.Replace(KLogStartedYearOffset,4,holdingBuf2);
	iLogger->LogString(logDateString);
	}

void CLogManager::ClearLog(const TFullName& aName)
	{
	if (!iLoggingEnabled) 
	   {
	   return;
	   }
	TRAP_IGNORE(iLogger->ClearLogL());	//if there is an error there is nothing we can do.
	TBuf8<KMaxFullName+KClearLogStringLength+KEolCharactersLength> buf(KClearLogString);
	buf.Append(aName);
	buf.Append(KEolCharacters);
	iLogger->LogString(buf);
	WriteDateIntoLog(ETrue);
	iTicksSinceLastLog=0;
	}

void CLogManager::SetTimeL(const TTime& aTime)
	{
	if (!iLoggingEnabled) 
	   {
	   return;
	   }
	//check whether date has rolled and write it to log if necessary
	TTime currentMicrosecs;
	TTimeIntervalDays newDate;
	currentMicrosecs.HomeTime();
	TTime zeroDate = TTime(TInt64(KZeroDate));
	newDate = currentMicrosecs.DaysFrom(zeroDate);
	if (newDate > iCurrentDate)
		{
		WriteDateIntoLog(EFalse);
		iCurrentDate = newDate;
		}
	
	//Print the time to the log
	//Have to do this stuff manually since TTime only provides Unicode formating
	//which is not what we want.
	// We format this each second regardless of whether it is used so that it is ready for use
	// if a client logs since in normal use it would be rare for flogger not to log something each second.
	TDateTime dateTime(aTime.DateTime());
	TBuf8<2> holdingBuf;
	holdingBuf.NumFixedWidth(dateTime.Hour(), EDecimal, 2);
	iTimeString.Replace(KHourOffset,2,holdingBuf);
	holdingBuf.NumFixedWidth(dateTime.Minute(), EDecimal, 2);
	iTimeString.Replace(KMinuteOffset,2,holdingBuf);
	holdingBuf.NumFixedWidth(dateTime.Second(), EDecimal, 2);
	iTimeString.Replace(KSecondOffset,2,holdingBuf);
	if (iTicksSinceLastLog++ < KSecondsToWriteTimestampOnNoActivity)
		{
		iLogger->LogString(iTimeString);
		iLogger->FlushLogL();
		}
		
	}

void CLogManager::LogString(const TDesC8& aLogString, const TDesC8& aSubSystem, const TDesC8& aComponent, const TThreadId& aThreadId)
	{
	if (!iLoggingEnabled) 
	   {
	   return;
	   }
	if (iTicksSinceLastLog > KSecondsToWriteTimestampOnNoActivity)
		{
		iLogger->LogString(iTimeString);
		}
	iTicksSinceLastLog=0;

	TBuf8<KMaxFinalLogStringLength> buf;
	buf.Append(aSubSystem);
	buf.Append(KTabCharacter);
	buf.Append(aComponent);
	buf.Append(KTabCharacter);
	buf.Append(KTypeIdentifierAscii);
	buf.Append(KTabCharacter);
	buf.AppendNum(aThreadId, EHex);
	buf.Append(KTabCharacter);
	buf.Append(aLogString);
	// check whether last two bytes of string are eol chars, since some lines have cr/lf, some don't
	TPtr8 ptr(&buf[buf.Length()-2], 2, 2);
	if (ptr.Compare(KEolCharacters)!=0)
		{
		buf.Append(KEolCharacters);
		}
	iLogger->LogString(buf);
	}

void CLogManager::LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent)
	{
	iLogger->LogBinaryDump(aBinaryString, aSubSystem, aComponent);
	}

void CLogManager::LogComment(const TDesC8& aComment)
	{
	if (iTicksSinceLastLog > KSecondsToWriteTimestampOnNoActivity)
		{
		iLogger->LogString(iTimeString);
		}
	iTicksSinceLastLog=0;

	iLogger->LogString(aComment);
	}

void CLogManager::MediaUpdate(const TDesC8& aMediaSetting, const TBool aForceFlush, const TDesC8& aLogPathSetting)
	{
	TRAPD(err, DoMediaUpdateL(aMediaSetting,aForceFlush, aLogPathSetting));
	if (err!=KErrNone)
		{
		TBuf8<KMaxFinalLogStringLength> buf;
		buf.Format(KUnableToUpdateMedia,err);
		iLogger->LogString(buf);
		}
	}

void CLogManager::DoMediaUpdateL(const TDesC8& aMediaSetting,const TBool aForceFlush, const TDesC8& aLogPathSetting)
	{
	//We should NEVER have no media selected, media is set on construction
	//and there should always be something set up.
	__ASSERT_ALWAYS(iLogger, User::Panic(KFloggerServerPanic, ENoLoggingMediaSetUp));

	if (aMediaSetting.Length() == 0)
		{
		return;
		}
		
	// if the ini file has been opened, the media setting will be either the default
	// or some other string so we can allow logging. If we are only opening media now, output extra info.
	TTime currentMicrosecs;
	currentMicrosecs.HomeTime();
	iCurrentDate = currentMicrosecs.DaysFrom(TTime(TInt64(KZeroDate)));
	
	if (!iLoggingEnabled)
		{
		iLoggingEnabled = ETrue;
		WriteDateIntoLog(ETrue);
		}
	
	//Media update is a best effort, if it fails we keep
	//logging to the old media
	//If bad media in ini file, carry on with old media and post message in log

	if (aMediaSetting.CompareF(iCurrentMediaSetting)==0 && aLogPathSetting.CompareF(iLogPath) == 0)
		{
		//Media hasn't changed, so update flush status only
		// ForceFlushing as far as media is concerned only applies to file since 
		// serial has no such buffer which needs flushing.
		iLogger->SetForceFlush(aForceFlush);
		return;
		}
	if (!aMediaSetting.CompareF(KFileMedia))
		{
		CLoggerMediaBase* media = CFileWriter::NewL();
		delete iLogger;
		iLogger = media;
		// ForceFlushing only applies to file since serial has no such buffer
		iLogger->SetForceFlush(aForceFlush);
		if(aLogPathSetting.Length() != 0)
			{
			(static_cast<CFileWriter*>(iLogger))->SetLogPath(aLogPathSetting);
			WriteDateIntoLog(ETrue);
			}
		}
	else if (!aMediaSetting.CompareF(KSerial1Media))
		{
		// lots of things can go wrong when constructing the serial, so
		// we set it up in stages, and log here if an error
		// If we get an error in NewL - just let generic error given by
		// SetMedia cover it
		//CLoggerMediaBase* media = CSerialWriter::NewL();
		CSerialWriter* serial = CSerialWriter::NewL();

		TInt res = serial->LoadDevices();
		if (res != KErrNone)
			{
			TBuf8<KMaxFinalLogStringLength> buf;
			buf.Format(KUnableToLoadSerialDevices,res);
			iLogger->LogString(buf);
			delete serial;
			User::Leave(res);
			}
		res = serial->OpenPort(KSerialPort1);
		if (res != KErrNone)
			{
			TBuf8<KMaxFinalLogStringLength> buf;
			buf.Format(KUnableToOpenSerial,res);
			iLogger->LogString(buf);
			delete serial;
			User::Leave(res);
			}

		res = serial->SetPortConfig();
		if (res != KErrNone)
			{
			TBuf8<KMaxFinalLogStringLength> buf;
			buf.Format(KUnableToSetSerialConfig,res);
			iLogger->LogString(buf);
			delete serial;
			User::Leave(res);
			}
		CLoggerMediaBase* media = serial;
		delete iLogger;
		iLogger = media;
		WriteDateIntoLog(ETrue);
		}
	else if (!aMediaSetting.CompareF(KSerial2Media))
		{
		// lots of things can go wrong when constructing the serial, so
		// we set it up in stages, and log here if an error.
		// If we get an error in NewL - just let generic error given by
		// SetMedia cover it

		CSerialWriter* serial = CSerialWriter::NewL();

		TInt res = serial->LoadDevices();
		if (res != KErrNone)
			{
			TBuf8<KMaxFinalLogStringLength> buf;
			buf.Format(KUnableToLoadSerialDevices,res);
			iLogger->LogString(buf);
			delete serial;
			User::Leave(res);
			}

		#if defined (__WINS__)
		res = serial->OpenPort(KSerialPort2OnEmulator);
		#else
		res = serial->OpenPort(KSerialPort2OnTarget);
		#endif

		if (res != KErrNone)
			{
			TBuf8<KMaxFinalLogStringLength> buf;
			buf.Format(KUnableToOpenSerial,res);
			iLogger->LogString(buf);
			delete serial;
			User::Leave(res);
			}

		res = serial->SetPortConfig();
		if (res != KErrNone)
			{
			TBuf8<KMaxFinalLogStringLength> buf;
			buf.Format(KUnableToSetSerialConfig,res);
			iLogger->LogString(buf);
			delete serial;
			User::Leave(res);
			}
		CLoggerMediaBase* media = serial;
		delete iLogger;
		iLogger = media;
		WriteDateIntoLog(ETrue);
		}
	else if (!aMediaSetting.CompareF(KRDebugMedia))
		{
		CLoggerMediaBase* media = CRDebugWriter::NewL();;
		delete iLogger;
		iLogger = media;
		WriteDateIntoLog(ETrue);
		}
	else if (!aMediaSetting.CompareF(KOSTv2Media))
		{
		CLoggerMediaBase* media = COstv2Writer::NewL();;
		delete iLogger;
		iLogger = media;
		WriteDateIntoLog(ETrue);
		}
	else	//Bad setting in the media file, leave media as is and return.
		{
		iLogger->LogString(KBadMediaString);
		return;	
		}
	iCurrentMediaSetting = aMediaSetting;
	iLogPath = aLogPathSetting;
	}

void CLogManager::ShutDown()
	{
	iShutDown = ETrue;
	}

//////////////////////////////////////////////////////////////////////////////

void CFileWriter::LogString(const TDesC8& aString)
	{
	TPtr8 ptr(iHBuf->Des());
	if (ptr.Length()+aString.Length() <=KHeapBufSize)
		{
		ptr.Append(aString);
		}
	else
		{
		TRAPD(err, DoFlushBufferToFileL());
		if (err==KErrNone)
			{
			ptr.Zero();
			ptr.Append(aString);
			}
		else
			{
			ptr.Zero();
			ptr.Append(KUnableToLog);
			ptr.Append(aString);
			}
		}
	if (iForceBufferFlushAlways)
		{
		TRAPD(err, DoFlushBufferToFileL());
		ptr.Zero();
		if (err!=KErrNone)
			{
			ptr.Append(KUnableToLog);
			}
		}

	}

void CFileWriter::LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent)
	{
	TRAPD(err, DoLogBinaryDumpL(aBinaryString, aSubSystem, aComponent));
	if (err!=KErrNone)
		{
		LogString(KUnableToLog);
		}
	}

void CFileWriter::DoLogBinaryDumpL(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent)
/**
 * Place a chunk of raw binary into the log file
 * @post Opens the output file, flushes the write buffer and then places some tags before writing
 *   the binary data.
 */
	{
	RFile logFile;
	TInt err = logFile.Open(iFs, iLogFileName, EFileWrite|EFileShareAny);
	if(err == KErrPathNotFound)
		{
		TName filePath;
		filePath.Copy(iLogPath);
		User::LeaveIfError(iFs.MkDirAll(filePath));
		}
	if(err == KErrNotFound || err == KErrPathNotFound)
		{
		err = logFile.Create(iFs, iLogFileName, EFileWrite|EFileShareAny);
		}
	User::LeaveIfError(err);
	
	CleanupClosePushL(logFile);
	TInt filePos = 0;
	User::LeaveIfError(logFile.Seek(ESeekEnd,filePos));
	User::LeaveIfError(logFile.Write(*iHBuf));
	TPtr8 ptr(iHBuf->Des());
	ptr.Zero();

	TUint32 length = static_cast<TUint32>(aBinaryString.Length());
	TBuf8<sizeof(TUint32)> lengthString(4);
	lengthString[0] = static_cast<TUint8>((length & 0x000000ff) >> 0);
	lengthString[1] = static_cast<TUint8>((length & 0x0000ff00) >> 8);
	lengthString[2] = static_cast<TUint8>((length & 0x00ff0000) >> 16);
	lengthString[3] = static_cast<TUint8>((length & 0xff000000) >> 24);

	User::LeaveIfError(logFile.Write(aSubSystem));
	User::LeaveIfError(logFile.Write(KTabCharacter));
	User::LeaveIfError(logFile.Write(aComponent));
	User::LeaveIfError(logFile.Write(KTabCharacter));
	User::LeaveIfError(logFile.Write(KTypeIdentifierBinary));
	User::LeaveIfError(logFile.Write(KTabCharacter));
	User::LeaveIfError(logFile.Write(lengthString));
	User::LeaveIfError(logFile.Write(KTabCharacter));
	User::LeaveIfError(logFile.Write(aBinaryString));
	User::LeaveIfError(logFile.Write(KEolCharacters));
	
	CleanupStack::PopAndDestroy();	//logFile
	//LogString(KEolCharacters);
	}

void CFileWriter::ClearLogL()
	{
	User::LeaveIfError(iFs.Delete(iLogFileName));
	RFile logFile;
	User::LeaveIfError(logFile.Create(iFs, iLogFileName, EFileWrite|EFileShareAny));
	logFile.Close();
	TPtr8 ptr(iHBuf->Des());
	ptr.Zero();
	}

void CFileWriter::DoFlushBufferToFileL()
	{
	//Check that the log file exists, if not create a blank one.
	RFile logFile;
	TBool writePathErrorCode = KErrNone;
	TInt err = logFile.Open(iFs, iLogFileName, EFileWrite|EFileShareAny);
	if (err==KErrPathNotFound)
		{
		TName filePath;
		filePath.Copy(iLogPath);
		User::LeaveIfError(iFs.MkDirAll(filePath));
		}
	else if (err == KErrBadName || err == KErrNotReady)
		{
		writePathErrorCode = err;			
		// duff pathspec in ini file
		TName filePath;
		filePath.Copy(KLogDefaultFilePath);
		err = iFs.MkDirAll(filePath);
		iLogFileName.Copy(KLogDefaultFilePath);
		iLogFileName.Append(KLogFileName);			
		if (err == KErrAlreadyExists)
			{
			err = logFile.Open(iFs, iLogFileName, EFileWrite|EFileShareAny);
			}
		}
		
	if (err==KErrNotFound||err==KErrPathNotFound||err==KErrBadName||err == KErrNotReady)
		{
		err = logFile.Create(iFs, iLogFileName, EFileWrite|EFileShareAny);
		}
	User::LeaveIfError(err);
	CleanupClosePushL(logFile);
	TInt filePos = 0;
	User::LeaveIfError(logFile.Seek(ESeekEnd,filePos));
	if (writePathErrorCode != KErrNone)
		{
		TBuf8<KMaxFinalLogStringLength> tmpBuf;
		tmpBuf.Format(KUnableToUpdateMedia,writePathErrorCode);
		User::LeaveIfError(logFile.Write(tmpBuf));
		}
	if (iHBuf)
		{
		User::LeaveIfError(logFile.Write(*iHBuf));
		}
	CleanupStack::PopAndDestroy();	//logFile
	}

void CFileWriter::FlushLogL()
	{
	DoFlushBufferToFileL();
	if (iHBuf)
		{
		TPtr8 ptr(iHBuf->Des());
		ptr.Zero();
		}
	}

void CFileWriter::SetForceFlush(TBool aOn)
	{
	iForceBufferFlushAlways = aOn;
	}

void CFileWriter::SetLogPath(const TDesC8& aLogPathSetting)
	{
	iLogPath.Copy(aLogPathSetting);
	
	//the path may be reconfigured, so check for '\' 
	//as its assumed to be at the end already
	TPtr8 iTempStr = iLogPath.RightTPtr(1);	//get last
	TPtr8 iTempExt = iLogPath.RightTPtr(4);	//get last 4

	if( iTempStr == KLogPathTree)
		{
			//ends with '\', so ok to copy
			iLogFileName.Copy(iLogPath);
			iLogFileName.Append(KLogFileName);
		}	
	else if(iTempExt.Find(KLogFileExt)==KErrNotFound)
		{
			//no file extension already set, so its just path
			iLogPath.Append(KLogPathTree);
			iLogFileName.Copy(iLogPath);
			iLogFileName.Append(KLogFileName);
		}
	else
		{
			//already has all we need
			iLogFileName.Copy(iLogPath);
		}
	}

CFileWriter* CFileWriter::NewL()
	{
	CFileWriter* self = new(ELeave) CFileWriter;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CFileWriter::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	//Create our buffer for logging into, at zero length but specify a maximum.
	iHBuf = HBufC8::NewL(KHeapBufSize);
	iForceBufferFlushAlways = EFalse;
	iLogPath.Copy(KLogDefaultFilePath);
	iLogFileName.Copy(iLogPath);
	iLogFileName.Append(KLogFileName);
	}

CFileWriter::~CFileWriter()
	{
	// Don't attempt flushing when it's an OOM leave from NewL()
	if (iHBuf)
		{
		TRAP_IGNORE(CFileWriter::FlushLogL());	//Ignore error. Nothing we can do now.
		delete iHBuf;
		}
	iFs.Close();
	}

///////////////////////////////////////////////////////////////////////

CSerialWriter* CSerialWriter::NewL()
	{
	CSerialWriter* self = new(ELeave) CSerialWriter;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CSerialWriter::ConstructL()
	{
	User::LeaveIfError(iTimeoutTimer.CreateLocal());
	}

TInt CSerialWriter::LoadDevices()
	{
	// load the device drivers
	TInt result = User::LoadPhysicalDevice(PDD_NAME);
	if ((result != KErrNone) && (result != KErrAlreadyExists))
		{
		 return result;
		}

	result = User::LoadLogicalDevice(LDD_NAME);
	if ((result != KErrNone) && (result != KErrAlreadyExists))
		{
		return result;
		}

	return KErrNone;
	}


TInt CSerialWriter::OpenPort(TInt aPort)
	{
	return iSerialPort.Open(aPort);
	}

TInt CSerialWriter::SetPortConfig()
	{
	TCommConfig tComConfig;
	TCommConfigV01 &tComConfigV = tComConfig();
	iSerialPort.Config(tComConfig);
	tComConfigV.iRate=KFloggerSerialRate;
	tComConfigV.iDataBits=EData8;
	tComConfigV.iStopBits=EStop1;
	tComConfigV.iParity=EParityNone;
	tComConfigV.iHandshake=0;
	tComConfigV.iFifo = EFifoEnable;
	return iSerialPort.SetConfig(tComConfig);
	}



CSerialWriter::~CSerialWriter()
	{
	iTimeoutTimer.Close();
	iSerialPort.Close();
	}

void CSerialWriter::ClearLogL()
/**
 * @note: Nothing to do for serial
 */
	{}

void CSerialWriter::FlushLogL()
/**
 * @note: Nothing to do for serial
 */
	{}

void CSerialWriter::SetForceFlush(TBool)
/**
 * @note: Nothing to do for serial.
 */
	{
	}


void CSerialWriter::LogString(const TDesC8& aString)
	{
	//iInvalidcounter is used to dump packets if we fail to get a write through before the
	//timer expires. It stops us getting backed up too much if there's a problem.
	if (iInvalidCounter==0)
		{
		TRequestStatus writeStatus(KRequestPending);
		TRequestStatus timeoutStatus(KRequestPending);
		iSerialPort.Write(writeStatus, aString, aString.Length());
		iTimeoutTimer.After(timeoutStatus, 
			TTimeIntervalMicroSeconds32(KSerialTimeoutInMicroSecs));
		User::WaitForRequest(writeStatus, timeoutStatus);
		if (writeStatus==KRequestPending)
			{//OK, still not completed, better cancel send.
			iSerialPort.WriteCancel();
			iInvalidCounter=KSerialRetryCount;
			}
		else
			{
			iTimeoutTimer.Cancel();
			}
		User::WaitForAnyRequest();
		}
	else
		--iInvalidCounter;
	}

void CSerialWriter::LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent)
	{
	TUint32 length = static_cast<TUint32>(aBinaryString.Length());
	TBuf8<sizeof(TUint32)> lengthString(4);
	lengthString[0] = static_cast<TUint8>((length & 0x000000ff) >> 0);
	lengthString[1] = static_cast<TUint8>((length & 0x0000ff00) >> 8);
	lengthString[2] = static_cast<TUint8>((length & 0x00ff0000) >> 16);
	lengthString[3] = static_cast<TUint8>((length & 0xff000000) >> 24);

	LogString(aSubSystem);
	LogString(KTabCharacter);
	LogString(aComponent);
	LogString(KTabCharacter);
	LogString(KTypeIdentifierBinary);
	LogString(KTabCharacter);
	LogString(lengthString);
	LogString(KTabCharacter);
	LogString(aBinaryString);
	LogString(KEolCharacters);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

//Don't strictly need a NewL. Doing it for consistency.
CRDebugWriter* CRDebugWriter::NewL()
	{
	return new(ELeave) CRDebugWriter;
	}

CRDebugWriter::~CRDebugWriter()
	{
	}

void CRDebugWriter::ClearLogL()
/**
 * @note: Nothing to do for RDebug
 */
	{}

void CRDebugWriter::FlushLogL()
/**
 * @note: Nothing to do for RDebug
 */
	{}

void CRDebugWriter::SetForceFlush(TBool)
/**
 * @note: Nothing to do for RDebug.
 */
	{
	}


void CRDebugWriter::LogString(const TDesC8& aString)
	{
	
	//RDebug truncates strings longer than a certain size
	//To work around this we chop our string into palatable chunks
	//and log those individually
	
	const TUint8 *p = aString.Ptr();
	TInt bytesSent(0);
	TInt totalToSend = aString.Length();
	TInt length(0);
	
	do
		{
		bytesSent += KRDebugLimit;
		if (bytesSent < totalToSend)
			{
			length = KRDebugLimit;
			}
		else
			{
			length = totalToSend - bytesSent + KRDebugLimit;
			}
		
		TPtrC8 ptr(p,length);
		RDebug::RawPrint(ptr);		
		p += KRDebugLimit;
		}
	while(bytesSent < totalToSend);
	}


void CRDebugWriter::LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent)
	{
	TUint32 length = static_cast<TUint32>(aBinaryString.Length());
	TBuf8<sizeof(TUint32)> lengthString(4);
	lengthString[0] = static_cast<TUint8>((length & 0x000000ff) >> 0);
	lengthString[1] = static_cast<TUint8>((length & 0x0000ff00) >> 8);
	lengthString[2] = static_cast<TUint8>((length & 0x00ff0000) >> 16);
	lengthString[3] = static_cast<TUint8>((length & 0xff000000) >> 24);

	RDebug::RawPrint(aSubSystem);
	RDebug::RawPrint(KTabCharacter);
	RDebug::RawPrint(aComponent);
	RDebug::RawPrint(KTabCharacter);
	RDebug::RawPrint(KTypeIdentifierBinary);
	RDebug::RawPrint(KTabCharacter);
	RDebug::RawPrint(lengthString);
	RDebug::RawPrint(KTabCharacter);
	LogString(aBinaryString);	
	RDebug::RawPrint(KEolCharacters);

	}


///////////////////////////////////////////////////////////////////////

#if defined (__WINS__)
#include <emulator.h>

CDebugPortProtocol* CDebugPortProtocol::NewL()
	{
	CDebugPortProtocol* protocol = new(ELeave) CDebugPortProtocol;
	CleanupStack::PushL(protocol);
	protocol->ConstructL();
	CleanupStack::Pop(protocol);
	return protocol;
	}
	
void CDebugPortProtocol::ConstructL()
	{
	iDebugWriter = CDebugPortWriter::NewL();
	}
	
CDebugPortProtocol::~CDebugPortProtocol()
	{
	delete iDebugWriter;
	}
void CDebugPortProtocol::ClearLog(const TFullName& /*aName*/)
	{
	}
void CDebugPortProtocol::SetTimeL(const TTime& /*aTime*/)
	{
	}
void CDebugPortProtocol::LogString(const TDesC8& aLogString, const TDesC8& aSubSystem, const TDesC8& aComponent, const TThreadId& aThreadId)
	{
	TBuf8<KMaxFinalLogStringLength> buf;
	buf.Append(aSubSystem);
	buf.Append(KTabCharacter);
	buf.Append(aComponent);
	buf.Append(KTabCharacter);
	buf.Append(KTypeIdentifierAscii);
	buf.Append(KTabCharacter);
	buf.AppendNum(aThreadId, EHex);
	buf.Append(KTabCharacter);
	buf.Append(aLogString);
	// check whether last two bytes of string are eol chars, since some lines have cr/lf, some don't
	TPtr8 ptr(&buf[buf.Length()-2], 2, 2);
	if (ptr.Compare(KEolCharacters) !=0 )
		{
		buf.Append(KEolCharacters);
		}
	iDebugWriter->LogString(buf);
	}
void CDebugPortProtocol::LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent)
	{
	iDebugWriter->LogBinaryDump(aBinaryString, aSubSystem, aComponent);
	}
void CDebugPortProtocol::LogComment(const TDesC8& aComment)
	{
	iDebugWriter->LogString(aComment);
	}
void CDebugPortProtocol::MediaUpdate(const TDesC8& /*aMediaSetting*/, const TBool /*aForceFlushOn*/, const TDesC8& /*aLogPathSetting*/)
	{
	}
void CDebugPortProtocol::ShutDown()
	{
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////

CDebugPortWriter* CDebugPortWriter::NewL()
	{
	return new(ELeave) CDebugPortWriter;
	}
	

void CDebugPortWriter::LogString(const TDesC8& aString)
	{
	char str[Win32DisplayStringLengthMax];
	int len = Min(sizeof(str) - 1, aString.Length());
	Mem::Copy(str, aString.Ptr(), len);
	str[len] = '\0';
	Emulator::Lock();
	::OutputDebugStringA(str);
	Emulator::Unlock();
	}
void CDebugPortWriter::ClearLogL()
	{
	}
void CDebugPortWriter::FlushLogL()
	{
	}
void CDebugPortWriter::LogBinaryDump(const TDesC8& /*aBinaryString*/, const TDesC8& /*aSubSystem*/, const TDesC8& /*aComponent*/)
	{
	}
void CDebugPortWriter::SetForceFlush(const TBool /*aOn*/)
	{
	}

#endif  //(__WINS__)
