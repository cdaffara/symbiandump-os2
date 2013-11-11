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
* Description:  Definitions for the class CATBase.
*
*/


#include "../inc/CATBase.h"
#include "../inc/CATParseXML.h"

// -----------------------------------------------------------------------------
// CATBase::CATBase
// Constructor.
// -----------------------------------------------------------------------------
CATBase::CATBase(void)
{
	LOG_FUNC_ENTRY("CATBase::CATBase");
}

// -----------------------------------------------------------------------------
// CATBase::~CATBase
// Destructor.
// -----------------------------------------------------------------------------
CATBase::~CATBase(void)
{
	LOG_FUNC_ENTRY("CATBase::~CATBase");
}

// -----------------------------------------------------------------------------
// CATBase::ChangeToLower
// Converts any uppercase letter to lowercase.
// -----------------------------------------------------------------------------
void CATBase::ChangeToLower( string& sInput )
{
	LOG_LOW_FUNC_ENTRY("CATBase::ChangeToLower");
	int iLength = (int)sInput.size();
	for( int i = 0 ; i < iLength ; i++ )
	{
		sInput[i] = (char)tolower( sInput[i] );
	}
}

// -----------------------------------------------------------------------------
// CATBase::ChangeToUpper
// Converts any uppercase letter to lowercase.
// -----------------------------------------------------------------------------
void CATBase::ChangeToUpper( string& sInput )
{
	LOG_LOW_FUNC_ENTRY("CATBase::ChangeToUpper");
	int iLength = (int)sInput.size();
	for( int i = 0 ; i < iLength ; i++ )
	{
		sInput[i] = (char)toupper( sInput[i] );
	}
}

// -----------------------------------------------------------------------------
// CATBase::TrimString
// Remove spaces and tabulatures from beginning and
// end of given string.
// -----------------------------------------------------------------------------
void CATBase::TrimString( string& sInput )
{
	LOG_LOW_FUNC_ENTRY("CATBase::TrimString");
	if( sInput.empty() )
		return;
	//Remove spaces and tabulatures from beginning of string
	while( !sInput.empty() && ( sInput[0] == SPACE_CHAR_VALUE || sInput[0] == TAB_CHAR_VALUE ) )
	{
		sInput.erase( 0, 1 );
	}
	//Remove spaces and tabulatures from end of string
	while( !sInput.empty() && ( sInput[sInput.size()-1] == SPACE_CHAR_VALUE || sInput[sInput.size()-1] == TAB_CHAR_VALUE ) )
	{
		sInput.erase( sInput.size()-1, 1 );
	}
}

// -----------------------------------------------------------------------------
// CATBase::SearchFileWithExtension
// Searches files with given extension from path.
// -----------------------------------------------------------------------------
bool CATBase::SearchFileWithExtension( const char* pPathAndExt, bool bPrintErrors, string& sErrorLog )
{
	LOG_FUNC_ENTRY("CATBase::SearchFileWithExtension");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	string sTemp( pPathAndExt );

	//Find file
	hFind = FindFirstFile( sTemp.c_str(), &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE)
	{
		string sErrorString( "No " );
		//Get extension
		string sExt( pPathAndExt );
		sExt.erase( 0, sExt.find_last_of( "." ) );

		sErrorString.append( sExt );
		sErrorString.append( " files in directory: " );

		string sWithoutExt( pPathAndExt );
		sWithoutExt.erase( sWithoutExt.find_last_of( "." )-1, string::npos );
		sErrorString.append( sWithoutExt );

		if( bPrintErrors )
		{
			//string sTemp( pPathAndExt );
			//printf( "Can not find: %s.\n", pPathAndExt );
			printf( sErrorString.c_str() );
		}
		else
		{
			//Add line change if sErrorString not empty
			if( !sErrorLog.empty() )
				sErrorString.insert( 0, "\n" );
			sErrorLog.append( sErrorString );
		}
		return false;
	} 
	else 
	{
		FindClose(hFind);
		return true;
	}
}

// -----------------------------------------------------------------------------
// CATBase::GetPathOrFileName
// Returns path to file or file name.
// -----------------------------------------------------------------------------
string CATBase::GetPathOrFileName( bool bFileName, string sInput )
{
	LOG_LOW_FUNC_ENTRY("CATBase::GetPathOrFileName");
	string sRet;
	size_t iPos = sInput.size();

	sInput = ChangeSlashToBackSlash( sInput );

	//Find character '\' starting from end of string
	while( iPos > 0 && sInput[iPos-1] != '\\' )
	{
		iPos--;
	}
	if( iPos > 0 )
	{
		//Return file name
		if( bFileName )
		{
			sInput.erase( 0, iPos );
			sRet = sInput;
		}
		else //Return file path
		{
			sInput.erase( iPos, string::npos );
			sRet = sInput;
		}
	}
	else
	{
		if( !bFileName )
			return sRet;
		sRet = sInput;
	}
	return sRet;
}

// -----------------------------------------------------------------------------
// CATBase::GetFileNameUsingExt
// Searches files with given extension from path.
// -----------------------------------------------------------------------------
string CATBase::GetFileNameUsingExt( const char* pPathAndExt )
{
	LOG_FUNC_ENTRY("CATBase::GetFileNameUsingExt");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	string sRet;

	//Find file
	hFind = FindFirstFile( pPathAndExt, &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE)
	{
		//if( bPrintErrors )
		printf( "Can not find: %s.\n", pPathAndExt );
		return sRet;
	} 
	else 
	{
		sRet.append( FindFileData.cFileName );
		FindClose(hFind);
		return sRet;
	}
}

// -----------------------------------------------------------------------------
// CATBase::GetStringUntilNextSpace
// Function returns string from begin of given string until next space,
// characters until next space are removed from sInput string.
// -----------------------------------------------------------------------------
string CATBase::GetStringUntilNextSpace( string& sInput, bool bEraseFromInput )
{
	LOG_LOW_FUNC_ENTRY("CATBase::GetStringUntilNextSpace");
	string sTemp( sInput );
	size_t iSize = sTemp.find_first_of(' ');
	if( iSize != string::npos )
	{
		sTemp.resize( iSize );
		if( bEraseFromInput )
			sInput.erase( 0, (iSize+1) );
	}
	else
	{
		if ( bEraseFromInput )
			sInput.clear();
	}
	return sTemp;
}

// -----------------------------------------------------------------------------
// CATBase::GetStringUntilMainId
// Function returns string from begin of given string until next atool's main id <AT>,
// characters until next main id are removed from sInput string.
// -----------------------------------------------------------------------------
string CATBase::GetStringUntilMainId( string& sInput, bool bEraseFromInput )
{
	LOG_LOW_FUNC_ENTRY("CATBase::GetStringUntilMainId");
	string sTemp( sInput );
	size_t iSize = sTemp.find(MAIN_ID);
	if( iSize != string::npos )
	{
		sTemp.resize( iSize );
		if( bEraseFromInput )
			sInput.erase( 0, (iSize) );
	}
	else
	{
		if ( bEraseFromInput )
			sInput.clear();
	}
	return sTemp;
}

// -----------------------------------------------------------------------------
// CATBase::ChangeSlashToBackSlash
// Function changes all BackSlash characters to Slash character from
// given string.
// -----------------------------------------------------------------------------
string CATBase::ChangeSlashToBackSlash( string sInput )
{
	LOG_LOW_FUNC_ENTRY("CATBase::ChangeSlashToBackSlash");
	for( unsigned int i = 0 ; i < sInput.length() ; i++ )
	{
		if( sInput[i] == '/' )
		{
			sInput[i] = '\\';
		}
	}
	return sInput;
}


// -----------------------------------------------------------------------------
// CATBase::ParseTimeStamp
// Function returns time parsed from start of trace message
// -----------------------------------------------------------------------------
unsigned __int64 CATBase::ParseTimeStamp( string sLineStart )
{
	unsigned __int64 iTime(0);

	int iHours(0), iMinutes(0), iSeconds(0), iMiliseconds(0), iMicroseconds(0);
	int iErr(0), iRet(0);

	TrimString( sLineStart );
	string sTimeString = GetStringUntilNextSpace( sLineStart );

	// Get time
	int iPos = sTimeString.find( ":" );
	if( iPos != string::npos ) // ':' found, this is timestamp from fastTrace/traceViewer 
	{
		// possible formats 
		// hh:mm:ss - seconds (ft)
        // hh:mm:ss:mmm - miliseconds (ft/tw)
		// hh:mm:ss:mmmmmm - microseconds (ft/tw)
		// hh:mm:ss:nnnnnnnnn - nanoseconds (ft) - ignore last 3digits

		iRet = sscanf_s( sTimeString.c_str(), "%d:%d:%d.%3d%3d", &iHours, &iMinutes, &iSeconds, &iMiliseconds, &iMicroseconds );
		if( iRet == 5 || iRet == 4 )
		{
			// get microseconds
			iTime = ( ( ( iHours*60 + iMinutes )*60 + iSeconds )*1000 + iMiliseconds )*1000 + iMicroseconds;
		}
		else
		{
			iErr = true;
		}
	}
	else if( sTimeString.find( "." ) != string::npos ) // epoc timestamp in format ssss.mmm
	{
		iRet = sscanf_s( sTimeString.c_str(), "%d.%d", &iSeconds, &iMiliseconds );
		if( iRet == 2 )
		{
			// get microseconds
			iTime = ( ( ( iHours*60 + iMinutes )*60 + iSeconds )*1000 + iMiliseconds )*1000 + iMicroseconds;
		}
		else
		{
			iErr = true;
		}
	}
	else // timestamp in microseconds from binary log file or from ft
	{
		iRet = sscanf_s( sTimeString.c_str(), "%016I64x", &iTime);
		if( iRet == 1 )
		{
		}
		else
		{
			iErr = true;
		}
	}

	if( iErr )
		cout << "Error, can not read timestamp.\n";

	return iTime;
}

// -----------------------------------------------------------------------------
// CATBase::FileExists
// Check if given file exists.
// -----------------------------------------------------------------------------
bool CATBase::FileExists( const char * pFilename )
{
	LOG_FUNC_ENTRY("CATBase::FileExists");
	DWORD dwRet = GetFileAttributes( pFilename );
	if( dwRet == INVALID_FILE_ATTRIBUTES )
	{
		return false;
	}
	else
	{
		//Is file directory?
		if( dwRet & FILE_ATTRIBUTE_DIRECTORY )
		{
			return false;
		}
	}
	return true;
}

bool CATBase::IsFileReadOnly( const char* pFilename )
{
	LOG_FUNC_ENTRY("CATBase::IsFileReadOnly");
	DWORD dwRet = GetFileAttributes( pFilename );
	if( dwRet == INVALID_FILE_ATTRIBUTES )
		return false;
	if( dwRet & FILE_ATTRIBUTE_READONLY )
		return true;
	return false;
}

bool CATBase::SetFileReadOnly( const char* pFileName )
{
	LOG_FUNC_ENTRY("CATBase::SetFileReadOnly");
	DWORD dw = GetFileAttributes( pFileName );
	if( dw == INVALID_FILE_ATTRIBUTES )
		return false;
	if( dw & FILE_ATTRIBUTE_READONLY )
		return true;
	dw = dw | FILE_ATTRIBUTE_READONLY ;
	if ( SetFileAttributes( pFileName, dw ) )
		return true;
	return false;
}
bool CATBase::SetFileWritable( const char* pFileName )
{
	LOG_FUNC_ENTRY("CATBase::SetFileWritable");
	DWORD dw = GetFileAttributes( pFileName );
	if( dw == INVALID_FILE_ATTRIBUTES )
		return false;
	if( ! dw & FILE_ATTRIBUTE_READONLY )
		return true;
	dw = dw ^ FILE_ATTRIBUTE_READONLY ;
	if ( SetFileAttributes( pFileName, dw ) )
		return true;
	return false;
}

// -----------------------------------------------------------------------------
// CATBase::FileCopyToPath
// Copies file to given path
// -----------------------------------------------------------------------------
bool CATBase::FileCopyToPath(const string& sFile, const string& sToPath)
{
	LOG_FUNC_ENTRY("CATBase::FileCopyToPath");
	// Display message
	cout << AT_MSG << "Copy " << sFile << AT_FILE_TO << sToPath << endl;
	if ( sFile.empty() || sToPath.empty() )
	{
		LOG_FUNC_EXIT("CATBase::FileCopyToPath Error, empty parameter");
		return false;
	}
	// Copy using windows api (seems not to work when relavite path ..
	/*
	// Full path where to copy
	string sDestination = sToPath;
	// Append '\' to string if not exists
	if ( sDestination.length() > 1 )
	{
		const char cLastChar = sDestination[ sDestination.length() -1 ];
		if ( cLastChar != DASH )
			sDestination.append("\\");
	}
	int iRet = 0;
	iRet = CopyFile( sFile.c_str(), sDestination.c_str(), false );
	if ( iRet != 0 )
	{
		return false;
	}
	*/
	string sCommand;
	sCommand.append( "copy /Y \"");
	sCommand.append( sFile );
	sCommand.append( "\" \"" );
	sCommand.append( sToPath );
	sCommand.append( "\" > nul 2>&1" );
	LOG_STRING( sCommand );
	int iRet = 0;
	iRet = (int)system( sCommand.c_str() );
	if ( iRet != 0 )
		return false;
	return true;
}

// -----------------------------------------------------------------------------
// CATBase::FileMoveToPath
// Copies file to given path
// -----------------------------------------------------------------------------
bool CATBase::FileMoveToPath(const string& sFile, const string& sToPath)
{
	LOG_FUNC_ENTRY("CATBase::FileMoveToPath");
	// Display message
	cout << AT_MSG << "Move " << sFile << AT_FILE_TO << sToPath << endl;
	if ( sFile.empty() || sToPath.empty() )
	{
		LOG_FUNC_EXIT("CATBase::FileMoveToPath Error, empty parameter");
		return false;
	}
	// Move (again windows api function does not support relative path .. in it
	/*
	// Get filename from sFile
	string sFileName = GetPathOrFileName( true, sFile );
	// Full path where to copy
	string sDestination = sToPath;
	// Append '\' to string if not exists
	if ( sDestination.length() > 1 )
	{
		const char cLastChar = sDestination[ sDestination.length() -1 ];
		if ( cLastChar != DASH )
			sDestination.append("\\");
	}
	int iRet = 0;
	iRet = MoveFile( sFile.c_str(), sDestination.c_str());
	if ( iRet != 0 )
	{
		return false;
	}
	*/
	string sCommand;
	sCommand.append( "move /Y \"");
	sCommand.append( sFile );
	sCommand.append( "\" \"" );
	sCommand.append( sToPath );
	sCommand.append( "\" > nul 2>&1" );
	LOG_STRING( sCommand );
	int iRet = 0;
	iRet = (int)system( sCommand.c_str() );
	if ( iRet != 0 )
		return false;
	return true;
}
// -----------------------------------------------------------------------------
// CATBase::CreateTempPath
// Creates temporary directory path for given mmp file
// -----------------------------------------------------------------------------
string CATBase::CreateTempPath(const string& sMmpFileWithPath)
{
	LOG_FUNC_ENTRY("CATBase::CreateTempPath");
	string sTempPath = GetPathOrFileName( false, sMmpFileWithPath );
	sTempPath.append( AT_TEMP_DIR );
	sTempPath.append( "\\" );
	return sTempPath;
}

// -----------------------------------------------------------------------------
// CATBase::RemovePathAndExt
// Removes extension from file name and returns file name without extension.
// -----------------------------------------------------------------------------
string CATBase::RemovePathAndExt( string sFileName, bool bReverseFindExt)
{
	LOG_LOW_FUNC_ENTRY("CATBase::RemovePathAndExt");
	string sRet;
	sFileName = GetPathOrFileName( true, sFileName );
	if ( bReverseFindExt )
	{
		// Remove extension from reverse
		size_t iPos = sFileName.find_last_of('.');
		if( iPos != string::npos )
		{
			sFileName.resize( sFileName.find_last_of('.') );
			sRet = sFileName;
		}
	}
	else
	{
		// Remove extension finding first .
		size_t iPos = sFileName.find_first_of('.');
		if( iPos != string::npos )
		{
			sFileName.resize( sFileName.find_first_of('.') );
			sRet = sFileName;
		}
	}
	return sRet;
}

// -----------------------------------------------------------------------------
// CATBase::IsTargetTypeSupported
// Checks from constant array is this target unsupported
// -----------------------------------------------------------------------------
bool CATBase::IsTargetTypeSupported(string sTargetType)
{
	LOG_FUNC_ENTRY("CATBase::IsTargetTypeSupported");
	// compare to list
	int iArraySize = sizeof( UNSUPPORTED_TARGET_TYPES ) / sizeof( string );
	for ( int i=0 ; i < iArraySize ; i++ )
	{
		string sUnsupported = UNSUPPORTED_TARGET_TYPES[i];
		// lowercase both
		ChangeToLower(sTargetType);
		ChangeToLower(sUnsupported);
		// compare
		if ( sUnsupported.compare( sTargetType ) == 0 )
		{
			return false;
		}
	}
	return true;
}

// -----------------------------------------------------------------------------
// CATBase::IsTargetTypeKernelSide
// Checks from constant array is this target type kernel side
// -----------------------------------------------------------------------------
bool CATBase::IsTargetTypeKernelSide(string sTargetType)
{
	LOG_FUNC_ENTRY("CATBase::IsTargetTypeKernelSide");
	// compare to list
	int iArraySize = sizeof( KERNEL_SIDE_TARGET_TYPES ) / sizeof( string );
	for ( int i=0 ; i < iArraySize ; i++ )
	{
		string sUnsupported = KERNEL_SIDE_TARGET_TYPES[i];
		// lowercase both
		ChangeToLower(sTargetType);
		ChangeToLower(sUnsupported);
		// compare
		if ( sUnsupported.compare( sTargetType ) == 0 )
		{
			return true;
		}
	}
	return false;
}

bool CATBase::CheckVariant( const string& sEpocRoot, const string& sVariant )
{
	LOG_FUNC_ENTRY("CATBase::CheckVariant");
	string sFileToCheck;
	// Add epoc root
	if( sEpocRoot.size() > 1 )
		sFileToCheck.append( sEpocRoot );
	// Add path
	sFileToCheck.append( VARIANT_DIR ) ;
	// Add variant
	sFileToCheck.append( sVariant );
	// Add extension
	sFileToCheck.append( VARIANT_FILE_EXTENSION );
	// check does FileExists
	return FileExists( sFileToCheck.c_str() );
}
bool CATBase::IsDefaultVariant( const string& sEpocRoot )
{
	LOG_FUNC_ENTRY("CATBase::IsDefaultVariant");
	string sFileToCheck;
	// Add epoc root
	if( sEpocRoot.size() > 1 )
		sFileToCheck.append( sEpocRoot );
	// Add path
	sFileToCheck.append( VARIANT_DIR ) ;
	// Add variant
	sFileToCheck.append( "DEFAULT" );
	// Add extension
	sFileToCheck.append( VARIANT_FILE_EXTENSION );
	// check does FileExists
	return FileExists( sFileToCheck.c_str() );
}

// -----------------------------------------------------------------------------
// CATBase::FileDelete
// FileDelete
// -----------------------------------------------------------------------------
bool CATBase::FileDelete(const string& sFile, bool bPrint )
{
	LOG_FUNC_ENTRY("CATBase::FileDelete");
	// does file even exists
	if ( !FileExists( sFile.c_str() ) )
		return false;
	// delete file
	int iRet = _unlink( sFile.c_str() );
	// if print on display error
	if ( iRet  && bPrint )
	{
		cout << AT_MSG << "Error, deleting file " << sFile
			<< endl;
	}
	// if print on display message
	else if ( !iRet && bPrint )
	{
		cout << AT_MSG << "Delete " << sFile << endl;
	}
	// return
	if ( iRet )
		return false;
	return true;
}
// -----------------------------------------------------------------------------
// CATBase::DirDelete
// Delelete directory
// -----------------------------------------------------------------------------
bool CATBase::DirDelete(const string& sDir, bool bPrint )
{
	LOG_FUNC_ENTRY("CATBase::DirDelete");
	if ( sDir.find( AT_TEMP_DIR) == string::npos )
		return false;
	
	if ( sDir.length() < 2 )
		return false;

	string sDir2;
	if ( sDir.at(1) != ':' )
	{
		char cDir[MAX_LINE_LENGTH];
		GetCurrentDirectory( MAX_LINE_LENGTH , cDir );
		sDir2.append( cDir );
		sDir2.append( "\\" );
		sDir2.append( sDir );
	}
	else
		sDir2.append( sDir );

	// does directory exists
	DWORD dwRet = GetFileAttributes( sDir2.c_str() );
	if ( dwRet == INVALID_FILE_ATTRIBUTES )
		return false;
	else if ( ! (dwRet & FILE_ATTRIBUTE_DIRECTORY) )
	{
		return false;
	}
	// Delete dir
	string sCmd( "rmdir /S /Q " );
	sCmd.append( sDir2 );
	sCmd.append( " > nul 2>&1" );
	int iRet = (int)system( sCmd.c_str() );
	if ( iRet && bPrint)
	{
		cout << AT_MSG << "Error, deleting directory " << sDir2 << endl;
	}
	else if ( !iRet && bPrint )
	{
		cout << AT_MSG << "Delete directory " << sDir2 << endl;
	}
	if ( iRet )
		return false;
	return true;
}

// -----------------------------------------------------------------------------
// CATBase::DirCreate
// Create directory
// -----------------------------------------------------------------------------
bool CATBase::DirCreate(const string& sDir, bool bPrint )
{
	LOG_FUNC_ENTRY("CATBase::DirCreate");

	if ( sDir.length() < 2 )
		return false;

	string sDir2;
	if ( sDir.at(1) != ':' )
	{
		char cDir[MAX_LINE_LENGTH];
		GetCurrentDirectory( MAX_LINE_LENGTH , cDir );
		sDir2.append( cDir );
		sDir2.append( "\\" );
		sDir2.append( sDir );
	}
	else
		sDir2.append( sDir );

	// does directory exists
	DWORD dwRet = GetFileAttributes( sDir2.c_str() );
	if ( dwRet != INVALID_FILE_ATTRIBUTES )
	{
		if( dwRet & FILE_ATTRIBUTE_DIRECTORY )
			return false;
	}
	// Create dir
	string sCmd( "mkdir " );
	sCmd.append( sDir2 );
	sCmd.append( " > nul 2>&1" );
	int iRet = (int)system( sCmd.c_str() );
	if ( iRet && bPrint)
	{
		cout << AT_MSG << "Error, creating directory " << sDir2 << endl;
	}
	else if ( !iRet && bPrint )
	{
		cout << AT_MSG << "Directory " << sDir2 << " created" << endl;
	}
	if ( iRet )
		return false;
	return true;
}

// -----------------------------------------------------------------------------
// CATBase::ConvertTCHARtoString
// Convert TCHAR* to std::string
// -----------------------------------------------------------------------------
string CATBase::ConvertTCHARtoString(TCHAR* charArray)
{
	LOG_LOW_FUNC_ENTRY("CATBase::ConvertTCHARtoString");
	// Loop char array
	stringstream ss;
	int iIndex = 0;
	char c = (char) charArray[iIndex];
	// until null termination
	while ( c != '\0' )
	{
		ss << c;
		iIndex++;
		c = (char) charArray[iIndex];
	}
	// return string
	return ss.str();
}

// -----------------------------------------------------------------------------
// CATBase::ConvertTCHARtoString
// Get list of files in directory
// -----------------------------------------------------------------------------
vector<string> CATBase::DirList(const string& sDirectory
								, bool bListDirs, bool bAddPathToFile)
{
	LOG_FUNC_ENTRY("CATBase::DirList");
	// Create string to modify it
	string sDir = sDirectory;
	// Add if missing '\' & '*' to the sDirectory
	if ( sDir.at( sDir.size()-1 ) != '\\' )
		sDir.append( "\\" );
	// Path to add to file string if specified
	string sPath = sDir;
	// Add * to for windows api to find all files
	sDir.append( "*" );
	// convert directory string to LPCSTR
	LPCSTR dir( sDir.c_str() );
	// vector to store file list
	vector<string> vFileList;
	// Using win32 api to find list of files in directory
	// file data "container"
	WIN32_FIND_DATA fileData;
	// handle to directory
	HANDLE hFinder = FindFirstFile( dir, &fileData );
	if ( hFinder == INVALID_HANDLE_VALUE )
	{
		// no files found
		return vFileList;
	}
	// loop files add to vector and return
	while( FindNextFile(hFinder, &fileData ) )
	{
		DWORD dw = fileData.dwFileAttributes;
		// skip if its directory and bListDirs not specified
		if ( dw & FILE_ATTRIBUTE_DIRECTORY && ! bListDirs)
			continue;
		// add files to vector
		string sFile = ConvertTCHARtoString( fileData.cFileName );
		// Add given path to file string if specified
		if ( bAddPathToFile )
			sFile.insert( 0, sPath );
		vFileList.push_back( sFile );
	}
	// Close file find handler
	FindClose( hFinder );
	return vFileList;
}

// -----------------------------------------------------------------------------
// CATBase::ParseRelativePathToString
// ParseRelative
// -----------------------------------------------------------------------------
void CATBase::ParseRelativePathString(string& sPathString)
{
	LOG_LOW_FUNC_ENTRY("CATBase::ParseRelativePathString");
	string sParsed;
	// find ..
	size_t iDots = sPathString.find( ".." );
	while ( iDots != string::npos )
	{
		RemoveRelativePath( sPathString, iDots );
		iDots = sPathString.find( ".." );
	}
}

// -----------------------------------------------------------------------------
// CATBase::RemoveRelativePath
// Remove relative path from string (using given index)
// -----------------------------------------------------------------------------
void CATBase::RemoveRelativePath(string& sString, size_t iDots)
{
	LOG_LOW_FUNC_ENTRY("CATBase::RemoveRelativePath");
	// Chck if accidentally given wrong parameter
	if ( iDots == string::npos 
		|| iDots < 1 )
		return;
	// Parsed string
	string sParsed;
	// Find position of last backslash before dots
	size_t i = sString.rfind("\\", iDots-2 );
	// Pickup start part (depending is the backslash at last parts first char)
	if ( sString.at(iDots+2) != '\\' )
		sParsed = sString.substr( 0, i+1 ) ;
	else
		sParsed = sString.substr( 0, i );
	// Pick up last part
	sParsed.append( sString.substr( iDots+2, sString.size() ) );
	sString = sParsed;
}

// -----------------------------------------------------------------------------
// Get extension from given string
// -----------------------------------------------------------------------------
string CATBase::GetExtension(const string& sString)
{
	LOG_LOW_FUNC_ENTRY("CATBase::GetExtension");
	// find last .
	size_t iDot = sString.find_last_of( "." );
	// return string after . if found
	if ( iDot != string::npos )
		return sString.substr(iDot+1, sString.length()-(iDot+1) );
	// otherwise return given string
	return sString;
}

// -----------------------------------------------------------------------------
// CATBase::DirectoryExists
// Check if given directory exists.
// -----------------------------------------------------------------------------
bool CATBase::DirectoryExists( const char* pDirname )
{
	LOG_FUNC_ENTRY("CATBase::DirectoryExists");
	size_t iLenght = strlen( pDirname );
	
	if ( iLenght < 2 )
		return false;

	string sDir;
	if ( pDirname[1] != ':' )
	{
		char cDir[MAX_LINE_LENGTH];
		GetCurrentDirectory( MAX_LINE_LENGTH , cDir );
		sDir.append( cDir );
		sDir.append( "\\" );
		sDir.append( pDirname );
	}
	else
		sDir.append( pDirname );

	DWORD dwRet = GetFileAttributes( sDir.c_str() );
	if( dwRet == INVALID_FILE_ATTRIBUTES )
	{
		return false;
	}
	else
	{
		//Is file directory?
		if( dwRet & FILE_ATTRIBUTE_DIRECTORY )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

// -----------------------------------------------------------------------------
// CATBase::ConvertUnixPathToWin
// -----------------------------------------------------------------------------
void CATBase::ConvertUnixPathToWin( string& sPath )
{
	LOG_LOW_FUNC_ENTRY("CATBase::ConvertUnixPathToWin");
	size_t iSpot = 0;
	// convert '/' to '\'
	iSpot = sPath.find( "/" );
	while( iSpot != string::npos )
	{
		sPath.replace(iSpot,1, "\\");
		iSpot = sPath.find( "/", iSpot+1 );
	}
	// convert '\\' to '\'
	iSpot = sPath.find( "\\\\" );
	while( iSpot != string::npos )
	{
		sPath.replace(iSpot,2,"\\");
		iSpot = sPath.find( "\\\\" );
	}
}

// -----------------------------------------------------------------------------
// CATBase::RemoveAllAfterDotIfTwoDots
// Removes all after first '.'
// if given string contains 2 '.' or more
// -----------------------------------------------------------------------------
void CATBase::RemoveAllAfterDotIfTwoDots(string& sModName)
{
	LOG_LOW_FUNC_ENTRY("CATBase::RemoveAllAfterDotIfTwoDots");
	// did we find variable?
	size_t found;
	// Find first '.'
	found = sModName.find(".");
	if ( found != string::npos )
	{
		// Try find second '.'
		found = sModName.find(".", found+1);
		if ( found != string::npos )
		{
			// Remove all after first '.'
			sModName = sModName.substr(0, sModName.find(".")+1 );
		}
	}
}
// -----------------------------------------------------------------------------
// CATBase::CreateTemporaryCpp
// -----------------------------------------------------------------------------
bool CATBase::CreateTemporaryCpp( const string& sId,
								 const string& sPath
								 ,const string& sS60FileName
								 ,const string& sS60FilePath
								 ,int iLogOption
								 ,int iIsDebug
								 ,int iAllocCallStackSize
								 ,int iFreeCallStackSize )
{
	LOG_FUNC_ENTRY("CATBase::CreateTemporaryCpp");
	// Add slash to path if missing
	string sTempCpp = sPath;
	if( sTempCpp.at( sTempCpp.length() - 1 ) != '\\' )
		sTempCpp.append("\\");

	// append temporary cpp name with id in middle
	sTempCpp.append( AT_TEMP_CPP_LOWER_START );
	sTempCpp.append( sId );
	sTempCpp.append( AT_TEMP_CPP_LOWER_END );

	//Open and truncate temporary cpp
	ofstream out( sTempCpp.c_str() , ios::trunc );
	if ( ! out.good() )
	{
		out.close();
		return false;
	}
	// Headers
	out << "#include <e32base.h>";
	// Is debug
	out << "\nconst TInt ATTempDebug(" << iIsDebug << ");";
	// Log option
	out << "\nconst TInt ATTempLogOption(" << iLogOption << ");";
	// Alloc call stack
	out << "\nconst TInt ATTempAllocCallStackSize(" << iAllocCallStackSize << ");";
	// Free call stack
	out << "\nconst TInt ATTempFreeCallStackSize(" << iFreeCallStackSize << ");";
	// Log file name
	out << "\n_LIT( ATTempLogFileName, \"" << sS60FileName << "\" );";
	// Log file path
	out << "\n_LIT( ATTempLogFilePath, \"" << sS60FilePath << "\" );";
	// Version number
	out << "\n_LIT( ATTempVersion, \"" << ATOOL_COMPATIBILITY_STRING << "\" );";
	// Variable functions use enumeration values that are defined in memoryhook (customuser.h)
	// We use constants here so that we don't need to include the header file, wich
	// might cause problems.
/* Enumeration copied to comment for notes
        enum TATOptions
            {
            ELogFileName = 1,   
            EVersion = 2 ,
            ELogOption = 3,
            EDebug = 4,
            EAllocCallStackSize = 5,
            EFreeCallStackSize = 6,
			ELogFilePath = 7
            };
*/
	out << "\nTInt GetInt( const TUint8 aType )";
	out << "\n{";
	out << "\nswitch( aType )";
	out << "\n{";
	out << "\ncase 4: return ATTempDebug; ";
	out << "\ncase 3: return ATTempLogOption;";
	out << "\ncase 5: return ATTempAllocCallStackSize;";
	out << "\ncase 6: return ATTempFreeCallStackSize;";
	out << "\ndefault: return KErrArgument;";
	out << "\n}";
	out << "\n}";
	out << "\nTPtrC GetString( const TUint8 aType )";
	out << "\n{";
	out << "\nswitch( aType )";
	out << "\n{";
	out << "\ncase 1: return ATTempLogFileName();";
	out << "\ncase 2: return ATTempVersion();";
	out << "\ncase 7: return ATTempLogFilePath();";
	out << "\ndefault: return KNullDesC();";
	out << "\n}";
	out << "\n}";

	/** Todo: Old way of separate functions, these here for backup support and to ease testing. */
	/** Unnessesary in the future, so can be removed then (1.8.2). */

	out << "\n_LIT( KFileName, \"";
	out << sS60FileName;
	out << "\" );\n";

	out << "\n_LIT( KFilePath, \"";
	out << sS60FilePath;
	out << "\" );\n";

	// Hardcoded version number for support.
	out << "\n/* The AnalyzeTool version number used. */";
	out << "\n_LIT( KAtoolVersion, \"1.7.6;1.10.1\" );\n";

	out << "\nconst TFileName LogFileName()";
	out << "\n    {";
	out << "\n    return TFileName( KFileName() );";
	out << "\n    }";

	out << "\nconst TPath LogFilePath()";
	out << "\n    {";
	out << "\n    return TPath( KFilePath() );";
	out << "\n    }";

	out << "\nTUint32 AllocCallStackSize()";
	out << "\n    {";
	out << "\n    return TUint32( ";
	out << iAllocCallStackSize;
	out << " );\n";
	out << "\n    }";
	
	out << "\nTUint32 FreeCallStackSize()";
	out << "\n    {";
	out << "\n    return TUint32( ";
	out << iFreeCallStackSize;
	out << " );\n";
	out << "\n    }";

	out << "\nconst TFileName AtoolVersion()";
	out << "\n    {";
	out << "\n    return TFileName( KAtoolVersion() );";
	out << "\n    }";

	out << "\nTUint32 LogOption()";
	out << "\n    {";
	out << "\n    return TUint32( ";
	out << iLogOption;
	out << " );";
	out << "\n    }";
	
	out << "\nTUint32 IsDebug()";
	out << "\n    {";
	out << "\n    return TUint32( ";
	out << iIsDebug;
	out << " );";
	out << "\n    }";

	// End of file and close
	out << "\n\n// End of File\n";
	out.close();
	cout << AT_MSG << "Created " << sTempCpp << endl;
	return true;
}

// -----------------------------------------------------------------------------
// CATBase::IsDataFile
// -----------------------------------------------------------------------------
bool CATBase::IsDataFile( string sFile )
{
	LOG_FUNC_ENTRY("CATBase::IsDataFile");
	// Check that sFile not empty
	if ( sFile.empty() || sFile.length() < 1 )
		return false;

	// Temporary line char array.
	char cLineFromFile[MAX_LINE_LENGTH];
	//Open file
	ifstream in( sFile.c_str() );

	//File open ok?
	if( !in.good() )
		return false;

	//Read all lines
	in.getline( cLineFromFile, MAX_LINE_LENGTH );

	string sLineFromFile( cLineFromFile );
	in.close();
	if( sLineFromFile.find( "DATA_FILE_VERSION" ) != string::npos )
		return true;
	else
		return false;
}

// -----------------------------------------------------------------------------
// CATBase::IsBinaryLogFile
// -----------------------------------------------------------------------------
bool CATBase::IsBinaryLogFile( string sFile )
{
	LOG_FUNC_ENTRY("CATBase::IsDataFile");
	// Check that sFile not empty
	if ( sFile.empty() || sFile.length() < 1 )
		return false;

	// Temporary line char array.
	char cLineFromFile[MAX_LINE_LENGTH];
	//Open file
	ifstream in( sFile.c_str() );

	//File open ok?
	if( !in.good() )
		return false;

	//Read all lines
	in.getline( cLineFromFile, MAX_LINE_LENGTH );

	string sLineFromFile( cLineFromFile );
	in.close();

	if( sLineFromFile.find( "ATOOL_BINARY_FILE_VERSION" ) != string::npos )
		return true;
	else
		return false;
}

// -----------------------------------------------------------------------------
// CATBase::ParseStringToVector
// -----------------------------------------------------------------------------
vector<string> CATBase::ParseStringToVector( const string& sInput, char separator )
{
	LOG_LOW_FUNC_ENTRY("CATBase::ParseStringToVector");
	string sString(sInput);
	// Elements vector
	vector<string> vStrings;
	size_t iPos = sString.find( separator );
	// If can not find it return vector with just one element
	if ( iPos == string::npos )
	{
		// Don't add empty item into vector.
		if ( sString.size() > 0 )
			vStrings.push_back( sString );
		return vStrings;
	}
	// Loop elements
	while( iPos != string::npos )
	{
		string sElement = sString.substr(0, iPos);
		vStrings.push_back( sElement );
		sString.erase(0, iPos +1 );
		iPos = sString.find( separator );
	}
	// Add last element if any
	if ( sString.size() > 0 )
		vStrings.push_back( sString );
	// Return elements
	return vStrings;
}

// -----------------------------------------------------------------------------
// CATBase::FilterString
// Filter string out of unwanted characters. The list of allowed
// characters is defined in CFILTERSTRING.
// -----------------------------------------------------------------------------
string CATBase::FilterString( const string& sString )
{
	LOG_LOW_FUNC_ENTRY("CATBase::FilterString");
	string sFiltered;
	for( size_t i = 0 ; i < sString.length() ; i++ )
	{
		const char p = sString.at( i );
		if ( strchr( CFILTERSTRING, p ) !=  0 )
			sFiltered.push_back( p );
	}
	return sFiltered;
}

// -----------------------------------------------------------------------------
// CATBase::FilterExtraSpaces
// Replaces multiple continuous spaces with single. Won't leave
// spaces in start or end of string.
// -----------------------------------------------------------------------------
void CATBase::FilterExtraSpaces( string& sString )
{
	LOG_LOW_FUNC_ENTRY("CATBase::FilterExtraSpaces");
	string sFiltered;
	// Loop thru char array.
	for( size_t i = 0 ; i < sString.length(); i++ )
	{
		// Is char space?
		if ( sString.at( i ) == ' ' )
		{
			// Pick up space if filtered does not contain char as last.
			if ( sFiltered.rbegin() == sFiltered.rend() )
				sFiltered.push_back( sString.at( i ) );
			else if ( * ( sFiltered.rbegin() ) != ' ' )
				sFiltered.push_back( sString.at( i ) );
		}
		else
			sFiltered.push_back( sString.at( i ) );
	}

	// Remove first and/or last character if it is space.
	if ( sFiltered.begin() != sFiltered.end() )
	{
		if( * ( sFiltered.begin() ) == ' ' )
			sFiltered.erase( 0, 1 );
	}
	if ( sFiltered.rbegin() != sFiltered.rend() )
	{
		if( * ( sFiltered.rbegin() ) == ' ' )
			sFiltered.resize( sFiltered.length()-1 );
	}
	sString = sFiltered;
}


bool CATBase::hexToDec( string& sHex, unsigned int& iDec )
{
	LOG_LOW_FUNC_ENTRY("CATBase::hexToDec");
	istringstream ss( sHex );
	ss.setf( ios::hex, ios::basefield );
	if( ( ss >> iDec ) )
		return true;
	return false;
}

bool CATBase::hexToDec( string& sHex, int& iDec )
{
	LOG_LOW_FUNC_ENTRY("CATBase::hexToDec");
	istringstream ss( sHex );
	ss.setf( ios::hex, ios::basefield );
	if( ( ss >> iDec ) )
		return true;
	return false;
}

bool CATBase::hexToDec( string& sHex, unsigned long& ulDec )
{
	LOG_LOW_FUNC_ENTRY("CATBase::hexToDec");
	istringstream ss( sHex );
	ss.setf( ios::hex, ios::basefield );
	if( ( ss >> ulDec ) )
		return true;
	return false;
}

bool CATBase::hexToDec( string& sHex, unsigned long long& ullDec )
{
	LOG_LOW_FUNC_ENTRY("CATBase::hexToDec");
	istringstream ss( sHex );
	ss.setf( ios::hex, ios::basefield );
	if( ( ss >> ullDec ) )
		return true;
	return false;
}

/**
* Used to create array of integer & hex value pairs.
*/
struct CHexMap
{
	char chr;
	int value;
};

// -----------------------------------------------------------------------------
// CATBase::_httoi
// -----------------------------------------------------------------------------
unsigned long CATBase::_httoi(const char *value)
{
	LOG_LOW_FUNC_ENTRY("CATBase::_httoi");
	unsigned long l;
	string s( value );
	if ( CATBase::hexToDec( s, l ) )
		return l;
	return 0;
}


// -----------------------------------------------------------------------------
// CATBase::NumberToHexString(int)
// -----------------------------------------------------------------------------
string CATBase::NumberToHexString( unsigned int i )
{
	LOG_LOW_FUNC_ENTRY("CATBase::IntToHexString");
	stringstream ss;
	ss << "0x" << hex << i;
	string retval; retval = ss.str().c_str();
	return retval;
}
// -----------------------------------------------------------------------------
// CATBase::NumberToHexString(long)
// -----------------------------------------------------------------------------
string CATBase::NumberToHexString( unsigned long i )
{
	LOG_LOW_FUNC_ENTRY("CATBase::IntToHexString");
	stringstream ss;
	ss << "0x" << hex << i;
	string retval; retval = ss.str().c_str();
	return retval;
}

// -----------------------------------------------------------------------------
// CATBase::IsHexCharacter
// -----------------------------------------------------------------------------
bool CATBase::IsHexCharacter(const TCHAR *value)
{
	LOG_LOW_FUNC_ENTRY("CATBase::IsHexCharacter");
	const int HexMapL = 22;
	CHexMap HexMap[HexMapL] =
	{
	    {'0', 0}, {'1', 1},
		{'2', 2}, {'3', 3},
		{'4', 4}, {'5', 5},
		{'6', 6}, {'7', 7},
		{'8', 8}, {'9', 9},
		{'A', 10}, {'B', 11},
		{'C', 12}, {'D', 13},
		{'E', 14}, {'F', 15},
		{'a', 10}, {'b', 11},
		{'c', 12}, {'d', 13},
		{'e', 14}, {'f', 15}
	};
	bool found = false;
	for (int i = 0; i < HexMapL; i++)
	{
		if(HexMap[i].chr == *value)
		{
			found = true;
			break;
		}
	}
	return found;
}

// -----------------------------------------------------------------------------
// CATBase::IsAscii(const char*,const unsigned int)
// -----------------------------------------------------------------------------
bool CATBase::IsAscii( const char* pInput, const unsigned int iLength )
{
	LOG_LOW_FUNC_ENTRY("CATBase::IsAscii");
	bool bRet = true;
	const char* pPoint = pInput;
	for( unsigned int i = 0 ; i < iLength ; i++)
	{
		if(	!__isascii(*pPoint) )
		{
			bRet = false;
			break;
		}
		pPoint++;
	}
	return bRet;
}

// -----------------------------------------------------------------------------
// CATBase::GetEpocRoot( string& sEpocRoot )
// -----------------------------------------------------------------------------
bool CATBase::GetEpocRoot( string& sEpocRoot )
{
	LOG_FUNC_ENTRY( "CATBase::GetEpocRoot" );
	bool bRet = true;
	//Find EPOCROOT from environment variable
	char* pEpocRoot = getenv ("EPOCROOT");
	if( pEpocRoot == NULL )
	{
		const char pDevicesPath[] = "C:\\Program Files\\Common Files\\Symbian\\devices.xml";
		CATParseXML parser;
		//Find EPOCROOT from devices
		sEpocRoot = parser.GetEpocRootPathFromXML(pDevicesPath);
		if( sEpocRoot.empty() )
		{
			printf("EPOCROOT not set to environment variables.\n");
			bRet = false;
		}
	}
	else
	{
		sEpocRoot.append( pEpocRoot );
		LOG_STRING( "EpocRoot :" << sEpocRoot );
	}
	//Remove trailing slash
	if ( sEpocRoot.size() > 1 && sEpocRoot[ sEpocRoot.length()-1 ] == '\\' )
		sEpocRoot.resize( sEpocRoot.length()-1 );
	return bRet;
}
//End of file
