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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERCONFIG_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERCONFIG_H

// System includes
#include <e32base.h>

// Constants
const TInt KMemSpySysMemTrackerConfigMinTimerPeriod = 30; // Seconds
const TInt KMemSpySysMemTrackerConfigMaxTimerPeriod = 60; // Seconds


NONSHARABLE_CLASS( TMemSpyEngineHelperSysMemTrackerConfig )
    {
public:
    inline TMemSpyEngineHelperSysMemTrackerConfig()
        : iTimerPeriod( KMemSpySysMemTrackerConfigMinTimerPeriod * 1000000 ),
          iDumpData( EFalse ),
          iEnabledCategories( EMemSpyEngineSysMemTrackerCategoryAll ),
          iMode( MemSpyEngineSysMemTrackerModeBasic )
        {
        }

public:
    inline TBool DumpData() const { return iDumpData; }
    inline const TTimeIntervalMicroSeconds32& TimerPeriod() const { return iTimerPeriod; }
    inline const TInt EnabledCategoryCount() 
        { 
        TInt count(0);
        for( TInt bit = EMemSpyEngineSysMemTrackerCategoryWindowGroups; bit > 0; bit = bit >> 1 )
            {
            if ( bit & iEnabledCategories )
                {
                count++;
                }
            }
        return count;
        }

public:
    enum TMemSpyEngineSysMemTrackerCategories
        {
        EMemSpyEngineSysMemTrackerCategoryNone            = 0,
        EMemSpyEngineSysMemTrackerCategoryFileServerCache = 0x0001,
        //EMemSpyEngineSysMemTrackerCategoryBitmapHandles   = 0x0002,
        EMemSpyEngineSysMemTrackerCategoryUserHeap        = 0x0002, //0x0004,
        //EMemSpyEngineSysMemTrackerCategoryKernelHeap      = 0x0008,
        EMemSpyEngineSysMemTrackerCategoryLocalChunks     = 0x0004, //0x0010,
        EMemSpyEngineSysMemTrackerCategoryGlobalChunks    = 0x0008, //0x0020,
        EMemSpyEngineSysMemTrackerCategoryRAMDrive        = 0x0010, //0x0040,
        //EMemSpyEngineSysMemTrackerCategoryUserStacks      = 0x0080,
        //EMemSpyEngineSysMemTrackerCategoryGlobalData      = 0x0100,
        EMemSpyEngineSysMemTrackerCategoryRAMLoadedCode   = 0x0020, //0x0200,
        EMemSpyEngineSysMemTrackerCategoryKernelHandles   = 0x0040, //0x0400,
        EMemSpyEngineSysMemTrackerCategoryOpenFiles       = 0x0080, //0x0800,
        EMemSpyEngineSysMemTrackerCategoryDiskusage       = 0x0100, //0x1000,
        EMemSpyEngineSysMemTrackerCategorySystemMemory    = 0x0200, //0x2000,
        EMemSpyEngineSysMemTrackerCategoryWindowGroups    = 0x0400, //0x4000,
        EMemSpyEngineSysMemTrackerCategoryAll             = 0xffff
        };
    
    enum TMemSpyEngineSysMemTrackerMode
        {
        MemSpyEngineSysMemTrackerModeBasic = 0,
        MemSpyEngineSysMemTrackerModeFull,
        MemSpyEngineSysMemTrackerModeCustom
        };
    
public:
    TTimeIntervalMicroSeconds32 iTimerPeriod;
    TBool iDumpData;
    TInt iEnabledCategories;
    TName iThreadNameFilter;
    TMemSpyEngineSysMemTrackerMode iMode;
    };


#endif
