// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "BaAssert.h"
#include "BaCompileAssert.h"

//You are not allowed to add/change something into KErrorMapTbl
//array without adding/changing TBafPanic enum value, which is used
//as an array index for accessing KErrorMapTbl array elements.
LOCAL_D const TInt KErrorMapTbl[] = 
	{
	KErrCorrupt,
	KErrCorrupt, //EBafPanicResourceLongerThanBuffer			=1,
	KErrCorrupt, //EBafPanicMatcherOutOfRange					=2,
	KErrCorrupt, //EBafPanicTextBufOutOfRange					=3,
	KErrEof,	 //EBafPanicResourceReaderEndExceeded			=4,
	KErrArgument,//EBafPanicResourceReaderStartExceeded			=5,
	KErrCorrupt, //EBafPanicBulkDataWriting						=6,
	KErrCorrupt, //EBafPanicBulkDataNotWriting					=7,
	KErrCorrupt, //EBafPanicBulkDataCannotSaveToFile			=8,
	KErrCorrupt, //EBafPanicBulkDataNotEmbedded					=9,
	KErrCorrupt, //EBafPanicSoundDataCannotChangeCompressorType	=10,
	KErrCorrupt, //EBafPanicSoundPlaying						=11,
	KErrCorrupt, //EBafPanicSoundSamplerActive					=12,
	KErrCorrupt, //EBafPanicLibAssocAlreadySet					=13,
	KErrCorrupt, //EBafPanicSystemSoundNoPath					=14,
	KErrCorrupt, //EBafPanicUnicodeTextPaddingError				=15,
	KErrCorrupt, //EBafPanicOldResourceFormat					=16, // no longer used
	KErrCorrupt, //EBafPanicInvalidSoundCategory				=17,
	KErrCorrupt, //EBafPanicInvalidSoundPriority				=18,
	KErrCorrupt, //EBafPanicNullUid								=19,
	KErrCorrupt, //EBafPanicBadResourceFileFormat				=20,
	KErrCorrupt, //EBafPanicBadArrayPosition					=21,
	KErrCorrupt, //EBafPanicBadCompareNames						=22,
	KErrCorrupt, //EBafPanicBadDrive							=23,
	KErrCorrupt, //EBafPanicBadDecompression					=24,
	KErrCorrupt, //EBafPanicIndexIsOddNumberOfBytes				=25,
	KErrCorrupt, //EBafPanicBadNumberOfResources				=26,
	KErrCorrupt, //EBafPanicBadCacheLength						=27,
	KErrCorrupt, //EBafPanicBadIndexLength1						=28,
	KErrCorrupt, //EBafPanicBadIndexLength2						=29,
	KErrCorrupt, //EBafPanicBadIndexLength3						=30,
	KErrArgument,//EBafPanicBadInitialization1					=31, We can't get ROM file address, so we expect that it is RAM file
	KErrArgument,//EBafPanicBadInitialization2					=32,
	KErrArgument,//EBafPanicBadInitialization3					=33,
	KErrCorrupt, //EBafPanicIndexOverRan						=34,
	KErrCorrupt, //EBafPanicNullPointer							=35,
	KErrGeneral, //EBafPanicNotOpened							=36, CExtra instance is NULL
	KErrCorrupt, //EBafPanicNegativeArrayIndex					=37,
	KErrEof,	 //EBafPanicEndOfStream1						=38,
	KErrEof,	 //EBafPanicEndOfStream2						=39,
	KErrCorrupt, //EBafPanicBadNumberOfConsecutivePrefixBits1	=40,
	KErrCorrupt, //EBafPanicBadNumberOfConsecutivePrefixBits2	=41,
	KErrCorrupt, //EBafPanicBadNumberOfBitsOffByteBoundary1		=42,
	KErrCorrupt, //EBafPanicBadNumberOfBitsOffByteBoundary2		=43,
	KErrCorrupt, //EBafPanicNegativeOffsetToFirstBit1			=44,
	KErrCorrupt, //EBafPanicNegativeOffsetToFirstBit2			=45,
	KErrCorrupt, //EBafPanicBadCurrentBitPosition1				=46,
	KErrCorrupt, //EBafPanicBadCurrentBitPosition2				=47,
	KErrCorrupt, //EBafPanicBadNumberOfBitsReadFromCurrentByte	=48,
	KErrCorrupt, //EBafPanicBadNumberOfBitsLeftToRead			=49,
	KErrCorrupt, //EBafPanicNotConstructed1						=50,
	KErrCorrupt, //EBafPanicNotConstructed2						=51,
	KErrCorrupt, //EBafPanicNotConstructed3						=52,
	KErrCorrupt, //EBafPanicNotConstructed4						=53,
	KErrCorrupt, //EBafPanicNotConstructed5						=54,
	KErrCorrupt, //EBafPanicNegativeLengthOfBitBuffer			=55,
	KErrArgument,//EBafPanicNegativeResourceIndex1				=56,
	KErrCorrupt, //EBafPanicNegativeResourceIndex2				=57,
	KErrCorrupt, //EBafPanicNegativeResourceIndex3				=58,
	KErrCorrupt, //EBafPanicBadFlags1							=59,
	KErrCorrupt, //EBafPanicBadFlags2							=60,
	KErrCorrupt, //EBafPanicBadFlags3							=61,
	KErrCorrupt, //EBafPanicBadFlags4							=62,
	KErrCorrupt, //EBafPanicBadFlags5							=63,
	KErrCorrupt, //EBafPanicBadFlags6							=64,
	KErrCorrupt, //EBafPanicNegativeIndexEntry					=65,
	KErrCorrupt, //EBafPanicBadSizeOfLargestResourceWhenCompletelyUncompressed1=66,
	KErrCorrupt, //EBafPanicBadSizeOfLargestResourceWhenCompletelyUncompressed2=67,
	KErrCorrupt, //EBafPanicBadSizeOfLargestResourceWhenCompletelyUncompressed3=68,
	KErrCorrupt, //EBafPanicBadHeaderLength						=69
	KErrCorrupt, //EBafPanicNullPtr1							=70,
	KErrArgument,//EBafPanicNullPtr2							=71
	KErrCorrupt, //EBafPanicNullPtr3							=72
	KErrCorrupt, //EBafPanicNullPtr4							=73
	KErrCorrupt, //EBafPanicNullPtr5							=74
	KErrCorrupt, //EBafPanicNullPtr6							=75
	KErrCorrupt, //EBafPanicNullPtr7							=76
	KErrCorrupt, //EBafPanicNullPtr8							=77
	KErrCorrupt, //EBafPanicNullPtr9							=78
	KErrCorrupt, //EBafPanicNullPtr10							=79
	KErrCorrupt, //EBafPanicNullPtr11							=80
	KErrCorrupt, //EBafPanicNullPtr12							=81
	KErrCorrupt, //EBafPanicNullPtr13							=82
	KErrCorrupt, //EBafPanicNullPtr14							=83
	KErrCorrupt, //EBafPanicNegativeNumber						=84
	KErrCorrupt, //EBafPanicBadHeader1							=85
	KErrCorrupt, //EBafPanicBadHeader2							=86
	KErrCorrupt, //EBafPanicBadHeader3							=87
	KErrCorrupt, //EBafPanicBadHeader4							=88
	KErrCorrupt, //EBafPanicBadHeader5							=89
	KErrCorrupt, //EBafPanicFileSize							=90
	KErrCorrupt, //EBafPanicOverflow							=91
	KErrCorrupt, //EBafPanicBadIndex							=92
	KErrCorrupt, //EBafPanicNegativeNumber2						=93
	KErrCorrupt, //EBafPanicFileSize2							=94
	KErrCorrupt, //EBafPanicNegativeNumber3						=95
	KErrCorrupt, //EBafPanicNegativePos							=96
	KErrCorrupt, //EBafPanicNegativeLength						=97
	KErrCorrupt, //EBafPanicMaxLength							=98
	KErrCorrupt, //EBafPanicBadIndex2							=99
	KErrCorrupt, //EBafPanicBadIndex3							=100
	KErrCorrupt, //EBafPanicMaxLength2							=101
	KErrCorrupt, //EBafPanicFileSize3							=102
	KErrCorrupt, //EBafPanicStartPos							=103
	KErrCorrupt, //EBafPanicStartPos2							=104
	KErrCorrupt, //EBafPanicFileSize4							=105
	KErrCorrupt, //EBafPanicBadOffset							=106
	KErrCorrupt, //EBafPanicBadOffset2							=107
	KErrArgument,//EBafPanicBadOpenArg							=108
	KErrCorrupt, //EBafPanicBufLength							=109
	KErrArgument //EBafPanicRFsConnectArg						=110
	};

/** This static funtion is used by TBaAssert instances to convert Panic code to
error codes. KErrorMapTbl static array is used. 

@internalComponent
@param aPanicCode - BAFL panic code.
@return Error code - KErrCorrupt, KErrArgument,...
@leave KErrArgument - if the panic code value can't be used as and array index
to access corresponding error code. */
LOCAL_C TInt Panic2ErrorL(TBafPanic aPanicCode) 
	{
	if(static_cast <TUint> (aPanicCode) >= (sizeof(KErrorMapTbl)/ sizeof(KErrorMapTbl[0])))
		{
		User::Leave(KErrArgument);
		}
	return KErrorMapTbl[static_cast <TUint> (aPanicCode)];
	}

/** Global Panic function. Used internaly by BAFL classes.
The default function behaviour is to call User::Panic().
@internalComponent */
GLDEF_C void Panic(TBafPanic aPanic)
	{
	User::Panic(_L("BAFL"), aPanic);
	}

/** By default TBaAssert methods will panic.
An error will be issued at compile time if the class size is not 
sizeof(TBaAssert::TErrorAction).
@internalComponent */
TBaAssert::TBaAssert() :
	iErrorAction(TBaAssert::EPanic)
	{
	COMPILE_TIME_ASSERT(sizeof(TBaAssert) == sizeof(TBaAssert::TErrorAction));
	}

/** Initializes TBaAssert object panic/leave behaviour.
@internalComponent
@param aErrorAction - TBaAssert::EPanic or TBaAssert::ELeave. */
TBaAssert::TBaAssert(TErrorAction aErrorAction) :
	iErrorAction(aErrorAction)
	{
	}

/** The method panics/leaves if aAssertExprVal value is EFalse. The panic will be raised
only in the DEBUG build.
@internalComponent
@param aAssertExprVal If EFalse, the method panics/leaves.
@param aPanic Panic code. If the method must leave, the code
will be mapped to some Symbian OS error code. */
void TBaAssert::AssertDebL(TBool aAssertExprVal, TBafPanic aPanic) const
	{
	DoAssertL(aAssertExprVal, ETrue, aPanic);
	}

/** The method panics/leaves if aAssertExprVal value is EFalse.
@internalComponent
@param aAssertExprVal If EFalse, the method panics/leaves.
@param aPanic Panic code. If the method must leave, the code
will be mapped to some Symbian OS error code. */
void TBaAssert::AssertRelL(TBool aAssertExprVal, TBafPanic aPanic) const
	{
	DoAssertL(aAssertExprVal, EFalse, aPanic);
	}

/** @internalComponent */
void TBaAssert::DoAssertL(TBool aAssertExprVal, TBool aDebug, TBafPanic aPanic) const
	{
	if(!aAssertExprVal)
		{
		iErrorAction == TBaAssert::ELeave ? DoLeaveL(aPanic) : DoAssert(aDebug, aPanic);
		}
	}

/** @internalComponent */
void TBaAssert::DoLeaveL(TBafPanic aPanic) const
	{
	TInt error = Panic2ErrorL(aPanic);
	User::Leave(error);
	}

/** @internalComponent */
void TBaAssert::DoAssert(TBool aDebug, TBafPanic aPanic) const
	{
	if(aDebug)
		{
		__ASSERT_DEBUG(EFalse, ::Panic(aPanic));
		}
	else
		{
		::Panic(aPanic);
		}
	}



