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

#include <memspy/engine/memspyenginehelperrom.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include "MemSpyEngineOutputListItem.h"
#include <memspy/engine/memspyengineoutputsink.h>


CMemSpyEngineHelperROM::CMemSpyEngineHelperROM( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperROM::~CMemSpyEngineHelperROM()
    {
    delete iPlatformVersion;
    delete iChecksum;
    }


void CMemSpyEngineHelperROM::ConstructL()
    {
    iPlatformVersion = HBufC::NewL( 128 );
    iChecksum = HBufC::NewL( 10 );
    //
    TPtr pChecksum( iChecksum->Des() );
    TPtr pPlatformVersion( iPlatformVersion->Des() );
    //
    MemSpyEngineUtils::GetRomInfoL( pPlatformVersion, pChecksum );
    }


CMemSpyEngineHelperROM* CMemSpyEngineHelperROM::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperROM* self = new(ELeave) CMemSpyEngineHelperROM( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C void CMemSpyEngineHelperROM::AddInfoL( CMemSpyEngineOutputList& aList )
    {
    _LIT( KRomChecksum, "ROM Checksum" );
    aList.AddItemL( KRomChecksum, *iChecksum );

    _LIT( KRomVersion, "ROM Version" );
    aList.AddItemL( KRomVersion, *iPlatformVersion );
    }
