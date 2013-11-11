// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <startup.hrh>
#include "StartupStateObserverObject.h"

CStartupStateObserver::CStartupStateObserver()
	:CBase(), iState(SsoUndefined)
	{
	}

CStartupStateObserver::~CStartupStateObserver()
	{
	}

CStartupStateObserver* CStartupStateObserver::NewL()
	{
	CStartupStateObserver* self = CStartupStateObserver::NewLC();
	CleanupStack::Pop(self);
	
	return self;
	}
	
CStartupStateObserver* CStartupStateObserver::NewLC()
	{
	CStartupStateObserver* self = new(ELeave) CStartupStateObserver;
	CleanupStack::PushL(self);
	self->Construct();
	
	return self;
	}


void CStartupStateObserver::Construct()
	{
	InitialiseEvent();
	}

	
void CStartupStateObserver::InitialiseEvent()
	{
	iState = SsoNoPluginsInternalized;
	}
	
void CStartupStateObserver::ProcessSSAEventL(TStartupStateIdentifier aKnownState)
	{
	if(iState == SsoNoPluginsInternalized)
		{
		if(aKnownState == EStartupStateCriticalStatic)
			{
			iState = SsoCriticalPlugingsInternalized;
			}
		else if(aKnownState == EStartupStateNonCritical)
			{
			iState = SsoAllPluginsInternalized;
			}
		}
	else if(iState == SsoCriticalPlugingsInternalized && 
	        aKnownState == EStartupStateNonCritical)
		{
		iState = SsoAllPluginsInternalized;
		}
	}
	
CStartupStateObserver::TSsoState CStartupStateObserver::GetState()
	{
	return iState;
	}

void CStartupStateObserver::SetState(CStartupStateObserver::TSsoState aState)
	{
	iState = aState;
	}
