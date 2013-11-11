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
* Description:  Defines CATFileReader class.
*
*/


#ifndef __CATFILEREADER_H__
#define __CATFILEREADER_H__

#include "ATCommonDefines.h"

/**
* Provides a feature to read a file as single block into memory and parse it
* from there line by line (iterating). This can fail or be very slow if file
* size is bigger than available physical memory for process.
* But is faster when file fits in free physical memory.
*/
class CATFileReader
{
public:
	/**
	* Constructor.
	*/
	CATFileReader();
	/**
	* Destructor.
	*/
	~CATFileReader();
#ifndef MODULE_TEST
private:
#endif
	/**
	* Prevent copy of this class.
	*/
	CATFileReader& operator =( const CATFileReader& /*other*/ ) { }
	CATFileReader( const CATFileReader& /*other*/ ) { }
public:
	/**
	* Open/Read file.
	* @param cFile file to be read/opened.
	* @return true if succesful.
	*/
	bool Open( const char* cFile );
	/**
	* Close file.
	* @return true if succesful.
	*/
	bool Close( void );
	/**
	* Get line from file.
	* @sLine where line content is stored.
	* @return true if lines left.
	*/
	bool GetLine( string& sLine );
	/**
	* Set the line delimiter.
	* @param cDelimiter char that ends the line.
	*/
	void SetDelimiter( char cDelimiter );
	/**
	* Get current line delimiter.
	* @return char that ends the line.
	*/
	char GetDelimiter() const;
#ifndef MODULE_TEST
private:
#endif
	// Stream where file content is stored.
	stringstream m_stream;
	// Line delimiting character used.
	char m_cDelimiter;
};
#endif
// EOF
