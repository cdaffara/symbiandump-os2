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
* Description:  Class representing a project.
*
*/


#include "../inc/CATProject.h"
#include "../inc/CATModule2.h"
#include "../inc/CATParseTraceFile.h"
#include "../inc/CATParseBinaryFile.h"
#include "../inc/CATDatParser.h"

//dbghelp.dll version function.
extern int showDbgHelpVersionInfo( bool showVersion );

// ----------------------------------------------------------------------------
// CATProject::CATProject()
// ----------------------------------------------------------------------------
CATProject::CATProject()
{
	LOG_FUNC_ENTRY("CATProject::CATProject");

	m_bUninstrumented = true;
	m_bAbldTest = false;

	// Use windows api to acquire current directory info.
	GetCurrentDirectory( MAX_LINE_LENGTH, m_cCurrentDir );

	m_eBuildSystem = SBS_V1;
	m_eBuildType = UDEB;
	m_eLoggingMode = TRACE;
	m_eMode = NOT_DEFINED;
	
	m_iLoggingLevel = 3;
	
	m_pAnalyzer = 0;

	m_sBinaryTarget = "";
	m_sBuildCommand = "";
	m_sDataFile = "";
	m_sDataFileOutput = "";
	m_sDataFileTemp = "";
	m_sBinaryLogFileTemp = "";
	m_sEpocRoot = "\\";
	m_sMakeFile = "";
	m_sPlatform = "";
	m_sS60FileName = "";
	m_sS60FilePath = "";
	m_sTargetModule = "";
	m_sVariant = "";

	m_vRomSymbolFiles.clear();
	m_vModules.clear();
	m_vStaticLibraries.clear();
	m_vTargetModules.clear();
	m_vUnsupportedModules.clear();
}

// ----------------------------------------------------------------------------
// CATProject::~CATProject()
// ----------------------------------------------------------------------------
CATProject::~CATProject()
{
	LOG_FUNC_ENTRY("CATProject::~CATProject");

	CleanModuleVectors();

	// Delete analyzer
	if ( m_pAnalyzer )
		delete m_pAnalyzer;

	// Temporary datafile
	if ( !m_sDataFileTemp.empty() )
	{
		if ( FileExists( m_sDataFileTemp.c_str() ) )
			FileDelete( m_sDataFileTemp, false );
	}

	// Temporary trace file
	if ( !m_sBinaryLogFileTemp.empty() )
	{
		if ( FileExists( m_sBinaryLogFileTemp.c_str() ) )
			FileDelete( m_sBinaryLogFileTemp, false );
	}
	
}

bool CATProject::SetArguments( ARGUMENTS& arguments )
{
	LOG_FUNC_ENTRY("CATProject::SetArguments");
	bool bRet = true;

	//Project mode.
	if( arguments.eHookSwitch == HOOK_INTERNAL )
	{
		SetMode( CATProject::COMPILE );
		SetLoggingMode( CATProject::FILE );
	}
	else if ( arguments.eHookSwitch == HOOK_EXTERNAL )
	{
		SetMode( CATProject::COMPILE );
		SetLoggingMode( CATProject::TRACE );
	}
	else if ( arguments.eHookSwitch == HOOK_EXTERNAL_FAST )
	{
		SetMode( CATProject::COMPILE );
		SetLoggingMode( CATProject::TRACE_FAST );
	}
	/* Extension*/
	else if ( arguments.eHookSwitch == HOOK_EXTENSION_INTERNAL )
	{
		SetMode( CATProject::INSTRUMENT );
		SetLoggingMode( CATProject::FILE );
	}
	else if ( arguments.eHookSwitch == HOOK_EXTENSION_EXTERNAL )
	{
		SetMode( CATProject::INSTRUMENT );
		SetLoggingMode( CATProject::TRACE );
	}
	else if ( arguments.eHookSwitch == HOOK_EXTENSION_EXTERNAL_FAST )
	{
		SetMode( CATProject::INSTRUMENT );
		SetLoggingMode( CATProject::TRACE_FAST );
	}
	else if ( arguments.eHookSwitch == HOOK_EXTENSION_UNINSTRUMENT )
	{
		SetMode( CATProject::UNINSTRUMENT );
	}
	else if ( arguments.eHookSwitch == HOOK_EXTENSION_FAILED )
	{
		SetMode( CATProject::UNINSTRUMENT_FAILED );
	}
	//Return if uninstrumenting because no other arguments are set.
    if ( GetMode() == CATProject::UNINSTRUMENT
		|| GetMode() == CATProject::UNINSTRUMENT_FAILED )
	{
		return bRet;
	}

	// No build / instrument.
	if ( arguments.HOOK.bNoBuild )
		SetMode( CATProject::INSTRUMENT_CONSOLE );

	// Call stack sizes
	SetAllocCallStackSize( arguments.HOOK.iAllocCallStackSize );
	SetFreeCallStackSize( arguments.HOOK.iFreeCallStackSize );

	//Build system.
	if ( arguments.HOOK.iBuildSystem == 1 )
		SetBuildSystem( CATProject::SBS_V1 );
	else if ( arguments.HOOK.iBuildSystem == 2 )
		SetBuildSystem( CATProject::SBS_V2 );

	//Test module build only
	if ( arguments.HOOK.bAbldTest == true )
		m_bAbldTest = true;

	//Platform.
	if ( !_stricmp( arguments.HOOK.sPlatform.c_str(), "armv5" ) )
		SetPlatform( arguments.HOOK.sPlatform );
	else if ( !_stricmp( arguments.HOOK.sPlatform.c_str(), "gcce" ) )
		SetPlatform( arguments.HOOK.sPlatform );
	else if ( !_stricmp( arguments.HOOK.sPlatform.c_str(), "winscw" ) )
		SetPlatform( arguments.HOOK.sPlatform );
	else
	{
		LOG_STRING( "Error, no supported platform specified (armv5/gcce/winscw).");
		bRet = false;
	}

	//BuildType.
	if ( !_stricmp( arguments.HOOK.sBuildType.c_str(), "urel" ) )
		SetBuildType( CATProject::UREL );
	else if ( !_stricmp( arguments.HOOK.sBuildType.c_str(), "udeb" ) )
		SetBuildType( CATProject::UDEB );
	else
	{
		LOG_STRING( "Error, no build type specified.");
		bRet = false;
	}

	//Internal data file name.
	if ( arguments.HOOK.bDataFileName )
		SetS60FileName( arguments.HOOK.sDataFileName );

	//Internal data file path.
	if ( arguments.HOOK.bDataFilePath )
		SetS60FilePath( arguments.HOOK.sDataFilePath );

	//Build command.
	if ( arguments.HOOK.sBuildCmd.empty() && ( 
		GetMode() == CATProject::COMPILE ||
		GetMode() == CATProject::INSTRUMENT ||
		GetMode() == CATProject::INSTRUMENT_CONSOLE
		))
	{
		cout << AT_MSG << "Error, no build command specified." << endl;
		bRet = false;
	}

	SetBuildCommand( arguments.HOOK.sBuildCmd );

	//Variant.
	SetVariant( arguments.HOOK.sFeatureVariant );

	//Target programs.
	SetTargetModules( arguments.HOOK.vTargetPrograms );

	return bRet;
}

void CATProject::CleanModuleVectors()
{
	LOG_FUNC_ENTRY("CATProject::CleanModuleVectors");
	// delete modules from vector.
	for( size_t i = 0; i < m_vModules.size()  ; i++ )
		delete m_vModules[i];
	m_vModules.clear();
	
	// Delete modules from vector (unsupported).
	for( size_t i = 0; i < m_vUnsupportedModules.size()  ; i++ )
		delete m_vUnsupportedModules[i];
	m_vUnsupportedModules.clear();

	// Delete modules from static library vector.
	for( size_t i = 0; i < m_vStaticLibraries.size() ; i++ )
		delete m_vStaticLibraries[i];
	m_vStaticLibraries.clear();
}
// ----------------------------------------------------------------------------
// CATProject::Run
// ----------------------------------------------------------------------------
int CATProject::Run()
{
	LOG_FUNC_ENTRY("CATProject::Run");
	int iReturnCode = 0;
	switch( m_eMode )
	{
	case COMPILE:
		// Run compile
		iReturnCode = RunCompile();
		if ( iReturnCode == AT_RETURN_CODE::READ_MAKEFILE_ERROR
			|| iReturnCode == AT_RETURN_CODE::KERNEL_SIDE_MODULE_ERROR
			|| iReturnCode == AT_RETURN_CODE::INVALID_MMP_DEFINED )
		{
			DeleteTemporaryDirs();
			DirDelete( AT_TEMP_DIR, true );
		}
		else
		{
			DisplayCompileSummary();
			DisplayBuildSummary();
		}
		break;
	case CLEAN:
		iReturnCode = RunClean();
		break;
	case ANALYZE:
		iReturnCode = RunAnalyze();
		break;
	case INSTRUMENT:
		iReturnCode = RunInstrument();
		break;
	case INSTRUMENT_CONSOLE:
		iReturnCode = RunInstrumentConsole();
		if ( iReturnCode == AT_RETURN_CODE::READ_MAKEFILE_ERROR
			|| iReturnCode == AT_RETURN_CODE::KERNEL_SIDE_MODULE_ERROR )
		{
			DeleteTemporaryDirs();
			DirDelete( AT_TEMP_DIR, true );
		}
		else
		{
			DisplayBuildSummary();
		}
		break;
	case UNINSTRUMENT:
		iReturnCode = RunUninstrument();
		// Show summary
		DisplayCompileSummary();
		DisplayBuildSummary();
		break;
	case UNINSTRUMENT_CONSOLE:
		iReturnCode = RunUninstrumentConsole();
		// Show summary
		DisplayCompileSummary();
		DisplayBuildSummary();
		break;
	case UNINSTRUMENT_FAILED:
		iReturnCode = RunUninstrumentFailed();
		// Display message
		cout << AT_MSG << "Build aborted, because project contains compile error(s)."
			<< endl;
		break;
	default:
		cout << AT_MSG << "Error, mode not supported / implemented." << endl;
		break;
	}
	// Error messages
	switch( iReturnCode )
	{
		case AT_RETURN_CODE::MAKEFILE_ERROR:
			cout << AT_MSG << "Error, creating/reading makefiles." << endl;
			break;
		case AT_RETURN_CODE::COMPILE_ERROR:
			cout << AT_MSG << "Error, compiling project." << endl;
			break;
		case AT_RETURN_CODE::UNKNOWN:
			cout << AT_MSG << "Error, unknown." << endl;
			break;
		case AT_RETURN_CODE::WRONG_DATA_FILE_VERSION:
			cout << AT_MSG << "unable to analyze the data file.\n";
			cout << AT_MSG << "wrong data file version.\n";
			break;
		case AT_RETURN_CODE::INVALID_DATA_FILE:
			cout << AT_MSG << "Error, invalid datafile." << endl;
			break;
		case AT_RETURN_CODE::RELEASABLES_ERROR:
			cout << AT_MSG << "Error, copying releasable(s)." << endl;
			break;
		case AT_RETURN_CODE::RESTORE_MODULES_ERROR:
			cout << AT_MSG << "Error, restoring mmp file(s)." << endl;
			break;
		case AT_RETURN_CODE::CREATING_TEMP_CPP_ERROR:
			cout << AT_MSG << "Error, creating temporary cpp file(s)." << endl;
			break;
		case AT_RETURN_CODE::CLEANING_TEMP_ERROR:
			cout << AT_MSG << "Error, cleaning temporary dir(s)." << endl;
			break;
		case AT_RETURN_CODE::READ_MAKEFILE_ERROR:
			cout << AT_MSG << "Error, reading makefile." << endl;
			break;
		case AT_RETURN_CODE::MODIFY_MODULES_ERROR:
			cout << AT_MSG << "Error, modifying mmp file(s)." << endl;
			break;
		case AT_RETURN_CODE::INVALID_MMP_DEFINED:
			break;
		case AT_RETURN_CODE::WRITE_ATTRIBUTES_ERROR:
			cout << AT_MSG << "Error, writing attributes." << endl;
			break;
		case AT_RETURN_CODE::READ_ATTRIBUTES_ERROR:
			cout << AT_MSG << "Error, reading project configuration. Instrument project again." << endl;
			break;
		case AT_RETURN_CODE::EMPTY_DATA_FILE:
			cout << AT_MSG << "Error, no data to be analyzed." << endl;
			break;
		case AT_RETURN_CODE::NO_SUPPORTED_MODULES_ERROR:
			cout << AT_MSG << "Error, no modules found with supported target type." << endl;
			break;
		case AT_RETURN_CODE::KERNEL_SIDE_MODULE_ERROR:
			cout << AT_MSG << "Error, kernel side component found component. Build/instrument aborted." << endl;
			break;
	}	
	return iReturnCode;
}
// ----------------------------------------------------------------------------
// CATProject::RunRecoveryAndExit()
// Restore modules quick and exit. Used when user wants to kill/end process.
// ----------------------------------------------------------------------------
int CATProject::RunRecoveryAndExit()
{
	LOG_FUNC_ENTRY("CATProject::RunRecoveryAndExit");
	cout << AT_MSG << "Error, user requested exit." << endl;
	VerifyAndRecoverModules();
	DeleteTemporaryDirs();
	DirDelete( AT_TEMP_DIR, true );
	cout << AT_MSG << "Exit." << endl;
	return AT_RETURN_CODE::USER_ISSUED_EXIT;
}

// ----------------------------------------------------------------------------
// CATProject::IsUninstrumented()
// Reads projects configuration file if it exists. 
// Return false in case the data contains information that project is
// uninstrumented. Otherwise returns always true.
// ----------------------------------------------------------------------------
bool CATProject::IsUninstrumented()
{
	LOG_FUNC_ENTRY("CATProject::IsUninstrumented");
	string sCfgFile( AT_TEMP_DIR );
	sCfgFile.append( "\\" );
	sCfgFile.append( AT_PROJECT_ATTRIBUTES_FILE_NAME );
	if ( ! FileExists( sCfgFile.c_str() ) )
		return true;
	if( !ReadAttributes() )
	{
		LOG_STRING( "Error, reading project.cfg file." );
		return false;
	}
	return m_bUninstrumented;
}

// ----------------------------------------------------------------------------
// CATProject::RunCompile()
// Helper functions to run different modes.
// ----------------------------------------------------------------------------
int CATProject::RunCompile()
{
	LOG_FUNC_ENTRY("CATProject::RunCompile");
	// Store attributes
	if( ! MakeTempDirIfNotExist() )
		return AT_RETURN_CODE::WRITE_ATTRIBUTES_ERROR;
	if ( ! WriteAttributes() )
		return AT_RETURN_CODE::WRITE_ATTRIBUTES_ERROR;
	// Create makefile
	if ( ! CreateMakeFile() )
		return AT_RETURN_CODE::MAKEFILE_ERROR;
	// Read makefile to get project attributes
	if ( ! ReadMakeFile() )
		return AT_RETURN_CODE::READ_MAKEFILE_ERROR;
	// Filter unsupported
	FilterModules();
	// Check that we have some "valid" modules to hook
	if ( m_vModules.size() == 0 &&(  m_vUnsupportedModules.size() > 0 || m_vStaticLibraries.size() > 0 ) )
		return AT_RETURN_CODE::NO_SUPPORTED_MODULES_ERROR;
	// Check is possible target module defined in project
	if ( ! IsTargetModuleInProject() )
		return AT_RETURN_CODE::INVALID_MMP_DEFINED;
	// Clean temporary dirs of modules
	if ( ! CleanTemporaryDirs() )
		return AT_RETURN_CODE::CLEANING_TEMP_ERROR;
	// Create temporary cpps for modulse
	if (! CreateTemporaryCpps() )
		return AT_RETURN_CODE::CREATING_TEMP_CPP_ERROR;
	// Hook modules
	if (! ModifyModules() )
		return AT_RETURN_CODE::MODIFY_MODULES_ERROR;
	// Compile all
	// Return code
	int iRetCode = AT_RETURN_CODE::OK;
	// Compile
	if ( ! Compile() )
		iRetCode = AT_RETURN_CODE::COMPILE_ERROR;
	// Listings
	if (! CreateListings() )
		iRetCode = AT_RETURN_CODE::COMPILE_ERROR;
	// Releasables
	if (! CopyReleasables() )
		iRetCode = AT_RETURN_CODE::RELEASABLES_ERROR;
	// Restore "unhook" modules
	if (! RestoreModules() )
		iRetCode = AT_RETURN_CODE::RESTORE_MODULES_ERROR;
	// Return error code OK
	return iRetCode;
}

int CATProject::RunClean()
{
	LOG_FUNC_ENTRY("CATProject::RunClean");
	int iRetCode = AT_RETURN_CODE::OK;
	bool bNothingFound = true;
	// Read attributes.
	if (  ReadAttributes() )
	{
		bNothingFound = false;
		if ( m_eBuildSystem == SBS_V1 )
			InitSbs1MakeFileWithPathToTemp();
		// Read makefile to get project attributes
		if( ReadMakeFile() )
		{
			// Filter unsupported
			FilterModules();
			// Restore modules to make sure no changes left
			if( VerifyAndRecoverModules() )
			{
				// Run reallyclean
				switch ( m_eBuildSystem )
				{
				case SBS_V1:
					RunReallyCleanSbs1();
					break;
				case SBS_V2:
					RunReallyCleanSbs2();
					break;
				default:
					break;
				}
				// Delete temporary dirs of modules
				if(! DeleteTemporaryDirs() )
				{
					
				}
			}
			else
			{
				
			}
		}
	}
	// Projects
	if ( ! DirDelete( AT_TEMP_DIR, true ) )
	{

	}
	else
		bNothingFound = false;

	if ( bNothingFound )
		cout << AT_MSG << "Nothing found to clean." << endl;
	else
		cout << AT_MSG << "Cleaning done." << endl;
	return iRetCode;
}

int CATProject::RunAnalyze()
{
	LOG_FUNC_ENTRY("CATProject::RunAnalyze");

	// Parse data file if it is not data file.
	if ( !IsDataFile( m_sDataFile ) )
	{
		m_sDataFileTemp.clear();
		m_sDataFileTemp.append( m_sDataFile );
		m_sDataFileTemp.append( ".atool" );

		// make filename for file with cleaned timestamps
		// <m_sDataFile>.cleaned in the same place as output file
		string sTraceFileCleaned( GetPathOrFileName( false, m_sDataFileOutput ) );
		sTraceFileCleaned.append( GetPathOrFileName( true, m_sDataFile ) );
		sTraceFileCleaned.append( ".cleaned" );

		//check if it is binary log file
		if( IsBinaryLogFile( m_sDataFile ) )
		{
			// convert binary log file to ASCII trace format
			m_sBinaryLogFileTemp.clear();
		    m_sBinaryLogFileTemp.append( m_sDataFile );
		    m_sBinaryLogFileTemp.append( ".totraces" );

			cout << AT_MSG << "Parsing binary data file..." << endl;

			CATParseBinaryFile Parser;
			stringstream dataToParse;
			if ( ! Parser.StartParse( m_sDataFile.c_str(), m_sBinaryLogFileTemp.c_str() ) )
			{
				return AT_RETURN_CODE::EMPTY_DATA_FILE;
			}
			m_sDataFile = m_sBinaryLogFileTemp;
			
		}
		else
		{
			cout << AT_MSG << "Parsing trace file..." << endl;
		}

		CATParseTraceFile Parser;
		if ( ! Parser.StartParse( m_sDataFile.c_str(), m_sDataFileTemp.c_str(), sTraceFileCleaned.c_str() ) )
		{
			return AT_RETURN_CODE::EMPTY_DATA_FILE;
		}
		m_sDataFile = m_sDataFileTemp;
	}

	// Init makefile member for this run mode.
	if ( m_eBuildSystem == SBS_V1 )
		InitSbs1MakeFileWithPathToTemp();
	// Read makefile to get project attributes
    if( ! ReadMakeFile() )
	{
		cout << AT_MSG << "Error, cannot find project build with AnalyzeTool." << endl;
	}
	else
		FilterModules();

	#ifndef ADDR2LINE
	// Initialize modules locating code lines.
	for( size_t i = 0 ; i < m_vModules.size() ; i++ )
	{
		m_vModules.at(i)->InitializeAddressToLine();
	}
	#endif

	// Create analyzer
	m_pAnalyzer = new CATDatParser( &m_vModules );

	// Pass some info from project if it "exists" to analyzer.
	if ( m_vModules.size() > 0 )
	{
		// Pass platform.
		m_pAnalyzer->SetProjectPlatform( m_sPlatform );
		// Pass build type.
		m_pAnalyzer->SetProjectBuildType( m_eBuildType );
	}
	
	// Set file.
	m_pAnalyzer->SetInputFile( m_sDataFile );

	// Set rom symbol file.
	m_pAnalyzer->SetRomSymbolFiles( m_vRomSymbolFiles );

	// Set output file if specified
	if ( ! m_sDataFileOutput.empty() )
	{
		m_pAnalyzer->SetOutputFile( m_sDataFileOutput );
	}
	// Set log level
	m_pAnalyzer->SetLogLevel( m_iLoggingLevel );

	// Analyze
	return m_pAnalyzer->Analyze();
}

int CATProject::RunInstrument()
{
	LOG_FUNC_ENTRY("CATProject::RunInstrument");
	// Store attributes
	if( ! MakeTempDirIfNotExist() )
		return AT_RETURN_CODE::WRITE_ATTRIBUTES_ERROR;
	if ( ! WriteAttributes() )
		return AT_RETURN_CODE::WRITE_ATTRIBUTES_ERROR;
	if ( m_eBuildSystem == SBS_V1 )
	{
		// Initialize level 1 make file member.
		if ( ! InitSbs1MakeFileWithPath() )
			return AT_RETURN_CODE::MAKEFILE_ERROR;
		// Copy it to temporary folder.
		CopyMakeFileSbs1ToTemporaryFolder();
		// Run export.
		if( ! RunExportSbs1() )
			return AT_RETURN_CODE::MAKEFILE_ERROR;
		// Create level 2 makefiles.
		if ( ! CreateMakeFileSbs1Level2() )
			return AT_RETURN_CODE::MAKEFILE_ERROR;
	}
	else if ( m_eBuildSystem == SBS_V2 )
	{
		// Create makefile only when using SBS v.2
		if ( ! CreateMakeFile() )
			return AT_RETURN_CODE::MAKEFILE_ERROR;
	}
	else
	{
		return AT_RETURN_CODE::UNKNOWN;
	}
	// Read makefile to get project attributes
	if ( ! ReadMakeFile() )
		return AT_RETURN_CODE::READ_MAKEFILE_ERROR;
	// Filter unsupported
	FilterModules();
	// Check that we have some "valid" modules to hook
	if ( m_vModules.size() == 0 &&(  m_vUnsupportedModules.size() > 0 || m_vStaticLibraries.size() > 0 ) )
		return AT_RETURN_CODE::NO_SUPPORTED_MODULES_ERROR;

	// Clean temporary dirs of modules
	if ( ! CleanTemporaryDirs() )
		return AT_RETURN_CODE::CLEANING_TEMP_ERROR;
	// Create temporary cpps for modulse
	if (! CreateTemporaryCpps() )
		return AT_RETURN_CODE::CREATING_TEMP_CPP_ERROR;
	// Hook modules
	if (! ModifyModules() )
		return AT_RETURN_CODE::MODIFY_MODULES_ERROR;
	return AT_RETURN_CODE::OK;
}

int CATProject::RunInstrumentConsole()
{
	LOG_FUNC_ENTRY("CATProject::RunInstrumentConsole");
	if( ! MakeTempDirIfNotExist() )
		return AT_RETURN_CODE::UNKNOWN;
	// Store attributes
	m_bUninstrumented = false;
	if ( ! WriteAttributes() )
		return AT_RETURN_CODE::WRITE_ATTRIBUTES_ERROR;
	// Create makefile
	if ( ! CreateMakeFile() )
		return AT_RETURN_CODE::MAKEFILE_ERROR;
	// Read makefile to get project attributes
	if ( ! ReadMakeFile() )
		return AT_RETURN_CODE::READ_MAKEFILE_ERROR;
	// Filter unsupported
	FilterModules();
	// Check that we have some "valid" modules to hook
	if ( m_vModules.size() == 0 &&(  m_vUnsupportedModules.size() > 0 || m_vStaticLibraries.size() > 0 ) )
		return AT_RETURN_CODE::NO_SUPPORTED_MODULES_ERROR;

	// Clean temporary dirs of modules
	if ( ! CleanTemporaryDirs() )
		return AT_RETURN_CODE::CLEANING_TEMP_ERROR;
	// Create temporary cpps for modulse
	if (! CreateTemporaryCpps() )
		return AT_RETURN_CODE::CREATING_TEMP_CPP_ERROR;
	// Hook modules
	if (! ModifyModules() )
		return AT_RETURN_CODE::MODIFY_MODULES_ERROR;
	// Run Reallyclean when using abld.
	if ( m_eBuildSystem == SBS_V1 )
		RunReallyCleanSbs1();
	return AT_RETURN_CODE::OK;
}

int CATProject::RunUninstrument()
{
	LOG_FUNC_ENTRY("CATProject::RunUninstrument");
	// Read attributes.
	if ( ! ReadAttributes() )
		return AT_RETURN_CODE::READ_ATTRIBUTES_ERROR;
	// Init makefile member for this run mode.
	if ( m_eBuildSystem == SBS_V1 )
		InitSbs1MakeFileWithPathToTemp();
	// Read makefile to get project attributes
	if ( ! ReadMakeFile() )
		return AT_RETURN_CODE::READ_MAKEFILE_ERROR;
	// Filter unsupported
	FilterModules();
	// Check that we have some "valid" modules to hook
	if ( m_vModules.size() == 0 &&(  m_vUnsupportedModules.size() > 0 || m_vStaticLibraries.size() > 0 ) )
		return AT_RETURN_CODE::NO_SUPPORTED_MODULES_ERROR;
	// Create lst files
    if (! CreateListings() )
		return AT_RETURN_CODE::COMPILE_ERROR;
	// Copy releasables of modules
	if (! CopyReleasables() )
		return AT_RETURN_CODE::RELEASABLES_ERROR;
	// Restore "unhook" modules
	if (! RestoreModules() )
		return AT_RETURN_CODE::RESTORE_MODULES_ERROR;
	// Return error code OK
	return AT_RETURN_CODE::OK;
}

int CATProject::RunUninstrumentConsole()
{
	LOG_FUNC_ENTRY("CATProject::RunUninstrumentConsole");
	int iErrorCode = AT_RETURN_CODE::OK;
	// Read attributes
	if ( ReadAttributes() )
	{
		// Init makefile member for this run mode.
		if ( m_eBuildSystem == SBS_V1 )
			InitSbs1MakeFileWithPathToTemp();
		// Read makefile to get project attributes
		if( ReadMakeFile() )
		{
			// Filter unsupported
			FilterModules();
			// Create lst files
			CreateListings();
			if (! CopyReleasables() )
				iErrorCode = AT_RETURN_CODE::RELEASABLES_ERROR;
		}
		else
			iErrorCode = AT_RETURN_CODE::READ_MAKEFILE_ERROR;
		// Change state to uninstrumented and write status
		m_bUninstrumented = true;
		if ( ! WriteAttributes() )
			iErrorCode = AT_RETURN_CODE::WRITE_ATTRIBUTES_ERROR;
	}
	else
	{
		iErrorCode = AT_RETURN_CODE::READ_ATTRIBUTES_ERROR;
	}
	return iErrorCode;
}

int CATProject::RunUninstrumentFailed()
{
	LOG_FUNC_ENTRY("CATProject::RunUninstrumentFailed");
	// Read attributes.
	if ( ! ReadAttributes() )
		return AT_RETURN_CODE::READ_ATTRIBUTES_ERROR;
	// Init makefile member for this run mode.
	if ( m_eBuildSystem == SBS_V1 )
		InitSbs1MakeFileWithPathToTemp();
	// Read makefile to get project attributes
	if( ReadMakeFile() )
	{
		// Filter modules
		FilterModules();
		// Restore modules to make sure no changes left
		if( RestoreModules() )
		{
			// Delete temporary dirs of modules
			if(! DeleteTemporaryDirs() )
			{

			}

		}
		else
		{

		}
	}
	else
	{

	}
	// Projects
	if ( ! DirDelete( AT_TEMP_DIR, true ) )
	{

	}
	return AT_RETURN_CODE::OK;
}

// ----------------------------------------------------------------------------
// Main Functions
// ----------------------------------------------------------------------------
void CATProject::DisplayCompileSummary()
{
	LOG_FUNC_ENTRY("CATProject::DisplayCompileSummary");
	cout << AT_BUILD_SUMMARY_HEADER;
	// Supported modules
	for( size_t i = 0; i < m_vModules.size(); i++ )
	{
		// Successful
		if ( m_vModules.at(i)->GetErrors().empty() )
		{
			// Create build complete file for Carbide xtension
			m_vModules.at(i)->CreateBuildCompleteFile();
			cout << AT_BUILD_SUMMARY_INSTRUMENTED_BUILD_COMPLETE
				<< GetPathOrFileName( true, m_vModules.at(i)->GetMmpFile() )
				<< endl
				<< AT_BUILD_SUMMARY_TARGET
				<< m_vModules.at(i)->GetBinaryName()
				<< endl;
			// Datafiles
			// Use module data file name if project's data file not defined.
			if ( m_eLoggingMode == CATProject::FILE 
				&& m_vModules.at(i)->GetTargetType().compare("exe") == 0 )
			{
				cout << AT_BUILD_SUMMARY_DATA_FILE_NAME
						<< m_vModules.at(i)->GetS60FileName()
						<< endl;

				// if filePath not empty, use it for log file
				if ( !m_sS60FilePath.empty() )
				{
					cout << AT_BUILD_SUMMARY_DATA_FILE_PATH
					<< m_sS60FilePath
					<< endl;
				}
			}
		}
		else
		{
			// Failed
			cout << AT_BUILD_SUMMARY_FAILED
				<< GetPathOrFileName( true, m_vModules.at(i)->GetMmpFile() )
				<< endl
				<< AT_BUILD_SUMMARY_TARGET
				<< m_vModules.at(i)->GetBinaryName()
				<< endl;
			// Print errors.
			cout << AT_BUILD_SUMMARY_ERRORS
				<< m_vModules.at(i)->GetErrors()
				<< endl;
		}
	}

	// Static libraries
	for( size_t i = 0; i < m_vStaticLibraries.size(); i++ )
	{
		if ( m_vStaticLibraries.at(i)->GetErrors().empty() )
		{
			cout << AT_BUILD_SUMMARY_NORMAL_BUILD_COMPLETE
				<< GetPathOrFileName( true, m_vStaticLibraries.at(i)->GetMmpFile())
				<< endl
				<< AT_BUILD_SUMMARY_TARGET
				<< m_vStaticLibraries.at(i)->GetBinaryName()
				<< endl
				<< AT_BUILD_SUMMARY_STATIC_LIBRARY
				<< endl;
				
		}
		else
		{
			// Failed
			cout << AT_BUILD_SUMMARY_FAILED
				<< GetPathOrFileName( true, m_vStaticLibraries.at(i)->GetMmpFile() )
				<< endl
				<< AT_BUILD_SUMMARY_TARGET
				<< m_vStaticLibraries.at(i)->GetBinaryName()
				<< endl;
			// Print errors.
			cout << AT_BUILD_SUMMARY_ERRORS
				<< m_vStaticLibraries.at(i)->GetErrors()
				<< endl;
		}
	}

	// Unsupported modules
	for( size_t i = 0; i < m_vUnsupportedModules.size(); i++ )
	{
		cout << AT_BUILD_SUMMARY_NORMAL_BUILD_COMPLETE
			<< GetPathOrFileName( true, m_vUnsupportedModules.at(i)->GetMmpFile() )
			<< endl
			<< AT_BUILD_SUMMARY_TARGET
			<< m_vUnsupportedModules.at(i)->GetBinaryName()
			<< endl;
		cout << m_vUnsupportedModules.at(i)->GetCompileInfoText() << endl;
	}
}

void CATProject::DisplayBuildSummary( void )
{
	LOG_FUNC_ENTRY("CATProject::DisplayBuildSummary");
	cout << endl;
	// Build information
	cout << AT_BUILD_SUMMARY_BUILD_TYPE << GetBuildTypeString() << endl;
	// Platform
	cout << AT_BUILD_SUMMARY_BUILD_PLATFORM << m_sPlatform << endl;
	// Possible variant
	if ( ! m_sVariant.empty() )
		cout << AT_BUILD_SUMMARY_BUILD_VARIANT << m_sVariant << endl;
	// Logging mode
	cout << AT_BUILD_SUMMARY_LOGGING_MODE;
	if ( m_eLoggingMode == FILE )
		cout  << AT_BUILD_SUMMARY_FILE;
	else if ( m_eLoggingMode == TRACE )
		cout << AT_BUILD_SUMMARY_TRACE;
	else if ( m_eLoggingMode == TRACE_FAST )
		cout << AT_BUILD_SUMMARY_TRACE_FAST;
	cout << endl;
	// Call stack sizes
	cout << AT_BUILD_SUMMARY_ALLOC_CALL_STACK_SIZE
		<< m_iAllocCallStackSize
		<< endl;
	cout << AT_BUILD_SUMMARY_FREE_CALL_STACK_SIZE
		<< m_iFreeCallStackSize
		<< endl;

	if(!_stricmp(m_sPlatform.c_str(), ("winscw")))
	{
		//print version info only when version is not up-to-date
		cout << endl;
		showDbgHelpVersionInfo( false );
	}
}

bool CATProject::CreateMakeFile()
{
	switch ( m_eBuildSystem )
	{
	case SBS_V1:
		if( ! CreateMakeFileSbs1() )
			return false;
		// Copy main make file.
		if( ! CopyMakeFileSbs1ToTemporaryFolder() )
			return false;
		// Run export.
		if( ! RunExportSbs1() )
			return false;
		// Create level 2 makefiles.
		if( ! CreateMakeFileSbs1Level2() )
			return false;
		return true;
	case SBS_V2:
		return CreateMakeFileSbs2();
	default:
		return false;
	}
}

bool CATProject::CreateMakeFileSbs1()
{
	LOG_FUNC_ENTRY("CATProject::CreateMakeFileSbs1");

	// If variant defined check does it exist.
	if( ! m_sVariant.empty() )
	{
		if ( ! CheckVariant( m_sEpocRoot, m_sVariant ) )
		{
			cout << INVALID_VARIANT_ERROR;
			if ( IsDefaultVariant( m_sEpocRoot ) )
			{
				m_sVariant = "default";
				cout << USING_DEFAULT_VARIANT_MESSAGE;
				if ( ! WriteAttributes() )
					return false;
			}
			else
			{
				cout << NO_DEFAULT_VARIANT_ERROR;
				return false;
			}
		}
	}

	// Create level 1 make file.
	string sCmd( "bldmake bldfiles " );
	sCmd.append( m_sPlatform );
	cout << AT_MSG_SYSTEM_CALL << sCmd << endl;
	(void)system( sCmd.c_str() );
	return InitSbs1MakeFileWithPath();
}

bool CATProject::CopyMakeFileSbs1ToTemporaryFolder()
{
	LOG_FUNC_ENTRY("CATProject::CopyMakeFileSbs1ToTemporaryFolder");
	// Check that temporary dir exists if not create it.
	if ( ! MakeTempDirIfNotExist() )
		return false;
	// Copy makefile to temporary directory
	string sMakeFileInTemp( AT_TEMP_DIR );
	sMakeFileInTemp.append( "\\" );
	sMakeFileInTemp.append( AT_LEVEL_1_MAKEFILE_NAME );
	if ( ! FileCopyToPath( m_sMakeFile, sMakeFileInTemp ) )
		return false;
	return true;

}

bool CATProject::RunReallyCleanSbs1()
{
	LOG_FUNC_ENTRY("CATProject::RunReallyCleanSbs1");
	// Check that abld.bat has been made.
	if ( ! FileExists( "abld.bat" ) )
		return false;
	// Run reallyclean.
	string sCmd;
	if ( m_bAbldTest )
		sCmd = "abld test reallyclean ";
	else
		sCmd = "abld reallyclean ";
	sCmd.append( m_sPlatform );
	if ( ! m_sVariant.empty() )
	{
		sCmd.append( "." );
		sCmd.append( m_sVariant );
	}
	sCmd.append( " " );
	sCmd.append( GetBuildTypeString() );
	if ( m_vTargetModules.size() > 1 )
	{
		RunAbldCommandToAllTargets( sCmd );
	}
	else
	{
		AddTargetModuleIfDefined( sCmd );
		cout << AT_MSG_SYSTEM_CALL << sCmd << endl;
		(void) system( sCmd.c_str() );
	}
	return true;
}

bool CATProject::RunReallyCleanSbs2()
{
	LOG_FUNC_ENTRY("CATProject::RunReallyCleanSbs2");
	string sCmd("");
	if ( m_sBuildCommand.empty() )
	{
		// If no build command defined (not found in project.cfg).
		sCmd.append( RAPTOR_CMD_BASE );;
		sCmd.append( m_sPlatform );
		sCmd.append( "_" );
		sCmd.append( GetBuildTypeString() );
		if ( ! m_sVariant.empty() )
		{
			sCmd.append( "." );
			sCmd.append( m_sVariant );
		}
		sCmd.append( RAPTOR_REALLYCLEAN_LOG );
		AddTargetModuleIfDefined( sCmd );
		sCmd.append( " REALLYCLEAN" );
	}
	else
	{
		// When build command set use it.
		sCmd.append( m_sBuildCommand );
		sCmd.append( RAPTOR_REALLYCLEAN_LOG );
		sCmd.append( " REALLYCLEAN" );
	}
	cout << AT_MSG_SYSTEM_CALL << sCmd << endl;
	int iRet = (int)system( sCmd.c_str() );
	if ( iRet == 0 )
		return true;
	return false;
}

bool CATProject::RunExportSbs1()
{
	LOG_FUNC_ENTRY("CATProject::RunExportSbs1");
	// Run export.
	string sCmd;
	if ( m_bAbldTest )
		sCmd = "abld test export";
	else
		sCmd = "abld export";
	cout << AT_MSG_SYSTEM_CALL << sCmd << endl;
	(void) system( sCmd.c_str() );
	return true;
}

bool CATProject::CreateMakeFileSbs1Level2()
{
	LOG_FUNC_ENTRY("CATProject::CreateMakeFileSbs1Level2");
	// Create level 2 makefiles.
	
	string sCmd;
	
	if ( m_bAbldTest )
		sCmd ="abld test makefile ";
	else
		sCmd ="abld makefile ";

	sCmd.append( m_sPlatform );
	if ( ! m_sVariant.empty() )
	{
		sCmd.append( "." );
		sCmd.append( m_sVariant );
	}
	
	// Check if multiple targets defined and sbs 1.
	if ( m_vTargetModules.size() > 1 )
	{
		RunAbldCommandToAllTargets( sCmd );
	}
	else
	{
		AddTargetModuleIfDefined( sCmd );
		cout << AT_MSG_SYSTEM_CALL << sCmd << endl;
		(void) system( sCmd.c_str() );
	}
	return true;
}

bool CATProject::CreateMakeFileSbs2()
{
	LOG_FUNC_ENTRY("CATProject::CreateMakeFileSbs2");
	// Delete build directory if it exists before creating new makefiles.
	if ( DirectoryExists( "atool_temp\\build" ) )
		DirDelete( "atool_temp\\build", true );
	// Create command to create makefiles.
	string sCmd( m_sBuildCommand );
	sCmd.append( " " );
	sCmd.append( RAPTOR_MAKEFILE_SWITCH );
	sCmd.append( " " );
	sCmd.append( RAPTOR_NOBUILD_SWITCH );
	cout << AT_MSG_SYSTEM_CALL << sCmd << endl;
	int iRet = (int)system( sCmd.c_str() );
	if ( iRet == 0 )
		return true;
	return false;
}

bool CATProject::ReadMakeFile()
{
	// Clean modules before reading.
	CleanModuleVectors();
	if ( m_eBuildSystem == SBS_V1 )
	{
		// Read level 1 makefile which contains module name and makefiles.
		if( ! ReadMakeFileSbs1Level1() )
			return false;
		// Read level 2 makefiles.
		vector<CATModule2*>::iterator it;
		// If we are compiling or etc... we need to create temporary directories.
		if ( m_eMode == COMPILE || m_eMode == INSTRUMENT || m_eMode == INSTRUMENT_CONSOLE )
		{
			// Read make makefiles from /epoc32/build... and create temporary directory.
			bool bLevel2 = true;
			for( it = m_vModules.begin(); it != m_vModules.end() ; it ++ )
			{
				if( ! (*it)->CreateTemporaryDirectory() )
					return false;
				if( ! (*it)->ReadMakeFile() )
				{
					bLevel2 = false;
					break;
				}
			}
			// If failed reading modules from level 2 makefiles.
			if ( ! bLevel2 )
			{
				// Clean modules.
				CleanModuleVectors();
				// Try use default variant if it exists.
				if ( CheckVariant( m_sEpocRoot, "default" ) )
				{
					m_sVariant = "default";
					cout << USING_DEFAULT_VARIANT_MESSAGE;
					if ( ! WriteAttributes() )
						return false;
					if ( ! InitSbs1MakeFileWithPath() )
						return false;
					if ( ! ReadMakeFileSbs1Level1() )
						return false;
					for( it = m_vModules.begin(); it != m_vModules.end() ; it ++ )
					{
						if( ! (*it)->CreateTemporaryDirectory() )
							return false;
						if( ! (*it)->ReadMakeFile() )
							return false;
					}

				}
			}
		}
		else
		{
			// Read make files from temporary directories.
			for( it = m_vModules.begin(); it != m_vModules.end() ; it ++ )
			{
				if ( ! (*it)->ReadMakeFileFromTemp() )
					return false;
			}
		}
		return true;
	}
	else if ( m_eBuildSystem == SBS_V2 )
	{
		// Read make file.
		if( ! ReadMakeFileSbs2() )
			return false;
		// Create module temporary directories if we are compiling or etc...
		if ( m_eMode == COMPILE || m_eMode == INSTRUMENT || m_eMode == INSTRUMENT_CONSOLE )
			{
			for( vector<CATModule2*>::iterator it = m_vModules.begin(); it < m_vModules.end(); it++ )
				(*it)->CreateTemporaryDirectory();
			for( vector<CATModule2*>::iterator it = m_vStaticLibraries.begin(); it < m_vStaticLibraries.end(); it++ )
				(*it)->CreateTemporaryDirectory();
			}
	}
	return true;
}

bool CATProject::ReadMakeFileSbs1Level1()
{
	LOG_FUNC_ENTRY("CATProject::ReadMakeFileSbs1Level1");

	bool bRet = false;

	//Try to open makefile
	ifstream in;
	in.open( m_sMakeFile.c_str() );

	//File open ok?
	if( !in.good() )
	{
		printf( "Can not open file: %s\n", m_sMakeFile.c_str() );
		in.close();
		return bRet;
	}

	// Add also these so "compatible with sbs2".
	// Releasables path (binaries).
	string sReleasePath( m_sEpocRoot );
	// add trailing '\' if root path is missing it
	if ( sReleasePath.size() < 1 )
		sReleasePath.append( "\\" );
	else if ( sReleasePath.at( sReleasePath.length() -1 ) != '\\' )
		sReleasePath.append( "\\" );
	sReleasePath.append( "epoc32\\release" );
	string sFullVariantPath( m_sPlatform );
	sFullVariantPath.append( "\\" );
	sFullVariantPath.append( GetBuildTypeString() );

	char cTemp[MAX_LINE_LENGTH];
	bool bContinueSearch = true;
	bool bMmpInfoFound = false;
	CATModule2* pModule = 0;
	string sTempLineFromFile;
	string sMmpFileSearchString;
	if ( m_bAbldTest )
		sMmpFileSearchString = MMPTESTFILE_SEARCH_STRING;
	else
		sMmpFileSearchString = MMPFILE_SEARCH_STRING;
	do
	{
		// get line from file
		in.getline( cTemp, MAX_LINE_LENGTH );
		sTempLineFromFile.clear();
		sTempLineFromFile.append( cTemp );

		//Search makefile string
		if( sTempLineFromFile.find( MAKEFILE_SEARCH_STRING ) != string::npos )
		{
			bMmpInfoFound = true;
			if( sTempLineFromFile.find( sMmpFileSearchString ) != string::npos )
			{
				bRet = true;
				//Parse mmp path + mmp filename
				sTempLineFromFile.erase( 0, sTempLineFromFile.find_first_of("\"") );
				sTempLineFromFile.erase( 0, 1 );

				string sPath = sTempLineFromFile.substr(0, sTempLineFromFile.find_first_of("\"") );

				sPath = ChangeSlashToBackSlash( sPath );
				//Remove text "bld.inf"
				sPath.erase( (sPath.find_last_of( "\\" ) + 1) , string::npos );

				string sFileName = sTempLineFromFile.substr( (sTempLineFromFile.find( sMmpFileSearchString ) + sMmpFileSearchString.length() + 3), string::npos );
				sFileName = ChangeSlashToBackSlash( sFileName );
				sFileName = sFileName.substr( 0, sFileName.find_first_of("\"") );

				// Append .mmp to filename if it does not exist
				if ( sFileName.find(".mmp") == string::npos )
					sFileName.append(".mmp");

				//Insert drive letter
				sPath.insert(0, string( m_cCurrentDir).substr(0,2) );
		
				//Insert mmp file to the end
				sPath.append( sFileName );

				ChangeToLower( sPath );
				
				// If target programs defined find from those or do not
				// add module to vector.
				bool bAddToVector = true;
				if ( m_vTargetModules.size() > 0 )
				{
					bAddToVector = false;
					vector<string>::iterator it;
					for( it = m_vTargetModules.begin() ; it != m_vTargetModules.end() ; it++ )
					{
						string sFind( *it );
						sFind.insert( 0, "\\" );
						if ( sPath.find( sFind ) != string::npos )
						{
							bAddToVector = true;
							break;
						}
					}	
				}

				if ( bAddToVector )
				{
					pModule = new CATModule2();
					pModule->SetMmpFile( sPath );
					pModule->SetVariantType( GetBuildTypeString() );
					pModule->SetVariantPlatform( m_sPlatform );
					pModule->SetReleasePath( sReleasePath );
					pModule->SetFullVariantPath( sFullVariantPath );
					pModule->SetBuildSystem( SBS_V1 );
					m_vModules.push_back( pModule );
				}
			}
		}
		else if( bMmpInfoFound )
			//Do not continue search if mmp info lines are all handled
			bContinueSearch = false;
		if( !in.good() )
			bContinueSearch = false;
	}
	while( bContinueSearch );

	bContinueSearch = true;

	//Search MAKEFILES for invidual modules
	do
	{
		in.getline( cTemp, MAX_LINE_LENGTH );
		sTempLineFromFile.clear();
		sTempLineFromFile.append( cTemp );
		// find the lines 'MAKEFILE[modulename]_FILES'
		if( (sTempLineFromFile.find( "MAKEFILE" ) == 0) && (sTempLineFromFile.find( "_FILES" ) != string::npos) )
		{
			//Math the makefile line with one of our modules
			for( size_t i = 0 ; i < m_vModules.size() ; i++ )
			{
				//Create name
				string sMakeFile( "MAKEFILE" );
				string sTempMmpFile( RemovePathAndExt( m_vModules.at(i)->GetMmpFile(), true ) );
				ChangeToUpper( sTempMmpFile );
				sMakeFile.append( sTempMmpFile );
				sMakeFile.append( "_FILES" );
				// matched
				if( sTempLineFromFile.find( sMakeFile ) != string::npos )
				{
					//parse the makefile name from line
					in.getline( cTemp, MAX_LINE_LENGTH );
					sTempLineFromFile.clear();
					sTempLineFromFile.append( cTemp );
					//Remove character "\""
					sTempLineFromFile.erase( 0, ( sTempLineFromFile.find_first_of("\"") + 1 ) );
					// in winscw last part is '" \' and on armd '"' so remove all after last '"'
					sTempLineFromFile.erase( sTempLineFromFile.find_last_of("\""), sTempLineFromFile.size() );
					// Set correct makefile for module
					m_vModules.at( i )->SetMakeFile( sTempLineFromFile );
					// break 
					break;
				} // If mathed to mmp
			} // End of mmp file loop
		} // found lines 'MAKEFILE[modulename]_FILES'
		if( !in.good() )
			bContinueSearch = false;
	}
	while( bContinueSearch );
	in.close();
	return bRet;
}

bool CATProject::ReadMakeFileSbs2( void )
{
	LOG_FUNC_ENTRY("CATProject::ReadMakeFileSbs2(void)");
	// File is by default named make_build.default but when building specific layer
	// make_build_LAYERNAME.default is produced by Raptor.
	// So find makefile(s) and read them.
	vector<string> vMakeFiles = DirList( "atool_temp\\build\\", false, true );
	bool bRet = true;
	for( vector<string>::iterator it = vMakeFiles.begin(); it != vMakeFiles.end() ; it++ )
	{
		// Recognize multiple makefiles.
		if ( it->find("make_build_") != string::npos && it->find(".default") != string::npos )
		{
			if ( ! ReadMakeFileSbs2( *it ) )
				bRet = false;
		}
		// Single makefile.
		else if ( it->find( "make_build.default" ) != string::npos )
		{
			if ( ! ReadMakeFileSbs2( *it ) )
				bRet = false;
		}
	}
	// We got some modules?
	if ( m_vModules.size() == 0 )
		bRet = false;
	return bRet;
}

bool CATProject::ReadMakeFileSbs2( string& sMakeFile )
{
	LOG_FUNC_ENTRY("CATProject::ReadMakeFileSbs2(string)");
	try {
		// Open file
		ifstream in;
		in.open( sMakeFile.c_str() , ios_base::in );
		// Check that open ok
		if ( ! in.good() )
		{
			cout << AT_MSG << "Error, opening file "
				<< RAPTOR_MAKEFILE << endl;
			in.close();
			return false;
		}
		// Source line from file
		string sSourceLine;
		// Module pointer
		CATModule2* pModule = 0;
		// Are we looking for module attributes
		bool bFindAttributes = false;
		// Until end of file
		while( in.good() )
		{
			// Get new line from file
			string sLine;
			getline(in, sLine);
			// New module
			if ( sLine.find( RAPTOR_PROJECT_META ) == 0 )
			{
				// Remove project_meta from line
				sLine.erase(0, strlen( RAPTOR_PROJECT_META ) );
				LOG_STRING("Found module: " << sLine );
				// Check is name empty
				// This seems to happen when sbs2 "wraps" i.e. mifconv to a module type item
				if ( sLine.empty() )
				{
					LOG_STRING("skipping empty module");
					// Skip it
					continue;
				}
				// If module add it to vector
				if ( pModule )
				{
					// Add sources,
					pModule->AddSources( sSourceLine );
					// Build system.
					pModule->SetBuildSystem( SBS_V1 );
					// Push to vector.
					m_vModules.push_back( pModule );
				}
				// New module
				pModule = new CATModule2();
				// Clear sourceline
				sSourceLine.clear();
				// Set modules mmp with path

				if ( ! pModule->SetMmpFile( sLine ) )
				{
					// Fatal error setting mmp file
					in.close();
					return false;
				}
				// Find attributes on
				bFindAttributes = true;
				// Get new line from file
				getline(in, sLine);
			}
			// If attribute finding on
			if ( bFindAttributes )
			{
				// Pickup modules attributes
				if ( sLine.find ( RAPTOR_SOURCE ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_SOURCE ) );
					sSourceLine = sLine;
				}
				else if ( sLine.find ( RAPTOR_TARGET ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_TARGET ) );
					pModule->SetTarget( sLine );
				}
				else if ( sLine.find ( RAPTOR_TARGETYPE ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_TARGETYPE ));
					pModule->SetTargetType( sLine );
				}
				else if ( sLine.find( RAPTOR_REQUESTEDTARGETEXT ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_REQUESTEDTARGETEXT ) );
					pModule->SetRequestedTargetExt( sLine );
				}
				else if ( sLine.find ( RAPTOR_VARIANTPLATFORM ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_VARIANTPLATFORM ));
					pModule->SetVariantPlatform( sLine );
				}
				else if ( sLine.find ( RAPTOR_VARIANTTYPE ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_VARIANTTYPE ));
					pModule->SetVariantType( sLine );
				}
				else if ( sLine.find ( RAPTOR_FEATUREVARIANT ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_FEATUREVARIANT ));
					pModule->SetFeatureVariant( sLine );
				}
				else if ( sLine.find ( RAPTOR_FEATUREVARIANTNAME ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_FEATUREVARIANTNAME ));
					pModule->SetFeatureVariantName( sLine );
				}
				else if ( sLine.find ( RAPTOR_RELEASEPATH ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_RELEASEPATH ));
					pModule->SetReleasePath( sLine );
				}
				else if ( sLine.find ( RAPTOR_FULLVARIANTPATH ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_FULLVARIANTPATH ));
					pModule->SetFullVariantPath( sLine );
				}
				else if ( sLine.find ( RAPTOR_COMPILE_DEFINITIONS ) == 0 )
				{
					sLine.erase(0, strlen( RAPTOR_COMPILE_DEFINITIONS ) );
					pModule->SetCompileDefinitions( sLine );
				}
			}
		} // while in.good()
		// Add last module if n number of modules found
		if ( pModule )
		{
			if ( ! pModule->GetTarget().empty() )
			{
				// Add sources
				pModule->AddSources( sSourceLine );
				// Build system.
				pModule->SetBuildSystem( SBS_V1 );
				// Push back to vector
				m_vModules.push_back( pModule );
			}
		}
		// Close file
		in.close();
		return true;
	} // try.
	catch(...)
	{
		LOG_STRING("Unexpected exception reading sbs 2 makefile");
		return false;
	}
}
bool CATProject::CreateTemporaryDirectories()
{
	LOG_FUNC_ENTRY("CATProject::CreateTemporaryDirectories");
	bool bRet = true;
	for( size_t i = 0 ; i < m_vModules.size(); i++)
	{
		if( ! m_vModules.at(i)->CreateTemporaryDirectory() )
			bRet = false;
	}
	for( size_t i = 0 ; i < m_vStaticLibraries.size(); i++)
	{
		if( ! m_vStaticLibraries.at(i)->CreateTemporaryDirectory() )
			bRet = false;
	}
	return bRet;

}

bool CATProject::CreateTemporaryCpps()
{
	LOG_FUNC_ENTRY("CATProject::CreateTemporaryCpps");
	bool bRet = true;
	for( size_t i = 0 ; i < m_vModules.size(); i++)
	{
		if( ! m_vModules.at(i)->CreateTempCpp(
			m_sS60FileName, m_sS60FilePath, m_eLoggingMode, m_eBuildType, m_iAllocCallStackSize, m_iFreeCallStackSize ) )
			bRet = false;
	}
	return bRet;
}

bool CATProject::FilterModules()
{
	LOG_FUNC_ENTRY("CATProject::FilterModules");
	vector<CATModule2*>::iterator it;
	// Loop thru modules.
	it = m_vModules.begin();
	while( it != m_vModules.end() )
	{
		// Get target type of module to separate string (will be modified).
		string sTargetType = (*it)->GetTargetType();
		// Modules compile definitions.
		string sCompileDefinition = (*it)->GetCompileDefinitions();
		// Check is it supported.
		if ( !IsTargetTypeSupported( sTargetType) )
		{
			(*it)->SetCompileInfoText( AT_UNSUPPORTED_TARGET_TYPE );
			// Not supported add to not supported vector.
			m_vUnsupportedModules.push_back( *it );
			// Erase cell.
			it = m_vModules.erase( it );
		}
		// Check if its static library
		else if ( _stricmp( sTargetType.c_str(), "lib" ) == 0 )
		{
			// Static librarie move to their vector.
			m_vStaticLibraries.push_back( *it );
			// Erase cell.
			it = m_vModules.erase( it );
		
		}
		else if ( sCompileDefinition.find( KERNEL_MODE_COMPILE_DEFINITION ) != string::npos )
		{
			(*it)->SetCompileInfoText( AT_UNSUPPORTED_COMPILE_DEFINITION );
			// Not supported add to not supported vector.
			m_vUnsupportedModules.push_back( *it );
			// Erase cell.
			it = m_vModules.erase( it );
		}
		else
			it++;
	}
	return true;
}

bool CATProject::ModifyModules()
{
	LOG_FUNC_ENTRY("CATProject::ModifyModules");
	bool bRet = true;
	for( size_t i = 0; i < m_vModules.size(); i++ )
	{
		if (! m_vModules.at(i)->ModifyMmp() )
			bRet = false;
	}
	for( size_t i = 0; i < m_vStaticLibraries.size(); i++ )
	{
		if (! m_vStaticLibraries.at(i)->ModifyMmp() )
			bRet = false;
	}
	return bRet;
}

bool CATProject::RestoreModules()
{
	LOG_FUNC_ENTRY("CATProject::RestoreModules");
	bool bRet = true;
	for( size_t i = 0; i < m_vModules.size(); i++ )
	{
		if (! m_vModules.at(i)->RestoreMmp() )
			bRet = false;
	}
	for( size_t i = 0; i < m_vStaticLibraries.size(); i++ )
	{
		if (! m_vStaticLibraries.at(i)->RestoreMmp() )
			bRet = false;
	}
	return bRet;
}

bool CATProject::VerifyAndRecoverModules()
{
	LOG_FUNC_ENTRY("CATProject::VerifyAndRecoverModules");
	bool bRet = true;
	for( size_t i = 0; i < m_vModules.size(); i++ )
	{
		if (! m_vModules.at(i)->VerifyAndRecoverMmp() )
			bRet = false;
	}
	for( size_t i = 0; i < m_vStaticLibraries.size(); i++ )
	{
		if (! m_vStaticLibraries.at(i)->VerifyAndRecoverMmp() )
			bRet = false;
	}
	return bRet;
}
bool CATProject::Compile()
{
	switch ( m_eBuildSystem )
	{
	case SBS_V1:
		// Run Reallyclean.
		if( ! RunReallyCleanSbs1() )
			return false;
		return CompileSbs1();
	case SBS_V2:
		return CompileSbs2();
	default:
		return false;
	}
}

bool CATProject::CompileSbs1()
{
	LOG_FUNC_ENTRY("CATProject::CompileSbs1");
	string sCommand("");
	if ( m_sBuildCommand.empty() )
	{
		sCommand = "abld build";
		// -debug if urel
		if ( m_eBuildType == UREL )
			sCommand.append( " -debug" );

		// No variant
		if ( m_sVariant.empty() )
		{
			sCommand.append( " " );
			sCommand.append( m_sPlatform );
		}
		else
		{
			// Add variant
			sCommand.append( " " );
			sCommand.append( m_sPlatform );
			sCommand.append( "." );
			sCommand.append( m_sVariant );
		}

		// urel vs udeb
		sCommand.append( " " );
		sCommand.append( GetBuildTypeString() );

		// Possible target module
		AddTargetModuleIfDefined( sCommand );
	}
	// Check that build command contains -debug switch if sbs v.1 used
	else if ( ! m_sBuildCommand.empty() 
		&& m_eBuildType == UREL 
		&& m_eBuildSystem == SBS_V1 
		&& m_sBuildCommand.find( "-debug" ) == string::npos )
	{
		// Find correct index to insert -debug switch
		size_t iPos = m_sBuildCommand.find( "build" );
		if ( iPos != string::npos )
		{
			sCommand = m_sBuildCommand;
			sCommand.insert( iPos+5, " -debug" );
		}
		else
		{
			LOG_STRING("Overwriting given build command to add -debug switch. Original command is: " << m_sBuildCommand );
			if ( m_bAbldTest )
				sCommand = "abld test build -debug ";
			else
				sCommand = "abld build -debug ";
			// No variant
			if ( m_sVariant.empty() )
			{
				sCommand.append( m_sPlatform );
			}
			else
			{
				// Add variant
				sCommand.append( m_sPlatform );
				sCommand.append( "." );
				sCommand.append( m_sVariant );
			}

			// urel vs udeb
			sCommand.append( " " );
			sCommand.append( GetBuildTypeString() );
			// Possible target module
			AddTargetModuleIfDefined( sCommand );
		}
	}
	else
		sCommand = m_sBuildCommand;
	// Run command
	cout << AT_MSG_SYSTEM_CALL << sCommand << endl;
	(void)system( sCommand.c_str() );
	return true;
}

bool CATProject::CompileSbs2()
{
	LOG_FUNC_ENTRY("CATProject::CompileSbs2");
	// Create command to compile with raptor
	string sCmd( m_sBuildCommand );
	sCmd.append( RAPTOR_BUILD_LOG );
	cout << AT_MSG_SYSTEM_CALL << sCmd << endl;
	int iRet = (int)system( sCmd.c_str() );
	if ( iRet == 0 )
		return true;
	return false;
}

bool CATProject::CreateListings()
{
	// Create listings if no addr2line defined
	#ifndef ADDR2LINE
	if ( _stricmp( m_sPlatform.c_str(), "armv5" ) == 0 )
	{
		switch ( m_eBuildSystem )
		{
		case SBS_V1:
			return CreateListingsSbs1();
		case SBS_V2:
			return CreateListingsSbs2();
		default:
			return false;
		}
	}
	#endif
	return true;
}

bool CATProject::CreateListingsSbs1()
{
	LOG_FUNC_ENTRY("CATProject::CreateListingsSbs1");
	string sCommand;
	if ( m_bAbldTest )
		sCommand = "abld test listing ";
	else
		sCommand = "abld listing ";

	// Listing
	if ( m_sVariant.empty() )
	{
		// No variant
		sCommand.append( m_sPlatform );
	}
	else
	{
		// Use specified variant
		sCommand.append( m_sPlatform );
		sCommand.append( "." );
		sCommand.append( m_sVariant );
	}
	// udeb vs urel
	sCommand.append( " " );
	sCommand.append( GetBuildTypeString() );

	if ( m_vTargetModules.size() > 1 )
	{
		RunAbldCommandToAllTargets( sCommand );
	}
	else
	{
		AddTargetModuleIfDefined( sCommand ); 
		cout << AT_MSG_SYSTEM_CALL << sCommand << endl;
		(void)system( sCommand.c_str() );
	}
	return true;
}

bool CATProject::CreateListingsSbs2()
{
	LOG_FUNC_ENTRY("CATProject::CreateListingsSbs2");
	string sCmd( m_sBuildCommand );
	sCmd.append( RAPTOR_LISTING_LOG );
	sCmd.append( " LISTING");
	cout << AT_MSG_SYSTEM_CALL << sCmd << endl;
	int iRet = (int)system( sCmd.c_str() );
	if ( iRet == 0 )
		return true;
	return false;
}

bool CATProject::CopyReleasables()
{
	bool bRet = true;
	LOG_FUNC_ENTRY("CATProject::CopyReleasables");
	// Only copy releasables on armv5 platform and no addr2line defined.
	#ifndef ADDR2LINE
	if ( _stricmp( m_sPlatform.c_str(), "armv5" ) == 0 ) 
	{
		for( size_t i = 0; i < m_vModules.size(); i++ )
		{
			if ( ! m_vModules.at(i)->CopyReleasables() )
				bRet = false;
		}
		// Static libraries lst files.
		// First create directory for them (delete old one if exists).
		if ( ! DirectoryExists( AT_TEMP_LST_DIR ) )
		{
			DirCreate( AT_TEMP_LST_DIR, true );
		}
		else
		{
			DirDelete( AT_TEMP_LST_DIR, true );
			DirCreate( AT_TEMP_LST_DIR, true );
		}
		for ( size_t i = 0 ; i < m_vStaticLibraries.size(); i ++ )
		{
			if( ! m_vStaticLibraries.at(i)->CopyLstFilesToDir( AT_TEMP_LST_DIR ) )
				bRet = false;
		}

		// Delete lst files from all type of modules in project.
		// Ignoring errors because different modules might use same source/lst files.
		for( size_t i = 0; i < m_vModules.size(); i++ )
			m_vModules.at(i)->DeleteLstFilesFromSrc();
		for ( size_t i = 0 ; i < m_vUnsupportedModules.size(); i ++ )
			m_vUnsupportedModules.at(i)->DeleteLstFilesFromSrc();
		for ( size_t i = 0 ; i < m_vStaticLibraries.size(); i ++ )
			m_vStaticLibraries.at(i)->DeleteLstFilesFromSrc();
		
		return bRet;
	}
	#endif
	// When addr2line defined and used we use symbol and map file(s).
	#ifdef ADDR2LINE
	if ( _stricmp( m_sPlatform.c_str(), "armv5" ) == 0 )
	{
		// Verify that module(s) symbol file(s) exist
		for( size_t i = 0; i < m_vModules.size(); i++ )
		{
			// Check symbol file.
			if ( ! m_vModules.at(i)->SymbolFileExist() )
				bRet = false;
			// Check map  file.
			if ( ! m_vModules.at(i)->MapFileExist() )
				bRet = false;
		}
	}
	#endif
	// Platform winscw.
	else if ( _stricmp( m_sPlatform.c_str(), "winscw" ) == 0 )
	{
		// Verify that module(s) binaries exist
		for( size_t i = 0; i < m_vModules.size(); i++ )
		{
			if ( ! m_vModules.at(i)->BinaryFileExist() )
				bRet = false;
		}
		// For static libraries binary/target is same as their library.
		for ( size_t i = 0 ; i < m_vStaticLibraries.size(); i ++ )
			if ( ! m_vStaticLibraries.at(i)->BinaryFileExist() )
				bRet = false;
	}
	// Platform gcce.
	else if ( _stricmp( m_sPlatform.c_str(), "gcce" ) == 0 )
	{
		// Verify that module(s) symbol file(s) exist
		for( size_t i = 0; i < m_vModules.size(); i++ )
		{
			// Check symbol file.
			if ( ! m_vModules.at(i)->SymbolFileExist() )
				bRet = false;
		}
	}
	return bRet;
}

// ----------------------------------------------------------------------------
// Write project's (user) attributes to a file under temp folder
// ----------------------------------------------------------------------------
bool CATProject::WriteAttributes() const
{
	LOG_FUNC_ENTRY("CATProject::WriteAttributes");
	// File to write to
	string sOutFile( AT_TEMP_DIR );
	sOutFile.append( "\\" );
	sOutFile.append( AT_PROJECT_ATTRIBUTES_FILE_NAME );
	// Open file truncate if exists
	ofstream out( sOutFile.c_str(), ios_base::trunc );
	// Check file opened successfully
	if ( ! out.good() )
		return false;
	// Write attributes line by line
	out << m_bUninstrumented << endl; // Is project instrumented
	// Sbs version
	if ( m_eBuildSystem == CATProject::SBS_V2 )
		out << "SBS_2" << endl; 
	else
		out << "SBS_1" << endl;
	out << endl; // Reserved for possible binary data
	out << m_sPlatform << endl;
	out << m_sVariant << endl;
	out << m_eLoggingMode << endl;
	out << m_eBuildType << endl;
	out << m_sS60FileName << endl;
	out << m_iAllocCallStackSize << endl;
	out << m_iFreeCallStackSize << endl;
	for ( size_t i = 0 ; i < m_vTargetModules.size() ; i++ )
		out << m_vTargetModules.at(i) << AT_PROJECT_ATTRIBUTES_SEPARATOR;
	out << endl;
	out << m_sBuildCommand << endl;
	out << m_sS60FilePath << endl;
	out.close();
	return true;
}
// ----------------------------------------------------------------------------
// Reads project's (user) attributes to a file under temp folder
// ----------------------------------------------------------------------------
bool CATProject::ReadAttributes()
{
	LOG_FUNC_ENTRY("CATProject::ReadAttributes");
	// File to read on
	string sInFile( AT_TEMP_DIR );
	sInFile.append( "\\" );
	sInFile.append( AT_PROJECT_ATTRIBUTES_FILE_NAME );
	// Open file
	ifstream in( sInFile.c_str() );
	// Check file opened successfully
	if ( ! in.good() )
		return false;
	// Read attributes
	char cLine[ MAX_LINE_LENGTH ];
	string sLine;
	in.getline( cLine, MAX_LINE_LENGTH );
	int iValue = atoi( cLine );
	if ( iValue == 1 )
		m_bUninstrumented = true;
	else
		m_bUninstrumented = false;
	// Sbs version
	in.getline( cLine, MAX_LINE_LENGTH ); string sBuildSystem = string( cLine );
	if ( sBuildSystem.compare( "SBS_1" ) == 0 )
		m_eBuildSystem = CATProject::SBS_V1;
	else if ( sBuildSystem.compare( "SBS_2" ) == 0 )
		m_eBuildSystem = CATProject::SBS_V2;
	else {
		LOG_STRING("Error invalid build system defined in project.cfg");
		m_eBuildSystem = CATProject::SBS_V1;
	}
	in.getline( cLine, MAX_LINE_LENGTH ); // reserverd for possible binary timestamp or similar
	in.getline( cLine, MAX_LINE_LENGTH ); m_sPlatform = string( cLine );
	in.getline( cLine, MAX_LINE_LENGTH ); m_sVariant = string( cLine );
	in.getline( cLine, MAX_LINE_LENGTH ); m_eLoggingMode = atoi( cLine );
	in.getline( cLine, MAX_LINE_LENGTH ); m_eBuildType = atoi( cLine );
	in.getline( cLine, MAX_LINE_LENGTH ); m_sS60FileName = string( cLine );
	in.getline( cLine, MAX_LINE_LENGTH ); m_iAllocCallStackSize = atoi( cLine );
	in.getline( cLine, MAX_LINE_LENGTH ); m_iFreeCallStackSize = atoi( cLine );
	in.getline( cLine, MAX_LINE_LENGTH ); sLine = cLine;
	size_t iSpot = sLine.find( AT_PROJECT_ATTRIBUTES_SEPARATOR );
	while ( iSpot != string::npos )
	{
		string sTarget = sLine.substr(0, iSpot );
		m_vTargetModules.push_back( sTarget );
		sLine.erase(0, iSpot + AT_PROJECT_ATTRIBUTES_SEPARATOR.size() );
		iSpot = sLine.find( AT_PROJECT_ATTRIBUTES_SEPARATOR );
	}
	in.getline( cLine, MAX_LINE_LENGTH ); m_sBuildCommand = cLine;
	in.getline( cLine, MAX_LINE_LENGTH ); m_sS60FilePath = string( cLine );
	in.close();
	return true;
}

// ----------------------------------------------------------------------------
// Creates atool_temp directory to current folder if does not exist
// ----------------------------------------------------------------------------
bool CATProject::MakeTempDirIfNotExist()
{
	LOG_FUNC_ENTRY("CATProject::MakeTempDirIfNotExist");
	if ( ! DirectoryExists( AT_TEMP_DIR ) )
	{
		if( !CreateDirectory( AT_TEMP_DIR , NULL ) )
		{
			return false;
		}
	}
	return true;
}
// ----------------------------------------------------------------------------
// Utilities
// ----------------------------------------------------------------------------

bool CATProject::RunAbldCommandToAllTargets( const string& sCommand )
{
	LOG_FUNC_ENTRY("CATProject::RunAbldCommandToAllTargets");

	// Check for space at the commands end.
	string sSystemCall( sCommand );
	if ( *(sSystemCall.rbegin()) != ' ' )
		sSystemCall.append( " " );

	// Loop calls.
	bool bRet = true;
	for ( vector<string>::iterator it = m_vTargetModules.begin(); it < m_vTargetModules.end(); it++ )
	{
		string sCall( sSystemCall );
		sCall.append( RemovePathAndExt( *it, true ) );
		cout << AT_MSG_SYSTEM_CALL << sCall << endl;
		if ( (int) system( sCall.c_str() ) != 0 )
			bRet = false;
	}
	return bRet;
}

void CATProject::AddTargetModuleIfDefined( string& sCmd )
{
	LOG_FUNC_ENTRY("CATProject::AddTargetModuleIfDefined");
	// Do we have target modules defined
	if ( m_vTargetModules.size() > 0 )
	{
		switch( m_eBuildSystem )
		{
		case SBS_V1:
			// Add first target modules name without extension to build cmd.
			sCmd.append( " " );
			sCmd.append( RemovePathAndExt( m_vTargetModules.at( 0 ), true ) );
			break;
		case SBS_V2:
			// Add all target modules to build command using raptor switch '-p'.
			for( size_t i = 0 ; i < m_vTargetModules.size() ; i++ )
			{
				LOG_STRING("Adding :" << m_vTargetModules.at( i ) );
				sCmd.append( " -p " );
				sCmd.append( m_vTargetModules.at( i ) );
			}
			break;
		}
	}
}

bool CATProject::IsTargetModuleInProject() const
{
	LOG_FUNC_ENTRY("CATProject::IsTargetModuleInProject");
	vector<CATModule2*>::const_iterator modules;
	vector<CATModule2*>::const_iterator staticModules;
	vector<string>::const_iterator targets;
	bool bRet = true;
	// Do we have target modules defined
	if ( m_vTargetModules.size() > 0 )
	{
		// Sbs version 1 support only single target module.
		if ( m_eBuildSystem == SBS_V1 )
		{
			// Try to find module from project.
			bRet = false;
			string sTarget = m_vTargetModules.at(0);
			for( modules = m_vModules.begin() ; modules != m_vModules.end() ; modules++ )
			{
				if( (*modules)->GetMmpFile().find( sTarget ) != string::npos )
				{
					// Found it return true.
					bRet = true;
					break;
				}
			}
			for( staticModules = m_vStaticLibraries.begin(); staticModules != m_vStaticLibraries.end(); staticModules++ )
			{
				if( (*staticModules)->GetMmpFile().find( sTarget ) != string::npos )
				{
					bRet = true;
					break;
				}
			}
			if ( ! bRet )
			{
				// Not found display error message.
				cout << AT_MSG << "Error, " << sTarget << " not defined in project." << endl;
			}
		}
		// Sbs version 2 supports multiple target modules.
		else if ( m_eBuildSystem == SBS_V2 )
		{
			// Check that all targets are defined in project.
			for( targets = m_vTargetModules.begin(); targets != m_vTargetModules.end() ; targets++ )
			{
				// Found iterated target?
				bool bFound = false;
				for ( modules = m_vModules.begin() ; modules != m_vModules.end() ; modules++ )
				{
					if( (*modules)->GetMmpFile().find( *targets ) != string::npos )
					{
						// yes.
						bFound = true;
						break;
					}
				}
				for( staticModules = m_vStaticLibraries.begin(); staticModules != m_vStaticLibraries.end(); staticModules++ )
				{
					if( (*staticModules)->GetMmpFile().find( *targets ) != string::npos )
					{
						bFound = true;
						break;
					}
				}
				if ( ! bFound )
				{
					// Display error when not found and set return value false.
					bRet = false;
					cout << AT_MSG << "Error, " << targets->c_str() << " not defined in project." << endl;
				}
			}
		}
	}
	return bRet;
}

string CATProject::GetBuildTypeString()
{
	LOG_LOW_FUNC_ENTRY("CATProject::GetBuildTypeString");
	// Return build type as lowercase string
	switch( m_eBuildType )
	{
	case UDEB:
		return string("udeb");
	case UREL:
		return string("urel");
	default:
		return "";
	}
}

string CATProject::GetBuildTypeString( int eType )
{
	LOG_LOW_FUNC_ENTRY("CATProject::GetBuildTypeString( int eType )");
	// Return build type as lowercase string
	switch( eType )
	{
	case UDEB:
		return string("udeb");
	case UREL:
		return string("urel");
	default:
		return string("unknown");
	}
}

bool CATProject::CleanTemporaryDirs()
{
	LOG_FUNC_ENTRY("CATProject::CleanTemporaryDirs");
	bool bRet = true;
	// Modules
	for( size_t i = 0; i < m_vModules.size(); i++ )
	{
		if ( ! m_vModules.at(i)->CleanTemporaryDir() )
			bRet = false;
	}
	for( size_t i = 0; i < m_vStaticLibraries.size(); i++ )
	{
		if ( ! m_vStaticLibraries.at(i)->CleanTemporaryDir() )
			bRet = false;
	}
	// Projects
	vector<string> vFileList = DirList( AT_TEMP_DIR, false , true );
	vector<string>::iterator it2 = vFileList.begin();
	// Size of constant table
	int iCount = sizeof( TEMP_EXTENSION_NO_DELETE ) / sizeof( string );
	while ( it2 != vFileList.end() )
	{
		// Get extension and compare it to list
		bool bDelete = true;
		string sExtension = GetExtension( *it2 );
		ChangeToLower( sExtension );
		for ( int i = 0 ; i < iCount ; i++ )
		{
			if( sExtension.compare( TEMP_EXTENSION_NO_DELETE[i] ) == 0 )
			{
				bDelete = false;
				break;
			}
		}
		if ( bDelete )
		{
			// Delete file
			if ( ! FileDelete( *it2, true ) )
				bRet = false;
		}
		// Increment
		it2++;
	}
	return bRet;
}

bool CATProject::DeleteTemporaryDirs()
{
	LOG_FUNC_ENTRY("CATProject::DeleteTemporaryDirs");
	bool bRet = true;
	// Modules
	for( size_t i = 0; i < m_vModules.size(); i++ )
	{
		if (! m_vModules.at(i)->DeleteTemporaryDir() )
			bRet = false;
	}
	for( size_t i = 0; i < m_vStaticLibraries.size(); i++ )
	{
		if (! m_vStaticLibraries.at(i)->DeleteTemporaryDir() )
			bRet = false;
	}
	return bRet;
}

bool CATProject::InitSbs1MakeFileWithPathToTemp()
{
	LOG_FUNC_ENTRY("CATProject::InitSbs1MakeFileWithPathToTemp");
	// Use temporary folder
	m_sMakeFile.clear();
	m_sMakeFile.append( AT_TEMP_DIR );
	m_sMakeFile.append( "\\" );
	m_sMakeFile.append( AT_LEVEL_1_MAKEFILE_NAME );
	// At end check does it exist, return the result.
	return FileExists( m_sMakeFile.c_str() );
}

bool CATProject::InitSbs1MakeFileWithPath()
{
	LOG_FUNC_ENTRY("CATProject::InitMakeFileWithPath");
	if ( m_sEpocRoot.empty() )
	{
		LOG_STRING("Error, epocroot is not set.");
		return false;
	}
	m_sMakeFile.clear();
	m_sMakeFile.append( m_sEpocRoot );
	if( *m_sMakeFile.rbegin() != '\\' )
		m_sMakeFile.append( "\\" );
	m_sMakeFile.append( "epoc32\\build\\" );
	// Add current directory to path (first remove driveletter).
	string sCurrentDir( m_cCurrentDir );
	if ( sCurrentDir.length() < 3 )
	{
		LOG_STRING("Error, current dir invalid.");
		return false;
	}
	sCurrentDir.erase(0,3);
	m_sMakeFile.append( sCurrentDir );
	m_sMakeFile.append( "\\" );
	// Platform
	string sPlatInUpper( m_sPlatform);
	ChangeToUpper( sPlatInUpper );
	m_sMakeFile.append( sPlatInUpper );
	// Possible variant
	if ( m_sVariant.empty() )
	{
		// Test modules only?
		if ( m_bAbldTest )
			m_sMakeFile.append( "TEST" );
		m_sMakeFile.append( "." );
		m_sMakeFile.append( "MAKE" );
	}
	else
	{
		m_sMakeFile.append( "." );
		m_sMakeFile.append( m_sVariant );
		// Test modules only?
		if ( m_bAbldTest )
			m_sMakeFile.append( "TEST" );
		m_sMakeFile.append( ".MAKE" );
	}
	// At end check does it exist, return the result.
	return FileExists( m_sMakeFile.c_str() );
}

// ----------------------------------------------------------------------------
// Get & Sets
// ----------------------------------------------------------------------------
void CATProject::SetBuildSystem( BUILD_SYSTEM eSystem )
{
	LOG_FUNC_ENTRY("CATProject::SetBuildSystem");
	m_eBuildSystem = eSystem;
}
void CATProject::SetMode( PROJECT_MODE eMode )
{
	LOG_FUNC_ENTRY("CATProject::SetMode");
	m_eMode = eMode;
}
int CATProject::GetMode() const
{
	LOG_LOW_FUNC_ENTRY("CATProject::GetMode");
	return m_eMode;
}
void CATProject::SetEpocRoot( const string& sEpocRoot )
{
	LOG_FUNC_ENTRY("CATProject::SetEpocRoot");
	m_sEpocRoot = sEpocRoot;
}
void CATProject::SetPlatform( const string& sPlatform )
{
	LOG_FUNC_ENTRY("CATProject::SetPlatform");
	m_sPlatform = sPlatform;
}
void CATProject::SetVariant( const string& sVariant )
{
	LOG_FUNC_ENTRY("CATProject::SetVariant");
	m_sVariant = sVariant;
}
void CATProject::SetLoggingMode( LOGGING_MODE eLoggingMode)
{
	LOG_FUNC_ENTRY("CATProject::SetLoggingMode");
	m_eLoggingMode = eLoggingMode;
}
void CATProject::SetAllocCallStackSize( int iAllocCallStackSize )
{
	m_iAllocCallStackSize = iAllocCallStackSize;
}
void CATProject::SetFreeCallStackSize( int iFreeCallStackSize )
{
	m_iFreeCallStackSize = iFreeCallStackSize;
}
void CATProject::SetBuildType( BUILD_TYPE eType )
{
	LOG_FUNC_ENTRY("CATProject::SetBuildType");
	m_eBuildType = eType;
}
void CATProject::SetS60FileName( const string& sFileName)
{
	LOG_FUNC_ENTRY("CATProject::SetS60FileName");
	m_sS60FileName = sFileName;
}
void CATProject::SetS60FilePath( const string& sFilePath)
{
	LOG_FUNC_ENTRY("CATProject::SetS60FilePath");
	m_sS60FilePath = sFilePath;
}
void CATProject::SetRomSymbolFiles(const vector<string>& vRomSymbolFiles)
{
	LOG_FUNC_ENTRY("CATProject::SetRomSymbolFiles");
	m_vRomSymbolFiles = vRomSymbolFiles;
}
void CATProject::SetTargetModule(const string& sTargetModule)
{
	LOG_FUNC_ENTRY("CATProject::SetTargetModule");
	m_sTargetModule = sTargetModule;
	ChangeToLower( m_sTargetModule);
}
void CATProject::SetBinaryTarget(const string& sBinaryTarget)
{
	LOG_FUNC_ENTRY("CATProject::SetBinaryTarget");
	m_sBinaryTarget = sBinaryTarget;
	ChangeToLower( m_sBinaryTarget );
}
void CATProject::SetDataFile( const string& sDataFile )
{
	LOG_FUNC_ENTRY("CATProject::SetDataFile");
	m_sDataFile = sDataFile;
}
void CATProject::SetLogLevel( int iLogLevel )
{
	LOG_FUNC_ENTRY("CATProject::SetLogLevel");
	m_iLoggingLevel = iLogLevel;
}
void CATProject::SetDataFileOutput( const string& sDataFileOutput )
{
	LOG_FUNC_ENTRY("CATProject::SetDataFileOutput");
	m_sDataFileOutput = sDataFileOutput;
}
void CATProject::SetTargetModules( const vector<string>& vTargetModules )
{
	LOG_FUNC_ENTRY("CATProject::SetTargetModules");
	m_vTargetModules = vTargetModules;
}
void CATProject::SetBuildCommand( const string& sBuildCommand )
{
	LOG_FUNC_ENTRY("CATProject::SetBuildCommand");
	m_sBuildCommand = sBuildCommand;
}
// End of file
