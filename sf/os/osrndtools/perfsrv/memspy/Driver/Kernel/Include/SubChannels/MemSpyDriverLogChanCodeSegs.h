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

#ifndef MEMSPYDRIVERLOGICALCHANCODESEGS_H
#define MEMSPYDRIVERLOGICALCHANCODESEGS_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverLogChanBase.h"

// Classes referenced
class DMemSpyDriverDevice;
class TMemSpyDriverInternalCodeSegParams;
class TMemSpyDriverInternalCodeSnapshotParams;


NONSHARABLE_CLASS(DMemSpyDriverLogChanCodeSegs) : public DMemSpyDriverLogChanBase
	{
public:
	DMemSpyDriverLogChanCodeSegs( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanCodeSegs();

public: // From DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt GetCodeSegs( TMemSpyDriverInternalCodeSnapshotParams* aParams );
	TInt GetCodeSegsForProcess( TMemSpyDriverInternalCodeSnapshotParams* aParams );
	TInt GetCodeSegInfo( TMemSpyDriverInternalCodeSegParams* aParams );

private: // Data members
	};


#endif
