// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__PCSTORECONST_H__)
#define __PCSTORECONST_H__

#include <pcstore/pcstoredef.h>

namespace PCStore
{
const TInt  KMaxTInt=0x7fffffff;
const TUint KMaxTUint8=0xffu;
const TUint KMaxTUint16=0xffffu;

const TInt KMaxCardinality=0x1fffffff;

const TUint32 KMaxStreamIdValue=0xfffffff;

const TInt KShiftCardinality8=1;
const TInt KShiftCardinality16=2;
const TInt KShiftCardinality32=3;

/**
Defines the number of TUids that form a TUidType.
*/
const TInt KMaxCheckedUid=3;

/**
Defines a Null UID value.
@see TUid
*/
const TInt KNullUidValue=0;

//error of not found, only used in UCMP.cpp, which is borrowed from Symbian
const TInt KErrNotFound=(-1);
}
#endif // !defined(__PCSTORECONST_H__)
