/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Server-side classes which are used to implement the SecSettings server.
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef SECSETTINGSSERVER_H
#define SECSETTINGSSERVER_H

#include <f32file.h>
#include <scs/scsserver.h>
#include "secsettingscommon.h"


namespace SecuritySettingsServer
{

inline TVersion Version();

// forward declarations
class CSecSettingsServer;


class CSecSettingsSession : public CScsSession
/**
	This session object is used to generate sessions for the client
	which wants to access the SecSettings Server.
 */
	{
public:
	static CSecSettingsSession* NewL(CSecSettingsServer &aServer);
	virtual ~CSecSettingsSession();

	// implement CScsSession
	virtual TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);

	inline CSecSettingsServer *SecSettingsServer();

private:
	CSecSettingsSession(CSecSettingsServer &aServer);
	};



class CSecSettingsServer : public CScsServer
/**
	A single instance of this object is used to generate server-side
	sessions for the SecSettings client.
 */
	{
public:
	static CSecSettingsServer* NewLC();
	virtual ~CSecSettingsServer();

	// implement CScsServer
	virtual CScsSession* DoNewSessionL(const RMessage2& aMessage);	
	
private:
	CSecSettingsServer();
	void ConstructL();
	};

//
// Inline functions
//
inline CSecSettingsServer *CSecSettingsSession::SecSettingsServer()
	{
	// Calls base class function and cast to correct type.
	return &static_cast<CSecSettingsServer &>(iServer);
	}

} // End of namespace SecuritySettingsServer


#endif	// #ifndef SECSETTINGSSERVER_H

