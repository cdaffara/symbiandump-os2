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
* Description:  Defines CATBase "utility" class.
*
*/

#ifndef __CATBASE_H__
#define __CATBASE_H__

#include "../inc/ATCommonDefines.h"

/**
* This class implements lot of utility type of functions used all around atool project.
* All functions are static so they can be used without inheritance of this class. But still
* this is a base class of almost all others.
*/
class CATBase
{
public:
	
	/**
	* Constructor
	*/
	CATBase();

	/**
	* Destructor
	*/
	virtual ~CATBase(void);

public:
	
	/**
	* FilterExtraSpaces
	* Filters/replaces multiple continuous spaces with single. Won't leave
	* spaces in start or end of string.
	* @param sString to filter.
	* @return void.
	*/
	static void FilterExtraSpaces( string& sString );

	/**
	* Convert hex value in string to signed decimal.
	* @param sHex
	* @param iDec
	* @return true if successful
	*/
	static bool hexToDec( string& sHex, int& iDec );

	/**
	* Convert hex value in string to unsigned decimal
	* @param sHex
	* @param iDec
	* @return true if successful
	*/
	static bool hexToDec( string& sHex, unsigned int& iDec );

	/**
	* Convert hex value in string to unsigned long.
	* @param sHex
	* @param ulDec
	* @return true if successful
	*/
	static bool hexToDec( string& sHex, unsigned long& ulDec );

	/**
	* Convert hex value in string to unsigned long long.
	* @param sHex
	* @param ullDec
	* @return true if successful
	*/
	static bool hexToDec( string& sHex, unsigned long long& ullDec );

	/**
	* Convert hex value to integer
	* @param value
	* @return unsigned long
	*/
	static unsigned long _httoi(const TCHAR *value);

	/**
	* Convert integer to hex string.
	* @param i
	* @return hex string
	*/
	static string NumberToHexString( unsigned int i );

	/**
	* Convert long to hex string.
	* @param i
	* @return hex string
	*/
	static string NumberToHexString( unsigned long i );

	/**
	* Helper function checks is given character hex.
	* @param value value to check.
	* @return true if value is hex char.
	*/
	static bool IsHexCharacter(const TCHAR *value);

	/**
	* Parse string to vector of strings using
	* separator. (Tokenizer with delimeter).
	* @param sInput string to split
	* @char separator
	* return vector<string>
	*/
	static vector<string> ParseStringToVector( const string& sInput, char separator );

	/**
	* Remove spaces and tabulatures from beginning and
	* end of given string.
	* @param sInput String to trim.
	*/
	static void TrimString( string& sInput );

	/**
	* Searches files with given extension from path.
	* @param pPathAndExt path with extension
	* @return string filename
	*/
	static string GetFileNameUsingExt( const char* pPathAndExt );

	/**
	* Changes all BackSlash characters to Slash character from string.
	* @param sInput String including backslashes.
	* @return String without backslashes.
	*/
	static string ChangeSlashToBackSlash( string sInput );

	/**
	* Changes given string to uppercase
	* @param sInput
	*/
	static void ChangeToUpper( string& sInput );

	/**
	* Converts any uppercase letter to lowercase.
	*
	* @param sInput Reference to string.
	*/
	static void ChangeToLower( string& sInput );

	/**
	* Filter string out of unwanted characters. The list of allowed
	* characters is defined in CFILTERSTRING.
	* @param sString string to filter.
	* @return filtered string.
	*/
	static string FilterString( const string& sString );

	/**
	* Removes path and extension from given filename string
	* @param sFileName
	* @param bReverseFindExt if true extension will be looked starting from end
	* @return string
	*/
	static string RemovePathAndExt( string sFileName, bool bReverseFindExt = false );

	/**
	* Returns time  stamp in microseconds parsed from start of trace message
	* @param sLineStart
	* @return int
	*/
	unsigned __int64 ParseTimeStamp( string sLineStart );

	/**
	* Check if given file exists.
	* @param pFilename Pointer to file name.
	* @return False If file does not exists.
	*/
	static bool FileExists( const char* pFilename );

	/**
	* Check if given file is flagged read only.
	* @param pFileName pointer to file name
	* @return true if read only flag set.
	*/
	static bool IsFileReadOnly( const char* pFileName );

	/**
	* Set file read only.
	* @param pFileName Pointer to file name
	* @return true if successful.
	*/
	static bool SetFileReadOnly( const char* pFileName );

	/**
	* Set file writable (remove read only flag).
	*
	* @param pFilename Pointer to file name.
	* @return true if successful.
	*/
	static bool SetFileWritable( const char* pFileName );

	/**
	* Copy file to given path
	* @param sFile
	* @param sToPath
	* @return true if successful
	*/
	static bool FileCopyToPath(const string& sFile, const string& sToPath);

	/**
	* Move file to given path
	* @param sFile File to be moved
	* @param sToPath path where to move file
	* @return true if successful
	*/
	static bool FileMoveToPath(const string& sFile, const string& sToPath);

	/**
	* Delete file
	* Note! if file does not exists no error message is displayed
	* but function returns false
	* @param sFile File to be deleted
	* @param bPrint display messages or not, default true
	* @return true if successful
	*/
	static bool FileDelete(const string& sFile, bool bPrint = true );
    
	/**
	* Delete dir
	* Note! if dir does not exists no error message is displayed
	* but function returns false.
	* This function wont delete directory if string does not contain
	* AT_TEMP...
	* @param sDir Directory to be deleted
	* @param bPrint display message or not, default true
	* @return true if successful
	*/
	static bool DirDelete(const string& sDir, bool bPrint = true );

	/**
	* Create dir
	* Note! if dir cannot be created no error message is displayed
	* but function returns false.
	* @param sDir Directory to be deleted
	* @param pPrint display message or not, default true
	* @return true if successful
	*/
	static bool DirCreate(const string& sDir, bool pPrint = true );

	/**
	* Create temp path string for given
	* mmpfile (full path+mmpname)
	* @param sMmpFileWithPath
	* @return string containing full path to
	* AnalyzeTool temporary directory
	*/
	static string CreateTempPath(const string& sMmpFileWithPath);

	/**
	* Search files with extensions from given path.
	* @param pPathAndExt path with extension definition
	* @param bPrintErrors do print errors?
	* @param sErrorLog errors
	* @return true if found.
	*/
	static bool SearchFileWithExtension( const char* pPathAndExt, bool bPrintErrors, string& sErrorLog );

	/**
	* Helper function to parse filename or path from given string
	* @param bFileName if true returns filename otherwise the path
	* @param sInput string where to get path or filename
	* @return string filename or path
	*/
	static string GetPathOrFileName( bool bFileName, string sInput );

	/**
	* Function returns string from begin of given string until next space,
	* characters until next space are removed from sInput string.
	*
	* @param sInput Line where data is separated with spaces.
	* @param bEraseFromInput If true characters before space will be removed.
	* @return string String until next space.
	*/
	static string GetStringUntilNextSpace( string& sInput, bool bEraseFromInput = true );

	/**
	* Function returns string from begin of given string until next atool's main id <AT>,
	* characters until next space are removed from sInput string.
	*
	* @param sInput Line which contains main id.
	* @param bEraseFromInput If true characters before main id will be removed.
	* @return string String until main id.
	*/
	static string GetStringUntilMainId( string& sInput, bool bEraseFromInput = true );

	/**
	* Convert unix path to windows
	* @param sPath
	*/
	static void ConvertUnixPathToWin( string& sPath );

	/**
	* Create Temporary AT Cpp file
	* @param sId unique id to add in file name
	* @param sPath where to create
	* @param sS60FileName of the logging file
	* @param sS60FilePath of the logging file
	* @param iLogOption logging mode
	* @param iIsDebug build type
	* @param iAllocCallStackSize
	* @param iFreeCallStackSize
	* @return true if successful
	*/
	static bool CreateTemporaryCpp( const string& sId
								 ,const string& sPath
								 ,const string& sS60FileName
								 ,const string& sS60FilePath
								 ,int iLogOption
								 ,int iIsDebug
								 ,int iAllocCallStackSize
								 ,int iFreeCallStackSize );
	/**
	* Acquire a list of files in given directory
	* @param sDirectory can end to \ or x but not to *
	* @param bListDirs if true directories will be listed as well, default false
	* @param bAddPathToFile if true given sDirectory path is added to file string, default false
	* @return vector<string> list of files in folder
	*/
	static vector<string> DirList(const string& sDirectory, bool bListDirs = false, bool bAddPathToFile = false);

	/**
	* Get extension from given "file" string
	* returns string after last . if any otherwise returns same
	* what was given
	* @param sString 
	* @return string string after last '.' if no '.' returns given string
	*/
	static string GetExtension(const string& sString);

	/**
	* Convert TCHAR pointer to string
	* @param charArray to convert
	* @return string
	*/
	static string ConvertTCHARtoString(TCHAR* charArray);

	/**
	* if given string contains two dots '.' this will remove
	* all characters after first '.'
	*/
	static void RemoveAllAfterDotIfTwoDots(string& sString);

	/**
	* checks given file is it data file
	* @param sFile
	* @return true if it is datafile
	*/
	static bool IsDataFile( string sFile );

	/**
	* checks given file is it binary log file
	* @param sFile
	* @return true if it is binary logfile
	*/
	static bool IsBinaryLogFile( string sFile );

	/**
	* Parses a path string containing ".." to a valid
	* path without relations. If given string does
	* not contain relations it will not be changed
	* @param sPathString
	* @return void
	*/
	static void ParseRelativePathString(string& sPathString);

	/**
	* Remove relative path ".." from string
	* @param sString string to remove from
	* @param iDots index of ".."
	* @return void
	*/
	static void RemoveRelativePath(string& sString, size_t iDots);

	/**
	* Check if given directory exists.
	*
	* @param pDirname Pointer to directory name.
	* @return False If directory does not exists.
	*/	
	static bool DirectoryExists( const char* pDirname );

	/**
	* Checks from constant array is this targettype
	* unsupported by AT
	* @param sTargetType type to check
	* @return true if it is supported by atool
	*/
	static bool IsTargetTypeSupported(string sTargetType);

	/**
	* Checks from constant array is this targettype
	* kernel side.
	* @param sTargetType type to check
	* @return true if it is kernel type
	*/
	static bool IsTargetTypeKernelSide(string sTargetType);

	/**
	* Check is given variant defined in environment.(SBS v.1)
	* @param sEpocroot
	* @param sVariantName
	* @return true if it is.
	*/
	static bool CheckVariant( const string& sEpocroot, const string& sVariantName );

	/**
	* Check has the environment defined "DEFAULT" variant
	* @param sEpocRoot
	* @return true if it is
	*/
	static  bool IsDefaultVariant( const string& sEpocRoot );

	/**
	* Check is all character ascii
	* @param pInput pointer to characters
	* @param iLength length of the string
	* @return true if all character are ascii
	*/
	static bool IsAscii( const char* pInput, const unsigned int iLength );

	/**
	* Get current environments epocroot.
	* @param sEpocRoot value is stored in this if successful.
	* @return true if successful.
	*/
	static bool GetEpocRoot( string& sEpocRoot );
};
#endif
