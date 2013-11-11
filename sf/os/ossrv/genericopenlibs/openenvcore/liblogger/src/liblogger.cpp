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
* Description:  Contained MRT logger class implementation.
 *
*/


// INCLUDE FILES

#include "liblogger.h"
#include "libloggerhandler.h"



extern "C" {

// -----------------------------------------------------------------------------
// LibCTracer
// Message logging interface
// -----------------------------------------------------------------------------
//
EXPORT_C int LibTracer(TLibTraceMessageType aLogMessageType,
							char *aFileName,
							int aLine,
							char *aFormat,
							...)
    {
	VA_LIST marker;
	VA_START(marker, aFormat);
    TInt len = CLibLogger::LogMessage(aLogMessageType,
				aFileName,
				aLine,
				aFormat,
				marker);
    VA_END(marker);
    return len;
    }

// -----------------------------------------------------------------------------
// LibCTracer
// Message logging interface
// -----------------------------------------------------------------------------
//
EXPORT_C int LibTracerMarkerList(TLibTraceMessageType aLogMessageType,
							char *aFileName,
							int aLine,
							char *aFormat,
							VA_LIST* aMarkerList)
    {
    return CLibLogger::LogMessage(aLogMessageType,
				aFileName,
				aLine,
				aFormat,
				*aMarkerList);
    }

// -----------------------------------------------------------------------------
// LibCTracerPartialHexDump
// dumping the message in hex format of specific length
// -----------------------------------------------------------------------------
//
EXPORT_C int LibTracerPartialHexDump(TLibTraceMessageType aLogMessageType,
                            char *aFileName,
							int aLine,
							char *aMessage,
							char *aStr,
							int aStrLen)
    {
    return CLibLogger::DumpMessage(aLogMessageType, aFileName, aLine, aMessage, aStr, aStrLen);
    }

// -----------------------------------------------------------------------------
// LibCTracerHexDump
// dumping the message in hex format
// -----------------------------------------------------------------------------
//
EXPORT_C int LibTracerHexDump(TLibTraceMessageType aLogMessageType,
                            char *aFileName,
							int aLine,
							char *aMessage,
							char *aFormat,
							...)
    {
	VA_LIST marker;
	VA_START(marker, aFormat);
    TInt len =  CLibLogger::DumpFormatMessage(aLogMessageType, aFileName, aLine, aMessage, aFormat, marker);
    VA_END(marker);
    return len;
    }

// -----------------------------------------------------------------------------
// LibCTracerHexDump
// dumping the message in hex format
// -----------------------------------------------------------------------------
//
EXPORT_C int LibTracerHexDumpMarkerList(TLibTraceMessageType aLogMessageType,
                            char *aFileName,
							int aLine,
							char *aMessage,
							char *aFormat,
							VA_LIST* aMarkerList)
    {
    return CLibLogger::DumpFormatMessage(aLogMessageType, aFileName, aLine, aMessage, aFormat, *aMarkerList);
    }


// -----------------------------------------------------------------------------
// LibCLineExecTracer
// Only logs filename and line no with timestamp
// -----------------------------------------------------------------------------
//
EXPORT_C int LibLineExecTracer(char *aFileName, int aLine)

    {
    return CLibLogger::LogMessage(aFileName, aLine);
    }

// -----------------------------------------------------------------------------
// LibCMessageTracer
// Only logging/trace message without timestamp
// -----------------------------------------------------------------------------
//

EXPORT_C int LibMessageTracer(TLibTraceMessageType aLogMessageType,
							char *aFormat,
							VA_LIST* aMarkerList)
    {
    return CLibLogger::LogMessage(aLogMessageType, aFormat, *aMarkerList);
    }

// -----------------------------------------------------------------------------
// LibCHexDumpMessagePartTracer
// Only logging/trace message without timestamp
// -----------------------------------------------------------------------------
//

EXPORT_C int LibHexDumpMessagePartTracer(TLibTraceMessageType aLogMessageType,
							char* aMessage,
							char *aFormat,
							VA_LIST* aMarkerList)
	{
	return CLibLogger::DumpFormatMessage(aLogMessageType, aMessage, aFormat, *aMarkerList);
	}



} // extern "C" {


// End of file
