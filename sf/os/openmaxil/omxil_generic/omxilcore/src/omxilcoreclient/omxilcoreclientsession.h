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
// omilcoreclientsession.h
// 
//

#ifndef OMXILCORECLIENTSESSION_H
#define OMXILCORECLIENTSESSION_H

#include <e32base.h>
#include "omxilcoreclientserver.h"

class COmxILCore;

/**
   Client-side of a an IL Client - IL Core communication
 */
NONSHARABLE_CLASS(ROmxILCoreClientSession) : public RSessionBase
	{

public:

	ROmxILCoreClientSession();
	~ROmxILCoreClientSession();

	TInt Connect();
	TInt Open();
	void Close();

	// Retrieves an IL Core reference from the IL Core thread 
	COmxILCore& ILCore();

	//
	// OMX IL Operations...
	//

	OMX_ERRORTYPE ListLoaders();

	OMX_ERRORTYPE DeinitAndClose();

	OMX_ERRORTYPE ComponentNameEnum(
	    OMX_STRING cComponentName,
		OMX_U32 nNameLength,
		OMX_U32 nIndex);

	OMX_ERRORTYPE GetHandle(
	    OMX_HANDLETYPE* pHandle,
	    OMX_STRING cComponentName,
	    OMX_PTR pAppData,
	    OMX_CALLBACKTYPE* pCallBacks);

	OMX_ERRORTYPE FreeHandle(
	    OMX_HANDLETYPE hComponent);

	OMX_ERRORTYPE SetupTunnel(
	    OMX_HANDLETYPE hOutput,
	    OMX_U32 nPortOutput,
	    OMX_HANDLETYPE hInput,
	    OMX_U32 nPortInput);

	OMX_ERRORTYPE GetContentPipe(
	    OMX_HANDLETYPE *hPipe,
	    OMX_STRING szURI);

	OMX_ERRORTYPE GetComponentsOfRole (
		OMX_STRING role,
	    OMX_U32 *pNumComps,
	    OMX_U8  **compNames);

	OMX_ERRORTYPE GetRolesOfComponent (
		OMX_STRING compName,
		OMX_U32 *pNumRoles,
		OMX_U8 **roles);

	};

#endif // OMXILCORECLIENTSESSION_H
