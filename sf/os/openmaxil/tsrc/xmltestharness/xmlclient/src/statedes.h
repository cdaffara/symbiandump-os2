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

#ifndef STATEDES_H_
#define STATEDES_H_

#include <e32cmn.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Other.h>

const TDesC* StateDes(OMX_STATETYPE aState);
const TDesC* ClockStateDes(OMX_TIME_CLOCKSTATE aClockState);

#endif /*STATEDES_H_*/
