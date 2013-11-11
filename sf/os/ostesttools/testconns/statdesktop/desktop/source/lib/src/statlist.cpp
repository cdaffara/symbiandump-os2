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
#include "statlist.h"

////////////////////////////////////////////////////////////////////////////////////////
// The one and only connection list
STATDLLList aList;

//////////////////////////////////////////////////////////////////////////////////////////
// Constructor
STATDLLList::STATDLLList()
: ListStart(0)
{
	InitializeCriticalSection(&CriticalSection);

	memset(szVersion, 0, sizeof(szVersion));
	sprintf(szVersion, "%s.%s.%s", STAT_VERSION_MAJOR, STAT_VERSION_MINOR, STAT_VERSION_PATCH);

}


//////////////////////////////////////////////////////////////////////////////////////////
// Destructor
//
STATDLLList::~STATDLLList()
{
	// release list members (passing 0 will return the last member in the list)
	STATDLLMember* pMember = Member(0);
	while(pMember)
	{
		RemoveListMember((DWORD)pMember);
		pMember = Member(0);
	}

	StopLogging();
	DeleteCriticalSection(&CriticalSection);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Turns on logging for STAT multiple connection management.
//
int
STATDLLList::SetLogging(const char *path)
{
	bool valid = false;
	if (!oRep.active())
	{
		// split path into folder/file
		char folder[MAX_PATH + 1] = {0};
		strcpy(folder, path);
		char *ptr = strrchr(folder, '\\');
		if (ptr)
		{
			char file[MAX_PATH + 1] = {0};
			strcpy(file, ptr + 1);
			(*ptr) = (char)0;

			if (*file)
			{
				if (oRep.init(STAT_APPNAME, RPT_ALL, RPT_FILE, folder, file))
				{
					oRep.dash();
					oRep.header("%s", STAT_APPNAME);
					valid = true;
				}
				else
					strcpy(szErrorText, "Connection logging failed (file system error).");
			}
			else
				strcpy(szErrorText, "Connection logging failed (invalid filename).");
		}
		else
			strcpy(szErrorText, "Connection logging failed (invalid path).");
	}
	else
		strcpy(szErrorText, "Connection logging is already initialised.");

	return valid;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Turns off logging
//
void
STATDLLList::StopLogging()
{
	if (oRep.active())
		oRep.kill();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Open a connection with a new list member
//
DWORD
STATDLLList::CreateListMember(const STATCONNECTTYPE iConnectType, const char *pszPlatformType)
{
	bool valid = false;

	(*szErrorText) = (char)0;
	oRep.info("Creating new list member (%d : %s)...", iConnectType, pszPlatformType);

	STATDLLMember *ptr = new STATDLLMember(iConnectType, pszPlatformType, &oRep);
	if (ptr)
	{
		if (ptr->GetErrorCode() == ITS_OK)
		{
			// add to list
			if (ListStart)
			{
				ptr->lPrevConnection = Member(0);				// returns last member in list
				ptr->lPrevConnection->lNextConnection = ptr;
				ptr->lNextConnection = NULL;
			}
			else
			{
				ListStart = ptr;
			}

//			WalkList();

			oRep.info("Member %ld (%d : %s) created", ptr, iConnectType, pszPlatformType);
			valid = true;
		}
		else
		{
			// save the error thrown
			strcpy(szErrorText, ptr->GetErrorText());
			delete ptr;
			ptr = NULL;
		}
	}

	if (!ptr || !valid)
		oRep.error("ERROR: Could not create new member");

	return (DWORD)ptr;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Find and return a member connection
// If 0 or bad pointer is passed, will return the last position in the list
// If nothing in list, will return NULL
//
STATDLLMember*
STATDLLList::Member(const DWORD dwHandle)
{
	STATDLLMember *current = 0, *next = 0;
	next = ListStart;
	while (next)
	{
		current = next;
		if (current == (STATDLLMember *)dwHandle)
			break;

		next = current->lNextConnection;
	}

	return current;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Close a member connection
//
bool
STATDLLList::RemoveListMember(const DWORD dwHandle)
{
	STATDLLMember *current = Member(dwHandle);
	oRep.info("Removing list member %ld...", dwHandle);
	bool valid = false;

	// remove from list
	if (current)
	{
		// something before
		if (current->lPrevConnection)
			current->lPrevConnection->lNextConnection = current->lNextConnection;
		else
			ListStart = current->lNextConnection;

		// something after
		if (current->lNextConnection)
			current->lNextConnection->lPrevConnection = current->lPrevConnection;

		delete current;
		oRep.info("Member removed");

//		WalkList();

		valid = true;
	}
	else
		oRep.error("ERROR: Could not remove");

	return valid;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
// walks the list for debug purposes
//
void
STATDLLList::WalkList()
{
	if (ListStart)
		oRep.dash();

	STATDLLMember *current = 0, *next = 0;
	next = ListStart;
	while (next)
	{
		current = next;

		oRep.info("Member details: Previous:%ld this:%ld Next:%ld",
			current->lPrevConnection, current, current->lNextConnection);

		next = current->lNextConnection;
	}

	if (ListStart)
		oRep.dash();
}
