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
* Description:  Definitions for class CATParseTrace, CProcessData and
*               CSubTestData.
*
*/


#ifndef __CATPARSEBINARYFILE_H__
#define __CATPARSEBINARYFILE_H__

#include "../inc/ATCommonDefines.h"
#include "../inc/CATDataSaver.h"
#include "../inc/CATBase.h"
#include "../inc/catallocs.h"

/**
* Parses raw trace data to AnalyzeTool specific data file.
*/
class CATParseBinaryFile : public CATBase
{
public:
	/**
	* Constructor
	*/
	CATParseBinaryFile();

	/**
	* Main function to start parse.
	*
	* @param pFileName A trace file name
	* @param pOutputFileName Pointer to output file name
	*/
	bool StartParse( const char* pFileName, const char* pOutputFileName );

	/**
	* Get data saver
	* @return CATDataSaver*
	*/
	CATDataSaver* GetDataSaver(void);
	
	/**
	* Gets 64bit number from input string
	* @param cVal Input data
	* @return Acquired number
	*/
	unsigned __int64 StringToNum64( unsigned char* cVal );

	/**
	* Gets 32bit number from input string
	* @param cVal Input data
	* @return Acquired number
	*/
	unsigned long StringToNum32( unsigned char* cVal );

	/**
	* Get next string from file
	* @param pData Acquired data
	* @param pIn Input file stream
	* @param pStreamPos Actuall position in file
	* @param pStreamEnd End of file
	* @return false if no error
	*/
	bool GetString( unsigned char* pData, ifstream &pIn, size_t &streamPos,  size_t streamEnd );

	/**
	* Get next 8bit number from file
	* @param pData Acquired data
	* @param pIn Input file stream
	* @param pStreamPos Actuall position in file
	* @param pStreamEnd End of file
	* @return false if no error
	*/
	bool GetNum8( unsigned char* pData, ifstream &pIn, size_t &pStreamPos, size_t pStreamEnd );

	/**
	* Get next 32bit number from file
	* @param pData Acquired data
	* @param pIn Input file stream
	* @param pStreamPos Actuall position in file
	* @param pStreamEnd End of file
	* @return false if no error
	*/
	bool GetNum32( unsigned char* pData, ifstream &pIn, size_t &pStreamPos, size_t pStreamEnd );

	/**
	* Get next 64bit number from file
	* @param pData Acquired data
	* @param pIn Input file stream
	* @param pStreamPos Actuall position in file
	* @param pStreamEnd End of file
	* @return false if no error
	*/
	bool GetNum64( unsigned char* pData, ifstream &pIn, size_t &pStreamPos, size_t pStreamEnd );

#ifndef MODULE_TEST
private:
#endif
	CATDataSaver m_DataSaver; /** Data saver */
};

#endif