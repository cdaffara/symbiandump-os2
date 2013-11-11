/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/




#ifndef STATEXP30_H
#define STATEXP30_H

#ifndef STAT_EXPORT
#define STAT_EXPORT // __declspec(dllexport) // exports now defined in .DEF file
#endif

#include <Stat.h>
#include <StatCommon.h>
#include <ScriptProgressMonitor.h>
#include <MessageReporter.h>
#include <CStatReturnCodes.h>

// These functions are exported by name from the STAT DLL and should
// be linked with dynamically by any application that needs them.

const char	ProcVersion[] =	{ "StdVersion" };
typedef	const char* (WINAPI *PROC_VERSION)( void );

const char	ProcGetError[] =	{ "StdGetError" };
typedef	const char* (WINAPI *PROC_GETERROR)( int handle );

const char	ProcGetErrorText[] =	{ "StdGetErrorText" };
typedef	const char* (WINAPI *PROC_GETERRORTEXT)( int handle, int errorCode );

const char	ProcGetTEFSharedData[] =	{ "StdGetTEFSharedData" };
typedef	const char* (WINAPI *PROC_GETTEFSHAREDDATA)( int handle );

const char	ProcSetConnectionLogging[] =	{ "StdSetConnectionLogging" };
typedef	int (WINAPI *PROC_SETCONNECTIONLOGGING)( const char *logPath );

const char	ProcCloseConnectionLogging[] =	{ "StdCloseConnectionLogging" };
typedef	void (WINAPI *PROC_CLOSECONNECTIONLOGGING)( void );

const char	ProcConnect[] =	{ "StdConnect" };
typedef	int (WINAPI *PROC_CONNECT)(	STATCONNECTTYPE iConnectType,
									const char * pszPlatformType,
									void *,
									void *);

const char	ProcDisconnect[] =	{ "StdDisconnect" };
typedef	int (WINAPI *PROC_DISCONNECT)( int handle );

const char	ProcSetCommandDelay[] =	{ "StdSetCommandDelay" };
typedef	int (WINAPI *PROC_SETCOMMANDDELAY)(	int handle,
											unsigned int iMillisecondDelay
											);

const char	ProcSetCommandLogging[] =	{ "StdSetCommandLogging" };
typedef	int (WINAPI *PROC_SETCOMMANDLOGGING)(	int handle, 
												const char *pszLogPath,
												MessageReporter *const aMessageReporter,
												STATLOGLEVEL iLevel,
												bool bAppend,
												void*,
												void*
												);

const char	ProcSetImageVerification[] =	{ "StdSetImageVerification" };
typedef	int (WINAPI *PROC_SETIMAGEVERIFICATION)(	int handle, 
													const char *pszRefDir, 
													bool bRemoveOldImages, 
													int iFactor
													);

const char	ProcOpenScriptFile[] =	{ "StdOpenScriptFile" };
typedef	int (WINAPI *PROC_OPENSCRIPTFILE)(	int handle, 
											LPCTSTR pszText,
											bool bIsFile
											);

const char	ProcRunScript[] =	{ "StdRunScript" };
typedef	int (WINAPI *PROC_RUNSCRIPT)(	int handle, 
										ScriptProgressMonitor *const monitor
										);

const char	ProcSendRawCommand[] =	{ "StdSendRawCommand" };
typedef	int (WINAPI *PROC_SENDRAWCOMMAND)(	int handle, 
											const char *pszText,
											ScriptProgressMonitor *const monitor
											);

const char	ProcSendCommandFile[] =	{ "StdSendCommandFile" };
typedef	int (WINAPI *PROC_SENDCOMMANDFILE)(	int handle, 
											const char *pszFile,
											ScriptProgressMonitor *const monitor
											);

const char	ProcGetCommandCount[] =	{ "StdGetCommandCount" };
typedef	int (WINAPI *PROC_GETCOMMANDCOUNT)(	int handle, 
											const char *pszFile,
											int *commandCount
											);

const char	ProcGetCurrentCommandNumber[] =	{ "StdGetCurrentCommandNumber" };
typedef	int (WINAPI *PROC_GETCURRENTCOMMANDNUMBER)(	int handle, 
													int *commandNumber
													);

const char	ProcGetReceivedData[] =	{ "StdGetReceivedData" };
typedef	const char* (WINAPI *PROC_GETRECEIVEDDATA)( int handle );
const char	ProcStopProcessing[] =	{ "StdStopProcessing" };
typedef	int (WINAPI *PROC_STOPPROCESSING)( int handle );

const char	ProcGetSnapShot[] =	{ "StdGetSnapShot" };
typedef	int (WINAPI *PROC_GETSNAPSHOT)(	int handle, 
										TBitmapFileHeader **ppFile,
										TBitmapInfoHeader **ppBitmap,
										char **ppData,
										unsigned long *pSize
										);

const char	DLLFolder[] =	{ "\\epoc32\\tools\\stat" };
const char	DLLName[] =	{ "stat.dll" };
const char	DLLUsbName[] =	{ "SymbianUsb.dll" };

// Exported funcion entrypoints
extern "C"
{
	// miscellaneous
	STAT_EXPORT const char * __cdecl Version();
	STAT_EXPORT const char * __stdcall StdVersion();

	STAT_EXPORT const char * __cdecl GetError(const int handle = 0);
	STAT_EXPORT const char * __stdcall StdGetError(const int handle = 0);

	STAT_EXPORT const char * __cdecl GetErrorText(const int handle, int errorCode);
	STAT_EXPORT const char * __stdcall StdGetErrorText(const int handle, int errorCode);

	STAT_EXPORT const char * __cdecl GetTEFSharedData(const int handle = 0);
	STAT_EXPORT const char * __stdcall StdGetTEFSharedData(const int handle = 0);

	STAT_EXPORT int __cdecl SetConnectionLogging(const char *pszLogPath);
	STAT_EXPORT void __cdecl CloseConnectionLogging();
	STAT_EXPORT int __stdcall StdSetConnectionLogging(const char *pszLogPath);
	STAT_EXPORT void __stdcall StdCloseConnectionLogging();
	
	// connection
	STAT_EXPORT int __cdecl Connect(const STATCONNECTTYPE iConnectType, 
							const char *pszPlatformType, 
							void *Reserved1 = NULL, 
							void *Reserved2 = NULL);
	STAT_EXPORT int __stdcall StdConnect(const STATCONNECTTYPE iConnectType, 
										 const char *pszPlatformType, 
										 void *Reserved1 = NULL, 
										 void *Reserved2 = NULL);

	STAT_EXPORT int __cdecl ConnectMT(const STATCONNECTTYPE iConnectType, 
							const char *pszPlatformType);
	STAT_EXPORT int __stdcall StdConnectMT(const STATCONNECTTYPE iConnectType, 
										 const char *pszPlatformType);

	STAT_EXPORT int __cdecl Disconnect(const int handle);
	STAT_EXPORT int __stdcall StdDisconnect(const int handle);

	// remote command delay (100 - 30000ms)
	STAT_EXPORT int __cdecl SetCommandDelay(const int handle, const unsigned int iMillisecondDelay);
	STAT_EXPORT int __stdcall StdSetCommandDelay(const int handle, const unsigned int iMillisecondDelay);

	// logging
	STAT_EXPORT int __cdecl SetCommandLogging(const int handle, 
									  const char *pszLogPath,
									  MessageReporter *const aMessageReporter,
									  const STATLOGLEVEL iLevel = EVerbose,
									  const bool bAppend = true,
									  void* Reserved1 = NULL,
									  void* Reserved2 = NULL);
	STAT_EXPORT int __stdcall StdSetCommandLogging(const int handle, 
												   const char *pszLogPath,
												   MessageReporter *const aMessageReporter,
												   const STATLOGLEVEL iLevel = EVerbose,
												   const bool bAppend = true,
												   void* Reserved1 = NULL,
												   void* Reserved2 = NULL);

	// image verification
	STAT_EXPORT int __cdecl SetImageVerification(const int handle, 
										 const char *pszRefDir, 
										 const bool bRemoveOldImages, 
										 const int iFactor);
	STAT_EXPORT int __stdcall StdSetImageVerification(const int handle, 
													  const char *pszRefDir, 
													  const bool bRemoveOldImages, 
													  const int iFactor);

	// target manipulation
	STAT_EXPORT int __cdecl GetCommandCount(const int handle, const char *pszFile, int *commandCount);
	STAT_EXPORT int __stdcall StdGetCommandCount(const int handle, const char *pszFile, int *commandCount);

	STAT_EXPORT int __cdecl GetCurrentCommandNumber(const int handle, int *commandNumber);
	STAT_EXPORT int __stdcall StdGetCurrentCommandNumber(const int handle, int *commandNumber);

	STAT_EXPORT const char* __cdecl GetReceivedData(const int handle);
	STAT_EXPORT const char* __stdcall StdGetReceivedData(const int handle);
	STAT_EXPORT int __cdecl OpenScriptFile(const int handle, LPCTSTR pszText, bool bIsFile);
	STAT_EXPORT int __stdcall StdOpenScriptFile(const int handle, LPCTSTR pszText, bool bIsFile);

	STAT_EXPORT int __cdecl RunScript(const int handle, ScriptProgressMonitor *const monitor);
	STAT_EXPORT int __stdcall StdRunScript(const int handle, ScriptProgressMonitor *const monitor);

	STAT_EXPORT int __cdecl SendRawCommand(const int handle, const char *pszText, ScriptProgressMonitor *const monitor);
	STAT_EXPORT int __stdcall StdSendRawCommand(const int handle, const char *pszText, ScriptProgressMonitor *const monitor);

	STAT_EXPORT int __cdecl SendCommandFile(const int handle, const char *pszFile, ScriptProgressMonitor *const monitor);
	STAT_EXPORT int __stdcall StdSendCommandFile(const int handle, const char *pszFile, ScriptProgressMonitor *const monitor);

	STAT_EXPORT int __cdecl StopProcessing(int handle);
	STAT_EXPORT int __stdcall StdStopProcessing(int handle);

	STAT_EXPORT int __cdecl GetSnapshot(const int handle, TBitmapFileHeader **ppFile, TBitmapInfoHeader **ppBitmap, char **ppData, unsigned long *pSize);
	STAT_EXPORT int __stdcall StdGetSnapshot(const int handle, TBitmapFileHeader **ppFile, TBitmapInfoHeader **ppBitmap, char **ppData, unsigned long *pSize);
}

#endif // STATEXP30_H
