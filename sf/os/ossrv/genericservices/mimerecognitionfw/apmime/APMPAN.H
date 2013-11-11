// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Panics and DLL stuff for APMIME.DLL
// 
//

#if !defined(__APMPAN_H__)
#define __APMPAN_H__

enum TApmPanic
	{
	EPanicFindAllAppsNotCalled,
	EPanicLibraryNotInList,
	EPanicIndexOutOfRange,
	EPanicNegativeIndex,
	EInvalidDataType,
	EInvalidArgument,
	EArrayNotEmpty,
	EPanicNotBeingUsedFromWithinApparcServerProcess,
	//
	EDPanicBadDirName,
	EDPanicDriveOutOfRange,
	EDPanicStartDriveOutOfRange,
	EDPanicInvalidData,
	EDPanicDuplicateRecognizer,
	EDPanicNullPointer1,
	EDPanicNullPointer2,
	EDPanicNullPointerExpected1,
	EDPanicNullPointerExpected2,
	EDPanicBadFlagState1,
	EDPanicBadFlagState2,
	};

GLREF_C void Panic(TApmPanic aPanic);
GLREF_C const TDesC& Category();

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
/**
@internalComponent
*/
#define KApaAppTypeDes _L8("x-epoc/x-app")
/**
@internalComponent
*/
const TInt KDataArrayGranularity=5;

/**
@internalComponent
*/
_LIT8(KEpocUrlDataTypeHeader, "X-Epoc-Url/");

#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

#endif
