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
*
*/




#ifndef STATCOMMON_H
#define STATCOMMON_H

// external definitions
#include "stat.h"

// current version - edit this for version updates to DLL
#define STAT_VERSION_MAJOR "3"
#define STAT_VERSION_MINOR "4"
#define STAT_VERSION_PATCH "1001"


// amount of time a thread sleeps between each processing cycle
#define STAT_THREAD_SLEEP_TIME 5

#define STAT_INI_NAME _T("stat.ini")


#define STAT_WORKINGPATH_VALUE		_T("c:\\apps\\stat\\Work")

#define STAT_LOGFILEPATH_VALUE		_T("c:\\apps\\stat\\LogFiles")

#define STAT_SCRIPTFILEPATH_VALUE	_T("c:\\apps\\stat\\Scripts")

#define STAT_INSTALLPATH_VALUE	_T("c:\\apps\\stat")


// STAT Desktop and DLL Tester registry settings
#define ST_TEST_KEY						_T("Settings")
#define ST_ITERATIONS					_T("Iterations")

#define ST_DELAY						_T("Delay")
#define ST_DELAY_VALUE					_T("")

#define ST_LOGFILE						_T("Logfile")
#define ST_CMDFILE						_T("Commandfile")
#define ST_RAWCMD						_T("RawCommand")

#define ST_FUDGE						_T("FudgeFactor")
#define ST_FUDGE_VALUE					_T("0")

#define ST_SNAPSHOT						_T("Snapshot")

#define ST_REFDIR						_T("RefDir")
#define ST_REFDIR_VALUE					_T("")

#define ST_ADDRESS						_T("Address")
#define ST_RAWCMD						_T("RawCommand")
#define ST_LINK							_T("LinkIndex")
#define ST_CONNECTIONIDX				_T("ConnectionIndex")
#define ST_CONNECTIONDLL				_T("ConnectionDLL")

#define ST_CONNECTION					_T("Connection")
#define ST_CONNECTION_VALUE				_T("SymbianSerial:COM2")

#define ST_PLATFORM						_T("PlatformIndex")
#define ST_VERIFYREMOVEIMAGES			_T("VerifyRemoveImages")

#define ST_VERIFYIMAGE					_T("VerifyImage")
#define ST_VERIFYIMAGE_VALUE			0

#define ST_LOGTOFILE					_T("LogToFile")
#define ST_LOGTOFILE_VALUE				0


#define ST_SCRIPT						_T("Script")
#define ST_SCRIPT_VALUE					_T("<b><s><e>")

#define ST_CUST_LOGFILE					_T("CustomLogFile")
#define ST_CUST_LOGFILE_VALUE			_T("")

#define ST_APPEND						_T("Append")
#define ST_APPEND_VALUE					0

#define ST_CHKRAWCMD					_T("ChkRawCmd")
#define ST_CHKCMDFILE					_T("ChkCmdFile")
#define ST_CHKVERIF						_T("ChkVerif")
#define ST_CHKSNAP						_T("ChkSnap")
#define ST_CHKLOGTOFILE					_T("ChkLogtofile")

#define ST_WORKINGPATH					_T("WorkingPath")
#define ST_WORKINGPATH_VALUE			_T("c:\\apps\\stat\\Work")

#define ST_LOGFILEPATH					_T("LogFilePath")
#define ST_LOGFILEPATH_VALUE			_T("c:\\apps\\stat\\LogFiles")

#define ST_SCRIPTFILEPATH				_T("ScriptFilePath")
#define ST_SCRIPTFILEPATH_VALUE			_T("c:\\apps\\stat\\Scripts")

#define ST_INSTALLPATH				_T("InstallPath")
#define ST_INSTALLPATH_VALUE			_T("c:\\apps\\stat")

// STAT Service registry settings
#define ST_CONTROLLER_LIST				_T("Controllers")
#define ST_CONNECTION_LIST				_T("Connections")
#define ST_TRANSPORT_TYPE				_T("ServiceTransport")
#define ST_TRANSPORT_PORT				_T("ServicePort")
#define ST_SCRIPTLOCATION				_T("ScriptLocation")
#define ST_LOGLOCATION					_T("LogLocation")

// STAT Service command types
typedef enum STATCommandType
{
	SymbianScript		= 1,
	SymbianError		= 2,
	SymbianDeviceInfo	= 3,
	SymbianServiceLogs	= 4
} STATCOMMANDTYPE;

// default socket communication ports
#define ST_DEFAULTDEVICEPORT			3000
#define ST_DEFAULTSERVICEPORT			3001
#define ST_MAX_CONNECTION_COUNT			25

// script execution states
typedef enum StatState
{
	STAT_RUN,
	STAT_PAUSE,
	STAT_STOP
} STAT_STATE;

#define STAT_APPNAME		"STAT DLL"
#define STAT_SERVICENAME	"STAT Service"

//not used
// logging defaults
//#define STAT_APPNAME	"STAT DLL"
//#define STAT_LOGDIR		"C:\\"
//#define STAT_LOGFILE	"stat_output.log"

// debug file returned from remote device
//#define STAT_DEBUGFILE	"C:\\stat_debug.log"

#define MAX_LOG_MSG_LEN		1024
#define MAX_ERROR_MSG_LEN	256

#ifdef UNICODE
#define MAX_UNICODE_LEN		2048
#endif

// communications
typedef struct STATCommand
{
	unsigned long cIdentifier;	// used as 'char' but 'unsigned long' to cater for word boundaries
//	char cIdentifier;
	unsigned long ulLength;
} STATCOMMAND;

// special command identifiers
#define STAT_FAILURE		'*'
#define STAT_RESYNCID		'?'
#define STAT_REFRESH		'Q'
#define STAT_BEGIN			'B'
#define STAT_END			'E'
#define STAT_REBOOT			'|'


// size of data transmissions
// This is moved to a header file common to desktop 
// and device side code.

// timeouts on no data
#define STAT_MAXTIME		240000	// max time to process a command
#define STAT_RETRYDELAY		330		// delay between checking the port when no data

// timeouts on transmission errors
#define STAT_MAX_ERRORLEN	1024	// max length of error text
#define STAT_MAXERRORS		7		// max errors allowed in a single command

// old #defines used in connect transport
#define STAT_ERRORDELAY		3000	// delay on an error
#define STAT_MAXRETRIES		10		// max retries on error

#endif
