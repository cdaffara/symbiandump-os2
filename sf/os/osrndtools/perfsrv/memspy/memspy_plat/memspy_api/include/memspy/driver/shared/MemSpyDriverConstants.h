/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYDRIVERCONSTANTS_H
#define MEMSPYDRIVERCONSTANTS_H

// System includes
#include <e32def.h>


/**
*
*/
_LIT(KMemSpyDriverDeviceName, "MEMSPYDRIVER");


/**
 *
 */
inline TVersion KMemSpyDriverVersion() { return TVersion(1, 0, 0); }


/**
 * As this LDD allows to bypass platform security, we need to restrict
 * access to a few trusted clients.
 */
const TUint32 KMemSpyUiSID = 0x2002129F;
const TUint32 KMemSpyConsoleUiSID = 0x2002129E;


/**
 *
 */
const TInt KMemSpyDriverEnumerateContainerHandles = -1;


/**
 * Used for get heap data calls when requesting the kernel heap
 * 
 */
const TUint KMemSpyDriverGetKernelHeapDataPseudoThreadId = 0;



#endif
