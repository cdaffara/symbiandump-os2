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

#ifndef __SCHSSAMAN_H__
#define __SCHSSAMAN_H__

#include <e32base.h>
#include <f32file.h>
#include <startup.hrh>
#include <domaindefs.h>
#include <startupdomaindefs.h>
#include <domainmember.h>

#include "SchSSAObserver.h"
	
/**
This class interacts with the Domain Manager to be kept aware of the current
system startup state. It also contains a list of MSchStartupStateObserver objects.
It will distribute the startup state changes to registered observers.

@internalComponent
*/
NONSHARABLE_CLASS(CSchStartupStateMgr) : public CDmDomain
	{
public:
	CSchStartupStateMgr(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId);
	virtual ~CSchStartupStateMgr();
	void InitialiseL();
	void RegisterObserverL(const MSchStartupStateObserver* aObs);
	TStartupStateIdentifier CurrentStartupState();

protected:
	//inherited from CActive
	void RunL();
	TInt RunError(TInt aError);

private:
	void UpdateStateAwareObjectsL(TStartupStateIdentifier aKnownState);
	TStartupStateIdentifier GetKnownStartupState(TDmDomainState aStartupState);
	
private:
	/** A list of MStartup StateObserver objects */
	RPointerArray<MSchStartupStateObserver> iObserverList;
	
	/** Current startup state*/
	TStartupStateIdentifier iCurrentStartupState;
	
	};
#endif // __SCHSSAMAN_H__
