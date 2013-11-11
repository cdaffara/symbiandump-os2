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

#include <memspy/engine/memspyenginehelpersysmemtrackercyclechange.h>

// System includes
#include <e32debug.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginemidwife.h>
#include <memspy/engine/memspyengineundertaker.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>



CMemSpyEngineHelperSysMemTrackerCycleChange::CMemSpyEngineHelperSysMemTrackerCycleChange( TUint8 aAttribs )
:   iAttributes( aAttribs )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChange::~CMemSpyEngineHelperSysMemTrackerCycleChange()
    {
    }


void CMemSpyEngineHelperSysMemTrackerCycleChange::BaseConstructL()
    {
    }


void CMemSpyEngineHelperSysMemTrackerCycleChange::FormatType( TMemSpySWMTTypeName& aBuffer ) const
    {
    _LIT( KTypeUnknown,                 "{????}" );
    _LIT( KTypeHeap,                    "{HEAP}" );
    _LIT( KTypeChunk,                   "{CHNK}" );
    _LIT( KTypeCode,                    "{CODE}" );
    _LIT( KTypeStack,                   "{STAK}" );
    _LIT( KTypeGlobalData,              "{GLOD}" );
    _LIT( KTypeRamDrive,                "{RAMD}" );
    _LIT( KTypeOpenFile,                "{FILE}" );
    _LIT( KTypeDiskSpace,               "{DISK}" );
    _LIT( KTypeBitmap,                  "{BITM}" );
    _LIT( KTypeHandleGeneric,           "{HGEN}" );
    _LIT( KTypeHandlePAndS,             "{HPAS}" );
    _LIT( KTypeFbserv,                  "{FABS}" );
    _LIT( KTypeFileServerCache,         "{F32C}" );
    _LIT( KTypeSystemMemory,            "{SYSM}" );
    _LIT( KTypeWindowGroup,             "{WNDG}" );
    //
    switch( Type() )
        {
        case EMemSpyEngineSysMemTrackerTypeHeapUser:
        case EMemSpyEngineSysMemTrackerTypeHeapKernel:
            aBuffer.Append( KTypeHeap );
            break;
        case EMemSpyEngineSysMemTrackerTypeChunk:
            aBuffer.Append( KTypeChunk );
            break;
        case EMemSpyEngineSysMemTrackerTypeCode:
            aBuffer.Append( KTypeCode );
            break;
        case EMemSpyEngineSysMemTrackerTypeHandleGeneric:
            aBuffer.Append( KTypeHandleGeneric );
            break;
        case EMemSpyEngineSysMemTrackerTypeHandlePAndS:
            aBuffer.Append( KTypeHandlePAndS );
            break;
        case EMemSpyEngineSysMemTrackerTypeStack:
            aBuffer.Append( KTypeStack );
            break;
        case EMemSpyEngineSysMemTrackerTypeGlobalData:
            aBuffer.Append( KTypeGlobalData );
            break;
        case EMemSpyEngineSysMemTrackerTypeRamDrive:
            aBuffer.Append( KTypeRamDrive );
            break;
        case EMemSpyEngineSysMemTrackerTypeOpenFile:
            aBuffer.Append( KTypeOpenFile );
            break;
        case EMemSpyEngineSysMemTrackerTypeDiskSpace:
            aBuffer.Append( KTypeDiskSpace );
            break;
        case EMemSpyEngineSysMemTrackerTypeBitmap:
            aBuffer.Append( KTypeBitmap );
            break;
        case EMemSpyEngineSysMemTrackerTypeFbserv:
            aBuffer.Append( KTypeFbserv );
            break;
        case EMemSpyEngineSysMemTrackerTypeFileServerCache:
            aBuffer.Append( KTypeFileServerCache );
            break;
        case EMemSpyEngineSysMemTrackerTypeSystemMemory:
            aBuffer.Append( KTypeSystemMemory );
            break;
        case EMemSpyEngineSysMemTrackerTypeWindowServer:
            aBuffer.Append( KTypeWindowGroup );
            break;            
        default:
            aBuffer.Append( KTypeUnknown );
            break;
        }
    }


void CMemSpyEngineHelperSysMemTrackerCycleChange::FormatAttributes( TDes& aBuffer ) const
    {
    _LIT( KAttribNew,   "[N]" );
    _LIT( KAttribDead,  "[D]" );
    _LIT( KAttribAlive, "[A]" );
    _LIT( KAttribPlus,  "+" );
    //
    aBuffer.Zero();
    // 
    if  ( iAttributes & EMemSpyEngineSysMemTrackerEntryAttributeIsNew )
        {
        aBuffer.Append( KAttribNew );
        }
    //
    const TBool isDead = !( iAttributes & EMemSpyEngineSysMemTrackerEntryAttributeIsAlive );
    if  ( isDead )
        {
        if  ( aBuffer.Length() )
            {
            aBuffer.Append( KAttribPlus );
            }

        aBuffer.Append( KAttribDead );
        }
    else
        {
        if  ( aBuffer.Length() )
            {
            aBuffer.Append( KAttribPlus );
            }

        aBuffer.Append( KAttribAlive );
        }
    }









