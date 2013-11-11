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
@ipublishedPartner
*/

#ifndef OMXILDROPPEDFRAMEEVENTEXTENSION_H_
#define OMXILDROPPEDFRAMEEVENTEXTENSION_H_

#include <openmax/il/khronos/v1_x/OMX_Types.h>

/**
 * The string that can be passed to get the index for the dropped frame event extension
 */
const char sOmxNokiaIndexParamDroppedFrameEvent [] = "OMX.NOKIA.INDEX.PARAM.DROPPEDFRAMEEVENT";

/**
 * Custom OpenMAX IL structure for the dropped frame error extension.
 */
struct OMX_NOKIA_PARAM_DROPPEDFRAMEEVENT
    {
    OMX_U32 nSize;                 /** Size of this structure, in Bytes */
    OMX_VERSIONTYPE nVersion;      /** OMX specification version information */
    OMX_U32 nPortIndex;            /** Port that this structure applies to */
	OMX_BOOL bEnabled;             /** Flag to indicate whether to generate the event */
    };

#endif /* OMXILDROPPEDFRAMEEVENTEXTENSION_H_ */
