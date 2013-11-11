// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SD_BUF.CPP
// HDbsBuf class - "DBMS security" related code only
// 
//

#include "SD_STD.H"

/**
Standard phase-one factory method for HDbsBuf instances.
@param aDbSession RDbs session instance.
@param aFunction DBMS server function code. EDbsDatabaseList DBMS function code is only used.
@param aArgs EDbsDatabaseList related set of arguments.
@return A pointer to the created HDbsBuf instance.
@leave One of the system-wide error codes, including KErrNoMemory.
*/
HDbsBuf* HDbsBuf::NewLC(const RDbs& aDbSession, TDbsFunction aFunction, TIpcArgs& aArgs)
	{
	HDbsBuf* self = new (ELeave) HDbsBuf;
	self->PushL();
	self->ConstructL(aDbSession, aFunction, aArgs);
	return self;
	}

/**
Standard phase-two construction method for HDbsBuf instances.
@param aDbSession RDbs session instance.
@param aFunction DBMS server function code. EDbsDatabaseList DBMS function code is only used.
@param aArgs EDbsDatabaseList related set of arguments.
@leave One of the system-wide error codes, including KErrNoMemory.
*/
void HDbsBuf::ConstructL(const RDbs& aDbSession, TDbsFunction aFunction, TIpcArgs& aArgs)
	{
	TPckg<TDbsStreamBuf> pckg(iBuf);
	aArgs.Set(3, &pckg);
	iIpc.OpenL(aDbSession, aFunction, aArgs);
	TUint8* base = iBuf.iData;
	// if reading we already have one buffer-full of data
	TInt avail = Max(0, Min(iBuf.iExt, KDbsStreamBufSize));
	SetBuf(ERead, base, base + avail);
	SetPos(ERead, avail);
	SetBuf(EWrite, base, base);
	SetPos(EWrite, 0);
	}
