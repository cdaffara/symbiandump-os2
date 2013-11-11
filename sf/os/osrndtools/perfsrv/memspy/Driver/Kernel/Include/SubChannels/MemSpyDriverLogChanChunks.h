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

#ifndef MEMSPYDRIVERLOGICALCHANCHUNKS_H
#define MEMSPYDRIVERLOGICALCHANCHUNKS_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>
#include <memspy/driver/memspydriverobjectsshared.h>
#include <memspy/driver/memspydriverenumerationsshared.h>

// User includes
#include "MemSpyDriverLogChanBase.h"

// Classes referenced
class DMemSpyDriverDevice;
class TMemSpyDriverInternalChunkInfoParams;
class TMemSpyDriverInternalChunkHandleParams;


NONSHARABLE_CLASS(DMemSpyDriverLogChanChunks) : public DMemSpyDriverLogChanBase
	{
public:
	DMemSpyDriverLogChanChunks( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanChunks();

public: // From DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt GetChunkInfo( TMemSpyDriverInternalChunkInfoParams* aParams );
    TInt GetChunkHandles( TMemSpyDriverInternalChunkHandleParams* aParams );

private: // Internal methods
    void PrintChunkInfo( DChunk& aChunk );
    TMemSpyDriverChunkType IdentifyChunkType( DChunk& aChunk );
    TBool IsHeapChunk( DChunk& aChunk, const TName& aName );
    TBool DoesChunkRelateToProcess( DChunk& aChunk, DProcess& aProcess );

private: // Data members
	};


#endif
