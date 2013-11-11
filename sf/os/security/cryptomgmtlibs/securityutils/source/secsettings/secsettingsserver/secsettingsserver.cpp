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
* Implements CSecSettingsServer.	See class and function definitions for
* more information.
*
*/


/**
 @file
*/

#include "secsettingsserver.h"
#include <e32property.h>
#include <sacls.h>

namespace SecuritySettingsServer
{
CSecSettingsServer* CSecSettingsServer::NewLC()
/**
	Factory function allocates new, initialized instance of
	CSecSettingsServer which is left on the cleanup stack.

	@return					New, initialized instance of CSecSettingsServer
							which is left on the cleanup stack.
 */
	{
	CSecSettingsServer* self = new(ELeave) CSecSettingsServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSecSettingsServer::CSecSettingsServer()
/**
	Initializes the superclass with this server's version.
 */
	:	CScsServer(SecuritySettingsServer::Version(), EPriorityStandard)
	{
	// empty.
	}

void CSecSettingsServer::ConstructL()
/**
	Second-phase construction initializes the superclass and
	starts the server.
 */
	{
	CScsServer::ConstructL(SecuritySettingsServer::KShutdownPeriodUs);

	StartL(SecuritySettingsServer::KSecSettingsServerName);
	}



CSecSettingsServer::~CSecSettingsServer()
/**
	Cleanup the server.
 */
	{
	}

CScsSession* CSecSettingsServer::DoNewSessionL(const RMessage2& /*aMessage*/)
/**
	Implement CScsServer by allocating a new instance of CSecSettingsSession.

	@param	aMessage		Standard server-side handle to message.	 Not used.
	@return					New instance of CSecSettingsSession which is owned by the
							caller.
 */
	{
	return CSecSettingsSession::NewL(*this);
	}



} // End of namespace SecuritySettingsServer
// End of file
