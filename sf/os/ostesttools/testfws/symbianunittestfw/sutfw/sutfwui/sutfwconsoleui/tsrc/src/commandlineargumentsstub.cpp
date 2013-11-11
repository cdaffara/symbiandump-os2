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

#include "bacline.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCommandLineArguments* CCommandLineArguments::NewL()
    {
    CCommandLineArguments* self = 
        CCommandLineArguments::NewLC(); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCommandLineArguments* CCommandLineArguments::NewLC()
    {
    CCommandLineArguments* self = 
        new( ELeave )CCommandLineArguments();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCommandLineArguments::CCommandLineArguments()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCommandLineArguments::ConstructL()
    {
    iArgs = new( ELeave )CDesCArrayFlat( 1 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCommandLineArguments::~CCommandLineArguments()
    {
    delete iArgs;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TPtrC CCommandLineArguments::Arg( TInt aArg ) const
    {
    TPtrC arg;
    if ( aArg < iArgs->Count() )
        {
        arg.Set( iArgs->MdcaPoint( aArg ) );
        }
    return arg;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCommandLineArguments::Count() const
    {
    return iArgs->Count();
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
// 
CDesCArray& CCommandLineArguments::Args()
    {
    return *iArgs;
    }
