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


#ifndef __CATPARSETRACEFILE_H__
#define __CATPARSETRACEFILE_H__

#include "../inc/ATCommonDefines.h"
#include "../inc/CATDataSaver.h"
#include "../inc/CATBase.h"
#include "../inc/catallocs.h"

/**
* Parses raw trace data to AnalyzeTool specific data file.
*/
class CATParseTraceFile : public CATBase
{
public:
	/**
	* Constructor
	*/
	CATParseTraceFile();

	/**
	* Main function to start parse.
	*
	* @param pFileName A trace file name
	* @param pOutputFileName Pointer to output file name
	*/
	bool StartParse( const char* pFileName, const char* pOutputFileName, const char* pCleanedTraceFile = NULL );

	/**
	* Get data saver
	* @return CATDataSaver*
	*/
	CATDataSaver* GetDataSaver(void);
	
	/**
	* Get time from timestamp in microseconds as string
	* @param iTimeStamp Timestamp for current message
	* @param iTimeSpan Timespan for current process
	* @return string Acquired time in microseconds(as string)
	*/
	static string GetTimeFromTimeStamp( unsigned __int64 iTimeStamp, unsigned __int64 iTimeSpan );

#ifndef MODULE_TEST
private:
#endif
	CATDataSaver m_DataSaver; /** Data saver */
};

#endif
