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

#ifndef __CATADDR2LINE_H__
#define __CATADDR2LINE_H__

#include "iaddresstoline.h"
#include "../inc/cataddr2lineserver.h"

// Allowed characters in output.
//const char ADDR2LINEALLOWEDCHARS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :/\\_.-";

/**
* Implements addresstoline interface using one of the GNU binutils tool called addr2line.
* This is used on gcce platform. In release build type also map files are used.
*/
class CATAddr2line : public IAddressToLine
{

public:
	
	/**
	* Constructor
	*/
	CATAddr2line();

	/**
	* Open binary.
	* @sString Full filename with path to binary.
	* @iLong base address of binary.
	* @return true if successful.
	*/
	bool Open( const string& sString, const unsigned long iLong);

	/**
	* Get error string. In case of any method failed use this to acquire details on error.
	* @return error string.
	*/
	string GetError( void );

	/**
	* Close binary.
	* @return true if succesful.
	*/
	bool Close( void );

	/**
	* Locate code line and file for given address.
	* @result
	* @return true if successful.
	*/
	bool AddressToLine( CATMemoryAddress* result );

#ifndef MODULE_TEST
private:
#endif

	// Modules map data (symbols).
	vector<MAP_FUNC_INFO> m_vMapFileFuncList;

	//Map file name
	string m_sMapFileName;

	//Addr2line server class
	CATAddr2lineServer server;

	/**
	* Get function name for given address.
	* @iAddress Memory address as unsigned long.
	* @return Function name as string or empty string if not found.
	*/
	string GetFunctionNameUsingAddress( unsigned long iAddress );

	/**
	* Read map file (armv5 platform).
	* @return true if map file read successfully.
	*/
	bool ReadMapFileArmv5();

	//Note: New filtering functions commented out until they are taken into use.
	//These were part of task which would filter unwanted characters, etc.. from results.

	/**
	* Filter any char not defined in constant
	* ADDR2LINEALLOWEDCHARS from given string.
	* @param sString string to be filtered.
	*/
	//void FilterString( string &sString );

	/**
	* Find first occurence of LF/CR from string.
	* @param sString string to find LF/CR.
	* @return position of first occurence.
	*/
	//size_t FindLineFeed( const string& sString );

	/**
	* Erase all LF/CR from start of the string until other
	* characters are found.
	* @param sString string to erase LF/CR.
	*/
	//void EraseUntilNoLineFeed( string& sString );

	/**
	* Split string containing multiple lines with mixed line feeds to
	* vector of lines.
	* @sMultiLineString string containing multiple lines.
	* @return vector containing one line per cell.
	*/
	//vector<string> SplitToStrings( string& sMultiLineString );
	
	//Debugging for addr2line task.
	//ofstream debug;
};

#endif
