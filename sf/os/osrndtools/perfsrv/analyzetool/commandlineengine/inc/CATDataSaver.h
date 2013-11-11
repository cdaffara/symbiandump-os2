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
* Description:  Defines CATDataSaver class and element type enumeration.
*
*/


#ifndef __CATDATASAVER_H__
#define __CATDATASAVER_H__

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include "ATCommonDefines.h"

/**
* Represents element types
*/
enum element_types
{
	RESULT = 0,
	RUN,
	LEAK,
	CALLSTACK,
	ITEM,
	RUN_END,
	ERROR_IN_RUN,
	MEM_LEAKS,
	MEM_LEAK_MODULE,
	HANDLE_LEAKS,
	HANDLE_LEAK_MODULE,
	TEST_START,
	TEST_END,
	SUBTEST_MEM_LEAKS,
	SUBTEST_MEM_LEAK_MODULE,
	LOGGING_CANCELLED,
	SUBTEST_HANDLE_LEAKS
};

//#define CARBIDE_DATA 0
#define XML_DATA     1
#define TEXT_DATA    2

/**
* TProvides features for creating the analyze report in
* basic text or XML formats. Uses xerces library for XML output.
*/
class CATDataSaver
{

public:

	/**
	* Constructor.
	*/
	CATDataSaver( void );

	/**
	* Destructor.
	*/
	~CATDataSaver(void);

	/**
	* Save all lines to file with given format.
	* @param pFileName Pointer to file name.
	* @param iDataToSave Format of data.
	*/
	void SaveLinesToFile( const char* pFileName, int iDataToSave );

	/**
	* Prints all saved lines to screen.
	*/
	void PrintLinesToScreen( void );

	/**
	* Adds saved line to first in database.
	*/
	void AddLineToFirst( void );

	/**
	* Adds saved line to last in database.
	*/
	void AddLineToLast();

	/**
	* Adds string to current line.
	* @param pData string to add
	* @param bSaveCarbideData add string to xml?
	*/
	void AddString( const char* pData, bool bSaveCarbideData = false );

	/**
	* Converts integer to string and adds it to current line.
	* @param iValue integer value to add
	* @param bSaveCarbideData add string to xml?
	*/
	void AddInteger( int iValue, bool bSaveCarbideData = false );

	/**
	* Sets logging level.
	* If value is invalid DEFAULT_LOGGING_LEVEL is used
	* value must be between MIN_LOGGING_LEVEL and MAX_LOGGING_LEVEL
	* @param iLoggingLevel Logging level.
	*/
	void SetLoggingLevel( int iLoggingLevel );

	/**
	* Gets logging level.
	* @return Logging level.
	*/
	int GetLoggingLevel( void );

	/**
	* Sets print immediately flag.
	* @param bPrintImmediately
	*/
	void SetPrintFlag( bool bPrintImmediately );

	/**
	* Sets data header for carbide data.
	*/
	void SaveCarbideDataHeader( void );

	/**
	* Initializes xercer xml parser.
	*/
	bool InitXML( void );

	/**
	* Writes data to xml tree.
	* @param sInput data 
	* @param iElementType element to write
	*/
	void SaveXML( string sInput, int iElementType );

	/**
	* Converts char* -> wchar_t*.
	* @param str
	* @return LPWSTR
	*/
	static LPWSTR CharToWChar( const char* str );

	/**
	* Converts wchar_t* -> char*.
	* @param sInput
	* @param Source
	*/
	static void WCharToChar( string& sInput, const WCHAR* Source );

	/**
	* Set is build type debug?
	* @param bUdebBuild true if build type debug
	*/
	void SetBuild( bool bUdebBuild );

	/**
	* Add xml data for extension
	* @param sInput data
	*/
	void AddCarbideData( const string& sInput );

	/**
	* Convert given integer to string
	* @param iValueToConvert
	* @return string
	*/
	static string IntegerToString( int iValueToConvert );

#ifndef MODULE_TEST
private:
#endif
	vector<string> m_vLines;
	string m_sLine;
	string m_sCarbideDataLine;

	string m_sCarbideDataHeader;

	int m_iLoggingLevel;
	int m_iRunNumber;
	bool m_bPrintImmediately;
	bool m_bXMLInitOk;
	bool m_bUdebBuild;

	xercesc::DOMDocument* m_pDomDoc;
	xercesc::DOMElement* m_pRootElem;
	xercesc::DOMElement* m_pCurrentLeakElem;
	xercesc::DOMWriter* m_Serializer;
	xercesc::DOMElement* m_pRunElement;
	xercesc::DOMElement* m_pMemoryLeaks;
	xercesc::DOMElement* m_pHandleLeaks;
	xercesc::DOMElement* m_pCurrentSubTestElem;
	xercesc::DOMElement* m_pSubtestMemoryLeaks;

	/**
	* Utility to get sub string from string using char as separator
	* @param sInput
	* @param cCharacter
	* @return string
	*/
	string GetStringUntilNextGivenChar( string& sInput, char cCharacter );

};

#endif
