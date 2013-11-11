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
* Description:  Main module for addr2line pinpointing.
*
*/

#include "../inc/cataddr2line.h"
#include "../inc/CATMemoryAddress.h"
#include "../inc/CATBase.h"
#include "../inc/CATDatParser.h"

#define ASCII_CHAR_CARRIAGE_RETURN 0x0D

CATAddr2line::CATAddr2line()
{
	LOG_FUNC_ENTRY("CATAddr2line::CATAddr2line");
}

bool CATAddr2line::Open( const string& sParameter, const unsigned long /* iLong */ )
{
	LOG_FUNC_ENTRY("CATAddr2line::Open");
	//Debugging for addr2line task.
	//debug.open( "addr2line-lines.txt", ios_base::trunc );

	m_sMapFileName.clear();
	// Add .map extendsion
	m_sMapFileName.append( sParameter );
	m_sMapFileName.append( ".map" );

	ReadMapFileArmv5();

	//Make symfile path+name
	string sFullPathToSymFile(sParameter);
	sFullPathToSymFile.erase( sFullPathToSymFile.find_last_of( "." ), string::npos );
	sFullPathToSymFile.append( ".sym" );

	// Check with extension + .sym also.
	if ( ! CATBase::FileExists( sFullPathToSymFile.c_str() ) )
	{
		sFullPathToSymFile.clear();
		sFullPathToSymFile.append( sParameter );
		sFullPathToSymFile.append( ".sym" );
	}

	return server.Initialize( sFullPathToSymFile );
}

string CATAddr2line::GetError( void )
{
	LOG_FUNC_ENTRY("CATAddr2line::GetError");
	string s;
	return s;
}

bool CATAddr2line::Close( void )
{
	LOG_FUNC_ENTRY("CATAddr2line::Close");
	//Debugging for addr2line task.
	//debug.close();
	return true;
}

bool CATAddr2line::AddressToLine( CATMemoryAddress* result )
{
	LOG_FUNC_ENTRY("CATAddr2line::AddressToLine");
	
	result->SetAddressToLineState( CATMemoryAddress::OUT_OF_RANGE);

	if( !server.GetProcessCreatedState() )
		return false;
	//Count address
	ULONG uStartA = result->GetModuleStartAddress();
	ULONG uMemoryA = result->GetAddress();
	ULONG uCountedA = uMemoryA - uStartA;
	uCountedA += FUNCTIONS_OFFSET_IN_GCCE;

	string sTemp = CATBase::NumberToHexString( uCountedA );
    //Remove "0x"
    size_t iCounter = sTemp.find_first_of('x');
    if( iCounter != string::npos )
    {
		sTemp.erase( 0, (int)iCounter+1 );
    }
	// Write to pipe that is the standard input for a child process.
	server.WriteToPipe( sTemp ); 
 
	// Read from pipe that is the standard output for child process.
    string s = server.ReadFromPipe();

	//If output not empty, parse output
	if( !s.empty() )
	{
		//Debugging code for addr2line task.
		//debug.write( "##########\n", 12 );
		//debug.write( s.c_str(), s.size() );
		result->SetAddressToLineState( CATMemoryAddress::EXACT );

		string s2;
		size_t iLocation = s.find_first_of( ASCII_CHAR_CARRIAGE_RETURN );

		bool bFunctionNameFoundUsingAddr2line = false;

		//Function name
		
		if(iLocation != string::npos )
		{
			s2 = s.substr( 0, iLocation );
			//All characters ascii?
			if( CATBase::IsAscii( s2.c_str(), (int)s2.length() ) )
			{
				//addr2line returns $x if function name not found
				//length must over 2 to be real function name
				if( s2.length() > 2 )
				{
					bFunctionNameFoundUsingAddr2line = true;
					result->SetFunctionName( s2 );
					s.erase( 0, iLocation+2 );
				}
			}
		}
		//If function name not found using addr2line find it from map file
		if( !bFunctionNameFoundUsingAddr2line )
		{
			string sFuncName( GetFunctionNameUsingAddress( uCountedA ) );
			//If function name empty, print "???"
			if( sFuncName.empty() )
			{
				s2 = "???";
				result->SetFunctionName( s2 );
				if(iLocation != string::npos )
				{
					s.erase( 0, iLocation+2 );
				}
			}
			else
				result->SetFunctionName( sFuncName );
		}
		iLocation = s.find_first_of( ':' );

		//Filename and location

		if(iLocation != string::npos )
		{
			s2 = s.substr( 0, iLocation );
			result->SetFileName( s2 );
			s.erase( 0, iLocation+1 );
		}

		//Exact line number

		s2 = s.substr( 0, s.find_first_of( ASCII_CHAR_CARRIAGE_RETURN ) );
		result->SetExactLineNumber( atoi( s2.c_str() ) );
	}
	return true;
}

bool CATAddr2line::ReadMapFileArmv5()
{
	LOG_FUNC_ENTRY("CATModule2::ReadMapFileArmv5");
	// Open .map file
	ifstream in( m_sMapFileName.c_str() );
	// File open ok?
	if( ! in.good() )
	{
		in.close();
		return false;
	}
	char cTemp[MAX_LINE_LENGTH];
	bool bFirstFuncFound = false;
	bool bFirstLine = true;
	// Get all lines where is "Thumb"
	do
	{
		// Load one line from .map file
		in.getline( cTemp, MAX_LINE_LENGTH );
		if( bFirstLine )
		{
			bFirstLine = false;
			if( strstr( cTemp, "ARM Linker" ) == NULL )
				return false;
		}
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
			if ( pStart == NULL )
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

// Find function name of given address
string CATAddr2line::GetFunctionNameUsingAddress( unsigned long iAddress )
{
	LOG_LOW_FUNC_ENTRY("CATAddr2line::GetSymbolIndexUsingAddress");
	string sRet;
	for( size_t i = 0; i < m_vMapFileFuncList.size(); i++ )
	{
		unsigned long iStart = m_vMapFileFuncList.at( i ).iAddress;
		unsigned long iEnd = ( m_vMapFileFuncList.at( i ).iAddress
			+ m_vMapFileFuncList.at( i ).iFuncLength );

		if ( iAddress >= iStart && iAddress < iEnd )
			return m_vMapFileFuncList.at( i ).sFunctionName;
	}
	return sRet;
}

//Note: New filtering functions commented out until they are taken into use.
/**
* Filter string out of unwanted characters.
*/
/*
void CATAddr2line::FilterString( string &sString )
{
	LOG_LOW_FUNC_ENTRY("CATAddr2line::FilterString");
	string sFiltered("");
	for( size_t i = 0 ; i < sString.length() ; i++ )
	{
		const char p = sString.at( i );
		if ( p != 0 && strchr( ADDR2LINEALLOWEDCHARS, p ) != 0 )
			sFiltered.push_back( p );
	}
	sString = sFiltered;
}
*/
/**
* Find line feed position from string.
*/
/*
size_t CATAddr2line::FindLineFeed( const string& sString )
{
	LOG_LOW_FUNC_ENTRY("CATAddr2line::FindLineFeed");
	size_t iLineFeed1 = sString.find( 12 );
	size_t iLineFeed2 = sString.find( 15 );
	if ( iLineFeed1 < iLineFeed2 && iLineFeed1 != string::npos )
		return iLineFeed1;
	else if ( iLineFeed2 != string::npos )
		return iLineFeed2;
	else
		return string::npos;
}
*/
/**
* Erase characters from start of the string until other char than linefeed found.
*/
/*
void CATAddr2line::EraseUntilNoLineFeed( string& sString )
{
	LOG_LOW_FUNC_ENTRY("CATAddr2line::EraseUntilNoLineFeed");
	for ( size_t i = 0 ; i < sString.length() ; i++ )
	{
		if ( sString.at( i ) != 15 && sString.at( i ) != 12 )
			break;
	}
	sString.erase( 0, i );
}
*/
/**
* Split multiple line string with unexpected line feeds to vector of strings.
*/
/*
vector<string> CATAddr2line::SplitToStrings( string& sMultiLineString )
{
	LOG_LOW_FUNC_ENTRY("CATAddr2line::SplitToStrings");
    vector<string> vLines;
	while ( 1 )
	{
		size_t iLineFeed = FindLineFeed( sMultiLineString );
		if ( iLineFeed == string::npos )
			break;
		string sCell = sMultiLineString.substr(0, iLineFeed );
		sMultiLineString.erase(0, iLineFeed );
		EraseUntilNoLineFeed( sMultiLineString );
		FilterString( sCell );
		vLines.push_back( sCell );
	}
	// If no lines were found set single one.
	if ( vLines.size() == 0 )
		vLines.push_back( sMultiLineString );
	return vLines;
}
*/
//EOF
