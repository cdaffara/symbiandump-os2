/*
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
*
*/



#include <e32std.h>
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
#include <ssm/ssmstate.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmdomaindefs.h>
#include <test/sysstartplugin.h>

CSysStartPlugin::CSysStartPlugin()
#else
#include <startup.hrh>
#include <startupdomaindefs.h>
#include <test/sysstartplugin.h>

CSysStartPlugin::CSysStartPlugin() : CDmDomain(KDmHierarchyIdStartup, KAppServicesDomain3)
#endif
	{
	
	}

/**
 * Destructor
 * Frees the resources located in second-phase constructor
 */
CSysStartPlugin::~CSysStartPlugin()
	{
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	if (iStateAwareSession)
		{
		delete iStateAwareSession;
		}
#endif
	}

/**
 * Creates, and returns a pointer to CSysStartPlugin object, leave on failure
 * @param aName - Path and name of the ini file to be parsed
 * @return A pointer to the CSysStartPlugin object
 */
CSysStartPlugin* CSysStartPlugin::NewL()
	{
	CSysStartPlugin* self = new(ELeave) CSysStartPlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSysStartPlugin::ConstructL()
	{
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	iStateAwareSession = CSsmStateAwareSession::NewL(KSM2AppServicesDomain3);
	iStateAwareSession->AddSubscriberL(*this);
	}

void CSysStartPlugin::StateChanged(TSsmState aSsmState)
	{
	//if (ESsmNormal==aSsmState.MainState() && ESsmNormalSubState==aSsmState.SubState())
	if (aSsmState.Int() >= ESsmStartupSubStateNonCritical)
		{
		CActiveScheduler::Stop();
		}
#else
	CDmDomain::ConstructL();
#endif
	}

void CSysStartPlugin::WaitForSystemStartL()
	{
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	TSsmState state = iStateAwareSession->State();
	if (state.Int() < ESsmStartupSubStateNonCritical)
		{
		CActiveScheduler::Start();
		}
	}
#else
	TDmDomainState state = GetState();
	if (state < EStartupStateNonCritical)
		{
		RequestTransitionNotification();
		CActiveScheduler::Start();
		}
	}
	
void CSysStartPlugin::RunL()
	{
	TDmDomainState state = GetState();
	if (state >= EStartupStateNonCritical)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		RequestTransitionNotification();
		}
	}
	
#endif
