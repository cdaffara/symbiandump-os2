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
* The definitions of MStartupStateObserver.
* This file contains all the APIs to implement MStartupStateObserver.  
* 
*
*/



/**
 @file
 @internalComponent
*/
#ifndef STARTUPSTATEOBSERVER_H
#define STARTUPSTATEOBSERVER_H
#include <startup.hrh>

/** 
The abstract interface used as a base for all classes that wish to be notified of system startup state changes in ECOM.

@internalComponent
*/
class MStartupStateObserver
	{
public:
	/**
	* the method used to process the SSA(Startup State Aware) event for the 
	* MStartupStateObserver object.
	* 
	* @param aKnownState the TDmDomainState passes into the MStartupStateObserver object.
	*/
	virtual void ProcessSSAEventL(TStartupStateIdentifier aKnownState)=0;	
	};
#endif

