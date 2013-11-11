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
* Description:  Contains functions to check AT libraries.
*
*/
#include "../inc/ATCommonDefines.h"
#include "../inc/CATBase.h"

// Function declarations
bool CheckATLibrariesArmv5( string sEpocRoot );
bool CheckATLibrariesArmv5Abiv2( string sEpocRoot );
bool CheckATLibrariesWinscw( string sEpocRoot );

// Error msg to user if missing lib
const char cMissingAToolLibs[] = "\nCan not find AnalyzeTool libraries from current SDK\n\nInstall libraries first\n";

// List of libraries what AnalyzeTool needs when compiled applications on armv5 platform
const string cAToolLibsArmv5[] = {
	"epoc32\\RELEASE\\armv5\\LIB\\AToolMemoryHook.lib",
	"epoc32\\RELEASE\\armv5\\udeb\\AtoolStaticLib.lib",
	"epoc32\\RELEASE\\armv5\\urel\\AtoolStaticLib.lib"
};

// List of libraries what AnalyzeTool needs when compiled applications on armv5 platform (using sbs2 / ABIV2 binaries)
const string cAToolLibsArmv5Abiv2[] = {
	"epoc32\\RELEASE\\armv5\\LIB\\AToolMemoryHook.dso",
	"epoc32\\RELEASE\\armv5\\udeb\\AtoolStaticLib.lib",
	"epoc32\\RELEASE\\armv5\\urel\\AtoolStaticLib.lib"
};

// List of libraries what AnalyzeTool needs when compiled applications on winscw platform
const string cAToolLibsWinscw[] = {
	"epoc32\\RELEASE\\winscw\\udeb\\AToolMemoryHook.lib",
	"epoc32\\RELEASE\\winscw\\udeb\\AtoolStaticLib.lib",
	"epoc32\\RELEASE\\winscw\\urel\\AtoolStaticLib.lib"
};

/**
* CheckATLibrariesArmv5
* Checks that armv5 libraries are in sEpocRoot
* @param sEpocRoot, epoc root where to search libs
* @return bool true if they are found otherwise false
*/
bool CheckATLibrariesArmv5(string sEpocRoot )
{
	LOG_FUNC_ENTRY("CheckATLibrariesArmv5");

	// check that epocroot is set
	if ( sEpocRoot.length() <= 0 )
	{
		LOG_FUNC_EXIT("CheckATLibrariesArmv5 Error, EpocRoot not set");
		return false;
	}

	// add trailing '\' if root path is missing it
	if ( sEpocRoot.at( sEpocRoot.length() -1 ) != '\\' )
		sEpocRoot.append( "\\" );

	// return boolean value
	bool bReturn = true;

	int arraySize = sizeof( cAToolLibsArmv5 ) / sizeof( string );
	for ( int i=0 ; i < arraySize ; i++ )
	{
		// append epocroot to file
		string sFileToCheck = sEpocRoot;
		sFileToCheck.append( cAToolLibsArmv5[i] );
		// check does it exists
		if ( ! CATBase::FileExists( sFileToCheck.c_str() ) )
		{
			bReturn = false;
			cout << AT_MSG << "Missing library file: " << sFileToCheck << endl;
			LOG_STRING("Missing library file: " << sFileToCheck);
		}
	}

	if ( ! bReturn )
	{
		// print error msg to user
		cout << cMissingAToolLibs;

	}
	return bReturn;
}

/**
* CheckATLibrariesArmv5Abiv2
* Checks that armv5 abiv2 libraries are in sEpocRoot
* @param sEpocRoot, epoc root where to search libs
* @return bool true if they are found otherwise false
*/
bool CheckATLibrariesArmv5Abiv2(string sEpocRoot )
{
	LOG_FUNC_ENTRY("CheckATLibrariesArmv5Abiv2");

	// check that epocroot is set
	if ( sEpocRoot.length() <= 0 )
	{
		LOG_FUNC_EXIT("CheckATLibrariesArmv5Abiv2 Error, EpocRoot not set");
		return false;
	}

	// add trailing '\' if root path is missing it
	if ( sEpocRoot.at( sEpocRoot.length() -1 ) != '\\' )
		sEpocRoot.append( "\\" );

	// return boolean value
	bool bReturn = true;

	int arraySize = sizeof( cAToolLibsArmv5Abiv2 ) / sizeof( string );
	for ( int i=0 ; i < arraySize ; i++ )
	{
		// append epocroot to file
		string sFileToCheck = sEpocRoot;
		sFileToCheck.append( cAToolLibsArmv5Abiv2[i] );
		// check does it exists
		if ( ! CATBase::FileExists( sFileToCheck.c_str() ) )
		{
			bReturn = false;
			cout << AT_MSG << "Missing library file: " << sFileToCheck << endl;
			LOG_STRING("Missing library file: " << sFileToCheck);
		}
	}

	if ( ! bReturn )
	{
		// print error msg to user
		cout << cMissingAToolLibs;

	}
	return bReturn;
}

/**
* CheckATLibrariesWinscw
* Checks that winscw libraries are in sEpocRoot
* @param sEpocRoot, epoc root where to search libs
* @return bool true if they are found otherwise false
*/
bool CheckATLibrariesWinscw(string sEpocRoot )
{
	LOG_FUNC_ENTRY("CheckATLibrariesWinscw");

	// check that epocroot is set
	if ( sEpocRoot.length() <= 0 )
	{
		LOG_FUNC_EXIT("CheckATLibrariesArmv5Abiv2 Error, EpocRoot not set");
		return false;
	}

	// add trailing '\' if root path is missing it
	if ( sEpocRoot.at( sEpocRoot.length() -1 ) != '\\' )
		sEpocRoot.append( "\\" );

	// return boolean value
	bool bReturn = true;

	int arraySize = sizeof( cAToolLibsWinscw ) / sizeof( string );
	for ( int i=0 ; i < arraySize ; i++ )
	{
		// append epocroot to file
		string sFileToCheck = sEpocRoot;
		sFileToCheck.append( cAToolLibsWinscw[i] );
		// check does it exists
		if ( ! CATBase::FileExists( sFileToCheck.c_str() ) )
		{
			bReturn = false;
			cout << AT_MSG << "Missing library file: " << sFileToCheck << endl;
			LOG_STRING("Missing library file: " << sFileToCheck);
		}
	}

	if ( ! bReturn )
	{
		// print error msg to user
		cout << cMissingAToolLibs;

	}
	return bReturn;
}

//EOF
