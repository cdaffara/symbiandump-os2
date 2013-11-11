/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYDRIVERLOGICALCHANCONTAINERBASE_H
#define MEMSPYDRIVERLOGICALCHANCONTAINERBASE_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverLogChanBase.h"
#include <memspy/driver/memspydriverenumerationsshared.h>

// Classes referenced
class DMemSpyDriverDevice;

// Constants
const TInt KMemSpyDriverMaxHandles = 1024 * 2;


NONSHARABLE_CLASS(DMemSpyDriverLogChanContainerBase) : public DMemSpyDriverLogChanBase
	{
public:
	DMemSpyDriverLogChanContainerBase( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanContainerBase();

protected: // Internal methods
    static TObjectType ObjectTypeFromMemSpyContainerType( TMemSpyDriverContainerType aType );

	// Must be in critical section to call
	DObject* CheckedOpen(TMemSpyDriverContainerType aContainerType, DObject* aObject, TBool aQuick=EFalse);

protected: // Internal methods
    void ResetTempHandles();
    void AddTempHandle( TAny* aHandle);
    TAny* TempHandleAt( TInt aIndex ) const;
    TInt TempHandleCount() const;
    TInt WriteToClient( TAny** aHandlePtr, TInt* aCountPtr, TInt aMaxCount );

private: // Data members
	TInt iTempHandleCount;
	TAny* iTempHandles[ KMemSpyDriverMaxHandles ];
	};


#endif
