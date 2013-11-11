// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// define the shared client/server definitions and 
// services provided by the ECom client session
// 
//

#ifndef __ECOMSERVERSTART_H__
#define __ECOMSERVERSTART_H__

#include <e32base.h>

// Definitions for location & startup of the ECom server
// Define the ECom server name
// Shared client/server definitions and services provided by
// the ECom server

#include <e32std.h>

/** 
The name of the server 
@internalComponent
*/
_LIT(KEComServerName,"!ecomserver");
_LIT(KEComServerImg,"z:\\system\\libs\\ecomserver");	// DLL/EXE full path

/** 
The unique UID identifying this server implementation 
@internalComponent
*/
const TUid KEComServerUid3={0x10009D8F};

/** 
The ECom server Major version number 
@internalComponent
*/
const TInt KEComServerMajorVN=1;
/** 
The ECom server Minor version number 
@internalComponent
*/
const TInt KEComServerMinorVN=0;
/** 
The ECom server Build version number 
@internalComponent
*/
const TInt KEComServerBuildVN=0;

/**
@fn				ServerStart()
@internalComponent
Intended Usage	: Called from E32Main to launch server	
Error Condition	: None.
@since			9.2
@return			A system wide error code indicating the success of the server launch
 */
TInt ServerStart();

#endif
