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

#ifndef OMXILSYMBIANSHAREDCHUNKTENSIONS_H
#define OMXILSYMBIANSHAREDCHUNKTENSIONS_H

#include <openmax/il/khronos/v1_x/OMX_Types.h>

/**
 * The string that the OpenMAX IL component will translate into a 32-bit
 * OpenMAX IL index to support the communication of Symbian shared chunk
 * metadata with a tunnelled component
 */
const char sOmxSymbianSharedChunkMetadata [] = "OMX.SYMBIAN.INDEX.CONFIG.SHAREDCHUNKMETADATA";


/*
 * Custom index used by an OpenMAX IL component to select the structure for
 * getting/setting the Symbian share chunk metadata
 *
 * Vendor specific structures should be in the range of 0x7F000000 to 0x7FFFFFFE.
 * This range is not broken out by vendor, so private indexes are not guaranteed unique
 * and therefore should only be sent to the appropriate component.
 */
#define OMX_SymbianIndexConfigSharedChunkMetadata 0x7FD19E5C // TBC

/*
 * This constant defines the max number of share chunk offsets
 */
#define OMX_SYMBIAN_SHARED_CHUNK_OFFSETS_MAX 128

/**
 * Custom OpenMAX IL structure to be used as a container for Symbian shared
 * chunk related information
 */
struct OMX_SYMBIAN_CONFIG_SHARED_CHUNK_METADATA
	{
    OMX_U32 nSize;                 /**< Size of this structure, in Bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex;            /**< Port that this structure applies to */
    OMX_U32 nHandleId;	           /** This is a handle id of the shared chunk */
    OMX_U64 nOwnerThreadId;	       /** This is thread that contains the handle */
	};

#endif // OMXILSYMBIANSHAREDCHUNKTENSIONS_H

