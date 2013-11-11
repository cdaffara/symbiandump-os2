/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef OMXILSYMBIANVIDEOGRAPHICSINKEXTENSIONS_H
#define OMXILSYMBIANVIDEOGRAPHICSINKEXTENSIONS_H


#include <openmax/il/khronos/v1_x/OMX_Types.h>


/**
 * The string that the Symbian's OpenMAX IL Graphic Sink component will
 * translate into a 32-bit OpenMAX IL index to support the TSurfaceConfig class
 */
const char sOmxSymbianGfxSurfaceConfig [] = "OMX.SYMBIAN.INDEX.PARAM.VIDEO.GFX.SURFACECONFIG";


/**
 * Custom OpenMAX IL structure to be used as a container for an
 * TSurfaceConfig class
 */
struct OMX_SYMBIAN_VIDEO_PARAM_SURFACECONFIGURATION
	{
    OMX_U32 nSize;                 // Size of this structure, in Bytes
    OMX_VERSIONTYPE nVersion;      // OMX specification version information
    OMX_U32 nPortIndex;            // Port that this structure applies to
    OMX_PTR pSurfaceConfig;        // This is a pointer to TSurfaceConfig class
	};


#endif // OMXILSYMBIANVIDEOGRAPHICSINKEXTENSIONS_H
