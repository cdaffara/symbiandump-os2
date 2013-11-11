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
* sisregistry - client registry session interface implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include <s32mem.h>
#include "clientserver.h"
#include "rtarights.h"
#include "rightsobject.h"


using namespace ReferenceTestAgent;

EXPORT_C RRtaRights::RRtaRights()
	{
	
	}

EXPORT_C TInt RRtaRights::Connect()
	{
	TInt err = KErrNone;
	err = RRtaClient::Connect();
	if(err == KErrNone)
		{
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(ECreateRightsMessageHandler, TIpcArgs());
			}
		else
			{
			err = SendReceive(ENoEnforceCreateRightsMessageHandler, TIpcArgs());
			}
		if(err != KErrNone)
			{
			Close();
			}
		}
	return err;
	}

EXPORT_C void RRtaRights::AddRightsL(CRightsObject& aRights)
	{
	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	aRights.ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	
	TPtr8 bufPtr = buf->Ptr(0);

	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EAddRights, TIpcArgs(&bufPtr)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceAddRights, TIpcArgs(&bufPtr)));
		}
	CleanupStack::PopAndDestroy(buf);
	}

EXPORT_C void RRtaRights::ClearAllRightsL()
	{
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EClearAllRights));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceClearAllRights));
		}
	}

EXPORT_C void RRtaRights::DumpRightsDatabaseL(const TDesC& aFileName)
	{
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EDumpRightsDatabase, TIpcArgs(&aFileName)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceDumpRightsDatabase, TIpcArgs(&aFileName)));
		}
	}

EXPORT_C void RRtaRights::DeleteRightsByKeyL(const TDesC8& aDeleteKey)
	{
	TRtaMessages funccode = IsDrmCapabilityEnforced() ? 
		EDeleteRightsByKey : ENoEnforceDeleteRightsByKey;
	User::LeaveIfError( SendReceive(funccode, TIpcArgs(&aDeleteKey)) );
	}

EXPORT_C void RRtaRights::EraseDeleteKeyL(const TDesC8& aDeleteKey)
	{
	TRtaMessages funccode = IsDrmCapabilityEnforced() ? 
		EEraseDeleteKey : ENoEnforceEraseDeleteKey;
	User::LeaveIfError( SendReceive(funccode, TIpcArgs(&aDeleteKey)) );
	}

EXPORT_C void RRtaRights::ReloadRightsDatabaseL(const TDesC& aFileName)
	{
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EReloadRightsDatabase, TIpcArgs(&aFileName)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceReloadRightsDatabase, TIpcArgs(&aFileName)));
		}
	}
