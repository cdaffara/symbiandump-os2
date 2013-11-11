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

#include <memspy/engine/memspyenginehelperthread.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>



CMemSpyEngineHelperThread::CMemSpyEngineHelperThread( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperThread::~CMemSpyEngineHelperThread()
    {
    }


void CMemSpyEngineHelperThread::ConstructL()
    {
    }


CMemSpyEngineHelperThread* CMemSpyEngineHelperThread::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperThread* self = new(ELeave) CMemSpyEngineHelperThread( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C void CMemSpyEngineHelperThread::OutputThreadInfoL( const CMemSpyThread& aThread, TBool aDetailed )
    {
    CMemSpyThreadInfoContainer* container = CMemSpyThreadInfoContainer::NewLC( const_cast< CMemSpyThread& >( aThread ), EMemSpyThreadInfoItemTypeGeneralInfo );

    if  ( aDetailed )
        {
        container->AddItemL( EMemSpyThreadInfoItemTypeStack );
        }

    container->PrintL();
    CleanupStack::PopAndDestroy( container );
    }
