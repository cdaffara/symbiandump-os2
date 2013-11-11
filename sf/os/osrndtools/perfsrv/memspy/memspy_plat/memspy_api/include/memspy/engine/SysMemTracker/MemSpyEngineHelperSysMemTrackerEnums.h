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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENUMS_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENUMS_H

// System includes
#include <e32std.h>

// Enumerations
enum TMemSpyEngineSysMemTrackerType
    {
    EMemSpyEngineSysMemTrackerTypeHeapUser = 0,
    EMemSpyEngineSysMemTrackerTypeHeapKernel,
    EMemSpyEngineSysMemTrackerTypeChunk,
    EMemSpyEngineSysMemTrackerTypeRamDrive,
    EMemSpyEngineSysMemTrackerTypeStack,
    EMemSpyEngineSysMemTrackerTypeGlobalData,
    EMemSpyEngineSysMemTrackerTypeCode,
    EMemSpyEngineSysMemTrackerTypeOpenFile,
    EMemSpyEngineSysMemTrackerTypeDiskSpace,
    EMemSpyEngineSysMemTrackerTypeBitmap,
    EMemSpyEngineSysMemTrackerTypeHandleGeneric,
    EMemSpyEngineSysMemTrackerTypeHandlePAndS,
    EMemSpyEngineSysMemTrackerTypeFbserv,
    EMemSpyEngineSysMemTrackerTypeFileServerCache,
    EMemSpyEngineSysMemTrackerTypeSystemMemory,
    EMemSpyEngineSysMemTrackerTypeWindowServer,
    
    //
    EMemSpyEngineSysMemTrackerTypeCount // MARKER - must be last
    };


enum TMemSpyEngineSysMemTrackerEntryAttributes
    {
    EMemSpyEngineSysMemTrackerEntryAttributeNone = 0,
    EMemSpyEngineSysMemTrackerEntryAttributeIsNew = 1,
    EMemSpyEngineSysMemTrackerEntryAttributeIsAlive = 2
    };


#endif