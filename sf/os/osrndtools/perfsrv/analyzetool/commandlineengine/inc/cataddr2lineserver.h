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
* Description:  Defines CATAddr2lineServer class.
*
*/

#ifndef __CATADDR2LINESERVER_H__
#define __CATADDR2LINESERVER_H__

// Includes.
#include "ATCommonDefines.h"

#define BUFSIZE 4096
 
/**
* This class "runs" the addr2line binary as a server for CATAddr2Line.
* It redirects the pipes from the tool.
*/
class CATAddr2lineServer
{
public:

	//Creator
	CATAddr2lineServer();
	
	//Destructor
	~CATAddr2lineServer();

	/**
	* Get function name for given address.
	* @sFullPathAndBinaryName
	* @return True if initialization succesfull.
	*/
	bool Initialize( const string& sFullPathAndBinaryName );

	/**
	* Write data to a pipe.
	* @sAddress
	*/
	VOID WriteToPipe( const string& sAddress );

	/**
	* Read data to a pipe.
	* @return Data from pipe.
	*/
	string ReadFromPipe(VOID);

	/**
	* Gets process created state.
	* @return True if process created successfully.
	*/
	bool GetProcessCreatedState( void );

#ifndef MODULE_TEST
private:
#endif

	HANDLE m_hChildStdinRd, m_hChildStdinWr, m_hChildStdinWrDup, 
	       m_hChildStdoutRd, m_hChildStdoutWr, m_hChildStdoutRdDup, 
	       m_hChildErroutRd, m_hChildErroutWr, m_hChildErroutRdDup, 
           m_hSaveStdin, m_hSaveStdout, m_hSaveErrout; 

    SECURITY_ATTRIBUTES m_saAttr;
    string m_sFullPathAndBinaryName;
	bool m_bProcessCreated;

	/**
	* Creates child process for addr2line.exe.
	* @return True if process created successfully.
	*/
	BOOL CreateChildProcess(VOID); 

	/**
	* Prints error and returns false.
	* @param sInput error string.
	* @return false.
	*/
	bool PrintErrorAndExit( const string sInput );


};
#endif
