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

#include <memspy/engine/memspyenginehelpercondvar.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>



CMemSpyEngineHelperCondVar::CMemSpyEngineHelperCondVar( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperCondVar::~CMemSpyEngineHelperCondVar()
    {
    }


void CMemSpyEngineHelperCondVar::ConstructL()
    {
    }


CMemSpyEngineHelperCondVar* CMemSpyEngineHelperCondVar::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperCondVar* self = new(ELeave) CMemSpyEngineHelperCondVar( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C void CMemSpyEngineHelperCondVar::GetCondVarSuspendedThreadsL( const TMemSpyDriverHandleInfoGeneric& aCondVarDetails, RArray<TMemSpyDriverCondVarSuspendedThreadInfo>& aThreads )
    {   
    const TInt KMaxCount = 256;
    TAny* handles[KMaxCount];
    TInt c = KMaxCount;

    TInt r = iEngine.Driver().GetCondVarSuspendedThreads( aCondVarDetails.iHandle, handles, c );
    if  ( r == KErrNone )
        {
        if  ( c > 0 )
            {
            if (c > KMaxCount)
                {
                c = KMaxCount;
                }

            TMemSpyDriverCondVarSuspendedThreadInfo info;
            for (TInt i=0; i<c; i++)
                {
                r = iEngine.Driver().GetCondVarSuspendedThreadInfo( handles[i], info );
                if (r == KErrNone)
                    {
                    aThreads.AppendL( info );
                    }
                }
            }
        }
    }
