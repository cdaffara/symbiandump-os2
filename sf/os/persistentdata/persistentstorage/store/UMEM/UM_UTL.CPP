// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "UM_STD.H"

#pragma BullseyeCoverage off

//
// Panic the process with STORE-Memory as the category.
//
GLDEF_C void Panic(TMemPanic aPanic)
	{
	_LIT(KCategory,"STORE-Memory");
	User::Panic(KCategory,aPanic);
	}

#pragma BullseyeCoverage on

HBufBuf* HBufBuf::NewL(CBufBase& aBuf,TInt aPos,TInt aMode)
//
// Create a pre-set buffer stream buffer.
//
	{
	HBufBuf* buf=new(ELeave) HBufBuf;
	buf->Set(aBuf,aPos,aMode);
	return buf;
	}

void HBufBuf::DoRelease()
//
// Finished with this stream buffer.
//
	{
	delete this;
	}

