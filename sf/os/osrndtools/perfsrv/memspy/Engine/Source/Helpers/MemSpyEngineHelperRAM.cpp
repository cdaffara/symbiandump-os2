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

#include <memspy/engine/memspyenginehelperram.h>

// System includes
#include <hal.h>
#include <e32debug.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include "MemSpyEngineOutputListItem.h"
#include <memspy/engine/memspyengineoutputsink.h>

// Type defs
typedef TInt(*TAknIconCacheConfigFunction)(TBool);

// Constants
const TInt KAknIconCacheConfigOrdinal = 44;

// Literal constants
_LIT( KMemSpyAknIconDllName, "AKNICON.DLL" );


CMemSpyEngineHelperRAM::CMemSpyEngineHelperRAM( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperRAM::~CMemSpyEngineHelperRAM()
    {
    }


void CMemSpyEngineHelperRAM::ConstructL()
    {
    CheckIfAknIconCacheCanBeConfiguredL();
    }


CMemSpyEngineHelperRAM* CMemSpyEngineHelperRAM::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperRAM* self = new(ELeave) CMemSpyEngineHelperRAM( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C TBool CMemSpyEngineHelperRAM::IsAknIconCacheConfigurable() const
    {
    return iIsAknIconCacheConfigurable;
    }


EXPORT_C TInt64 CMemSpyEngineHelperRAM::SetAknIconCacheStatusL( TBool aEnabled )
    {
    TInt64 ret( 0 );
    //
    if  ( iIsAknIconCacheConfigurable )
        {
        TInt freeRamBeforeOp = KErrNotFound;
        User::LeaveIfError( HAL::Get( HALData::EMemoryRAMFree, freeRamBeforeOp ) );
        //
        const TInt error = SetAknIconStatus( aEnabled );
        User::LeaveIfError( error );
        //
        if  ( aEnabled == EFalse )
            {
            // Turning cache off, check how much ram we saved...
            TInt freeRamAfterOp = KErrNotFound;
            User::LeaveIfError( HAL::Get( HALData::EMemoryRAMFree, freeRamAfterOp ) );

            // Work out how much ram was saved... 
            ret = ( freeRamAfterOp - freeRamBeforeOp );
            }
        }
    //
    return ret;
    }


void CMemSpyEngineHelperRAM::CheckIfAknIconCacheCanBeConfiguredL()
    {
#ifndef __WINS__
    RLibrary lib;
    const TInt loadErr = lib.Load( KMemSpyAknIconDllName );
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineHelperRAM::SetAknIconStatus() - dll load: %d", loadErr );
#endif
    if  ( loadErr == KErrNone )
        {
        TLibraryFunction ordinal = lib.Lookup( KAknIconCacheConfigOrdinal );
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyEngineHelperRAM::SetAknIconStatus() - ordinal: 0x%08x", ordinal );
#endif
        //
        iIsAknIconCacheConfigurable = ( ordinal != NULL );
        lib.Close();
        }
#endif
    }


TInt CMemSpyEngineHelperRAM::SetAknIconStatus( TBool aEnabled )
    {
    TInt ret = KErrNotSupported;
    //
#ifndef __WINS__
    RLibrary lib;
    const TInt loadErr = lib.Load( KMemSpyAknIconDllName );
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineHelperRAM::SetAknIconStatus() - dll load: %d", loadErr );
#endif
    if  ( loadErr == KErrNone )
        {
        TLibraryFunction ordinal = lib.Lookup( KAknIconCacheConfigOrdinal );
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyEngineHelperRAM::SetAknIconStatus() - ordinal: 0x%08x", ordinal );
#endif
        //
        if  ( ordinal )
            {
            TAknIconCacheConfigFunction func = (TAknIconCacheConfigFunction) ordinal;
            ret = (*func)( aEnabled );
#ifdef _DEBUG
            RDebug::Printf( "CMemSpyEngineHelperRAM::SetAknIconStatus() - ret: %d", ret );
#endif
            }
        //
        lib.Close();
        }
#else
	aEnabled = EFalse;
#endif
    //
    return ret;
    }
