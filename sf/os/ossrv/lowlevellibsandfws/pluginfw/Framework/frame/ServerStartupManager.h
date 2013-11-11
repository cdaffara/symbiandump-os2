
/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The definitions of CServerStartupMgr.
* This file contains all the APIs to implement CServerStartrupMgr.  
* 
*
*/



/**
 @file
 @internalComponent
*/
#ifndef __SERVERSTARTUPMANAGER_H__
#define __SERVERSTARTUPMANAGER_H__

#include <f32file.h>
#include <e32base.h>
#include <startup.hrh>
#include <domaindefs.h>

#ifdef __ECOM_SERVER_TESTABILITY__
	// use the TestHarnessDomainMember.h file for the testing purpose.
	#include "TestHarnessDomainMember.h"
#else
	// use the real domainmember.h for the production code.
	#include <domainmember.h>
#endif
#include "StartupStateObserver.h"
	
/**
This class interacts with the Domain Manager to be kept aware of the current
system startup state. It also contains a list of MStartupStateObserver objects.
It will distribute the startup state changes to the 
MStartupStateObserver objects in ECOM. 

@internalComponent 
*/
#ifdef __ECOM_SERVER_TESTABILITY__
class CServerStartupMgr: public CDmDomainTestHarness
#else
class CServerStartupMgr: public CDmDomain
#endif
	{
public:
	CServerStartupMgr(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId, RFs& aFs);
	virtual ~CServerStartupMgr();
	void InitialiseL(TBool aSsaEnabled);
	void RegisterObserverL(const MStartupStateObserver* aObs);
	TStartupStateIdentifier CurrentStartupState();
#ifdef __ECOM_SERVER_TESTABILITY__
public:
#else
protected:
#endif 
	//inherited from CActive
	void RunL();
protected:
	TInt RunError(TInt aError);
private:

	void UpdateStateAwareObjectsL(TStartupStateIdentifier aKnownState);
	TStartupStateIdentifier GetKnownStartupState(TDmDomainState aStartupState);
	
private:
    /**Final startup state  */
	TStartupStateIdentifier EKFinalStartupState;
	
	/** A list of MStartup StateObserver objects */
	RPointerArray<MStartupStateObserver> iObserverList;
	
	/** Current startup state*/
	TStartupStateIdentifier iCurrentStartupState;
	
	/** A reference to a connected file session */
	RFs& iFs;
	
	/** Required for test code to have access to internal state */
	friend class TServerStartupManager_StateAccessor;
	};
#endif // __SERVERSTARTUPMANAGER_H__


