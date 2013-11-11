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
#include "statscriptdecoder.h"

//----------------------------------------------------------------------------
// Contructor
CSTATScriptCommand::CSTATScriptCommand()
: cCommandID(0), pCommand(NULL), ulLength(0)
{
}

//----------------------------------------------------------------------------
// Destructor
CSTATScriptCommand::~CSTATScriptCommand()
{
	Reset();
}


//----------------------------------------------------------------------------
// Set command data
bool CSTATScriptCommand::SetData(const char *pData, const unsigned long theLength)
{
	Reset();

	if (pData && theLength)
	{
		pCommand = new char[theLength + 1];
		if (!pCommand)
			return false;

		memcpy(pCommand, pData, theLength);
		*(pCommand + theLength) = (char)0;
		ulLength = theLength;
	}

	return true;
}


//----------------------------------------------------------------------------
// Destructor
void CSTATScriptCommand::Reset()
{
	if (pCommand)
	{
		delete [] pCommand;
		pCommand = NULL;
	}

	ulLength = 0;
}



////////////////////////////////////////////////////////////////////////////////////////
// Script Decoder class
////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Constructor
CSTATScriptDecoder::CSTATScriptDecoder(CSTATLogFile *theLog)
: lFileLength(0), iCounter(0),
  lpBuffer(NULL)
{
	pLog = theLog;
}


//----------------------------------------------------------------------------
// Destructor
CSTATScriptDecoder::~CSTATScriptDecoder()
{
	Release();
}


//----------------------------------------------------------------------------
// Read the script into memory
int
CSTATScriptDecoder::Initialise(const CString& file, bool bIsFile)
{
	lpBuffer = NULL;
	int ret = OPEN_SCRIPT_FILE_COMPLETE_FAILURE;

	// read the commands into memory
	if (bIsFile)
	{
		CFile script_file;
		if (script_file.Open(file, CFile::modeRead))
		{
			ret = ReadFileIntoMemory(script_file);
			script_file.Abort();
		}
	}
	else
		ret = ReadDataIntoMemory(file);

	// make sure we have valid start/end points in the script
	if (ret == ITS_OK)
		ret = ValidateScript();

	if (ret == ITS_OK)
	{
		pLog->WriteTimeToLog();

		if(bIsFile)
		{
			CString cBuffer = _T("Script file is : ");
			cBuffer += file;
			pLog->Write(cBuffer);
		}
		else
			pLog->Write(_T("Interpreting command script..."));

		ret = ITS_OK;
	}
	else
		Release();

	return ret;
}


//----------------------------------------------------------------------------
// Release resources
void CSTATScriptDecoder::Release()
{
	LastCommand.Reset();

	// release the script
	if (lpBuffer && lFileLength)
	{
		delete [] lpBuffer;
		lpBuffer = NULL;
		lFileLength = iCounter = 0;
	}
}


//----------------------------------------------------------------------------
// get the contents of a single command
int
CSTATScriptDecoder::GetNextCommand(CSTATScriptCommand **ppCommand)
{
	// get to the start of the command
	while (lpBuffer[iCounter] != '<')
	{
		// comment
		if (lpBuffer[iCounter] == '/' && lpBuffer[iCounter + 1] == '*')
		{
			if (ExtractComment())
			{
				*ppCommand = &LastCommand;
				return LastCommand.cCommandID;
			}

			return 0;
		}

		if (!IncrementCounter())
			return 0;
	}

	// step over the '<'
	if (!IncrementCounter())
		return 0;

	// save the command ID
	LastCommand.cCommandID = (char)toupper(lpBuffer[iCounter]);
	if (!IncrementCounter())
		return 0;

	// need to step over 'image' with '#image' command
	if (LastCommand.cCommandID == '#')
	{
		while (lpBuffer[iCounter] != ' ')
			if (!IncrementCounter())
				return 0;

		// step over the space
		if (!IncrementCounter())
			return 0;
	}

	int iBeginning = iCounter;

	// extract the command
	while (lpBuffer[iCounter] != '>')
		if (!IncrementCounter())
			return 0;

	// save our command
	LastCommand.SetData(lpBuffer + iBeginning, iCounter - iBeginning);

	// step over the '>'
	IncrementCounter();

	// set pointer to the command
	*ppCommand = &LastCommand;
	return LastCommand.cCommandID;
}


//----------------------------------------------------------------------------
// PRIVATE METHODS
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//reads string data into memory so that it can be decoded
int CSTATScriptDecoder::ReadDataIntoMemory(const CString& commands)
{
	// Convert CString to char *
	lFileLength = commands.GetLength();		//set length
	lpBuffer = new char [lFileLength + 1];		//allocate temporary buffer
	if (lpBuffer)
	{
		if (!pLog->ToAnsi(commands, 
								  lpBuffer, 
								  lFileLength))
		{
			return INTERPRET_COMMANDS_FAILURE;		//unable to convert to 8-bit
		}

		lpBuffer[lFileLength] = 0;
		return ITS_OK;
	}

	return E_OUTOFMEM;
}
//----------------------------------------------------------------------------
//reads file data into memory so that it can be decoded
int CSTATScriptDecoder::ReadFileIntoMemory(CFile& scriptdata)
{
	try
	{
		lFileLength = scriptdata.GetLength();		//file length
		lpBuffer = new char [lFileLength + 1];		//allocate temporary buffer
		if (lpBuffer)
		{
			scriptdata.Read(lpBuffer, lFileLength);	//read in from the file and store in the buffer
			lpBuffer[lFileLength] = 0;
			return ITS_OK;
		}
	}
	catch(CFileException *e)
	{
		e -> Delete();
		return OPEN_SCRIPT_FILE_COMPLETE_FAILURE;
	}

	return E_OUTOFMEM;
}


//----------------------------------------------------------------------------
// checks that start and end codes exist in the script
// on success, iCounter will be left ready for the first STAT command
int CSTATScriptDecoder::ValidateScript()
{
	CSTATScriptCommand *pCommand;
	bool bFound = false;

	// reset
	iCounter = 0;

	// look for end
	while (GetNextCommand(&pCommand))
	{
		if (pCommand->cCommandID == 'E')
		{
			bFound = true;
			break;
		}
	}

	if (bFound)
	{
		// reset
		iCounter = 0;
		bFound = false;

		// look for begin
		while (GetNextCommand(&pCommand))
		{
			if (pCommand->cCommandID == 'B')
			{
				bFound = true;
				iCounter -= 3;	// step back so this is the first command sent
				break;
			}
		}
	}

	if (!bFound)
		return BEGIN_COMMAND_FAILURE;

	return ITS_OK;
}


//----------------------------------------------------------------------------
// get a comment
int
CSTATScriptDecoder::ExtractComment()
{
	int iBeginning = iCounter;

	LastCommand.cCommandID = lpBuffer[iCounter];
	iCounter += 2;

	while (((iCounter + 1) < lFileLength) &&
		   !(lpBuffer[iCounter] == '*' && lpBuffer[iCounter + 1] == '/') )
	{
		if (!IncrementCounter())
			return 0;
	}

	// get the last bit of the comment '*/'
	if ((iCounter + 1) < lFileLength)
		iCounter += 2;

	// save our comment
	LastCommand.SetData(lpBuffer + iBeginning, iCounter - iBeginning);
	return LastCommand.cCommandID;
}


//----------------------------------------------------------------------------
// increment counter but make sure we don't go over the end of the script
bool CSTATScriptDecoder::IncrementCounter(void)
{
	return (++iCounter < lFileLength);
}
