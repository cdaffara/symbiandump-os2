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
* Description:  Class for reading ascii files in AnalyzeTool.
*
*/

#include "../inc/ATCommonDefines.h"
#include "../inc/catfilereader.h"

// -----------------------------------------------------------------------------
// CATFileReader::CATFileReader
// Constructor.
// -----------------------------------------------------------------------------
CATFileReader::CATFileReader()
{
	LOG_FUNC_ENTRY("CATFileReader::CATFileReader");
	m_cDelimiter = '\r'; // default line delimeter
}

// -----------------------------------------------------------------------------
// CATFileReader::~CATFileReader
// Destructor.
// -----------------------------------------------------------------------------
CATFileReader::~CATFileReader()
{
	LOG_FUNC_ENTRY("CATFileReader::~CATFileReader");
}

// -----------------------------------------------------------------------------
// CATFileReader::Open
// Open / Read file.
// -----------------------------------------------------------------------------
bool CATFileReader::Open( const char* cFile )
{
	LOG_FUNC_ENTRY("CATFileReader::Open");
	if ( strlen( cFile ) < 1 )
	{
		LOG_STRING("CATFileReader::Open empty file argument.");
		return false;
	}
	try {
		ifstream in;
		in.open( cFile, ios::binary );
		if ( ! in.good() )
			return false;
		m_stream << in.rdbuf();
		in.close();
	}
	catch(...)
	{
		LOG_STRING("CATFileReader::Open unhandled exception.");
		return false;
	}
	return true;
}

// -----------------------------------------------------------------------------
// CATFileReader::Close
// Close file.
// -----------------------------------------------------------------------------
bool CATFileReader::Close( void )
{
	LOG_FUNC_ENTRY("CATFileReader::Close");
	return true;
}

// -----------------------------------------------------------------------------
// CATFileReader::GetLine
// Get line from file.
// -----------------------------------------------------------------------------
bool CATFileReader::GetLine( string& sLine )
{
	//LOG_FUNC_ENTRY("CATFileReader::GetLine");
	char cLine[MAX_LINE_LENGTH];
	if ( !m_stream.good() )
		return false;
	m_stream.getline( cLine, MAX_LINE_LENGTH, m_cDelimiter ); m_stream.get();
	sLine = cLine;
	return true;
}

// -----------------------------------------------------------------------------
// CATFileReader::SetDelimiter
// Set line delimiting character.
// -----------------------------------------------------------------------------
void CATFileReader::SetDelimiter( char cDelimiter )
{
	LOG_FUNC_ENTRY("CATFileReader::SetDelimiter");
	m_cDelimiter = cDelimiter;
}

// -----------------------------------------------------------------------------
// CATFileReader::GetDelimiter
// Get current line delimiting character.
// -----------------------------------------------------------------------------
char CATFileReader::GetDelimiter() const
{
	LOG_FUNC_ENTRY("CATFileReader::GetDelimiter()");
	return m_cDelimiter;
}