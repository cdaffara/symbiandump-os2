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
* Description:  Inline methods for CAwsEngineBase class.
*
*/

/*
* %version: 5 %
*/

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CAwsEngineBase* CAwsEngineBase::NewL(
    TInt aUid,
    TAwsEngineConstructionParameters* aConstructionParameters )
    {
    if( !aConstructionParameters )
        {
        User::Leave( KErrArgument );
        }
    
    const TUid KTMPUid = { aUid };

    TAny* interface = REComSession::CreateImplementationL(
        KTMPUid,
        _FOFF( CAwsEngineBase, iInstanceIdentifier ),
        aConstructionParameters );

    return reinterpret_cast<CAwsEngineBase*>( interface );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CAwsEngineBase::~CAwsEngineBase()
    {
    REComSession::DestroyedImplementation( iInstanceIdentifier );
    }

// ---------------------------------------------------------
// CAwsEngineBase::ListImplementationsL
// ---------------------------------------------------------
inline void CAwsEngineBase::ListImplementationsL(
        RImplInfoPtrArray& aImplInfoArray)
    {
    const TUid KInterfaceUid = { KAwsClientInterfaceUid };
    
    REComSession::ListImplementationsL(KInterfaceUid,
            aImplInfoArray);
    }

