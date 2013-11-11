#ifndef __US_STD_H__
#define __US_STD_H__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file
 @internalComponent
*/
enum TStreamPanic
	{
	EStreamNotOpen,
	EStreamReadLengthNegative,
	EStreamReadBeyondEnd,
	EStreamReadNoTransfer,
	EStreamReadInBreach,
	EStreamWriteLengthNegative,
	EStreamWriteBeyondEnd,
	EStreamWriteNoTransfer,
	EStreamWriteInBreach,
	EStreamDoesNotUnderstand,
	EStreamCannotRead,
	EStreamCannotWrite,
	EStreamCannotSeek,
	EStreamTransferNegative,
	EStreamPosInvalid,
	EStreamMarkInvalid,
	EStreamLocationInvalid,
	EStreamAreaInvalid,
	EStreamModeInvalid,
	EStreamUnderflowInBreach,
	EStreamOverflowInBreach,
	EStreamFilterInBreach,
	EStreamPushLengthNegative,
	EStreamPushNoTransfer,
	EStreamPushInBreach,
	EStreamPullLengthNegative,
	EStreamPullNoTransfer,
	EStreamPullInBreach,
	EStreamCardinalityOutOfRange,
	EStreamOffsetNegative,
	EStreamExtentNegative,
	EStreamTypeInvalid,
	EStreamCommitted
	};


#endif
