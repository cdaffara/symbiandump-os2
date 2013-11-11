/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

#include <e32base.h>
#include <usb/acmserver.h>
#include "acmserverclient.h"
#include "acmserverconsts.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "acmserverclientTraces.h"
#endif

/** Constructor */
RAcmServerClient::RAcmServerClient() 
	{
	OstTraceFunctionEntry0( RACMSERVERCLIENT_RACMSERVERCLIENT_CONS_ENTRY );
	
	OstTraceFunctionExit0( RACMSERVERCLIENT_RACMSERVERCLIENT_CONS_EXIT );
	}
	   
/** Destructor */
RAcmServerClient::~RAcmServerClient()
	{
	OstTraceFunctionEntry0( RACMSERVERCLIENT_RACMSERVERCLIENT_DES_ENTRY);
	
	OstTraceFunctionExit0( RACMSERVERCLIENT_RACMSERVERCLIENT_ENTRY_DES_EXIT );
	}

/**
Getter for the version of the server.
@return Version of the server
*/
TVersion RAcmServerClient::Version() const
	{
	OstTraceFunctionEntry0( RACMSERVERCLIENT_VERSION_ENTRY );
	

	return TVersion(	KAcmSrvMajorVersionNumber,
						KAcmSrvMinorVersionNumber,
						KAcmSrvBuildNumber
					);
	}

/**
Connect the handle to the server.
Must be called before all other methods (except Version and Close).
@return Symbian error code
*/
TInt RAcmServerClient::Connect()
	{
	OstTraceFunctionEntry0( RACMSERVERCLIENT_CONNECT_ENTRY );
	

	return CreateSession(KAcmServerName, Version(), 1);
	}

TInt RAcmServerClient::CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum, const TDesC& aAcmControlIfcName, const TDesC& aAcmDataIfcName)
	{
	OstTraceFunctionEntry0( RACMSERVERCLIENT_CREATEFUNCTIONS_ENTRY );
	OstTraceExt4( TRACE_NORMAL, RACMSERVERCLIENT_CREATEFUNCTIONS, "RAcmServerClient::CreateFunctions;aNoAcms=%d;aProtocolNum=%d;aAcmControlIfcName=%S;aAcmDataIfcName=%S", aNoAcms, aProtocolNum, aAcmControlIfcName, aAcmDataIfcName );
	
	TIpcArgs args;
	args.Set(0, aNoAcms);
	args.Set(1, aProtocolNum);
	args.Set(2, &aAcmControlIfcName);
	args.Set(3, &aAcmDataIfcName);
	return SendReceive(EAcmCreateAcmFunctions, args);
	}

TInt RAcmServerClient::DestroyFunctions(const TUint aNoAcms)
	{
	OstTraceFunctionEntry0( RACMSERVERCLIENT_DESTROYFUNCTIONS_ENTRY );
	OstTrace1( TRACE_NORMAL, RACMSERVERCLIENT_DESTROYFUNCTIONS, "RAcmServerClient::DestroyFunctions;aNoAcms=%d", aNoAcms );
	return SendReceive(EAcmDestroyAcmFunctions, TIpcArgs(aNoAcms));
	}
