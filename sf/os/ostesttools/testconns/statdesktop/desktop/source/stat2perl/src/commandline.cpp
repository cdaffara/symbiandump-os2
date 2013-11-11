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
#include "CommandLine.h"

CommandLine::CommandLine()
: iCount(0), pCmdLine(NULL)
{
	char *pTmpCmdLine = GetCommandLine();

	// see if we have one
	if (pTmpCmdLine && *pTmpCmdLine)
	{
		char *pStart, *ptr;
		unsigned int len = strlen(pTmpCmdLine);

		// save a copy
		pCmdLine = new char[len + 1];
		strcpy(pCmdLine, pTmpCmdLine);

		// get a working copy
		pTmpCmdLine = new char[len + 1];
		strcpy(pTmpCmdLine, pCmdLine);
		ptr = pTmpCmdLine;

		// reset the array
		for (int i=0;i<256;i++)
			pArguments[i] = NULL;

		// limit of 256 arguments
		for (i=0;i<256;i++)
		{
			pStart = ptr;

			// step to start of argument
			while (*pStart && *pStart == ' ')
				pStart++;

			// step to end of argument
			ptr = pStart;
			while (*ptr && *ptr != ' ')
			{
				// if path is enclosed in quotes, we skip any spaces that might exist within them
				// NOTE: if a quote exists inside the path, we will lose the first part of the argument!
				if (*ptr == '"')
				{
					ptr++;
					pStart = ptr;					// don't want first quote in the returned argument
					while (*ptr && *ptr != '"')
						ptr++;

					if (*ptr == '"')
					{
						*ptr = (char)0;				// don't want last quote in the returned argument
					}
				}
				ptr++;
			}

			// not at the end so separate arguments
			if (*ptr)
			{
				(*ptr) = (char)0;
				ptr++;
			}

			if (*pStart)
			{
				// store argument
				pArguments[i] = new char[strlen(pStart) + 1];
				strcpy(pArguments[i], pStart);
				iCount++;
			}
			else
				break;
		}

		delete [] pTmpCmdLine;
	}
}


// return whole command line less the first arg (app name)
char * CommandLine::GetArgumentString()
{
	if (pCmdLine && *pCmdLine)
		return pCmdLine;

	return NULL;
}


// return an argument match
bool CommandLine::FindArgument(const char *pArg)
{
	bool valid = false;

	for (unsigned int i=0;i<iCount;i++)
	{
		if (strcmp(pArg, pArguments[i]) == 0)
		{
			valid = true;
			break;
		}
	}

	return valid;
}


// return the argument according to the supplied index
char * CommandLine::GetProgramArgument(const unsigned int iIndex)
{
	if (iIndex < iCount)
	{
		return pArguments[iIndex];
	}

	return NULL;
}


// find the argument that begins with the supplied 'switch' and return the value less the 'switch'
char * CommandLine::GetArgumentBySwitch(const char *pSwitch, const bool bIncludeSwitch)
{
	unsigned int len = strlen(pSwitch);

	for (unsigned int i=0;i<iCount;i++)
	{
		if (strncmp(pArguments[i], pSwitch, len) == 0)
		{
			if (bIncludeSwitch)
				return pArguments[i];
			else
				return pArguments[i] + len;
		}
	}

	return NULL;
}


