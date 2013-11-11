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
* Description:  Class representing a module in project (sbs2)
*
*/


#include "../inc/CATModule2.h"
#include "../inc/CATProject.h"
#include "../inc/CATDatParser.h"
#include "../inc/CATMemoryAddress.h"
#include "../inc/catdbghelper.h"
#include "../inc/cataddr2line.h"

CATModule2::CATModule2(void)
{
	LOG_FUNC_ENTRY("CATModule2::CATModule2");
	m_bAddressToLineInitialized = false;
	m_bS60FileNameResolved = false;
	m_pAddressToLine = 0;
	m_sErrors = "";
	m_sMakeFile = "";
	m_eBuildSystem = CATProject::SBS_V1;
	m_sCompileInfoText = "";
}

CATModule2::~CATModule2(void)
{
	LOG_FUNC_ENTRY("CATModule2::~CATModule2");
	if ( m_pAddressToLine )
		m_pAddressToLine->Close();
	delete m_pAddressToLine;
}

bool CATModule2::AddressToLine( CATMemoryAddress* pMemoryAddress )
{
	LOG_FUNC_ENTRY("CATModule2::AddressToLine");
	if ( _stricmp( m_sVariantPlatform.c_str(), "winscw" ) == 0 )
	{
		return AddressToLineWinscw( pMemoryAddress );
	}
	else if ( _stricmp( m_sVariantPlatform.c_str(), "armv5" ) == 0 )
	{
		// addr2line exe.
		#ifdef ADDR2LINE
		return AddressToLineAddr2lineExe( pMemoryAddress );
		#endif
		// lst and map files.
		#ifndef ADDR2LINE
		return AddressToLineArmv5( pMemoryAddress );
		#endif
	}
	else if ( _stricmp( m_sVariantPlatform.c_str(), "gcce" ) == 0 )
	{
		return AddressToLineAddr2lineExe( pMemoryAddress );
	}
	return false;
}

bool CATModule2::AddressToLineWinscw( CATMemoryAddress* pMemoryAddress )
{
	LOG_FUNC_ENTRY("CATModule2::AddressToLineWinscw( CATMemoryAddress* pMemoryAddress )");
	if ( m_pAddressToLine == 0 && ! m_bAddressToLineInitialized )
	{
		// Use debug helper to locate codelines on winscw platform.
		m_pAddressToLine = new CATDbgHelper();

		// Create full path to binary which we open using CATDbgHelper.
		string sFullPathToBinary = GetBinaryFile();

		// If opening of binary not succesfull set return value to false.
		if ( ! m_pAddressToLine->Open( sFullPathToBinary, pMemoryAddress->GetModuleStartAddress() ) )
		{
			LOG_STRING("Error, m_pAddressToLine->Open()");
			return false;
		}
		m_bAddressToLineInitialized = true;
	}
	// Check pointer before calling.
	if ( m_pAddressToLine == 0 )
		return false;
	m_pAddressToLine->AddressToLine( pMemoryAddress );
	return true;
}

bool CATModule2::AddressToLineArmv5( CATMemoryAddress* pMemoryAddress )
{
	LOG_FUNC_ENTRY("CATModule2::AddressToLine( CATMemoryAddress* pMemoryAddress )");
	if ( ! m_bAddressToLineInitialized )
		return false;
	// Find from map file
	int iMapIndex = GetSymbolIndexUsingAddress( pMemoryAddress->GetOffSetFromModuleStart() );
	if ( iMapIndex == -1 )
	{
		pMemoryAddress->SetAddressToLineState( CATMemoryAddress::OUT_OF_RANGE );
		return true;
	}
	// Set symbol name
	string sSymbolName = m_vMapFileFuncList.at( iMapIndex ).sFunctionName;

	// Remove (... from symbol name
	string sSymbolNameRefined( sSymbolName );
	size_t iPos = sSymbolNameRefined.find( "(" );
	if ( iPos != string::npos )
		sSymbolNameRefined.resize( iPos );

	// Set symbol name as function name for memory address
	pMemoryAddress->SetFunctionName( sSymbolNameRefined );

	// Set state to symbol
	pMemoryAddress->SetAddressToLineState( CATMemoryAddress::SYMBOL );
	
	// Offset from function start addr
	int iOffSetFromFuncStart = pMemoryAddress->GetOffSetFromModuleStart()
		- m_vMapFileFuncList.at( iMapIndex ).iAddress;

	// Find from lst list
	int iLstIndex = GetLineInFileIndexUsingSymbolName( sSymbolName );
	if ( iLstIndex == -1 )
		return true;

	// Set pinpointing
	int iFuncLineNumber = m_vLineInFile.at( iLstIndex ).iLine;
	string sFileName = m_vLineInFile.at( iLstIndex ).sFileName;
	string sLstFileName = m_vLineInFile.at( iLstIndex ).sLstName;

	pMemoryAddress->SetFunctionLineNumber( iFuncLineNumber );
	pMemoryAddress->SetFileName( sFileName );
	
	pMemoryAddress->SetAddressToLineState( CATMemoryAddress::FUNCTION );

	// In urel mode don't get exact code line
	if ( ! IsUDEB() )
		return true;

	// Next calculate the code line inside function
	int iExactLineNumber = FindLeakCodeLine( sLstFileName, iFuncLineNumber, iOffSetFromFuncStart );
	pMemoryAddress->SetExactLineNumber( iExactLineNumber );

	// State is now exact
	pMemoryAddress->SetAddressToLineState( CATMemoryAddress::EXACT );
	return true;
}

bool CATModule2::AddressToLineAddr2lineExe( CATMemoryAddress* pMemoryAddress )
{
	LOG_FUNC_ENTRY("CATModule2::AddressToLineAddr2lineExe( CATMemoryAddress* pMemoryAddress )");
	if ( m_pAddressToLine == 0 && ! m_bAddressToLineInitialized )
	{
		// Use addr2line.exe to locate codelines on armv5 and gcce platform.
		m_pAddressToLine = new CATAddr2line();

		// Create full path to binary .sym file which we open using addr2line.exe.
		string sFullPathToBinary = GetBinaryFile();

		// If opening of binary not succesfull set return value to false.
		if ( ! m_pAddressToLine->Open( sFullPathToBinary, pMemoryAddress->GetModuleStartAddress() ) )
		{
			LOG_STRING("Error, m_pAddressToLine->Open()");
			return false;
		}
		m_bAddressToLineInitialized = true;
	}
	// Check pointer before calling.
	if ( m_pAddressToLine == 0 )
		return false;

	m_pAddressToLine->AddressToLine( pMemoryAddress );
	return true;
}

// Find symbol of given address
int CATModule2::GetSymbolIndexUsingAddress( unsigned long iAddress ) const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetSymbolIndexUsingAddress");
	for( size_t i = 0; i < m_vMapFileFuncList.size(); i++ )
	{
		unsigned long iStart = m_vMapFileFuncList.at( i ).iAddress;
		unsigned long iEnd = ( m_vMapFileFuncList.at( i ).iAddress
			+ m_vMapFileFuncList.at( i ).iFuncLength );

		if ( iAddress >= iStart && iAddress < iEnd )
			return (int) i;
	}
	return -1;
}

// Find index of function line in file vector of given symbolname
int CATModule2::GetLineInFileIndexUsingSymbolName( const string& sSymbolName ) const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetLineInFileIndexUsingSymbolName");
	for( size_t i = 0; i < m_vLineInFile.size(); i++ )
	{
		string sLineInFileName = m_vLineInFile.at( i ).sFunction;
		if( sLineInFileName.find( sSymbolName ) != string::npos )
		{
			return (int) i;
		}
	}
	return -1;
}


// Check does modules symbol file(s) exist.
bool CATModule2::SymbolFileExist( void )
{
	LOG_FUNC_ENTRY("CATModule2::SymbolFileExist");
	string sFullPathToSym = GetSymbolFile();
	if ( !FileExists( sFullPathToSym.c_str() ) )
	{
		// Add missing symbol file to error string.
		m_sErrors.append( "Missing symbol file: " );
		m_sErrors.append( sFullPathToSym );
		m_sErrors.append( "\n" );
		return false;
	}
	return true;
}

// Check does modules map file(s) exists.
bool CATModule2::MapFileExist( void )
{
	LOG_FUNC_ENTRY("CATModule2::MapFileExist");
	string sFullPathToMap = GetMapFile();
	if ( !FileExists( sFullPathToMap.c_str() ) )
	{
		// Add missing symbol file to error string.
		m_sErrors.append( "Missing map file: " );
		m_sErrors.append( sFullPathToMap );
		m_sErrors.append( "\n" );
		return false;
	}
	return true;
}

//Check does modules binary file(s) exist.
bool CATModule2::BinaryFileExist( void )
{
	LOG_FUNC_ENTRY("CATModule2::BinaryFileExist");
	string sFullPathToBinary = GetBinaryFile();
	if ( ! FileExists( sFullPathToBinary.c_str() ) )
	{
		// Add missing binary to error string.
		m_sErrors.append( "Missing binary file: " );
		m_sErrors.append( sFullPathToBinary );
		m_sErrors.append( "\n" );
		return false;
	}
	return true;
}

void CATModule2::AddSource(const string &sSourceFile, const string& sLstFile)
{
	LOG_LOW_FUNC_ENTRY("CATModule2::AddSource");
	// Parse sources which are separated by spaces
	if( sSourceFile.length() < 1  || sLstFile.length() < 1 )
		return;

	// Skip if its temporary cpp.
	if ( sSourceFile.find( AT_TEMP_CPP_LOWER_START) != string::npos )
		return;

	// Source structure
	SOURCE sNew;
	sNew.sCpp =  sSourceFile;
	sNew.sLst = sLstFile;

	// Verify paths.
	ConvertUnixPathToWin( sNew.sCpp );
	ConvertUnixPathToWin( sNew.sLst );

	// Lower case them.
	ChangeToLower( sNew.sCpp );
	ChangeToLower( sNew.sLst );

	// Add it
	m_vSources.push_back( sNew );
}

void CATModule2::AddSources(string& sSource)
{
	LOG_LOW_FUNC_ENTRY("CATModule2::AddSources");
	// Parse sources which are separated by spaces
	if( sSource.length() < 1 )
		return;
	// Source structure
	SOURCE sNew;
	size_t iSpot = string::npos;
	iSpot = sSource.find( " " );
	while( iSpot != string::npos )
	{
		// Pickup source
		sNew.sCpp = sSource.substr(0, iSpot);
		// Convert path from Unix to Win
		ConvertUnixPathToWin( sNew.sCpp );
		// Lowercase it
		ChangeToLower( sNew.sCpp );
		// If its temp skip this
		if ( sNew.sCpp.find( AT_TEMP_CPP_LOWER_START ) == string::npos )
		{
			// Get corresponding lst file for source
			sNew.sLst = GetLstNameOfSource( sNew.sCpp );
			m_vSources.push_back( sNew );
			// Remove it from sSource
			sSource.erase(0,iSpot+1);
			// Find new one
		}
		iSpot = sSource.find( " " );
	}
	// Pickup last or only one source
	sNew.sCpp = sSource;
	// Convert path from unix to win
	ConvertUnixPathToWin( sNew.sCpp );
	// Lowercase it
	ChangeToLower( sNew.sCpp );
	// Lst name
	sNew.sLst = GetLstNameOfSource( sNew.sCpp );
	if ( sNew.sCpp.find( AT_TEMP_CPP_LOWER_START ) == string::npos )
	{
		// Get corresponding lst file for source
		sNew.sLst = GetLstNameOfSource( sNew.sCpp );
		m_vSources.push_back( sNew );
	}
}
bool CATModule2::CreateTempCpp(const string& sS60FileName
							        , const string& sS60FilePath
									, int eLoggingMode
									, int eBuildType
									, int iAllocCallStackSize
									, int iFreeCallStackSize )
{
	LOG_FUNC_ENTRY("CATModule2::CreateTemporaryCpp");

	// Make s60 filename target.type.dat if its empty and logging mode is file
	if ( eLoggingMode == CATProject::FILE )
	{
		// S60 filename
		SetS60FileName( sS60FileName );
		
		// S60 filepath
		// use double slashes in temp cpp file
		m_sS60FilePath = sS60FilePath;
		int iIgnore(0);
		size_t iPos = m_sS60FilePath.find("\\", iIgnore );
		while( iPos != string::npos )
		{
			m_sS60FilePath.replace( iPos, 1, "\\\\" );
			// dont replace previously replaced slashes
			iIgnore = iPos + 2;
			iPos = m_sS60FilePath.find("\\", iIgnore );
		}


	}
	return CreateTemporaryCpp( GetUniqueId(), m_sTempPath,
		m_sS60FileName, m_sS60FilePath, eLoggingMode, eBuildType, iAllocCallStackSize, iFreeCallStackSize );
}

bool CATModule2::ModifyMmp()
{
	LOG_FUNC_ENTRY("CATModule2::ModifyMmp");
	// Create backup
	if ( ! m_Mmp.BackupMmpFile() )
		return false;
	// Hook
	return m_Mmp.EditMmpFile( m_sTargetType, GetUniqueId() );
}

bool CATModule2::RestoreMmp()
{
	LOG_FUNC_ENTRY("CATModule2::RestoreMmp");
	// Restore mmp from backup
	return m_Mmp.RestoreMmpFile();
}

bool CATModule2::VerifyAndRecoverMmp()
{
	LOG_FUNC_ENTRY("CATModule2::VerifyAndRecoverMmp");
	// Verify mmp
	return m_Mmp.VerifyAndRecover();
}

// ----------------------------------------------------------------------------
// Releasables Handling methos
// ----------------------------------------------------------------------------
bool CATModule2::CopyReleasables()
{
	LOG_FUNC_ENTRY("CATModule2::CopyReleasables");
	bool bRet = true;
	if ( ! CopyLstFilesToTemp() )
		bRet = false;
	if ( ! CopyMapFileToTemp() )
		bRet = false;
	return bRet;
}

bool CATModule2::CopyLstFilesToDir( const string& sDir )
{
	LOG_FUNC_ENTRY("CATModule2::CopyLstFilesToDir");
	bool bRet = true;
	// Copy lst files to given directory.
	vector<SOURCE>::const_iterator source;
	for( source = m_vSources.begin(); source != m_vSources.end() ; source++ )
	{
		if ( ! FileCopyToPath( source->sLst, sDir ) )
		{
			if ( !FileExists( source->sLst.c_str() ) )
			{
				m_sErrors.append( "Missing listing file: " );
				m_sErrors.append( source->sLst );
				m_sErrors.append( "\n" );
			}
			if ( !DirectoryExists( sDir.c_str() ) )
			{
				m_sErrors.append( "Missing folder: " );
				m_sErrors.append( sDir );
				m_sErrors.append( "\n" );
			}
			bRet = false;
		}
	}
	// Return.
	return bRet;
}

bool CATModule2::CopyLstFilesToTemp()
{
	LOG_FUNC_ENTRY("CATModule2::CopyLstFilesToTemp");
	// Return boolean
	bool bRet = true;
	// Move all lst files except tmp cpp
	vector<SOURCE>::iterator it = m_vSources.begin();
	while ( it != m_vSources.end() )
	{
		if ( !FileCopyToPath( it->sLst, m_sTempPath ) )
		{
			if ( !FileExists( it->sLst.c_str() ) )
			{
				m_sErrors.append( "Missing listing file: " );
				m_sErrors.append( it->sLst );
				m_sErrors.append( "\n" );
			}
			if ( !DirectoryExists( m_sTempPath.c_str() ) )
			{
				m_sErrors.append( "Missing folder: " );
				m_sErrors.append( m_sTempPath );
				m_sErrors.append( "\n" );
			}
			bRet = false;
		}
		it++;
	}
	return bRet;
}

bool CATModule2::DeleteLstFilesFromSrc( void )
{
	LOG_FUNC_ENTRY("CATModule2::DeleteLstFilesFromSrc");
	vector<SOURCE>::iterator it = m_vSources.begin();
	bool bRet = true;
	// Delete lst files
	while ( it != m_vSources.end() )
	{
		if ( ! FileDelete( it->sLst, true ) )
			bRet = false;
		it++;
	}
	return bRet;
}

bool CATModule2::CopyMapFileToTemp()
{
	LOG_FUNC_ENTRY("CATModule2::CopyMapFileToTemp");
	// Return boolean
	bool bRet = true;
	// Map File to copy
	string sMapFile = GetMapFile();
	if ( !FileCopyToPath( sMapFile, m_sTempPath ) )
	{
		bRet = false;
		if ( !FileExists( sMapFile.c_str() ) )
		{
			// Add missing map file to error string.
			m_sErrors.append( "Missing map file: " );
			m_sErrors.append( sMapFile );
			m_sErrors.append( "\n" );
		}
		if ( !DirectoryExists( m_sTempPath.c_str() ) )
		{
			// Add missing temporary folder
			m_sErrors.append( "Missing folder: " );
			m_sErrors.append( m_sTempPath );
			m_sErrors.append( "\n" );
		}
	}
	return bRet;
}

bool CATModule2::CleanTemporaryDir()
{
	LOG_FUNC_ENTRY("CATModule2::CleanTemporaryDir");
	bool bRet = true;
	// Verify mmp
	if ( ! m_Mmp.VerifyAndRecover() )
		bRet = false;
	// Clean temporary dir
	vector<string> vFileList = DirList( m_sTempPath, false , true );
	vector<string>::iterator it = vFileList.begin();
	// Size of constant table
	int iCount = sizeof( TEMP_EXTENSION_NO_DELETE ) / sizeof( string );
	while ( it != vFileList.end() )
	{
		// Get extension and compare it to list
		bool bDelete = true;
		string sExtension = GetExtension( *it );
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
			if ( ! FileDelete( *it, true ) )
				bRet = false;
		}
		// Increment
		it++;
	}
	return bRet;
}

bool CATModule2::DeleteTemporaryDir()
{
	LOG_FUNC_ENTRY("CATModule2::DeleteTemporaryDir");
	bool bRet = true;
	// Verify mmp
	if ( ! m_Mmp.VerifyAndRecover() )
		bRet = false;
	// Delete temp dir
	if ( !DirDelete( m_sTempPath, true ) )
		bRet = false;
	return bRet;
}

bool CATModule2::IsUDEB() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::IsUDEB");
	// Determine from variant is this udeb
	if ( m_sVariantType.find( "udeb" ) != string::npos )
		return true;
	return false;
}
// ----------------------------------------------------------------------------
// Private AddressToLine related methods
// ----------------------------------------------------------------------------
bool CATModule2::InitializeAddressToLine()
{
	LOG_FUNC_ENTRY("CATModule2::InitializeAddressToLine");
	bool bRet = true;
	// Read in different way depending on platform
	if ( m_sVariantPlatform.compare("armv5") == 0 )
	{
		// Add static library lst files to source vector,
		// before reading them.
		vector<string> vFiles = DirList( AT_TEMP_LST_DIR, false, true );
		for(vector<string>::iterator it = vFiles.begin() ; it != vFiles.end() ; it ++ )
		{
			SOURCE source;
			source.bStatic = true;
			source.sLst = *it;
			source.sCpp = *it;
			source.sCpp = CATBase::RemovePathAndExt( source.sCpp, false );
			source.sCpp.append( ".cpp" );
			m_vSources.push_back( source );
		}

		if ( ! ReadListingFilesArmv5() )
			bRet = false;
		if ( ! ReadMapFileArmv5() )
			bRet = false;

		if ( bRet )
			m_bAddressToLineInitialized = true;
	}
	return bRet;
}


bool CATModule2::ReadListingFilesArmv5()
{
	LOG_FUNC_ENTRY("CATModule2::ReadListingFilesArmv5");
	char cTemp[MAX_LINE_LENGTH];
	vector<SOURCE>::iterator viFileIter = m_vSources.begin();
	int iNumberOfLstFiles = (int)m_vSources.size();
	vector<string> vTempLines;
	string sFileName;

	// Open all .lst files
	while( iNumberOfLstFiles > 0 )
	{
		// Make .lst file name
		sFileName.clear();
	
		// If lst file is not from static library make path to modules temporary directory.
		if ( viFileIter->bStatic != true )
		{
			// Remove path
			if( viFileIter->sLst.find("\\") != string::npos )
				sFileName.append(
				viFileIter->sLst.substr( viFileIter->sLst.find_last_of( "\\" ) + 1
				, viFileIter->sLst.size() ) );
			else
				sFileName.append( viFileIter->sLst );

			// Add temporary dir
			sFileName.insert( 0, m_sTempPath );
		}
		else
		{
			// Lst from static library don't change path.
			sFileName = viFileIter->sLst;
		}
		// Open lst file
		ifstream in( sFileName.c_str() );

		// If file can not be opened, try to open next file
		if( !in.good() )
		{
			viFileIter++;
			iNumberOfLstFiles--;
			continue;
		}

		string sTemp;
		// Clear temporary lines
		vTempLines.clear();
		// Add all lines to temp list
		do
		{
			in.getline( cTemp, MAX_LINE_LENGTH );
			sTemp.clear();
			sTemp.append( cTemp );
			vTempLines.push_back( sTemp );
		}
		while( in.good() );

		LINE_IN_FILE structLineInFile;
		
		bool bFindENDP = false;
		vector<string>::iterator viLinesIter = vTempLines.begin();

		// Loop throw all lines in .lst file
		while( viLinesIter != vTempLines.end() )
		{
			// Find ";;;"
			if( !bFindENDP && strstr(viLinesIter->c_str(), ";;;") != NULL )
			{
				bFindENDP = true;

				vector<string>::iterator viLineTempIter = viLinesIter;

				// Find top line of function definition
				while( viLineTempIter->size() > 0 )
				{
					viLineTempIter--;
				}
				viLineTempIter++;
				structLineInFile.sFunction.clear();
				structLineInFile.sFunction.append( viLineTempIter->c_str() );

				viLinesIter++;
				// Get Line
				sTemp.clear();
				sTemp.append( viLinesIter->c_str() );
				sTemp.erase(0,3);
				size_t iSize = sTemp.find_first_of(' ');
				if( iSize != string::npos )
					sTemp.resize(iSize);
				structLineInFile.iLine = atoi( sTemp.c_str() );

				structLineInFile.sFileName.clear();
				structLineInFile.sFileName.append( viFileIter->sCpp.c_str() );
				structLineInFile.sLstName = sFileName;
				m_vLineInFile.push_back( structLineInFile );
			}
			else if( strstr(viLinesIter->c_str(), "ENDP") != NULL )
				bFindENDP = false;
			viLinesIter++;
		}
		viFileIter++;
		iNumberOfLstFiles--;
	}
	if( m_vLineInFile.size() > 0 )
		return true;
	return false;
}

bool CATModule2::ReadMapFileArmv5()
{
	LOG_FUNC_ENTRY("CATModule2::ReadMapFileArmv5");
	// Map file name
	string sMapFileName	= GetMapFile();
	// Remove path
	if ( sMapFileName.find("\\") != string::npos )
		sMapFileName.erase(0, sMapFileName.find_last_of('\\')+1 );
	// Add temp path
	sMapFileName.insert(0, m_sTempPath );

	// Open .map file
	ifstream in( sMapFileName.c_str() );
	
	// File open ok?
	if( ! in.good() )
	{
		in.close();
		return false;
	}
	char cTemp[MAX_LINE_LENGTH];
	bool bFirstFuncFound = false;
	// Get all lines where is "Thumb"
	do
	{
		// Load one line from .map file
		in.getline( cTemp, MAX_LINE_LENGTH );
		// Find _E32Startup
		if( !bFirstFuncFound && ( strstr( cTemp, "_E32Startup" ) != NULL) )
		{
			bFirstFuncFound = true;
		}
		else if( !bFirstFuncFound && ( strstr( cTemp, "_E32Dll" ) != NULL) )
		{
			bFirstFuncFound = true;
		}
		else if( !bFirstFuncFound )
			// Skip if _E32Startup not found
			continue;

		if( strstr( cTemp, "Thumb Code" ) != NULL || strstr( cTemp, "ARM Code" ) != NULL)
		{
			MAP_FUNC_INFO structMapFileLineInfo;
			structMapFileLineInfo.sWholeLine.append( cTemp );

			// Get memory string address from line
			char* pStart = strstr( cTemp, "0x" );
			// Check did strstr return null.
			if( pStart == NULL )
				continue;
			char* pTemp = pStart;
			char TempString[MAX_LINE_LENGTH];
			TempString[0] = 0;
			size_t iLength = 0;
			while( *pTemp != ' ' )
			{
				TempString[iLength] = *pTemp;
				pTemp++;
				iLength++;
			}
			TempString[iLength] = 0;

			structMapFileLineInfo.iAddress = CATDatParser::_httoi( TempString );

			pTemp = cTemp;
			TempString[0] = 0;
			
			// Get function name

			// Skip spaces
			while( *pTemp == ' ' )
			{
				pTemp++;
			}
			iLength = 0;
			// Find end of function name
			string sTemp( pTemp );

			// Location of character ')'
			iLength = sTemp.find_first_of(')');

			// Location of character ' '
			size_t iLength2 = sTemp.find_first_of(' ');
			
			// If ')' character is the last char and
			// character ' ' is closer than ')' use location of ' '
			if( ( iLength + 1 ) == sTemp.length() && iLength2 < iLength )
				iLength = iLength2 - 1;
			
			if( iLength != string::npos )
				sTemp.resize( (iLength + 1) );

			structMapFileLineInfo.sFunctionName.append( sTemp.c_str() );

			bool bARM = false;
			// Find function length
			pStart = strstr( cTemp, "Thumb Code" );
			if( pStart == NULL )
			{
				pStart = strstr( cTemp, "ARM Code" );
				bARM = true;
			}
			if( pStart != NULL )
			{
				if( bARM )
					pStart += 8;
				else
					pStart += 10;
				while(*pStart == ' ')
				{
					pStart++;
				}
				sTemp.clear();
				sTemp.append( pStart );
				size_t iSize = sTemp.find_first_of(' ');
				if( iSize != string::npos )
					sTemp.resize( iSize );
			}

			structMapFileLineInfo.iFuncLength = atoi( sTemp.c_str() );
			if( bFirstFuncFound && structMapFileLineInfo.iFuncLength > 0 )
				// Save to list
				m_vMapFileFuncList.push_back( structMapFileLineInfo );
		}
	}
	while( in.good() );
	in.close();
	return true;
}

int CATModule2::FindLeakCodeLine( string& sFileName, int iLine, unsigned long iFromFuncAddress ) const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::FindLeakCodeLine");
	if ( sFileName.empty() )
		return -1;
	char cLineFromFile[MAX_LINE_LENGTH];
	vector<string> vTempLines;
	string sTemp;
	char* pTemp = NULL;
	char* pTempEnd = NULL;
	int iFoundLine = -1;
	int iRet = -1;
	
	// Open lst file
	ifstream in( sFileName.c_str() );

	bool bLineFound = false;
	bool bFirstAddressInFuncFound = false;
	unsigned long iFirstAddressInFunc = 0;
	while( in.good() )
	{
		in.getline( cLineFromFile, MAX_LINE_LENGTH );

		if( bLineFound )
		{
			vTempLines.push_back( cLineFromFile );
			// Is first character digit
			if( isdigit( cLineFromFile[0] ) )
			{
				if( !bFirstAddressInFuncFound )
				{
					bFirstAddressInFuncFound = true;
					sTemp.clear();
					sTemp.append( cLineFromFile );
					// Get value until next space
					sTemp.resize( sTemp.find_first_of(' ') );

					iFirstAddressInFunc = CATDatParser::_httoi( sTemp.c_str() );

					// Return function start line if margin 0
					if( iFromFuncAddress == 0 )
					{
						iRet = iLine;
						return iRet;
					}
				}
				else
				{
					// Find correct line using iFromFuncAddress variable
					sTemp.clear();
					sTemp.append( cLineFromFile );
					// Get value until next space
					sTemp.resize( sTemp.find_first_of(' ') );

					unsigned long iValue = CATDatParser::_httoi( sTemp.c_str() );

					if( ( iValue - iFirstAddressInFunc ) >= iFromFuncAddress )
					{
						// If there is data in function, code line can not be found
						if( strstr( cLineFromFile , "DCB" ) != NULL )
						{
							iRet = -1;
							return iRet;
						}
						pTemp = strstr( cLineFromFile, ";" );
						// Get line number
						bool bStringNumber = true;
						if( pTemp != NULL )
						{
							string sTempLine( pTemp + 1 );
							// Are all characters numbers?
							for( unsigned int i = 0 ; i < sTempLine .size() ; i++ )
							{
								if( !isdigit(sTempLine[i]) )
								{
									bStringNumber = false;
									break;
								}
							}
						}
						else
							bStringNumber = false;
						if( bStringNumber )
						{
							pTemp++;
							// Get line number
							iRet = atoi( pTemp );
						}
						else
						{
							vector<string>::iterator sTempIter = vTempLines.end();

							sTempIter--;

							// Find last code line
							while( sTempIter != vTempLines.begin() )
							{
								if( strstr( sTempIter->c_str() , "DCB" ) != NULL )
								{
									iRet = -1;
									return iRet;
								}
								if( strstr( sTempIter->c_str() , ";;;" ) == NULL )
									sTempIter--;
								else
									break;
							}
							if(sTempIter == vTempLines.begin() && strstr( sTempIter->c_str() , ";;;" ) == NULL)
							{
								iRet = -1;
								return iRet;
							}
							sTempIter->erase( 0, 3 );
							sTempIter->resize( sTempIter->find(' ') );

							// Leak line
							iRet = atoi( sTempIter->c_str() );
						}
						return iRet;
					}
				}
			}
		}
		else // Line in file not found
		{
			// Find line of function
			if( strstr( cLineFromFile, ";;;" ) != NULL )
			{
				pTemp = &cLineFromFile[0];
				// Skip characters ";;;"
				pTemp += 3;
				pTempEnd = pTemp;
				// Find end of line number
				while( *pTempEnd != ' ' )
				{
					pTempEnd++;
				}
				*pTempEnd = 0;
				iFoundLine = atoi( pTemp );
				*pTempEnd = ' ';
				if( iLine == iFoundLine )
				{
					bLineFound = true;
				}
			}
		}
	}
	return iRet;
}

bool CATModule2::IsMakeSuccessfull()
{
	LOG_FUNC_ENTRY("CATModule2::IsMakeSuccessfull");
	m_sErrors.clear();

	string sSearch;
	bool bMakeSuccess = true;
	
	// Lst files checked only with armv5 platform.
	if ( IsPlatformArmv5() )
	{
		sSearch.append( m_sTempPath );
		sSearch.append( "*.lst" );
		if( !SearchFileWithExtension( sSearch.c_str(), false, m_sErrors ) )
			bMakeSuccess = false;
		
		// Map
		sSearch.clear();
		sSearch.append( m_sTempPath );
		sSearch.append( "*.map" );
		if( !SearchFileWithExtension( sSearch.c_str(), false, m_sErrors ) )
			bMakeSuccess = false;
	}

	// .tmp
	sSearch.clear();
	sSearch.append( m_sTempPath );
	sSearch.append( "*.tmp" );
	if( !SearchFileWithExtension( sSearch.c_str(), false, m_sErrors ) )
		bMakeSuccess = false;

	return bMakeSuccess;
}

bool CATModule2::CreateBuildCompleteFile()
{
	LOG_FUNC_ENTRY("CATModule2::CreateBuildCompleteFile");
	// Don't create file if temp path not set cause might be anywhere
	if ( m_sTempPath.empty() )
		return false;
	// Create empty file indicating this module is build
	string sFile = m_sTempPath;
	if( sFile.at( sFile.length() - 1 ) != '\\' )
		sFile.append("\\");
	sFile.append( "BuildComplete" );
	ofstream out( sFile.c_str() );
	out << m_sVariantPlatform << endl;
	out << m_sVariantType << endl;
	out.close();
	return true;
}

bool CATModule2::ReadMakeFileFromTemp()
{
	LOG_FUNC_ENTRY("CATModule2::ReadMakeFileFromTemp");
	// Set makefile to point to temporary directory.
	string sMakeFile = m_sTempPath;
	sMakeFile.append( RemovePathAndExt( m_Mmp.m_sMmpFile, true ) );
	sMakeFile.append( "." );
	sMakeFile.append( AT_LEVEL_2_MAKEFILE_EXT );
	m_sMakeFile = sMakeFile;
	return ReadMakeFilePrivate();
}

bool CATModule2::ReadMakeFile()
{
	LOG_FUNC_ENTRY("CATModule2::ReadMakeFile");
	// Read makefile
	if ( ReadMakeFilePrivate() )
	{
		// Copy makefile to temporary directory.
		string sMakeFile = m_sTempPath;
		sMakeFile.append( RemovePathAndExt( m_Mmp.m_sMmpFile, true ) );
		sMakeFile.append( "." );
		sMakeFile.append( AT_LEVEL_2_MAKEFILE_EXT );
		FileCopyToPath( m_sMakeFile, sMakeFile );
		return true;
	}
	return false;
}

bool CATModule2::ReadMakeFilePrivate()
{
	LOG_FUNC_ENTRY("CATModule2::ReadMakeFilePrivate");

	if ( m_sMakeFile.empty() )
		return false;

	LOG_STRING( "using makefile :" << m_sMakeFile );

	// Stream object to read files
	ifstream in;
	// Char array to read line from file
	char cLine[MAX_LINE_LENGTH];
	// String to use as buffer from file
	string sLine;
	// Open file
	in.open( m_sMakeFile.c_str(), ios_base::in );
	// Check that its open
	if ( ! in.good() )
	{
		// Cannot open file
		cout << AT_MSG << "Error, can not open file: " << m_sMakeFile << endl;
		return false;
	}
	// Check is it wrapper makefile (starts with "%:")
	in.getline( cLine, MAX_LINE_LENGTH );
	if ( cLine[0] == '%' && cLine[1] == ':' )
	{
		LOG_STRING("Found wrapper makefile");
		in.close();
		// Use ".default" makefile
		string sDefaultMakeFile = m_sMakeFile.substr( 0, m_sMakeFile.find_last_of( "." ) );
		sDefaultMakeFile.append( ".DEFAULT" );
		LOG_STRING( "using makefile :" << m_sMakeFile );
		// Does default exists. If not we need to run "wrapper make"
		if ( ! FileExists( sDefaultMakeFile.c_str() ) )
		{
			// Run the wrapper make to create "real" makefile
			string sMakeFileCmd;
			sMakeFileCmd.append("make -f \"");
			sMakeFileCmd.append( m_sMakeFile );
			sMakeFileCmd.append( "\"" );
			LOG_STRING( "using makefile :" << m_sMakeFile );
			cout << AT_MSG_SYSTEM_CALL << sMakeFileCmd << endl;
			int iRet = (int)system( sMakeFileCmd.c_str() );
			if ( iRet )
			{
				cout << MAKE_ERROR;
				return false;
			}
		}
		m_sMakeFile = sDefaultMakeFile;
		// Open new file
		in.open( m_sMakeFile.c_str(), ios_base::in );
		// Check that it's open
		if ( ! in.good() )
		{
			// Cannot open file
			cout << AT_MSG << "Error, can not open makefile: " << m_sMakeFile << endl;
			return false;
		}
	}
	in.seekg( ios_base::beg );

	// Number of lines to read at max for basic module information.
	int iReadLineCount = 20;
	// Extension from target line. to be compared with targettype.
	string sTargetExtension;
	// Read line at a time. Loop until we find it or eof
	do {
		// Read line from file to array
		in.getline( cLine, MAX_LINE_LENGTH );
		iReadLineCount--;

		sLine.clear();
		// Put that to string
		sLine.append( cLine );
		// Search target
		if ( sLine.find( MAKEFILE_TARGET_STRING ) != string::npos )
		{
			// Found it. Now remove other than type from line
			sLine.erase( 0, strlen( MAKEFILE_TARGET_STRING ) );
			ChangeToLower( sLine );
			sTargetExtension.clear();
			sTargetExtension = GetExtension( sLine );
			m_sTarget = RemovePathAndExt( sLine, true);
			LOG_STRING("found target: " << sLine );
		}
		// Search targettype
		else if ( sLine.find( MAKEFILE_TARGETTYPE_STRING ) != string::npos )
		{
			// Found it. Now remove other than type from line
			sLine.erase( 0, strlen( MAKEFILE_TARGETTYPE_STRING ) );
			ChangeToLower( sLine );
			m_sTargetType = sLine;
			LOG_STRING("found target type: " << m_sTargetType );
		}
		else if ( sLine.find( MAKEFILE_BASIC_TARGETTYPE_STRING ) != string::npos )
		{
			sLine.erase( 0, strlen( MAKEFILE_BASIC_TARGETTYPE_STRING ) );
			ChangeToLower( sLine );
			m_sRequestedTargetExt = sLine;
			// Compare with the extension in target line if not same use target lines if its "valid".
			if ( m_sRequestedTargetExt.compare( sTargetExtension ) != 0  && sTargetExtension.size() > 0 )
				m_sRequestedTargetExt = sTargetExtension;
			LOG_STRING("found requested target extension: " << m_sTargetType );		
		}
		// Feature variant details
		else if ( sLine.find( MAKEFILE_FEATURE_VARIANT_NAME ) != string::npos )
		{
			sLine.erase( 0, strlen( MAKEFILE_FEATURE_VARIANT_NAME ) );
			m_sFeatureVariantName = sLine;
			LOG_STRING("found feature variant name: " << sLine );
		}
		else if ( sLine.find( MAKEFILE_FEATURE_VARIANT_UREL_LABEL ) != string::npos )
		{
			sLine.erase( 0, strlen( MAKEFILE_FEATURE_VARIANT_UREL_LABEL ) );
			LOG_STRING("found feature variant urel label: " << sLine );
			if ( sLine.compare("INVARIANT") != 0 )
				m_sFeatureVariantURELLabel = sLine;
		}
		else if ( sLine.find( MAKEFILE_FEATURE_VARIANT_UDEB_LABEL ) != string::npos )
		{
			sLine.erase( 0, strlen( MAKEFILE_FEATURE_VARIANT_UDEB_LABEL ) );
			LOG_STRING("found feature variant udeb label: " << sLine );
			if ( sLine.compare("INVARIANT") != 0 )
				m_sFeatureVariantUDEBLabel = sLine;
		}
	} while( in.good() && iReadLineCount > 0 );

	// Search compile definitions
	// CWDEFS CCDEFS ARMCCDEFS
	do
	{
		in.getline( cLine, MAX_LINE_LENGTH );
		sLine.clear();
		sLine.append( cLine );
		if ( sLine.substr( 0 , 6 ).compare( string("CWDEFS") ) == 0 
			|| sLine.substr( 0 , 6 ).compare( string("CCDEFS") ) == 0 )
		{
			sLine.erase( 0, 8 );
			m_sCompileDefinitions = sLine;
			break;
		}
		else if( sLine.substr( 0 , 9 ).compare( string("ARMCCDEFS") ) == 0  )
		{
			sLine.erase( 0, 11 );
			m_sCompileDefinitions = sLine;
			break;
		}
	} while( in.good() );
	// Move reading back to start if we could not find compile flags.
	in.seekg( ios_base::beg );

	// Search listing information (modules source files).
	int iFindItem = 1; //1 = Source, 2 = LISTINGUDEB/UREL, 3 = lst file
	string sCdefs;
	string sSource;
	string sLst;
	do
	{
		in.getline( cLine, MAX_LINE_LENGTH );
		sLine.clear();
		sLine.append( cLine );

		switch( iFindItem )
		{
			case 1:
				if( sLine.find( "# Source " ) != string::npos )
				{
					iFindItem = 2;
					// Remove text "# Source "
					sLine.erase( 0, 9 );
					sSource = sLine;
				}
			break;
			case 2:
				if( IsUDEB() )
				{
					if( sLine.find( "LISTINGUDEB" ) != string::npos )
					{
						iFindItem = 3;
					}
				}
				else
				{
					if( sLine.find( "LISTINGUREL" ) != string::npos )
					{
						iFindItem = 3;
					}
				}
			break;
			case 3:
				if( sLine.find( "perl -S ecopyfile.pl" ) != string::npos )
				{
					// Save lst file to list
					sLine.erase( 0, ( sLine.find_first_of( "\\" ) ) );
					// remove last char if '"'
					if ( sLine.at( sLine.size()-1 ) == '"' )
						sLine.erase( sLine.size()-1, sLine.size() );
					sLst = sLine;
					AddSource( sSource, sLst );
					iFindItem = 1;
					sSource.clear(); sLst.clear();
					
				}
			break;
		}
	}
	while( in.good() );
	// close and return
	in.close();
	return true;
}

// ----------------------------------------------------------------------------
// Get & Sets
// ----------------------------------------------------------------------------
string CATModule2::GetErrors() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetErrors");
	return m_sErrors;
}

string CATModule2::GetS60FileName() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetS60FileName");
	return m_sS60FileName;
}

void CATModule2::SetS60FileName( const string& aFileName  )
{
	LOG_LOW_FUNC_ENTRY("CATModule2::SetS60FileName");

	m_sS60FileName = aFileName;

	//check new m_sS60FileName and change it if needed
	string sProcessName = "";
	sProcessName.append( m_sTarget );
	sProcessName.append(".");
	sProcessName.append( m_sTargetType );
		
	if ( m_sS60FileName.empty() )
	{
		m_sS60FileName = sProcessName;
		m_sS60FileName.append(".dat");
	}
	else
	{
		// if data file name contains %processname% string, replace it with process name
		string sProcessnameTemp = "[";
		sProcessnameTemp.append( sProcessName );
		sProcessnameTemp.append( "]" );
		size_t iSpot;

		//create temp name in lowercase
		string sS60FileNameLower = m_sS60FileName;
		ChangeToLower( sS60FileNameLower );

		// find %processname% string in lowercase name, replace it with process name in in m_sS60FileName
		// replace it also in temp string (matching indexes)
		while( ( iSpot = sS60FileNameLower.find( AT_PROCESSNAME_TAG ) ) != string::npos )
		{
			m_sS60FileName.replace( iSpot, AT_PROCESSNAME_TAG.length(), sProcessnameTemp );
			sS60FileNameLower.replace( iSpot, AT_PROCESSNAME_TAG.length(), sProcessnameTemp );
		}
    }
}

string CATModule2::GetLstNameOfSource(string sSource) const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetLstNameOfSource");
	// Find . before xtension
	size_t iSpot = sSource.find_last_of( "." );
	// Get sub string to there
	string sLst = sSource.substr(0, iSpot+1);
	if ( m_sVariantPlatform.compare( "winscw" ) != 0 )
	{
		// Add variant platform (i.e. armv5)
		sLst.append( m_sVariantPlatform );
		sLst.append( "." );
		// Add variant type (i.e. build type liek urel)
		sLst.append( m_sVariantType );
		sLst.append( "." );
		// Add target binary name
		sLst.append( m_sTarget );
		sLst.append( "." );
		// Add target requested binary extension
		sLst.append( m_sRequestedTargetExt );
		sLst.append( "." );
		// Add lst extension
		sLst.append( "lst" );
	}
	else
	{
		sLst.append( "WINSCW.lst" );
	}
	return sLst;
}

bool CATModule2::IsPlatformArmv5() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::IsPlatformArmv5");
	if ( _stricmp( m_sVariantPlatform.c_str(), "armv5" ) == 0 )
		return true;
	return false;
}

string CATModule2::GetMapFile() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetMapFile");
	// Map file with path using variables
	string sMapFile( m_sReleasePath );
	if ( ! sMapFile.empty() )
		sMapFile.append( "\\" );
	sMapFile.append( m_sFullVariantPath );
	if ( ! m_sFullVariantPath.empty() )
		sMapFile.append( "\\" );
	sMapFile.append( m_sTarget );
	sMapFile.append( "." );
	// Possible feature variant. Note debug might not be defined
	// when release has got one.
	if ( IsUDEB() && !m_sFeatureVariantUDEBLabel.empty() )
	{
		sMapFile.append( m_sFeatureVariantUDEBLabel );
		sMapFile.append( "." );
	}

	if ( !IsUDEB() && !m_sFeatureVariantURELLabel.empty() )
	{
		sMapFile.append( m_sFeatureVariantURELLabel );
		sMapFile.append( "." );
	}
	sMapFile.append( m_sRequestedTargetExt );
	sMapFile.append( ".map" );
	return sMapFile;
}

string CATModule2::GetSymbolFile() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetSymbolFile");
	// Symbol file with path using variables
	string sSymbolFile( m_sReleasePath );
	sSymbolFile.append( "\\" );
	sSymbolFile.append( m_sFullVariantPath );
	sSymbolFile.append( "\\" );
	sSymbolFile.append( m_sTarget );
	sSymbolFile.append( "." );
	// Possible feature variant.
	if ( ! m_sFeatureVariantUDEBLabel.empty() || ! m_sFeatureVariantURELLabel.empty() )
	{
		if ( IsUDEB() )
			sSymbolFile.append( m_sFeatureVariantUDEBLabel );
		else
			sSymbolFile.append( m_sFeatureVariantURELLabel );
		sSymbolFile.append( "." );
	}
	
	if ( m_eBuildSystem == CATProject::SBS_V1 )
	{
		sSymbolFile.append( "sym" );
		return sSymbolFile;
	}
	sSymbolFile.append( m_sRequestedTargetExt );
	sSymbolFile.append( ".sym" );
	return sSymbolFile;
}

string CATModule2::GetBinaryFile() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetBinaryFile");
	// Binary file with path using variables
	string sBinaryFile( m_sReleasePath );
	if ( ! sBinaryFile.empty() )
		sBinaryFile.append( "\\" );
	sBinaryFile.append( m_sFullVariantPath );
	if ( ! m_sFullVariantPath.empty() )
		sBinaryFile.append( "\\" );
	sBinaryFile.append( m_sTarget );
	sBinaryFile.append( "." );
	// Possible feature variant.
	if ( ! m_sFeatureVariantUDEBLabel.empty() || ! m_sFeatureVariantURELLabel.empty() )
	{
		if ( IsUDEB() )
			sBinaryFile.append( m_sFeatureVariantUDEBLabel );
		else
			sBinaryFile.append( m_sFeatureVariantURELLabel );
		sBinaryFile.append( "." );
	}
	sBinaryFile.append( m_sRequestedTargetExt );
	return sBinaryFile;
}

bool CATModule2::SetMmpFile(const string& sMmpFile)
{
	LOG_FUNC_ENTRY("CATModule2::SetMmpFile");
	// Set mmp file
	m_Mmp.m_sMmpFile = sMmpFile;
	// Change to lower
	ChangeToLower( m_Mmp.m_sMmpFile );
	// Convert
	ConvertUnixPathToWin( m_Mmp.m_sMmpFile );
	// Set the temporary path.
	m_sTempPath.clear();
	m_sTempPath = CreateTempPath( m_Mmp.m_sMmpFile );
	return true;
}

bool CATModule2::CreateTemporaryDirectory()
{
	LOG_FUNC_ENTRY("CATModule2::CreateTemporaryDirectory");
	if ( m_sTempPath.empty() )
	{
		LOG_STRING("Temporary path is not set.");
		return false;
	}
	// Create temp dir if not exists
	if ( ! DirectoryExists( m_sTempPath.c_str() ) )
	{
		if ( !CreateDirectory( m_sTempPath.c_str(), NULL ) )
		{
			cout << AT_MSG << "Error, can not create directory: "
				<< m_sTempPath << endl;
			return false;
		}
		cout << AT_MSG << "Directory created: " << m_sTempPath << endl;
	}
	return true;
}

void CATModule2::SetMakeFile( const string& sMakeFile )
{
	LOG_FUNC_ENTRY("CATModule2::SetMakeFile");
	m_sMakeFile = sMakeFile;
}
string CATModule2::GetMakeFile() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetMakeFile");
	return m_sMakeFile;
}
string CATModule2::GetMmpFile() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetMmpFile");
	return m_Mmp.m_sMmpFile;
}
string CATModule2::GetTempPath() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetTempPath");
	return m_sTempPath;
}
void CATModule2::SetTarget(const string& sTarget)
{
	LOG_FUNC_ENTRY("CATModule2::SetTarget");
	m_sTarget = sTarget;
	ChangeToLower( m_sTarget );
}
string CATModule2::GetTarget() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetTarget");
	return m_sTarget;
}
string CATModule2::GetBinaryName() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetBinaryName");
	string sBinaryName;
	sBinaryName.append( m_sTarget );
	sBinaryName.append( "." );
	sBinaryName.append( m_sRequestedTargetExt );
	return sBinaryName;
}

void CATModule2::SetTargetType(const string& sTargetType)
{
	LOG_FUNC_ENTRY("CATModule2::SetTargetType");
	m_sTargetType = sTargetType;
	ChangeToLower( m_sTargetType );
}
string CATModule2::GetTargetType() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetTargetType");
	return m_sTargetType;
}
void CATModule2::SetRequestedTargetExt( const string& sRequestedTargetExt )
{
	LOG_FUNC_ENTRY("CATModule2::SetRequestedTargetExt");
	m_sRequestedTargetExt = sRequestedTargetExt;
	ChangeToLower( m_sRequestedTargetExt );
}

string CATModule2::GetRequestedTargetExt() const
{
	LOG_LOW_FUNC_ENTRY("CATmodule2::GetRequestedTargetExt");
	return m_sRequestedTargetExt;
}

void CATModule2::SetVariantPlatform(const string& sVariantPlatform)
{
	LOG_FUNC_ENTRY("CATModule2::SetVariantPlatform");
	m_sVariantPlatform = sVariantPlatform;
	ChangeToLower( m_sVariantPlatform );
}
string CATModule2::GetVariantPlatform() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetVariantPlatform");
	return m_sVariantPlatform;
}
void CATModule2::SetVariantType(const string& sVariantType)
{
	LOG_FUNC_ENTRY("CATModule2::SetVariantType");
	m_sVariantType = sVariantType;
	ChangeToLower( m_sVariantType );
}
string CATModule2::GetVariantType() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetVariantType");
	return m_sVariantType;
}
void CATModule2::SetFeatureVariant(const string& sFeatureVariant)
{
	LOG_FUNC_ENTRY("CATModule2::SetFeatureVariant");
	m_sFeatureVariant = sFeatureVariant;
	ChangeToLower( m_sFeatureVariant );
}
string CATModule2::GetFeatureVariant() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetFeatureVariant");
	return m_sFeatureVariant;
}
void CATModule2::SetFeatureVariantName(const string& sFeatureVariantName)
{
	LOG_FUNC_ENTRY("CATModule2::SetFeatureVariantName");
	m_sFeatureVariantName = sFeatureVariantName;
	ChangeToLower( m_sFeatureVariantName );
}
string CATModule2::GetFeatureVariantName() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetFeatureVariantName");
	return m_sFeatureVariantName;
}
void CATModule2::SetReleasePath(const string& sReleasePath)
{
	LOG_FUNC_ENTRY("CATModule2::SetReleasePath");
	m_sReleasePath = sReleasePath;
	ChangeToLower( m_sReleasePath );
	ConvertUnixPathToWin( m_sReleasePath );

}
string CATModule2::GetReleasePath() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetReleasePath");
	return m_sReleasePath;
}
void CATModule2::SetFullVariantPath(const string& sFullVariantPath)
{
	LOG_FUNC_ENTRY("CATModule2::SetFullVariantPath");
	m_sFullVariantPath = sFullVariantPath;
	ChangeToLower( m_sFullVariantPath );
	ConvertUnixPathToWin( m_sFullVariantPath );
}
string CATModule2::GetFullVariantPath() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetFullVariantPath");
	return m_sFullVariantPath;
}
string CATModule2::GetUniqueId() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetUniqueId");
	return FilterString( m_sTarget );
}
void CATModule2::SetBuildSystem( int eBuildSystem )
{
	LOG_FUNC_ENTRY("CATModule2::SetBuildSystem");
	m_eBuildSystem = eBuildSystem;
}

int CATModule2::GetBuildSystem() const
{
	LOG_LOW_FUNC_ENTRY("CATModule2::GetBuildSystem");
	return m_eBuildSystem;
}

void CATModule2::SetCompileDefinitions( const string& sCompileDefinitions )
{
	LOG_LOW_FUNC_ENTRY( "CATModule2::SetCompileDefinitions" );
	m_sCompileDefinitions = sCompileDefinitions;
}

string CATModule2::GetCompileDefinitions() const
{
	LOG_LOW_FUNC_ENTRY( "CATModule2::GetCompileDefinitions" );
	return m_sCompileDefinitions;
}

void CATModule2::SetCompileInfoText( string sCompileInfoText )
{
	LOG_LOW_FUNC_ENTRY( "CATModule2::SetCompileInfoText" );
	m_sCompileInfoText = sCompileInfoText;
}
string CATModule2::GetCompileInfoText() const
{
	LOG_LOW_FUNC_ENTRY( "CATModule2::GetCompileInfoText" );
	return m_sCompileInfoText;
}
//EOF
