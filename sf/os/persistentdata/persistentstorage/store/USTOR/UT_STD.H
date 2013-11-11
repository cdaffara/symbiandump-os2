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

#include <s32stor.h>
#include "U32STD.H"
#include "U32PERM.H"

enum TStorePanic
	{
	EStoreNotOpen,
	EStoreInUse,
	EStoreIdOutOfRange,
	EStoreSwizzleBadId,
	EStoreSwizzleBadPtr,
	EStoreSwizzleBadReference,
	EStoreMapSwizzleMissing,
	EStoreMapIdMissing,
	EStoreMapIntroducingAlias,
	EStoreMapFreeIdPresent
	};

GLREF_C void Panic(TStorePanic aPanic);

#include "UT_STD.INL"

