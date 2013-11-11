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
* Executable entrypoint and server object factory function.
* The latter is supplied to the SCS boilerplate code.
*
*/


/**
 @file
*/

#include "secsettingsserver.h"


static CScsServer* NewSecSettingsServerLC()
/**
	This factory function is called by SCS.  It allocates
	the server object and leaves it on the cleanup stack.

	@return					New initialized instance of CScsTestServer.
							On return this is on the cleanup stack.
 */
	{
	return SecuritySettingsServer::CSecSettingsServer::NewLC();
	}

TInt E32Main()
/**
	Executable entrypoint.

	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	return StartScsServer(NewSecSettingsServerLC);
	}

