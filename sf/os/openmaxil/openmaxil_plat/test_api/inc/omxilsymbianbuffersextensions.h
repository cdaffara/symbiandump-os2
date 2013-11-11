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

#ifndef OMXILSYMBIANBUFFERSOURCEEXTENSIONS_H
#define OMXILSYMBIANBUFFERSOURCEEXTENSIONS_H

#include <openmax/il/khronos/v1_x/OMX_Types.h>

/**
 * The string that the Symbian's OpenMAX IL Buffer components will
 * translate into a 32-bit OpenMAX IL index to support the message queue communication
 * with user client
 */
#if 1
const char sOmxSymbianBufferMsgQueue [] = "OMX.SYMBIAN.INDEX.PARAM.BUFFERMSGQUEUE";
#endif
const char sOmxSymbianBufferSize[] = "OMX.SYMBIAN.INDEX.PARAM.BUFFERSIZE";
const char sOmxSymbianBufferChunk[] = "OMX.SYMBIAN.INDEX.PARAM.BUFFERCHUNK";

/*
 * Custom index used by Symbian's OpenMAX IL Buffers to select the
 * structure for getting message queue global names
 * 
 * Vendor specific structures should be in the range of 0x7F000000 to 0x7FFFFFFE.
 * This range is not broken out by vendor, so private indexes are not guaranteed unique 
 * and therefore should only be sent to the appropriate component.
 */
#if 1
#define OMX_SymbianIndexParamBufferMsgQueueData 0x7F000006 // TBC
#endif
#define OMX_SymbianIndexParamBufferSize 0x7F000007
#define OMX_SymbianIndexParam3PlaneBufferChunk 0x7F000008

#if 1 
/**
 * Custom OpenMAX IL structure to be used as a container for message queue global names
 */
struct OMX_SYMBIAN_PARAM_BUFFER_MSGQUEUE
	{
    OMX_U32 nSize;                 /**< Size of this structure, in Bytes */
    OMX_VERSIONTYPE nVersion;      /**< OMX specification version information */
    OMX_U32 nPortIndex;            /**< Port that this structure applies to */
    OMX_PTR pBufSrcComponentHandles;	/** This is a pointer to TBufSrcComponentHandles */
	};
#endif

/**
 Custom OpenMAX IL structure used as container for buffer size on RChunk backed buffers
 */

struct OMX_SYMBIAN_PARAM_BUFFER_SIZE
	{
	OMX_U32 nSize;				/**< Size of this structure, in Bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_U32 nPortIndex;			/**< Port that this structure applies to */
	OMX_S32 nBufferSize;		/**< Buffer size in bytes */
	};

struct OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA
	{
	OMX_U32 nSize;                 /**< Size of this structure, in Bytes */
	OMX_VERSIONTYPE nVersion;      /**< OMX specification version information */
	OMX_U32 nPortIndex;	           /**< Port that this structure applies to */
	OMX_U32 nChunk;	               /**< Chunk */
	OMX_U32 nFilledBufferQueue;	   /**< FilledBufferQueue */
	OMX_U32 nAvailableBufferQueue; /**< AVailableBufferQueue */	
	OMX_U64 nThreadId;             /**< The thread which creates the chunk */
	};

#endif // OMXILSYMBIANBUFFERSOURCEEXTENSIONS_H
