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
* Description:  Show / Check atool version.
*
*/
#include "../inc/ATCommonDefines.h"
#include "../inc/CATBase.h"

//Function declarations.
int showVersionInfo( void );
int showCoreVersionInfo( void );
bool readCoreVersionInfo( const string& sFile, string& sVersion );
string findAnalyzeToolHeader( void );
int showDbgHelpVersionInfo( bool showVersion );
int getDllVersion( LPCTSTR libName, WORD *majorVersion, WORD *minorVersion, WORD *buildNumber, WORD *revisionNumber );

//External GetEpocRoot function
extern bool GetEpocRoot( string &sEpocRoot );

void convertWriteTimeToLocalTime( FILETIME ftWrite, LPSTR lpszString );

int showCoreVersionInfo( void )
{
	LOG_FUNC_ENTRY( "version.cpp::showCoreVersionInfo" );
	// Try find header from environment where to locate version info.
	string sHeaderFile = findAnalyzeToolHeader();
	if ( sHeaderFile.empty() )
		return 0;
	string sVersion("");
	if ( readCoreVersionInfo( sHeaderFile, sVersion ) )
	{
		cout << "AnalyzeTool SDK binaries version: " 
			<< sVersion
			<< endl;
	}
	return 0;
}


/**
* Find analyzetool.h header file
* @return string containing full path to file or empty string if not found.
*/
string findAnalyzeToolHeader( void )
{
	LOG_FUNC_ENTRY( "version.cpp::findAnalyzeToolHeader" );
	string sEpocRoot;
	if ( ! CATBase::GetEpocRoot( sEpocRoot ) )
		return string("");
	int iC = sizeof( AT_CORE_INCLUDE_FILE_WITH_VERSION_NUMBER ) / sizeof ( string );
	for( int i = 0 ; i < iC ; i++ )
	{
		string sCheck( sEpocRoot );
		sCheck.append( AT_CORE_INCLUDE_FILE_WITH_VERSION_NUMBER[i] );
		if ( CATBase::FileExists( sCheck.c_str() ) )
			return sCheck;
	}
	return string("");
}
/**
* Read core version string.
* @param sVersion string will contain version info if funtion returns true.
* @return true if successful.
*/
bool readCoreVersionInfo( const string& sFile, string& sVersion )
{
	LOG_FUNC_ENTRY( "version.cpp::readCoreVersionInfo" );
	try {
		ifstream in;
		in.open( sFile.c_str() );
		if ( ! in.good() )
			return false;
		char cBuff[MAX_LINE_LENGTH];
		while ( in.good() )
		{
			in.getline( cBuff, MAX_LINE_LENGTH );
			string s( cBuff );
			if ( s.find( AT_CORE_VERSION_NUMBER_TAG ) != string::npos )
			{
				// Find spot after first space (ignore first 3 chars).
				size_t t =  s.find_first_of( ' ', 3 )+1;
				sVersion = s.substr( t, s.size()-t );
				return true;
			}
		}
	}
	catch(...)
	{
		LOG_STRING(AT_MSG << "Exception reading core version info.");
	}
	return false;
}

/**
* Print version information of atool.exe binary.
*/
int showVersionInfo( void )
{
	LOG_FUNC_ENTRY( "version.cpp::showVersionInfo" );
	string sTemp( "Version: " );
	sTemp.append( ATOOL_VERSION );
	sTemp.append( "\n" );
	//Print atool version
	printf( sTemp.c_str() );

	//atool.exe:s path + filename
	char buffer[MAX_PATH];

	GetModuleFileName( NULL, buffer, MAX_PATH );

	printf( "Path: %s\n", buffer );

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//Get file handle
	hFind = FindFirstFile( buffer, &FindFileData );

	if( hFind == INVALID_HANDLE_VALUE )
	{
		printf( "Can not find file:%s", buffer );
		return 0;
	}

	convertWriteTimeToLocalTime( FindFileData.ftLastWriteTime , buffer );
	printf( "Modified: %s\n", buffer );

	// Show core version information.
	showCoreVersionInfo();
	return 0;
}

// Convert the last-write time to local time.
void convertWriteTimeToLocalTime( FILETIME ftWrite, LPSTR lpszString )
{
	LOG_FUNC_ENTRY( "version.cpp::convertWriteTimeToLocalTime" );
	SYSTEMTIME stUTC, stLocal;
    // Convert the last-write time to local time.
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    // Build a string showing the date and time.
    wsprintf(lpszString, "%02d/%02d/%d %02d:%02d:%02d",
        stLocal.wDay, stLocal.wMonth, stLocal.wYear,
        stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
}

/**
* Print version information of dbghelp.dll.
*/
int showDbgHelpVersionInfo( bool showVersion )
{
	LOG_FUNC_ENTRY( "version.cpp::showDbgHelpVersionInfo" );

	WORD majorVersion;
	WORD minorVersion;
	WORD buildNumber;
	WORD revisionNumber;

	if(getDllVersion( _T(DBGHELP_DLL_NAME), &majorVersion, &minorVersion, &buildNumber, &revisionNumber))
	{
		//Check if current dll version is lower than latest version
		if((majorVersion < DBGHELP_VERSION_MAJ) ||
			(majorVersion == DBGHELP_VERSION_MAJ && minorVersion < DBGHELP_VERSION_MIN) ||
			(majorVersion == DBGHELP_VERSION_MAJ && minorVersion == DBGHELP_VERSION_MIN && buildNumber < DBGHELP_VERSION_BUILD) ||
			(majorVersion == DBGHELP_VERSION_MAJ && minorVersion == DBGHELP_VERSION_MIN && buildNumber == DBGHELP_VERSION_BUILD && revisionNumber < DBGHELP_VERSION_REVIS))
		{
			cout << "WARNING: Version of your dbghelp.dll is "
					<< majorVersion << "."
					<< minorVersion << "."
					<< buildNumber << "."
					<< revisionNumber << ".\n";

			cout << "Source code pinpointing for winscw may not work properly with this version."
				<< endl
				<< "Please, update it to at least version "
				<< DBGHELP_VERSION_MAJ << "." 
				<< DBGHELP_VERSION_MIN << "." 
				<< DBGHELP_VERSION_BUILD << "." 
				<< DBGHELP_VERSION_REVIS << ".\n"
				<< "dbghelp.dll file is included in Debugging Tools from Microsoft. "
				<< "You can download and install it from\n"
				<< "http://www.microsoft.com/whdc/devtools/debugging/\n";

			return 1;
		}
		else
		{
			if( showVersion )
			{
				cout << "Version of your dbghelp.dll is "
					<< majorVersion << "."
				    << minorVersion << "."
				    << buildNumber << "."
				    << revisionNumber << "."
				    << endl;
				cout << "No need for update.\n";
			}
		}
	}

	return 0;
}

/**
* Get dll version.
* @param libName library name.
* @param majorVersion will contain major version if funtion returns true.
* @param minorVersion will contain minor version if funtion returns true.
* @param buildNumber will contain build number if funtion returns true.
* @param revisionNumber will contain revision number if funtion returns true.
* @return true if successful.
*/
int getDllVersion( LPCTSTR  libName, WORD *majorVersion, WORD *minorVersion, WORD *buildNumber, WORD *revisionNumber ) 
{
	LOG_FUNC_ENTRY( "version.cpp::getDllVersion" );

	DWORD dwHandle, dwLen;
	UINT BufLen;
	LPTSTR lpData;
	VS_FIXEDFILEINFO *pFileInfo;
	dwLen = GetFileVersionInfoSize( libName, &dwHandle );
	if (!dwLen)
		return 0;
	
	lpData = (LPTSTR) malloc (dwLen);
	if (!lpData)
		return 0;
	
	if( !GetFileVersionInfo( libName, dwHandle, dwLen, lpData ) )
	{
		free (lpData);
		return 0;
	}
	
	if( VerQueryValue( lpData, _T("\\"), (LPVOID *) &pFileInfo, (PUINT)&BufLen ) )
	{
		*majorVersion = HIWORD(pFileInfo->dwFileVersionMS);
		*minorVersion = LOWORD(pFileInfo->dwFileVersionMS);
		*buildNumber = HIWORD(pFileInfo->dwFileVersionLS);
		*revisionNumber = LOWORD(pFileInfo->dwFileVersionLS);
		return 1;
	 }
	free (lpData);
	return 0;
}

//EOF
