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
#include "statmember.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////
// Construct and initialise a STAT object and check it hasn't passed it's expiry date.
//
STATDLLMember::STATDLLMember(const STATCONNECTTYPE iConnectType, const char *pszPlatformType, Reporter *theRep)
: pRep(theRep), 
  iConnectionType(iConnectType), 
  iErrorCode(GENERAL_FAILURE), 
  lNextConnection((STATDLLMember*)0), 
  lPrevConnection((STATDLLMember*)0),
  pEngine(NULL)
{
	CString lPrefix="";

	pszAddress = NULL;
	iErrorCode = GENERAL_FAILURE;
	strcpy(szErrorText, "An unknown error has occured");

	// set logging identifier
	lPrefix.Format(LPCTSTR("[%d:%s] "), iConnectType, pszPlatformType);
 	const char *lPrefixPtr=(const char *)lPrefix.GetBuffer(lPrefix.GetLength());
	strncpy(szPrefix, lPrefixPtr, MAX_ID_PREFIX);

	pEngine = new CSTATEngine;
	if (pEngine)
	{
		if ((iErrorCode = pEngine->Initialise(iConnectType, pszPlatformType)) != ITS_OK)
		{
			ErrorMessage("Could not initialise new connection object");
			delete pEngine;
			pEngine = NULL;
		}
		else
		{
			pszAddress = new char[strlen(pszPlatformType) + 1];
			if (pszAddress)
				strcpy(pszAddress, pszPlatformType);
		}
	}
	else
		ErrorMessage("Could not create new connection object");

	// did we make it?
	if (iErrorCode == ITS_OK)
		Message("Connection initialised successfully");
}


////////////////////////////////////////////////////////////////////////////////////////
// Release any STAT resources.
//
STATDLLMember::~STATDLLMember()
{
	if (pEngine)
	{
		Message("Releasing member resources");
		pEngine->Release();
		delete pEngine;
	}

	if (pszAddress)
		delete [] pszAddress;
}


////////////////////////////////////////////////////////////////////////////////////////
// Do all the preparations in one go
//
bool
STATDLLMember::Prepare(int iMillisecondDelay, LPTSTR pszLogPath, bool bAppend, MessageReporter *const aMessageReporter, LPTSTR pszRefDir, const bool bRemoveOldImages, const int iFactor, bool bState)
{
	if (pEngine)
	{
		pEngine->SetMultithreaded(bState);
	}

	if (!SetCommandDelay(iMillisecondDelay))
		return false;

	if (pszLogPath && (*pszLogPath) && !SetLogging(pszLogPath, bAppend, aMessageReporter))
		return false;

	if (pszRefDir && (*pszRefDir) && !SetImageVerification(pszRefDir, bRemoveOldImages, iFactor))
		return false;

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////
// Set the delay time between commands
//
int
STATDLLMember::SetCommandDelay(int iMillisecondDelay)
{
	if (pEngine)
	{
		pEngine->SetCommandDelay(iMillisecondDelay);
		Message("Command delay set");
		iErrorCode = ITS_OK;
		return true;
	}

	iErrorCode = GENERAL_FAILURE;
	ErrorMessage("Command delay could not be set");
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////
// Set logging of STAT commands.
//
int
STATDLLMember::SetLogging(LPTSTR pszLogPath, bool bAppend, MessageReporter *const aMessageReporter)
{
	iErrorCode = GENERAL_FAILURE;
	CString logpath = pszLogPath;

	if (pEngine)
	{
		if (LOG_FILE_OK == (iErrorCode = pEngine->SetLogging(logpath, szPrefix, bAppend, 
				(aMessageReporter != NULL), true, aMessageReporter)))
		{
			Message("Logging set");
			return LOG_FILE_OK;
		}
	}

	ErrorMessage("Logging could not be set");
	return GENERAL_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////////
// Send a string of STAT commands through to the STAT object.
//
int
STATDLLMember::SendRawCommand(LPCTSTR pszText, ScriptProgressMonitor *const monitor)
{
	iErrorCode = GENERAL_FAILURE;
	CString file = pszText;

	if (pEngine)
	{
		{
		CString msg;
		msg.Format( "Sending raw command...%s", pszText );
		Message(msg.operator LPCTSTR());
		}
		if ((iErrorCode = pEngine->OpenScriptFile(file, false)) == ITS_OK)
			if ((iErrorCode = pEngine->RunScript(monitor)) == END_SCRIPT)
				return ITS_OK;
	}

	ErrorMessage("Command text could not be processed successfully");
	return iErrorCode;
}


////////////////////////////////////////////////////////////////////////////////////////
// Open a script file or text block in the engine.
//
int
STATDLLMember::OpenScriptFile(LPCTSTR pszText, bool bIsFile)
{
	iErrorCode = GENERAL_FAILURE;
	CString file = pszText;

	if (pEngine)
	{
		{
		CString msg;
		msg.Format( "Opening script file...%s", pszText );
		Message(msg.operator LPCTSTR());
		}
		if ((iErrorCode = pEngine->OpenScriptFile(pszText, bIsFile)) == ITS_OK)
				return iErrorCode;
	}

	ErrorMessage("Open script file could not be processed successfully");
	return iErrorCode;
}


////////////////////////////////////////////////////////////////////////////////////////
// Runs the script in the engine.
//
int
STATDLLMember::RunScript(ScriptProgressMonitor *const monitor)
{
	iErrorCode = GENERAL_FAILURE;

	if (pEngine)
	{
		{
		CString msg;
		msg.Format( "Running script..." );
		Message(msg.operator LPCTSTR());
		}

		iErrorCode = pEngine->RunScript(monitor);
	}

	if( iErrorCode != END_SCRIPT )
	{
		ErrorMessage("Run script could not be processed successfully");
	}

	if(monitor)
	{
		monitor->OnCompleteScript( iErrorCode );
	}

	return iErrorCode;
}


////////////////////////////////////////////////////////////////////////////////////////
// Send a command file containing a string of STAT commands through to the STAT object.
// Checks that it is actually an accessible file before accepting it.
//
int
STATDLLMember::SendCommandFile(LPCTSTR pszFile, ScriptProgressMonitor *const monitor)
{
	iErrorCode = GENERAL_FAILURE;
	CString file = pszFile;

	if (pEngine)
	{
		Message("Sending command file...");
		if ((iErrorCode = pEngine->OpenScriptFile(file, true)) == ITS_OK)
			if ((iErrorCode = pEngine->RunScript(monitor)) == END_SCRIPT)
				return true;
	}

	ErrorMessage("Script file could not be processed successfully");
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////
// Stops processing the current command.
//
int
STATDLLMember::StopProcessing(void)
{
	iErrorCode = GENERAL_FAILURE;

	if (pEngine)
	{
		Message("Stopping processing...");
		pEngine->eStopProcessing = STAT_PAUSE;
		return true;
	}

	ErrorMessage("Stopping the process could not be processed successfully");
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////
// Returns the number of commands in the current script.
//
int
STATDLLMember::GetCommandCount(LPCTSTR pszFile, int* commandCount)
{
	iErrorCode = GENERAL_FAILURE;

	if (pEngine)
	{
		Message("Sending command file...");
		if ((iErrorCode = pEngine->GetCommandCount(pszFile, commandCount)) == ITS_OK)
				return ITS_OK;
	}

	ErrorMessage("Getting the command count could not be processed successfully");
	return GENERAL_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////////
// Returns the number of the current command.
//
int
STATDLLMember::GetCurrentCommandNumber( int* commandNumber )
{
	iErrorCode = GENERAL_FAILURE;

	if (pEngine)
	{
		Message("Getting current command number...");
		*commandNumber =	pEngine->iCurrentCommand;
		iErrorCode = ITS_OK;
			return iErrorCode;
	}

	ErrorMessage("Getting the current command number could not be processed successfully");
	return iErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////
// Returns the text recevied by the commands.
//
const char *
STATDLLMember::GetReceivedData(void)
{
	if (pEngine)
	{
		Message("Getting data received by commands...");
		return ( pEngine->GetReceivedData() );
	}

	return ( "Getting the error text could not be processed successfully" );
}

////////////////////////////////////////////////////////////////////////////////////////
// Returns the text associated with an error value.
//
const char *
STATDLLMember::GetErrorText( int errorCode )
{
	if (pEngine)
	{
		Message("Getting current command number...");
		return ( pEngine->GetErrorText(errorCode) );
	}

	return ( "Getting the error text could not be processed successfully" );
}

////////////////////////////////////////////////////////////////////////////////////////
// Sets image verification with supplied reference directory, and will remove existing
// images if specified.  Also sets the 'fudge' factor for the verification itself.
//
int
STATDLLMember::SetImageVerification(LPTSTR pszRefDir, const bool bRemoveOldImages, const int iFactor)
{
	iErrorCode = GENERAL_FAILURE;
	Message("Setting image verification Dir [%s] Remove Old %d Factor %d...", ToAnsi(pszRefDir), bRemoveOldImages, iFactor);

	if (pEngine && pEngine->pImageVerify)
	{
		//set default logging path
		CString defaultDirectory=STAT_LOGFILEPATH_VALUE;
		//read from inifile if entry exists
		if(statIniFile.SectionExists(ST_TEST_KEY) )
		{
			CString setting;
			setting.Empty();
			setting=statIniFile.GetKeyValue(ST_LOGFILEPATH,ST_TEST_KEY);
			if(!setting.IsEmpty())
				defaultDirectory = setting;
		}
		iErrorCode = pEngine->pImageVerify->Initialise(defaultDirectory);
		if(iErrorCode == ERROR_REGISTRY)
		{
			ErrorMessage("Config file failure");
		}
		else if(iErrorCode == REFDIR_FOUND)
		{
			if (bRemoveOldImages)
			{
				Message("Removing old images...");
				iErrorCode = pEngine->pImageVerify->DeleteReferenceImages();
			}
			else
				Message("Old reference images exist");
		}

		// if preparations went OK...
		if(iErrorCode == ITS_OK)
		{
			Message("Copying new images from [%s]...", ToAnsi(pszRefDir));
			iErrorCode = pEngine->pImageVerify->CopyReferenceImages(pszRefDir);
			if(iErrorCode != ITS_OK)
				ErrorMessage("Images could not be copied");
		}

		// success?
		if(iErrorCode == ITS_OK || iErrorCode == REFDIR_FOUND)
		{
			if (pEngine->pImageVerify->EnableVerification(iFactor) > 0)
			{
				Message("Image verification set");
				return true;
			}
			else
			{
				iErrorCode = GENERAL_FAILURE;
				ErrorMessage("No images available for verification");
			}
		}

		// disable on error
		pEngine->pImageVerify->DisableVerification();
	}

	ErrorMessage("Image verification could not be set");
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////
// Gets a 'snapshot' of the screen of the remote target and returns the information
// and data in the supplied parameters. Only stores the most recent snapshot.
//
STATDLLMember::GetSnapshot(TBitmapFileHeader **ppFile, TBitmapInfoHeader **ppBitmap, char **ppData, unsigned long *pSize)
{
	// basic screenshot command
	CString file = "<B><S><E>";

	iErrorCode = GENERAL_FAILURE;
	*ppFile = NULL;
	*ppBitmap = NULL;
	*ppData = NULL;
	*pSize = NULL;

	if (pEngine)
	{
		Message("Sending snapshot command...");
		if ((iErrorCode = pEngine->OpenScriptFile(file, false)) == ITS_OK)
		{
			pEngine->pConverter->bWriteToFile = false;
			if ((iErrorCode = pEngine->RunScript(NULL)) == END_SCRIPT)
			{
				Message("Accessing bitmap data...");
				pEngine->pConverter->GetScreenshotData(ppFile, ppBitmap, ppData, pSize);
			}
			pEngine->pConverter->bWriteToFile = true;
		}
	}

	// success
	if (ppFile && (*ppFile) && ppBitmap && (*ppBitmap) && ppData && (*ppData) && pSize && (*pSize))
	{
		Message("Bitmap data retrieved successfully");
		return true;
	}

	ErrorMessage("Snapshot could not be taken");
	return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Converts a char * to it's Unicode equivalent
//
LPTSTR
STATDLLMember::ToUnicode(const char *string)
{
#ifdef UNICODE
	static TCHAR szBuffer[MAX_UNICODE_LEN + 1] = {0};
	szBuffer[0] = (TCHAR)0;

    // Convert to UNICODE.
    if (!MultiByteToWideChar(CP_ACP,					// conversion type
							 0,							// flags
							 string,					// source
							 -1,						// length
							 szBuffer,					// dest
							 MAX_UNICODE_LEN))			// length
    {
        return _T("Could not convert");
    }

    return szBuffer;
#else
	return (LPTSTR)string;
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
// Converts a Unicode to it's char * equivalent
//
const char *
STATDLLMember::ToAnsi(LPCTSTR string)
{
#ifdef UNICODE
	static char szBuffer[MAX_UNICODE_LEN + 1] = {0};
	szBuffer[0] = (char)0;

    // Convert to ANSI.
    if (!WideCharToMultiByte(CP_ACP,					// conversion type
							 0,							// flags
							 string,					// source
							 -1,						// length
							 szBuffer,					// dest
							 MAX_UNICODE_LEN,			// length
							 NULL,
							 NULL ))
    {
        return "Could not convert";
    }

    return szBuffer;
#else
	return (char *)string;
#endif
}


//////////////////////////////////////////////////////////////////////////////////////
// Split a connection registry entry into its respective parts
void
STATDLLMember::ParseConnection(char *pConnection, STATCONNECTTYPE *pType, char **ppAddress)
{
	(*pType) = SymbianInvalid;
	(*ppAddress) = NULL;

	static char szConnection[256];
	memset(szConnection, 0, sizeof(szConnection));
	strcpy(szConnection, pConnection);

	char *p = strchr(szConnection, ':');
	if (p)
	{
		(*p) = (char)0;
		(*ppAddress) = p + 1;

		if (stricmp(szConnection, "SymbianSocket") == 0)
		{
			(*pType) = SymbianSocket;
		}
		else if (stricmp(szConnection, "SymbianSerial") == 0)
		{
			(*pType) = SymbianSerial;
		}
		else if (stricmp(szConnection, "SymbianInfrared") == 0)
		{
			(*pType) = SymbianInfrared;
		}
		else if (stricmp(szConnection, "SymbianBluetooth") == 0)
		{
			(*pType) = SymbianBluetooth;
		}

		(*p) = ':';
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
// Writes a message to file
//
void
STATDLLMember::Message(const char * message, ...)
{
	if (pRep)
	{
		char szText[MAX_ERROR_MSG_LEN * 2] = {0};
		va_list pMsg;

		strcpy(szText, szPrefix);

		va_start (pMsg, message);
		vsprintf (szText + strlen(szText), message, pMsg);
		va_end (pMsg);

		pRep->info(szText);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// Writes an error message to file
//
void
STATDLLMember::ErrorMessage(const char * message, ...)
{
	if (pRep)
	{
		va_list pMsg;
		char szErrorMsg[MAX_ERROR_MSG_LEN + 1] = {0};

		// get the API error
		va_start (pMsg, message);
		vsprintf (szErrorMsg, message, pMsg);
		va_end (pMsg);

		// save the complete error
		if (pEngine)
		{
			if (pEngine->GetDeviceReturnCode() != 0)
			{
				sprintf(szErrorText, "%s (%d : %s - Device return code %d)", 
										szErrorMsg, 
										iErrorCode, 
										ToAnsi(pEngine->GetErrorText(iErrorCode)),
										pEngine->GetDeviceReturnCode());
			}
			else
			{
				sprintf(szErrorText, "%s (%d : %s)", 
										szErrorMsg, 
										iErrorCode, 
										ToAnsi(pEngine->GetErrorText(iErrorCode)));
			}
		}
		else
			strcpy(szErrorText, "(STAT Engine not initialised)");

		// construct a full log message
		char szLogError[MAX_ERROR_MSG_LEN * 2] = {0};
		sprintf(szLogError, "%sERROR: %s", szPrefix, szErrorText);
		pRep->error(szLogError);
	}
}
