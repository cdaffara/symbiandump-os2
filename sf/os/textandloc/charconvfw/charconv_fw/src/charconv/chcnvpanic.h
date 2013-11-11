/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CHCNVPANIC_H__
#define __CHCNVPANIC_H__

#include "e32std.h"

enum TPanic
	{
	EPanicDestructionDuringConversion=1,
	EPanicBadTlsDataReferenceCount,
	EPanicNoTlsData,
	EPanicBadToggleOfCurrentCharacterSetConverter,
	EPanicPastEndOfFile,
	EPanicNotPastFlagPoleForReAllocation,
	EPanicReAllocatedCellMoved,
	EPanicNegativeNumberOfBytes,
	EPanicInconsistentFileReader1,
	EPanicInconsistentFileReader2,
	EPanicInconsistentFileReader3,
	EPanicInconsistentFileReader4,
	EPanicInconsistentFileReader5,
	EPanicInconsistentFileReader6,
	EPanicInconsistentFileReader7,
	EPanicInconsistentFileReader8,
	EPanicInconsistentFileReader9,
	EPanicInconsistentFileReader10,
	EPanicInconsistentFileReader11,
	EPanicInconsistentFileReader12,
	EPanicInconsistentFileReader13,
	EPanicInconsistentFileReader14,
	EPanicInconsistentFileReader15,
	EPanicInconsistentFileReader16,
	EPanicInconsistentFileReader17,
	EPanicInconsistentFileReader18,
	EPanicNegativeBufferLength1,
	EPanicNegativeBufferLength2,
	EPanicCharacterSetAlreadyAdded,
	EPanicNotInitialisedToNull1,
	EPanicNotInitialisedToNull2,
	EPanicCharacterSetsNotSorted,
	EPanicNotInitialisedToZero1,
	EPanicNotInitialisedToZero2,
	EPanicPointerPastUpperLimit1,
	EPanicPointerPastUpperLimit2,
	EPanicPointerPastUpperLimit3,
	EPanicPointerPastUpperLimit4,
	EPanicPointerPastUpperLimit5,
	EPanicPointerPastUpperLimit6,
	EPanicPointerPastUpperLimit7,
	EPanicPointerPastUpperLimit8,
	EPanicPointerPastUpperLimit9,
	EPanicPointerPastUpperLimit10,
	EPanicPointerPastUpperLimit11,
	EPanicPointerPastUpperLimit12,
	EPanicPointerPastUpperLimit13,
	EPanicPointerPastUpperLimit14,
	EPanicPointerPastUpperLimit15,
	EPanicPointerPastUpperLimit16,
	EPanicPointerPastUpperLimit17,
	EPanicPointerPastUpperLimit18,
	EPanicPointerPastUpperLimit19,
	EPanicPointerPastUpperLimit20,
	EPanicPointerPastUpperLimit21,
	EPanicPointerPastUpperLimit22,
	EPanicPointerPastUpperLimit23,
	EPanicPointerPastUpperLimit24,
	EPanicPointerPastUpperLimit25,
	EPanicBadDefaultEndiannessOfForeignCharacters,
	EPanicBadDowngradeForExoticLineTerminatingCharacters1,
	EPanicBadDowngradeForExoticLineTerminatingCharacters2,
	EPanicNullCharacterSetIdentifier1,
	EPanicNullCharacterSetIdentifier2,
	EPanicNullCharacterSetIdentifier3,
	EPanicNameIsNotFileName,
	EPanicNotReachedEndOfFile,
	EPanicNoReplacementForUnconvertibleUnicodeCharacters,
	EPanicNoConversionData1,
	EPanicNoConversionData2,
	EPanicInputCharacterCodeNotInRange,
	EPanicZeroOffset1,
	EPanicZeroOffset2,
	EPanicEmptyKeyedTable1616,
	EPanicEmptyKeyedTable3232,
	EPanicBadIndices1,
	EPanicBadIndices2,
	EPanicEmptyKeyedTable16OfIndexedTables16,
	EPanicEmptyKeyedTable32OfIndexedTables32,
	EPanicBadAlgorithm1,
	EPanicBadAlgorithm2,
	EPanicNullPointer1,
	EPanicNullPointer2,
	EPanicCrossedPointers,
	EPanicBadNumberOfRanges1,
	EPanicBadNumberOfRanges2,
	EPanicBadNumberOfRanges3,
	EPanicBadNumberOfRanges4,
	EPanicBadNumberOfRanges5,
	EPanicPointerPastLowerLimit1,
	EPanicPointerPastLowerLimit2,
	EPanicBadNon16BitCharacterCode1,
	EPanicBadNon16BitCharacterCode2,
	EPanicBadSizeOfForeignOutputCharacterCode,
	EPanicBadEndianness1,
	EPanicBadEndianness2,
	EPanicBadEndianness3,
	EPanicBadNumberOfRemainingForeignBytes1,
	EPanicBadNumberOfRemainingForeignBytes2,
	EPanicBadNumberOfSubsequentBytes,
	EPanicBadIndex,
	EPanicDuplicateIndexOrNotAscending,
	EPanicBadNumberOfUnconvertibleCharacters,
	EPanicCharacterSetNotPresent,
	EPanicCharacterSetConverterNotDestroyed,	
 	EPanicNullCharacterSetIdentifier4,			
 	EPanicCharacterSetConverterNotLoaded
	};

_LIT(KLitPanicText, "CHARCONV");

inline void Panic(TPanic aPanic)
	{
	User::Panic(KLitPanicText, aPanic);
	}

#endif //__CHCNVPANIC_H__
