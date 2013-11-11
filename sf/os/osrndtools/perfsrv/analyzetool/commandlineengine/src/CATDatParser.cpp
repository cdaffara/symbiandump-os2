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
* Description:  Class responsible to parse data files
*
*/


#include "../inc/CATDatParser.h"
#include "../inc/CATProject.h"
#include "../inc/CATModule2.h"
#include "../inc/CATMemoryAddress.h"
#include "../inc/catromsymbol.h"

// -----------------------------------------------------------------------------
// CATDatParser::CATDatParser
// Constructor only for testing!
// (No module vector defined so no locating codelines / call stacks)
// -----------------------------------------------------------------------------
CATDatParser::CATDatParser()
{
	LOG_FUNC_ENTRY("CATDatParser::CATDatParser");
	Construct();
}

// -----------------------------------------------------------------------------
// CATDatParser::CATDatParser
// Constructor
// -----------------------------------------------------------------------------
CATDatParser::CATDatParser(vector<CATModule2*>* pModules )
{
	LOG_FUNC_ENTRY("CATDatParser::CATDatParser");
	Construct();
	m_pModules = pModules;
}

// -----------------------------------------------------------------------------
// CATDatParser::Construct
// "Real" constructor
// -----------------------------------------------------------------------------
void CATDatParser::Construct()
{
	LOG_FUNC_ENTRY("CATDatParser::Construct");

	m_iDataVersion = 1; // Default version of data.
	m_bDllLoadFound = false;
	m_bProcessStartFound = false;
	m_bSubtestOnGoing = false;

	m_DataSaver.InitXML();
	
	m_eBuildType = -2;
	m_eProcess_state = not_started;
	m_eProjectBuildType = -1;

	m_iCurrentProcessId = 0;
	m_iLeakNumber = 0;
	m_iLogLevel = 3;
	m_iOffSet = 0;

	m_iPinPointedLeaks = 0;
	m_iPinPointedSubTestLeaks = 0;
	m_iSubtestStartHandleCount = 0;
	m_iSuccesfullRuns = 0;
	m_iTotalNumberOfLeaks = 0;
	m_iTotalRuns = 0;

	m_pRomSymbol = 0;
	m_pModules = 0;

	m_sCurrentProcessName = "";
	m_sInputFile = "";
	m_sInputFileTemp = "";
	m_sOutputFile = "";
	m_sProjectPlatform = "";
	m_vRomSymbolFiles.clear();
	m_vDllLoadModList.clear();
	m_vDllLoadModListSubTest.clear();
	m_vHandleLeaks.clear();
	m_vMemoryAddress.clear();
}

// -----------------------------------------------------------------------------
// CATDatParser::~CATDatParser
// Destructor
// -----------------------------------------------------------------------------
CATDatParser::~CATDatParser()
{
	LOG_FUNC_ENTRY("CATDatParser::~CATDatParser");

	if ( m_In.is_open() )
		m_In.close();
	// Delete temporary input file if any
	if ( !m_sInputFileTemp.empty() )
	{
		if ( FileExists( m_sInputFileTemp.c_str() ) )
			FileDelete( m_sInputFileTemp, false );
	}
	// Clean memory addresses if any
	CleanMemoryAddresses();
	// Delete rom symbol.
	if ( m_pRomSymbol )
	{
        delete m_pRomSymbol;
		m_pRomSymbol = NULL;
	}
}

// -----------------------------------------------------------------------------
// CATDatParser::Analyze
// Analyze given data file
// -----------------------------------------------------------------------------
int CATDatParser::Analyze()
{
	LOG_FUNC_ENTRY("CATDatParser::Analyze");
	// Return if input file not set
	if ( m_sInputFile.empty() )
		return AT_RETURN_CODE::INVALID_DATA_FILE;
	// If open close first
	if ( m_In.is_open() )
		m_In.close();
	// Open file
	m_In.open( m_sInputFile.c_str() );
	if ( ! m_In.good() )
		return AT_RETURN_CODE::INVALID_DATA_FILE;
	try {
		// If rom symbol file specified.
		if ( ! m_vRomSymbolFiles.empty() )
		{
			// Create new rom symbol file "parser".
			m_pRomSymbol = new CATRomSymbol();
			m_pRomSymbol->m_bShowProgressMessages = true;
			// Set symbol files.
			if ( ! m_pRomSymbol->SetSymbols( m_vRomSymbolFiles ) )
			{
				cout << AT_MSG << "Rom/Rofs symbols error: " << m_pRomSymbol->GetError() << endl;
				// If file open fails we delete it and will not use it.
				delete m_pRomSymbol;
				m_pRomSymbol = NULL;
				cout << AT_MSG << "Analyze aborted." << endl;
				return AT_RETURN_CODE::SYMBOL_FILE_ERROR;
			}
		}
		// Return code
		int iRet = 0;
		// Clear variables
		ClearParsingVariables();
		// If output defined disable printing
		if ( ! m_sOutputFile.empty() )
			m_DataSaver.SetPrintFlag( false );
		// Header
		Header();
		// Parsing
		iRet = Parse();
		// Footer
		if ( iRet == AT_RETURN_CODE::OK )
			Footer();
		// If output defined save xml
		if ( ! m_sOutputFile.empty() )
			m_DataSaver.SaveLinesToFile( m_sOutputFile.c_str(), XML_DATA );
		// Return
		return iRet;
	} catch ( int i )
	{
		cout << AT_MSG << "Error, Analyze failed. : " << i << endl;
		return AT_RETURN_CODE::UNHANDLED_EXCEPTION;
	}
}
// -----------------------------------------------------------------------------
// CATDatParser::Header
// Print header of report
// -----------------------------------------------------------------------------
void CATDatParser::Header()
{
	LOG_FUNC_ENTRY("CATDatParser::Header");
	// Analyze report header
	m_DataSaver.AddString( "Atool.exe v." );
	m_DataSaver.AddString( ATOOL_VERSION );
	m_DataSaver.AddString( "\n" );
	m_DataSaver.AddString( "Analyzing memory leaks..." );
	m_DataSaver.AddLineToLast();
}

// -----------------------------------------------------------------------------
// CATDatParser::Footer
// Print footer of report
// -----------------------------------------------------------------------------
void CATDatParser::Footer()
{
	LOG_FUNC_ENTRY("CATDatParser::Footer");
	m_DataSaver.AddString( "\nTotal Runs: " );
	m_DataSaver.AddInteger( m_iTotalRuns );
	m_DataSaver.AddLineToLast();

	int iFailedRuns = m_iTotalRuns - m_iSuccesfullRuns;
	m_DataSaver.AddString( "Failed Runs: " );
	m_DataSaver.AddInteger( iFailedRuns );
	m_DataSaver.AddLineToLast();

	char cTemp[128];
	string sResult( itoa( m_iTotalRuns, cTemp, 10 ) );
	sResult.append( ";" );
	sResult.append( itoa( iFailedRuns, cTemp, 10 ) );
	sResult.append( ";" );

	m_DataSaver.SaveXML( sResult, RESULT );
}

// -----------------------------------------------------------------------------
// CATDatParser::ClearParsingVariables
// Clear/Reset all member variables related to parsing data file
// -----------------------------------------------------------------------------
void CATDatParser::ClearParsingVariables()
{
	LOG_FUNC_ENTRY("CATDatParser::ClearParsingVariables");
	// Clear variables related to analyze
	m_eProcess_state = not_started;
	m_bProcessStartFound = false;
	m_bDllLoadFound = false;
	m_iTotalNumberOfLeaks = 0;
	m_iPinPointedLeaks = 0;
	m_iLeakNumber = 0;
	m_iTotalRuns = 0;
	m_iSuccesfullRuns = 0;
	m_bSubtestOnGoing = false;
	m_iSubtestStartHandleCount = 0;
	CleanMemoryAddresses();
}

// -----------------------------------------------------------------------------
// CATDatParser::Parse
// Parses data file. Note! header and footer of the report are done in 
// separate functions.
// -----------------------------------------------------------------------------
int CATDatParser::Parse()
{
	LOG_FUNC_ENTRY("CATDatParser::Parse");
	// Read all lines
	char cLine[MAX_LINE_LENGTH];
	do
	{
		string sLine;
		try {
			m_In.getline( cLine, MAX_LINE_LENGTH );
			sLine = cLine ;
		} catch(...)
		{
			LOG_STRING( AT_MSG << "Unexpected error, reading data file." );
			continue;
		}
		if( sLine.find( LABEL_DATA_FILE_VERSION ) != string::npos )
		{
			// Check data file version
			if(  sLine.find( AT_DATA_FILE_VERSION ) == string::npos )
			{
				return AT_RETURN_CODE::WRONG_DATA_FILE_VERSION;
			}
		}
		else if( sLine.find( LABEL_PROCESS_START ) != string::npos )
		{
			if ( ! ParseProcessStart( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
		else if( sLine.find( LABEL_DLL_LOAD ) != string::npos )
		{
			if ( ! ParseDllLoad( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
		else if( sLine.find( LABEL_DLL_UNLOAD ) != string::npos )
		{
			if ( ! ParseDllUnload( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
		else if( sLine.find( LABEL_MEM_LEAK ) != string::npos)
		{
			if ( ! ParseMemLeak( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
		else if( sLine.find( LABEL_PROCESS_END ) != string::npos )
		{
			if ( ! ParseProcessEnd( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
		else if( sLine.find( LABEL_ERROR_OCCURED ) != string::npos )
		{
			if ( ! ParseErrorOccured( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
		else if( sLine.find( LABEL_HANDLE_LEAK ) != string::npos )
		{
			if ( ! ParseHandleLeak( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
		else if( sLine.find( LABEL_TEST_START ) != string::npos )
		{
			if ( ! ParseTestStart( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
		else if( sLine.find( LABEL_TEST_END ) != string::npos )
		{
			if ( ! ParseTestEnd( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
		else if( sLine.find( LABEL_LOGGING_CANCELLED ) != string::npos )
		{
			if ( ! ParseLoggingCancelled( sLine ) )
				return AT_RETURN_CODE::ANALYZE_ERROR;
		}
	}
	while( m_In.good() );
	// Message of failed run if process start was last line in data.
	if ( m_eProcess_state == ongoing )
	{
		m_DataSaver.AddString( "Test run failed.\n" );
		m_DataSaver.AddLineToLast();
	}
	return AT_RETURN_CODE::OK;
}

// -----------------------------------------------------------------------------
// CATDatParser::ParseProcessStart
// -----------------------------------------------------------------------------
bool CATDatParser::ParseProcessStart( string& sLine)
{
	LOG_FUNC_ENTRY("CATDatParser::ParseProcessStart");
	if ( m_eProcess_state == ongoing )
	{
		m_DataSaver.AddString( "Test run failed.\n" );
		m_DataSaver.AddLineToLast();
	}
	m_eProcess_state = ongoing;
	m_bProcessStartFound = true;

	// Clear handle leaks
	m_vHandleLeaks.clear();
	// Increment runs
	m_iTotalRuns++;
	// Clean leak count
	m_iTotalNumberOfLeaks = 0;
	// Clean pin pointed leaks count.
	m_iPinPointedLeaks = 0;
	// Clean leak number
	m_iLeakNumber = 0;

	// Clean loaded mods
	m_vDllLoadModList.clear();
	m_vDllLoadModListSubTest.clear();

	// Skip text PROCESS_START
	GetStringUntilNextSpace( sLine );
	// Get process name
	m_sCurrentProcessName = GetStringUntilNextSpace( sLine );
	// Get Pid
	string sPid = GetStringUntilNextSpace( sLine );
	m_iCurrentProcessId = _httoi( sPid.c_str() );

	// Header for process start
	m_DataSaver.AddString( "\n--------------------------------\n" );
	m_DataSaver.AddString( "Test Run start (" );
	m_DataSaver.AddString( m_sCurrentProcessName.c_str() );
	m_DataSaver.AddString( "): " );

	// Get start time
	string sTime = GetStringUntilNextSpace( sLine );
	sTime = ConvertTimeToLocalTime( sTime );
	m_DataSaver.AddString( sTime.c_str() );

	// Create data for xml
	string sData( sTime );
	sData.append( ";" );
	
	// Build mode UDEB/UREL.
	string sBuildType = GetStringUntilNextSpace( sLine );

	m_DataSaver.AddString( " Build target: " );
	if( sBuildType.compare( "0" ) == 0 )
	{
		m_eBuildType = CATProject::UREL;
	}
	else if( sBuildType.compare( "1" ) == 0 )
	{
		m_eBuildType = CATProject::UDEB;
	}
	m_DataSaver.AddString( CATProject::GetBuildTypeString( m_eBuildType ).c_str() );

	// Version.
	string sVersion = GetStringUntilNextSpace( sLine );
	unsigned int iVer = 0;
	if ( hexToDec( sVersion, iVer ) && iVer != 0 )
		m_iDataVersion = iVer;

	// End line in data.
	m_DataSaver.AddLineToLast();
	
	// xml
	sData.append( CATProject::GetBuildTypeString( m_eBuildType ) );
	sData.append( ";" );
	sData.append( m_sCurrentProcessName );
	m_DataSaver.SaveXML( sData, RUN );

	// If projects platform defined check that it is same in data. (future feature).
	if ( ! m_sProjectPlatform.empty() )
	{
		// If platform info is added to data file do check here.
	}
	// If projects build type defined check that it is same in data.
	if ( m_eProjectBuildType != -1 )
	{
		if ( m_eBuildType != m_eProjectBuildType )
		{
			string sError(AT_MSG);
			sError.append( "Error, analyzed data has build type of " );
			sError.append( CATProject::GetBuildTypeString( m_eBuildType ) );
			sError.append( " and project has build type " );
			sError.append( CATProject::GetBuildTypeString( m_eProjectBuildType ) );
			sError.append( ". Pinpointed code lines are not valid." );
			m_DataSaver.AddString( sError.c_str(), false );
			m_DataSaver.AddLineToLast();
		}
	}
	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::ParseProcessEnd
// -----------------------------------------------------------------------------
bool CATDatParser::ParseProcessEnd( string& sLine )
{
	LOG_FUNC_ENTRY("CATDatParser::ParseProcessEnd");
	GetStringUntilNextSpace( sLine );

	// Get process id
	string sProcessID = GetStringUntilNextSpace( sLine );
	unsigned long iProcessID = _httoi( sProcessID.c_str() );

	// Get time
	string sTime = GetStringUntilNextSpace( sLine );

	// Convert leak time
	sTime = ConvertTimeToLocalTime( sTime );

	// Process started?
	if( iProcessID == m_iCurrentProcessId )
	{
		m_iSuccesfullRuns++;
		m_DataSaver.AddLineToLast();
		m_DataSaver.AddString( "Test Run end (" );
		m_DataSaver.AddString( m_sCurrentProcessName.c_str() );
		m_DataSaver.AddString( "): " );
		m_DataSaver.AddString( sTime.c_str() );
		m_DataSaver.AddLineToLast();
		m_DataSaver.AddString( "Build target: " );
		m_DataSaver.AddString( CATProject::GetBuildTypeString( m_eBuildType ).c_str() );
		m_DataSaver.AddLineToLast();

		m_eProcess_state = stopped;
		// Number of leaks
		if ( m_iLogLevel == 1 || m_iLogLevel == 2 )
		{
			if ( m_iPinPointedLeaks > 0 )
			{
				m_DataSaver.AddInteger( m_iPinPointedLeaks );
				m_DataSaver.AddString( " number of pinpointed memory leak(s)." );
				m_DataSaver.AddLineToLast();
			}
			m_DataSaver.AddInteger( m_iLeakNumber );
			m_DataSaver.AddString( " total number of memory leak(s)." );
			m_DataSaver.AddLineToLast();
		}
		else
		{
			m_DataSaver.AddInteger( m_iTotalNumberOfLeaks );
			m_DataSaver.AddString( " memory leak(s) found." );
			m_DataSaver.AddLineToLast();
		}
		
		// xml
		char cTemp[128];
		m_DataSaver.SaveXML( itoa( m_iTotalNumberOfLeaks, cTemp, 10 ) , MEM_LEAKS );

		// Print all modules which have leaks
		for( size_t i = 0 ; i < m_vDllLoadModList.size() ; i++ )
		{
			if( m_vDllLoadModList.at(i).iLeaks > 0 )
			{
				m_DataSaver.AddInteger( m_vDllLoadModList.at(i).iLeaks );
				m_DataSaver.AddString( " memory leak(s) in module: " );
				m_DataSaver.AddString( m_vDllLoadModList.at(i).sModuleName.c_str() );
				m_DataSaver.AddLineToLast();

				// xml
				string sModuleNameAndLeaks( m_vDllLoadModList[i].sModuleName );
				sModuleNameAndLeaks.append(";");
				sModuleNameAndLeaks.append( itoa( m_vDllLoadModList[i].iLeaks, cTemp, 10 ) );
				m_DataSaver.SaveXML( sModuleNameAndLeaks , MEM_LEAK_MODULE );
			}
		}
		
		if ( m_vHandleLeaks.size() > 0 )
		{
			// We have handle leaks
			bool bHandLeaksFound = false;
			int iTotalNrOfLeaks = 0;
			// Print handle leaks
			for( size_t i = 0 ; i < m_vHandleLeaks.size() ; i++ )
			{
				string sTempHandleLeak( m_vHandleLeaks[i] );
				// Count.
				string sNrOfLeaks( GetStringUntilNextSpace(sTempHandleLeak) );

				// Name.
				//string sHandleLeakModule( GetStringUntilNextSpace( sTempHandleLeak ) );
				string sHandleLeakModule( "Unknown" );

				unsigned long iNrOfLeaks = _httoi( sNrOfLeaks.c_str() );
				iTotalNrOfLeaks += iNrOfLeaks;
				if( iNrOfLeaks )
				{
					if( !bHandLeaksFound )
					{
						m_DataSaver.SaveXML( sNrOfLeaks , HANDLE_LEAKS );
					}
					bHandLeaksFound = true;
					m_DataSaver.AddInteger( iNrOfLeaks );
					// Just print out how many leaks found.
					// Because its always unknown.
					m_DataSaver.AddString( " handle leak(s) found." );
					m_DataSaver.AddLineToLast();

					// xml
					string sXMLInfo( sHandleLeakModule );
					sXMLInfo.append( ";" ); sXMLInfo.append( sNrOfLeaks );
					m_DataSaver.SaveXML( sXMLInfo , HANDLE_LEAK_MODULE );
				}
			}
			// Update number if handle leaks
			m_DataSaver.SaveXML( itoa( iTotalNrOfLeaks, cTemp, 10 ) , HANDLE_LEAKS );
			if( !bHandLeaksFound )
			{
				//m_DataSaver.AddLineToLast();
				m_DataSaver.AddString( TEXT_NO_HANDLE_LEAKS );
				m_DataSaver.AddLineToLast();
			}
		}
		else
		{
			// No handle leaks
			m_DataSaver.AddLineToLast();
			m_DataSaver.AddString( TEXT_NO_HANDLE_LEAKS );
			m_DataSaver.AddLineToLast();
		}

		// Process end to xml
		m_DataSaver.SaveXML( sTime, RUN_END );
		// Reset current process
		m_iCurrentProcessId = 0;
	}
	
	// If no dll load or process start found
	if ( ! m_bProcessStartFound || !m_bDllLoadFound )
	{
		m_DataSaver.AddLineToLast();
		m_DataSaver.AddString( AT_ANALYZE_INSUFFICIENT_LOGGING_DATA );
		m_DataSaver.AddLineToLast();
	}
	
	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::ParseDllLoad
// -----------------------------------------------------------------------------
bool CATDatParser::ParseDllLoad( string& sLine )
{
	LOG_FUNC_ENTRY("CATDatParser::ParseDllLoad");
	//DLL_LOAD <DLL name> <Time stamp> <Memory start address> <Memory end address>
	m_bDllLoadFound = true;
	DLL_LOAD_INFO structDllInfo;
	structDllInfo.iStartAddress = 0;
	structDllInfo.iEndAddress = 0;
	structDllInfo.iLeaks = 0;

	// Skip "DLL_LOAD "
	GetStringUntilNextSpace( sLine );

	// Get module name
	structDllInfo.sModuleName = GetStringUntilNextSpace( sLine );
	ChangeToLower( structDllInfo.sModuleName );

	// Create module from this if project platform emulator
	if ( _stricmp( "winscw", m_sProjectPlatform.c_str() ) == 0 )
		CreateWinscwModule( structDllInfo.sModuleName );

	// Get dll start memory string address from line
	// Convert string address to real memory address
	structDllInfo.iStartAddress = 
		_httoi( GetStringUntilNextSpace( sLine ).c_str() );

	// Get dll end memory string address from line
	// Convert string address to real memory address
	structDllInfo.iEndAddress = 
		_httoi( 
		GetStringUntilNextSpace( sLine ).c_str() );

	if ( m_iDataVersion >= AT_DLL_TIMESTAMP_DATA_VERSION )
	{
		// Pickup module loading time.
		string sLoadTime = GetStringUntilNextSpace( sLine );
		unsigned long long ull;
		if ( hexToDec( sLoadTime, ull ) )
			structDllInfo.iLoadTime = ull;
	}

	// Is module already loaded, if not add it to list.
	bool bFound = false;
	for( vector<DLL_LOAD_INFO>::iterator it = m_vDllLoadModList.begin();
		it != m_vDllLoadModList.end() ; it++ )
	{
		if( (*it).sModuleName.compare( structDllInfo.sModuleName ) == 0 )
		{
			bFound = true;
			break;
		}
	}
	if( ! bFound )
		m_vDllLoadModList.push_back( structDllInfo );

	// Sub test module list.
	bFound = false;
	for( vector<DLL_LOAD_INFO>::iterator it = m_vDllLoadModListSubTest.begin();
		it != m_vDllLoadModListSubTest.end() ; it++ )
	{
		if( (*it).sModuleName.compare( structDllInfo.sModuleName ) == 0 )
		{
			bFound = true;
			break;
		}
	}
	if( ! bFound )
		m_vDllLoadModListSubTest.push_back( structDllInfo );

	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::ParseDllUnload
// -----------------------------------------------------------------------------
bool CATDatParser::ParseDllUnload( string& sLine )
{
	LOG_FUNC_ENTRY("CATDatParser::ParseDllUnload");

	// Ignore unloads on older version because no timestamps.
	if ( m_iDataVersion < AT_DLL_TIMESTAMP_DATA_VERSION )
	{
		return true;
	}

	// Skip "DLL_UNLOAD "
	GetStringUntilNextSpace( sLine );

	// Get module name
	string sModuleName = GetStringUntilNextSpace( sLine );
	ChangeToLower( sModuleName );

	// skip adresses - not currently used
	GetStringUntilNextSpace( sLine );
	GetStringUntilNextSpace( sLine );

	// Unload time
	unsigned long long ull;
	string sUnload = GetStringUntilNextSpace( sLine );
	if ( ! hexToDec( sUnload, ull ) )
		return true;

	// Set module unload time.
	vector<DLL_LOAD_INFO>::iterator it;
	for( it = m_vDllLoadModList.begin() ; it != m_vDllLoadModList.end() ; it++ )
	{
		if ( sModuleName.compare( it->sModuleName ) == 0 )
		{
			(*it).iUnloadTime = ull;
			break;
		}
	}
	for( it = m_vDllLoadModListSubTest.begin() ; it != m_vDllLoadModListSubTest.end() ; it++ )
	{
		if ( sModuleName.compare( it->sModuleName ) == 0 )
		{
			(*it).iUnloadTime = ull;
			break;
		}
	}
	return true;
}
// -----------------------------------------------------------------------------
// CATDatParser::ParseLoggingCancelled
// -----------------------------------------------------------------------------
bool CATDatParser::ParseLoggingCancelled( string& sLine )
{
	LOG_FUNC_ENTRY("CATDatParser::ParseLoggingCancelled");
	// Skip text "LOGGING_CANCELLED"
	GetStringUntilNextSpace( sLine );

	// Get time
	string sTime( GetStringUntilNextSpace( sLine ) );
	sTime = ConvertTimeToLocalTime( sTime );
	m_DataSaver.AddString( "Logging Cancelled." );
	m_DataSaver.AddLineToLast();
	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::ParseHandleLeak
// -----------------------------------------------------------------------------
bool CATDatParser::ParseHandleLeak( string& sLine )
{
	LOG_FUNC_ENTRY("CATDatParser::ParseHandleLeak");
	// Skip text "HANDLE_LEAK"
	GetStringUntilNextSpace( sLine );
	m_vHandleLeaks.push_back( sLine );
	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::ParseTestStart
// -----------------------------------------------------------------------------
bool CATDatParser::ParseTestStart( string& sLine )
{
	LOG_FUNC_ENTRY("CATDatParser::ParseTestStart");
	m_bSubtestOnGoing = true;
	m_iLeakNumber = 0;
	m_iPinPointedSubTestLeaks = 0;

	// Reset subtest leaked modules list
	for( size_t i = 0 ; i < m_vDllLoadModListSubTest.size() ; i++ )
	{
		m_vDllLoadModListSubTest.at(i).iLeaks = 0;
	}

	// Skip text "TEST_START"
	GetStringUntilNextSpace( sLine );
	// Time
	string sTime( GetStringUntilNextSpace( sLine ) );
	sTime = ConvertTimeToLocalTime( sTime );
	// Name
	string sSubTestName( GetStringUntilNextSpace( sLine ) );				
	m_DataSaver.AddLineToLast();

	// Get handle count in subtest start
	string sSubTestStartHandleCount( GetStringUntilNextSpace( sLine ) );
	m_iSubtestStartHandleCount = atoi( sSubTestStartHandleCount.c_str() );

	// Add start to report
	m_DataSaver.AddString( "\nSub test (" );
	m_DataSaver.AddString( sSubTestName.c_str() );
	m_DataSaver.AddString( ") start: " );
	m_DataSaver.AddString( sTime.c_str() );

	// m_DataSaver.AddLineToLast();

	// Add start to xml
	string sResult( sSubTestName );
	sResult.append( ";" );
	sResult.append( sTime );
	sResult.append( ";" );
	m_DataSaver.SaveXML( sResult, TEST_START );
	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::ParseTestEnd
// -----------------------------------------------------------------------------
bool CATDatParser::ParseTestEnd( string& sLine )
{
	LOG_FUNC_ENTRY("CATDatParser::ParseTestEnd");
	// Skip text "TEST_END"
	GetStringUntilNextSpace( sLine );

	// Time
	string sTime( GetStringUntilNextSpace( sLine ) );
	sTime = ConvertTimeToLocalTime( sTime );

	// Name
	string sSubTestName( GetStringUntilNextSpace( sLine ) );
	m_DataSaver.AddLineToLast();

	// Add test end info to report
	m_DataSaver.AddString( "Sub test (" );
	m_DataSaver.AddString( sSubTestName.c_str() );
	m_DataSaver.AddString( ") end: " );
	m_DataSaver.AddString( sTime.c_str() );
	m_DataSaver.AddLineToLast();

	// Leak count to report in subtest
	if( m_iLeakNumber > 0 )
	{
		if ( m_iLogLevel == 1 || m_iLogLevel == 2 )
		{
			m_DataSaver.AddInteger( m_iPinPointedSubTestLeaks );
			m_DataSaver.AddString( " number of pinpointed memory leaks." );
			m_DataSaver.AddLineToLast();
			m_DataSaver.AddInteger( m_iLeakNumber );
			m_DataSaver.AddString( " memory leaks found." );
		}
		else
		{
			m_DataSaver.AddInteger( m_iLeakNumber );
			m_DataSaver.AddString( " memory leaks found." );
		}
	}
	else
	{
		m_DataSaver.AddString( "No memory leaks found." );
	}
	m_DataSaver.AddLineToLast();

	// Use sTime to store info to xml
	sTime.append(";");
	char cTemp[128];
	// Print all modules whitch have leaks
	for( unsigned int i = 0 ; i < m_vDllLoadModListSubTest.size() ; i++ )
	{
		if( m_vDllLoadModListSubTest.at(i).iLeaks > 0 )
		{
			// Normal report
			m_DataSaver.AddInteger( m_vDllLoadModListSubTest[i].iLeaks );
			m_DataSaver.AddString( " memory leaks in module: " );
			m_DataSaver.AddString( m_vDllLoadModListSubTest.at(i).sModuleName.c_str() );
			m_DataSaver.AddLineToLast();
			// xml
			string sModuleNameAndLeaks( m_vDllLoadModListSubTest.at(i).sModuleName );
			sModuleNameAndLeaks.append(";");
			sModuleNameAndLeaks.append( itoa( m_vDllLoadModListSubTest.at(i).iLeaks, cTemp, 10 ) );
			m_DataSaver.SaveXML( sModuleNameAndLeaks , SUBTEST_MEM_LEAK_MODULE );
		}
	}
	// Handle count
	int iEndHandleCount = atoi( GetStringUntilNextSpace( sLine ).c_str() );
	// Is there handle leaks in subtest?
	if( iEndHandleCount > m_iSubtestStartHandleCount )
	{
		// Print normal report
		m_DataSaver.AddInteger( iEndHandleCount - m_iSubtestStartHandleCount );
		m_DataSaver.AddString( " handle leaks in subtest: " );
		m_DataSaver.AddString( sSubTestName.c_str() );
		m_DataSaver.AddString( "." );
		m_DataSaver.AddLineToLast();

		// Print handle leaks to XML
		string sNrOfHandleLeaks( itoa( iEndHandleCount - m_iSubtestStartHandleCount, cTemp, 10 ) );
		sNrOfHandleLeaks.append( ";" );
		m_DataSaver.SaveXML( sNrOfHandleLeaks, SUBTEST_HANDLE_LEAKS );
	}
	else
	{
		// No handle leaks
		m_DataSaver.AddString( TEXT_NO_HANDLE_LEAKS );
		m_DataSaver.AddLineToLast();
	}
	// Save xml
	m_DataSaver.SaveXML( sTime, TEST_END );
	// Back to normal leaks
	m_bSubtestOnGoing = false;
	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::ParseErrorOccured
// -----------------------------------------------------------------------------
bool CATDatParser::ParseErrorOccured( string& sLine )
{
	LOG_FUNC_ENTRY("CATDatParser::ParseErrorOccured");
	string sTime,sError;

	// Skip text "ERROR_OCCURED:"
	GetStringUntilNextSpace( sLine );

	// Get error
	sError = GetStringUntilNextSpace( sLine );
	// Get and convert error time
	sTime = GetStringUntilNextSpace( sLine );
	sTime = ConvertTimeToLocalTime( sTime );

	// Print error line
	m_DataSaver.AddLineToLast();
	m_DataSaver.AddString( "Error occured on: " );
	m_DataSaver.AddString( sTime.c_str() );
	m_DataSaver.AddString( ". " );
	m_DataSaver.AddString( "Symbian error code: " );
	m_DataSaver.AddString( sError.c_str() );
	m_DataSaver.AddString( "." );
	m_DataSaver.AddLineToLast();

	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::ParseMemLeak
// -----------------------------------------------------------------------------
bool CATDatParser::ParseMemLeak( string& sLine )
{
	LOG_FUNC_ENTRY("CATDatParser::ParseMemLeak");
	// Increment leak count
	if ( ! m_bSubtestOnGoing )
		m_iTotalNumberOfLeaks++;

	// Increase leak number
	m_iLeakNumber++;

	// Leak data variables
	string sModuleName;
	string sLeakSize;
	string sTime;
	unsigned long long iTime = 0;
	string sLeakAddress;
	
	// Skip text "MEM_LEAK"
	GetStringUntilNextSpace( sLine );
	// Get leak address
	sLeakAddress = GetStringUntilNextSpace( sLine );
	// Get time
	sTime = GetStringUntilNextSpace( sLine );
	// Convert time to decimal
	hexToDec( sTime, iTime );
	// Get memory reserve size
	sLeakSize = GetStringUntilNextSpace( sLine );
	// Convert leak time
	sTime = ConvertTimeToLocalTime( sTime );

	// Loop thru call stack and put memory addresses in vector
	CleanMemoryAddresses(); // Clean memory address vector
	CATMemoryAddress* pMemAddr = 0;
	vector<string> vStrings = ParseStringToVector( sLine, ' ' );
	for( size_t i = 0; i < vStrings.size() ; i++ )
	{
		pMemAddr = new CATMemoryAddress( vStrings.at(i), m_iOffSet );
		// Set address time
		pMemAddr->SetTime( iTime );
		// Set address module name
		if ( pMemAddr->FindSetModuleName( &m_vDllLoadModList ) )
		{
			// Increment leaks in module once
			if ( sModuleName.empty() )
			{
				if ( m_bSubtestOnGoing )
					m_vDllLoadModListSubTest.at( pMemAddr->GetDllLoadInfoIndex() ).iLeaks++;
				else
					m_vDllLoadModList.at( pMemAddr->GetDllLoadInfoIndex() ).iLeaks++;
				// Set leak's module where it was located.
				sModuleName = pMemAddr->GetModuleName();
			}
		}
		// Add it to vector
		m_vMemoryAddress.push_back( pMemAddr );
	}
	// If logging level is 0 Skip printing / locating code lines for call stack items.
	if ( m_iLogLevel == 0 )
		return true;
	if ( m_pModules && vStrings.size() > 0 )
	{
		// Have we successfully located code line for memory address
		bool bSuccesfullAddressToLine = false;
		for( size_t x = 0; x < m_vMemoryAddress.size(); x++ )
		{
			int iIndexInDll = m_vMemoryAddress.at( x )->GetDllLoadInfoIndex();
			if ( iIndexInDll != -1 )
			{
				// Dll module name from data file
				string sDllName = m_vDllLoadModList.at( iIndexInDll ).sModuleName;
				// Find module from project. These are read from makefiles.
				for ( size_t y = 0; y < m_pModules->size() ; y++ )
				{
					// Module name from project data (makefiles)
					string sModuleName = m_pModules->at( y )->GetBinaryName();
					// If we find module from project ones, use it to located code line for memory address
					// Note! dll names can be pretty messy i.e. DLL_LOAD 10281fc6.dll{000a0000}[10281fc6] 81d57b88 81e60a90
					if ( sDllName.find( sModuleName ) != string::npos )
					{
						m_pModules->at( y )->AddressToLine( m_vMemoryAddress.at( x ) );
						if ( ! bSuccesfullAddressToLine )
						{
							int iPPState = m_vMemoryAddress.at( x )->GetAddressToLineState();
							if ( iPPState == CATMemoryAddress::EXACT || iPPState == CATMemoryAddress::FUNCTION )
							{
								bSuccesfullAddressToLine = true;
								if ( m_bSubtestOnGoing )
									m_iPinPointedSubTestLeaks++;
								else
									m_iPinPointedLeaks++;
							}
						}
					}
				}
			}
		}
		// If rom/rofs specified we use it to try get binary and function names
		// for addresses currently out of process range.
		if ( m_pRomSymbol )
		{
			for( size_t x = 0; x < m_vMemoryAddress.size(); x++ )
			{
				if ( m_vMemoryAddress.at(x)->GetAddressToLineState() == CATMemoryAddress::OUT_OF_PROCESS
					|| m_vMemoryAddress.at(x)->GetAddressToLineState() == CATMemoryAddress::OUT_OF_RANGE )
				{
					m_pRomSymbol->AddressToLine( m_vMemoryAddress.at(x) );
				}
			}
		}
	}
	// Print leak
	PrintMemLeak( sTime, sLeakSize, sLeakAddress, sModuleName);
	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::PrintMemLeak
// -----------------------------------------------------------------------------
void CATDatParser::PrintMemLeak(const string& sTime,
							   const string& sLeakSize,
							   const string& sLeakAddr,
							   const string& sModuleName)
{
	LOG_FUNC_ENTRY("CATDatParser::PrintMemLeak");
	// Print header data of leak
	m_DataSaver.AddString("\nMemory leak ");
	m_DataSaver.AddInteger( m_iLeakNumber, true);

	// Leak size
	m_DataSaver.AddString( " (" );
	m_DataSaver.AddInteger( _httoi( sLeakSize.c_str() ), true );
	m_DataSaver.AddString( " bytes) " );

	// Leak address
	m_DataSaver.AddString("(0x");
	m_DataSaver.AddString( sLeakAddr.c_str(), true );
	m_DataSaver.AddString( ") " );

	// Time
	m_DataSaver.AddString( sTime.c_str(), true );
	m_DataSaver.AddString( " " );
	
	// Module name
	m_DataSaver.AddString( sModuleName.c_str(), true );
	m_DataSaver.SaveCarbideDataHeader();
	
	// Add header line
	m_DataSaver.AddLineToLast();

	// Print the call stack items
	for( size_t i = 0 ; i < m_vMemoryAddress.size() ; i++ )
	{
		// On log levels 1 & 2 we only print located code lines.
		#ifndef ADDR2LINE
		if( 
			( m_iLogLevel == 1 || m_iLogLevel == 2 )
			&&
			( m_vMemoryAddress.at(i)->GetAddressToLineState() != CATMemoryAddress::EXACT
			&& m_vMemoryAddress.at(i)->GetAddressToLineState() != CATMemoryAddress::FUNCTION )
			)
		{
			// Skips to next
			continue;
		}
		#endif
		#ifdef ADDR2LINE
		if( ( m_iLogLevel == 1 || m_iLogLevel == 2 )
			&& m_vMemoryAddress.at(i)->GetAddressToLineState() != CATMemoryAddress::EXACT )
		{
			// Skips to next
			continue;
		}
		#endif
		else if ( m_vMemoryAddress.at(i)->GetAddressToLineState() == CATMemoryAddress::OUT_OF_PROCESS )
		{
			// Is memory address out of modules range
			string sTemp;
			sTemp.append( m_vMemoryAddress.at(i)->GetAddressString() );
			sTemp.append( " Address out of process memory.");
			m_DataSaver.AddString( sTemp.c_str(), true );
			m_DataSaver.AddLineToLast();
			continue;
		}
		
		// Print memory address
		m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetAddressString().c_str(), true );

		// Space (only for console output)
		m_DataSaver.AddString( " " );

		m_DataSaver.AddCarbideData( 
			NumberToHexString( m_vMemoryAddress.at(i)->GetOffSetFromModuleStart() ) );

		// Module name
		m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetModuleName().c_str(), true );

		// Print call stack memory address details depending on state of memory address
		switch( m_vMemoryAddress.at(i)->GetAddressToLineState() )
		{
			// Address outside of known processes
		case CATMemoryAddress::OUT_OF_PROCESS:
			m_DataSaver.AddLineToLast();
			break;
			// Address located outside of known modules symbols
		case CATMemoryAddress::OUT_OF_RANGE:
			m_DataSaver.AddString( " " );
			m_DataSaver.AddString( "???", true );
			m_DataSaver.AddLineToLast();
			break;
		// Symbol state is currently used when using rom symbol file.
		// From it we get module name & function name.
		case CATMemoryAddress::SYMBOL:
			m_DataSaver.AddString( " " );
			m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetFunctionName().c_str(), true );
			if ( ! m_vMemoryAddress.at( i )->GetFileName().empty() )
			{
				m_DataSaver.AddString( " (" );
				m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetFileName().c_str(), true );
				m_DataSaver.AddString( ")" );
			}
			m_DataSaver.AddLineToLast();
			break;
		// Lst & Map implementation
		#ifndef ADDR2LINE
		case CATMemoryAddress::FUNCTION:
		case CATMemoryAddress::EXACT:
			m_DataSaver.AddString( " " );
			m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetFunctionName().c_str(), true );
			// Small difference displaying details depending on build urel/udeb
			if ( m_eBuildType == CATProject::UREL )
			{
				// UREL
				// Set build info to data saver
				m_DataSaver.SetBuild( false );
				// urel = functionname: linenumber (filename)
				m_DataSaver.AddString( ": " );
				if (  m_vMemoryAddress.at(i)->GetFunctionLineNumber() != -1 )
					m_DataSaver.AddInteger( m_vMemoryAddress.at(i)->GetFunctionLineNumber(), true );
				else if (  m_vMemoryAddress.at(i)->GetExactLineNumber() != -1 )
					m_DataSaver.AddInteger( m_vMemoryAddress.at(i)->GetExactLineNumber(), true );
				m_DataSaver.AddString( " (" );
				m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetFileName().c_str(), true );
				m_DataSaver.AddString( ")" );
				m_DataSaver.AddLineToLast();
			}
			else
			{
				// UDEB
				// udeb = functionname: (filename:linenumber)
				m_DataSaver.AddString( " (" );
				m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetFileName().c_str(), true );
				m_DataSaver.AddString( ":" );
				if(  m_vMemoryAddress.at(i)->GetExactLineNumber() != -1 )
					m_DataSaver.AddInteger( m_vMemoryAddress.at(i)->GetExactLineNumber(), true );
				else
					m_DataSaver.AddString( "???", true );
				m_DataSaver.AddString( ")" );
				m_DataSaver.AddLineToLast();
			}
			break;
		#endif
		// addr2line implementation (new).
		#ifdef ADDR2LINE
		case CATMemoryAddress::FUNCTION:
			m_DataSaver.AddString( " " );
			m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetFunctionName().c_str(), true );
			m_DataSaver.AddString( " (" );
			m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetFileName().c_str(), true );
			m_DataSaver.AddString( ":" );
			if(  m_vMemoryAddress.at(i)->GetExactLineNumber() != -1 )
				m_DataSaver.AddInteger( m_vMemoryAddress.at(i)->GetFunctionLineNumber(), true );
			else
				m_DataSaver.AddString( "???", true );
			m_DataSaver.AddString( ")" );
			m_DataSaver.AddLineToLast();
			break;
		case CATMemoryAddress::EXACT:
			m_DataSaver.AddString( " " );
			m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetFunctionName().c_str(), true );
			m_DataSaver.AddString( " (" );
			m_DataSaver.AddString( m_vMemoryAddress.at(i)->GetFileName().c_str(), true );
			m_DataSaver.AddString( ":" );
			if(  m_vMemoryAddress.at(i)->GetExactLineNumber() != -1 )
				m_DataSaver.AddInteger( m_vMemoryAddress.at(i)->GetExactLineNumber(), true );
			else
				m_DataSaver.AddString( "???", true );
			m_DataSaver.AddString( ")" );
			m_DataSaver.AddLineToLast();
			break;
		#endif
		} // End switch
		// On logging level 1 we only print one located code line
		#ifndef ADDR2LINE
		if ( m_iLogLevel == 1 && ( m_vMemoryAddress.at(i)->GetAddressToLineState() == CATMemoryAddress::EXACT ||
			m_vMemoryAddress.at(i)->GetAddressToLineState() == CATMemoryAddress::FUNCTION ) )
			break;
		#endif
		#ifdef ADDR2LINE
		if ( m_iLogLevel == 1 && m_vMemoryAddress.at(i)->GetAddressToLineState() == CATMemoryAddress::EXACT )
			break;
		#endif
	} // End call stack items loop
}

// -----------------------------------------------------------------------------
// CATDatParser::SetInputFile
// -----------------------------------------------------------------------------
void CATDatParser::SetInputFile(const string& sInputFile)
{
	LOG_FUNC_ENTRY("CATDatParser::SetInputFile");
	m_sInputFile = sInputFile;
}

// -----------------------------------------------------------------------------
// CATDatParser::SetOutputFile
// -----------------------------------------------------------------------------
void CATDatParser::SetOutputFile(const string& sOutpuFile)
{
	LOG_FUNC_ENTRY("CATDatParser::SetOutputFile");
	m_sOutputFile = sOutpuFile;
}

// -----------------------------------------------------------------------------
// CATDatParser::SetRomSymbolFiles
// -----------------------------------------------------------------------------
void CATDatParser::SetRomSymbolFiles(const vector<string>& vRomSymbolFiles)
{
	LOG_FUNC_ENTRY("CATDatParser::SetRomSymbolFiles");
	m_vRomSymbolFiles = vRomSymbolFiles;
}

// -----------------------------------------------------------------------------
// CATDatParser::SetLogLevel
// -----------------------------------------------------------------------------
void CATDatParser::SetLogLevel(int iLogLevel)
{
	LOG_FUNC_ENTRY("CATDatParser::SetLogLevel");
	m_iLogLevel = iLogLevel;
}

// -----------------------------------------------------------------------------
// CATDatParser::GetLogLevel
// -----------------------------------------------------------------------------
int CATDatParser::GetLogLevel() const
{
	LOG_LOW_FUNC_ENTRY("CATDatParser::GetLogLevel");
	return m_iLogLevel;
}

// -----------------------------------------------------------------------------
// CATDatParser::CleanMemoryAddresses
// -----------------------------------------------------------------------------
void CATDatParser::CleanMemoryAddresses()
{
	LOG_LOW_FUNC_ENTRY("CATDatParser::CleanMemoryAddresses");
	// Cleanup memory addressses.
	for( vector<CATMemoryAddress*>::iterator it = m_vMemoryAddress.begin(); it != m_vMemoryAddress.end(); it++ )
	{
		delete *it;
	}
	m_vMemoryAddress.clear();
}

// -----------------------------------------------------------------------------
// CATDatParser::ConvertTimeToLocalTime
// -----------------------------------------------------------------------------
string CATDatParser::ConvertTimeToLocalTime( string sInputTime )
{
	LOG_LOW_FUNC_ENTRY("CATDatParser::ConvertTimeToLocalTime");
	//Is process end abnormal?
	if( sInputTime.compare( LABEL_ABNORMAL ) == 0 )
	{
		return string( AT_ANALYZE_ABNORMAL_EXIT );
	}
	else
	// Check that input time is at least 32-bit
	if( sInputTime.length() <= 8 )
	{
		sInputTime.clear();
		return sInputTime;
	}

	string sTemp = sInputTime;
	const char* pTemp = sTemp.c_str();

	// Are all characters hex
	for( unsigned int i = 0 ; i < sTemp.size() ; i++ )
	{
		if( !IsHexCharacter( (pTemp + i) ) )
		{
			return sInputTime;
		}
	}
	
	// Get LSB bits
	string sLsb;
	sLsb.append( sInputTime.substr( sInputTime.length()-8, sInputTime.length() ) );
	unsigned int iLsbTime = (unsigned int)_httoi( sLsb.c_str() );

	// Get MSB bits
    string sMsb;
	sMsb.append( sInputTime.substr( 0, sInputTime.length()-8 ) );
	unsigned int iMsbTime = (unsigned int)_httoi( sMsb.c_str() );

	// Get time in microsecods
	long long sdf = iMsbTime * 0x100000000 + iLsbTime;

	// Get original time (starting at year 1970 )
	long long llOrigTime = sdf;

	// Get seconds
	sdf = ( sdf )/1000000;
		
	// Check that sdf contains some time value
	if( sdf <= 0)
	{
		// Error in time calculation
		// Return empty string
		sInputTime.clear();
		return sInputTime;
	}

	// Original time after year 1970 in seconds
	long long llOrignTimeInSeconds = sdf;
	
	// Calculate new time which does not include millisecods
	long long llDiffTime = (llOrignTimeInSeconds * 1000000);

	// Calculate time difference in milliseconds
	int llDiffTimeInMilliSecods = (int)( llOrigTime - llDiffTime )/1000;
	
	// Convert difference time to char
	char cDiffInMilliSeconds[20];
    _itoa( llDiffTimeInMilliSecods, cDiffInMilliSeconds, 10 );

	// Time info structure
	struct tm *timeinfo;

	// Get local time
	timeinfo = localtime ( (time_t*) &sdf );

	// Create string and append memory leak time to it
	string sTime;
	sTime.append( asctime( timeinfo ) );

	// Remove last char of locale time string which is \n
	sTime.resize( (int)sTime.length()-1 );
	
	// Get last space index
	int iLastSpace = (int)sTime.find_last_of(" ");

	// If last space index is valid
	if( iLastSpace <= (int)sTime.length() && iLastSpace > 0)
	{
		string sTempTime;
		// Append first part of original time string
		sTempTime.append( sTime.substr( 0, iLastSpace ) );
		
		// Append millisecods
		sTempTime.append( "." );
		sTempTime.append( cDiffInMilliSeconds );

		// Append the rest of the original time string part
		sTempTime.append( sTime.substr( iLastSpace, sTime.length()));

		// Clear original and append new time string which includes millisecods
		sTime.clear();
		sTime.append( sTempTime );
	}

	// Return memory leak time
	return sTime.c_str();
}

bool CATDatParser::CreateWinscwModule( const string& sBinaryName )
{
	LOG_FUNC_ENTRY( "CATDatParser::CreateWinscwModule" );
	// Is module already created?
	for( vector<CATModule2*>::iterator it = m_pModules->begin(); it != m_pModules->end(); it++ )
	{
		if ( _stricmp( sBinaryName.c_str(), (*it)->GetBinaryName().c_str() ) == 0 )
			return true;
	}
	// No create new one and set its values.
	CATModule2* mod = new CATModule2();
	mod->SetTarget( RemovePathAndExt( sBinaryName, true ) );
	mod->SetRequestedTargetExt( GetExtension( sBinaryName ) );
	mod->SetReleasePath( string( "\\epoc32\\release" ) );
	if ( m_eBuildType == CATProject::UDEB )
		mod->SetFullVariantPath( string( "winscw\\udeb" ) );
	else
		mod->SetFullVariantPath( string( "winscw\\urel" ) );
	mod->SetVariantPlatform( string( "winscw" ) );
	m_pModules->push_back( mod );
	return true;
}

// -----------------------------------------------------------------------------
// CATDatParser::FindModuleUsingAddress
// Function finds module using given address.
// -----------------------------------------------------------------------------
int CATDatParser::FindModuleUsingAddress( unsigned long iAddress ) const
{
	LOG_LOW_FUNC_ENTRY("CATDatParser::FindModuleUsingAddress");
	int iRet = -1;
	for( unsigned int i = 0 ; i < m_vDllLoadModList.size() ; i++ )
	{
		// Is address between start and end address?
		if( iAddress > m_vDllLoadModList[i].iStartAddress && iAddress < m_vDllLoadModList[i].iEndAddress )
		{
			iRet = i;
			break;
		}	
	}
	return iRet;
}

// -----------------------------------------------------------------------------
// CATDatParser::FindModuleUsingPID
// Function finds module using module id.
// -----------------------------------------------------------------------------
/*
int CATDatParser::FindModuleUsingPID( unsigned long iPID ) const
{
	LOG_FUNC_ENTRY("CATDatParser::FindModuleUsingPID");

	int iRet = -1;

	// Change module name characters to lowercase
	for( unsigned int i = 0 ; i < m_vDllLoadModList.size() ; i++ )
	{
		if( m_vDllLoadModList[i].iPID == iPID )
		{
			iRet = i;
			break;
		}	
	}
	return iRet;
}
*/
// -----------------------------------------------------------------------------
// CATDatParser::FindModuleUsingName
// Function finds module using module name.
// -----------------------------------------------------------------------------
int CATDatParser::FindModuleUsingName( const char* pModName )
{
	LOG_LOW_FUNC_ENTRY("CATDatParser::FindModuleUsingName");

	// Mod name empty?
	if( pModName == NULL || *pModName == 0 )
		return -1;

	int iRet = -1;
	string sModName( pModName );
	// Change module name characters to lowercase
	ChangeToLower( sModName );
	// Remove variant marks (dots)
	RemoveAllAfterDotIfTwoDots( sModName);
	for( unsigned int i = 0 ; i < m_vDllLoadModList.size() ; i++ )
	{
		string sTemp( m_vDllLoadModList[i].sModuleName );
		ChangeToLower( sTemp );
		// Remove variant marks (dots)
		RemoveAllAfterDotIfTwoDots( sTemp );
		if( sTemp.find( sModName ) != string::npos )
		{
			iRet = i;
			break;
		}	
	}
	return iRet;
}

// -----------------------------------------------------------------------------
// CATDatParser::SetPrintFlag
// -----------------------------------------------------------------------------
void CATDatParser::SetPringFlag( bool bPrintFlag )
{
	LOG_FUNC_ENTRY("CATDatParser::SetPringFlag");
	m_DataSaver.SetPrintFlag( bPrintFlag );
}
// -----------------------------------------------------------------------------
// CATDatParser::SetOffSet
// -----------------------------------------------------------------------------
void CATDatParser::SetOffSet( int iOffSet )
{
	LOG_FUNC_ENTRY("CATDatParser::SetOffSet");
	m_iOffSet = iOffSet;
}

// -----------------------------------------------------------------------------
// CATDatParser::GetOffSet
// -----------------------------------------------------------------------------
int CATDatParser::GetOffSet( ) const
{
	LOG_LOW_FUNC_ENTRY("CATDatParser::GetOffSet");
	return m_iOffSet;
}

// -----------------------------------------------------------------------------
// Set project platform.
// -----------------------------------------------------------------------------
void CATDatParser::SetProjectPlatform( const string& sPlatform )
{
	LOG_FUNC_ENTRY("CATDatParser::SetProjectPlatform");
	m_sProjectPlatform = sPlatform;

	// Check that platform not empty before determing platform from it.
	if ( sPlatform.empty() )
		return;

	// Set functions offset in mapfiles correct (depending on platform).
	if ( _stricmp( sPlatform.c_str(), "armv5" ) == 0 )
	{
		m_iOffSet = FUNCTIONS_OFFSET_IN_MAP_FILE_ARMV5;
	}
	else if ( _stricmp( sPlatform.c_str(), "winscw" ) == 0 )
	{
		m_iOffSet = FUNCTIONS_OFFSET_IN_MAP_FILE_WINSCW;
	}
	else if ( _stricmp( sPlatform.c_str(), "gcce" ) == 0 )
	{
		m_iOffSet = FUNCTIONS_OFFSET_IN_GCCE;
	}
	else
	{
		LOG_STRING( AT_MSG << "Error, cannot set function's offset in map file, invalid platform: " << sPlatform );
	}
}

// -----------------------------------------------------------------------------
// Set projects build type. Use enumeration defined in CATProject.
// -----------------------------------------------------------------------------
void CATDatParser::SetProjectBuildType( int eBuildType )
{
	LOG_FUNC_ENTRY("CATDatParser::SetProjectBuildType");
	m_eProjectBuildType = eBuildType;
}

//EOF
