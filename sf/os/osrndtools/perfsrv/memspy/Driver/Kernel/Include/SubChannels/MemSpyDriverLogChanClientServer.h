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

#ifndef MEMSPYDRIVERLOGICALCHANCLIENTSERVER_H
#define MEMSPYDRIVERLOGICALCHANCLIENTSERVER_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverLogChanContainerBase.h"

// Classes referenced
class DMemSpyDriverDevice;
class TMemSpyDriverServerSessionInfo;
class TMemSpyDriverInternalServerSessionHandleParams;



NONSHARABLE_CLASS(DMemSpyDriverLogChanClientServer) : public DMemSpyDriverLogChanContainerBase
	{
public:
	DMemSpyDriverLogChanClientServer( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanClientServer();

public: // From DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt GetServerSessionHandles( TMemSpyDriverInternalServerSessionHandleParams* aParams );
    TInt GetServerSessionInfo( TAny* aSessionHandle, TMemSpyDriverServerSessionInfo* aParams );

private: // Internal methods

private: // Data members
	};


#endif
