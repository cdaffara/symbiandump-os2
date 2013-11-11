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
#include "acmserverimpl.h"
#include "acmserverconsts.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "acmserverTraces.h"
#endif

#ifdef _DEBUG
/** Panic category for users of RAcmServer. */
_LIT(KAcmSrvPanicCat, "ACMSVR");
#endif


/** Panic codes for users of RAcmServer. */
enum TAcmServerClientPanic
	{
	/** The handle has not been connected. */
	EPanicNotConnected = 0,
	
	/** The handle has already been connected. */
	EPanicAlreadyConnected = 1,

	/** The client has requested to instantiate zero ACM functions, which 
	makes no sense. */
	EPanicCantInstantiateZeroAcms = 2,
	
	/** The client has requested to destroy zero ACM functions, which makes no 
	sense. */
	EPanicCantDestroyZeroAcms = 3,

	/** Close has not been called before destroying the object. */
	EPanicNotClosed = 4,
	};

EXPORT_C RAcmServer::RAcmServer() 
 :	iImpl(NULL)
	{
	OstTraceFunctionEntry0( RACMSERVER_RACMSERVER_CONS_ENTRY );
	
	OstTraceFunctionExit0( RACMSERVER_RACMSERVER_CONS_EXIT );
	}
	   
EXPORT_C RAcmServer::~RAcmServer()
	{
	OstTraceFunctionEntry0( RACMSERVER_RACMSERVER_ENTRY_DES );
	if (iImpl)
		{
		OstTraceExt1( TRACE_FATAL, RACMSERVER_RACMSERVER_DESTRUCTURE, "RAcmServer::~RAcmServer;iImpl=%p", iImpl );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicNotClosed) );
		}
	OstTraceFunctionExit0( RACMSERVER_RACMSERVER_EXIT_DES );
	}

EXPORT_C TInt RAcmServer::Connect()
	{
	OstTraceFunctionEntry0( RACMSERVER_CONNECT_ENTRY );
	if (iImpl)
		{
		OstTraceExt1( TRACE_FATAL, RACMSERVER_CONNECT, "RAcmServer::Connect;iImpl=%p", iImpl );	
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicAlreadyConnected) );
		}
	TRAPD(err, iImpl = CAcmServerImpl::NewL());
	OstTraceFunctionExit0( RACMSERVER_CONNECT_EXIT );
	return err;
	}

EXPORT_C void RAcmServer::Close()
	{
	OstTraceFunctionEntry0( RACMSERVER_CLOSE_ENTRY );
	
	delete iImpl;
	iImpl = NULL;
	OstTraceFunctionExit0( RACMSERVER_CLOSE_EXIT );
	}

EXPORT_C TInt RAcmServer::CreateFunctions(const TUint aNoAcms)
	{
	OstTraceFunctionEntry0( RACMSERVER_CREATEFUNCTIONS_ENTRY );
	OstTrace1( TRACE_NORMAL, RACMSERVER_CREATEFUNCTIONS, "RAcmServer::CreateFunctions;aNoAcms=%d", aNoAcms );
	
	if (!iImpl)
		{
		OstTrace0( TRACE_FATAL, RACMSERVER_CREATEFUNCTIONS_DUP2, "RAcmServer::CreateFunctions;EPanicNotConnected" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicNotConnected) );
		}

	if (!aNoAcms)
		{
		OstTrace0( TRACE_FATAL, RACMSERVER_CREATEFUNCTIONS_DUP1, "RAcmServer::CreateFunctions;EPanicCantInstantiateZeroAcms" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicCantInstantiateZeroAcms) );
		}
	return iImpl->CreateFunctions(aNoAcms, KDefaultAcmProtocolNum, KControlIfcName, KDataIfcName);
	}
							
EXPORT_C TInt RAcmServer::CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum)
	{
	OstTraceFunctionEntry0( RACMSERVER_CREATEFUNCTIONS_TUINT_TUINT8_ENTRY );
	OstTraceExt2( TRACE_NORMAL, RACMSERVER_CREATEFUNCTIONS_TUINT_TUINT8, "RAcmServer::CreateFunctions;aNoAcms=%d;aProtocolNum=%d", aNoAcms, aProtocolNum );

	if (!iImpl)
		{
		OstTrace0( TRACE_FATAL, RACMSERVER_CREATEFUNCTIONS_TUINT_TUINT8_DUP1, "RAcmServer::CreateFunctions;EPanicNotConnected" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicNotConnected) );
		}

	if (!aNoAcms)
		{
		OstTrace0( TRACE_FATAL, RACMSERVER_CREATEFUNCTIONS_TUINT_TUINT8_DUP2, "RAcmServer::CreateFunctions;EPanicCantInstantiateZeroAcms" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicCantInstantiateZeroAcms) );
		}
	return iImpl->CreateFunctions(aNoAcms, aProtocolNum, KControlIfcName, KDataIfcName);
	}

EXPORT_C TInt RAcmServer::CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum, const TDesC& aAcmControlIfcName, const TDesC& aAcmDataIfcName)
	{
	OstTraceFunctionEntry0( RACMSERVER_CREATEFUNCTIONS_TUINT_TUINT8_TDESCREF_TDESCREF_ENTRY );
	
	if (!iImpl)
		{
		OstTrace0( TRACE_FATAL, RACMSERVER_CREATEFUNCTIONS_TUINT_TUINT8_TDESCREF_TDESCREF, "RAcmServer::CreateFunctions;EPanicNotConnected" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicNotConnected) );
		}

	if (!aNoAcms)
		{
		OstTrace0( TRACE_FATAL, RACMSERVER_CREATEFUNCTIONS_TUINT_TUINT8_TDESCREF_TDESCREF_DUP1, "RAcmServer::CreateFunctions;EPanicCantInstantiateZeroAcms" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicCantInstantiateZeroAcms) );
		}
	return iImpl->CreateFunctions(aNoAcms, aProtocolNum, aAcmControlIfcName, aAcmDataIfcName);
	}

EXPORT_C TInt RAcmServer::DestroyFunctions(const TUint aNoAcms)
	{
	OstTraceFunctionEntry0( RACMSERVER_DESTROYFUNCTIONS_ENTRY );
	OstTrace1( TRACE_NORMAL, RACMSERVER_DESTROYFUNCTIONS, "RAcmServer::DestroyFunctions;aNoAcms=%d", aNoAcms );

	if (!iImpl)
		{
		OstTrace0( TRACE_FATAL, RACMSERVER_DESTROYFUNCTIONS_DUP1, "RAcmServer::CreateFunctions;EPanicNotConnected" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicNotConnected) );
		}

	if (!aNoAcms)
		{
		OstTrace0( TRACE_FATAL, RACMSERVER_DESTROYFUNCTIONS_DUP2, "RAcmServer::CreateFunctions;EPanicCantDestroyZeroAcms" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmSrvPanicCat, EPanicCantInstantiateZeroAcms) );
		}

	return iImpl->DestroyFunctions(aNoAcms);
	}
