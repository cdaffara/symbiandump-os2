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

#ifndef MEMSPYDRIVEROPCODES_H
#define MEMSPYDRIVEROPCODES_H

enum TMemSpyDriverOpCode
	{
    // RAW MEMORY
    EMemSpyDriverOpCodeRawMemoryBase = 100,
    EMemSpyDriverOpCodeRawMemoryRead,
    EMemSpyDriverOpCodeRawMemoryEnd,

    // THREAD & PROCESS
	EMemSpyDriverOpCodeThreadAndProcessBase = 120,
	EMemSpyDriverOpCodeThreadAndProcessGetInfoThread,
	EMemSpyDriverOpCodeThreadAndProcessGetInfoProcess,
    EMemSpyDriverOpCodeThreadAndProcessEndThread,
    EMemSpyDriverOpCodeThreadAndProcessOpenThread,
    EMemSpyDriverOpCodeThreadAndProcessOpenProcess,
    EMemSpyDriverOpCodeThreadAndProcessSuspendAllThreads,
    EMemSpyDriverOpCodeThreadAndProcessResumeAllThreads,
	EMemSpyDriverOpCodeThreadAndProcessGetThreads,
    EMemSpyDriverOpCodeThreadAndProcessSetPriorityThread,
	EMemSpyDriverOpCodeThreadAndProcessEnd,

    // CODE SEGS
	EMemSpyDriverOpCodeCodeSegsBase = 140,
	EMemSpyDriverOpCodeCodeSegsGetAll,
	EMemSpyDriverOpCodeCodeSegsGetCodeSegsForProcess,
	EMemSpyDriverOpCodeCodeSegsGetCodeSegInfo,
	EMemSpyDriverOpCodeCodeSegsEnd,

    // CHUNKS
	EMemSpyDriverOpCodeChunkBase = 160,
    EMemSpyDriverOpCodeChunkGetHandles,
    EMemSpyDriverOpCodeChunkGetInfo,
	EMemSpyDriverOpCodeChunkEnd,

    // HEAP USER DATA
    EMemSpyDriverOpCodeHeapUserDataBase = 180,
    EMemSpyDriverOpCodeHeapUserDataGetInfo,
    EMemSpyDriverOpCodeHeapUserDataFetchCellList,
    EMemSpyDriverOpCodeHeapUserDataGetFull,
    EMemSpyDriverOpCodeHeapUserDataEnd,    

    // HEAP KERNEL DATA
    EMemSpyDriverOpCodeHeapKernelDataBase = 200,
    EMemSpyDriverOpCodeHeapKernelDataGetInfo,
    EMemSpyDriverOpCodeHeapKernelDataGetIsDebugKernel,
    EMemSpyDriverOpCodeHeapKernelDataFetchCellList,
    EMemSpyDriverOpCodeHeapKernelDataCopyHeap,
    EMemSpyDriverOpCodeHeapKernelDataGetFull,
    EMemSpyDriverOpCodeHeapKernelDataFreeHeapCopy,
    EMemSpyDriverOpCodeHeapKernelDataEnd,    
    
    // HEAP USER WALK
    EMemSpyDriverOpCodeHeapUserWalkBase = 220,
    EMemSpyDriverOpCodeHeapUserWalkInit,
    EMemSpyDriverOpCodeHeapUserWalkGetCellInfo,
    EMemSpyDriverOpCodeHeapUserWalkReadCellData,
    EMemSpyDriverOpCodeHeapUserWalkNextCell,
    EMemSpyDriverOpCodeHeapUserWalkClose,
    EMemSpyDriverOpCodeHeapUserWalkEnd,

    // STACK
    EMemSpyDriverOpCodeStackBase = 240,
    EMemSpyDriverOpCodeStackGetInfo,
    EMemSpyDriverOpCodeStackGetData,
    EMemSpyDriverOpCodeStackEnd,

    // CONTAINERS
    EMemSpyDriverOpCodeContainersBase = 260,
    EMemSpyDriverOpCodeContainersGetHandles,
    EMemSpyDriverOpCodeContainersGetHandleInfo,
    EMemSpyDriverOpCodeContainersGetApproxSize,
    EMemSpyDriverOpCodeContainersGetReferencesToMyThread,
    EMemSpyDriverOpCodeContainersGetReferencesToMyProcess,
    EMemSpyDriverOpCodeContainersGetPAndSInfo,
    EMemSpyDriverOpCodeContainersGetCondVarSuspendedThreads,
    EMemSpyDriverOpCodeContainersGetCondVarSuspendedThreadInfo,
    EMemSpyDriverOpCodeContainersEnd,

    // CLIENT SERVER
    EMemSpyDriverOpCodeClientServerBase = 280,
    EMemSpyDriverOpCodeClientServerGetServerSessionHandles,
    EMemSpyDriverOpCodeClientServerGetServerSessionInfo,
    EMemSpyDriverOpCodeClientServerEnd,
    
    // PROCESS INSPECTION
    EMemSpyDriverOpCodeProcessInspectBase = 300,
    EMemSpyDriverOpCodeProcessInspectOpen,
    EMemSpyDriverOpCodeProcessInspectClose,
    EMemSpyDriverOpCodeProcessInspectRequestChanges,
    EMemSpyDriverOpCodeProcessInspectRequestChangesCancel,
    EMemSpyDriverOpCodeProcessInspectAutoStartListReset,
    EMemSpyDriverOpCodeProcessInspectAutoStartListAdd,
    EMemSpyDriverOpCodeProcessInspectEnd,

    // EVENT MONITOR
    EMemSpyDriverOpCodeEventMonitorBase = 320,
    EMemSpyDriverOpCodeEventMonitorOpen,
    EMemSpyDriverOpCodeEventMonitorClose,
    EMemSpyDriverOpCodeEventMonitorNotify,
    EMemSpyDriverOpCodeEventMonitorNotifyCancel,
    EMemSpyDriverOpCodeEventMonitorEnd,

    // MISC
    EMemSpyDriverOpCodeMiscBase = 340,
    EMemSpyDriverOpCodeMiscSetRHeapVTable,
    EMemSpyDriverOpCodeMiscGetMemoryModelType,
    EMemSpyDriverOpCodeMiscGetRoundToPageSize,
    EMemSpyDriverOpCodeMiscImpersonate,
    EMemSpyDriverOpCodeMiscEnd,
	};
	
#endif
