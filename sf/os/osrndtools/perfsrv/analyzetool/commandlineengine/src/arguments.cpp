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
* Description:  Argument parsing functions.
*/
#include "../inc/ATCommonDefines.h"
#include "../inc/CATBase.h"

//Forward declarations.
bool parseBaseArguments( vector<string>& vArgs, ARGUMENTS& args );
bool parseHookArguments( vector<string>& vArgs, ARGUMENTS& args );
bool parseAnalyzeArguments( vector<string>& vArgs, ARGUMENTS& args );
bool parseParseArguments( vector<string>& vArgs, ARGUMENTS& args );
bool checkDataFileName( string& sFileName );
bool checkDataFilePath( string& sFilePath );
bool parseSbsConfiguration( string& sConfiguration, ARGUMENTS& args );

// Constants for old "hooking" parameter parsing.
#define INVALID_PARAMETER "AnalyzeTool : Error, invalid parameter: "
const char DATAFILENAME_INVALID_CHARS[] = " &^+-@$*()|\\/[]{}<>?;:,\"'";
const char DATAFILEPATH_INVALID_CHARS[] = " &^+-@$%*()|/[]{}<>?;,\"'";

/**
* Check datafile name for invalid characters.
* @return true if file name ok.
*/
bool checkDataFileName( string& sFileName )
{
	for ( size_t i = 0; i < sFileName.length(); i++ )
	{
		const char c = sFileName.at( i );
		if( strchr( DATAFILENAME_INVALID_CHARS, c ) != 0 )
			return false;
	}
	return true;
}

/**
* Check datafile path for invalid characters and correct format.
* @return true if file path ok.
*/
bool checkDataFilePath( string& sFilePath )
{
	for ( size_t i = 0; i < sFilePath.length(); i++ )
	{
		char c = sFilePath.at( i );
		if( strchr( DATAFILEPATH_INVALID_CHARS, c ) != 0 )
			return false;
		// first char must be drive letter a-z
		if( i == 0 && ( c = tolower( c ) < 'a' || c > 'z' ) )
			return false;
		// if last char is not '\', add it
		if( i == sFilePath.length()-1 &&  c != '\\' )
			sFilePath.append( "\\" );
	}

	// ':\' after drive letter
	if( sFilePath.find( ":\\" ) != 1 )
		return false;

	// there can be only one ':' on pos 1
	if( sFilePath.find( ":", 2 ) != string::npos )
		return false;

	//check double slashes
	if( sFilePath.find( "\\\\" ) != string::npos )
		return false;

	return true;
}

/**
* Parse base arguments from given vector of strings.
* Removes debug / help arguments from vector.
*/
bool parseBaseArguments( vector<string>& vArgs, ARGUMENTS& args )
{
	// Iterator used in this function.
	vector<string>::iterator it;
	// If no arguments set show help true.
 	if ( vArgs.size() == 0 )
	{
		args.eMainSwitch = SWITCH_UNKNOWN;
		args.bHelp = true;
	}
	//Try find help and debug switches.
	//Note: -help is main switch what shows syntax examples.
	for(it = vArgs.begin(); it != vArgs.end(); it++ )
	{
		//Help switches.
		if ( ! _stricmp( (*it).c_str(), "-?" ) )
		{
			args.bHelp = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "--?" ) )
		{
			args.bHelp = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "--help" ) )
		{
			args.bHelp = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "/?" ) )
		{
			args.bHelp = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		//Debug switches.
		else if ( ! _stricmp( (*it).c_str(), "-show_debug" ) )
		{
			args.bDebugConsole = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "--show_debug" ) )
		{
			args.bDebugConsole = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "-show_debug_all" ) )
		{
			args.bDebugConsole = true;
			args.bDebugLowLevel = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "--show_debug_all" ) )
		{
			args.bDebugConsole = true;
			args.bDebugLowLevel = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "-show_dbgview" ) )
		{
			args.bDebugDbgView = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "--show_dbgview" ) )
		{
			args.bDebugDbgView = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "-show_dbgview_all" ) )
		{
			args.bDebugDbgView = true;
			args.bDebugLowLevel = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		else if ( ! _stricmp( (*it).c_str(), "--show_dbgview_all" ) )
		{
			args.bDebugDbgView = true;
			args.bDebugLowLevel = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
		//Raptor switch.
		else if ( ! _stricmp( (*it).c_str(), "-sbs2" ) )
		{
			args.bEnableSbs2 = true;
			it = vArgs.erase( it );
			if ( it == vArgs.end() )
				break;
		}
	}
	if ( vArgs.size() > 0 )
	{
		//Pick up main switch.
		it = vArgs.begin();
		if ( ! _stricmp( (*it).c_str(), "-a" ) )
			args.eMainSwitch = SWITCH_ANALYZE;
		else if ( ! _stricmp( (*it).c_str(), "-p" ) )
			args.eMainSwitch = SWITCH_PARSE_TRACE;
		else if ( ! _stricmp( (*it).c_str(), "-c" ) )
			args.eMainSwitch = SWITCH_CLEAN;
		else if ( ! _stricmp( (*it).c_str(), "-v" ) )
			args.eMainSwitch = SWITCH_VERSION;
		else if ( ! _stricmp( (*it).c_str(), "-vdbghelp" ) )
			args.eMainSwitch = SWITCH_DBGHELP_VERSION;
		else if ( ! _stricmp( (*it).c_str(), "-help" ) )
			args.eMainSwitch = SWITCH_HELP;
		else if ( ! _stricmp( (*it).c_str(), "-me" ) )
		{
			cout << AT_MSG << "This feature is no longer supported. You can use -tr parameter for output to trace. \n" << endl;
		}
		else if ( ! _stricmp( (*it).c_str(), "-e" ) ||  ! _stricmp( (*it).c_str(), "-tr" ) )
		{
			args.eMainSwitch = SWITCH_HOOK;
			args.eHookSwitch = HOOK_EXTERNAL_FAST;
		}
		else if ( ! _stricmp( (*it).c_str(), "-mi" ) )
		{
			cout << AT_MSG << "This feature is no longer supported. You can use -lf parameter for logging to file. \n" << endl;
		}
		else if ( ! _stricmp( (*it).c_str(), "-lf" ) )
		{
			args.eMainSwitch = SWITCH_HOOK;
			args.eHookSwitch = HOOK_INTERNAL;
		}
		else if ( ! _stricmp( (*it).c_str(), "-instrument_i" ) )
		{
			args.eMainSwitch = SWITCH_HOOK;
			args.eHookSwitch = HOOK_EXTENSION_INTERNAL;
		}
		else if ( ! _stricmp( (*it).c_str(), "-instrument_e" ) )
		{
			args.eMainSwitch = SWITCH_HOOK;
			args.eHookSwitch = HOOK_EXTENSION_EXTERNAL;
		}
		else if ( ! _stricmp( (*it).c_str(), "-instrument_ef" ) )
		{
			args.eMainSwitch = SWITCH_HOOK;
			args.eHookSwitch = HOOK_EXTENSION_EXTERNAL_FAST;
		}
		else if ( ! _stricmp( (*it).c_str(), "-uninstrument" ) )
		{
			args.eMainSwitch = SWITCH_UNHOOK;
			args.eHookSwitch = HOOK_EXTENSION_UNINSTRUMENT;
		}
		else if ( ! _stricmp( (*it).c_str(), "-uninstrument_failed" ) )
		{
			args.eMainSwitch = SWITCH_UNHOOK;
			args.eHookSwitch = HOOK_EXTENSION_FAILED;
		}
	}
	return true;
}

/**
* Parse analyze related arguments from given vector of strings.
*/
bool parseAnalyzeArguments( vector<string>& vArgs, ARGUMENTS& args )
{
	bool bRet = true;
	if ( vArgs.size() < 2 )
	{
		cout << AT_MSG << "Error, missing datafile." << endl;
		return false;
	}
	// Iterator used in this function.
	vector<string>::const_iterator it;
	for(it = vArgs.begin()+1; it != vArgs.end(); it++ )
	{
		if ( it->find("-l") != string::npos )
		{
			if ( it->length() == 3 )
			{
				// Create char array for atoi function
				char level[2];
				level[0] = it->at(2);
				level[1] = 0; // null terminate
				// check that its digit first
				if ( isdigit(level[0]) )
				{
					// pass array to atoi
					int iLoggingLevel = atoi( level );
					if ( iLoggingLevel >= 0 && iLoggingLevel <= 3 )
					{
						// log level ok
						args.ANALYZE.iLoggingLevel = iLoggingLevel;
						continue;
					}
				}
				bRet = false;
				cout << AT_MSG << "Invalid logging level specified (0-3)." << endl;
				args.ANALYZE.iLoggingLevel = -1;
			}
		}
		// No else here because logging level check is done to all args in list.
		// Rom symbol file
		if( _stricmp( it->c_str(), "-s" ) == 0 )
		{
			it++;
			if ( it == vArgs.end() )
			{
				bRet = false;
				cout << AT_MSG << "Missing symbol file." << endl;
				break; // Leave for loop.
			}
			else
			{
				args.ANALYZE.bSymbolFile = true;
				args.ANALYZE.vSymbolFiles.push_back( *it );
				continue;
			}
		}
		else 
		{
			// If we got datafile we must assume this is output
			if( ! args.ANALYZE.sDataFile.empty() )
			{
				if ( args.ANALYZE.sOutputFile.empty() )
					args.ANALYZE.sOutputFile = *it;
				else
				{
					bRet = false;
					cout << AT_MSG << "Invalid parameter: " << *it << endl;
				}
			}
			// If this is file we assume datafile
			else if( CATBase::FileExists( it->c_str() ) )
			{
				args.ANALYZE.sDataFile = *it;
			}
			else
			{
				bRet = false;
				cout << AT_MSG << "Specified datafile does not exist." << endl;
			}
		}
	}
	if ( args.ANALYZE.sDataFile.empty() )
		bRet = false;
	return bRet;
}


/**
* Parse hooking related arguments from given vector of strings.
*/
bool parseHookArguments( vector<string>& vArgs, ARGUMENTS& args )
{
	bool bRet = true;
	try {
		// Iterator used in this function.
		vector<string>::const_iterator it;

		// Check that we have some arguments except main switch.
		if ( vArgs.size() < 2 )
		{
			if ( args.eHookSwitch == HOOK_EXTENSION_UNINSTRUMENT
				|| args.eHookSwitch == HOOK_EXTENSION_FAILED
				)
				return bRet;
			cout << AT_MSG << "Error, Missing build command." << endl;
			bRet = false;
		}
		bool bBuildFound = false;
		for(it = vArgs.begin()+1; it != vArgs.end(); it++ )
		{
			// If's to pickup atool options
			// no build switch
			if ( _stricmp( it->c_str(), "-nobuild" ) == 0 )
			{
				args.HOOK.bNoBuild = true;
			}
			// call stack size(s)
			else if ( _stricmp( it->c_str(), "-acs" ) == 0 || _stricmp( it->c_str(), "-fcs" ) == 0 )
			{
				// Free vs Alloc
				bool bAlloc = true;
				if ( _stricmp( it->c_str(), "-fcs" ) == 0 )
					bAlloc = false;
				// Value
				it++;
				if ( it== vArgs.end() )
				{
					bRet = false;
					cout << AT_MSG << "Error, missing call stack size parameter." << endl;
					break;
				}
				else if ( ! _stricmp( it->c_str(), "sbs" ) 
					|| ! _stricmp( it->c_str(), "abld" )
					|| ! _stricmp( it->c_str(), "-f" )
					|| ! _stricmp( it->c_str(), "-fp" ) )
				{
					bRet = false;
					cout << AT_MSG << "Error, missing call stack size parameter." << endl;
					break;
				}
				else
				{
					int i;
					// Try to parse integer value using stream.
					istringstream ss( *it );
					if ( ss>>i )
					{
						// Value parsed ok now check bounds.
						if ( i < AT_CALL_STACK_SIZE_MIN  )
						{
							bRet = false;
							cout << AT_MSG << "Error, specified call stack size value too small." << endl;
							break;
						}
						else if ( i > AT_CALL_STACK_SIZE_MAX )
						{
							bRet = false;
							cout << AT_MSG << "Error, specified call stack size value too big." << endl;
							break;
						}
						else
						{
							// Value valid.
							if ( bAlloc )
								args.HOOK.iAllocCallStackSize = i;
							else
								args.HOOK.iFreeCallStackSize = i;
						}
					}
					else
					{
						// Error parsing value using stream.
						bRet = false;
						cout << AT_MSG << "Error, specified call stack size value invalid." << endl;
						break;
					}

				}
			}
			// Data file name.
			else if ( _stricmp( it->c_str(), "-f" ) == 0 )
			{
				it++;
				if ( it == vArgs.end() )
				{
					bRet = false;
					cout << AT_MSG << "Error, missing log file name." << endl;
					break;
				}
				else if ( ! _stricmp( it->c_str(), "sbs" ) || ! _stricmp( it->c_str(), "abld" ) )
				{
					bRet = false;
					cout << AT_MSG << "Error, missing log file name." << endl;
					break;
				}
				else
				{
					if ( checkDataFileName( string( *it ) ) )
					{
						// Pickup filename.
						args.HOOK.bDataFileName = true;
						args.HOOK.sDataFileName = *it;
					}
					else
					{
						bRet = false;
						cout << AT_MSG << "Error, specified log file name contains invalid character(s)." << endl;
						break;
					}
				}
			}
			// Data file path.
			else if ( _stricmp( it->c_str(), "-fp" ) == 0 )
			{
				it++;
				if ( it == vArgs.end() )
				{
					bRet = false;
					cout << AT_MSG << "Error, missing path for log file." << endl;
					break;
				}
				else if ( ! _stricmp( it->c_str(), "sbs" ) || ! _stricmp( it->c_str(), "abld" ) )
				{
					bRet = false;
					cout << AT_MSG << "Error, missing path for log file." << endl;
					break;
				}
				else
				{
					string sFormattedPath = string(*it);
					if ( checkDataFilePath( sFormattedPath ) )
					{
						// Pickup filename.
						args.HOOK.bDataFilePath = true;
						args.HOOK.sDataFilePath = sFormattedPath;
					}
					else
					{
						bRet = false;
						cout << AT_MSG << "Error, specified log file path contains invalid character(s) "
							<< "or is in wrong format. Please, check help for correct format," << endl;
						break;
					}
				}
			}
			// Build command parsing.
			else if ( _stricmp( it->c_str(), "sbs" ) == 0 )
			{
				// By default sbs command is not valid.
				bRet = false;
				// By default build found
				bBuildFound = true;
				// Use raptor build system, pickup all rest arguments to sbs commmand.
				bool bFoundConfig = false; // Is configuration defined.
				args.HOOK.iBuildSystem = 2;
				vector<string>::const_iterator itC = it;
				args.HOOK.sBuildCmd.clear();
				for ( ; itC != vArgs.end() ; itC++ )
				{
					args.HOOK.sBuildCmd.append( *itC );
					args.HOOK.sBuildCmd.append( " " );
					args.HOOK.vBuildCmd.push_back( *itC );
				}
				// Remove last space
				if ( args.HOOK.vBuildCmd.size() > 1 )
					args.HOOK.sBuildCmd.erase( args.HOOK.sBuildCmd.size()-1 );

				// Parse needed variables from sbs command.
				vector<string>::iterator itSbs;
				for( itSbs = args.HOOK.vBuildCmd.begin(); itSbs != args.HOOK.vBuildCmd.end() ; itSbs++ )
				{
					// Program(s).
					if ( _stricmp( itSbs->c_str(), "-p" ) == 0 )
					{
						// Next is program.
						itSbs++;
						args.HOOK.vTargetPrograms.push_back( *itSbs );
					}
					else if ( itSbs->find( "--project=" ) != string::npos )
					{
						itSbs->erase(0, 10 );
						args.HOOK.vTargetPrograms.push_back( *itSbs );
					}
					// platform & build type ( configuration )
					else if ( _stricmp( itSbs->c_str(), "-c" ) == 0 || itSbs->find( "--config=" ) != string::npos )
					{
						// Error message if config found more than once.
						if ( bFoundConfig )
						{
							cout << AT_MSG << "Error, no support defining more than one configuration." << endl;
							bRet = false;
							continue;
						}

						if (_stricmp( itSbs->c_str(), "-c" ) == 0 )
						{
							// Next is the configuration
							itSbs++;
							// Check that iterator is valid.
							if ( itSbs == args.HOOK.vBuildCmd.end() )
								break;
						}
						else
						{
							// Remove the "--config=".
							itSbs->erase( 0, 9 );
							// Check its not empty.
							if ( itSbs->size() == 0 )
								break;
						}
						
						// Identify configuration, if successful set sbs command as valid.
						if ( parseSbsConfiguration( *itSbs, args ) )
							bRet = true;
						// Set we encountered one configuration.
						bFoundConfig = true;
					}
				}
				// Error message if command is missing configuration.
				if ( !bFoundConfig )
					cout << AT_MSG << "Error, missing configuration definition from sbs cmd." << endl;
			}
			else if ( _stricmp( it->c_str(), "abld" ) == 0 )
			{
				bBuildFound = true;
				// Use abld build system, pickup all rest argumenst as abld options.
				args.HOOK.iBuildSystem = 1;
				
				vector<string>::const_iterator itC = it;
				args.HOOK.sBuildCmd.clear();
				for ( ; itC != vArgs.end() ; itC++ )
				{
					args.HOOK.sBuildCmd.append( *itC );
					args.HOOK.sBuildCmd.append( " " );
					args.HOOK.vBuildCmd.push_back( *itC );
				}
				
				string sCmd( args.HOOK.sBuildCmd ); // build command to lower case here.
				for( size_t i = 0 ; i < sCmd.size(); i++ )
					sCmd.at(i) = tolower( sCmd.at(i) );

				// Remove all until platform
				if ( sCmd.find("build ") != string::npos )
				{
					// Check is test defined
					if ( sCmd.substr(0, sCmd.find("build ")).find("test") != string::npos )
						args.HOOK.bAbldTest = true;
					sCmd.erase( 0, sCmd.find("build")+6 );
				}
				else
					return false;
				
				//Is -debug switch in command?
				if( sCmd.find( "-debug " ) != string::npos )
				{
					sCmd.erase( sCmd.find( "-debug " ), 7 );
				}

				// Parse needed "variables" from command.
				bool bOk = false;

				// Find platform
				if ( sCmd.find( "armv5" ) != string::npos )
				{
					bOk = true;
					args.HOOK.sPlatform = "armv5";
					sCmd.erase( sCmd.find( "armv5" ), 5 );
				}
				else if ( sCmd.find( "winscw" ) != string::npos )
				{
					bOk = true;
					args.HOOK.sPlatform = "winscw";
					sCmd.erase( sCmd.find( "winscw" ), 6 );
				}
				else if ( sCmd.find( "gcce" ) != string::npos )
				{
					bOk = true;
					args.HOOK.sPlatform = "gcce";
					sCmd.erase( sCmd.find( "gcce" ), 4 );
				}
				if ( bOk )
				{
					// Feature variant.
					if ( sCmd.at(0 ) == '.' )
					{
						sCmd.erase(0,1);
						args.HOOK.sFeatureVariant = sCmd.substr( 0, sCmd.find_first_of(' ') );
						sCmd.erase(0, sCmd.find_first_of(' ')+1 );
					}
				}
				else
				{
					// not platform specified.
					cout << AT_MSG << "Error, no supported platform found in abld parameters (armv5/winscw/gcce)." << endl;
					bRet = false;
				}
				
				// find build type
				bOk = false;
				if (  sCmd.find( "urel" ) != string::npos )
				{
					bOk = true;
					args.HOOK.sBuildType = "urel";
					sCmd.erase( sCmd.find( "urel" ), 4 );
				}

				else if ( sCmd.find( "udeb" ) != string::npos )
				{
					bOk = true;
					args.HOOK.sBuildType = "udeb";
					sCmd.erase( sCmd.find( "udeb" ), 4 );
				}
				if( !bOk )
				{
					// no build type specified.
					cout << AT_MSG << "Error, no build type specified in abld parameters (udeb/urel)." << endl;
					bRet = false;
				}
		
				// Is there multiple programs (only should be used from extension).
				if ( sCmd.find(" -p") != string::npos )
				{
					sCmd.erase( sCmd.find(" -p" ), sCmd.size() - sCmd.find(" -p" ) );
					// Loop thru all parameters and pick up programs.
					vector<string>::iterator it;
					for( it = args.HOOK.vBuildCmd.begin(); it != args.HOOK.vBuildCmd.end(); it++ )
					{
						if ( _stricmp( it->c_str(), "-p" ) == 0 )
						{
							// Next is program.
							it++;
							string sProgram = *it;
							// Make sure program name ends with ".mmp".
							CATBase::ChangeToLower( sProgram );
							if ( sProgram.length() >= 4 )
							{
                                string sEnd = sProgram.substr( sProgram.length()-4, 4 );
								if ( sEnd.compare( ".mmp" ) != 0 )
									sProgram.append( ".mmp" );
							}
							else
								sProgram.append( ".mmp" );
							args.HOOK.vTargetPrograms.push_back( sProgram );
						}
					}
				}
				else {
					// find single defined program.
					if ( sCmd.find_first_not_of(' ') != string::npos )
					{
						size_t iS = sCmd.find_first_not_of(' ');
						size_t iE = sCmd.find_first_of(' ', iS );
						string sProgram;
						if ( iE == string::npos )
							sProgram = sCmd.substr( iS, sCmd.size()-iS );
						else
							sProgram =  sCmd.substr( iS, iE-iS);
						// Make sure program name ends with ".mmp".
						CATBase::ChangeToLower( sProgram );
						if ( sProgram.length() >= 4 )
						{
                            string sEnd = sProgram.substr( sProgram.length()-4, 4 );
							if ( sEnd.compare( ".mmp" ) != 0 )
								sProgram.append( ".mmp" );
						}
						else
							sProgram.append( ".mmp" );
						args.HOOK.vTargetPrograms.push_back( sProgram );
					}
				}
			}
			else
			{
				if ( ! bBuildFound )
				{
					bRet = false;
					cout << AT_MSG << "Error, invalid parameter :" << *it << endl;
					break;
				}
			}
		}
	}
	catch(...)
	{
		bRet = false;
		cout << AT_MSG << "Error parsing arguments." << endl;
	}
	return bRet;
}


/**
* Parse trace parsing related arguments from given vector of strings.
*/
bool parseParseArguments( vector<string>& vArgs, ARGUMENTS& args )
{
	// Iterator used in this function.
	vector<string>::const_iterator it = vArgs.begin();

	if ( it == vArgs.end() )
		return false;

	it++;

	if ( it == vArgs.end() )
	{
		cout << AT_MSG << "Error, input file not defined (raw data file)." << endl;
		return false;
	}

	//Input.
	args.PARSE.bDataFile = true;
	args.PARSE.sDataFile = *it;
	
	it++;
	if ( it == vArgs.end() )
	{
	
		cout << AT_MSG << "Error, output file not defined (device data file)." << endl;
		return false;
	}

	//Output.
	args.PARSE.bOutputFile = true;
	args.PARSE.sOutputFile = *it;
	return true;
}

/**
* Identifies/parses the configuration string when raptor used (-c / --config= ).
*/
bool parseSbsConfiguration( string& sConfiguration, ARGUMENTS& args )
{
	CATBase::ChangeToLower( sConfiguration );
	vector<string> tokens = CATBase::ParseStringToVector( sConfiguration, '.' );
	
	//we check if winscw/armv5 and udeb/urel is used
	//rest of the "." are variants which we all support by default.

	//first we check if some of aliases is used
	if ( tokens.at(0).compare("armv5_urel") == 0 )
	{
		args.HOOK.sPlatform = "armv5";
		args.HOOK.sBuildType = "urel";
		return true;
	}
	else if ( tokens.at(0).compare("armv5_udeb") == 0 )
	{
		args.HOOK.sPlatform = "armv5";
		args.HOOK.sBuildType = "udeb";
		return true;
	}
	else if ( tokens.at(0).compare("winscw_udeb") == 0)
	{
		args.HOOK.sPlatform = "winscw";
		args.HOOK.sBuildType = "udeb";
		return true;
	}
	else if ( tokens.at(0).compare("winscw_urel") == 0 )
	{
		args.HOOK.sPlatform = "winscw";
		args.HOOK.sBuildType = "urel";
		return true;
	}
	//if 1st token is not an alias, lets try dot configurations
	else if ( tokens.at(0).compare("arm") == 0 )
	{
		// check we have atleast 3 tokens. i.e arm v5 debug x ...
		if ( tokens.size() >= 3 )
		{
			// Next configuration is arm version, we only support v5.
			if ( tokens.at(1).compare( "v5" ) == 0)
			{
				args.HOOK.sPlatform = "armv5";

				// Check next configuration part is debug or release
				if ( tokens.at(2).compare( "udeb" ) == 0)
				{
					args.HOOK.sBuildType = "udeb";
					return true;
				}
				else if ( tokens.at(2).compare( "urel" ) == 0 )
				{
					args.HOOK.sBuildType = "urel";
					return true;
				}
				else
				{
					cout << AT_MSG << "Error in sbs configuration part: " << tokens.at(2) << endl;
				    cout << AT_MSG << "Supported are: udeb,urel." << endl;
				    return false;
				}
			}
			else
			{
				cout << AT_MSG << "Error in sbs configuration part: " << tokens.at(1) << endl;
				cout << AT_MSG << "Supported are: v5." << endl;
				return false;
			}
		}
	}
	
	cout << AT_MSG << "Error in sbs configuration part: " << tokens.at(0) << endl;
	cout << AT_MSG << "Supported are: arm, armv5_urel, armv5_udeb, winscw_udeb, winscw_urel." << endl;
	return false;

}

//EOF
