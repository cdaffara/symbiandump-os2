/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


/**
   @file
   @internalComponent
*/

#ifndef MMFBUFFERSHARED_H
#define MMFBUFFERSHARED_H

#include <e32cmn.h>

const TInt KMinBuffers = 1;
const TInt KMinBufferSize = 4000;

/**
 * The T class holds the filled buffer information for each buffer.
 */
class TFilledBufferHeaderV2 
	{
public:
	/** Amount of buffer filled with actual data */
	TUint iFilledLength; 
	/** Any timestamp associated with the buffer */
	TInt64 iTimeStamp;
	/** Combination of OMX_BUFFERFLAG_*, specified at page 68 in the OpenMAX spec. */
	TUint iFlags; 
	/** offset **/
	TUint32 iOffset;
	};

/** Shared chunk buffers information. It can be set by the user. */
class TMMSharedChunkBufConfig
	{
public:
	/** The number of buffers. */
	TInt iNumBuffers;
	/** The size of each buffer in bytes. */
	TInt iBufferSizeInBytes;
	};

struct TBufSrcComponentHandles
	{
	/** The descriptor containing the name of the global chunk to be opened  */
	TName iChunkName;
	/** The descriptor containing the name of the global message queue 
	 for the available buffer notification */
	TName iBufferAvailMsgQueue;
	/** The descriptor containing the name of the global message queue 
	 for the filled buffer */
	TName iBufferFilledMsgQueue;
	};

#endif // MMFBUFFERSHARED_H
