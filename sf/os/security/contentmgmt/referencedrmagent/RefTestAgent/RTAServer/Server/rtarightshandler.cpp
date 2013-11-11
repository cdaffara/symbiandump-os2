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

#include "rtarightshandler.h"
#include "RTAsession.h"
#include "RTAserver.h"
#include "rightsobject.h"
#include "rtaArchive.h"
#include "drmfilecontent.h"
#include "drmfiles.h"

using namespace ReferenceTestAgent;



CRtaRightsHandler* CRtaRightsHandler::NewL(const CSession2& aSession)
	{
	return new (ELeave) CRtaRightsHandler(aSession);
	}
	
CRtaRightsHandler::CRtaRightsHandler(const CSession2& aSession) : CRtaMessageHandler(aSession)
	{
	}
	
CRtaRightsHandler::~CRtaRightsHandler()
	{
	}

void CRtaRightsHandler::ServiceL(const RMessage2& aMessage)
//
// Handle a client request.
	{
	switch (aMessage.Function())
		{	
	// RRtaRights functions
	case EAddRights:
	case ENoEnforceAddRights:
		AddRightsL(aMessage);
		break;
	case EClearAllRights:
	case ENoEnforceClearAllRights:
		ClearAllRightsL(aMessage);
		break;
	case EDumpRightsDatabase:
	case ENoEnforceDumpRightsDatabase:
		DumpRightsDatabaseL(aMessage);
		break;
	case EDeleteRightsByKey:
	case ENoEnforceDeleteRightsByKey:
		DeleteRightsByKeyL(aMessage);
		break;
	case EEraseDeleteKey:
	case ENoEnforceEraseDeleteKey:
		EraseDeleteKeyL(aMessage);
		break;
	case EReloadRightsDatabase:
	case ENoEnforceReloadRightsDatabase:
		ReloadRightsDatabaseL(aMessage);
		break;
	default:
		CRtaSession::PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	}

void CRtaRightsHandler::AddRightsL(const RMessage2& aMessage)
	{
	HBufC8* buffer = ReadDes8LC(aMessage,0);
	RDesReadStream stream(*buffer);
	
	CRightsObject* rights = CRightsObject::NewL(stream, ETrue);
	CleanupStack::PushL(rights);
	Server().AddRightsL(rights);
	CleanupStack::Pop(rights);  // now owned by server
	
	CleanupStack::PopAndDestroy(buffer);
	}

void CRtaRightsHandler::ClearAllRightsL(const RMessage2&)
	{
	Server().ClearAllRightsL();
	}

void CRtaRightsHandler::DumpRightsDatabaseL(const RMessage2& aMessage)
	{
	HBufC* fileName = ReadDesC16LC(aMessage,0);
	Server().DumpRightsDatabaseL(*fileName);
	CleanupStack::PopAndDestroy(fileName);
	}

void CRtaRightsHandler::DeleteRightsByKeyL(const RMessage2& aMessage)
	{
	HBufC8* key = ReadDesC8LC(aMessage,0);
	Server().DeleteRightsByKeyL(*key);
	CleanupStack::PopAndDestroy(key);
	}

void CRtaRightsHandler::EraseDeleteKeyL(const RMessage2& aMessage)
	{
	HBufC8* key = ReadDesC8LC(aMessage,0);
	Server().EraseDeleteKeyL(*key);
	CleanupStack::PopAndDestroy(key);
	}

void CRtaRightsHandler::ReloadRightsDatabaseL(const RMessage2& aMessage)
	{
	HBufC* fileName = ReadDesC16LC(aMessage,0);
	Server().ReloadRightsDatabaseL(*fileName);
	CleanupStack::PopAndDestroy(fileName);
	}
