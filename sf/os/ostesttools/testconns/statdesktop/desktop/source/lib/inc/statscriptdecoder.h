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




#ifndef STATSCRIPTDECODER_H
#define STATSCRIPTDECODER_H

#include "stat.h"
#include "CSTATDataFormatConverter.h"
#include "CSTATImageVerify.h"

class CSTATScriptCommand
{
public:
	CSTATScriptCommand();
	~CSTATScriptCommand();
	char *Command()			{ return pCommand; }
	unsigned long Length()	{ return ulLength; }
	bool SetData(const char *pData, const unsigned long theLength);
	void Reset();

	char cCommandID;
	unsigned long ulLength;

private:
	char *pCommand;
};

class CSTATScriptDecoder
{
public:
	CSTATScriptDecoder(CSTATLogFile *theLog);
	~CSTATScriptDecoder();
	int Initialise(const CString& file, bool bIsFile);
	int GetNextCommand(CSTATScriptCommand **ppCommand);
	void Release();

private:
	int ReadDataIntoMemory(const CString& commands);
	int ReadFileIntoMemory(CFile& scriptdata);
	int ValidateScript();
	int ExtractComment();
	bool IncrementCounter(void);

	CSTATLogFile *pLog;
	char *lpBuffer;										// buffer that holds read script file
	long lFileLength;									// length of file read
	int iCounter;										// step counter through file
	CSTATScriptCommand LastCommand;
};

#endif
