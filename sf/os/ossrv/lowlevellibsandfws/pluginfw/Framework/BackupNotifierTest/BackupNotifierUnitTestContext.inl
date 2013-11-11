// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The context for the unit tests upon the CBackupNotifier class methods.
// Additionally supply a stub for the CBackupNotifier observation class that
// logs the resume and suspend calls
// 
//

#include "BackupNotifierTransitions.h"

// ______________________________________________________________________________
//
inline CNotificationStub::CNotificationStub(CDataLogger& aDataLogger)
: CBase(), iDataLogger(aDataLogger)
	{
	// Do nothing
	}

inline CNotificationStub::~CNotificationStub()
	{
	// Do nothing
	}

inline TInt CNotificationStub::Suspend()
	{
	_LIT(KBackupNotifierObserverSuspendMethod, "MBackupNotifierObserver::Suspend called");
	iDataLogger.LogInformation(KBackupNotifierObserverSuspendMethod);
	return KErrNone;
	}

inline TInt CNotificationStub::Resume()
	{
	_LIT(KBackupNotifierObserverResumeMethod, "MBackupNotifierObserver::Resume called");
	iDataLogger.LogInformation(KBackupNotifierObserverResumeMethod);
	return KErrNone;
	}

// ______________________________________________________________________________
//
inline CBackupNotifier_UnitTestContext::CBackupNotifier_UnitTestContext(CDataLogger& aDataLogger,
												MStateAccessor& aStateAccessor,
												MTransitionObserver& aObserver)
: CUnitTestContext(aDataLogger, aStateAccessor, aObserver)
	{
	//Do nothing
	}

inline CBackupNotifier_UnitTestContext::~CBackupNotifier_UnitTestContext()
	{
	delete iBackupNotifier;
	delete iStub;
	}

