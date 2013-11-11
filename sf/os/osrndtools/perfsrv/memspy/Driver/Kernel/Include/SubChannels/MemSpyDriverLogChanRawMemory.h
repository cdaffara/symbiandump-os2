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

#ifndef MEMSPYDRIVERLOGICALCHANRAWMEMORY_H
#define MEMSPYDRIVERLOGICALCHANRAWMEMORY_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverLogChanBase.h"

// Classes referenced
class DMemSpyDriverDevice;
class TMemSpyDriverInternalReadMemParams;


NONSHARABLE_CLASS(DMemSpyDriverLogChanRawMemory) : public DMemSpyDriverLogChanBase
	{
public:
	DMemSpyDriverLogChanRawMemory( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanRawMemory();

public: // From DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
	TInt ReadMem( TMemSpyDriverInternalReadMemParams* aParams );

private: // Data members
	};


#endif
