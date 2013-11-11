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
* Description:  Reads rom symbol file and provides interface to acquire
*               binary and function information using memory addresss.
*
*/

#include "../inc/ATCommonDefines.h"
#include "../inc/catromsymbol.h"
#include "../inc/catfilereader.h"
#include "../inc/CATBase.h"
#include "../inc/CATMemoryAddress.h"

// -----------------------------------------------------------------------------
// RofsBinary::RofsBinary
// Default construcor
// -----------------------------------------------------------------------------
RofsBinary::RofsBinary()
{
	LOG_LOW_FUNC_ENTRY("RofsBinary::RofsBinary");
	m_sBinary = "";
	vSymbols.clear();
}

// -----------------------------------------------------------------------------
// RofsBinary::RofsBinary
// Construcor
// -----------------------------------------------------------------------------
RofsBinary::RofsBinary( const string& sBinary )
{
	LOG_LOW_FUNC_ENTRY("RofsBinary::RofsBinary");
	m_sBinary = sBinary;
	vSymbols.clear();
}

// -----------------------------------------------------------------------------
// RofsBinary::~RofsBinary
// Destructor
// -----------------------------------------------------------------------------
RofsBinary::~RofsBinary()
{
	LOG_LOW_FUNC_ENTRY("RofsBinary::~RofsBinary");
	for ( vector<Symbol*>::iterator it = vSymbols.begin() ; it != vSymbols.end() ; it++ )
		delete *it;
	vSymbols.clear();
}

// -----------------------------------------------------------------------------
// CATRomSymbol::CATRomSymbol
// Constructor.
// -----------------------------------------------------------------------------
CATRomSymbol::CATRomSymbol()
{
	LOG_FUNC_ENTRY("CATRomSymbol::CATRomSymbol");
	m_bSymbolsRead = false;
	m_iRomEndAddress = 0;
	m_iRomStartAddress = 0;
	m_vRomFiles.clear();
	m_sErrorMessage = "";
	m_vRomCache.clear();
	m_vRomSymbols.clear();
	m_bShowProgressMessages = false;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::~CATRomSymbol
// Destructor.
// -----------------------------------------------------------------------------
CATRomSymbol::~CATRomSymbol()
{
	LOG_FUNC_ENTRY("CATRomSymbol::~CATRomSymbol");
	// Rom
	for ( vector<Symbol*>::iterator it = m_vRomSymbols.begin() ; it != m_vRomSymbols.end() ; it++ )
	{
		delete *it;
	}
	m_vRomSymbols.clear();
	// Rofs
	for ( vector<RofsBinary*>::iterator it = m_vRofsBinaries.begin() ; it != m_vRofsBinaries.end() ; it++ )
	{
		delete *it;
	}
	m_vRofsBinaries.clear();
}

// -----------------------------------------------------------------------------
// CATRomSymbol::Open
// This funtion should not be used anymore since
// we support multiple rom/rofs files.
// -----------------------------------------------------------------------------
bool CATRomSymbol::Open( const string& /*sString*/, const unsigned long /*iLong*/)
{
	LOG_FUNC_ENTRY("CATRomSymbol::Open");
	return false;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::SetSymbols
// Set symbol file(s) to be used.
// This also checks that files exists and identifies them as rom/rofs.
// -----------------------------------------------------------------------------
bool CATRomSymbol::SetSymbols( const vector<string>& vSymbols )
{
	LOG_FUNC_ENTRY("CATRomSymbol::SetSymbols");
	bool ok = true;
	// Check no same symbol defined twice.
	for( vector<string>::const_iterator it = vSymbols.begin() ;
		it != vSymbols.end(); it++ )
	{
		for( vector<string>::const_iterator it2 = vSymbols.begin() ;
			it2 != vSymbols.end(); it2++ )
		{
			if ( it == it2 )
				continue;
			if ( _stricmp( (*it).c_str(), (*it2).c_str() ) == 0 )
			{
				m_sErrorMessage.append( "Same symbol file defined twice (" );
				m_sErrorMessage.append( (*it) );
				m_sErrorMessage.append( ")\n" );
				return false;
			}
		}
	}
	// Loop given symbol files.
	for( vector<string>::const_iterator it = vSymbols.begin() ;
		it != vSymbols.end(); it++ )
	{
		// Symbol file exists?
		if ( ! CATBase::FileExists( (*it).c_str() ) )
		{
			ok = false;
			m_sErrorMessage.append( "Symbol file does not exists (" );
			m_sErrorMessage.append( *it );
			m_sErrorMessage.append( ").\n");
			continue;
		}
		// Identify symbol file.
		int type = IdentifySymbolFile( *it );
		// Depending on type move it correct vector.
		switch( type )
		{
		case SYMBOL_FILE_INVALID:
			ok = false;
			m_sErrorMessage.append( "Symbol file with invalid content (" );
			m_sErrorMessage.append( *it );
			m_sErrorMessage.append( ").\n");
			break;
		case SYMBOL_FILE_ROM:
			m_vRomFiles.push_back( *it );
			break;
		case SYMBOL_FILE_ROFS:
			m_vRofsFiles.push_back( *it );
			break;
		default:
			ok = false;
			LOG_STRING("IdentifySymbolFile returned unknown type.");
			break;
		}
	}
	if ( ok )
	{
		// Read symbols.
		if ( m_vRomFiles.size() > 0 )
		{
			if ( ! ReadRomFiles() )
				ok = false;
			else
				m_bSymbolsRead = true;
		}
		if ( m_vRofsFiles.size() > 0 )
		{
			if ( ! ReadRofsFiles() )
				ok = false;
			else
				m_bSymbolsRead = true;
		}
	}
	return ok;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::IdentifySymbolFile
// Identify given file is it rom / rofs.
// -----------------------------------------------------------------------------
int CATRomSymbol::IdentifySymbolFile( const string& sFile )
{
	LOG_FUNC_ENTRY("CATRomSymbol::IdentifySymbolFile");
	// Set type as invalid.
	int iType = SYMBOL_FILE_INVALID;
	// Line counter.
	int iLineCount = 0;
	// Minimun line length to identify it.
	size_t iLineMinLength = MAX_LINE_LENGTH;
	if ( ROFS_SYMBOL_IDENTIFY_STRING.length() > ROM_SYMBOL_IDENTIFY_STRING.length() )
		iLineMinLength = ROFS_SYMBOL_IDENTIFY_STRING.length();
	else
		iLineMinLength = ROM_SYMBOL_IDENTIFY_STRING.length();
	try {
		ifstream in;
		in.open( sFile.c_str(), ios::in );
		if ( ! in.good() )
			return SYMBOL_FILE_INVALID;
		char cLine[MAX_LINE_LENGTH];
		do {
			// Dont read too many lines. (File might be contain invalid data).
			iLineCount++;
			if ( iLineCount > IDENTIFY_MAX_LINES_READ )
				break;

			// Get line -> string.
			in.getline( cLine, MAX_LINE_LENGTH );
			string sLine(cLine);
			
			// Check its not too short.
			if( sLine.length() < iLineMinLength )
				continue;

			// Take substring from start of line to identify it to rofs/rom.
			if ( ! sLine.substr( 0, ROFS_SYMBOL_IDENTIFY_STRING.length() ).compare( ROFS_SYMBOL_IDENTIFY_STRING ) )
			{
				iType = SYMBOL_FILE_ROFS;
				break;
			}
			else if ( ! sLine.substr( 0, ROM_SYMBOL_IDENTIFY_STRING.length() ).compare( ROM_SYMBOL_IDENTIFY_STRING ) )
			{
				iType = SYMBOL_FILE_ROM;
				break;
			}
		} while ( in.good() );
		in.close();
	}
	catch(...)
	{
		LOG_STRING("CATRomSymbol::IdentifySymbolFile unhandled exception.");
	}
	return iType;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::ReadRomFiles
// Reads rom file(s) and creates symbols to vector.
// -----------------------------------------------------------------------------
bool CATRomSymbol::ReadRomFiles()
{
	LOG_FUNC_ENTRY("CATRomSymbol::ReadRomFile");

	// Clear symbols.
	for ( vector<Symbol*>::iterator it = m_vRomSymbols.begin() ; it != m_vRomSymbols.end() ; it++ )
	{
		delete *it;
	}
	m_vRomSymbols.clear();
	
	// Clear cache. note cache is just pointers dont delete them.
	m_vRomCache.clear();

	// Any errors?
	bool ok = true;

	for( vector<string>::iterator it = m_vRomFiles.begin();
		it != m_vRomFiles.end() ; it++ )
		ok = ReadRomFile( *it );
	
	// If size smaller than 1 it is not good rom file(s).
	if ( m_vRomSymbols.size() < 1  || ok != true)
		return false;

	// Rom start and end addresses.
	m_iRomStartAddress = (*m_vRomSymbols.begin())->iStartAddress;
	m_iRomEndAddress = (*m_vRomSymbols.rbegin())->iEndAddress;
	return true;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::ReadRofsFiles
// Read rofs files.
// -----------------------------------------------------------------------------
bool CATRomSymbol::ReadRofsFiles()
{
	LOG_FUNC_ENTRY("CATRomSymbol::ReadRofsFiles");
	// Clear.
	for ( vector<RofsBinary*>::iterator it = m_vRofsBinaries.begin() ; it != m_vRofsBinaries.end() ; it++ )
	{
		delete *it;
	}
	m_vRofsBinaries.clear();
	
	// Any errors?
	bool ok = true;

	for( vector<string>::iterator it = m_vRofsFiles.begin();
		it != m_vRofsFiles.end() ; it++ )
		ok = ReadRofsFile( *it );
	
	// If size smaller than 1 it is not good rofs file(s).
	if ( m_vRofsBinaries.size() < 1  || ok != true)
		return false;
	return true;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::ReadRomFile
// Read given rom file
// -----------------------------------------------------------------------------
bool CATRomSymbol::ReadRomFile( const string& sFile )
{
	LOG_FUNC_ENTRY("CATRomSymbol::ReadRomfile");
	// Open rom file.
	CATFileReader* reader = new CATFileReader();
	// Show progress message if flag set.
	if ( m_bShowProgressMessages )
		cout << AT_MSG << "Reading rom symbol file: " << sFile << "..." << endl;
	if ( ! reader->Open( sFile.c_str() ) )
	{
		reader->Close();
		delete reader;
		return false;
	}

	// Show progress message if flag set.
	if ( m_bShowProgressMessages )
		cout << AT_MSG << "Parsing rom symbol file: " << sFile << "..." << endl;

	// Loop thrue lines.
	string sLine("");
	string sBinary("");
	while( reader->GetLine( sLine ) )
	{
		// From rom we just read symbols that have lenght, no need to separate them into diff binaries.
		try {
			if ( sLine.size() < 2 )
			{
				continue;
			}
			else if ( sLine.at(0) == '8' )
			{
				// Create new item.
				Symbol* symbol = new Symbol();
				ParseSymbolFromLine( sLine, symbol);
				// Ignore symbols which have same start & end address (zero length).
				if ( symbol->iStartAddress != symbol->iEndAddress )
					m_vRomSymbols.push_back( symbol );
				else
					delete symbol;
			}
		} catch(...)
		{
			// Catch all possible exception here so analyze will succeed even rom file invalid.
			m_sErrorMessage.append( "Unhandled exception parsing rom symbol file.\n" );
			// Close and delete reader.
			reader->Close();
			delete reader;
			return false;
		}
	}
	// Close and delete reader.
	reader->Close();
	delete reader;
	return true;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::ReadRofsFile
// Read given rofs file
// -----------------------------------------------------------------------------
bool CATRomSymbol::ReadRofsFile( const string& sFile )
{
	LOG_FUNC_ENTRY("CATRomSymbol::ReadRofsFile");
	// open/read rofs file.
	CATFileReader* reader = new CATFileReader();
	// Show progress message if flag set.
	if ( m_bShowProgressMessages )
		cout << AT_MSG << "Reading rofs symbol file: " << sFile << "..." << endl;
	if ( ! reader->Open( sFile.c_str() ) )
	{
		reader->Close();
		delete reader;
		return false;
	}

	// Show progress message if flag set.
	if ( m_bShowProgressMessages )
		cout << AT_MSG << "Parsing rofs symbol file: " << sFile << "..." << endl;

	// Loop thrue lines.
	string sLine("");
	string sBinary("");
	RofsBinary* rb = NULL;
	while( reader->GetLine( sLine ) )
	{
		try {
			if ( sLine.size() < 2 )
			{
				continue;
			}
			else if ( sLine.at(0) == 'F' )
			{
				if ( rb != NULL )
				{
					// Check last binary if no symbols in it dont add it.
					if ( rb->vSymbols.size() == 0 )
					{
						delete rb;
						rb = NULL;
					}
					else
						m_vRofsBinaries.push_back( rb );
				}
				// new binary name.
				size_t i = sLine.rfind("\\");
				sLine.erase(0, i+1);
				rb = new RofsBinary( sLine );

			}
			else if ( sLine.at(0) == '0' )
			{
				// Cannot pickup symbols if no binary defined.
				if ( rb == NULL )
					continue;
				// Create new item.
				Symbol* symbol = new Symbol();
				ParseSymbolFromLine( sLine, symbol);
				// Ignore symbols which have same start & end address (zero length).
				if ( symbol->iStartAddress != symbol->iEndAddress )
					rb->vSymbols.push_back( symbol );
				else
					delete symbol;
			}
		} catch(...)
		{
			// Catch all possible exception here so analyze will succeed even rofs file invalid.
			m_sErrorMessage.append( "Unhandled exception parsing rofs symbol file.\n" );
			// Close and delete reader.
			reader->Close();
			delete reader;
			return false;
		}
	}
	// Last added binary.
	if ( rb != NULL )
	{
		if ( rb->vSymbols.size() == 0 )
		{
			delete rb;
			rb = NULL;
		}
		else
			m_vRofsBinaries.push_back( rb );
	}
	// Close and delete reader.
	reader->Close();
	delete reader;
	return true;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::ParseSymbolFromLine
// Parses given line into given symbol.
// -----------------------------------------------------------------------------
void CATRomSymbol::ParseSymbolFromLine( const string& sLine, Symbol* pSymbol )
{
	LOG_LOW_FUNC_ENTRY("CATRomSymbol::ParseSymbolFromLine");
	if ( pSymbol == NULL )
		return;
	size_t s,x;
	string temp;
	// address.
	x = sLine.find( ' ' );
	temp = sLine.substr( 0, x );
	pSymbol->iStartAddress = CATBase::_httoi( temp.c_str() );
	// "Erase spaces" move starting point.
	s = x;
	s = sLine.find_first_not_of( ' ', s );
	// length.
	x = sLine.find( ' ', s );
	temp = sLine.substr(s,x-s);
	unsigned long length = CATBase::_httoi( temp.c_str() );
	pSymbol->iEndAddress = pSymbol->iStartAddress + length;
	// "Erase spaces" move starting point.
	s = x;
	s = sLine.find_first_not_of( ' ', s);
	// function. Function might have spaces so we find 2 spaces which indicates end of it.
	x = sLine.find( "  ", s );
	temp = sLine.substr( s, x-s );
	pSymbol->sFunction = temp;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::GetError
// Get error string if error occured in other methods.
// -----------------------------------------------------------------------------
string CATRomSymbol::GetError( void )
{
	LOG_FUNC_ENTRY("CATRomSymbol::GetError");
	return m_sErrorMessage;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::Close
// Close (stop using).
// -----------------------------------------------------------------------------
bool CATRomSymbol::Close( void )
{
	LOG_FUNC_ENTRY("CATRomSymbol::Close");
	return true;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::AddressToLine
// Try locate binary and function name for given memory address.
// -----------------------------------------------------------------------------
bool CATRomSymbol::AddressToLine( CATMemoryAddress* result )
{
	LOG_LOW_FUNC_ENTRY("CATRomSymbol::AddressToLine");
	// Have symbols been read.
	if ( ! m_bSymbolsRead )
		return false;
	// check that its lenght > 2
	if ( result->GetAddressString().size() < 2 )
		return false;
	/* Check first is address in range of rom */
	if ( result->GetAddress() < m_iRomStartAddress
		|| result->GetAddress() > m_iRomEndAddress )
	{
		return AddressToLineRofs( result );		
	}
	return AddressToLineRom( result );
}

// -----------------------------------------------------------------------------
// CATRomSymbol::AddressToLineRom
// Locate function from rom address range.
// -----------------------------------------------------------------------------
bool CATRomSymbol::AddressToLineRom( CATMemoryAddress* result )
{
	LOG_LOW_FUNC_ENTRY( "CATRomSymbol::AddressToLineRom" );
	// Address to find in integer & string.
	unsigned long iAddressToFind = result->GetAddress();
	string sAddressToFind = result->GetAddressString();
	
	// Find symbol.
	Symbol* pFound = NULL;

	// Check from cache first.
	vector<Symbol*>::iterator it;
	for ( it = m_vRomCache.begin(); it != m_vRomCache.end(); it++ )
	{
		if ( iAddressToFind >= (*it)->iStartAddress
			&& (*it)->iEndAddress > iAddressToFind )
		{
			pFound = *it;
			break;
		}
	}
	
	if ( pFound == NULL )
	{
		// From all symbols.
		bool reverse = false;
		int offSetFromStart = iAddressToFind - m_iRomStartAddress;
		int offSetFromEnd = m_iRomEndAddress - iAddressToFind;
		if ( offSetFromEnd < offSetFromStart ) 
			reverse = true;
		
		if ( reverse )
		{
			// Iterate vector in reverse.
			vector<Symbol*>::reverse_iterator it;
			for ( it = m_vRomSymbols.rbegin(); it != m_vRomSymbols.rend(); ++it )
			{
				if ( iAddressToFind >= (*it)->iStartAddress
					&& (*it)->iEndAddress > iAddressToFind )
				{
					pFound = *it;
					break;
				}
			}
		}
		else
		{
			// Iterate vector normal direction.
			vector<Symbol*>::iterator it;
			for ( it = m_vRomSymbols.begin(); it != m_vRomSymbols.end(); it++ )
			{
				if ( iAddressToFind >= (*it)->iStartAddress
					&& (*it)->iEndAddress > iAddressToFind )
				{
					pFound = *it;
					break;
				}
			}
		}
	}

	// Set result if found.
	if ( pFound != NULL )
	{
		result->SetFunctionName( pFound->sFunction );
		result->SetAddressToLineState( CATMemoryAddress::SYMBOL );
		// Add found symbols pointer to cache.
		m_vRomCache.push_back( pFound );
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------
// CATRomSymbol::AddressToLineRofs
// Locate function from rofs address range.
// -----------------------------------------------------------------------------
bool CATRomSymbol::AddressToLineRofs( CATMemoryAddress* result)
{
	LOG_LOW_FUNC_ENTRY("CATRomSymbol::AddressToLineRofs");
	// Check that binary name is defined in memory address.
	string sBinary = result->GetModuleName();
	if ( sBinary.empty() )
		return false;
	// Try find that named module.
	vector<RofsBinary*>::iterator rofs = m_vRofsBinaries.begin();
	while( rofs != m_vRofsBinaries.end() )
	{
		if ( (*rofs)->m_sBinary.compare( sBinary ) == 0 )
			break;
		rofs++;
	}
	if ( rofs == m_vRofsBinaries.end() )
		return false;

	// Offset what we are looking from binary
	unsigned long offSet = result->GetAddress();
	offSet -= result->GetModuleStartAddress();
	for( vector<Symbol*>::iterator it = (*rofs)->vSymbols.begin() ;
		it != (*rofs)->vSymbols.end(); it++ )
	{
		if ( (*it)->iStartAddress <= offSet && offSet < (*it)->iEndAddress )
		{
			result->SetFunctionName( (*it)->sFunction );
			result->SetAddressToLineState( CATMemoryAddress::SYMBOL );
			return true;
		}
	}
	return false;
}

//EOF
