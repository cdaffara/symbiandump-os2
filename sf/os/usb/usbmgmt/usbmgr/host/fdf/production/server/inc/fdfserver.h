/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef FDFSERVER_H
#define FDFSERVER_H

#include <e32base.h>

class CFdfSession;
class CFdf;

NONSHARABLE_CLASS(CFdfServer) : public CServer2
	{
public:
	static void NewLC();
	~CFdfServer();

public: // called by session objects
	void SessionClosed();

private:
	CFdfServer();
	void ConstructL();
	
private: // from CPolicyServer
	/**
	Called by the base class to create a new session.
	@param aVersion Version of client
	@param aMessage Client's IPC message
	@return A new session to be used for the client. If this could not be made, 
	this function should leave.
	*/
	CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;

private: // unowned
	CFdfSession* iSession;

private: // owned
	CFdf* iFdf;
	};

#endif // FDFSERVER_H
