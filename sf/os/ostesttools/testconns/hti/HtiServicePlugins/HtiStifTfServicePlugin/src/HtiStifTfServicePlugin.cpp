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
* Description:  Implements the ECom plugin for HTI STIF Test Framework control
*                service.
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

#include "HtiStifTfServicePlugin.h"
#include "HtiStifTfIf.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiStifTfServicePlugin::CHtiStifTfServicePlugin
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHtiStifTfServicePlugin::CHtiStifTfServicePlugin()
    {
    }


// -----------------------------------------------------------------------------
// CHtiStifTfServicePlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHtiStifTfServicePlugin::ConstructL()
    {
    iStifTfInterface = CHtiStifTfIf::NewL( this );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfServicePlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHtiStifTfServicePlugin* CHtiStifTfServicePlugin::NewL()
    {
    CHtiStifTfServicePlugin* self = new (ELeave) CHtiStifTfServicePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Destructor
CHtiStifTfServicePlugin::~CHtiStifTfServicePlugin()
    {
    delete iStifTfInterface;
    }


// -----------------------------------------------------------------------------
// CHtiStifTfServicePlugin::ProcessMessageL
// Sends the message to be processed by the class implementing the STIF TF
// interface.
// -----------------------------------------------------------------------------
//
void CHtiStifTfServicePlugin::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority aPriority)
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfServicePlugin::ProcessMessageL" );
    iStifTfInterface->SetDispatcher( iDispatcher );
    iStifTfInterface->ProcessMessageL( aMessage, aPriority );
    HTI_LOG_FUNC_OUT( "CHtiStifTfServicePlugin::ProcessMessageL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfServicePlugin::NotifyMemoryChange
// Just forwards the call to the HtiStifTwIf instance.
// -----------------------------------------------------------------------------
//
void CHtiStifTfServicePlugin::NotifyMemoryChange( TInt aAvailableMemory )
    {
    iStifTfInterface->NotifyMemoryChange( aAvailableMemory );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfServicePlugin::IsBusy
// -----------------------------------------------------------------------------
//
TBool CHtiStifTfServicePlugin::IsBusy()
    {
    return iStifTfInterface->iIsBusy;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
