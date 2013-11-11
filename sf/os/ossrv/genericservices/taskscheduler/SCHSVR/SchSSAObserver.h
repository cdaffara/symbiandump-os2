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

#ifndef __SCHSSAOBSERVER_H__
#define __SCHSSAOBSERVER_H__

#include <startup.hrh>

static const TStartupStateIdentifier KSchFinalStartupState = EStartupStateNonCritical;

/**
The abstract interface used as a base for all classes that wish to
be notified of system startup state changes

@internalComponent
*/
class MSchStartupStateObserver
	{
public:
	// the method used to process the Startup State change for the 
	// MSchStartupStateObserver objects.
	virtual void ProcessSSAEventL(TStartupStateIdentifier aKnownState)=0;	
	};
#endif // __SCHSSAOBSERVER_H__
