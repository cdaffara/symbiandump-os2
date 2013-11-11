// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__BACKUP_STD_H__)
#define __BACKUP_STD_H__

#include <e32std.h>

/**
@internalComponent
*/
_LIT(__BACKUP_SERVER_NAME_V2,"!BackupServer");

/**
@internalComponent
*/
enum TBaBakOpCode
	{
	EBakOpCodeEventReady=20, // EBakOpCodeStartNotifications,
	EBakOpCodeGetEvent,
	EBakOpCodeCloseAllFiles,
	EBakOpCodeRestartAll,
	EBakOpCodeCloseFile,
	EBakOpCodeRestartFile,
	EBakOpCodeNotifyLockChange,
	EBakOpCodeNotifyLockChangeCancel,
	EBakOpCodeCloseServer,		// no longer supported
	EBakOpCodeNotifyBackupOperation,
	EBakOpCodeCancelOutstandingBackupOperationEvent,
	EBakOpCodeGetBackupOperationState,
	EBakOpCodeBackupOperationEventReady,
	EBakOpCodeGetBackupOperationEvent,
	EBakOpCodeSetBackupOperationObserverIsPresent,
	EBakOpCodeStopNotifications
	};

#endif
