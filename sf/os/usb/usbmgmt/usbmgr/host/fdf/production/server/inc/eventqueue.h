/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <e32base.h>
#include "event.h"

class CFdf;

/**
CEventQueue wraps all the events which are to be passed to USBMAN.
These are of two kinds:
1/ Devmon events. These are non-device-specific, and are sent to USBMAN
through the NotifyDevmonEvent server API. We cannot drop any of these, so a
count of each possible type is kept. When more than one such error is waiting
to be given to USBMAN, they are given up in an arbitrary order.
2/ Device events. These are device-specific, and indicate attachment, driver
loading, or detachment. They are sent to USBMAN through the NotifyDeviceEvent
server API. We keep a count of unsuccessful device attachments. These events
are given up to USBMAN in the following order: unsuccessful attachments, then
as they come in the queue. (For any one device, the queued events come in the
order attachment, driver loading, detachment.)
*/
NONSHARABLE_CLASS(CEventQueue) : public CBase
	{
public:
	static CEventQueue* NewL(CFdf& aFdf);
	~CEventQueue();

public:
	// Called by CFdf when an attachment failure occurs.
	void AttachmentFailure(TInt aError);

	// Called by CFdf when a successful attachment, driver load or detachment
	// occurs.
	// NB aEvent is not const because we need to add it to a linked list
	void AddDeviceEvent(TDeviceEvent& aEvent);

	// Called by CFdf when a devmon event occurs.
	void AddDevmonEvent(TInt aEvent);

	// Called by CEventQueue (when an event is added, we poke the session to
	// see if it's waiting for any new events) and by CFdfSession when the
	// relevant notification (device or devmon) is placed.
	TBool GetDeviceEvent(TDeviceEvent& aEvent);
	TBool GetDevmonEvent(TInt& aEvent);

private:
	CEventQueue(CFdf& aFdf);

private:
#ifdef _DEBUG
	void Log();
#endif
	void PokeSession();

private: // unowned
	CFdf& iFdf;

private: // owned
	TSglQue<TDeviceEvent> iDeviceEvents;

	// These are used to index our array of the counts of different attachment
	// failures.
	enum
		{
		KSetAddrFailed					= 0,
		KNoPower						= 1,
		KBadPower						= 2,
		KIOError						= 3,
		KTimeout						= 4,
		KStalled						= 5,
		KNoMemory						= 6,
		KConfigurationHasNoInterfaces	= 7,
		KInterfaceCountMismatch			= 8,
		KDuplicateInterfaceNumbers		= 9,
		KBadHandle						= 10,
		KAttachmentFailureGeneralError  = 11,
		KNumberOfAttachmentFailureTypes	= 12,
		};
	TFixedArray<TUint, KNumberOfAttachmentFailureTypes> iAttachmentFailureCount;

	// These are used to index our array of the counts of different devmon
	// events.
	enum
		{
		KUsbDeviceRejected			= 0,
		KUsbDeviceFailed			= 1,
		KUsbBadDevice				= 2,
		KUsbBadHubPosition			= 3,
		KUsbBadHub					= 4,
		KUsbEventOverflow			= 5,
		KNumberOfDevmonEventTypes	= 6
		};
	TFixedArray<TUint, KNumberOfDevmonEventTypes> iDevmonEventCount;
	};

#endif // EVENTQUEUE_H
