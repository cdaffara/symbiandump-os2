/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




/**
 @file
*/

#ifndef __T_TCERTSTOREDEFS_H__
#define __T_TCERTSTOREDEFS_H__

#include <e32std.h>
#include <e32base.h>

// action types
_LIT8(KInitManager, "InitManager");
_LIT8(KDestroyManager, "DestroyManager");
_LIT8(KClearAllApps, "ClearAllApps");
_LIT8(KAddApp, "AddApp");
_LIT8(KRemoveApp, "RemoveApp");
_LIT8(KGetApp, "GetApp");
_LIT8(KAppCount, "AppCount");
_LIT8(KGetApplications, "GetApplications");

// tags
_LIT8(KReturn, "return");
_LIT8(KUid, "uid");
_LIT8(KAppName, "appname");
_LIT8(KCount, "count");

#endif	//	__T_TCERTSTOREDEFS_H__
