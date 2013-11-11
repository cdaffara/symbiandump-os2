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
* Description: This file contains kernel testclass implementation.
*
*/

// INCLUDE FILES
#include "TemplateKernelScriptXXX.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ========================== OTHER EXPORTED FUNCTIONS =========================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TInt DTSKernelTestDriver::Create(DLogicalChannelBase*& aChannel)
// Creates the Logical Channel in EKA2
// -----------------------------------------------------------------------------
//
TInt DTemplateKernelScriptXXXDriver::Create(DLogicalChannelBase*& aChannel)
    {
    aChannel = new DTemplateKernelScriptXXX( this );
    return aChannel?KErrNone:KErrNoMemory;

    }

// -----------------------------------------------------------------------------
// TInt DTSKernelTest::DoCreate
// Second phase constructor for DLogicalChannelBase object in EKA2
// -----------------------------------------------------------------------------
//
TInt DTemplateKernelScriptXXX::DoCreate( TInt /* aUnit */, 
                                         const TDesC8* /* anInfo */, 
                                         const TVersion& /* aVer */ )
    {   
    SetDfcQ(Kern::DfcQue0());
    iMsgQ.Receive();

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// DECLARE_STANDARD_LDD()
// EKA2 entry point
// -----------------------------------------------------------------------------
//
DECLARE_STANDARD_LDD()
    {
    return new DTemplateKernelScriptXXXDriver( KDriverName );

    }

// End of File
