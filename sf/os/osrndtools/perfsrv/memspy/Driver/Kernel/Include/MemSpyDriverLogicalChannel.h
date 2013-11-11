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

#ifndef MEMSPYDRIVERLOGICALCHANNEL_H
#define MEMSPYDRIVERLOGICALCHANNEL_H

// System includes
#include <e32cmn.h>
#include <kernel.h>

// Classes referenced
class DMemSpyDriverDevice;
class DMemSpyDriverLogChanBase;


NONSHARABLE_CLASS(DMemSpyDriverLogicalChannel) : public DLogicalChannelBase
	{
public:
	DMemSpyDriverLogicalChannel();
	~DMemSpyDriverLogicalChannel();

private: // from DLogicalChannelBase
	TInt DoCreate( TInt aUnit, const TDesC8* anInfo, const TVersion& aVer );
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );

private: // Channel operation handlers
    TInt SubChannelsRegister();
    TInt SubChannelConstructAndSave( DMemSpyDriverLogChanBase*& aSubChannel );
    void SubChannelsDestroy();
    DMemSpyDriverLogChanBase* SubChannelForFunction( TInt aFunction );

private: // Internal methods
    DMemSpyDriverDevice& MemSpyDevice();

private: // Data members
	DThread* iClientThread;
    RPointerArray<DMemSpyDriverLogChanBase> iSubChannels;
	};


#endif
