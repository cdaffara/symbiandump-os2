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

#ifndef MEMSPYDRIVERLOGICALCHANSTACK_H
#define MEMSPYDRIVERLOGICALCHANSTACK_H

// System includes
#include <e32cmn.h>
#include <kernel.h>

// Shared includes
#include <memspy/driver/memspydriverenumerationsshared.h>

// User includes
#include "MemSpyDriverLogChanBase.h"

// Classes referenced
class DMemSpyDriverDevice;
class TMemSpyDriverStackInfo;
class TMemSpyDriverInternalStackDataParams;



NONSHARABLE_CLASS(DMemSpyDriverLogChanStack) : public DMemSpyDriverLogChanBase
	{
public:
	DMemSpyDriverLogChanStack( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanStack();

public: // From DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt GetStackData( TMemSpyDriverInternalStackDataParams* aParams );
    TInt GetStackInfo( TUint aTid, TMemSpyDriverStackInfo* aParams );

private: // Internal methods
    void GetStackPointers( DThread* aThread, TUint& aSupSP, TUint& aUsrSP );
    TInt GetStackHighWatermark( DThread& aThread, TLinAddr& aHighWaterMark, TMemSpyDriverDomainType aDomain, TUint aRune );
    TInt ReadStackData( DThread& aThread, TDes8& aDestination, TUint8*& aReadAddress, TMemSpyDriverDomainType aDomain );
    TLinAddr GetStackPointerByDomain( DThread* aThread, TMemSpyDriverDomainType aDomainType );

private: // Data members
	};


#endif
