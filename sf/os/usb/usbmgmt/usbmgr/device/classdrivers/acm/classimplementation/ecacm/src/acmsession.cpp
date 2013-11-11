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
* ACM session.
*
*/

/**
 @file
 @internalComponent
*/

#include "acmserverconsts.h"
#include "acmsession.h"
#include "AcmPortFactory.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "acmsessionTraces.h"
#endif


CAcmSession* CAcmSession::NewL(MAcmController& aAcmController)
	{
	OstTraceFunctionEntry0( CACMSESSION_NEWL_ENTRY );
	CAcmSession* self = new(ELeave) CAcmSession(aAcmController);
	OstTraceFunctionExit0( CACMSESSION_NEWL_EXIT );
	return self;
	}

CAcmSession::CAcmSession(MAcmController& aAcmController)
 :	iAcmController(aAcmController)
	{
	OstTraceFunctionEntry0( CACMSESSION_CACMSESSION_CONS_ENTRY );
	OstTraceFunctionExit0( CACMSESSION_CACMSESSION_CONS_EXIT );
	}

CAcmSession::~CAcmSession()
	{
	OstTraceFunctionEntry0( CACMSESSION_CACMSESSION_DES_ENTRY );	
	OstTraceFunctionExit0( CACMSESSION_CACMSESSION_DES_EXIT );
	}
	
void CAcmSession::CreateFunctionsL(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CACMSESSION_CREATEFUNCTIONSL_ENTRY );
	RBuf acmControlIfcName, acmDataIfcName;
	TInt size = aMessage.GetDesLengthL(2);
	acmControlIfcName.CreateL(size);
	acmControlIfcName.CleanupClosePushL();
	aMessage.ReadL(2, acmControlIfcName);

	size = aMessage.GetDesLengthL(3);
	acmDataIfcName.CreateL(size);
	acmDataIfcName.CleanupClosePushL();
	aMessage.ReadL(3, acmDataIfcName);

	OstTraceExt4( TRACE_DUMP, CACMSESSION_CREATEFUNCTIONSL, 
			"CAcmSession::CreateFunctionsL;\taNoAcms = %d, aProtocolNum = %d, "
			"Control Ifc Name = %S, Data Ifc Name = %S", 
			aMessage.Int0(), aMessage.Int1(), acmControlIfcName, acmDataIfcName );
	

	TInt err = iAcmController.CreateFunctions(aMessage.Int0(), aMessage.Int1(), acmControlIfcName, acmDataIfcName);
	if (err < 0)
		{
		OstTrace1( TRACE_NORMAL, CACMSESSION_CREATEFUNCTIONSL_DUP1, "CAcmSession::CreateFunctionsL;err=%d", err );
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(2);
	OstTraceFunctionExit0( CACMSESSION_CREATEFUNCTIONSL_EXIT );
	}

void CAcmSession::ServiceL(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CACMSESSION_SERVICEL_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMSESSION_SERVICEL, "CAcmSession::ServiceL;aMessage.Function()=%d", aMessage.Function() );
	switch ( aMessage.Function() )
		{
	case EAcmCreateAcmFunctions:
		{
		TRAPD (err, CreateFunctionsL(aMessage));
		aMessage.Complete(err);
		break;
		}

	case EAcmDestroyAcmFunctions:
		{
		iAcmController.DestroyFunctions(aMessage.Int0());
		aMessage.Complete(KErrNone);
		break;
		}
		
	default:
		// Unknown function, panic the user
		aMessage.Panic(KAcmSrvPanic, EAcmBadAcmMessage);
		break;
		}
	OstTraceFunctionExit0( CACMSESSION_SERVICEL_EXIT );
	}
