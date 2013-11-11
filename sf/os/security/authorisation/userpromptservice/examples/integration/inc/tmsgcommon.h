/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Example Code
* No changes necessary to support User Prompt Service
*
*/


/**
 @file
 @internalComponent
 @test
*/

#ifndef TMSGCOMMON_H_
#define TMSGCOMMON_H_

#include <e32cmn.h>
#include <e32std.h>

// The User Prompt Services requires systems servers to
// have the ProtServ capability
_LIT(KMsgServerName, "!TMsgServer");
_LIT(KMsgServerImg,"TMsgServer.exe");
static const TUid KMsgServerUid3 = {0x01041000};
static const TInt KMsgServerVersionMajor = 1;
static const TInt KMsgServerVersionMinor = 0;
static const TInt KMsgServerVersionBuild = 0;

enum TMsgServerMessages
	{
	ESendMsg,
	ECancelSendMsg,
	ESeperatorEndAll
	};



#endif /* TMSGCOMMON_H_*/
