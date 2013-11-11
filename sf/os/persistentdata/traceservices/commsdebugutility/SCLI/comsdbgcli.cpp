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
// Implements the Flogger client side
// 
//

/**
 @file
 @internalTechnology
*/

#include <f32file.h>
#include <comms-infras/commsdebugutility.h>
#include "comsdbgstd.h"
#include <e32def.h>

#include <utf.h>

#define BLANK	_S("")

const TInt KHexDumpWidth=16;			///< Number of bytes written per line when formatting as hex.
const TInt KNumberMessageSlots=1;       ///< Number of message slots on flogger client.no asynchronous IPC so never need more than 1 slot
const TInt KLowestPrintableCharacter = 32; ///< In Hex output, replace chars below space with a dot.
const TInt KHighestPrintableCharacter = 126; ///< In Hex output, replace chars above 7-bits with a dot.

_LIT(KFirstFormatString,"%s%04x : ");   ///< Format string used in Hexdump to format first part: header and byte numbers.
_LIT(KSecondFormatString,"%02x ");      ///< Format string used in Hexdump to format mid part: each of the 16 bytes as hex
_LIT(KThirdFormatString,"%c");          ///< Format string used in Hexdump to format the last part: each of the 16 bytes as characters
_LIT(KThreeSpaces,"   ");               ///< Format string used in Hexdump to define padding between first and mid parts
_LIT(KTwoSpaces," ");                   ///< Format string used in Hexdump to define padding between hex and char bytes.

_LIT8(KFirstFormatString8,"%04x : ");   ///< Format string used in Hexdump to format first part: header and byte numbers.
_LIT8(KSecondFormatString8,"%02x ");      ///< Format string used in Hexdump to format mid part: each of the 16 bytes as hex
_LIT8(KThirdFormatString8,"%c");          ///< Format string used in Hexdump to format the last part: each of the 16 bytes as characters
_LIT8(KThreeSpaces8,"   ");               ///< Format string used in Hexdump to define padding between first and mid parts
_LIT8(KTwoSpaces8," ");                   ///< Format string used in Hexdump to define padding between hex and char bytes.



//
// RFileLogger class definition
//

EXPORT_C RFileLogger::RFileLogger() : iLoggerBody(NULL)
/**
 * Create a new flogger client interface object with an empty body.
 * @internalTechnology 
 */
	{}

EXPORT_C RFileLogger::~RFileLogger()
/**
 * Destructor
 * @internalTechnology 
 */
	{}

EXPORT_C TVersion RFileLogger::Version() const
/**
 * Return the client side version number
 * @internalTechnology 
 * @return TVersion 3-part version number: major, minor, build.
 */
	{

	return(TVersion(KFLogSrvMajorVersionNumber,KFLogSrvMinorVersionNumber,KFLogSrvBuildVersionNumber));
	}

EXPORT_C TInt RFileLogger::Connect()
/**
 Connect to the flogger server - default number of message slots = 1
 @internalTechnology 
 @return TInt indicating success code (KErrNone), KErrNoMemory if failed to allocate log body
         or an error from RSessionBase::CreateSession.
         KErrAlreadyExists if Connect has already been called.
 */
	{
	if (iLoggerBody)
		{
		return KErrAlreadyExists;
		}
	iLoggerBody = new RFileLoggerBody;
	if (iLoggerBody)
		{
		TInt ret=DoConnect();
		if (ret==KErrNotFound)
			{
			ret=FLogger::Start();
			if (ret==KErrNone || ret==KErrAlreadyExists)
				ret=DoConnect();
			}
		if (ret != KErrNone)
			{
			// we had a problem (perhaps no memory) so kill loggerbody again
			delete iLoggerBody;
			iLoggerBody = NULL;
			}
			
		return ret;
		} 
	else
		{
		//OOM, so return KErrNoMemory so that OOM tests know this
		return KErrNoMemory;
		}
	}

EXPORT_C void RFileLogger::Close()
/**
 * Close a client side session with the flogger server.
 * @internalTechnology 
 * @post The client session is closed and the body of the class is deleted.
 *       Further calls to the Write functions will fail silently until a new session is opened.
 */
	{
	if (iLoggerBody)
		{
		iLoggerBody->Close();
		delete iLoggerBody;
		}
	iLoggerBody = NULL;
	}

EXPORT_C void RFileLogger::SetDateAndTime(TBool /*aUseDate*/,TBool /*aUseTime*/)
/**
 * Does nothing.
 * @internalTechnology 
 * @removed    This function no longer needed since now logging to one file and
 *             date/time comes from system.
 */
	{}

EXPORT_C TInt RFileLogger::SetLogTags(const TDesC8& aSubsystem, const TDesC8& aComponent)
/**
 * Set the two tag strings that all further writes by this client will use to
 * idenitfy it in the log file.
 * @internalTechnology 
 * @param aSubsystem	Specifies the tag1 name that goes into the log file
 * @param aComponent	specifies the tag2 name that goes into the log file
 * @post  The client session is updated so that all future calls use this tag set.
 *        Tags are truncated to KMaxTagLength.
 * @return TInt indicating success code (KErrNone) or an error code.
 * @note If an error occurs, the client connection will be silently closed to protect
 *       the client.
 */
	{
	TPtrC8 validSubsystem;
	TPtrC8 validComponent;
		
	validSubsystem.Set(aSubsystem.Left(KMaxTagLength));
	validComponent.Set(aComponent.Left(KMaxTagLength));
	return DoSetLogTags(validSubsystem, validComponent);
	}

EXPORT_C void RFileLogger::CreateLog(const TDesC& aSubsystem, const TDesC& aComponent, TFileLoggingMode /*aMode*/)
/**
 * Sets the log tags.
 * @internalTechnology 
 * @removed             Not fully supported since flogger only uses one log file. Use SetLogTags instead.
 * @param aSubsystem	Specifies the tag1 name that goes into the log file
 * @param aComponent	specifies the tag2 name that goes into the log file
 * @param aMode         not used
 * @note	            This function is partially supported and is equivalent to calling SetLogTags.
 * @see SetLogTags
 */
	{

	TNameTag narrowComponent;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowComponent,aComponent);
	TNameTag narrowSubsystem;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowSubsystem,aSubsystem);
	
	(void)DoSetLogTags(narrowSubsystem,narrowComponent);
	}

EXPORT_C void RFileLogger::CloseLog()
/**
 * Close a client side session with the flogger server.
 * @internalTechnology 
 * @deprecated   With the advent of a single log file for all clients, closing the log file is no longer necessary. Use Close to close the session.
 * @see          Close
 */
	{
	Close();
	}

EXPORT_C TBool RFileLogger::LogValid() const
/**
 * Always returns ETrue.
 * @internalTechnology 
 * @removed  With the advent of a single log file for all clients, checking for log validity is no longer necessary.
 * @return   ETrue always.
 */
	{
	return ETrue;
	}

EXPORT_C TInt RFileLogger::LastError() const
/**
 * Always returns KErrNone
 * @internalTechnology 
 * @removed  Flogger no longer retains internal errors.
 * @return   KErrNone always.
 */
	{
	return KErrNone;
	}

EXPORT_C TInt RFileLogger::ClearLog()
/**
 * Request that the server empty the log file.
 * @internalTechnology 
 * @pre  The client requesting the log be cleared must be listed in the flogger "ini" file
 *       as an enabled logging client. This prevents unwanted clients clearing the log.
 *       The session with the server must be active, otherwise this will fail silently.
 * @post A message is added to the server write queue that indicates to clear the log.
 *       Once the message reaches the head of the queue flogger will empty the log file
 *       and begin filling it again.
 * @return TInt indicating success code (KErrNone) or an error code.
 */
	{
	if (IsLogging())
		{
		__ASSERT_ALWAYS(iLoggerBody,User::Panic(KFloggerPanic, EInternalConsistencyFault));
		return iLoggerBody->DoSendReceive(EClearLog, TIpcArgs());
		}
	else
		{
		return KErrNone;
		}
	}

//
// 16-bit non-static writes
//

EXPORT_C void RFileLogger::Write(const TDesC16& aText)
/**
 * Write 16-bit aText to the log file.
 * @internalTechnology 
 * @pre  The client requesting to log must be listed in the flogger "ini" file
 *       as an enabled logging client, otherwise no logging will occur.
 *       The session with the server must be active, otherwise this will fail silently.
 * @param aText Text to write
 * @post The 16-bit text is converted to 8-bit text before writing, and is truncated to KLogBufferSize
 *       if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 @note There is no need to supply CR, LF. If these are supplied it may cause the log output to be incorrect.
 */
	{
	if (!IsLogging())
		{
		return;
		}

	TPtrC16 textValid;
	textValid.Set(aText.Left(KLogBufferSize));
	TBuf8<KLogBufferSize> buf;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf,textValid);
	DoWrite(buf);
	}
	
EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC16> aFmt,...)
/**
 * Write the formatted 16-bit string aFmt to the log file
 * @internalTechnology 
 * @pre  The client requesting to log must be listed in the flogger "ini" file
 *       as an enabled logging client, otherwise no logging will occur.
 *       The session with the server must be active, otherwise this will fail silently.
 * @param aFmt c-style format descriptor, followed by any variables required by the format.
 * @post The 16-bit text is converted to 8-bit text before writing, and is truncated to KLogBufferSize
 *       if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 */
	{
	if (!IsLogging())
		{
		return;
		}
  //coverity[var_decl]
	VA_LIST list;
	VA_START(list,aFmt);
	//coverity[uninit_use_in_call]
	DoWriteFormat(aFmt,list);
	
	}

EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
/**
 * Write the formatted 16-bit string aFmt to the log file.
 * @internalTechnology 
 * @pre  The client requesting to log must be listed in the flogger "ini" file
 *       as an enabled logging client, otherwise no logging will occur.
 *       The session with the server must be active, otherwise this will fail silently.
 * @param aFmt c-style format descriptor
 * @param aList any variables required by the format.
 * @post The 16-bit text is converted to 8-bit text before writing, and is truncated to KLogBufferSize
 *       if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 */
	{
	if (IsLogging())
		{
		DoWriteFormat(aFmt,aList);
		}
	}

//
// 8-bit non-static writes
//

EXPORT_C void RFileLogger::Write(const TDesC8& aText)
/**
 * Write 8-bit aText to the log file.
 * @internalTechnology 
 * @pre  The client requesting to log must be listed in the flogger "ini" file
 *       as an enabled logging client, otherwise no logging will occur.
 *       The session with the server must be active, otherwise this will fail silently.
 * @param aText  Text to log.
 * @post The text is truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 */
	{
	TPtrC8 textValid;
	textValid.Set(aText.Left(KLogBufferSize));
	if (IsLogging())
		{
		DoWrite(textValid);
		}
	}
	
EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
/**
 * Write the formatted 8-bit string aFmt to the log file.
 * @internalTechnology 
 * @pre  The client requesting to log must be listed in the flogger "ini" file
 *       as an enabled logging client, otherwise no logging will occur.
 *       The session with the server must be active, otherwise this will fail silently.
 * @param aFmt c-style format descriptor, followed by any variables required by the format.
 * @post The text is truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 */
	{
	if (!IsLogging())
		{
		return;
		}
  //coverity[var_decl]		
	VA_LIST list;
	VA_START(list,aFmt);
	//coverity[uninit_use_in_call]  
	DoWriteFormat(aFmt,list);
	}

EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/**
 * Write the formatted 8-bit string aFmt to the log file if it is a valid file.
 * @internalTechnology 
 * @pre  The client requesting to log must be listed in the flogger "ini" file
 *       as an enabled logging client, otherwise no logging will occur.
 *       The session with the server must be active, otherwise this will fail silently.
 * @param aFmt c-style format descriptor
 * @param aList any variables required by the format.
 * @post The text is truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 */
	{
	if (IsLogging())
		{
		DoWriteFormat(aFmt,aList);
		}
	}

EXPORT_C void RFileLogger::WriteBinary(const TDesC8& aData)
/**
 * Dump arbitrary data to the log file in a binary format.
 * @internalTechnology 
 * @pre  The client requesting to log must be listed in the flogger "ini" file
 *       as an enabled logging client, otherwise no logging will occur.
 *       The session with the server must be active, otherwise this will fail silently.
 * @param aData Descriptor of the data to be dumped
 * @post The 8-bit binary dump is preceded in the log file by the two client tags
 *
 * @note Unlike all other write API's, no thread ID is written with this API.
 */
	{
	if (IsLogging())
		{
		__ASSERT_ALWAYS(iLoggerBody,User::Panic(KFloggerPanic, EInternalConsistencyFault));
		(void)iLoggerBody->DoSendReceive(EWriteBinary, TIpcArgs(&aData, aData.Length()));
		}
	}


//
// 16-bit static writes
//

EXPORT_C void RFileLogger::Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC16& aText)
/**
 * Static write. Write 16-bit aText to the log file if it is a valid file.
 * @internalTechnology 
   @pre The client requesting to log must be listed in the flogger "ini" file
         as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aMode not used
 * @param aText Text to write
 * @post The text is converted to 8-bit text before writing, and truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	// truncate tags
	TPtrC8 validSubsystem;
	TPtrC8 validComponent;
			
	validSubsystem.Set(aSubsystem.Left(KMaxTagLength));
	validComponent.Set(aComponent.Left(KMaxTagLength));

	TBuf8<KLogBufferSize> buf;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf,aText);

	DoStaticWrite(validSubsystem, validComponent, buf);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, const TRefByValue<const TDesC16> aFmt,...)
/**
 * Static write. Write the formatted 16-bit string aFmt to the log file.
 * @internalTechnology 
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aFmt c-style format descriptor, followed by any variables required by the format.
 * @post The text is converted to 8-bit text before writing, and truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	//coverity[var_decl]
	VA_LIST list;
	VA_START(list,aFmt);
	
	// truncate tags
	TPtrC8 validSubsystem;
	TPtrC8 validComponent;
		
	validSubsystem.Set(aSubsystem.Left(KMaxTagLength));
	validComponent.Set(aComponent.Left(KMaxTagLength));
	//coverity[uninit_use_in_call]
	DoStaticWriteFormat(validSubsystem,validComponent,aFmt,list);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, const TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
/**
 * Static write. Write the formatted 16-bit string aFmt to the log file.
 * @internalTechnology 
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aFmt c-style format descriptor
 * @param aList any variables required by the format.
 * @post The text is converted to 8-bit text before writing, and truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	// truncate tags
	TPtrC8 validSubsystem;
	TPtrC8 validComponent;
		
	validSubsystem.Set(aSubsystem.Left(KMaxTagLength));
	validComponent.Set(aComponent.Left(KMaxTagLength));
	
	DoStaticWriteFormat(validSubsystem,validComponent,aFmt,aList);
	}

//
// 8-bit static writes
//

EXPORT_C void RFileLogger::Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aText)
/**
 * Static write. Write 8-bit aText to the log file.
 * @internalTechnology 
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aText  Text to log.
 * @post The text is truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	// truncate tags
	TPtrC8 validSubsystem;
	TPtrC8 validComponent;
			
	validSubsystem.Set(aSubsystem.Left(KMaxTagLength));
	validComponent.Set(aComponent.Left(KMaxTagLength));
	DoStaticWrite(validSubsystem,validComponent, aText);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, const TRefByValue<const TDesC8> aFmt,...)
/**
 * Static write. Write the formatted 8-bit string aFmt to the log file.
 * @internalTechnology 
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aFmt c-style format descriptor, followed by any variables required by the format.
 * @post The text is truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */

	{
	//coverity[var_decl]
	VA_LIST list;
	VA_START(list,aFmt);
	
	// truncate tags
	TPtrC8 validSubsystem;
	TPtrC8 validComponent;
		
	validSubsystem.Set(aSubsystem.Left(KMaxTagLength));
	validComponent.Set(aComponent.Left(KMaxTagLength));
	//coverity[uninit_use_in_call]
	DoStaticWriteFormat(validSubsystem,validComponent,aFmt,list);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, const TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/**
 * Static write. Write the formatted 16-bit string aFmt to the log file.
 * @internalTechnology 
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aFmt c-style format descriptor
 * @param aList any variables required by the format.
 * @post The text is truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	//truncate tags
	TPtrC8 validSubsystem;
	TPtrC8 validComponent;
		
	validSubsystem.Set(aSubsystem.Left(KMaxTagLength));
	validComponent.Set(aComponent.Left(KMaxTagLength));
		
	DoStaticWriteFormat(validSubsystem,validComponent,aFmt,aList);
	}






//
// Removed 16-bit static writes
//

EXPORT_C void RFileLogger::Write(const TDesC& aSubsystem, const TDesC& aComponent, TFileLoggingMode /*aMode*/, const TDesC16& aText)
/**
 * Static write. Write 16-bit aText to the log file if it is a valid file.
 * @internalTechnology 
 * @removed With the advent of a single log file for all clients, this function has been replaced by an equivalent without the aMode parameter.
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aMode not used
 * @param aText Text to write
 * @post The text is converted to 8-bit text before writing, and truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	// the convert also truncates if necessary	
	TNameTag narrowComponent;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowComponent,aComponent);
	TNameTag narrowSubsystem;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowSubsystem,aSubsystem);

	Write(narrowSubsystem, narrowComponent, aText);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aSubsystem, const TDesC& aComponent, TFileLoggingMode aMode, TRefByValue<const TDesC16> aFmt,...)
/**
 * Static write. Write the formatted 16-bit string aFmt to the log file.
 * @internalTechnology 
 * @removed With the advent of a single log file for all clients, this function has been replaced by an equivalent without the aMode parameter.
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aMode not used
 * @param aFmt c-style format descriptor, followed by any variables required by the format.
 * @post The text is converted to 8-bit text before writing, and truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	// Just to remove the warning otherwise this does nothing 
	if (aMode == EFileLoggingModeUnknown) { }
	//coverity[var_decl]
	VA_LIST list;
	VA_START(list,aFmt);
	
	// the convert also truncates if necessary	
	TNameTag narrowComponent;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowComponent,aComponent);
	TNameTag narrowSubsystem;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowSubsystem,aSubsystem);
	//coverity[uninit_use_in_call]
	DoStaticWriteFormat(narrowSubsystem, narrowComponent,aFmt,list);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aSubsystem, const TDesC& aComponent, TFileLoggingMode /*aMode*/, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
/**
 * Static write. Write the formatted 16-bit string aFmt to the log file.
 * @internalTechnology 
 * @removed With the advent of a single log file for all clients, this function has been replaced by an equivalent without the aMode parameter.
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aMode not used
 * @param aFmt c-style format descriptor
 * @param aList any variables required by the format.
 * @post The text is converted to 8-bit text before writing, and truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	// the convert also truncates if necessary	
	TNameTag narrowComponent;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowComponent,aComponent);
	TNameTag narrowSubsystem;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowSubsystem,aSubsystem);
	
	DoStaticWriteFormat(narrowSubsystem, narrowComponent,aFmt,aList);
	}

//
// Removed 8-bit static writes
//

EXPORT_C void RFileLogger::Write(const TDesC& aSubsystem, const TDesC& aComponent, TFileLoggingMode /*aMode*/, const TDesC8& aText)
/**
 * Static write. Write 8-bit aText to the log file.
 * @internalTechnology 
 * @removed With the advent of a single log file for all clients, this function has been replaced by an equivalent without the aMode parameter.
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aMode not used
 * @param aText  Text to log.
 * @post The text is truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	// the convert also truncates if necessary
	TNameTag narrowComponent;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowComponent,aComponent);
	TNameTag narrowSubsystem;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowSubsystem,aSubsystem);
	
	Write(narrowSubsystem, narrowComponent, aText);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aSubsystem, const TDesC& aComponent, TFileLoggingMode aMode, TRefByValue<const TDesC8> aFmt,...)
/**
 * Static write. Write the formatted 8-bit string aFmt to the log file.
 * @internalTechnology 
 * @removed With the advent of a single log file for all clients, this function has been replaced by an equivalent without the aMode parameter.
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aMode not used
 * @param aFmt c-style format descriptor, followed by any variables required by the format.
 * @post The text is truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */

	{
	// Just to remove the warning otherwise this does nothing 
	if (aMode == EFileLoggingModeUnknown) { }
	//coverity[var_decl]
	VA_LIST list;
	VA_START(list,aFmt);
	
	// the convert also truncates if necessary	
	TNameTag narrowComponent;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowComponent,aComponent);
	TNameTag narrowSubsystem;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowSubsystem,aSubsystem);
	//coverity[uninit_use_in_call]
	DoStaticWriteFormat(narrowSubsystem, narrowComponent, aFmt, list);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aSubsystem, const TDesC& aComponent, TFileLoggingMode /*aMode*/, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/**
 * Static write. Write the formatted 16-bit string aFmt to the log file.
 * @internalTechnology 
 * @removed With the advent of a single log file for all clients, this function has been replaced by an equivalent without the aMode parameter.
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aMode not used
 * @param aFmt c-style format descriptor
 * @param aList any variables required by the format.
 * @post The text is truncated to KLogBufferSize if necessary.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 */
	{
	// the convert also truncates if necessary	
	TNameTag narrowComponent;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowComponent,aComponent);
	TNameTag narrowSubsystem;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowSubsystem,aSubsystem);
	
	DoStaticWriteFormat(narrowSubsystem, narrowComponent,aFmt,aList);
	}

//
// Hex Dumps
//

EXPORT_C void RFileLogger::HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
/**
 * Dump arbitrary data to the log file as a standard hex dump.
 * @internalTechnology 
 * @pre   The session with the server must be active, otherwise this no action is taken.
         The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aHeader Specify a zero-terminated string to be printed before the first hex line. Leave as null or an empty string for no header.
 * @param aMargin Specify a zero-terminated string to be printed before each subsequent line. Leave as null or an empty string for no Margin.
 * @param aPtr pointer to the data being dumped.
 * @param aLen the number of bytes to dump
 * @post Each line is preceded in the log file by the two client tags and the client thread ID.
 * @note Example of aHeader/aMargin. If "aHeader" is set to "Fn Output:" and "aMargin" is set to "         ", then output would look
 *       like (for a print of the alphabet):
 *       TLOG	Example	20	FnOutput:0000 : 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70  abcdefghijklmnop
 *       TLOG	Example	20	         0010 : 71 72 73 74 75 76 77 78 79 7a                    qrstuvwxyz
 *   
 */
	{
	if (IsLogging())
		{
		DoHexDump(aHeader,aMargin,aPtr,aLen);
		}


	}



EXPORT_C void RFileLogger::HexDump(const TDesC8& aData, const TDesC8& aHeader)
/**
 * Dump arbitrary data to the log file as a standard hex dump.
 * @internalTechnology 
 * @pre   The session with the server must be active, otherwise this no action is taken.
          The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aData the data being dumped.
 * @param aHeader Specify a string to be printed before the first hex line. If not supplied, no header or margin is written.
 *        If supplied, then subsequent lines are indented to the length of aHeader.
 * @post Each line is preceded in the log file by the two client tags and the client thread ID.
 * @note Example of aHeader. If "aHeader" is set to "Fn Output:" then output would look
 *       like (for a print of the alphabet):
 *       TLOG	Example	20	FnOutput:0000 : 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70  abcdefghijklmnop
 *       TLOG	Example	20	         0010 : 71 72 73 74 75 76 77 78 79 7a                    qrstuvwxyz
 *   
 */
	{
	if (IsLogging())
		{
		DoHexDump(aData,aHeader,TPtrC8(NULL,0));
		}


	}






EXPORT_C void RFileLogger::HexDump(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aData, const TDesC8& aHeader)
/**
 * Static Write. Dump arbitrary data to the log file as a standard hex dump.
 * @internalTechnology 
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aData the data being dumped.
 * @param aHeader Specify a string to be printed before the first hex line. If not supplied, no header or Margin is written.
 * @param aMargin Specify a string to be printed before each subsequent line. If not supplied, a string of spaces equal to the length of aHeader is used.
 * @post Each line is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 *       Example of aHeader/aMargin. If "aHeader" is set to "Fn Output:" and "aMargin" is set to "         ", then output would look
 *       like (for a print of the alphabet):
 *       TLOG	Example	20	FnOutput:0000 : 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70  abcdefghijklmnop
 *       TLOG	Example	20	         0010 : 71 72 73 74 75 76 77 78 79 7a                    qrstuvwxyz
 *   
 */
	{
	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		ret = logger.SetLogTags(aSubsystem,aComponent);
		if (((ret == KErrNone) && logger.iLoggerBody) && logger.iLoggerBody->iLoggingOnPckg())
			{
			logger.DoHexDump(aData,aHeader,TPtrC8(NULL,0));
			}
		logger.Close();
		}
	}





EXPORT_C void RFileLogger::HexDump(const TDesC& aSubsystem, const TDesC& aComponent, TFileLoggingMode /*aMode*/, const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
/**
 * Static Write. Dump arbitrary data to the log file as a standard hex dump.
 * @internalTechnology 
 * @removed With the advent of a single log file for all clients, this function has been replaced by an equivalent without the aMode parameter.
   @pre The client requesting to log must be listed in the flogger "ini" file
        as an enabled logging client, otherwise no logging will occur.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aMode not used
 * @param aHeader Specify a zero-terminated string to be printed before the first hex line. Leave as null or an empty string for no header.
 * @param aMargin Specify a zero-terminated string to be printed before each subsequent line. Leave as null or an empty string for no Margin.
 * @param aPtr pointer to the data being dumped.
 * @param aLen the number of bytes to dump
 * @post Each line is preceded in the log file by the two client tags and the client thread ID.
 *       "aSubsystem" and "aComponent" are each truncated to KMaxTagLength.
 * @note This function has poor performance since it performs a full connection and disconnection to the flogger server.
 *       Example of aHeader/aMargin. If "aHeader" is set to "Fn Output:" and "aMargin" is set to "         ", then output would look
 *       like (for a print of the alphabet):
 *       TLOG	Example	20	FnOutput:0000 : 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70  abcdefghijklmnop
 *       TLOG	Example	20	         0010 : 71 72 73 74 75 76 77 78 79 7a                    qrstuvwxyz
 *   
 */
	{
	// the convert also truncates if necessary
	TNameTag narrowComponent;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowComponent,aComponent);
	TNameTag narrowSubsystem;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowSubsystem,aSubsystem);
	
	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		ret = logger.SetLogTags(narrowSubsystem,narrowComponent);
		if (((ret == KErrNone) && logger.iLoggerBody) && logger.iLoggerBody->iLoggingOnPckg())
			{
			logger.DoHexDump(aHeader,aMargin,aPtr,aLen);
			}
		logger.Close();
		}

	}


EXPORT_C TInt RFileLogger::Handle() const
// Returns handle of session, or Null if no session.
	{
	if (iLoggerBody)
		{
		return iLoggerBody->Handle();
		}
	else
		{
		return 0;
		}
	}

EXPORT_C TInt RFileLogger::Share() 
	{
	if (iLoggerBody)
		{
  		return iLoggerBody->ShareAuto();
		}
	return KErrSessionClosed;
	}


//
// Debug tools to check for memory leaks
//

EXPORT_C void RFileLogger::__DbgShutDownServer()
/**
 * Debugging Tool. Ask the flogger server to shutdown. Only valid in DEBUG builds.
 * @internalTechnology 
 */
	{
#ifdef _DEBUG
	if (iLoggerBody)
		(void)iLoggerBody->DoSendReceive(EShutDownServer);
#endif
	}

EXPORT_C void RFileLogger::__DbgSetHeapFailure(TInt aFailAfter)
/**
 * Debugging Tool. Ask the flogger server to set its heap failure. Only valid in DEBUG builds.
 * @internalTechnology 
 * @param aFailAfter The number of successful memory allocations which will occur before
 *        a memory allocation is failed by the memory manager.
 */
	{
#ifdef _DEBUG
	if (iLoggerBody)
		{
		(void)iLoggerBody->DoSendReceive(ESetHeapFailure, TIpcArgs(aFailAfter));
		}
#else
	(void)aFailAfter;
#endif
	}

//
// Private functions
//

TInt RFileLogger::DoConnect()
/**
 * Connect to the flogger server
 * @return TInt indicating success code (KErrNone) or an error code.
 * @note: creates 1 slot: no asynchronous IPC so never need more than 1 reserved message slot.
 @internalComponent
 */
	{
	__ASSERT_ALWAYS(iLoggerBody,User::Panic(KFloggerPanic, EInternalConsistencyFault));
	return iLoggerBody->DoCreateSession(KFLoggerServerName,Version(),KNumberMessageSlots);
	}

TInt RFileLogger::DoSetLogTags(const TDesC8& aSubsystem, const TDesC8& aComponent)
/**
 * Set the two tag strings that all further writes by this client will use to
 * identify it in the log file.
 * @param aSubsystem	Specifies the tag1 name that goes into the log file
 * @param aComponent	specifies the tag2 name that goes into the log file
 * @return TInt indicating success code (KErrNone), or (KErrNotFound) if
  the connection is not valid, or an error code from SendReceive.
 * @note If an error occurs, the client connection will be silently closed to protect
 *       the client.
 */
	{
	TInt err(KErrNone);
	if (iLoggerBody)	//check that the connection was set up correctly
		{
		err = iLoggerBody->DoSendReceive(ESetLogTag, TIpcArgs(&aSubsystem, &aComponent, &(iLoggerBody->iLoggingOnPckg)));
		if (err !=KErrNone )
			{	//Something went wrong. We need to protect the client because error can be ignored.
			Close();
			}
		}
	else
		{
		err = KErrNotFound;
		}
	return err;
	}

void RFileLogger::DoWrite(const TDesC8& aBuf)
/**
 * Send to the flogger server the pre-formatted write string.
 * @internalTechnology 
 * @pre session is already open.
 * @param aBuf 8-bit text to be written. It must not exceed KLogBufferSize.
 */
	{
	__ASSERT_ALWAYS(iLoggerBody,User::Panic(KFloggerPanic, EInternalConsistencyFault));
	(void)iLoggerBody->DoSendReceive(EWriteToLog,TIpcArgs(&aBuf, aBuf.Length()));	
	}

void RFileLogger::DoWriteFormat(TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
/**
 * Trim and convert format string before sending to the flogger server.
 * @pre session is already open.
 * @param aFmt c-style formatted text to be written.
 * @param aList any variables required by the format.
 * @post The final string is truncated to KLogBufferSize and converted to 8-bit.
 */
	{
	TBuf16<KLogBufferSize> wideBuf;
	__ASSERT_ALWAYS(iLoggerBody,User::Panic(KFloggerPanic, EInternalConsistencyFault));
	wideBuf.AppendFormatList(aFmt, aList, &(iLoggerBody->iFlogOverflow16));
	TBuf8<KLogBufferSize> narrowBuf;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowBuf,wideBuf);
	DoWrite(narrowBuf);
	}

void RFileLogger::DoWriteFormat(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/**
 * Trim format string before sending to the flogger server.
 * @pre session is already open.
 * @param aFmt c-style formatted text to be written.
 * @param aList any variables required by the format.
 * @post The final string is truncated to KLogBufferSize.
 */
	{
	TBuf8<KLogBufferSize> buf;
	__ASSERT_ALWAYS(iLoggerBody,User::Panic(KFloggerPanic, EInternalConsistencyFault));
	buf.AppendFormatList(aFmt,aList, &(iLoggerBody->iFlogOverflow8));
	DoWrite(buf);
	}
	
void RFileLogger::DoSendStaticWrite(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aText)
/**
 * Send to the flogger server the pre-formatted write string.
 * @pre session is already open.
 *      aText is not longer than KLogBufferSize
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aText text to write
 * @post The text is only written if the tag1+tag2 combination is listed as an enabled client
 *       in the flogger "ini" file, otherwise no action is taken.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 */
	{
	__ASSERT_ALWAYS(iLoggerBody,User::Panic(KFloggerPanic, EInternalConsistencyFault));
	(void)iLoggerBody->DoSendReceive(EStaticWriteToLog,TIpcArgs(&aSubsystem, &aComponent, &aText, aText.Length()));		// ignore error
	}	

void RFileLogger::DoStaticWrite(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aText)
/**
 * Send to the flogger server the pre-formatted write string.
 * @pre session is already open.
 *      aText is not longer than KLogBufferSize
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aText text to write
 * @post The text is only written if the tag1+tag2 combination is listed as an enabled client
 *       in the flogger "ini" file, otherwise no action is taken.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 */
	{
	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		TPtrC8 textValid;
		textValid.Set(aText.Left(KLogBufferSize));

		logger.DoSendStaticWrite(aSubsystem, aComponent, textValid);
		}

	logger.Close();
	}	



void RFileLogger::DoStaticWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
/**
 * Send to the flogger server a format write string.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aFmt c-style formatted text to be written.
 * @param aList any variables required by the format.
 * @post The text is only written if the tag1+tag2 combination is listed as an enabled client
 *       in the flogger "ini" file, otherwise no action is taken.
 *       If necessary, the final string is truncated to KLogBufferSize.
 *       The final string is converted to an 8-bit string.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 */
	{
	TFlogOverflow16 objFlogBody16;
	TBuf<KLogBufferSize> wideBuf;
	TBuf8<KLogBufferSize> narrowBuf;

    wideBuf.AppendFormatList(aFmt, aList, &objFlogBody16);
	CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowBuf,wideBuf);
		
    DoStaticWrite(aSubsystem, aComponent, narrowBuf);
	}

void RFileLogger::DoStaticWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/**
 * Send to the flogger server a format write string.
 * @param aSubsystem Specifies the tag1 name that goes into the log file
 * @param aComponent specifies the tag2 name that goes into the log file
 * @param aFmt c-style formatted text to be written.
 * @param aList any variables required by the format.
 * @post The text is only written if the tag1+tag2 combination is listed as an enabled client
 *       in the flogger "ini" file, otherwise no action is taken.
 *       If necessary, the final string is truncated to KLogBufferSize.
 *       The text is preceded in the log file by the two client tags and the client thread ID.
 */	{
	TFlogOverflow8 objFlogBody8;
	TBuf8<KLogBufferSize> buf;
	buf.AppendFormatList(aFmt, aList, &objFlogBody8);
	
    DoStaticWrite(aSubsystem, aComponent, buf);
	}

void RFileLogger::DoHexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
/**
 * Static Write. Dump arbitrary data to the log file as a standard hex dump.
 * @pre session is already open.
 * @param aHeader Specify a zero-terminated string to be printed before the first hex line. Leave as null or an empty string for no header.
 * @param aMargin Specify a zero-terminated string to be printed before each subsequent line. Leave as null or an empty string for no Margin.
 * @param aPtr pointer to the data being dumped.
 * @param aLen the number of bytes to dump
 * @post The text is only written if the tag1+tag2 combination is listed as an enabled client
 *       in the flogger "ini" file, otherwise no action is taken.
 *       Each line is preceded in the log file by the two client tags and the client thread ID.
 *       The data is written in lines of 16 characters.
 * @note Example of aHeader/aMargin. If "aHeader" is set to "Fn Output:" and "aMargin" is set to "         ", then output would look
 *       like (for a print of the alphabet):
 *       TLOG	Example	20	FnOutput:0000 : 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70  abcdefghijklmnop
 *       TLOG	Example	20	         0010 : 71 72 73 74 75 76 77 78 79 7a                    qrstuvwxyz
 *   
 */
	{
	// this delightful art-deco code lifted straight from old flogger
	if (aPtr==NULL)		// nothing to do
		{
		return;
		}

	TBuf<KMaxHexDumpWidth> buf;
	TBuf8<KMaxHexDumpWidth> temp;


	TInt i=0;
	const TText* p=aHeader;
	while (aLen>0)
		{
		if (p==NULL)
			{
			p=BLANK;	// if NULL set p to a blank string
			}
		TInt n=(aLen>KHexDumpWidth ? KHexDumpWidth : aLen);
		__ASSERT_ALWAYS(iLoggerBody,User::Panic(KFloggerPanic, EInternalConsistencyFault));
		buf.AppendFormat(KFirstFormatString,&(iLoggerBody->iFlogOverflow16),p,i);
		TInt j;
		for (j=0; j<n; j++)
			{
			buf.AppendFormat(KSecondFormatString,aPtr[i+j]);
			}
		while (j++<KHexDumpWidth)
			{
			buf.Append(KThreeSpaces);
			}
		buf.Append(KTwoSpaces);
		for (j=0; j<n; j++)
			{
			buf.AppendFormat(KThirdFormatString,(aPtr[i+j]<KLowestPrintableCharacter || aPtr[i+j]>KHighestPrintableCharacter) ? KFullStopChar : aPtr[i+j]);
			}
		
		CnvUtfConverter::ConvertFromUnicodeToUtf8(temp,buf);
		DoWrite(temp);
		
		buf.SetLength(0);
		temp.SetLength(0);
		aLen-=n;
		i+=n;
		p=aMargin;
		}
	}



void RFileLogger::DoHexDump(const TDesC8& aData, const TDesC8& aHeader, const TDesC8& aMargin)
/**
 * Static Write. Dump arbitrary data to the log file as a standard hex dump.
 * @see RFileLogger::HexDump(const TDesC8& aData, const TDesC8& aHeader = 0)
 * @param aMargin - supply a margin - if left null, then a margin of spaces of equal length to "aHeader"
 *                  is used.
 * @pre session is already open.
 */
	{
	HBufC8* marginStr = NULL;
	TBuf8<KMaxHexDumpWidth> buf;
	TInt aRemainingLen = aData.Length();
	TInt aHeaderLen = aHeader.Length();
	
	__ASSERT_ALWAYS(iLoggerBody,User::Panic(KFloggerPanic, EInternalConsistencyFault));
	
	if (aData.Length()==0)		// nothing to do
		{
		return;
		}


	if (aHeaderLen > 0)
		{
		
		if (aMargin.Length() == 0)
			{
			marginStr = HBufC8::New(aHeader.Length());
			if (marginStr == NULL)
				{
				return;		// abort if No memory
				}
			TPtr8 marginStrPtr(marginStr->Des());
			marginStrPtr.AppendFill(' ',aHeader.Length());
			}
		else
			{
			marginStr = aMargin.Alloc();
			}
		}
		
		
	
	TUint blockStartPos = 0;
	while (aRemainingLen>0)
		{
		TInt blockLength = (aRemainingLen>KHexDumpWidth ? KHexDumpWidth : aRemainingLen);
		
		// write the header/margin and print in hex which bytes we are about to write
		if (blockStartPos == 0)
			{
			if (aHeaderLen > 0)
				{
				buf.Append(aHeader);
				}
			buf.AppendFormat(KFirstFormatString8,&(iLoggerBody->iFlogOverflow8), blockStartPos);
			}
		else
			{
			if (marginStr)
				{
				buf.Append(*marginStr);
				}
			buf.AppendFormat(KFirstFormatString8,&(iLoggerBody->iFlogOverflow8),blockStartPos);
			}
		
		TInt bytePos;
		// write the bytes as hex
		for (bytePos = 0; bytePos < blockLength; bytePos++)
			{
			buf.AppendFormat(KSecondFormatString8,aData[blockStartPos + bytePos]);
			}
		while (bytePos++ < KHexDumpWidth)
			{
			buf.Append(KThreeSpaces8);
			}
		buf.Append(KTwoSpaces8);
		// print the bytes as characters, or full stops if outside printable range
		for (bytePos = 0; bytePos < blockLength; bytePos++)
			{
			buf.AppendFormat(KThirdFormatString8,(aData[blockStartPos + bytePos] < KLowestPrintableCharacter || aData[blockStartPos + bytePos] > KHighestPrintableCharacter) ? KFullStopChar8 : aData[blockStartPos + bytePos]);
			}
		
		DoWrite(buf);
		
		buf.SetLength(0);
		aRemainingLen -= blockLength;
		blockStartPos += blockLength;
		}
	delete marginStr;
	}



EXPORT_C void ClientRunStubOrdinal1()
/**
 * @removed This function has been removed because the flogsvrl dll contains the equivalent function
@internalComponent
 */
	{User::Panic(KFloggerPanic, KErrNotSupported);}

