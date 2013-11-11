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
#include <acminterface.h>
#include "acmserverimpl.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "acmserverimplTraces.h"
#endif

/** Constructor */
CAcmServerImpl::CAcmServerImpl() 
	{
	OstTraceFunctionEntry0( CACMSERVERIMPL_CACMSERVERIMPL_CONS_ENTRY );
	
	OstTraceFunctionExit0( CACMSERVERIMPL_CACMSERVERIMPL_CONS_EXIT );
	}
	   
/** Destructor */
CAcmServerImpl::~CAcmServerImpl()
	{
	OstTraceFunctionEntry0( CACMSERVERIMPL_CACMSERVERIMPL_DES_ENTRY);
	
	iCommServ.Close();
	iAcmServerClient.Close();
	OstTraceFunctionExit0( CACMSERVERIMPL_CACMSERVERIMPL_ENTRY_DES_EXIT );
	}

/**
2-phase construction.
@return Ownership of a new CAcmServerImpl.
*/
CAcmServerImpl* CAcmServerImpl::NewL()
	{
	OstTraceFunctionEntry0( CACMSERVERIMPL_NEWL_ENTRY );
	
	CAcmServerImpl* self = new(ELeave) CAcmServerImpl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CACMSERVERIMPL_NEWL_EXIT );
	return self;
	}

void CAcmServerImpl::ConstructL()
	{
	OstTraceFunctionEntry0( CACMSERVERIMPL_CONSTRUCTL_ENTRY );
	

	// In order to connect a session, the ECACM CSY must be loaded (it 
	// contains the server).
	TInt	err;
	err = iCommServ.Connect();
	if (err < 0)
		{
		OstTrace1( TRACE_ERROR, CACMSERVERIMPL_CONSTRUCTL, "CAcmServerImpl::ConstructL;err=%d", err );
		User::Leave(err);
		}
	
	err = iCommServ.LoadCommModule(KAcmCsyName);
	if (err < 0)
		{
		OstTrace1( TRACE_ERROR, CACMSERVERIMPL_CONSTRUCTL_DUP1, "CAcmServerImpl::ConstructL;err=%d", err );
		User::Leave(err);
		}

	// NB RCommServ::Close undoes LoadCommModule.
	err = iAcmServerClient.Connect();
	if (err < 0)
		{
		OstTrace1( TRACE_ERROR, CACMSERVERIMPL_CONSTRUCTL_DUP2, "CAcmServerImpl::ConstructL;err=%d", err );
		User::Leave(err);
		}

	// iCommServ is eventually cleaned up in our destructor. It must be held 
	// open at least as long as our session on the ACM server, otherwise 
	// there's a risk the ACM server will be pulled from under our feet.
	OstTraceFunctionExit0( CACMSERVERIMPL_CONSTRUCTL_EXIT );
	}

TInt CAcmServerImpl::CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum, const TDesC& aAcmControlIfcName, const TDesC& aAcmDataIfcName)
	{
	OstTraceFunctionEntry0( CACMSERVERIMPL_CREATEFUNCTIONS_ENTRY );
	return iAcmServerClient.CreateFunctions(aNoAcms, aProtocolNum, aAcmControlIfcName, aAcmDataIfcName);
	}

TInt CAcmServerImpl::DestroyFunctions(const TUint aNoAcms)
	{
	OstTraceFunctionEntry0( CACMSERVERIMPL_DESTROYFUNCTIONS_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMSERVERIMPL_DESTROYFUNCTIONS, "CAcmServerImpl::DestroyFunctions;aNoAcms=%d", aNoAcms );
	return iAcmServerClient.DestroyFunctions(aNoAcms);
	}
