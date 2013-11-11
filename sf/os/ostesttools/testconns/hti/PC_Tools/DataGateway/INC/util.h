/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*   This file contains the header file of the Util and UtilError.
*/

#ifndef UTIL_H
#define UTIL_H

#pragma warning (push, 3)
#pragma warning( disable : 4702 ) // Unreachable code warning
#pragma warning( disable : 4290 ) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning ( disable : 4786 )

// INCLUDE FILES
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <time.h>
#include <windows.h>
#include <sys/timeb.h>
#include "error.h"

#define ENABLE_LOG_SYNC //makes logging methods synced to access by multiple threads

#ifdef ENABLE_LOG_SYNC 
	#include "sync.h"
#endif

using namespace std;

//**********************************************************************************
// Class UtilError
//
// Error class which is thrown if an exception occurs
// in Util operations. Holds error code and string.
//**********************************************************************************
class UtilError
{
public:
	string iError;
	HRESULT iResult;
	UtilError(string aError, HRESULT aResult)
	{
		iError = aError;
		iResult = aResult;
	}
};

//**********************************************************************************
// Class Util
//
// Provides static utility functions for logging and string handling.
//**********************************************************************************
class Util
{
public: // Enumerations
	enum VerboseLevel {none,     // no output
		               result,   // only result is reported
					   error,    // errors reported
					   info,     // more info reported
					   debug};

public: // Constructors
	/*
	 * Default constructor which initializes logging level.
	 */
	Util(const VerboseLevel aVerbose = none) { iVerbose = aVerbose; }

public:
	/*
	 * Returns current verbose level.
	 */
	static VerboseLevel GetVerboseLevel();

	/*
	 * Sets new verbose level by string "none", "result", "error", "info" or "debug"
	 */
	static void SetVerboseLevel(const string& aVerbose);

	/*
	 * Sets new verbose level by enumeration.
	 */
	static void SetVerboseLevel(const VerboseLevel aVerbose);

	/*
	 * Enables/disables timestamp from output by string "yes" or "no"
	 */
	static void SetTimestamp(const string& aTimestamp);

	/*
	 * Enables/disables timestamp from output. 0 is disabled.
	 */
	static void SetTimestamp(const int aTimestamp);

	/*
	 * Parses command line parameters to map.
	 */
	static void ParseCmdLine(const int aArgc, char **aArgs, map<string, string>& aMap) throw (UtilError);

	/*
	 * Returns value by key from a map.
	 */
	static void GetValue(const string& aKey, const map<string, string>& aMap, string& aValue)  throw (UtilError);

	/*
	 * Reads properties from a file to a map.
	 */
	static void ReadProperties(const string& aFilename, map<string, string>& aMap)  throw (UtilError);

	/*
	 * Prints message to output in error level.
	 */
	static void Error(const string& aMsg);

	/*
	 * Prints message to output in error level with error code number.
	 */
	static void Error(const string& aMsg, const long aCode);

	/*
	 * Prints hex dump of data to output.
	 */
	static void Hex(void *aData, const int aLength);

	/*
	 * Prints hex dump of char table to output.
	 */
	static void Hex(const char aData[]);

	/*
	 * Prints hex dump of string to output.
	 */
	static void Hex(const string& aMsg);

	/*
	 * Prints hex dump of data to output.
	 */
	static void Hex(const unsigned char* aData, const int aLength);

	/*
	 * Prints hex dump of data to output.
	 */
	static void Hex(void* aData, const int aLength, string* aHexDump);

	/*
	 * Prints message to output in result level.
	 */
	static void Log(const string& aMsg);

	/*
	 * Prints message to output in info level.
	 */
	static void Info(const string& aMsg);

	/*
	 * Prints message to output in debug level.
	 */
	static void Debug(const string& aMsg);

	/*
	 * Prints output to a stream.
	 */
	static void Print(ostream& out,
		              const string& aMsg,
					  const VerboseLevel aLevel = none,
					  const long aCode = 0);

	/*
	 * Converts string to lower case.
	 */
	static string& ToLower(string& aString);

	/*
	 * Converts string to upper case.
	 */
	static string& ToUpper(string& aString);
	
	/*
	 * Sets paramvalue if command line parameter is found
	 */
	static void CheckCommandlineParam( const string& paramname, string& paramvalue );

private:
	static VerboseLevel iVerbose;
	static int iTimestamp;
#ifdef ENABLE_LOG_SYNC 
	static Mutex iCritical;
#endif
};

#pragma warning (pop)

#endif

// End of file