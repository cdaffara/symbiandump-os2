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




#ifndef STATCOMMS_H
#define STATCOMMS_H

#include "statcommon.h"
#include "stattransport.h"

class STATComms : public CSTATTransport
{
public:
	STATComms();
	~STATComms();

	int SetTransport(const STATCONNECTTYPE eConnect);
	int Initialise(void);
	int Connect(const char *pAddress = NULL);
	int Send(const char cIdentifier, const char *pData = NULL, const unsigned long ulLength = 0);
	int Receive(char *cIdentifier, char **ppData = NULL, unsigned long *pLength = NULL);
	int Disconnect(void);
	int Release(void);
	char * Error(void);
	unsigned int GetMaxPacketSize(void) const;	// Get the maximum size
												// of each data paacket we transfer
												// in one go.
												// Sub-classes will override this
												// if tey support breaking up of
												// packets.

private:
	CSTATTransport *pTransport;			// pointer to the selected transport
};

/////////////////////////////////////////////////////////////////////////////
// STATComms::GetMaxPacketSize
// Our implementation checks if we have a transport component
// and delegates to that object or returns the base class
// implementation.
inline unsigned int STATComms::GetMaxPacketSize(void) const
{
	return (pTransport->GetMaxPacketSize());
}

#endif
