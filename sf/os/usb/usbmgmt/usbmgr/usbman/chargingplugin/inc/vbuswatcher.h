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

#ifndef VBUSWATCHER_H
#define VBUSWATCHER_H

#include <e32def.h>
#include <e32property.h> //Publish & Subscribe header
#include <e32cmn.h>
#include <e32base.h>
#include "motgobserver.h"

NONSHARABLE_CLASS(CVBusWatcher) : public CActive
	{
public:
	static CVBusWatcher* NewL(MOtgPropertiesObserver* aObserver);
	~CVBusWatcher();

	TInt VBusState() const;

private:
	CVBusWatcher(MOtgPropertiesObserver* aObserver);
	void ConstructL();
	
private:
	void RunL();
	void DoCancel();

private:
	MOtgPropertiesObserver*  iObserver;
	RProperty				iVBusProp;
	TInt					iVBusState;
	};

#endif //VBUSWATCHER_H
