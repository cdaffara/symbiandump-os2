// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Source file for the client api
// 
//

/**
 @file Client.cpp
*/
#include <test/rfilelogger.h>


//_LIT8(KxmlHeader,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n");


// EKA1 requires DLL entry point
//
// RFileLogger class definition
//

EXPORT_C RFileFlogger::RFileFlogger() : ilogbody(NULL)
/**
 * Create a new flogger client interface object with an empty body.
 * @internalTechnology 
 */
	{
	iLogfileTag=FALSE;
	}

EXPORT_C RFileFlogger::~RFileFlogger()
/**
 * Destructor
 * @internalTechnology 
 */
	{}



EXPORT_C TInt RFileFlogger::Connect()
/**
 * @return int - Standard error codes
 * EKA2 all variants and EKA1 target.
 * Server is an exe
 */
	{
	// Sanity check to make sure it's not been called multiple times
	if (ilogbody)
		{
		return KErrAlreadyExists;
		}
	ilogbody = new RFileLoggerBody;
	if(!ilogbody)
		return KErrNoMemory;

	TVersion version(KRFileLoggerMajorVersion,KRFileLoggerMinorVersion,KRFileLoggerBuildVersion);
	// Assume the server is already running and attempt to create a session
	
	TInt err = ilogbody->DoCreateSession(KFileLogrerServerName,version,8);
	if(err== KErrServerTerminated)
	{
		User::After(1000000); // OS need time to close previous server properly
		err = ilogbody->DoCreateSession(KFileLogrerServerName,version,8);
	}

	if(err == KErrNotFound)
		{
		// Server not running
		// Construct the server binary name
		_LIT(KEmpty,"");
		// EKA2 is simple
		// No path required
		TBuf<32> serverFile;
		serverFile.Copy(KFileLogrerServerName);
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
		err = ilogbody->DoCreateSession(KFileLogrerServerName,version,8);
		}
		if (err != KErrNone)
		{  // some other problem, kill the logbody and clean the mamory
			delete ilogbody;
			ilogbody=NULL;
		}
		else
		{   // Makes the session shared among all threads in the process
			err = ilogbody->ShareAuto();
		}


	return err;
	}

///////
EXPORT_C TInt RFileFlogger::CreateLog(const TDesC& aLogFilePath, TLogMode aMode)
/**
 * @param aLogFilePath - Full path and filename of the log file
 * @param aMode - Overwrite or Append
 * Makes synchronous call to the log server to create a log session
 */
	{
	iloglevel = ESevrAll; //ELogNone;

	if(aLogFilePath.Length() > KMaxLoggerFilePath)
		return KErrTooBig;

	TIpcArgs args;
	args.Set(0,&aLogFilePath);
	args.Set(1,aMode);
    TInt err = ilogbody->DoSendReceive(ECreateLog,args);
    return err; 

	}

EXPORT_C void RFileFlogger::Log(const TText8* aFile, TInt aLine, TLogSeverity aSeverity, TRefByValue<const TDesC> aFmt,...)
/**
 * @param aFile - Source file name
 * @param aLine - Source file line number
 * @param aSeverity - ERR, WARN, INFO
 * @param aFmt - UNICODE format string
 */
	{
	// Set up a Variable argument list and call private method
	if (aSeverity>iloglevel && aSeverity != ESevrTEFUnit) 
		{
		return;
		}
	VA_LIST aList;
	VA_START(aList, aFmt);
	Log(aFile, aLine, aSeverity, aFmt, aList);
	VA_END(aList); 

	}
EXPORT_C void RFileFlogger::Log(const TText8* aFile, TInt aLine, TLogSeverity aSeverity,TInt arraylength, TExtraLogField* aLogFields, TRefByValue<const TDesC> aFmt,...)
/**
 * @param aFile - Source file name
 * @param aLine - Source file line number
 * @param aSeverity - ERR, WARN, INFO
 * @param aFmt - UNICODE format string
 */
	{
	// Set up a Variable argument list and call private method
	if (aSeverity>iloglevel && aSeverity != ESevrTEFUnit) 
		{
		return;
		}
	VA_LIST aList;
	VA_START(aList, aFmt);
	Log(aFile, aLine, aSeverity, arraylength, aLogFields, aFmt, aList);
	VA_END(aList); 
	}


void RFileFlogger::AddTime(TDes8& aLogBuffer)
	{
	TTime now;
	now.UniversalTime();
	TDateTime dateTime = now.DateTime();
	_LIT8(KFormat,"%02d:%02d:%02d:%03d");
	// add the current time. 
/*--------- Maintaince Warning for aLogBuffer -----------------------------------
******* the fomat of below string is sensible to server.  
******* Adding any string to the aLogBuffer has to be checked 
******* in code on server side 
--------------------------------------------------------------------------------*/
	aLogBuffer.AppendFormat(KFormat,dateTime.Hour(),dateTime.Minute(),dateTime.Second(),(dateTime.MicroSecond()/1000)); 
/*--------------- End of Maintaince Warning  ----------------*/
	}

EXPORT_C void RFileFlogger::Log(const TText8* aFile, TInt aLine, TLogSeverity aSeverity, TRefByValue<const TDesC> aFmt, VA_LIST aList)
	{
	if (aSeverity>iloglevel && aSeverity != ESevrTEFUnit) 
		{
		return;
		}

	TInt arraylength = 0;
	TExtraLogField* aLogFields =NULL;	
	Log(aFile, aLine, aSeverity, arraylength, aLogFields, aFmt, aList);
	}

EXPORT_C void RFileFlogger::Log(const TText8* aFile, TInt aLine, TLogSeverity aSeverity,TInt arraylength, TExtraLogField* aLogFields, TRefByValue<const TDesC> aFmt, VA_LIST aList)
/**
 * @param aFile - Source file name
 * @param aLine - Source file line number
 * @param aSeverity - ERR, WARN, INFO
 * @param arraylength
 * @param aLogFields
 * @param aFmt - UNICODE format string
 * @param aList - Variable argument list
 *
 * Format a log output line
 */
 	{
	if (aSeverity>iloglevel && aSeverity != ESevrTEFUnit) 
		{
		return;
		}

	if (aSeverity == ESevrTEFUnit)
		{
		aSeverity = ESevrInfo;
		}
/*----- Maintaince Warning for this section: -----------------------------------
******* the fomat of below string is very sensible to server Server 
******* defomating these string with the understanding of this 
******* perticular format. Any change made here should be checked 
******* in code on server side 
--------------------------------------------------------------------------------*/
	// Create a filename string
	TBuf16<KMaxFilename> fileName;
	GetCPPModuleName(fileName, aFile);  
	// Create a buffer for formatting
	HBufC* buffer = HBufC::New(KMaxLoggerLineLength*2);
	if(!buffer)
		return;  // no memory
	TPtr ptr(buffer->Des());
	_LIT(KEnd,"\r\n");
	_LIT(KErr,"ERROR");
	_LIT(KHigh,"HIGH");
	_LIT(KWarn,"WARN");
	_LIT(KMedium,"MEDIUM");
	_LIT(KInfo,"INFO");
	_LIT(KLow,"LOW");
//	ptr.Append(KTypeTagBeging);
	ptr.Append(KSeperation);
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
	// Add the thread id -------- read CIniData to decide the level of details
	ptr.AppendFormat(KMessageFormat,(TInt)(RThread().Id()),&fileName, aLine);
	ptr.AppendFormatList(aFmt, aList);
	if(arraylength>0)  // trust user providing correct number with actual arrary length
	{
		_LIT(KTab,"\t");
		// presuming the following string is hardly being apart of log message from users
		// and no carrige return and line feed in their log field name and field value
		ptr.Append(KEnd);
		ptr.Append(KTagSeperation);
		ptr.Append(KTab);
		ptr.AppendNum(TInt64(arraylength));
		TInt loopValue(arraylength);
		while(loopValue)
		{
			ptr.Append(KEnd);
			ptr.Append(aLogFields->iLogFieldName);
			ptr.Append(KTab);
			ptr.Append(aLogFields->iLogFieldValue);
			loopValue--; // Decrement the looping until all fields are exhausted
			aLogFields++; // Increment the pointer address to access sucessive array index values			
		}
		ptr.Append(KEnd);
		ptr.Append(KTagSeperationEnd);

	}
/*----------------- End of Maintaince warning section --------------------------*/
	TRAP_IGNORE(WriteL(ptr));

	delete buffer;

	}


void RFileFlogger::WriteL(const TDesC& aLogBuffer)
/**
 * @param aLogBuffer - UNICODE buffer
 */
	{
	HBufC8* buffer = HBufC8::NewLC(aLogBuffer.Length()+100);
	if(!buffer)
		return;  // no memory
	TPtr8 ptr(buffer->Des());
	AddTime(ptr);
	ptr.Append(aLogBuffer);
	TRAP_IGNORE(WriteL(ptr));

	CleanupStack::PopAndDestroy(buffer);
	}

void RFileFlogger::WriteL(TDes8& aLogBuffer)
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
		HBufC8* buffer = HBufC8::NewLC(aLogBuffer.Length()+2);
		if(!buffer)
			return;  // no memory
		TPtr8 ptr(buffer->Des());
		ptr.Copy(aLogBuffer);
		TBuf8<4> tempBuf(_L8("\r\n"));
		if (aLogBuffer.Mid(aLogBuffer.Length()-2,2).CompareF(tempBuf) != 0)
			ptr.Append(KEnd);
		TIpcArgs args;
		args.Set(0,&ptr);
		args.Set(1,ptr.Length());
		User::LeaveIfError(ilogbody->DoSendReceive(EWriteLog,args));
		CleanupStack::PopAndDestroy(buffer);
		}
	else
		{
		TBuf8<4> tempBuf(_L8("\r\n"));
		if (aLogBuffer.Mid(aLogBuffer.Length()-2,2).CompareF(tempBuf) != 0)
			aLogBuffer.Append(KEnd);
		TIpcArgs args;
		args.Set(0,&aLogBuffer);
		args.Set(1,aLogBuffer.Length());
		User::LeaveIfError(ilogbody->DoSendReceive(EWriteLog,args));
		}
	}

void RFileFlogger::GetCPPModuleName(TDes& aModuleName, const TText8* aCPPFileName)
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



EXPORT_C void RFileFlogger::SetLogLevel(TLogSeverity aloglevel)
	{
	iloglevel=aloglevel;
	}


EXPORT_C void RFileFlogger::Close()
	{
  	if(ilogbody)
  		{
		ilogbody->Close();
		delete ilogbody;
		ilogbody = NULL;
  		}
	}

	
