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
* Description:  Defines CATRomSymbol, RofsBinary classes and Symbol
*               structure.
*
*/


#ifndef __CATROMSYMBOL_H__
#define __CATROMSYMBOL_H__

#include "ATCommonDefines.h"
#include "iaddresstoline.h"

const string ROM_SYMBOL_IDENTIFY_STRING = "80";
const string ROFS_SYMBOL_IDENTIFY_STRING = "00";
const int IDENTIFY_MAX_LINES_READ = 20;

/**
* Represents a single symbol in a binary.
*/
struct Symbol
{
	unsigned long iStartAddress; /** Start address */
	unsigned long iEndAddress; /** End address */
	string sFunction; /** Function/Symbol name */
	/**
	* Default constructor for structure to set default values
	*/
	Symbol() { iStartAddress=0; iEndAddress=0; sFunction = ""; }
	/**
	* Empty destructor
	*/
	~Symbol() {}
};


/**
* Represents a single binary in ROM/ROFS which
* contains a collection of Symbols.
*/
class RofsBinary {
public:
	/**
	* Constructor
	*/
	RofsBinary();
	/**
	* Constructor
	* @param sbinary
	*/
	RofsBinary( const string& sbinary );
	/**
	* Destructor
	*/
	~RofsBinary();

	// Members

	string m_sBinary; /** Binary / code segment */
	vector<Symbol*> vSymbols; /** Collection of symbols */
};

/**
* Implements IAddressToLine interface to acquire symbol information
* using memory addresses from rom/rofs symbol files.
*/
class CATRomSymbol : public IAddressToLine
{
public:
	/**
	* Constructor.
	*/
	CATRomSymbol();
	/**
	* Destructor.
	*/
	virtual ~CATRomSymbol();
	bool m_bShowProgressMessages; /** "Flag" will we show progress when reading files */
	/**
	* Empty functions does nothing returns false always.
	* @param sString
	* @param iLong
	* @return true if successful.
	*/
	bool Open( const string& sString, const unsigned long iLong);
	/**
	* Set symbol files.
	* This also checks that files exists and identifies them as rom/rofs.
	* @param vSymbols
	* @return bool
	*/
	bool SetSymbols( const vector<string>& vSymbols);
	/**
	* Get error string. In case of any method failed use this to acquire details on error.
	* @return error string.
	*/
	string GetError( void );
	/**
	* Close rom symbol file.
	* @return true if succesful.
	*/
	bool Close( void );
	/**
	* Locates symbol and binary name for given address if found in rom.
	* @param result memory address object.
	* @return true if successful.
	*/
	bool AddressToLine( CATMemoryAddress* result );
#ifndef MODULE_TEST
private:
#endif
	/**
	* Represents the symbol files type
	* (content is different / format )
	*/
	enum SYMBOL_FILE_TYPE {
		SYMBOL_FILE_INVALID = 0, /** not valid */
		SYMBOL_FILE_ROM, /** rom type */
		SYMBOL_FILE_ROFS /** rofs type */
	};
	/**
	* Identify symbol file
	* @param sFile
	* @return int
	*/
	int IdentifySymbolFile( const string& sFile );
	/**
	* Locate symbol and binary name for given address if found in rom.
	* @param result
	* @return bool
	*/
	bool AddressToLineRom( CATMemoryAddress* result );
	/**
	* Locate symbol and binary name for given address if found in rofs.
	* @param result
	* @return bool
	*/
	bool AddressToLineRofs( CATMemoryAddress* result );
	/**
	* Reads rom files.
	* @return bool
	*/
	bool ReadRomFiles();
	/**
	* Read specified rom file
	* @param sFile
	* @return bool
	*/
	bool ReadRomFile( const string& sFile );
	/**
	* Read rofs files
	* @return bool
	*/
	bool ReadRofsFiles();
	/**
	* Read specified rofs file
	* @param sFile
	* @return bool
	*/
	bool ReadRofsFile( const string& sFile );
	/**
	* Parse symbol from a line in rom/rofs file.
	* @param sLine
	* @param pSymbol
	*/
	void ParseSymbolFromLine( const string& sLine, Symbol* pSymbol );
#ifndef MODULE_TEST
private:
#endif

	// Members

	bool m_bFilesIdentified; /**  Have we identified symbol file(s) */
	bool m_bSymbolsRead; /** Have we read symbol file(s) */
	vector<string> m_vRomFiles; /** Rom symbol file(s) */
	vector<Symbol*> m_vRomCache; /** Cached rom symbols */
	vector<Symbol*> m_vRomSymbols; /** All rom symbols */
	unsigned long m_iRomStartAddress; /** Rom start address */
	unsigned long m_iRomEndAddress; /** Rom end address */
	vector<string> m_vRofsFiles; /** Rofs symbol file(s) */
	vector<RofsBinary*> m_vRofsBinaries; /** Rofs binaries */
	string m_sErrorMessage; /** Error message */
};
#endif