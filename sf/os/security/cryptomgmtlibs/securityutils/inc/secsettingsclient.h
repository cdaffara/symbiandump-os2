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
* Client-side API via which the Central Repository server is accessed.
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef SECSETTINGSCLIENT_H
#define SECSETTINGSCLIENT_H


#include <scs/scsclient.h>
#include <securityerr.h>


namespace SecuritySettingsServer
	{


class RSecSettingsSession : public RScsClientBase
/**
	This class is used to establish a connection with
	the Central Repository server.
 */
	{
public:
	IMPORT_C RSecSettingsSession();
	IMPORT_C TInt Connect();
	IMPORT_C void Close();
	
	IMPORT_C TInt SettingValueL(TUid aUidRepository, TUint32 aKey);
	};


	} // End of namespace SecuritySettingsServer

#endif	// #ifndef SECSETTINGSCLIENT_H

