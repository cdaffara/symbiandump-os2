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




#include "stdafx.h"
#include "statexp.h"
#include "statlist.h"

//////////////////////////////////////////////////////////////////////////
// our thread-safe mechanism for list manipulation and back-end processing
CRITICAL_SECTION CriticalSection;
//////////////////////////////////////////////////////////////////////////

// get the version
STAT_EXPORT const char * __cdecl Version()
{
	return aList.Version();
}

// set logging for main STAT connection interface
STAT_EXPORT int __cdecl SetConnectionLogging(const char *pszLogPath)
{
	EnterCriticalSection(&CriticalSection);

	int valid = aList.SetLogging(pszLogPath);

	LeaveCriticalSection(&CriticalSection);
	return valid;
}

// set logging for main STAT connection interface
STAT_EXPORT void __cdecl CloseConnectionLogging()
{
	EnterCriticalSection(&CriticalSection);

	aList.StopLogging();

	LeaveCriticalSection(&CriticalSection);
}

// connect to the remote target
STAT_EXPORT int __cdecl Connect(const STATCONNECTTYPE iConnectType, const char *pszPlatformType, void *Reserved1, void *Reserved2)
{
	EnterCriticalSection(&CriticalSection);

	int valid = false;

	// unused parameters
	(void)Reserved1;
	(void)Reserved2;

	if (iConnectType && pszPlatformType && (*pszPlatformType))
		valid = aList.CreateListMember(iConnectType, pszPlatformType);
	else
		strcpy(aList.szErrorText, "Invalid or missing parameter(s)");

	LeaveCriticalSection(&CriticalSection);
	return valid;
}

STAT_EXPORT int __cdecl ConnectMT(const STATCONNECTTYPE iConnectType, const char *pszPlatformType)
{
	EnterCriticalSection(&CriticalSection);

	int valid = Connect(iConnectType, pszPlatformType);
	if (valid)
	{
		aList.Member(valid)->SetMultithreaded();
	}

	LeaveCriticalSection(&CriticalSection);
	return valid;
}

// set the delay between executed commands
STAT_EXPORT int __cdecl SetCommandDelay(const int handle, const unsigned int iMillisecondDelay)
{
	EnterCriticalSection(&CriticalSection);

	int valid = false;

	if (handle && aList.ListExists())
		valid = aList.Member(handle)->SetCommandDelay(iMillisecondDelay);
	else
		strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");

	LeaveCriticalSection(&CriticalSection);
	return valid;
}

// set logging for individual STAT connection command processing
STAT_EXPORT int __cdecl SetCommandLogging(const int handle,
						   const char *pszLogPath,
						   MessageReporter *const aMessageReporter,
						   const STATLOGLEVEL iLevel,
					       const bool bAppend,
						   void* Reserved1,
						   void* Reserved2)
{
	EnterCriticalSection(&CriticalSection);

	int valid = false;

	// unused parameters
	(void)iLevel;
	(void)Reserved1;
	(void)Reserved2;

	if (handle && aList.ListExists())
		valid = aList.Member(handle)->SetLogging(STATDLLMember::ToUnicode(pszLogPath), bAppend, aMessageReporter);
	else
		strcpy(aList.szErrorText, "Invalid handle or connection does not exist");

	LeaveCriticalSection(&CriticalSection);
	return valid;
}

// set image verification settings
STAT_EXPORT int __cdecl SetImageVerification(const int handle, const char * pszRefDir, const bool bRemoveOldImages, const int iFactor)
{
	EnterCriticalSection(&CriticalSection);

	int valid = false;

	if (handle && aList.ListExists())
		valid = aList.Member(handle)->SetImageVerification(STATDLLMember::ToUnicode(pszRefDir), bRemoveOldImages, iFactor);
	else
		strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");

	LeaveCriticalSection(&CriticalSection);
	return valid;
}

// open a script file within the member
STAT_EXPORT int __cdecl OpenScriptFile(const int handle, const char *pszText, bool bIsFile)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->OpenScriptFile(STATDLLMember::ToUnicode(pszText), bIsFile);

	strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");
	return false;
}

// run a command in the script engine
STAT_EXPORT int __cdecl RunScript(const int handle, ScriptProgressMonitor *const monitor)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->RunScript(monitor);

	strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");
	return false;
}

// send a command to the remote target
STAT_EXPORT int __cdecl SendRawCommand(const int handle, const char *pszText, ScriptProgressMonitor *const monitor)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->SendRawCommand(STATDLLMember::ToUnicode(pszText), monitor);

	strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");
	return false;
}

// send a command to the remote target
STAT_EXPORT int __cdecl SendCommandFile(const int handle, const char *pszText, ScriptProgressMonitor *const monitor)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->SendCommandFile(STATDLLMember::ToUnicode(pszText), monitor);

	strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");
	return false;
}

// get the number of commands in the current script
STAT_EXPORT int __cdecl GetCommandCount(const int handle, const char *pszFile, int *commandCount)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->GetCommandCount(pszFile, commandCount);

	strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");
	return false;
}

// get the number of current command
STAT_EXPORT int __cdecl GetCurrentCommandNumber(const int handle, int *commandNumber)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->GetCurrentCommandNumber(commandNumber);

	strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");
	return false;
}

// get the text received by the last command
STAT_EXPORT const char* __cdecl GetReceivedData(const int handle)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->GetReceivedData();

	strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");
	return NULL;
}

// stop processing the current command on the remote target
STAT_EXPORT int __cdecl StopProcessing(int handle)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->StopProcessing();

	strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");
	return false;
}

// get a bitmap snapshot of the remote target
STAT_EXPORT int __cdecl GetSnapshot(const int handle, TBitmapFileHeader **ppFile, TBitmapInfoHeader **ppBitmap, char **ppData, unsigned long *pSize)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->GetSnapshot(ppFile, ppBitmap, ppData, pSize);

	strcpy(aList.szErrorText, "Invalid parameter(s) or connection does not exist");
	return false;
}

// disconnect from the remote target
STAT_EXPORT int __cdecl Disconnect(const int handle)
{
	EnterCriticalSection(&CriticalSection);

	int valid = false;

	if (handle && aList.ListExists())
		valid = aList.RemoveListMember(handle);
	else
		strcpy(aList.szErrorText, "Invalid handle or connection does not exist");

	LeaveCriticalSection(&CriticalSection);
	return valid;
}

// get the last error thrown
STAT_EXPORT const char * __cdecl GetError(const int handle)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->GetErrorText();

	return aList.szErrorText;
}

// get the error text for a particular error code
STAT_EXPORT const char * __cdecl GetErrorText(const int handle, int errorCode)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->GetErrorText(errorCode);

	return aList.szErrorText;
}

// get the TEF Shared Data
STAT_EXPORT const char * __cdecl GetTEFSharedData(const int handle)
{
	if (handle && aList.ListExists())
		return aList.Member(handle)->GetTEFSharedData();

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
// STD CALL ENTRYPOINTS
///////////////////////////////////////////////////////////////////////////////////////

STAT_EXPORT const char * __stdcall StdVersion()
{
	return Version();
}

STAT_EXPORT const char * __stdcall StdGetError(const int handle)
{
	return GetError(handle);
}

STAT_EXPORT const char * __stdcall StdGetErrorText(const int handle, int errorCode)
{
	return GetErrorText(handle, errorCode);
}

STAT_EXPORT const char * __stdcall StdGetTEFSharedData(const int handle)
{
	return GetTEFSharedData(handle);
}

STAT_EXPORT int __stdcall StdSetConnectionLogging(const char *filename)
{
	return (SetConnectionLogging(filename));
}

STAT_EXPORT void __stdcall StdCloseConnectionLogging()
{
	CloseConnectionLogging();
}

STAT_EXPORT int __stdcall StdConnect(const STATCONNECTTYPE iConnectType, const char *pszPlatformType, void *Reserved1, void *Reserved2)
{
	return (Connect(iConnectType, pszPlatformType, Reserved1, Reserved2));
}

STAT_EXPORT int __stdcall StdConnectMT(const STATCONNECTTYPE iConnectType, const char *pszPlatformType)
{
	return (ConnectMT(iConnectType, pszPlatformType));
}

STAT_EXPORT int __stdcall StdDisconnect(const int handle)
{
	return Disconnect(handle);
}

STAT_EXPORT int __stdcall StdSetCommandDelay(const int handle, const unsigned int iMillisecondDelay)
{
	return SetCommandDelay(handle, iMillisecondDelay);
}

STAT_EXPORT int __stdcall StdSetCommandLogging(const int handle, 
											const char *pszLogPath,
											MessageReporter *const aMessageReporter,
											const STATLOGLEVEL iLevel,
											const bool bAppend,
											void* Reserved1,
											void* Reserved2)
{ 
	return SetCommandLogging(handle,
							pszLogPath,
							aMessageReporter,
							iLevel,
							bAppend,
							Reserved1,
							Reserved2);
}

STAT_EXPORT int __stdcall StdSetImageVerification(const int handle, const char *pszRefDir, const bool bRemoveOldImages, const int iFactor)
{
	return SetImageVerification(handle, pszRefDir, bRemoveOldImages, iFactor);
}

STAT_EXPORT int __stdcall StdOpenScriptFile(const int handle, LPCTSTR pszText, bool bIsFile)
{
	return OpenScriptFile(handle, pszText, bIsFile);
}

STAT_EXPORT int __stdcall StdRunScript(const int handle, ScriptProgressMonitor *const monitor)
{
	return RunScript(handle, monitor);
}

STAT_EXPORT int __stdcall StdSendRawCommand(const int handle, const char *pszText, ScriptProgressMonitor *const monitor)
{
	return SendRawCommand(handle, pszText, monitor);
}

STAT_EXPORT int __stdcall StdSendCommandFile(const int handle, const char *pszFile, ScriptProgressMonitor *const monitor)
{
	return SendCommandFile(handle, pszFile, monitor);
}

STAT_EXPORT int __stdcall StdGetCommandCount(const int handle, const char *pszFile, int *commandCount)
{
	return GetCommandCount(handle, pszFile, commandCount);
}

STAT_EXPORT int __stdcall StdGetCurrentCommandNumber(const int handle, int *commandNumber)
{
	return GetCurrentCommandNumber(handle, commandNumber);
}

STAT_EXPORT const char* __stdcall StdGetReceivedData(const int handle)
{
	return GetReceivedData(handle);
}

STAT_EXPORT int __stdcall StdStopProcessing(int handle)
{
	return StopProcessing(handle);
}

STAT_EXPORT int __stdcall StdGetSnapshot(int handle, TBitmapFileHeader **ppFile, TBitmapInfoHeader **ppBitmap, char **ppData, unsigned long *pSize)
{
	return GetSnapshot(handle, ppFile, ppBitmap, ppData, pSize);
}
