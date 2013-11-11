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




#ifndef STATLIST30_H
#define STATLIST30_H

#include "statmember.h"

#define MAX_VERSION_LEN 32

class STATDLLList
{
public:
	STATDLLList();
	~STATDLLList();
	int SetLogging(const char *path);
	void StopLogging();
	DWORD CreateListMember(const STATCONNECTTYPE iConnectType, const char *pszPlatformType);
	STATDLLMember* Member(const DWORD dwHandle);
	bool RemoveListMember(const DWORD dwHandle);
	bool ListExists() { return (ListStart != 0); }
	char *Version() { return szVersion; }

	char szErrorText[MAX_ERROR_MSG_LEN * 2];	// connection error message (if any)

private:
	void WalkList();

	Reporter oRep;								// debug logging
	STATDLLMember *ListStart;					// start of connection list
	char szVersion[MAX_VERSION_LEN];
};

// stat connection list
extern STATDLLList aList;

#endif // STATLIST30_H
