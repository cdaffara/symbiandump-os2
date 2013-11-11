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
*   This module contains the implementation of Util class member functions. 
*   Util class provides static utility functions for logging and string
*	manipulation.
*/

// INCLUDE FILES
#include "util.h"

// command line parameters
extern map<string, string> g_parameters;

//**********************************************************************************
// Class Util
//
// Provides static utility functions for logging and string handling.
//**********************************************************************************

// The default verbose level for Util output is error
Util::VerboseLevel Util::iVerbose(Util::error);

// The default output is without timestamp
int Util::iTimestamp(0);

#ifdef ENABLE_LOG_SYNC 
	Mutex Util::iCritical;
#endif

/*-------------------------------------------------------------------------------

    Class: Util

    Method: SetVerboseLevel

    Description: Sets new verbose level by string:
	           
				 "none"
				 "result"
				 "error"
				 "info"
				 "debug"
    
    Parameters: const string& aVerbose : in: Verbose level as string

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::SetVerboseLevel(const string& aVerbose)
{
	string verbose(aVerbose);
	ToLower(verbose);
	if (verbose.compare("result") == 0)
	{
		iVerbose = result;
	}
	else if (verbose.compare("error") == 0)
	{
		iVerbose = error;
	}
	else if (verbose.compare("info") == 0)
	{
		iVerbose = info;
	}
	else if (verbose.compare("debug") == 0)
	{
		iVerbose = debug;
	}
	else
	{
	 	iVerbose = none;
	}
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: SetVerboseLevel

    Description: Sets new verbose level by enumeration.
    
    Parameters: const VerboseLevel aVerbose : in: Verbose level

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::SetVerboseLevel(const VerboseLevel aVerbose)
{
	iVerbose = aVerbose;
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: GetVerboseLevel

    Description: Returns current verbose level.
    
    Parameters: None

    Return Values: VerboseLevel : Current verbose level

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
Util::VerboseLevel Util::GetVerboseLevel()
{
	return iVerbose;
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: SetTimestamp

    Description: Enables/disables timestamp from output by string "yes" or "no".
    
    Parameters: const string& aTimestamp : in : "yes" to enable timestamp, "no" to
	                                            disable.

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::SetTimestamp(const string& aTimestamp)
{
	string timestamp(aTimestamp);
	ToLower(timestamp);
	if (timestamp.compare("true") == 0)
	{
		SetTimestamp(1);
	}
	else
	{
		SetTimestamp(0);
	}
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: SetTimestamp

    Description: Enables/disables timestamp from output. 0 is disabled.
    
    Parameters: const int aTimestamp : in : Enable/disable time stamp

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::SetTimestamp(const int aTimestamp)
{
	iTimestamp = aTimestamp;
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: ParseCmdLine

    Description: Parses command line parameters to map. The map which is passed
	             as argument must contain all valid keys (command line swicthes).
				 Function adds values to map to all keys found from command line.
				 The command line switch is form <key>=<value>. Default values for
				 keys can be specified.
    
    Parameters: const int aArgc           : in     : The number of arguments
	            char **aArgs              : in     : Pointer array to arguments
				map<string, string>& aMap : in/out : Map containing possible command
				                                     line switches (keys). Values of
													 switches are returned as values.

    Return Values: None

    Errors/Exceptions: UtilError

-------------------------------------------------------------------------------*/
void Util::ParseCmdLine(const int aArgc,
						char **aArgs,
						map<string, string>& aMap) throw (UtilError)
{
	for (int i = 1; i < aArgc; i++)
	{
		string p(aArgs[i]);
		string v;
		int idx = p.find_first_of("=");
		if (idx != string::npos)
		{
			if ((idx + 1) == p.length())
			{
				string err("Parameter '" + p.substr(0, idx) + "' without value");
				throw UtilError(err, ERR_UTIL_NO_PARAM_VALUE);
			}
			v = &aArgs[i][idx + 1];
			p = p.substr(0, idx);
		}
		else
		{
			v = "";
		}
		aMap[p] = v;
	}
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: GetValue

    Description: Returns value by key from a map.
    
    Parameters: const string& aKey        : in  : Key to value 
				map<string, string>& aMap : in  : Map containing valid keys
				string& aValue            : out : Value of key

    Return Values: None

    Errors/Exceptions: UtilError

-------------------------------------------------------------------------------*/
void Util::GetValue(const string& aKey,
					const map<string,
					string>& aMap, string& aValue) throw (UtilError)
{
	map<string, string>::const_iterator i = aMap.find(aKey);
	if (i != aMap.end())
	{
		aValue = i->second;
	}
	else
	{
		string err("Unknown parameter '" + aKey + "'");
		throw UtilError(err, ERR_UTIL_UNKNOWN_PARAM);
	}
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: ReadProperties

    Description: Reads properties from a file to a map. Line beginning with
	             hash character ('#') are considered as comments. The form
				 of prooerties in file is <property> = <value>.
    
    Parameters: const string& aFilename   : in  : Filename where properties are read
	            map<string, string>& aMap : out : Map where property values are put

    Return Values: None

    Errors/Exceptions: UtilError

-------------------------------------------------------------------------------*/
void Util::ReadProperties(const string& aFilename,
						  map<string, string>& aMap) throw (UtilError)
{
	ifstream in(aFilename.c_str());
	if (!in)
	{
		string err("Cannot open properties file '");
		err += aFilename + "'.";
		throw UtilError(err, ERR_UTIL_PROPERTIES_NOT_FOUND);
	}
	while (in)
	{
		char tmp[256];
		in.getline(tmp, 256);
		string line(tmp);
		if (line.length() == 0 || line.find_first_of("#") == 0)
		{
			continue;
		}
		int idx = line.find_first_of('=');
		string v, p;
		if (idx != string::npos)
		{
			p = line.substr(0, idx);
			int r = p.find(" ");
			while (r != string::npos)
			{
				p.replace(r, 1, "");
				r = p.find(" ");
			}
			v = line.substr(idx + 1);
			string::iterator i = v.begin();
			int spaces = 0;
			while (i != v.end() && *i == ' ')
			{
				++spaces;
				++i;
			}
			v.erase(0, spaces);
			if (v.length() == 0)
			{
				string err = "Value not specified for parameter '";
				err += line.substr(0, idx) + "'.";
				throw UtilError(err, ERR_UTIL_NO_PROPERTY_VALUE);			
			}
		}
		else 
		{
			p = line;
			v = "";
		}
		aMap[p] = v;
	}
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Error

    Description: Prints message to output in error level.
    
    Parameters: const string& aMsg : in : Error message to output

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Error(const string& aMsg)
{
	Print(cerr, aMsg, error);
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Error

    Description: Prints message to output in error level with error code number.
    
    Parameters: const string& aMsg : in : Error message to output
				const long aCode   : in : Error code to output

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Error(const string& aMsg, const long aCode)
{
	Print(cerr, aMsg, error, aCode);
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Log

    Description: Prints message to output in result level.
    
    Parameters: const string& aMsg : in : Result message to output

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Log(const string& aMsg)
{
	Print(cout, aMsg, result);
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Info

    Description: Prints message to output in info level.
    
    Parameters: const string& aMsg : in : Info message to output

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Info(const string& aMsg)
{
	Print(cout, aMsg, info);
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Debug

    Description: Prints message to output in debug level.
    
    Parameters: const string& aMsg : in : Debug message to output

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Debug(const string& aMsg)
{
	Print(cout, aMsg, debug);
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Print

    Description: Prints output to a stream.
    
    Parameters: ostream& out              : in : Stream where to write
                const string& aMsg        : in : Message to output
                const VerboseLevel aLevel : in : Verbose level of message
				const long aCode          : in : Possible error code if != 0

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Print(ostream& out,
				 const string& aMsg,
				 const VerboseLevel aLevel,
				 const long aCode)
{
#ifdef ENABLE_LOG_SYNC 
	iCritical.Lock();
#endif

	if (aLevel <= iVerbose)
	{
		if (iTimestamp)
		{
			char timestamp[128];
			_strdate(timestamp);
			char tmp[16];
			_strtime(tmp);
			strcat(timestamp, " ");
			strcat(timestamp, tmp);
			struct _timeb tb;
			_ftime(&tb);
			sprintf(tmp, ".%03d", tb.millitm);
			strcat(timestamp, tmp);
			out << "[" << timestamp << "] ";
		}

		out << aMsg;
		//if (aLevel == debug)
		{
			OutputDebugString(aMsg.c_str());
			OutputDebugString("\r\n");
		}
		if (aCode != 0)
		{
			out.setf(ios_base::hex, ios_base::basefield);
			out << " (error: 0x" << aCode << ")";
		}
		out << endl;
	}
#ifdef ENABLE_LOG_SYNC 
	iCritical.Unlock();
#endif
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: ToLower

    Description: Converts string to lower case.
    
    Parameters: string& aString : in/out : String which is converted to lower case

    Return Values: string& : Reference to converted string.

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
string& Util::ToLower(string& aString)
{
	string::iterator p = aString.begin();
	while (p != aString.end())
	{
		*p = tolower(*p);
		++p;
	}
	return aString;
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: ToUpper

    Description: Converts string to upper case.
    
    Parameters: string& aString : in/out : String which is converted to upper case

    Return Values: string& : Reference to converted string.

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
string& Util::ToUpper(string& aString)
{
	string::iterator p = aString.begin();
	while (p != aString.end())
	{
		*p = toupper(*p);
		++p;
	}
	return aString;
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Hex

    Description: Prints hex dump of char table to output.
    
    Parameters: const char aData[] : in : Data to be output as hex dump

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Hex(const char aData[])
{
	string s(aData);
	Hex(s);
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Hex

    Description: Prints hex dump of string to output.
    
    Parameters: const string& aMsg : in : Data to be output as hex dump

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Hex(const string& aMsg)
{
	Hex((const unsigned char *)aMsg.c_str(), aMsg.length());
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Hex

    Description: Prints hex dump of data to output.
    
    Parameters: const unsigned char* aData : in : Pointer to data to be output
	                                              as hex dump
                const int aLength          : in : The lenght of data

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Hex(const unsigned char* aData, const int aLength)
{
	Hex((void *)aData, aLength);
}

/*-------------------------------------------------------------------------------

    Class: Util

    Method: Hex

    Description: Prints hex dump of data to output.
    
    Parameters: void *aData       : in : Pointer to data to be output as hex dump
                const int aLength : in : The lenght of data

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------*/
void Util::Hex(void *aData, const int aLength)
{
	Hex(aData, aLength, NULL);
}

void Util::Hex(void* aData, const int aLength, string* aHexDump)
{
#ifdef ENABLE_LOG_SYNC 
	iCritical.Lock();
#endif

	char line[16];
	int line_len = 16;
	int bytes_to_copy = 0;
	int printed = 0;
	bool print_out = (aHexDump == NULL ? true : false);
	unsigned char *data = (unsigned char *)aData;

	if (print_out)
	{
		printf("\n");
	}
	else
	{
		aHexDump->append("\r\n");
	}
	while (printed < aLength)
	{
		bytes_to_copy = ((aLength - printed >= line_len ? line_len : (aLength - printed)));
		memset(line, 0, sizeof(line));
		memcpy(line, &data[printed], bytes_to_copy);
		for (int j = 0; j < line_len; j++)
		{
			char hex[4];
			sprintf(hex, "%02X ", (unsigned char)line[j]);
			if (print_out)
			{
				printf("%s", hex);
			}
			else
			{
				string s(hex);
				aHexDump->append(s);
			}
		}
		if (print_out) printf(" | ");
		for (int j = 0; j < line_len; j++)
		{
			char c = line[j];
			if (c >= ' ' && c <= 'z')
			{
				if (print_out)
				{
					printf("%c", c);
				}
				else
				{
					char tmp[2];
					sprintf(tmp, "%c", c);
					string s(tmp);
					aHexDump->append(s);
				}
			} 
			else
			{
				if (print_out)
				{
					printf(".");
				}
				else
				{
					aHexDump->append(".");
				}
			}
		}
		if (print_out)
		{
			printf("\n");
		}
		else
		{
			aHexDump->append("\r\n");
		}
		if ((printed - line_len) < aLength)
		{
			printed += 16;
		}
		else
		{
			printed = aLength - printed;
		}
	}
	if (print_out) printf("\n");

#ifdef ENABLE_LOG_SYNC 
	iCritical.Unlock();
#endif
}

/*
 * Sets paramvalue if command line parameter is found
 */
void Util::CheckCommandlineParam( const string& paramname, string& paramvalue )
{
	// Command line parameter overrides ini-file value
	if ( !g_parameters[paramname].empty() )
		paramvalue = g_parameters[paramname];
}

// End of the file