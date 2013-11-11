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
* Description:  Definitions for the class DAnalyzeToolDevice.
*
*/


// INCLUDE FILES
#include "analyzetooldevice.h"
#include "analyzetoolchannel.h"
#include "atlog.h"

// ================= MEMBER FUNCTIONS =========================================

// -----------------------------------------------------------------------------
// DAnalyzeToolDevice::DAnalyzeToolDevice()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
DAnalyzeToolDevice::DAnalyzeToolDevice()
    {
    LOGSTR1( "ATDD DAnalyzeToolDevice::DAnalyzeToolDevice()" );
    // Set version number
    iVersion = KAnalyzeToolLddVersion();
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolDevice::Install()
// Second stage constructor.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolDevice::Install()
    {
    LOGSTR1( "ATDD DAnalyzeToolDevice::Install()" );
    // Set device name
    return SetName( &KAnalyzeToolLddName );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolDevice::GetCaps()
// Gets the driver's capabilities.
// -----------------------------------------------------------------------------
//
void DAnalyzeToolDevice::GetCaps( TDes8& /*aDes*/ ) const
    {
    LOGSTR1( "ATDD DAnalyzeToolDevice::GetCaps()" );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolDevice::Create()
// Creates the logical channel.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolDevice::Create( DLogicalChannelBase*& aChannel )
    {
    LOGSTR1( "ATDD DAnalyzeToolDevice::Create()" );

    // create new channel
    aChannel = new DAnalyzeToolChannel;

    // check that everything is OK
    return ( aChannel != NULL ) ? KErrNone : KErrNoMemory;
    }

// -----------------------------------------------------------------------------
// DECLARE_STANDARD_LDD
// Defines the entry point for a standard logical device driver (LDD),
// and declares the ordinal 1 export function for creating 
// the LDD factory object
// -----------------------------------------------------------------------------
//
DECLARE_STANDARD_LDD()
    {
    LOGSTR1( "ATDD DECLARE_STANDARD_LDD()" );
    return new DAnalyzeToolDevice;
    }
    
// ================= OTHER EXPORTED FUNCTIONS =================================

// None

// End of File
