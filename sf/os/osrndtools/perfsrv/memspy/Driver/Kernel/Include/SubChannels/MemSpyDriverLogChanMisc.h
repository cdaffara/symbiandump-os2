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

#ifndef MEMSPYDRIVERLOGICALCHANMISC_H
#define MEMSPYDRIVERLOGICALCHANMISC_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverLogChanBase.h"

// Classes referenced
class DMemSpyDriverDevice;


NONSHARABLE_CLASS(DMemSpyDriverLogChanMisc) : public DMemSpyDriverLogChanBase
	{
public:
	DMemSpyDriverLogChanMisc( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanMisc();

public: // From DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt SetRHeapVTable( TAny* aRHeapVTable );
    TInt GetMemoryModelType();
    TInt GetRoundToPageSize( TUint32* aValue );
    TInt Impersonate( TUint32 aValue );

private: // Data members
    TUint32 iSID;
    TUint32 iSecurityZone;
    TBool iClientIsMemSpy;
	};


#endif
