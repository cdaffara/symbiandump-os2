/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SisRegistry - server session implementation
*
*/


/**
 @file
 @test
 @internalComponent
*/
#include <s32mem.h>
#include <f32file.h>
#include <s32file.h>

#include "RTAsession.h"
#include "RTAserver.h"
#include "rtarightshandler.h"
#include "rtadatahandler.h"
#include "rtamanagerhandler.h"
#include "rtacontenthandler.h"

using namespace ReferenceTestAgent;


_LIT(KRta, "RefTestAgent");
	
void CRtaSession::PanicClient(const RMessagePtr2& aMessage, TRtaServerPanic aPanic)
	{
	aMessage.Panic(KRta, aPanic);
	}

CRtaSession::CRtaSession()
	{
	}
	
void CRtaSession::CreateL()
	{
	Server().AddSession();
	}
	
CRtaSession::~CRtaSession()
	{
	delete iMessageHandler;	
	Server().DropSession();
	}

void CRtaSession::ServiceL(const RMessage2& aMessage)
//
// Handle a client request.
	{
 	TInt err = KErrNone;
	if(iMessageHandler)
		{
		TRAP(err, iMessageHandler->ServiceL(aMessage));
		}
	else
		{
		switch (aMessage.Function())
			{	
		// RRtaRights functions
		case ECreateRightsMessageHandler:
		case ENoEnforceCreateRightsMessageHandler:
			TRAP(err, iMessageHandler = CRtaRightsHandler::NewL(*this));
			break;
		// RRtaData functions
		case ECreateDataMessageHandler:
		case ENoEnforceCreateDataMessageHandler:
			TRAP(err, iMessageHandler = CRtaDataHandler::NewL(*this));
			break;
		// RRtaManager functions
		case ECreateManagerMessageHandler:
		case ENoEnforceCreateManagerMessageHandler:
			TRAP(err, iMessageHandler = CRtaManagerHandler::NewL(*this));
			break;
		case ECreateContentMessageHandler:
		case ENoEnforceCreateContentMessageHandler:
			TRAP(err, iMessageHandler = CRtaContentHandler::NewL(*this));
			break;
		default:
			PanicClient(aMessage,EPanicIllegalFunction);
			break;
			}
		}
	aMessage.Complete(err);
	}

void CRtaSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	CSession2::ServiceError(aMessage, aError);	
	}

