/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef OTGSTATEWATCHER_H
#define OTGSTATEWATCHER_H

#include <e32def.h>
#include <e32property.h> //Publish & Subscribe header
#include <e32cmn.h>
#include <e32base.h>
#include <usbotgdefs.h>
#include "motgobserver.h"

NONSHARABLE_CLASS(COtgStateWatcher) : public CActive
	{
public:
	static COtgStateWatcher* NewL(MOtgPropertiesObserver* aObserver);
	~COtgStateWatcher();

	TUsbOtgState OtgState() const;

private:
	COtgStateWatcher(MOtgPropertiesObserver* aObserver);
	void ConstructL();
	
private:
	void RunL();
	void DoCancel();

private:
	MOtgPropertiesObserver* iObserver;
	RProperty				iOtgStateProp;
	TUsbOtgState			iOtgState;
	};

#endif //OTGSTATEWATCHER_H
