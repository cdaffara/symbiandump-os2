// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#include <f32file.h>
#include <flogger.h>
#include "FLOGSTD.H"

/** 
Macro for Single blank space.

*/
#define BLANK	_S("")

const TInt KHexDumpWidth=16;

/**
Literal constant that hold a string and unicode hex.

*/
_LIT(KFirstFormatString,"%s%04x : ");

/**
Literal constant that hold a string and unicode hex.

*/
_LIT(KSecondFormatString,"%02x ");

/**
Character Constant.

*/
_LIT(KThirdFormatString,"%c");

/** 
Literal Constant for 3 blank spaces. 

*/
_LIT(KThreeSpaces,"   ");

/** 
Literal Constant for 2 blank spaces. 

*/
_LIT(KTwoSpaces," ");


IMPORT_C extern const TBool KFloggerSTI;

/*
RFileLogger class definition
*/

EXPORT_C RFileLogger::RFileLogger() 
	: RSessionBase(), iFormatter(), iLastError(KErrNone), iLogSTI(KFloggerSTI)
/** Creates a default RFileLogger object. */
	{}

EXPORT_C RFileLogger::~RFileLogger()
/** Empty destructor. 
Clients with open sessions must end the session by calling Close() beforehand.
Note that it does not delete the log file.*/
	{}

EXPORT_C TVersion RFileLogger::Version() const
/** Returns the version number.

@return The version number. */
	{

	return(TVersion(KFLogSrvMajorVersionNumber,KFLogSrvMinorVersionNumber,KFLogSrvBuildVersionNumber));
	}

EXPORT_C TInt RFileLogger::Connect()
/** Connects to the file logger server with the default number of message slots, which is 8.

This function does not need to be called if you are using the static versions 
of Write(), WriteFormat() or HexDump().

@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{

	TInt ret=DoConnect();
	if (ret==KErrNotFound)
		{
		ret=FLogger::Start();
		if (ret==KErrNone || ret==KErrAlreadyExists)
			ret=DoConnect();
		}
	return ret;
	}

EXPORT_C void RFileLogger::SetDateAndTime(TBool aUseDate,TBool aUseTime)
/** Specifies whether time and/or date should be appended to log data.

Appending of the time and date to log entries can be switched on and off 
at anytime between creation and destruction of RFileLogger.

This function does not need to be called if you are using the static versions 
of Write(), WriteFormat() or HexDump().

@param aUseDate Use ETrue to log the date, otherwise EFalse.
@param aUseTime Use ETrue to log the time, otherwise EFalse. */
	{

	iFormatter.SetDateAndTime(aUseDate,aUseTime);
	}

EXPORT_C void RFileLogger::CreateLog(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode)
/** Creates or opens a file for logging.

When specifying a directory for logging only specify the relative path to 
'C:\\Logs\\' and do not append a '\\' to the end of the path either. CreateLog() 
only creates the specified log file if the directory exists. This means that 
switching logging on and off can be achieved without having to re-compile, 
by just removing or renaming the log directory.

aLogFile.iValid is set according to whether logging will actually occur or not. 

@param aDir The directory relative to 'C:\\Logs\\' where the log file resides 
or is to be created.
@param aName The name of the log file.
@param aMode The mode in which the log file should be opened, either EAppend 
or EOverwrite. */
	{

	iLogFile.Set(aDir,aName,aMode);
	TPckg<TLogFile> logFilePckg(iLogFile);
 	iLastError=SendReceive(ECreateLog, TIpcArgs( &logFilePckg));

	if (iLastError!=KErrNone && !LogSTI())
		iLogFile.SetValid(EFalse);
	}

EXPORT_C void RFileLogger::CloseLog()
/** Closes the log file, iLogFile.

This function closes the log file previously created or opened by CreateLog(). */
	{
	TPckg<TLogFile> logFilePckg(iLogFile);
 	iLastError=SendReceive(ECloseLog,TIpcArgs( &logFilePckg));
	}

EXPORT_C TBool RFileLogger::LogValid() const
/** Returns the status of the log.

@return ETrue if the log file is valid; otherwise EFalse. */
	{

	return iLogFile.Valid();
	}

EXPORT_C TInt RFileLogger::LastError() const
/** Returns the last error status that has been set.

@return This value can be set to any of the valid error codes from any of 
the functions in this class. */
	{

	return iLastError;
	}

EXPORT_C TBool RFileLogger::LogSTI() const
/** Returns patchable constant value.

@return This value specify the logging output. The default value EFalse 
cause that the logs are stored to filesystem. ETrue intorduced as in CR 1688
is used for logging using RDebug::Print */
	{

	return iLogSTI;
	}

//
// 16-bit non-static writes
//

EXPORT_C void RFileLogger::Write(const TDesC16& aText)
/** Writes a string of Unicode characters to an open log, iLogFile, if it is a valid file.

Note that the text will be converted to an 8 bit format for the log file. 

@param aText The Unicode string to write to the open log. */
	{

	if (iLogFile.Valid() || LogSTI())
		{
		TBuf8<KLogBufferSize> buf;
		iLastError=iFormatter.FormatTextToWritableBuffer(buf,aText);
		if (iLastError==KErrNone)
			DoWrite(buf);
		}
	}
	
EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC16> aFmt,...)
/** Formats the remaining arguments of the function according to aFmt, and writes the 
result to the log, iLogFile, if it is a valid file.

The format string aFmt contains literal text, embedded with directives, for converting 
the trailing list of arguments into text. The number and type of arguments is dictated 
by the structure of the directives in aFmt.

Note that the text will be converted to an 8 bit format for the log file. 

@param aFmt The 16-bit non modifiable descriptor containing the format string. 
The TRefByValue class provides a constructor which takes a TDesC16 type. */
	{

	VA_LIST list;
	VA_START(list,aFmt);
	DoWriteFormat(aFmt,list);
	}

EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
/** Formats the arguments pointed to by aList according to aFmt, and writes the 
result to the log, iLogFile, if it is a valid file.

The format string aFmt contains literal text, embedded with directives, 
for converting the trailing list of arguments into text. The number and type 
of arguments pointed to by aList is dictated by the structure of the directives 
in aFmt.

Note that the text will be converted to an 8 bit format for the log file. 

@param aFmt The 16-bit non modifiable descriptor containing the format string. 
The TRefByValue class provides a constructor which takes a TDesC16 type. 
@param aList A pointer to an argument list. */
	{

	DoWriteFormat(aFmt,aList);
	}

//
// 8-bit non-static writes
//

EXPORT_C void RFileLogger::Write(const TDesC8& aText)
/** Writes a string of 8-bit characters to an open log, iLogFile, if it is a valid file.

@param aText The 8-bit character string to write to the open log. */
	{

	if (iLogFile.Valid() || LogSTI())
		{
		TBuf8<KLogBufferSize> buf;
		iLastError=iFormatter.FormatTextToWritableBuffer(buf,aText);
		if (iLastError==KErrNone)
			DoWrite(buf);
		}
	}
	
EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
/** Formats the remaining arguments of the function according to aFmt, and writes the 
result to the log, iLogFile, if it is a valid file.

The format string aFmt contains literal text, embedded with directives, 
for converting the trailing list of arguments into text. The number and type 
of arguments is dictated by the structure of the directives in aFmt.

@param aFmt The 8 bit non modifiable descriptor containing the format string. 
The TRefByValue class provides a constructor which takes a TDesC8 type.*/
	{

	VA_LIST list;
	VA_START(list,aFmt);
	DoWriteFormat(aFmt,list);
	}

EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/** Formats the arguments pointed to by aList according to aFmt, and writes the result 
to the log, iLogFile, if it is a valid file.

The format string aFmt contains literal text, embedded with directives, 
for converting the trailing list of arguments into text. The number and type 
of arguments pointed to by aList is dictated by the structure of the directives 
in aFmt.

@param aFmt The 8 bit non modifiable descriptor containing the format string. 
The TRefByValue class provides a constructor which takes a TDesC8 type. 
@param aList A pointer to an argument list. */
	{

	DoWriteFormat(aFmt,aList);
	}

//
// 16-bit static writes
//

EXPORT_C void RFileLogger::Write(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, const TDesC16& aText)
/** Writes a string of 16-bit characters to an open log.

Connects to the logging server, creates/opens the log file and write aText to it.

This is a "static write".

Note that the text will be converted to an 8 bit format for the log file. 

@param aDir The directory relative to 'C:\\Logs\\' where the log file resides.
@param aName The name of the log file.
@param aMode The mode in which the log file should be opened: either EAppend or EOverwrite.
@param aText The Unicode string to write to the log. */
	{

	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime(ETrue,ETrue);
		logger.iLogFile.Set(aDir,aName,aMode);
		TBuf8<KLogBufferSize> buf;
		ret=logger.iFormatter.FormatTextToWritableBuffer(buf,aText);
		if (ret==KErrNone)
			{
			if (logger.LogSTI()) 
				{
				logger.DoStaticWrite(buf);
				} else 
					{
					TPckg<TLogFile> logFilePckg(logger.iLogFile);
					logger.SendReceive(ECreateWriteAndCloseLog, TIpcArgs ( &logFilePckg, &buf));		// ignore error
					}
 			}
		}
	logger.Close();
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, TRefByValue<const TDesC16> aFmt,...)
/** Formats the remaining arguments of the function according to aFmt and writes the 
result to the log.

The format string aFmt contains literal text, embedded with directives, 
for converting the trailing list of arguments into text. The number and type 
of arguments is dictated by the structure of the directives in aFmt.

Connects to the logging server, creates/opens the log file and writes the text arguments to it.

This is a "static write".

Note that the text will be converted to an 8 bit format for the log file.

@param aDir The directory relative to 'C:\\Logs\\' where the log file resides.
@param aName The name of the log file.
@param aMode The mode in which the log file should be opened: either EAppend 
or EOverwrite.
@param aFmt The 16 bit non modifiable descriptor containing the format string. 
The TRefByValue class provides a constructor which takes a TDesC16 type. */
	{
	
	VA_LIST list;
	VA_START(list,aFmt);
	DoStaticWriteFormat(aDir,aName,aMode,aFmt,list);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
/** Formats the arguments pointed to by aList according to aFmt, and writes the result 
to the log.

The format string aFmt contains literal text, embedded with directives, 
for converting the trailing list of arguments into text. The number and type 
of arguments pointed to by aList is dictated by the structure of the directives 
in aFmt.

Connects to the logging server, creates/opens the log file and writes the text arguments to it.

This is a "static write".

Note that the text will be converted to an 8 bit format for the log file. 

@param aDir The directory relative to 'C:\\Logs\\' where the log file resides.
@param aName The name of the log file.
@param aMode The mode in which the log file should be opened: either EAppend 
or EOverwrite.
@param aFmt The 16 bit non modifiable descriptor containing the format string. 
The TRefByValue class provides a constructor which takes a TDesC16 type. 
@param aList A pointer to an argument list. */
	{

	DoStaticWriteFormat(aDir,aName,aMode,aFmt,aList);
	}

//
// 8-bit static writes
//

EXPORT_C void RFileLogger::Write(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, const TDesC8& aText)
/** Writes a string of 8-bit characters to an open log.

Connects to the logging server, creates/opens the log file and writes aText to it.

This is a "static write".

@param aDir The directory relative to 'C:\\Logs\\' where the log file resides.
@param aName The name of the log file.
@param aMode The mode in which the log file should be opened: either EAppend 
or EOverwrite.
@param aText The 8-bit string to write to the log. */

	{

	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime(ETrue,ETrue);
		logger.iLogFile.Set(aDir,aName,aMode);
//		TBuf8<KLogBufferSize> buf;
		TBuf8<1600> buf; //Want at least an mtu sized buffer
		//PG 14/08/2002 - If mode is set to *Raw, Don't change format of client buffer
		if(aMode == EFileLoggingModeAppendRaw || aMode == EFileLoggingModeOverwriteRaw)
            {
            TPtrC8 ptr8;
            if (logger.LogSTI()) 
                {
                ptr8.Set(aText.Left(KLogBufferSize)); //in LogSTI mode, truncate to KLogBufferSize
                }
            else
                {
                ptr8.Set(aText.Left(1600)); // truncate to 1600
                }
            buf.Copy(ptr8);
            }
		else
            {
			ret=logger.iFormatter.FormatTextToWritableBuffer(buf,aText);
            }
		if (ret==KErrNone)
            {
			logger.DoStaticWrite(buf);
            }
		}
	logger.Close();
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, TRefByValue<const TDesC8> aFmt,...)
/** Formats the remaining arguments of the function according to aFmt and writes the 
result to the log.

The format string aFmt contains literal text, embedded with directives, 
for converting the trailing list of arguments into text. The number and type 
of arguments is dictated by the structure of the directives in aFmt.

Connects to the logging server, creates/opens the log file and writes the text arguments to it.

This is a "static write".

@param aDir The directory relative to 'C:\\Logs\\' where the log file resides.
@param aName The name of the log file.
@param aMode The mode in which the log file should be opened: either EAppend 
or EOverwrite.
@param aFmt The 8 bit non modifiable descriptor containing the format string. 
The TRefByValue class provides a constructor which takes a TDesC8 type. */
	{
	
	VA_LIST list;
	VA_START(list,aFmt);
	DoStaticWriteFormat(aDir,aName,aMode,aFmt,list);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/** Formats the arguments pointed to by aList according to aFmt, and writes the 
result to the log.

The format string aFmt contains literal text, embedded with directives, 
for converting the trailing list of arguments into text. The number and type 
of arguments pointed to by aList is dictated by the structure of the directives 
in aFmt.

Connects to the logging server, creates/opens the log file and writes the text arguments to it.

This is a "static write".

@param aDir The directory relative to 'C:\\Logs\\' where the log file resides.
@param aName The name of the log file.
@param aMode The mode in which the log file should be opened: either EAppend 
or EOverwrite.
@param aFmt The 8 bit non modifiable descriptor containing the format string. 
The TRefByValue class provides a constructor which takes a TDesC8 type. 
@param aList A pointer to an argument list. */
	{

	DoStaticWriteFormat(aDir,aName,aMode,aFmt,aList);
	}

//
// Hex Dumps
//

EXPORT_C void RFileLogger::HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
/** Writes a hex dump of the specified data to the log.

The format of the hex dump entry is a header followed by the hex string of 
the data followed by any printable characters (non printable characters are 
substituted with '.'). For example,

@code
RLog_Ex0000 : 41 42 6C 6B 0A 0A 45 46 20 20 78 7A                       ABlk..EF 
 xz
RL_cont0001 : 43 44 6C 6B 0A 0A 45 46 20 20 78 7A                       CDlk..EF 
 xz
RL_cont0002 : 45 46 6C 6B 0A 0A 47 48 20 20 78 7A                       EFlk..GH 
 xz
.
.
.
@endcode
@param aHeader A label for the hex dump entry. The label has a sequence number 
appended to it automatically.
@param aMargin A continuation label if the hex dump exceeds one line. This 
label is displayed on all subsequent lines after line one and also has a sequence 
number appended to it.
@param aPtr The data that is to be converted to a hex string.
@param aLen How many of the characters in aPtr are to be converted. Conversion 
always starts from position 0 within aPtr. */
	{
	
	if (iLogFile.Valid() || LogSTI())
		DoHexDump(aHeader,aMargin,aPtr,aLen);
	}

EXPORT_C void RFileLogger::HexDump(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
/** Writes a hex dump of the specified data to the log.

The format of the hex dump entry is a header followed by the hex string of 
the data followed by any printable characters (non printable characters are 
substituted with '.'). For example,

@code
RLog_Ex0000 : 41 42 6C 6B 0A 0A 45 46 20 20 78 7A                       ABlk..EF 
 xz
RL_cont0001 : 43 44 6C 6B 0A 0A 45 46 20 20 78 7A                       CDlk..EF 
 xz
RL_cont0002 : 45 46 6C 6B 0A 0A 47 48 20 20 78 7A                       EFlk..GH 
 xz
.
.
.
@endcode


@param aDir The directory relative to 'C:\\Logs\\' where the log file resides.
@param aName The name of the log file.
@param aMode The mode in which the log file should be opened: either EAppend 
or EOverwrite.
@param aHeader A label for the hex dump entry. The label has a sequence number 
appended to it automatically.
@param aMargin A continuation label if the hex dump exceeds one line. This 
label is displayed on all subsequent lines after line one and also has a sequence 
number appended to it.
@param aPtr The data that is to be converted to a hex string.
@param aLen How many of the characters in aPtr are to be converted. Conversion 
always starts from position 0 within aPtr. */
	{

	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		logger.CreateLog(aDir,aName,aMode);
		if (logger.iLogFile.Valid())
			{
			logger.SetDateAndTime(ETrue,ETrue);
			logger.DoHexDump(aHeader,aMargin,aPtr,aLen);
			}
		logger.CloseLog();
		}
	logger.Close();
	}

//
// Private functions
//

TInt RFileLogger::DoConnect()
/**
Connects to the flogger server - default number of message slots = 8 

@return TInt indicating success code (KErrNone) or an error code.
*/
	{

	return CreateSession(KFLoggerServerName,Version(),0);	// no async IPC
	}

void RFileLogger::DoWrite(const TDesC8& aBuf)
/**
Sends the pre-formatted write string to the flogger server. 

@pre
session is already open. 
@param aBuf  8-bit text to be written. 
*/
	{
	if (LogSTI()) 
		{
		TBuf<KLogBufferSize> n;
		n.Copy(aBuf);
		TFileName logFileName = iLogFile.Name();
		RDebug::Print(_L("FLG %S %S"),&logFileName,&n);
		} 
	else 		
		{
		TPckg<TLogFile> logFilePckg(iLogFile);
		iLastError=SendReceive(EWriteLog,TIpcArgs (&logFilePckg, &aBuf));
		}
	}

void RFileLogger::DoStaticWrite(const TDesC8& aBuf)
/**
Sends the pre-formatted write string to the flogger server. 

@pre 
session is already open. aText is not longer than KLogBufferSize 
@param aBuf  text to write  
@post
The text is only written if the original connection was successful. 
No text is ever written if the directory specified in the original connection request does not exist. 
Each line is preceded in the date and time.
*/
	{
 	TPckg<TLogFile> logFilePckg(iLogFile);
 	if (LogSTI()) 
 		{
 	 	TBuf<KLogBufferSize> n;
 	 	n.Copy(aBuf);
		TFileName logFileName = iLogFile.Name();
		RDebug::Print(_L("FLG %S %S"),&logFileName,&n);
        }
    else
        {
        SendReceive(ECreateWriteAndCloseLog, TIpcArgs( &logFilePckg, &aBuf));       // ignore error
        }
	}		

void RFileLogger::DoWriteFormat(TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
/**
Trim format string before sending to the flogger server. 

@pre
session is already open. 
@param aFmt  c-style formatted text to be written.  
@param aList  any variables required by the format.  
@post
The final string is truncated to KLogBufferSize.
@see void RFileLogger::DoWriteFormat(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
*/
	{		

	if (iLogFile.Valid() || LogSTI())
		{
		TBuf8<KLogBufferSize> buf;
		iLastError=iFormatter.ConvertToWritableBuffer(buf,aFmt,aList);
		if (iLastError==KErrNone)
			DoWrite(buf);
		}
	}

void RFileLogger::DoWriteFormat(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/**
Trim format string before sending to the flogger server. 

@pre
session is already open. 
@param aFmt  c-style formatted text to be written.  
@param aList  any variables required by the format.  
@post
The final string is truncated to KLogBufferSize.
*/
	{		

	if (iLogFile.Valid() || LogSTI())
		{
		TBuf8<KLogBufferSize> buf;
		iLastError=iFormatter.ConvertToWritableBuffer(buf,aFmt,aList);
		if (iLastError==KErrNone)
			DoWrite(buf);
		}
	}

void RFileLogger::DoStaticWriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
/**
Sends a format write string to the flogger server to write to the specified file.

@param aDir Path of the log file.
@param aName Name of the log file.
@param aFmt  c-style formatted text to be written.  
@param aList  any variables required by the format.  
@post
The text is only written if the original connection was successful. 
No text is ever written if the directory specified in the original connection request does not exist. 
Each line is preceded in the date and time.
@see void RFileLogger::DoStaticWriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
*/
	{

	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime(ETrue,ETrue);
		logger.iLogFile.Set(aDir,aName,aMode);
		TBuf8<KLogBufferSize> buf;
		ret=logger.iFormatter.ConvertToWritableBuffer(buf,aFmt,aList);
		if (ret==KErrNone)
			logger.DoStaticWrite(buf);
		}
	logger.Close();
	}

void RFileLogger::DoStaticWriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/**
Sends a format write string to the flogger server to write to the specified file.
There is no pre-condition.

@param aDir Path of the log file.
@param aName Name of the log file.
@param aFmt  c-style formatted text to be written.  
@param aList  any variables required by the format.  
@post 
The text is only written if the original connection was successful. 
No text is ever written if the directory specified in the original 
connection request does not exist. Each line is preceded in the date and time.
*/
	{

	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime(ETrue,ETrue);
		logger.iLogFile.Set(aDir,aName,aMode);
		TBuf8<KLogBufferSize> buf;
		ret=logger.iFormatter.ConvertToWritableBuffer(buf,aFmt,aList);
		if (ret==KErrNone)
			logger.DoStaticWrite(buf);
		}
	logger.Close();
	}

void RFileLogger::DoHexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
/**
Static Write. Dumps arbitrary data to the log file as a standard hex dump. 

@pre 
session is already open. 
@param aHeader  Specify a string to be printed before the first hex line. 
Leave as null or an empty string for no header.  
@param aMargin  Specify a string to be printed before each subsequent line. 
Leave as null or an empty string for no Margin.  
@param aPtr  pointer to the data being dumped.  
@param aLen  the number of bytes to dump  
@post
The text is only written if the original connection was successful. 
No text is ever written if the directory specified in the original connection
request does not exist. Each line is preceded in the date and time. 
@note This function has poor performance since it performs a full connection and 
disconnection to the flogger server. Example of aHeader/aMargin. 
If "aHeader" is set to "Fn Output:" and "aMargin" is set to " ", then output 
would look like (for a print of the alphabet):
14/11/2002	12:32:24	 Fn Output:0000 : 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 abcdefghijklmnop 
14/11/2002	12:32:24	  	   0010 : 71 72 73 74 75 76 77 78 79 7a qrstuvwxyz
*/
	{

	if (aPtr==NULL)		// nothing to do
		return;

	TBuf<KLogBufferSize> buf;
	TBuf8<KLogBufferSize> temp;
	TInt i=0;
	const TText* p=aHeader;
	while (aLen>0)
		{
		if (p==NULL)
			p=BLANK;	// if NULL set p to a blank string
		TInt n=(aLen>KHexDumpWidth ? KHexDumpWidth : aLen);
		buf.AppendFormat(KFirstFormatString,p,i);
		TInt j;
		for (j=0; j<n; j++)
			buf.AppendFormat(KSecondFormatString,aPtr[i+j]);
		while (j++<KHexDumpWidth)
			buf.Append(KThreeSpaces);
		buf.Append(KTwoSpaces);
		for (j=0; j<n; j++)
			buf.AppendFormat(KThirdFormatString,(aPtr[i+j]<32 || aPtr[i+j]>126) ? KFullStopChar : aPtr[i+j]);
		
		iLastError=iFormatter.FormatTextToWritableBuffer(temp,buf);
		if (iLastError==KErrNone)
			DoWrite(temp);
		
		buf.SetLength(0);
		temp.SetLength(0);
		aLen-=n;
		i+=n;
		p=aMargin;
		}
	}

