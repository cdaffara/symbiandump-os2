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
* Description:  Defines CATDatParser class and its used enumerations.
*
*/


#ifndef __CATDatParser_H__
#define __CATDatParser_H__

// Includes.
#include "ATCommonDefines.h"
#include "CATBase.h"
#include "CATDataSaver.h"

// Forward declarations.
class CATModule2;
class CATMemoryAddress;
class CATRomSymbol;

/**
* Provides the analyze feature for AnalyzeTool specific data file.
* Is used by CATProject. Uses CATModule2 and/or CATRomSymbol to locate symbols or
* codelines of memory addressses. Uses CATDataSaver to create report.
*/
class CATDatParser : public CATBase
{

public:
	/**
	* enumeration representing the "mode" depending on build system
	*/
	enum ANALYZE_MODE
	{
		SBS_1 = 0,
		SBS_2 = 1
	};
	/**
	* Representing current process state
	*/
	enum PROCESS_STATE
	{
		not_started = 0,
		ongoing = 1,
		stopped = 2
	};

	/**
	* Constructor.
	*/
	CATDatParser();

#ifndef MODULE_TEST
private:
#endif
	/**
	* Real constructor.
	*/
	void Construct();
	
	/**
	* Prevent copy
	*/
	CATDatParser& operator =( const CATDatParser& /*other*/ ) { }
	CATDatParser( const CATDatParser& /*other*/ ) { }
	
public:
	/**
	* Constructor for SBS2
	* @param pModules pointer to vector containing project modules
	*/
	CATDatParser( vector<CATModule2*>* pModules );

	/**
	* Destructor
	*/
	~CATDatParser();

	/**
	* Set offset to be used with mapfiles
	* @param iOffSet
	*/
	void SetOffSet( int iOffSet );

	/**
	* Get offset value
	* @return int
	*/
	int GetOffSet( ) const;
	
	/**
	* Analyze data file set
	* This method includes try/catch in parsing
	* @return int error codes specified in CATProject (0=OK)
	*/
	int Analyze();

	/**
	* Set log level
	* @param iLogLevel
	*/
	void SetLogLevel(int iLogLevel);
	/**
	* Get Log level
	* @return int
	*/
	int GetLogLevel() const ;

	/**
	* Set data file to be analyzed
	* @param sInputFile
	*/
	void SetInputFile(const string& sInputFile);

	/**
	* Set output file
	* @param sOutputFile
	*/
	void SetOutputFile(const string& sOutputFile);

	/**
	* Set rom symbol file(s).
	* @param sRomSymbolFile
	*/
	void SetRomSymbolFiles(const vector<string>& vRomSymbolFile);

	/**
	* Set print flag
	* @param pPringFlag
	*/
	void SetPringFlag( bool bPrintFlag );

	/**
	* Set addr2line.exe pinpoint state
	* @param bInput
	*/
	void SetAddr2lineExeState( bool bInput );

	/**
	* Set project platform.
	* @param sPlatform platform.
	*/
	void SetProjectPlatform( const string& sPlatform );

	/**
	* Set projects build type. Use enumeration defined in CATProject.
	* @param eBuildType.
	*/
	void SetProjectBuildType( int eBuildType );

#ifndef MODULE_TEST
private:
#endif

	/**
	* Start parsing datafile
	* @return error codes defined in CATProject
	*/
	int Parse();
	
	/**
	* Helper functio to print header of report
	*/
	void Header();

	/**
	* Helper functio to print footer of report
	*/
	void Footer();

	/**
	* Helper function reseting/clearing all
	* member variables related to parsing
	*/
	void ClearParsingVariables();

	// Helpers for data file tags
	bool ParseProcessStart( string& sLine );
	bool ParseDllLoad( string& sLine );
	bool ParseDllUnload( string& sLine );
	bool ParseMemLeak( string& sLine );
	bool ParseProcessEnd( string& sLine );
	bool ParseHandleLeak( string& sLine );
	bool ParseTestStart( string& sLine );
	bool ParseTestEnd( string& sLine );
	bool ParseLoggingCancelled( string& sLine );
	bool ParseErrorOccured( string& sLine );

	void PrintMemLeak(const string& sTime,
					   const string& sLeakSize,
					   const string& sLeakAddr,
					   const string& sModuleName);

	// Utilities
	string ConvertTimeToLocalTime( string sInputTime );

	/**
	* Create a winscw module into pModules vector.
	* Used when we have dll load of module not in project to create them
	* after this they are used in locating code lines.
	* @param sBinaryName name of the binary with extension.
	* @return true if successful.
	*/
	bool CreateWinscwModule( const string& sBinaryName );

#ifndef MODULE_TEST
private:
#endif

	void CleanMemoryAddresses();

	// Members
	unsigned int m_iDataVersion; // Version of data file.
	int m_eProcess_state; // is process started, etc..
	bool m_bProcessStartFound;
	bool m_bDllLoadFound;
	int m_iLogLevel; // specified logging level
	string m_sInputFile; // input file
	vector<string> m_vRomSymbolFiles; // Rom symbol file.
	string m_sOutputFile; // output file
	string m_sInputFileTemp; // temporary input file (parsed from trace)
	ifstream m_In; // Handle to input file
	// Datasaver
	CATDataSaver m_DataSaver;

	// Modules to be used in pinpointing
	vector<CATModule2*>* m_pModules;

	// Rom symbol file.
	CATRomSymbol* m_pRomSymbol;

	// Build type from dat.
	int m_eBuildType;
	// Build type from project.
	int m_eProjectBuildType;
	// Offset of map file.
	int m_iOffSet;
	// Platform of project.
	string m_sProjectPlatform;

	// Process related
	unsigned long m_iCurrentProcessId;
	string m_sCurrentProcessName;
	int m_iTotalRuns;
	int m_iSuccesfullRuns;
	
	// Leak counting related
	int m_iTotalNumberOfLeaks;
	bool m_bSubtestOnGoing;
	int m_iPinPointedLeaks;
	int m_iPinPointedSubTestLeaks;
	int m_iLeakNumber;
	
	// Handle count related
	int m_iSubtestStartHandleCount;
	vector<string> m_vHandleLeaks;

	// Module related
	vector<DLL_LOAD_INFO> m_vDllLoadModList;
	vector<DLL_LOAD_INFO> m_vDllLoadModListSubTest;
	
	// Sbs 1 support functions
	int FindModuleUsingAddress( unsigned long iAddress ) const;
	//int FindModuleUsingPID( unsigned long iPID ) const;
	int FindModuleUsingName( const char* pModName );

	// Memory addresses
	vector<CATMemoryAddress*> m_vMemoryAddress;

};
#endif
//EOF
