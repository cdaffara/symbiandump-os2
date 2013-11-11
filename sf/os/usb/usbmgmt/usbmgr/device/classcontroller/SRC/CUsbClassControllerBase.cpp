/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements part of UsbMan USB Class Controller Framework.
* All USB classes to be managed by UsbMan must derive
* from this class.
*
*/

/**
 @file
*/

#include <cusbclasscontrollerbase.h>
#include <musbclasscontrollernotify.h>


/**
 * Constructor.
 *
 * @param aOwner Owner and manager of this object
 * @param aStartupPriority The priority of the Class Controller (the 
 * priorities of all present Class Controllers determines the order in which 
 * they are started).
 */
EXPORT_C CUsbClassControllerBase::CUsbClassControllerBase(
	MUsbClassControllerNotify& aOwner, TInt aStartupPriority)
	: CActive(EPriorityStandard), iStartupPriority(aStartupPriority),
	  iState(EUsbServiceIdle), iOwner(aOwner)
	{
	CActiveScheduler::Add(this);
	}

/**
 * Destructor.
 */
EXPORT_C CUsbClassControllerBase::~CUsbClassControllerBase()
	{
	}

/**
 * Fetch the owner of the USB class controller.
 *
 * @return	The owner of the USB class controller
 */
EXPORT_C MUsbClassControllerNotify& CUsbClassControllerBase::Owner() const
	{
	return iOwner;
	}

/**
 * Get the state of this USB class controller.
 *
 * @return	The state of this USB class controller
 */
EXPORT_C TUsbServiceState CUsbClassControllerBase::State() const
	{
	return iState;
	}

/**
 * Get the startup priority of this USB class controller.
 *
 * @return	The startup priority of this USB class controller
 */
EXPORT_C TInt CUsbClassControllerBase::StartupPriority() const
	{
	return iStartupPriority;
	}

EXPORT_C TInt CUsbClassControllerBase::Compare(const CUsbClassControllerBase& aFirst,
	const CUsbClassControllerBase& aSecond)
	{
	if (aFirst.StartupPriority() < aSecond.StartupPriority()) {
		return -1;
	}
	else if (aFirst.StartupPriority() > aSecond.StartupPriority()) {
		return 1;
	}
	return 0;
	}

