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
* Description:  Interface definition for locating code lines using memory addresses.
*
*/
#ifndef __IADDRESSTOLINE_H__
#define __IADDRESSTOLINE_H__

#include "ATCommonDefines.h"

class CATMemoryAddress;

/**
* Codeline locating interface. Thru this interface CATModule2 locates code lines
* using different implementations depending on platform.
*/
class IAddressToLine
{
public:
	IAddressToLine() {};
	virtual ~IAddressToLine() {};
	/**
	* Open binary.
	* @sString
	* @iNumber
	* @return true if successful.
	*/
	virtual bool Open( const string& sString, const unsigned long iLong) = 0;
	
	/**
	* Get error string. In case of any method failed use this to acquire details on error.
	* @return error string.
	*/
	virtual string GetError( void ) = 0;

	/**
	* Close binary.
	* @return true if succesful.
	*/
	virtual bool Close( void ) = 0;

	/**
	* Locate code line and file for given address.
	* @result
	* @return true if successful.
	*/
	virtual bool AddressToLine( CATMemoryAddress* result ) = 0;
};
#endif