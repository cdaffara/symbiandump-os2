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

#ifndef MEMSPYDRIVERENUMERATIONSSHARED_H
#define MEMSPYDRIVERENUMERATIONSSHARED_H

// System includes
#include <u32std.h>

// Enumerations
enum TMemSpyDriverCellType
	{
	EMemSpyDriverAllocatedCellMask	= 0x000000FF,
	EMemSpyDriverFreeCellMask		= 0x0000FF00,
	EMemSpyDriverBadCellMask		= 0xFF000000,

	EMemSpyDriverHeapAllocation		= 0x00000001,
	EMemSpyDriverDlaAllocation		= 0x00000002,
	EMemSpyDriverPageAllocation		= 0x00000003,
	EMemSpyDriverSlabAllocation		= 0x00000004,
	
	EMemSpyDriverHeapFreeCell		= 0x00000100,
	EMemSpyDriverDlaFreeCell		= 0x00000200,
	EMemSpyDriverSlabFreeCell		= 0x00000300, // Used to track free cells in partially-filled slabs
	EMemSpyDriverSlabFreeSlab		= 0x00000400, // Used to track entirely empty slabs (that don't have a specific cell size)

	EMemSpyDriverHeapBadFreeCellAddress			= 0x01000000,
	EMemSpyDriverHeapBadFreeCellSize			= 0x02000000,
	EMemSpyDriverHeapBadAllocatedCellSize		= 0x03000000,
	EMemSpyDriverHeapBadAllocatedCellAddress	= 0x04000000,
	};


enum TMemSpyDriverThreadOrProcess
    {
    EMemSpyDriverThreadOrProcessTypeThread = EThread, // From TObjectType
    EMemSpyDriverThreadOrProcessTypeProcess = EProcess
    };


enum TMemSpyDriverDomainType
    {
    EMemSpyDriverDomainUser = 0,
    EMemSpyDriverDomainKernel
    };


enum TMemSpyDriverChunkType
    {
    EMemSpyDriverChunkTypeUnknown = 0,
    EMemSpyDriverChunkTypeHeap,
    EMemSpyDriverChunkTypeHeapKernel,
    EMemSpyDriverChunkTypeStackAndProcessGlobalData,
    EMemSpyDriverChunkTypeStackKernel,
    EMemSpyDriverChunkTypeGlobalData,
    EMemSpyDriverChunkTypeCode,
    EMemSpyDriverChunkTypeCodeGlobal,
    EMemSpyDriverChunkTypeCodeSelfModifiable,
    EMemSpyDriverChunkTypeLocal,
    EMemSpyDriverChunkTypeGlobal,
    EMemSpyDriverChunkTypeRamDrive
    };


enum TMemSpyDriverContainerType
    {
    EMemSpyDriverContainerTypeUnknown          = -1,
    EMemSpyDriverContainerTypeFirst            = EThread,
	EMemSpyDriverContainerTypeThread           = EMemSpyDriverContainerTypeFirst,
	EMemSpyDriverContainerTypeProcess          = EProcess,
	EMemSpyDriverContainerTypeChunk            = EChunk,
	EMemSpyDriverContainerTypeLibrary          = ELibrary,
	EMemSpyDriverContainerTypeSemaphore        = ESemaphore,
	EMemSpyDriverContainerTypeMutex            = EMutex,
	EMemSpyDriverContainerTypeTimer            = ETimer,
	EMemSpyDriverContainerTypeServer           = EServer,
	EMemSpyDriverContainerTypeSession          = ESession,
	EMemSpyDriverContainerTypeLogicalDevice    = ELogicalDevice,
	EMemSpyDriverContainerTypePhysicalDevice   = EPhysicalDevice,
	EMemSpyDriverContainerTypeLogicalChannel   = ELogicalChannel,
	EMemSpyDriverContainerTypeChangeNotifier   = EChangeNotifier,
	EMemSpyDriverContainerTypeUndertaker       = EUndertaker,
	EMemSpyDriverContainerTypeMsgQueue         = EMsgQueue,
	EMemSpyDriverContainerTypePropertyRef      = EPropertyRef,
	EMemSpyDriverContainerTypeCondVar          = ECondVar,
	EMemSpyDriverContainerTypeLast             = EMemSpyDriverContainerTypeCondVar
    };


enum TMemSpyDriverTimerType // Same as TTimer::TTimerType
    {
    EMemSpyDriverTimerTypeUnknown = 0,
	EMemSpyDriverTimerTypeRelative = 1,
    EMemSpyDriverTimerTypeAbsolute = 2,
    EMemSpyDriverTimerTypeLocked = 4,
    EMemSpyDriverTimerTypeHighRes = 8,
    EMemSpyDriverTimerTypeInactivity = 16
    };


enum TMemSpyDriverTimerState // Same as TTimer::TTimerState
    {
    EMemSpyDriverTimerStateUnknown = -1,
	EMemSpyDriverTimerStateIdle = 0,
    EMemSpyDriverTimerStateWaiting,
    EMemSpyDriverTimerStateWaitHighRes
    };

enum TMemSpyDriverEventType
    {
    EMemSpyDriverEventTypeThreadCreate = 0,
    EMemSpyDriverEventTypeThreadKill,
    EMemSpyDriverEventTypeProcessRemove,
    EMemSpyDriverEventTypeProcessCreate,
    EMemSpyDriverEventTypeChunkAdd,
    EMemSpyDriverEventTypeChunkDestroy,
    };

enum TMemSpyMemoryModelType
    {
    EMemSpyMemoryModelTypeUnknown = 0,
    EMemSpyMemoryModelTypeMoving,
    EMemSpyMemoryModelTypeMultiple,
    EMemSpyMemoryModelTypeEmulator
    };

enum TMemSpyChunkAttributes
	{
	EMemSpyChunkAttributesNormal            = 0x00,
	EMemSpyChunkAttributesDoubleEnded	    = 0x01,
	EMemSpyChunkAttributesDisconnected	    = 0x02,
	EMemSpyChunkAttributesConstructed   	= 0x04,
	EMemSpyChunkAttributesMemoryNotOwned	= 0x08
	};

#endif