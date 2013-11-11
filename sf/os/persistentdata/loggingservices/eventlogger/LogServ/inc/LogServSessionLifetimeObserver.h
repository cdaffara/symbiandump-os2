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
//

#ifndef __LOGSERVSESSIONLIFETIMEOBSERVER_H__
#define __LOGSERVSESSIONLIFETIMEOBSERVER_H__

#include "LogServDefs.h"

/////////////////////////////////////////////////////////////////////////////////////////
// -----> MLogServSessionLifetimeObserver (header)
/////////////////////////////////////////////////////////////////////////////////////////
/**
@internalComponent
*/
class MLogServSessionLifetimeObserver
	{
public:
	enum TLogServSessionEvent
		{
		ELogServSessionEventCreated = 0,
		ELogServSessionEventDestroyed
		};

public:
	virtual void SLOHandleEvent(TLogServSessionId aId, TLogServSessionEvent aEvent) = 0;
	};


#endif

