// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef OMXILCORECLIENTSERVER_H
#define OMXILCORECLIENTSERVER_H

#include <e32std.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>

_LIT(KOmxILCoreServerName,"OmxILCoreServer");

// A larger stack is used due to Bellagio loader which uses ANSI C calls (fopen etc.). These calls require a large stack, and panic on ARM platforms if a default 8 KB stack is used 
const TInt KOmxILCoreServerStackSize=0x4000;			//  16KB

const TUint KOmxILCoreServerVersion = 1;
const TUint KOmxILCoreServerMinorVersionNumber = 0;
const TUint KOmxILCoreServerBuildVersionNumber = 0;

// Function opcodes for client to server messaging
enum TOmxILCoreClientToServerMessages
	{
	EOmxILCoreListLoaders = 0,
	EOmxILCoreDeinit,
	EOmxILCoreGetILCore,
	EOmxILCoreComponentNameEnum,
	EOmxILCoreGetHandle,
	EOmxILCoreFreeHandle,
	EOmxILCoreSetupTunnel,
	EOmxILCoreGetContentPipe,
	EOmxILCoreGetComponentsOfRole,
	EOmxILCoreGetRolesOfComponent
	};

enum TOmxILCoreServerState
	{
	EOmxILCoreServerStateIdle = 0
	};


class TComponentNameEnum
	{
public:
	OMX_STRING cComponentName;
	OMX_U32 nNameLength;
	OMX_U32 nIndex;
	};

class TGetHandle
	{
public:
	OMX_HANDLETYPE* pHandle;
	OMX_STRING cComponentName;
	OMX_PTR pAppData;
	OMX_CALLBACKTYPE* pCallBacks;
	};

class TSetupTunnel
	{
public:
	OMX_HANDLETYPE hOutput;
	OMX_U32 nPortOutput;
	OMX_HANDLETYPE hInput;
	OMX_U32 nPortInput;
	};

class TGetContentPipe
	{
public:
	OMX_HANDLETYPE* hPipe;
	OMX_STRING szURI;
	};

class TGetComponentsOfRole
	{
public:
	OMX_STRING role;
	OMX_U32 *pNumComps;
	OMX_U8  **compNames;
	};

class TGetRolesOfComponent
	{
public:
	OMX_STRING compName;
	OMX_U32 *pNumRoles;
	OMX_U8 **roles;
	};

IMPORT_C TInt StartOmxILCoreServer(TUint32 *aServerHandle);

#endif // OMXILCORECLIENTSERVER_H
