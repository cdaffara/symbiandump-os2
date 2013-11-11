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
* Description:  Class representing a module in project (sbs2).
*
*/


#ifndef __CATMODULE2_H__
#define __CATMODULE2_H__

// Includes.
#include "ATCommonDefines.h"
#include "CATBase.h"
#include "CATMmp.h"

// Forward declarations.
class CATMemoryAddress;
class IAddressToLine;

/**
* CATModule2 represents a module/component (single binary) in project.
* CATProject contains a collection of these.
*/
class CATModule2 : public CATBase
{

public:
	/**
	* Constructor.
	*/
	CATModule2(void);
	
	/**
	* Destructor.
	*/
	~CATModule2(void);
	
	/**
	* Read modules attributes from SBS v.1 makefile.
	* Also copies makefile to temporary directory if successful.
	*/
	bool ReadMakeFile();

	/**
	* Read modules attributes from SBS v.1 makefile.
	* From make file which is in modules temporary directory.
	*/
	bool ReadMakeFileFromTemp();

	/**
	* Add sources using line where source files separated by spaces.
	* @param sSourceLine.
	*/
	void AddSources(string& sSourceLine);

	/**
	* Add sources by giving source file and its corresponding listing file.
	* @param sSourceFile
	* @param sLstFile
	*/
	void AddSource(const string& sSourceFile, const string& sLstFile);

	/**
	* Create AT temporary cpp file for module.
	* @param sS60FileName.
	* @param sS60FileName.
	* @param eLoggingMode.
	* @param eBuildType.
	* @param iAllocCallStackSize.
	* @param iFreeCallStackSize.
	* @return true if successful.
	*/
	bool CreateTempCpp(const string& sS60FileName
		, const string& sS60FilePath
		, int eLoggingMode
		, int eBuildType
		, int iAllocCallStackSize
		, int iFreeCallStackSize );

	/**
	* returns default data file name without extension in format processName.targetType
	* @return string with default file name
	*/
	string GetDefaultFileName();

	/**
	* Add AT changes to modules mmp file.
	* @return true if successful.
	*/
	bool ModifyMmp();

	/**
	* Restore any changes made to modules mmp file.
	* @return true if successful.
	*/
	bool RestoreMmp();

	/**
	* Verify tha mmp does not contain AnalyzeTool made changes.
	* If it does contain them will remove them manually or using backup.
	* @return true if successful.
	*/
	bool VerifyAndRecoverMmp();

	/**
	* Copy modules releasables to its temporary dir.
	* This includes map & lst files.
	* @return true if successful.
	*/
	bool CopyReleasables();

	/**
	* Copy modules listing files to given directory.
	* After copy they will be deleted.
	* @param sDir target directory.
	* @return true if successful.
	*/
	bool CopyLstFilesToDir( const string& sDir );

	/**
	* Delete modules lst files from their source
	* directories.
	* @return true if succesfful.
	*/
	bool DeleteLstFilesFromSrc();

	/**
	* Clean modules temporary directory of files,
	* not defined in UNDELETE list.
	* @return true if successful
	*/
	bool CleanTemporaryDir();

	/**
	* Delete modules temporary directory.
	* @return true if successful.
	*/
	bool DeleteTemporaryDir();

	/**
	* Locate codeline of given memory address.
	* Given address must be calculated to correspond map file addresses.
	* @param pMemoryAddress object where to store results.
	* @return true if successful.
	*/
	bool AddressToLine(CATMemoryAddress* pMemoryAddress);

	/**
	* Check does modules symbol file(s) exist.
	* @return true if it exists.
	*/
	bool SymbolFileExist( void );

	/**
	* Check does modules map file(s) exists.
	* @return true if it exists.
	*/
	bool MapFileExist( void );

	/**
	* Check does modules binary file(s) exist.
	* @return true if it exists.
	*/
	bool BinaryFileExist( void );

#ifndef MODULE_TEST
private:
#endif

	/**
	* Read modules attributes from make file.
	*/
	bool ReadMakeFilePrivate();

	/**
	* Locate codeline of given memory address (winscw platform).
	* Given address must be calculated to correspond map file addresses.
	* @param pMemoryAddress object where to store results.
	* @return true if successful.
	*/
	bool AddressToLineWinscw(CATMemoryAddress* pMemoryAddress );

	/**
	* Locate codeline of given memory address (armv5 platform).
	* Given address must be calculated to correspond map file addresses.
	* @param pMemoryAddress object where to store results.
	* @return true if successful.
	*/
	bool AddressToLineArmv5(CATMemoryAddress* pMemoryAddress );

	/**
	* Locate codeline of given memory address.
	* Given address must be calculated to correspond map file addresses.
	* @param pMemoryAddress object where to store results.
	* @return true if successful.
	*/
	bool AddressToLineAddr2lineExe( CATMemoryAddress* pMemoryAddress );

	/**
	* Search map file data (symbols) using given address.
	* @param iAddress memory address.
	* @return index of the symbol or -1 if not found.
	*/
	int GetSymbolIndexUsingAddress(unsigned long iAddress) const;

	/**
	* Search listing  files data using the given symbol name.
	* @param sSymbolName symbols name to find.
	* @return index of the found lst data or -1 if not found.
	*/
	int GetLineInFileIndexUsingSymbolName(const string& sSymbolName) const;
	
	/**
	* Search the exact code line from given file.
	* @param sFileName listing file to search from.
	* @param iLine functions line number.
	* @param iFromFuncAddress how many bytes from functions start.
	* @return line number or -1 if not found.
	*/
	int FindLeakCodeLine( string& sFileName, int iLine, unsigned long iFromFuncAddress ) const;

public:

	/**
	* Set used build system (CATProject::BUILD_SYSTEM).
	* @param eBuildSystem.
	*/
	void SetBuildSystem( int eBuildSystem );

	/**
	* Get defined build system.
	* @return int CATProject::BUILD_SYSTEM
	*/
	int GetBuildSystem( void ) const ;

	/**
	* Set SBS v.1 makefile.
	* @param sMakeFile
	*/
	void SetMakeFile( const string& sMakeFile );
	
	/**
	* Get SBS v.1 makefile.
	* @return string makefile.
	*/
	string GetMakeFile( void ) const;

	/**
	* Initialize module ready for locating code lines.
	* with memory addresses (reads map and listing files).
	* return true if successful.
	*/
	bool InitializeAddressToLine();
	
	/**
	* Set modules mmp file.
	* This also creates temporary dir if it is missing.
	* @return true if successful.
	*/
	bool SetMmpFile(const string& sMmpFile);

	/**
	* Create modules own temporary directory.
	* @return true if successful.
	*/
	bool CreateTemporaryDirectory();

	/**
	* Get modules mmp file.
	* @return mmp file.
	*/
	string GetMmpFile() const;

	/**
	* Get modules atool_temp directory path.
	* @return atool_temp directory path.
	*/
	string GetTempPath() const;

	/**
	* Get S60 logging file name.
	* @return s60 logging file name.
	*/
	string GetS60FileName() const;

	/**
	* Set S60 logging file name.
	* @param aFileName s60 logging file name.
	*/
	void SetS60FileName( const string& aFileName );

	/**
	* Set target binary name.
	* @param sTarget binary file name.
	*/
	void SetTarget(const string& sTarget);

	/**
	* Get target binary name.
	* @return modules binary file name.
	*/
	string GetTarget() const;

	/**
	* Set binary target type.
	* @param sTargetType binary target type.
	*/
	void SetTargetType(const string& sTargetType);

	/**
	* Get binary target type.
	* @return binary target type.
	*/
	string GetTargetType() const;

	/**
	* Set requested binary target file extension.
	* @param sRequestedTargetExt binary target file extension.
	*/
	void SetRequestedTargetExt( const string& sRequestedTargetExt );

	/**
	* Get requested binary target file extension.
	* @return binary target file extension.
	*/
	string GetRequestedTargetExt() const;

	/**
	* Get full name of modules binary.
	* @return modules binary files full name.
	*/
	string GetBinaryName() const;

	/**
	* Set variant platform.
	* @param sVariantPlatform variant platform.
	*/
	void SetVariantPlatform(const string& sVariantPlatform);

	/**
	* Get variant platform.
	* @return variant platform.
	*/
	string GetVariantPlatform() const;

	/**
	* Set variant type.
	* @param sVariantType variant type.
	*/
	void SetVariantType(const string& sVariantType);

	/**
	* Get variant type.
	* @return variant type.
	*/
	string GetVariantType() const;

	/**
	* Set feature variant.
	* @param sFeatureVariant feature variant.
	*/
	void SetFeatureVariant(const string& sFeatureVariant);

	/**
	* Get feature variant.
	* @return feature variant.
	*/
	string GetFeatureVariant() const;

	/**
	* Set feature variant name.
	* @param sFeatureVariantName feature variant name.
	*/
	void SetFeatureVariantName(const string& sFeatureVariantName);

	/**
	* Get feature variant name.
	* @return feature variant name.
	*/
	string GetFeatureVariantName() const;

	/**
	* Set release path.
	* @param sReleasePath release path.
	*/
	void SetReleasePath(const string& sReleasePath);

	/**
	* Get release path.
	* @return release path.
	*/
	string GetReleasePath() const;

	/**
	* Set full variant path.
	* @param sFullVariantPath full variant path.
	*/
	void SetFullVariantPath(const string& sFullVariantPath);

	/**
	* Get full variant path.
	* @return full variant path.
	*/
	string GetFullVariantPath() const;

	/**
	* Set compile definition(s) of the module.
	* @param sCompileDefinitions
	*/
	void SetCompileDefinitions( const string& sCompileDefinitions );

	/**
	* Get compile definition(s) of the module.
	* @return compile definition(s).
	*/
	string GetCompileDefinitions() const;

	/**
	* Is modules target type udeb?
	* @return true if modules target type is udeb.
	*/
	bool IsUDEB() const;

	/**
	* Is module build successfully?
	* This includes check of listing and map files from temporary directory.
	* @return true if module build successfully.
	*/
	bool IsMakeSuccessfull();

	/**
	* Get error string. This includes possible erros what are generated when
	* IsMakeSuccesfull is called.
	* @return error string.
	*/
	string GetErrors() const;

	/**
	* Create build complete file to modules temporary directory.
	* @return true if build complete file created successfully.
	*/
	bool CreateBuildCompleteFile( void );

	/**
	* Read map file (armv5 platform).
	* @return true if map file read successfully.
	*/
	bool ReadMapFileArmv5();

	/**
	* Set compile info text
	* @param sCompileInfoText
	*/
	void SetCompileInfoText( string sCompileInfoText );

	/**
	* Get compile info text
	* @return compile info text
	*/
	string GetCompileInfoText() const;

#ifndef MODULE_TEST
private:
#endif

	/**
	* Struct for saving source information
	* sCpp is source file.
	* sLst sources corresponding lst file.
	* bStatic true if source information from static library.
	*/
	struct SOURCE
	{
		bool bStatic;
		string sCpp;
		string sLst;
		SOURCE() : bStatic(false), sCpp(""), sLst("") {}
	};

	// Build system
	int m_eBuildSystem;
	// Sbs v.1 makefile
	string m_sMakeFile;
	// Mmp file.
	CATMmp m_Mmp;
	// Temporary directory with path.
	string m_sTempPath;
	// Target.
	string m_sTarget;
	// Target type.
	string m_sTargetType;
	// True target extension.
	string m_sRequestedTargetExt;
	// Variant platform.
	string m_sVariantPlatform;
	// Variant type.
	string m_sVariantType;
	// Feature variant.
	string m_sFeatureVariant;
	// Feature variant name.
	string m_sFeatureVariantName;
	// Release path (releasables).
	string m_sReleasePath;
	// Full variant path (path to releasables).
	string m_sFullVariantPath;
	// String to store information of compile
	string m_sCompileInfoText;
	// String to store possible error messages.
	string m_sErrors;
	// Compile definition(s)
	string m_sCompileDefinitions;
	// Source files.
	vector<SOURCE> m_vSources;
	// Listing data.
	vector<LINE_IN_FILE> m_vLineInFile;
	// Modules map data (symbols).
	vector<MAP_FUNC_INFO> m_vMapFileFuncList;
	// File logging mode filename.
	string m_sS60FileName;
	// true if file name not empty and %processname% string replaced
	bool m_bS60FileNameResolved;
	// File logging mode file path.
	string m_sS60FilePath;
	// Is all data loaded for address to code line functions.
	bool m_bAddressToLineInitialized;
	// Read listing files (armv5 platform).
	bool ReadListingFilesArmv5();
	// Get listing file name of given source file.
    string GetLstNameOfSource(string sSource) const;
	// Copy listing files to temporary directory.
	bool CopyLstFilesToTemp();
	// Copy map file to temporary directory.
	bool CopyMapFileToTemp();
	// Get full map file name.
	string GetMapFile() const;
	// Get full symbol file name.
	string GetSymbolFile() const;
	// Get full binary file name.
	string GetBinaryFile() const;
	// Helper function to check is platform armv5.
	bool IsPlatformArmv5() const;
	// Get "unique" id of module
	string GetUniqueId() const;
	// Sbs v.1 variant urel label.
	string m_sFeatureVariantURELLabel;
	// Sbs v.1 variant udeb label.
	string m_sFeatureVariantUDEBLabel;
	// Object used to get codelines of memory addresses.
	IAddressToLine* m_pAddressToLine;
};
#endif
//EOF
