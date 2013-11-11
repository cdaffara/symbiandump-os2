// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32mem.h>
#include "U32STD.H"

NONSHARABLE_CLASS(HBufBuf) : public TBufBuf
	{
public:
	static HBufBuf* NewL(CBufBase& aBuf,TInt aPos,TInt aMode=ERead|EWrite);
private:
	void DoRelease();
	};
//
enum TMemPanic
	{
	EMemReadLengthNegative,
	EMemReadNoTransfer,
	EMemWriteLengthNegative,
	EMemWriteNoTransfer,
	EMemCannotRead,
	EMemCannotWrite,
	EMemMarkInvalid,
	EMemLocationInvalid
	};

GLREF_C void Panic(TMemPanic aPanic);

#include "UM_STD.INL"

