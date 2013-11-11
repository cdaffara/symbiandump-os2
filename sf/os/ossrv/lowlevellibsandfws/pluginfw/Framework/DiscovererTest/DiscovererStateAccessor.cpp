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
// Implementation of the State Accessor class for CDiscoverer
// 
//

#include "DiscovererStateAccessor.h"

// ______________________________________________________________________________
//
void TDiscoverer_StateAccessor::Notification(TAny* aTestObject)
	{
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	__ASSERT_DEBUG(discoverer!=NULL, User::Invariant());
	discoverer->Notification(KErrNone);
	}

void TDiscoverer_StateAccessor::ScanDirectoryL(TAny* aTestObject)
	{
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	__ASSERT_DEBUG(discoverer!=NULL, User::Invariant());
	discoverer->ScanDirectoryL();
	}

void TDiscoverer_StateAccessor::ScanDirectoryCancel(TAny* aTestObject)
	{
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	__ASSERT_DEBUG(discoverer!=NULL, User::Invariant());
	discoverer->ScanDirectoryCancel();
	}

void TDiscoverer_StateAccessor::CompleteNotificationProcessing(TAny* aTestObject)
	{
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	__ASSERT_DEBUG(discoverer!=NULL, User::Invariant());
	discoverer->CompleteNotificationProcessing();
	}

void TDiscoverer_StateAccessor::ValidateEntryL(TAny* aTestObject, const TEntry& anEntry, const TParse& aPath, TEntry*& anEntryToFill)
	{
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	__ASSERT_DEBUG(discoverer!=NULL, User::Invariant());
	discoverer->ValidateEntryL(anEntry, aPath, anEntryToFill);
	}

void TDiscoverer_StateAccessor::ProcessEntryL(TAny* aTestObject, const TEntry& anEntry)
	{
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	__ASSERT_DEBUG(discoverer!=NULL, User::Invariant());
	discoverer->ProcessEntryL(anEntry);
	}

void TDiscoverer_StateAccessor::DriveMountedL(TAny* aTestObject, TDriveUnit aDrive)
	{
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	__ASSERT_DEBUG(discoverer!=NULL, User::Invariant());
	discoverer->DriveMountedL(aDrive);
	}

void TDiscoverer_StateAccessor::DriveUnmountedL(TAny* aTestObject, TDriveUnit aDrive)
	{
	CDiscoverer* discoverer = REINTERPRET_CAST(CDiscoverer*, aTestObject);
	__ASSERT_DEBUG(discoverer!=NULL, User::Invariant());
	discoverer->DriveUnmountedL(aDrive);
	}
