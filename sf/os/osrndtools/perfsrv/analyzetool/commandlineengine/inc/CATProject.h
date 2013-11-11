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


#ifndef __CATPROJECT_H__
#define __CATPROJECT_H__

#include "ATCommonDefines.h"
#include "CATBase.h"
#include "../inc/CATParseBinaryFile.h"

class CATModule2;
class CATDatParser;

// Raptor cmd and makefile constants used
#define RAPTOR_CMD_BASE "sbs -b bld.inf  --makefile=atool_temp/build/make_build --config="
#define RAPTOR_MAKEFILE "atool_temp\\build\\make_build.default"
#define RAPTOR_MAKEFILE_LOG " --logfile=atool_temp\\build\\makefile.xml"
#define RAPTOR_BUILD_LOG " --logfile=atool_temp\\build\\build.xml"
#define RAPTOR_REALLYCLEAN_LOG " --logfile=atool_temp\\build\\reallyclean.xml"
#define RAPTOR_BUILD_LOG_NAME "atool_temp\\build\\build.xml"
#define RAPTOR_LISTING_LOG " --logfile=atool_temp\\build\\listing.xml"

#define RAPTOR_NOBUILD_SWITCH " --nobuild"
#define RAPTOR_MAKEFILE_SWITCH "--makefile=atool_temp/build/make_build"


// Raptor makefile constant variable names
#define RAPTOR_PROJECT_META "PROJECT_META:="
#define RAPTOR_SOURCE "SOURCE:="
#define RAPTOR_TARGET "TARGET:="
#define RAPTOR_TARGETYPE "TARGETTYPE:="
#define RAPTOR_REQUESTEDTARGETEXT "REQUESTEDTARGETEXT:="
#define RAPTOR_VARIANTPLATFORM "VARIANTPLATFORM:="
#define RAPTOR_VARIANTTYPE "VARIANTTYPE:="
#define RAPTOR_FEATUREVARIANT "FEATUREVARIANT:="
#define RAPTOR_FEATUREVARIANTNAME "FEATUREVARIANTNAME:="
#define RAPTOR_RELEASEPATH "RELEASEPATH:="
#define RAPTOR_FULLVARIANTPATH "FULLVARIANTPATH:="
#define RAPTOR_COMPILE_DEFINITIONS "CDEFS:="

// Sbs v.1 makefile constants.
#define MAKEFILE_SEARCH_STRING "@perl -S checksource.pl"
#define MMPFILE_SEARCH_STRING "PRJ_MMPFILES MMP"
#define MMPTESTFILE_SEARCH_STRING "PRJ_TESTMMPFILES MMP"
#define SOURCE_PATH "SOURCEPATH"
#define SOURCE_STRING "SOURCE"
#define TARGET_STRING "TARGET"

/**
* CATProject represents a project (collection of modules).
* It has to be set mode in which it operates like
* compile / analyze and necessary parameters for that.
* Then call Run method which also returs error codes
* defined in AT_ERROR_CODES enumeration.
* @author
*/
class CATProject : public CATBase
{
public:
	
	/**
	* Enumeration representing used build systems
	*/
	enum BUILD_SYSTEM {
		SBS_V1 = 1,
		SBS_V2 = 2
	};

	/**
	* Enumeration representing the mode project is run.
	*/
	enum PROJECT_MODE {
		COMPILE = 0,
		INSTRUMENT = 1,
		UNINSTRUMENT = 2,
		UNINSTRUMENT_FAILED =3,
		FAILED = 4,
		CLEAN = 5,
		ANALYZE = 6,
		INSTRUMENT_CONSOLE = 7,
		UNINSTRUMENT_CONSOLE = 8,
		FAILED_CONSOLE = 9,
		NOT_DEFINED = 10
	};

	/**
	* Enumeration repserenting the logging mode.
	*/
	enum LOGGING_MODE {
		DEFAULT = 0,
		FILE = 1,
		TRACE = 2,
		TRACE_FAST = 3
	};

	/**
	* Enumeration representing build type.
	*/
	enum BUILD_TYPE {
		UREL = 0,
		UDEB = 1
	};

public:

	/**
	* Constructor
	*/
	CATProject();

	/**
	* Destructor
	*/
	~CATProject();

	CATParseBinaryFile Parser;

	/**
	* Set arguments.
	* @param arguments.
	* @return true if arguments ok.
	*/
	bool SetArguments( ARGUMENTS& arguments );

	/**
	* Set build system to be used with project.
	* See BUILD_SYSTEM enumeration for available systems.
	* @param eSystem system.
	*/
	void SetBuildSystem( BUILD_SYSTEM eSystem );

	/**
	* Set mode which to run.
	* See PROJECT_MODE enumeration for available modes.
	* @param eMode mode.
	*/
	void SetMode(PROJECT_MODE eMode);

	/**
	* Get project mode.
	* @return mode of project.
	*/
	int GetMode() const;

	/**
	* Set epocroot.
	* @param sEpocroot
	*/
	void SetEpocRoot( const string& sEpocRoot );

	/**
	* Set project platform.
	* @param sPlatform platform.
	*/
	void SetPlatform( const string& sPlatform );

	/**
	* Set variant.
	* @sVariant variant.
	*/
	void SetVariant( const string& sVariant );

	/**
	* Set logging mode. See LOGGING_MODE enumeration for available modes.
	* @param eLogginMode logging mode.
	*/
	void SetLoggingMode( LOGGING_MODE eLoggingMode);

	/**
	* Set build type. See BUILD_TYPE enumeration for available types.
	* @param eType build type.
	*/
	void SetBuildType( BUILD_TYPE eType );
	
	/**
	* Set S60 logging filename.
	* @param sFileName filename.
	*/
	void SetS60FileName( const string& sFileName);

	/**
	* Set S60 logging file path.
	* @param sFilePath filename.
	*/
	void SetS60FilePath( const string& sFilePath );

	/**
	* Set target module.
	* @param sTargetModule target module name.
	*/
	void SetTargetModule( const string& sTargetModule );

	/**
	* Set multiple target modules used in project.
	* @param vTargetModules target module names.
	*/
	void SetTargetModules( const vector<string>& vTargetModules );

	/**
	* Set Binary target name to create analysis results to.
	* @param sBinaryTarget binary target name.
	*/
	void SetBinaryTarget( const string& sBinaryTarget );

	/**
	* Set data file to analyze.
	* @param sDataFile datafile.
	*/
	void SetDataFile( const string& sDataFile );

	/**
	* Set rom symbol file(s) used in analyze.
	* @param sRomSymbolFile.
	*/
	void SetRomSymbolFiles(const vector<string>& vRomSymbolFiles);

	/**
	* Set the logging level of analysis report.
	* @param iLogLevel log level.
	*/
	void SetLogLevel( int iLogLevel );

	/**
	* Set the size of allocation call stack to be written in temporary cpp.
	* @param iAllocCallStackSize
	*/
	void SetAllocCallStackSize( int iAllocCallStackSize );

	/**
	* Set the size of free call stack to be written in temporary cpp.
	* @param iFreeCallStackSize
	*/
	void SetFreeCallStackSize( int iFreeCallStackSize );

	/**
	* Set the output file name to store analyse report.
	* @param sDataFileOutput data file name.
	*/
	void SetDataFileOutput( const string& sDataFileOutput );

	/**
	* Set build command used in compile phase.
	* @param sBuildCommand build command.
	*/
	void SetBuildCommand( const string& sBuildCommand );

	/**
	* Run the set mode.
	* @return error code.
	*/
	int Run();

	/**
	* Run recovery, used when "instatly" need to recover modules and exit.
	*/
	int RunRecoveryAndExit();

	/**
	* Get build type string.
	* @param eType type from which to get correponding string.
	* @return types corresponding string.
	*/
	static string GetBuildTypeString( int eType );

	/**
	* Reads projects configuration file if it exists.
	* @return false in case the data contains information that project is uninstrumented. Otherwise returns always true.
	*/
	bool IsUninstrumented();

#ifndef MODULE_TEST
private:
#endif
	/**
	* Run compiling in console.
	* @return error code.
	*/
	int RunCompile( void );

	/**
	* Run cleaning project.
	* @return error code.
	*/
	int RunClean( void );

	/**
	* Run running analyze.
	* @return error code.
	*/
	int RunAnalyze( void );

	/**
    * Run instrumenting of project for Carbide extension (pre-build).
	* @return error code.
	*/
	int RunInstrument( void );

	/**
	* Run uninstrumenting of project for Carbide extension (post-build).
	* @return error code.
	*/
	int RunUninstrument( void );

	/**
	* Run uninstrumenting of project for Carbide extension(post-build), when build failed.
	* @return error code.
	*/
	int RunUninstrumentFailed( void );

	/**
	* Run console instrumenting.
	* @return error code.
	*/
	int RunInstrumentConsole( void );

	/**
	* Run Console uninstrumenting.
	* @return error code.
	*/
	int RunUninstrumentConsole( void );

	/**
	* Run console uninstrumenting, when build failed.
	* @return error code.
	*/
	int RunFailedConsole( void );

	/**
	* Show summary of compilation.
	*/
	void DisplayCompileSummary( void );

	/**
	* Show summary, build target, build type, logging mode...
	*/
	void DisplayBuildSummary( void );

	/**
	* Create make files.
	* @return true if successful
	*/
	bool CreateMakeFile( void );
	/**
	* Create SBS v.1 make files.
	* @return true if successful.
	*/
	bool CreateMakeFileSbs1( void );
	/**
	* Copy sbs1 makefile to temporary folder.
	* @return true if successful.
	*/
	bool CopyMakeFileSbs1ToTemporaryFolder( void );
	/**
	* Run really clean in SBS v.1.
	* @return true if successful.
	*/
	bool RunReallyCleanSbs1( void );
	/**
	* Run really clean in SBS v.2.
	* @return true if successful.
	*/
	bool RunReallyCleanSbs2( void );
	/**
	* Run export in SBS v.1.
	* @return true if successful.
	*/
	bool RunExportSbs1( void );
	/**
	* Create make files (level2) SBS v.1.
	* @return true if successful.
	*/
	bool CreateMakeFileSbs1Level2( void );
	/**
	* Create makefile SBS v.2.
	* @return true if successful.
	*/
	bool CreateMakeFileSbs2( void );
	/**
	* Read makefile.
	* @return true if successful.
	*/
	bool ReadMakeFile( void );
	/**
	* Read SBS v.1 makefile.
	* @return true if successful.
	*/
	bool ReadMakeFileSbs1( void );
	/**
	* Read SBS v.1 makefile (Level1).
	* @return true if successful.
	*/
	bool ReadMakeFileSbs1Level1( void );
	/**
	* Read SBS v.2 makefiles.
	* @return true if successful.
	*/
	bool ReadMakeFileSbs2( void );
	/**
	* Read specified SBS v.2 makefile.
	* @param sMakeFile makefile to be read.
	* @return true if succesful.
	*/
	bool ReadMakeFileSbs2( string& sMakeFile );

	/**
	* Filter unsupported and static libraries to their own vectors.
	* Unsupported include kernel types, modules with kernel mode compile definition.
	* @return true if successful.
	*/
	bool FilterModules();

	/**
	* Creates temporary directories for all modules
	* in member vector.
	* @return true if successful.
	*/
	bool CreateTemporaryDirectories();

	/**
	* Creates tempororary cpp files for all modules
	* in member vector.
	* @return true if successful.
	*/
	bool CreateTemporaryCpps();

	/**
	* Hooks all modules in member vector
	* (modifies mmp files)
	* @return true if successful
	*/
	bool ModifyModules( void );

	/**
	* Unhooks all modules in member vector
	* (removes changes from mmp files)
	* @return true if successful
	*/
	bool RestoreModules( void );

	/**
	* Verifys that member vectors modules
	* mmp files unchanged. Restores if they are
	* from backup or from original.
	* @return true if successful
	*/
	bool VerifyAndRecoverModules( void );

	/**
	* Runs compilation.
	* @return true if successful.
	*/
	bool Compile( void );
	/**
	* @return true if successful.
	*/
	bool CompileSbs1( void );
	/**
	* @return true if successful.
	*/
	bool CompileSbs2( void );

	/**
	* Runs listing creatation.
	* @return true if successful
	*/
	bool CreateListings( void );
	/**
	* @return true if successful.
	*/
	bool CreateListingsSbs1( void );
	/**
	* @return true if successful.
	*/
	bool CreateListingsSbs2( void );
	/**
	* Copies releasables of modules including lst & map files
	* for all modules in project to their temporary directories.
	* @return true if successful.
	*/
	bool CopyReleasables( void );
	/**
	* Deletes all atool_temp directories
	* of member vector modules.
	* @return true if successful.
	*/
	bool DeleteTemporaryDirs( void );
	/**
	* Deletes files from atool_temp directory
	* of member vector modules which extension not defined
	* in TEMP_EXTENSION_NO_DELETE table.
	* @return true if successful.
	*/
	bool CleanTemporaryDirs( void );
	/**
	* Writes class attributes to a file
	* under atool_temp directory.
	* @return true if successful.
	*/
	bool WriteAttributes( void ) const;
	/**
	* Read class attributes from a file
	* under atool_temp directory.
	* @return true if successful.
	*/
	bool ReadAttributes( void );

	/**
	* Creates atool_temp directory if it does not
	* exist in current directory.
	* @return true if successful.
	*/
	bool MakeTempDirIfNotExist( void );

#ifndef MODULE_TEST
private:
#endif

	/**
	* Clean all module vectors.
	*/
	void CleanModuleVectors();

	/**
	* Get build type as string.
	* @return buildtype string.
	*/
	string GetBuildTypeString();

	/**
	* Helper function to add target module(s)
	* if any defined in class to given sbs command.
	* @param sCmd command to add modules to.
	*/
	void AddTargetModuleIfDefined(string& sCmd);

	/**
	* Run given system/abld command to all defined target modules in vector.
	* Space char (if its missing) will be added to given command + one target
	* module at a time.
	* @param sCommand sCommand to run.
	* @return true if none system call sets error level other than 0.
	*/
	bool RunAbldCommandToAllTargets( const string& sCommand );

	/**
	* Check is target module in project.
	* If no modules / targets defined return value is true.
	* @return true if target module is in project.
	*/
	bool IsTargetModuleInProject( void ) const;

	/**
	* Initializes member make file variable with correct full path to point (epoc32/build)...
	* @return true if successful.
	*/
	bool InitSbs1MakeFileWithPath();

	/**
	* Initializes member make file variable with correct full path to (atoo_temp...)
	* @return true if successful.
	*/
	bool InitSbs1MakeFileWithPathToTemp();

#ifndef MODULE_TEST
private:
#endif
	// System current directory.
	char m_cCurrentDir[ MAX_LINE_LENGTH ];
	// Projects build system
	int m_eBuildSystem;
	// Project modules.
	vector<CATModule2*> m_vModules;
	// Static libraries.
	vector<CATModule2*> m_vStaticLibraries;
	// Unsupported project modules.
	vector<CATModule2*> m_vUnsupportedModules;
	// Run mode.
	int m_eMode;
	// Logging level (used in analyse).
	int m_iLoggingLevel;
	// Is project unsinstrumented.
	bool m_bUninstrumented;
	// Is build just for test modules
	bool m_bAbldTest;
	// Build command.
	string m_sBuildCommand;
	// Epocroot
	string m_sEpocRoot;
	// Platform i.e. armv5.
	string m_sPlatform;
	// SBS2 variant.
	string m_sVariant;
	// Logging mode (used in compile,instrumenting).
	int m_eLoggingMode;
	// Allocation call stack size
	int m_iAllocCallStackSize;
	// Free call stack size
	int m_iFreeCallStackSize;
	// Build type udeb / urel.
	int m_eBuildType;
	// Sbs v.1 level 1 make file
	string m_sMakeFile;
	// User given S60 log file name.
	string m_sS60FileName;
	// User given S60 log file path.
	string m_sS60FilePath;
	// Target module.
	string m_sTargetModule;
	// Target modules (used in carbide instrumenting).
	vector<string> m_vTargetModules;
	// Binary target (used in analyse).
	string m_sBinaryTarget;
	// User given datafile to analyse.
	string m_sDataFile;
	//
	vector<string> m_vRomSymbolFiles;
	// Temporary data file name if user gave trace file.
	string m_sDataFileTemp;
	// Temporary data file name if user gave binary log file.
	string m_sBinaryLogFileTemp;
	// User given output file to store analyse results.
	string m_sDataFileOutput;
	// Analyser object.
	CATDatParser* m_pAnalyzer;
};
#endif
// End of file
