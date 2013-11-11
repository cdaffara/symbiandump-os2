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
* Description:  Defines the entry point for the console application.
*
*/

// Includes.
#include "../inc/ATCommonDefines.h"
#include "../inc/CATDatParser.h"
#include "../inc/CATParseTraceFile.h"
#include "../inc/CATProject.h"

//Debug logging parameters
//To enable debugging as argument to atool.exe use:
//--show_debug / -show_debug : Normal logging in console.
//--show_debug_all / -show_debug_all : Log all (low level functions also) to console.
//--show_dbgview / -show_dbgview : Normal logging to windows debug messages.
//--show_dbgview_all / -show_dbgview_all : Log all (low level functions also) to windows debug messages.

//Return codes (errorlevel) defined in AT_RETURN_CODE structure see ATCommonDefines.h.

extern bool g_bDebugConsole = false;
extern bool g_bDebugDbgView = false;
extern bool g_bDebugLowLevel = false;

//Argument parsing.
extern bool parseBaseArguments( vector<string>& vArgs, ARGUMENTS& args );
extern bool parseHookArguments( vector<string>& vArgs, ARGUMENTS& args );
extern bool parseAnalyzeArguments( vector<string>& vArgs, ARGUMENTS& args );
extern bool parseParseArguments( vector<string>& vArgs, ARGUMENTS& args );

//Helps.
extern void print_help( void );
extern void print_syntax_examples( void );

//AT Library check functions
extern bool CheckATLibrariesArmv5( string sEpocRoot );
extern bool CheckATLibrariesArmv5Abiv2( string sEpocRoot );
extern bool CheckATLibrariesWinscw( string sEpocRoot );

//CLE version functions.
extern int showVersionInfo( void );

//dbghelp.dll version function.
extern int showDbgHelpVersionInfo( bool showVersion );

//Miscelllanaeous functions.
extern bool CheckSBS2Folder( void );

const char DEBUG_PARAMETER_CONSOLE[] = "-debug";
const char DEBUG_PARAMETER_DBGVIEW[] = "-dbgview";
const char SBS2_PARAMETER[] = "-sbs2";
char g_cCurrentDir[MAX_LINE_LENGTH];

//Global compile class objects are neededif ctrl+c is pressed mmp file must be restored.
CATProject project_module;

//Parse object.
CATParseTraceFile Parser;

/**
* Handle process control signals.
*/
BOOL WINAPI HandlerRoutine( DWORD dwCtrlType )
{
	//Run recovery and exit for project if user presses ctrl+c
	//or close signal is received.
	if( dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_CLOSE_EVENT )
	{
		int iMode = project_module.GetMode();
		if ( iMode == CATProject::COMPILE
			|| iMode == CATProject::INSTRUMENT
			|| iMode == CATProject::INSTRUMENT_CONSOLE )
			project_module.RunRecoveryAndExit();
	}
	//Return false so program execution is stopped.
	return false;
}
// TESTING
int _tmain( int argc, _TCHAR* argv[] )
{
	#ifdef MEM_LEAK_CHECK
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	try {
		//Set function to handle process control signals.
		SetConsoleCtrlHandler( HandlerRoutine, true );
		vector<string> vArguments;
		for( int i = 1 ; i < argc ; i++ )
		{
			vArguments.push_back( argv[i] );
		}
		//Help variables.
		string sEpocRoot("");
		//Parse base arguments.
		ARGUMENTS args;
		parseBaseArguments( vArguments, args );

		// Debugging messages.
		if ( args.bDebugConsole == true )
			g_bDebugConsole = true;
		if ( args.bDebugDbgView == true )
			g_bDebugDbgView = true;
		if ( args.bDebugLowLevel == true )
			g_bDebugLowLevel = true;

		//According to main switch parse rest arguments.
		switch( args.eMainSwitch )
		{
		case SWITCH_UNKNOWN:
			print_help();
			return AT_RETURN_CODE::INVALID_ARGUMENT_ERROR;
		case SWITCH_ANALYZE:
			if ( ! parseAnalyzeArguments( vArguments, args ) )
				return AT_RETURN_CODE::INVALID_ARGUMENT_ERROR;
			// Get epocroot
			if( ! CATBase::GetEpocRoot( sEpocRoot ) )
				return AT_RETURN_CODE::CANNOT_FIND_EPOCROOT;
			project_module.SetEpocRoot( sEpocRoot );
			// project not uninstrumented run it first.
			if ( ! project_module.IsUninstrumented() )
			{
				project_module.SetMode( CATProject::UNINSTRUMENT_CONSOLE );
				project_module.Run();
			}
			// Set mode.
			project_module.SetMode( CATProject::ANALYZE );
			project_module.SetLogLevel( args.ANALYZE.iLoggingLevel );
			project_module.SetDataFile( args.ANALYZE.sDataFile );
			if ( args.ANALYZE.bSymbolFile )
				project_module.SetRomSymbolFiles( args.ANALYZE.vSymbolFiles );
			project_module.SetDataFileOutput( args.ANALYZE.sOutputFile);
			return project_module.Run();
		case SWITCH_HOOK:
			// Parse arguments related to hooking.
			if ( ! parseHookArguments( vArguments, args ) )
				return AT_RETURN_CODE::INVALID_ARGUMENT_ERROR;
			// Set variables for project.
			if ( ! project_module.SetArguments( args ) )
				return AT_RETURN_CODE::INVALID_ARGUMENT_ERROR;
			// Get epocroot
			if( ! CATBase::GetEpocRoot( sEpocRoot ) )
				return AT_RETURN_CODE::CANNOT_FIND_EPOCROOT;
			project_module.SetEpocRoot( sEpocRoot );
			// Check AnalyzeTool libraries
			if ( _stricmp( args.HOOK.sPlatform.c_str(), "winscw") == 0 )
			{
				// Emulator winscw platform
				if ( ! CheckATLibrariesWinscw(sEpocRoot) )
					return AT_RETURN_CODE::AT_LIBS_MISSING;
			}
			else
			{
				// Armv5
				if ( args.HOOK.iBuildSystem == 2 )
				{
					// Abiv2
					if ( ! CheckATLibrariesArmv5Abiv2(sEpocRoot) )
						return AT_RETURN_CODE::AT_LIBS_MISSING;
				}
				else
				{
					// Abiv1
					if( ! CheckATLibrariesArmv5(sEpocRoot) )
						return AT_RETURN_CODE::AT_LIBS_MISSING;
				}
			}
			// Run hooking.
			return project_module.Run();
			//Uninstrument
		case SWITCH_UNHOOK:
			// Set variables for project.
			if ( ! project_module.SetArguments( args ) )
				return AT_RETURN_CODE::INVALID_ARGUMENT_ERROR;
			// Get epocroot
			if( ! CATBase::GetEpocRoot( sEpocRoot ) )
				return AT_RETURN_CODE::CANNOT_FIND_EPOCROOT;
			project_module.SetEpocRoot( sEpocRoot );
			return project_module.Run();
		case SWITCH_VERSION:
			return showVersionInfo();
		case SWITCH_DBGHELP_VERSION:
			return showDbgHelpVersionInfo( true );
		case SWITCH_CLEAN:
			project_module.SetMode( CATProject::CLEAN );
			if ( CheckSBS2Folder() )
				project_module.SetBuildSystem( CATProject::SBS_V2 );
			else
				project_module.SetBuildSystem( CATProject::SBS_V1 );
			return project_module.Run();
		case SWITCH_PARSE_TRACE:
			if ( ! parseParseArguments( vArguments, args ) )
				return AT_RETURN_CODE::INVALID_ARGUMENT_ERROR;
			if (  CATBase::IsDataFile( args.PARSE.sDataFile ) )
			{
				cout << AT_MSG << "Error, " << args.PARSE.sDataFile << " is already parsed." << endl;
				return AT_RETURN_CODE::INVALID_ARGUMENT_ERROR;
			}
			else if( CATBase::IsBinaryLogFile( args.PARSE.sDataFile ) )
			{
				cout << AT_MSG << "Error, " << args.PARSE.sDataFile << " is binary log file." << endl;
				return AT_RETURN_CODE::INVALID_ARGUMENT_ERROR;
			}

			if ( args.PARSE.bOutputFile )
			{
				//Save data with name in arguments[3]
				Parser.StartParse( args.PARSE.sDataFile.c_str(), args.PARSE.sOutputFile.c_str() );
			}
			else
			{
				Parser.StartParse( args.PARSE.sDataFile.c_str(), NULL );
			}
			return AT_RETURN_CODE::OK;
		case SWITCH_HELP:
			print_help();
			print_syntax_examples();
			return AT_RETURN_CODE::OK;
		default:
			cout << AT_MSG << "Invalid parameters." << endl;
			return AT_RETURN_CODE::INVALID_ARGUMENT_ERROR;
		}

	} catch(...)
	{
		cout << AT_MSG << "Error, unhandled exception." << endl;
		return AT_RETURN_CODE::UNHANDLED_EXCEPTION;
	}
}

//EOF
