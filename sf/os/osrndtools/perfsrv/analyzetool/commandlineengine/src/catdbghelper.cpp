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
* Description:  Windows debug api implementation for IAddressToLine interface.
*
*/

#include "../inc/catdbghelper.h"
#include "../inc/CATBase.h"
#include "../inc/CATMemoryAddress.h"
#include <dbghelp.h>

/**
* Notes on version number of api functions.
* 5.1	Windows XP
* 5.2	Windows Server 2003
* 6.8	Debugging Tools for Windows 6.8
* SymSetOptions			DbgHelp.dll 5.1 or later
* SymSetSearchPath		DbgHelp.dll 5.1 or later
* SymLoadModuleEx			Versions 5.2 and 6.0
* SymLoadModule64			DbgHelp.dll 5.1 or later
* SymFromAddr				Versions 4.0 and 5.1
* SymGetLineFromAddr64	DbgHelp.dll 5.1 or later
*/

// Wrapper class for symbol information package.
struct CSymbolInfo : public SYMBOL_INFO_PACKAGE
{
	CSymbolInfo()
	{
		si.SizeOfStruct = sizeof( SYMBOL_INFO );
		si.MaxNameLen = sizeof( name );
	}
};

// Wrapper class for line information container.
struct CLineInfo : public IMAGEHLP_LINE64
{
	CLineInfo()
	{
		SizeOfStruct = sizeof( IMAGEHLP_LINE64 );
	}
};

CATDbgHelper::CATDbgHelper()
{
	LOG_FUNC_ENTRY("CATDbgHelper::CDbgHelper");
	// Set the some "default" base address.
	m_BaseAddress = 0x2;
	m_bMap = false;
	m_pBinaryFile = NULL;
}

CATDbgHelper::~CATDbgHelper()
{
	LOG_FUNC_ENTRY("CATDbgHelper::~CDbgHelper");
	// Close dbghelper only once.
	if ( CDBGHELPER_OPEN )
	{
		Close();
	}
	if ( m_pBinaryFile )
	{
		delete[] m_pBinaryFile;
		m_pBinaryFile = NULL;
	}
}

bool CATDbgHelper::Open( const string& sParameter, const unsigned long iLong )
{
	LOG_FUNC_ENTRY("CATDbgHelper::Open");
	// Verify that file exits. Version 5.1.2600.5512 of dbghelp.dll does not correctly
	// return error code if missing image file. This can lead upto applicaton crash.
	if ( ! CATBase::FileExists( sParameter.c_str() ) )
	{
		LOG_STRING( "Missing image file: " << sParameter );
		return false;
	}

	// Is it urel try read map?
	if ( sParameter.find( "\\urel\\" ) != string::npos )
	{
		string sMapFile = sParameter;
		sMapFile.append( ".map" );
		ReadMapFile( sMapFile );
	}

	// Set base address used
	m_BaseAddress = iLong + AT_VIRTUAL_OFFSET_DBGHELPER;
	// Binary file (also referred as symbol).
	size_t length = sParameter.length();
	if ( length == 0 )
	{
		LOG_STRING("DbgHelp:Invalid binary parameter.");
		return false;
	}

	char* pChar = new char[ sParameter.length()+1 ];
	strcpy( pChar, sParameter.c_str() );
	// Have to be casted to PSTR before using dbg api. Even tho its typedef same.
	// This will avoid access violations bug.
	// Note pChar is not deleted because its the member pointer just casted its
	// memory allocation freed in destructor.
	if ( m_pBinaryFile )
		delete[] m_pBinaryFile;

	m_pBinaryFile = (PSTR) pChar;

	// Initialize dbghelper if not done only once.
	if ( ! CDBGHELPER_OPEN )
	{
		// Set symbol options
		SymSetOptions( SYMOPT_LOAD_LINES | SYMOPT_DEBUG | SYMOPT_UNDNAME | SYMOPT_LOAD_ANYTHING );
		if ( !SymInitialize( GetCurrentProcess(), NULL, TRUE ) )
		{
			LOG_STRING("DbgHelp:Error initializing dbghelper " << (int) GetLastError());
			return false;
		}
		LOG_STRING("DbgHelp:dbghelper opened.");
		CDBGHELPER_OPEN = true;
	}

	// Set symbol search path.
	if ( !SymSetSearchPath( GetCurrentProcess(), NULL ) )
	{
		LOG_STRING("DbgHelp:Error setting symbol search path " << (int) GetLastError());
		return false;
	}

	// Load module.
	DWORD64 ret;
	ret = SymLoadModule64( GetCurrentProcess(), NULL, m_pBinaryFile, NULL, m_BaseAddress, NULL ); // 5.1 api version.
	if ( ret != m_BaseAddress  && ret != 0)
	{
		LOG_STRING("Dbghelp:Module load failed " << (int) GetLastError());
		return false;
	}
	CDBGHELPER_CLIENTS++;
	return true;
}

string CATDbgHelper::GetError( void )
{
	LOG_FUNC_ENTRY("CATDbgHelper::GetError");
	return string("not implemented.");
}

bool CATDbgHelper::Close( void )
{
	LOG_FUNC_ENTRY("CATDbgHelper::Close");
	if ( ! SymUnloadModule64( GetCurrentProcess(), m_BaseAddress ) )
	{
		LOG_STRING("Dbghelp:Module unload failed.");
	}
	CDBGHELPER_CLIENTS--;
	if ( CDBGHELPER_OPEN && CDBGHELPER_CLIENTS == 0)
	{
		// Cleanup dbghelper.
		if ( ! SymCleanup( GetCurrentProcess() ) )
		{
			LOG_STRING("dbghelper cleanup failed.");
			return false;
		}
		LOG_STRING("dbghelper closed.");
		// Set state not opened.
		CDBGHELPER_OPEN = false;
	}
	return true;
}

bool CATDbgHelper::AddressToLine( CATMemoryAddress* result )
{
	LOG_FUNC_ENTRY("CATDbgHelper::AddressToLine");
	
	// Set state out of range
	result->SetAddressToLineState( CATMemoryAddress::OUT_OF_RANGE );

	// check that dbghelper has been initialized successfully.
	if ( ! CDBGHELPER_OPEN )
		return false;

	// Check has binary been moved, if so unload and load to new base address.
	if ( result->GetModuleStartAddress() + AT_VIRTUAL_OFFSET_DBGHELPER != m_BaseAddress )
	{
		// Unload.
		if ( SymUnloadModule64( GetCurrentProcess(), m_BaseAddress ) )
		{
			// Set new base address.
			m_BaseAddress = result->GetModuleStartAddress() + AT_VIRTUAL_OFFSET_DBGHELPER;
			// (Re)load.
			DWORD64 loading = SymLoadModule64( GetCurrentProcess(), NULL, m_pBinaryFile, NULL, m_BaseAddress, NULL );
			if ( loading != m_BaseAddress  && loading != 0)	
			{
				LOG_STRING("Dbghelp:Module load failed " << (int) GetLastError());
				return false;
			}
		}
		else
			LOG_STRING("Dbghelp:Module unload failed " << (int) GetLastError() );
	}
	// Address to find (offset+given address).
	unsigned long iAddressToFind = result->GetAddress() + AT_VIRTUAL_OFFSET_DBGHELPER;
	// Displacements of line/symbol information.
	DWORD64 displacementSymbol;
	DWORD displacementLine;
	// Structure to get symbol information.
	CSymbolInfo symbol;
	// Structure to get line information.
	CLineInfo line;
	// Find Symbol for given address 
	if( ! SymFromAddr( GetCurrentProcess(), iAddressToFind , &displacementSymbol, &symbol.si ) )
	{
		LOG_STRING("Failed to find symbol information for given line.");
		return AddressToFunction( result );
	}
	// Find line information
	if( ! SymGetLineFromAddr64( GetCurrentProcess(), iAddressToFind, &displacementLine, &line ) )
	{
		// If it fails get symbol line information
		LOG_STRING("Dbghelp:Failed to find line information for address, trying for symbol of address.");
		if( ! SymGetLineFromAddr64( GetCurrentProcess(), symbol.si.Address, &displacementLine, &line ) )
		{
			LOG_STRING("Dbghelp:Failed to find line information for symbol address.");
			return AddressToFunction( result );
		}
	}
	// Set the results.
	result->SetFileName( string( line.FileName ) );
	result->SetFunctionName( string( symbol.si.Name ) );
	result->SetExactLineNumber( (int) line.LineNumber );
	result->SetAddressToLineState( CATMemoryAddress::EXACT );
	// Return.
	return true;
}

bool CATDbgHelper::AddressToFunction( CATMemoryAddress* result )
{
	LOG_FUNC_ENTRY("CATDbgHelper::AddressToFunction");
	bool bFound = false;
	// If map file read use it and return.
	if ( m_bMap )
	{
		ULONG uCountedA = result->GetOffSetFromModuleStart();
		for ( vector<MAP_FUNC_INFO>::iterator it = m_vMapFileFuncList.begin() ; it != m_vMapFileFuncList.end() ; it++ )
		{
			// Check is this the symbol where address is.
			unsigned long iStart = it->iAddress;
			unsigned long iEnd = it->iAddress + it->iFuncLength;
			if ( uCountedA >= iStart 
				&& uCountedA < iEnd )
			{
				result->SetAddressToLineState( CATMemoryAddress::SYMBOL );
				result->SetFunctionName( it->sMangledName );
				bFound = true;
				break;
			}
		}
	}
	return bFound;
}

void CATDbgHelper::ReadMapFile( const string sMapFileName )
{
	LOG_FUNC_ENTRY("CATDbgHelper::ReadMapFile");
	try {
		ifstream in( sMapFileName.c_str() );
		if ( ! in.good() )
		{
			in.close();
			return;
		}
		char cLine[MAX_LINE_LENGTH];
		do {
			in.getline( cLine, MAX_LINE_LENGTH );
			// Search pattern for 'image ro' symbols is ".text"
			string sLine( cLine );
			if ( sLine.find( ".text" ) != string::npos )
			{
				MAP_FUNC_INFO symbol;
				// Pickup symbol attributes
				// Address
				string sAddress = CATBase::GetStringUntilNextSpace( sLine, true );
				symbol.iAddress = CATBase::_httoi( sAddress.c_str() );
				// Lenght
				string sLength = CATBase::GetStringUntilNextSpace( sLine, true );
				symbol.iFuncLength = CATBase::_httoi( sLength.c_str() );
				// Name
				size_t iStart = sLine.find_first_of( '(' );
				size_t iEnd = sLine.find_last_of( ')' );
				if ( iStart != string::npos && iEnd != string::npos )
				{
					symbol.sMangledName = sLine.substr( iStart+1, iEnd-iStart-1 );
					// Add symbol to vector
					m_vMapFileFuncList.push_back( symbol );
				}
			}
		} while ( in.good() );
		in.close();
		m_bMap = true;
	} catch (...) {
		m_bMap = false;
		LOG_STRING("DbgHelp: Error reading map file.");
	}
}