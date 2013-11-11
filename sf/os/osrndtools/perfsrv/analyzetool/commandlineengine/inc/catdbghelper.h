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
* Description:  Defines the CATDbgHelper class.
*
*/


#include "iaddresstoline.h"
// Static variable to represent state of dbghelper api.
static bool CDBGHELPER_OPEN = false;
static unsigned int CDBGHELPER_CLIENTS = 0;

// Used "virtual" offset for all loaded modules and addresses.
// This is used so no module is loaded to the binary address range of atool.exe.
const unsigned long AT_VIRTUAL_OFFSET_DBGHELPER = 0x1000000;

/**
* Provides feature to locate code lines using Windows Debug Api. This is done
* by using dbghelper library. Implements the IAddressToLine interface.
*/
class CATDbgHelper : public IAddressToLine
{

public:
	
	/**
	* Constructor
	*/
	CATDbgHelper();
	
	/**
	* Destructor
	*/
	~CATDbgHelper();

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
	* Read map file for function's.
	*/
	void ReadMapFile( const string sMapFileName );

	/**
	* Locate code line and file for given address.
	* @result
	* @return true if successful.
	*/
	bool AddressToLine( CATMemoryAddress* result );

	/**
	* Try get function name for address (only when map is read).
	* @result
	* @return true if succesful.
	*/
	bool AddressToFunction( CATMemoryAddress* result );

#ifndef MODULE_TEST
private:
#endif
	// Base address used.
	DWORD64 m_BaseAddress;
	// Map file read?
	bool m_bMap;
	// Map file functions.
	vector<MAP_FUNC_INFO> m_vMapFileFuncList;
	// Binary in file system.
	PSTR m_pBinaryFile;
};