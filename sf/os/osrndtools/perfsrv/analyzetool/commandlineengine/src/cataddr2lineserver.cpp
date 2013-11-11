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
* Description:  Class representing a server that uses addr2line.exe.
*
*/

#include "../inc/cataddr2lineserver.h"
#include "../inc/CATBase.h"

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
CATAddr2lineServer::CATAddr2lineServer()
{
	LOG_FUNC_ENTRY("CATAddr2lineServer::CATAddr2lineServer");
	m_bProcessCreated = false;

	// Pipe handles.
	m_hChildErroutRd = 0;
	m_hChildErroutRdDup = 0;
	m_hChildErroutWr = 0;
	m_hChildStdinRd = 0;
	m_hChildStdinWr = 0;
	m_hChildStdinWrDup = 0;
	m_hChildStdoutRd = 0;
	m_hChildStdoutWr = 0;
	m_hChildStdoutRdDup = 0;
	
	m_hSaveErrout = 0;
	m_hSaveStdin = 0;
	m_hSaveStdout = 0;

    // Set the bInheritHandle flag so pipe handles are inherited.
    m_saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    m_saAttr.bInheritHandle = TRUE;
    m_saAttr.lpSecurityDescriptor = NULL;

	// Symbol file.
	m_sFullPathAndBinaryName = "";
}
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CATAddr2lineServer::~CATAddr2lineServer()
{
	LOG_FUNC_ENTRY("CATAddr2lineServer::~CATAddr2lineServer");
	// Close the pipe handle so the child process stops reading.
    CloseHandle(m_hChildStdinWrDup);
    // Close the write end of the pipe
    CloseHandle(m_hChildStdoutWr);
    // Close the write end of the error pipe
	CloseHandle(m_hChildErroutWr);
}
bool CATAddr2lineServer::Initialize( const string& sFullPathAndBinaryName )
{
    LOG_FUNC_ENTRY("CATAddr2lineServer::Initialize");
	BOOL fSuccess;
	m_sFullPathAndBinaryName = sFullPathAndBinaryName;

    // The steps for redirecting child process's STDOUT:
    //     1. Save current STDOUT, to be restored later.
    //     2. Create anonymous pipe to be STDOUT for child process.
    //     3. Set STDOUT of the parent process to be write handle to
    //        the pipe, so it is inherited by the child process.
    //     4. Create a noninheritable duplicate of the read handle and
    //        close the inheritable read handle.
 
    // Save the handle to the current STDOUT. 
    m_hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE); 

    // Create a pipe for the child process's STDOUT.
    if (! CreatePipe(&m_hChildStdoutRd, &m_hChildStdoutWr, &m_saAttr, 0))
		return PrintErrorAndExit( "Stdout pipe creation failed\n" );

    // Set a write handle to the pipe to be STDOUT.
   if (! SetStdHandle(STD_OUTPUT_HANDLE, m_hChildStdoutWr))
		return PrintErrorAndExit( "Redirecting STDOUT failed\n" );

    // Create noninheritable read handle and close the inheritable read
    // handle.
    fSuccess = DuplicateHandle(GetCurrentProcess(), m_hChildStdoutRd,
        GetCurrentProcess(), &m_hChildStdoutRdDup , 0,
        FALSE,
        DUPLICATE_SAME_ACCESS);
    if( !fSuccess )
		return PrintErrorAndExit( "DuplicateHandle failed" );
    CloseHandle(m_hChildStdoutRd);

    // The steps for redirecting child process's STDIN:
    //     1.  Save current STDIN, to be restored later.
    //     2.  Create anonymous pipe to be STDIN for child process.
    //     3.  Set STDIN of the parent to be the read handle to the
    //         pipe, so it is inherited by the child process.
    //     4.  Create a noninheritable duplicate of the write handle,
    //         and close the inheritable write handle.
 
    // Save the handle to the current STDIN.
    m_hSaveStdin = GetStdHandle(STD_INPUT_HANDLE);

    // Create a pipe for the child process's STDIN.
    if (! CreatePipe(&m_hChildStdinRd, &m_hChildStdinWr, &m_saAttr, 0))
		return PrintErrorAndExit( "Stdin pipe creation failed\n" );
 
    // Set a read handle to the pipe to be STDIN. 
    if (! SetStdHandle(STD_INPUT_HANDLE, m_hChildStdinRd)) 
		return PrintErrorAndExit( "Redirecting Stdin failed\n" ); 
 
    // Duplicate the write handle to the pipe so it is not inherited. 
	
    fSuccess = DuplicateHandle(GetCurrentProcess(), m_hChildStdinWr, 
        GetCurrentProcess(), &m_hChildStdinWrDup, 0, 
        FALSE,                  // not inherited 
        DUPLICATE_SAME_ACCESS); 
    if (! fSuccess) 
		return PrintErrorAndExit( "DuplicateHandle failed\n" );

    CloseHandle(m_hChildStdinWr);
 
    // The steps for redirecting child process's ERROUT:
    //     1. Save current STDOUT, to be restored later.
    //     2. Create anonymous pipe to be STDOUT for child process.
    //     3. Set STDOUT of the parent process to be write handle to
    //        the pipe, so it is inherited by the child process.
    //     4. Create a noninheritable duplicate of the read handle and
    //        close the inheritable read handle.
 
    // Save the handle to the current STDOUT. 
    m_hSaveErrout = GetStdHandle( STD_ERROR_HANDLE ); 

    // Create a pipe for the child process's ERROUT.
    if (! CreatePipe(&m_hChildErroutRd, &m_hChildErroutWr, &m_saAttr, 0))
		return PrintErrorAndExit( "Errout pipe creation failed\n" );

    // Set a write handle to the pipe to be ERROUT.
   if (! SetStdHandle(STD_ERROR_HANDLE, m_hChildErroutWr))
		return PrintErrorAndExit( "Redirecting ERROUT failed\n" );

    // Create noninheritable read handle and close the inheritable read
    // handle.
   
    fSuccess = DuplicateHandle(GetCurrentProcess(), m_hChildErroutRd,
        GetCurrentProcess(), &m_hChildErroutRdDup , 0,
        FALSE,
        DUPLICATE_SAME_ACCESS);
    if( !fSuccess )
		return PrintErrorAndExit( "DuplicateHandle failed" );
    
	CloseHandle(m_hChildErroutRd);
	CloseHandle(m_hChildErroutRdDup);
	
	// Now create the child process. 
    fSuccess = CreateChildProcess();
    if (!fSuccess )
		return PrintErrorAndExit( "Create process failed\n" );
 
    // After process creation, restore the saved STDIN and STDOUT.
	if (! SetStdHandle(STD_INPUT_HANDLE, m_hSaveStdin))
        return PrintErrorAndExit( "Re-redirecting Stdin failed\n" );

	if (! SetStdHandle(STD_OUTPUT_HANDLE, m_hSaveStdout))
       return PrintErrorAndExit( "Re-redirecting Stdout failed\n" );

	if (! SetStdHandle(STD_ERROR_HANDLE, m_hSaveErrout))
       return PrintErrorAndExit( "Re-redirecting Stderrout failed\n" );

	m_bProcessCreated = true;
    return true;
}
BOOL CATAddr2lineServer::CreateChildProcess() 
{ 
    LOG_FUNC_ENTRY("CATAddr2lineServer::CreateChildProcess");
    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFO siStartInfo;
    BOOL bFuncRetn = FALSE; 
 
    // Set up members of the PROCESS_INFORMATION structure.
    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
	// Set up members of the STARTUPINFO structure.
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO); 
 
	//atool.exe:s path + filename
	char buffer[MAX_PATH];

	GetModuleFileName( NULL, buffer, MAX_PATH );

	string sCommand( buffer );

	//Remove 'atool.exe'
	size_t tPos = sCommand.rfind( "\\" );
	if ( tPos != string::npos )
		sCommand.resize( sCommand.rfind( "\\" )+1 );
	else
		sCommand.clear();

	sCommand.append( "addr2line.exe" );

	//addr2line file exists
	if( !CATBase::FileExists( sCommand.c_str() ) )
		return PrintErrorAndExit( "File not found addr2line.exe\n" );

	//Sym file exists
	if( !CATBase::FileExists( m_sFullPathAndBinaryName.c_str() ) )
	{
		string sTemp( "File not found " );
		sTemp.append( m_sFullPathAndBinaryName );
		sTemp.append( "\n" );
		return PrintErrorAndExit( sTemp );
	}

	sCommand.append(" -f -C -e ");
	sCommand.append( m_sFullPathAndBinaryName );

	// Create the child process. 
    bFuncRetn = CreateProcess(NULL,
		(LPSTR)sCommand.c_str(), // command line
        NULL,          // process security attributes
        NULL,          // primary thread security attributes
        TRUE,          // handles are inherited
        0,             // creation flags
        NULL,          // use parent's environment
        NULL,          // use parent's current directory
        &siStartInfo,  // STARTUPINFO pointer
        &piProcInfo);  // receives PROCESS_INFORMATION

    if (bFuncRetn == 0)
    {
       //CreateProcess failed
       return false;
    }
    else
    {
       CloseHandle(piProcInfo.hProcess);
       CloseHandle(piProcInfo.hThread);
       return bFuncRetn;
    }
}
VOID CATAddr2lineServer::WriteToPipe( const string& sAddress ) 
{ 
	LOG_LOW_FUNC_ENTRY("CATAddr2lineServer::WriteToPipe");
    DWORD dwRead, dwWritten;
    CHAR chBuf[2]; //This is for enter key

    //Write data to a pipe. 
    dwRead = (DWORD)sAddress.length();
	WriteFile(m_hChildStdinWrDup, sAddress.c_str(), dwRead, &dwWritten, NULL);

	chBuf[0] = 0x0A; //Enter
	chBuf[1] = 0; //NULL
	//Write enter key to a pipe
	WriteFile(m_hChildStdinWrDup, chBuf, 1, &dwWritten, NULL);
} 
string CATAddr2lineServer::ReadFromPipe(VOID) 
{
	LOG_LOW_FUNC_ENTRY("CATAddr2lineServer::ReadFromPipe");
	string s;
	DWORD dwRead;
    CHAR chBuf[BUFSIZE];
    // Read output from the child process, and save data to string.
    ReadFile( m_hChildStdoutRdDup, chBuf, BUFSIZE, &dwRead, NULL);
	s.append( chBuf );
    return s;
}
bool CATAddr2lineServer::GetProcessCreatedState( void )
{
	LOG_LOW_FUNC_ENTRY("CATAddr2lineServer::GetProcessCreatedState");
	return m_bProcessCreated;
}
bool CATAddr2lineServer::PrintErrorAndExit( const string sInput )
{
	LOG_FUNC_ENTRY("CATAddr2lineServer::PrintErrorAndExit");
	cout << AT_MSG << sInput;
	return false;
}
