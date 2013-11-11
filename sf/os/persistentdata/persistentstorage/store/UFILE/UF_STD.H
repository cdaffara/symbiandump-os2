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

#include <s32file.h>
#include "U32STD.H"
#include "U32PERM.H"

#ifndef MAX_READ_AHEAD_VALUE
	#define MAX_READ_AHEAD_VALUE 2048
#else
	#if MAX_READ_AHEAD_VALUE < 512
		#error "MAX_READ_AHEAD_VALUE macro value can't be less than 512"
	#endif
#endif

const TInt KMaxFileBufReadAhead=MAX_READ_AHEAD_VALUE;

#ifndef FILE_BLOCK_SIZE
	#define FILE_BLOCK_SIZE 1024
#else
	#if FILE_BLOCK_SIZE < 512
		#error "FILE_BLOCK_SIZE macro value can't be less than 512"
	#endif
	#if ((FILE_BLOCK_SIZE) & ((FILE_BLOCK_SIZE) - 1)) != 0
		#error "FILE_BLOCK_SIZE macro value must be power of 2"
	#endif
#endif

const TInt KFileBufBlockSize=FILE_BLOCK_SIZE;

//
enum TFilePanic
	{
	EFileReadLengthNegative,
	EFileReadBeyondEnd,
	EFileReadNoTransfer,
	EFileWriteLengthNegative,
	EFileWriteBeyondEnd,
	EFileWriteNoTransfer,
	EFileWriteOutstanding,
	EFileMarkInvalid,
	EFileLocationInvalid,
	EFileStoreBadType,
	EStoreUid3MustNotBeNull
	};

GLREF_C void Panic(TFilePanic aPanic);

#include "UF_STD.INL"

