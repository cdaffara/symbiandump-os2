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
// The stub classes for testing the CRegistrar class 
// 
//

#include "Discoverer.h"
#include "BackupNotifier.h"
#include "RegistryData.h"

//******************************************
// CDiscoverer Stubs
//******************************************
CDiscoverer* CDiscoverer::NewL(MDiscovererObserver& aDiscovererObserver, RFs& aFs)
	{
	return new(ELeave) CDiscoverer(aDiscovererObserver, aFs);
	}

CDiscoverer::CDiscoverer(MDiscovererObserver& aDiscovererObserver, RFs& aFs) :
CBase(),
iDiscovererObserver(aDiscovererObserver),
iFs(aFs)
	{
	}

CDiscoverer::~CDiscoverer()
	{
	}

TInt CDiscoverer::Suspend()
	{
	return KErrNone;
	}

TInt CDiscoverer::Resume()
	{
	return KErrNone;
	}

//******************************************
// CBackupNotifier Stubs
//******************************************
CBackupNotifier* CBackupNotifier::NewL(MBackupNotifierObserver& aObserver)
	{
	return new(ELeave) CBackupNotifier(aObserver);
	}

CBackupNotifier::CBackupNotifier(MBackupNotifierObserver& aObserver) :
CBase(),
iObserver(aObserver)
	{
	}

CBackupNotifier::~CBackupNotifier()
	{
	}

void CBackupNotifier::HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	switch(aBackupOperationAttributes.iOperation)
		{
		case EStart:
			iObserver.Suspend();
			break;
		case EEnd:
			iObserver.Resume();
			break;
		default:
			break;
		}
	}
