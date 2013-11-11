// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CSdpCodecStringPool.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <stringpool.h>
#include "csdpcodecstringpool.h"
#include "sdpcodecstringconstants.h"
#include "_sdpdefs.h"


// ----------------------------------------------------------------------------
// CSdpCodecStringPool::NewLC
// ----------------------------------------------------------------------------
//
CSdpCodecStringPool* CSdpCodecStringPool::NewLC()
	{
	CSdpCodecStringPool* self = new(ELeave)CSdpCodecStringPool;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpCodecStringPool::~CSdpCodecStringPool
// ----------------------------------------------------------------------------
//
CSdpCodecStringPool::~CSdpCodecStringPool()
	{
	iPool.Close();
	}

// ----------------------------------------------------------------------------
// CSdpCodecStringPool::StringPool
// ----------------------------------------------------------------------------
//
RStringPool CSdpCodecStringPool::StringPool()
	{
	return iPool;
	}

// ----------------------------------------------------------------------------
// CSdpCodecStringPool::StringTable
// ----------------------------------------------------------------------------
//
const TStringTable& CSdpCodecStringPool::StringTable()
	{
	return SdpCodecStringConstants::Table;
	}

// ----------------------------------------------------------------------------
// CSdpCodecStringPool::CSdpCodecStringPool
// ----------------------------------------------------------------------------
//
CSdpCodecStringPool::CSdpCodecStringPool()
	{
	}
	
// ----------------------------------------------------------------------------
// CSdpCodecStringPool::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpCodecStringPool::ConstructL()
	{
	iPool.OpenL(SdpCodecStringConstants::Table);
	}

