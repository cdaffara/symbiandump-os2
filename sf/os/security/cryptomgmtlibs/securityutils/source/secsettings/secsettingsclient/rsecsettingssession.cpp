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
* RSecSettingsSession implementation.	 See class and function definitions
* for more detail.
*
*/


/**
 @file
*/


#include <secsettings/secsettingsclient.h>
#include <e32property.h>
#include <e32debug.h>
#include "secsettingscommon.h"

namespace SecuritySettingsServer
	{


EXPORT_C RSecSettingsSession::RSecSettingsSession()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
:	RScsClientBase()
	{
	// empty.
	}

EXPORT_C TInt RSecSettingsSession::Connect()
/**
	Connect to the SecSettings server.

	The thread which calls this function must be the same one which later calls Close().

	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	TVersion v = SecuritySettingsServer::Version();
	TUidType serverFullUid = SecuritySettingsServer::ServerImageFullUid();

	TInt r = RScsClientBase::Connect(
	        SecuritySettingsServer::KSecSettingsServerName, v, SecuritySettingsServer::KServerImageName, serverFullUid);
	

	if(r != KErrNone)
		{
		Close();
		}
	
	return r;
	}
	

	

EXPORT_C void RSecSettingsSession::Close()
/**
Frees the configuration object and calls RScsClientBase::Close

This function MUST be called by the same thread which Connect().
*/
	{
	RScsClientBase::Close();
	}


EXPORT_C TInt RSecSettingsSession::SettingValueL(TUid aUidRepository, TUint32 aKey)
/**
 * Retrieves the value of the setting from the repository.
 *
 * @param	aUidRepository		UID of the Central Repository.
 * @param	aKey				32-bit unsigned Key identifying the setting. 
 * @return						Value of the setting.
 */
    {    	
    TPckgBuf<TInt> p;
    TIpcArgs args(aUidRepository.iUid, aKey, &p);  
        
    User::LeaveIfError(CallSessionFunction(ESettingValue, args));

    TInt Value = p();
    
    return Value;
    }


	} // End of namespace SecuritySettingsServer.

// End of file

