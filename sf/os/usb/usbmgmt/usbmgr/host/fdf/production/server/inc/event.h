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

#ifndef EVENT_H
#define EVENT_H

#include <e32base.h>
#include "usbhoststack.h"

/**
TEvent wraps up information about a single device-related event.
*/
NONSHARABLE_CLASS(TDeviceEvent)
	{
public:
	TSglQueLink iLink;

	TDeviceEvent();
	~TDeviceEvent();

	void Log() const;

public:
	TDeviceEventInformation iInfo;
	};

#endif // EVENT_H
