// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LOGSERVOPERATIONFACTORY.H
// 
//

#include "LogServOperationFactory.h"

// User includes
#include "logservpanic.h"
#include "LogServOperations.h"
#include "LogServView.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> LogServFactory (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServOperationBase* LogServFactory::NewOperationL(const TLogClientServerData& aCliServData, 
                                                     MLogServTaskInterface& aTaskInterface, 
                                                     MLogServOperationManager& aOperationManager, 
                                                     const RMessage2& aMessage, 
                                                     CLogPackage& aLogPackage, 
                                                     TLogServSessionId aSessionId)
	{
	// The operations are all owned by the operation queue/manager as soon
	// as they are contructed (since they are all members of a queue).
	switch(aCliServData.iOperationType)
		{
	case ELogOperationEventAdd:		// security checked in CLogAddEvent::StartL
		return new(ELeave) CLogServOpEventAdd(aTaskInterface, aOperationManager, aMessage, aLogPackage,
												   aCliServData, aSessionId);
	case ELogOperationEventGet:		// security checked in CLogGetEvent::DoRunL
		return new(ELeave) CLogServOpEventGet(aTaskInterface, aOperationManager, aMessage, aLogPackage,
												   aCliServData, aSessionId);
	case ELogOperationEventChange:	// security checked in CLogChangeEvent::DoChangeL
		return new(ELeave) CLogServOpEventChange(aTaskInterface, aOperationManager, aMessage, aLogPackage,
													  aCliServData, aSessionId);
	case ELogOperationEventDelete:	// security checked in CLogDeleteEvent::DoRunL
		return new(ELeave) CLogServOpEventDelete(aTaskInterface, aOperationManager, aMessage, aLogPackage,
													  aCliServData, aSessionId);
	case ELogOperationTypeAdd:
		if(aMessage.HasCapability(ECapabilityWriteDeviceData))
			{
			return new(ELeave) CLogServOpTypeAdd(aTaskInterface, aOperationManager, aMessage, aLogPackage,
													  aCliServData, aSessionId);
			}
		break;
	case ELogOperationTypeGet:		// no security check
		return new(ELeave) CLogServOpTypeGet(aTaskInterface, aOperationManager, aMessage, aLogPackage,
												  aCliServData, aSessionId);
	case ELogOperationTypeChange:
		if(aMessage.HasCapability(ECapabilityWriteDeviceData))
			{
			return new(ELeave) CLogServOpTypeChange(aTaskInterface, aOperationManager, aMessage, aLogPackage,
														 aCliServData, aSessionId);
			}
		break;
	case ELogOperationTypeDelete:
		if(aMessage.HasCapability(ECapabilityWriteDeviceData))
			{
			return new(ELeave) CLogServOpTypeDelete(aTaskInterface, aOperationManager, aMessage, aLogPackage,
														 aCliServData, aSessionId);
			}
		break;
	case ELogOperationClearLog:
		if(aMessage.HasCapability(ECapabilityWriteDeviceData))
			{
			return new(ELeave) CLogServOpClearLog(aTaskInterface, aOperationManager, aMessage, aLogPackage,
													   aCliServData, aSessionId);
			}
		break;
	case ELogOperationClearRecent:
		if(aMessage.HasCapability(ECapabilityWriteDeviceData))
			{
			return new(ELeave) CLogServOpClearRecent(aTaskInterface, aOperationManager, aMessage, aLogPackage,
														  aCliServData, aSessionId);
			}
		break;
	case ELogOperationConfigGet:	// no security check
		return new(ELeave) CLogServOpConfigGet(aTaskInterface, aOperationManager, aMessage, aLogPackage,
													aCliServData, aSessionId);
	case ELogOperationConfigChange:
		if(aMessage.HasCapability(ECapabilityWriteDeviceData))
			{
			return new(ELeave) CLogServOpConfigChange(aTaskInterface, aOperationManager, aMessage, aLogPackage,
														   aCliServData, aSessionId);
			}
		break;
	case ELogOperationMaintain:		// no security check
		return new(ELeave) CLogServOpMaintenance(aTaskInterface, aOperationManager, aMessage, aLogPackage,
													  aCliServData, aSessionId);
	default:
		::PanicClientL(aMessage, ELogServFactoryUnrecognizedOperationType);
		break;
		}

	// clients who don't have the required capability need to be informed.
	User::Leave(KErrPermissionDenied);
	return NULL;
	}

CLogServOperationBase* LogServFactory::NewViewOperationL(const TLogClientServerData& aCliServData, 
                                                         MLogServTaskInterface& aTaskInterface, 
                                                         MLogServOperationManager& aOperationManager, 
                                                         const RMessage2& aMessage, 
                                                         CLogPackage& aLogPackage, 
                                                         TLogServSessionId aSessionId, 
                                                         CLogServViewBase& aView)
	{
	// The operations are all owned by the operation queue/manager as soon
	// as they are contructed (since they are all members of a queue).
	switch(aCliServData.iOperationType)
		{
	case ELogOperationViewSetup:
		return new(ELeave) CLogServOpViewSetup(aTaskInterface, aOperationManager, aMessage, aLogPackage, aView,
											   aCliServData, aSessionId);
	case ELogOperationViewRemoveEvent:
		return new(ELeave) CLogServOpViewEventRemove(aTaskInterface, aOperationManager, aMessage, aLogPackage, aView,
													 aCliServData, aSessionId);
	case ELogOperationViewClearDuplicates:
		return new(ELeave) CLogServOpViewClearDuplicates(aTaskInterface, aOperationManager, aMessage, aLogPackage, aView,
														 aCliServData, aSessionId);
	case ELogOperationViewSetFlags:
		return new(ELeave) CLogServOpViewSetFlags(aTaskInterface, aOperationManager, aMessage, aLogPackage, aView,
												  aCliServData, aSessionId);
	case ELogOperationViewWindowFetch:
		return new(ELeave) CLogServOpViewWindowFetcher(aTaskInterface, aOperationManager, aMessage, aLogPackage, aView,
													   aCliServData, aSessionId);
	default:
		::PanicClientL(aMessage, ELogServFactoryUnrecognizedOperationType2);
		break;
		}
	return NULL;
	}

CLogServViewBase* LogServFactory::NewViewL(TLogViewType aType, 
                                           TLogViewId aId, 
                                           MLogServDatabaseTransactionInterface& aDatabase, 
                                           MLogServBackupInterface& aBackupInterface, 
                                           CLogPackage& aPackage, 
                                           const RMessage2& aMessage)
	{
	switch(aType)
		{
	case ELogViewTypeEvent:
		return CLogServViewEvent::NewL(aDatabase, aBackupInterface, aPackage, aId, aMessage);
	case ELogViewTypeRecent:
		return CLogServViewRecent::NewL(aDatabase, aBackupInterface, aPackage, aId, aMessage);
	case ELogViewTypeDuplicate:
		return CLogServViewDuplicate::NewL(aDatabase, aBackupInterface, aPackage, aId, aMessage);
	default:
		::PanicClientL(aMessage, ELogServFactoryUnrecognizedViewType);
		break;
		}
	return NULL;
	}
