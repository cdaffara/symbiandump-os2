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
* Description: This file contains testmeasurementstub declaration.
*
*/

// INCLUDE FILES
#include "STIFTestMeasurementStub.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTIFTestMeasurementStub::CSTIFTestMeasurementStub
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTIFTestMeasurementStub::CSTIFTestMeasurementStub( 
            CSTIFTestMeasurement::TSTIFMeasurementType aMeasurementType ):
    iMeasurementType( aMeasurementType )
    {

    }

// -----------------------------------------------------------------------------
// CSTIFTestMeasurementStub::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTIFTestMeasurementStub::ConstructL( const TDesC& aConfigurationInfo )
    {

    }

// -----------------------------------------------------------------------------
// CSTIFTestMeasurementStub::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTIFTestMeasurementStub* CSTIFTestMeasurementStub::NewL(
                    const TDesC& aConfigurationInfo,
                    CSTIFTestMeasurement::TSTIFMeasurementType aMeasurementType )
    {
    CSTIFTestMeasurementStub* self = new (ELeave) CSTIFTestMeasurementStub( 
                                                            aMeasurementType );

    CleanupStack::PushL( self );
    self->ConstructL( aConfigurationInfo );
    CleanupStack::Pop();

    return self;

    }

// Destructor
// -----------------------------------------------------------------------------
// CSTIFTestMeasurementStub::~CSTIFTestMeasurementStub
// Destructor.
// -----------------------------------------------------------------------------
//
CSTIFTestMeasurementStub::~CSTIFTestMeasurementStub()
    {

    }

// -----------------------------------------------------------------------------
// CSTIFTestMeasurementStub::Start
// Starts test measurement
// Returns Symbian error code
// -----------------------------------------------------------------------------
//
TInt CSTIFTestMeasurementStub::Start( )
    {
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CSTIFTestMeasurementStub::Stop
// Stops test measurement
// Returns Symbian error code
// -----------------------------------------------------------------------------
//
TInt CSTIFTestMeasurementStub::Stop( )
    {
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CSTIFTestMeasurementStub::MeasurementType
// 
// Returns measurement module's type
// -----------------------------------------------------------------------------
//
CSTIFTestMeasurement::TSTIFMeasurementType CSTIFTestMeasurementStub::MeasurementType( )
    {
    return iMeasurementType;

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CSTIFTestMeasurementImplementation*: Pointer to STIF Test 
// Measurement Module
// object
// -----------------------------------------------------------------------------
//
EXPORT_C CSTIFTestMeasurementImplementation* LibEntryL( 
                const TDesC& aConfigurationInfo,
                CSTIFTestMeasurement::TSTIFMeasurementType aMeasurementType )
    {
    CSTIFTestMeasurementImplementation* measurement;
    measurement = CSTIFTestMeasurementStub::NewL( 
                                        aConfigurationInfo, aMeasurementType );
    return measurement;

    }

// End of File
