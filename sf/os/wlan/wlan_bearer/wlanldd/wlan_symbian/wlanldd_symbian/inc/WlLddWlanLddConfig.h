/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Module configuration file. 
*                This file is the 1st one to be included 
*                by every source file of this project
*
*/

/*
* %version: 8 %
*/

#ifndef WLLDDWLANLDDCONFIG_H
#define WLLDDWLANLDDCONFIG_H

#include "umac_types.h"
#include "gendebug.h"
#include <kernel.h>

extern void os_assert( 
    const TUint8* aError, 
    const TUint8* aFile, 
    TUint32 aLine );

#include "trace_util.h"

#endif // WLLDDWLANLDDCONFIG_H
