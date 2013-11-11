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

#include <s32btree.h>
#include "U32STD.H"

enum TBtreePanic
	{
	EBadEntryCount,
	EBadEntryPos,
	EBadEntrySize,
	EIllegalSplit,
	EIllegalOverflow,
	ECannotConcatenate,
	ECannotMakeRoot,
	ETreeOverflow,
	EPathUnderflow,
	EBadTreeHeight,
	EInvalidTreePos,
	ENoConnect,
	ENoTree,
	EInvalidKeyComparison,
	EIllegalPivot
	};

GLREF_C void Panic(TBtreePanic aPanic);

#include "UB_STD.INL"

