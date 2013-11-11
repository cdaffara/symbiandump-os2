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

#ifndef __STARTUP_STATE_OBSERVER_OBJECT_H__
#define __STARTUP_STATE_OBSERVER_OBJECT_H__


#include "StartupStateObserver.h"

/**
@internalTechnology

A dummy startup state observer enabled class useful for unit testing.
*/
class CStartupStateObserver: public CBase, public MStartupStateObserver
	{
public:
	/**
	* Factory method to create an instance of the class
	*/
	static CStartupStateObserver* NewL();
	
	/**
	* Factory method to create an instance of the class
	* The object is left on the CleanupStack;
	*/
	static CStartupStateObserver* NewLC();
	
	/*
	* Destructor
	*/
	virtual ~CStartupStateObserver();

	/** 
	Internal state for CStartupStateObserver 
	@internalComponent
	*/
	enum TSsoState
		{
		SsoUndefined,
		SsoNoPluginsInternalized,
		SsoCriticalPlugingsInternalized,
		SsoAllPluginsInternalized
		};
	
	/**
	* the method used to process the SSA(Startup State Aware) event for the 
	* MStartupStateObserver object.
	* 
	* @param aState the TDmDomainState passes into the MStartupStateObserver object.
	*/
	virtual void ProcessSSAEventL(TStartupStateIdentifier aKnownState);
	
	/**
	* Initialises the internal state of the object
	*/
	void InitialiseEvent();
	
	/**
	* Returns the current state of the object
	* @return the current state
	*/
	CStartupStateObserver::TSsoState GetState();
	
	/**
	* Sets the current state of the object
	* @param aState the new state of the object
	*/
	void SetState(CStartupStateObserver::TSsoState aState);
	
private:
	/**
	* Default constructor. Make it private so the objects can only be
	* instantiated with NewL
	*/
	CStartupStateObserver();
	
	/**
	* Initialises the object
	*/
	void Construct();
	
private:
	TSsoState iState;
	};


#endif //__STARTUP_STATE_OBSERVER_OBJECT_H__
