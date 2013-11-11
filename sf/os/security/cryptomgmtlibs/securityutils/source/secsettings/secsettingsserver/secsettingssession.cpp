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
* Implements CSecSettingsSession.	 See class and function definitions for
* more information.
*
*/


/**
 @file
*/

#include "secsettingsserver.h"
#include <scs/ipcstream.h>
#include <scs/nullstream.h>

// For KErrSettingNotFound.
#include<securityerr.h>

// For CRepository class.
#include <centralrepository.h>


namespace SecuritySettingsServer
{

CSecSettingsSession* CSecSettingsSession::NewL(CSecSettingsServer &aServer)
/**
	Factory function allocates new instance of CSecSettingsSession.

	@return					New, initialized instance of CSecSettingsSession
							which is owned by the caller.
 */
	{
	CSecSettingsSession* self = new(ELeave) CSecSettingsSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();			// CScsSession implementation
	CleanupStack::Pop(self);
	return self;
	}

CSecSettingsSession::CSecSettingsSession(CSecSettingsServer &aServer)
/**
	This private constructor prevents direct instantiation.
 */
 :	CScsSession(aServer)
	{
	}

CSecSettingsSession::~CSecSettingsSession()
	{
	}

TBool CSecSettingsSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CScsSession by handling the supplied message.

	@param	aFunction		Function identifier for the repository operations.
	@param	aMessage		Standard server-side handle to message.	 Not used.
 */
	{

	switch (aFunction)
		{
		case ESettingValue:
			{								
			// Create a CRepository object with the supplied UID.
			CRepository* repository = CRepository::NewL(TUid::Uid(aMessage.Int0()));
			TInt Value = 0;

				
			// Get the value of the setting whose key is supplied as the first argument. 
			TInt result = repository->Get(aMessage.Int1(), Value);

			delete repository;

			if( result == KErrNotFound )
			{
				// Interpreting this as - setting not found.
				User::Leave(KErrSettingNotFound);
			}
			else
			{
				User::LeaveIfError(result);
			}
			
			TPckgBuf<TInt> Pkg(Value);
			aMessage.WriteL(2, Pkg);

			break;
			}

		default:
			User::Leave(KErrNotSupported);
		}

	return ETrue;
	}



} // End of namespace SecuritySettingsServer
// End of file
