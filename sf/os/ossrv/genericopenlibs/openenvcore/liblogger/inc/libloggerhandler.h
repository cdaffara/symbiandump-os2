/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contained MRT library code tracing macros and class definition.
 *
*/


#ifndef LIB_LOGGER_HANDLER_H
#define LIB_LOGGER_HANDLER_H

//  INCLUDES
#include <e32base.h>
#include "liblogger.h"

// FORWARD DECLARATIONS
 class RFile;
 class RFs;

// FORWARD DECLARATIONS
/*
 * Here, we are providing all utility functions that will be
 * used by all modules. We are expecting that all codes specific
 * to symbian will use active object for asynchronous handling
 * of all their functionalities. In case of thread provide
 * synchnorization for accessing the shared objects.
 */

NONSHARABLE_CLASS ( CLibLogger )
{
private:
	/*
	 * create one session with file server
	 */
	static int CreateFileSession(RFs &aFsSession, RFile &aFile);

	/*
	 * create one session with file server
	 */
    static void GetLogFilename(TDes& aFileName);

	/*
	 * Write message to the log file
	 */
    static TInt WriteMessage(const TDesC8& aMessage);

	/*
	 * Default constructor
	 */
	CLibLogger() {}

	/* all the methods within this class are public and static. */
public:
	/* logging related */

	/*
	 * Message logging interface
	 */
	static int LogMessage(TLibTraceMessageType aLogMessageType,
							char *aFileName,
							int aLine,
							char *aFormat,
							VA_LIST& aMarkerList);

	/*
	 * Only filename and line number with timestamp.
	 */
	static int LogMessage(char *aFileName, int aLine);

	/*
	 * Only message without timestamp
	 */
	static int LogMessage(TLibTraceMessageType aLogMessageType,
							char *aFormat,
							VA_LIST& aMarkerList);

	/*
	 * dumping the message in hex format
	 */
	static int DumpMessage(TLibTraceMessageType aLogMessageType,
	                        char *aFileName,
							int aLine,
							char* aMessage,
							char *aStr,
							int aStrLen = -1);

	/*
	 * dumping the message in hex format
	 */
    static int DumpMessage(char *aStr, int aStrLen);

	/*
	 * dumping the message in hex format
	 */
    static int DumpFormatMessage(TLibTraceMessageType aLogMessageType,
                            char *aFileName, 
							int aLine,  
							char *aMessage,
							char *aFormat,
							VA_LIST& aMarkerList);
	/*
	 * dumping the message in hex format
	 */
    static int DumpFormatMessage(TLibTraceMessageType aLogMessageType,
							char *aMessage,
							char *aFormat,
							VA_LIST& aMarkerList);
};

#endif //LIBC_LOGGER_HANDLER_H


// End of file
