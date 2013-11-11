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




#ifndef STATTRANSPORT_H
#define STATTRANSPORT_H

#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include "CSTATReturnCodes.h"


// abstract transport class
class CSTATTransport
{
public:
	CSTATTransport();
	~CSTATTransport() {}

	// these must be derived for selected transport
	virtual int Initialise(void) = 0;
	virtual int Connect(const char *pAddress = NULL) = 0;
	virtual int Send(const char cIdentifier, const char *pData = NULL, const unsigned long ulLength = 0) = 0;
	virtual int Receive(char *cIdentifier, char **ppData = NULL, unsigned long *pLength = NULL) = 0;
	virtual int Disconnect(void) = 0;
	virtual int Release(void) = 0;
	virtual char * Error(void);
	virtual unsigned int GetMaxPacketSize(void) const = 0;
	
protected:
	void SetError(const char *pText, ...);

private:
	char szError[STAT_MAX_ERRORLEN + 1];
};


////////////////////////////////////////////////////////////////////////////////////////
// Constructor
inline CSTATTransport::CSTATTransport(void)
{
	memset(&szError, 0, sizeof(szError));
}


////////////////////////////////////////////////////////////////////////////////////////
// Return error text
inline char * CSTATTransport::Error(void)
{
	return szError;
}


////////////////////////////////////////////////////////////////////////////////////////
// Set error text
inline void CSTATTransport::SetError(const char *pText, ...)
{
	memset(&szError, 0, sizeof(szError));

	va_list pCurrent = (va_list)0;
    va_start (pCurrent, pText);
	vsprintf (szError, pText, pCurrent);
    va_end (pCurrent);

	char szExtra[20] = {0};
	sprintf(szExtra, " (%d)", GetLastError());
	strcat (szError, szExtra);
}


#endif
