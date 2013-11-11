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
* Source file for the client api
*
*/



/**
 @file Client.cpp
*/
#include <test/testexecutelog.h>


const TInt KMaxFilename = 50;
const TInt KLowMemBufLength = 128;

// EKA1 requires DLL entry point


EXPORT_C TInt RTestExecuteLogServ::Connect()
/**
 * @return int - Standard error codes
 * EKA2 all variants and EKA1 target.
 * Server is an exe
 */
	{
	TVersion version(KTestExecuteLoggerMajorVersion,KTestExecuteLoggerMinorVersion,KTestExecuteLoggerBuildVersion);
	// Assume the server is already running and attempt to create a session
	TInt err = CreateSession(KTestExecuteLogServerName,version,8);
	if(err == KErrNotFound)
		{
		// Server not running
		// Construct the server binary name
		_LIT(KEmpty,"");
		// EKA2 is simple
		// No path required
		TBuf<32> serverFile;
		serverFile.Copy(KTestExecuteLogServerName);
		_LIT(KExe,".exe");
		serverFile.Append(KExe);
		RProcess server;
		err = server.Create(serverFile,KEmpty);
		if(err != KErrNone)
			return err;
		// Synchronise with the server
		TRequestStatus reqStatus;
		server.Rendezvous(reqStatus);
		server.Resume();
		// Server will call the reciprocal static synchronise call
		User::WaitForRequest(reqStatus);
		server.Close();
		if(reqStatus.Int() != KErrNone)
			return reqStatus.Int();
		// Create the root server session
		err = CreateSession(KTestExecuteLogServerName,version,8);
		}

#ifdef TEF_SHARE_AUTO
	// Makes the session shared among all threads in the process
  	if( err == KErrNone )
  		{
  		err = ShareAuto();
  		}
#endif

	return err;
	}

///////
EXPORT_C TInt RTestExecuteLogServ::CreateLog(const TDesC& aLogFilePath, TLogMode aMode)
/**
 * @param aLogFilePath - Full path and filename of the log file
 * @param aMode - Overwrite or Append
 * Makes synchronous call to the log server to create a log session
 */
	{
	if(aLogFilePath.Length() > KMaxTestExecuteLogFilePath)
		return KErrTooBig;

	iLogLevel = TLogSeverity(ESevrAll);
	TIpcArgs args;
	args.Set(0,&aLogFilePath);
	args.Set(1,aMode);
	return SendReceive(ECreateLog,args);

	}

EXPORT_C void RTestExecuteLogServ::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,TRefByValue<const TDesC> aFmt,...)
/**
 * @param aFile - Source file name
 * @param aLine - Source file line number
 * @param aSeverity - ERR, WARN, INFO
 * @param aFmt - UNICODE format string
 */
	{
	if (aSeverity>iLogLevel) 
		{
		return;
		}
	// Set up a Variable argument list and call private method
	VA_LIST aList;
	VA_START(aList, aFmt);
	LogExtra(aFile, aLine, aSeverity, aFmt, aList);
	VA_END(aList); 
	}

EXPORT_C void RTestExecuteLogServ::WriteFormat(TRefByValue<const TDesC> aFmt,...)
/**
 * @param aFmt - UNICODE format string
 */
	{
	// Set up a Variable argument list and call private method
	VA_LIST aList;
	VA_START(aList, aFmt);
	WriteFormat(aFmt, aList);
	VA_END(aList); 
	}


EXPORT_C void RTestExecuteLogServ::Write(const TDesC& aLogBuffer)
/**
 * @param aLogBuffer - Pre-formatted UNICODE buffer
 */
	{
	// Just call private method
	// Ignore errors. Could put in ASSERT
	TRAP_IGNORE(WriteL(aLogBuffer));
	}


EXPORT_C void RTestExecuteLogServ::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
/**
 * @param aFmt - Narrow format string
 */
	{
	// Set up a Variable argument list and call private method
	VA_LIST aList;
	VA_START(aList, aFmt);
	WriteFormat(aFmt, aList);
	VA_END(aList); 
	}


EXPORT_C void RTestExecuteLogServ::Write(const TDesC8& aLogBuffer)
/**
 * @param aLogBuffer - Pre-formatted Narrow buffer
 */
	{
	// Create a larger buffer for adding terminators
	// Could do more formatting but just allow for CRLF plus time stamp for the time being
	HBufC8* buffer = HBufC8::New(aLogBuffer.Length()+20);
	if(buffer)
		{
		TPtr8 ptr(buffer->Des());
		AddTime(ptr);
		ptr.Append(aLogBuffer);
		// Ignore error for the time being. Could do an ASSERT
		TRAP_IGNORE(WriteL(ptr));
		delete buffer;
		}
	}

void RTestExecuteLogServ::AddTime(TDes8& aLogBuffer)
	{
	TTime now;
	now.UniversalTime();
	TDateTime dateTime = now.DateTime();
	_LIT8(KFormat,"%02d:%02d:%02d:%03d ");
	// add the current time 
	aLogBuffer.AppendFormat(KFormat,dateTime.Hour(),dateTime.Minute(),dateTime.Second(),(dateTime.MicroSecond()/1000)); 
	}

EXPORT_C void RTestExecuteLogServ::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,TRefByValue<const TDesC> aFmt, VA_LIST aList)
/**
 * @param aFile - Source file name
 * @param aLine - Source file line number
 * @param aSeverity - ERR, WARN, INFO
 * @param aFmt - UNICODE format string
 * @param aList - Variable argument list
 *
 * Format a log output line
 */
	{
	if (aSeverity>iLogLevel) 
		{
		return;
		}
	// Create a filename string
	TBuf16<KMaxFilename> fileName;
	GetCPPModuleName(fileName, aFile);
	// Create a buffer for formatting
	HBufC* buffer = HBufC::New(KMaxTestExecuteLogLineLength*2);
	if (buffer)
		{
		TPtr ptr(buffer->Des());
		_LIT(KErr,"ERROR - ");
		_LIT(KHigh,"HIGH - ");
		_LIT(KWarn,"WARN - ");
		_LIT(KMedium,"MEDIUM - ");
		_LIT(KInfo,"INFO - ");
		_LIT(KLow,"LOW - ");
		_LIT(KFormat," %d %S %d ");
		if(aSeverity == ESevrErr)
			ptr.Append(KErr);
		else if(aSeverity == ESevrHigh)
			ptr.Append(KHigh);
		else if(aSeverity == ESevrWarn)
			ptr.Append(KWarn);
		else if(aSeverity == ESevrMedium)
			ptr.Append(KMedium);
		else if (aSeverity == ESevrInfo)
			ptr.Append(KInfo);
		else if(aSeverity == ESevrLow)
			ptr.Append(KLow);
		else //if(aSeverity == ESevrAll)
			ptr.Append(KInfo);
		// Add the thread id
		ptr.AppendFormat(KFormat,(TInt)RThread().Id(),&fileName, aLine);
		TTEFDes16Overflow des16OverflowObject;
		ptr.AppendFormatList(aFmt, aList, &des16OverflowObject);
		

		TRAP_IGNORE(WriteL(ptr));
		delete buffer;
		}
	else
		{
		_LIT(KWarn,"WARN - ");
		TBuf8<KLowMemBufLength> noMemory;
		AddTime(noMemory);
		noMemory.Append(KWarn);
		noMemory.AppendFormat(_L8(" %d "), (TInt)RThread().Id());
		noMemory.Append(fileName);
		noMemory.AppendFormat(_L8(" %d %S"), aLine, &KLoggerNotEnoughMemory8());
		TRAP_IGNORE(WriteL(noMemory));
		}
	}

EXPORT_C void RTestExecuteLogServ::WriteFormat(TRefByValue<const TDesC> aFmt, VA_LIST aList)
/**
 * @param aFmt - UNICODE format string
 * @param aList - Variable argument list
 */
	{
	HBufC* buffer = HBufC::New(KMaxTestExecuteLogLineLength*2);
	if (buffer)
		{
		TPtr ptr(buffer->Des());
		TTEFDes16Overflow des16OverflowObject;
		ptr.AppendFormatList(aFmt, aList, &des16OverflowObject);

		TRAP_IGNORE(WriteL(ptr));
		delete buffer;
		}
	else
		{
		TRAP_IGNORE(WriteL((TDes8&)KLoggerNotEnoughMemory8()));
		}
	}

EXPORT_C void RTestExecuteLogServ::WriteFormat(TRefByValue<const TDesC8> aFmt, VA_LIST aList)
/**
 * @param aFmt - UNICODE format string
 * @param aList - Variable argument list
 */
	{
	HBufC8* buffer = HBufC8::New(KMaxTestExecuteLogLineLength*2);
	if (buffer)
		{
		TPtr8 ptr(buffer->Des());
		AddTime(ptr);
		TTEFDes8Overflow des8OverflowObject;
		ptr.AppendFormatList(aFmt, aList, &des8OverflowObject);

		TRAP_IGNORE(WriteL(ptr));
		delete buffer;
		}
	else
		{
		TRAP_IGNORE(WriteL((TDes8&)KLoggerNotEnoughMemory8()));
		}
	}

void RTestExecuteLogServ::WriteL(const TDesC& aLogBuffer)
/**
 * @param aLogBuffer - UNICODE buffer
 */
	{
	HBufC8* buffer = NULL;
	TRAPD(err, buffer=HBufC8::NewL(aLogBuffer.Length()+20));
	if (KErrNoMemory == err)
		{
		_LIT8(KWarn,"WARN - ");
		TInt startPos = 13; // The place change "INFO" to "WARN"
		TInt endPos = 7;
		TInt wordCount = 5; // The message before the actual log info
		TBuf8<KLowMemBufLength> buffer;
		AddTime(buffer);
		TLex lex(aLogBuffer);
		lex.Mark();
		for (TInt i=0; i<wordCount; i++)
			{
			lex.NextToken();
			}
		lex.SkipSpace();
		buffer.Append(lex.MarkedToken());
		buffer.Append(KLoggerNotEnoughMemory8);
		buffer.Replace(startPos, endPos, KWarn);
		WriteL(buffer);
		}
	else
		{
		CleanupStack::PushL(buffer);
	  	TPtr8 ptr(buffer->Des());
	  	AddTime(ptr);
	  	ptr.Append(aLogBuffer);
	  	WriteL(ptr);
	  
	  	CleanupStack::PopAndDestroy(buffer);
		}
	}

void RTestExecuteLogServ::WriteL(TDes8& aLogBuffer)
/**
 * @param aLogBuffer - pre-formatted narrow buffer
 * 
 * Synchronous write to the server
 */
	{
	_LIT8(KEnd,"\r\n");
	// Check to see if there's room to add CRLF
	if(aLogBuffer.Length()+2 > aLogBuffer.MaxLength())
		{
		HBufC8* buffer = NULL;
		TRAPD(err, buffer=HBufC8::NewL(aLogBuffer.Length()+2));
		if (KErrNoMemory == err)
			{
			aLogBuffer.Replace(aLogBuffer.Length()-2, 2, KEnd);
			TIpcArgs args;
			args.Set(0,&aLogBuffer);
			args.Set(1,aLogBuffer.Length());
			User::LeaveIfError(SendReceive(EWriteLog,args));
			}
		else
			{
			CleanupStack::PushL(buffer);
			TPtr8 ptr(buffer->Des());
			ptr.Copy(aLogBuffer);
			ptr.Append(KEnd);
			TIpcArgs args;
			args.Set(0,&ptr);
			args.Set(1,ptr.Length());
			User::LeaveIfError(SendReceive(EWriteLog,args));
			CleanupStack::PopAndDestroy(buffer);
			}
		}
	else
		{
		aLogBuffer.Append(KEnd);
		TIpcArgs args;
		args.Set(0,&aLogBuffer);
		args.Set(1,aLogBuffer.Length());
		User::LeaveIfError(SendReceive(EWriteLog,args));
		}
	}

void RTestExecuteLogServ::GetCPPModuleName(TDes& aModuleName, const TText8* aCPPFileName)
/**
 * @return aModuleName - Filename in descriptor
 * @param aCppFileName - Filename
 * Borrowed from scheduletest
 */
	{
	TPtrC8 fileNamePtrC8(aCPPFileName);
	// We do our own filename munging here; TParse can't help us since that's
	// expressly for EPOC filepaths and here we've got whatever the build system is
	// At present Win32 and Unix directory delimiters are supported
	TInt lastDelimiter = Max(fileNamePtrC8.LocateReverse('\\'), fileNamePtrC8.LocateReverse('/'));
	if(lastDelimiter >= 0 && lastDelimiter < fileNamePtrC8.Length() - 1)
		{
		// Found a delimiter which isn't trailing; update the ptr to start at the next char
		TInt fileNameLen = Min(KMaxFilename, fileNamePtrC8.Length() - (lastDelimiter + 1));
		fileNamePtrC8.Set(aCPPFileName + lastDelimiter + 1, fileNameLen);
		}
	else
		{
		// Didn't find a delimiter; take as much of the right-end of the name as fits
		fileNamePtrC8.Set(aCPPFileName + Max(0, fileNamePtrC8.Length() - KMaxFilename), Min(fileNamePtrC8.Length(), KMaxFilename));
		}
	aModuleName.Copy(fileNamePtrC8);
	}

EXPORT_C void RTestExecuteLogServ::SetLogLevel(TLogSeverity aSeverity)
	{
	iLogLevel = aSeverity;
	}
