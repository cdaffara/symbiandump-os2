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

#include "ut_racecar.h"
#include "racecar.h"
#include <symbianunittestmacros.h>

_LIT( KRed, "red" );
_LIT( KBlue, "blue" );
const TInt KInitialFuel( 100 );


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CRaceCar* UT_CRaceCar::NewL()
    {
    UT_CRaceCar* self = UT_CRaceCar::NewLC(); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CRaceCar* UT_CRaceCar::NewLC()
    {
    UT_CRaceCar* self = new( ELeave )UT_CRaceCar();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CRaceCar::UT_CRaceCar()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CRaceCar::ConstructL()
    {
    BASE_CONSTRUCT
    
    DEFINE_TEST_CLASS( UT_CRaceCar )
    
    ADD_SUT( UT_FuelL )
    ADD_SUT( UT_SteeringL )
    ADD_SUT( UT_SpeedL ) 
    // Setup and teardown functions can be changed for each test function
    // Usually this is not needed, but this is possible.
    ADD_SUT_WITH_SETUP_AND_TEARDOWN( SetupL, UT_ColorL, Teardown )

    ADD_SUT( UT_TimeoutL )
    ADD_SUT(UT_CrashL ) 
    ADD_SUT(UT_QuitL ) 

    
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CRaceCar::~UT_CRaceCar()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CRaceCar::SetupL()
    {
    iRaceCar = CRaceCar::NewL( KRed, KInitialFuel );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CRaceCar::Teardown()
    {
    delete iRaceCar;
    iRaceCar = NULL; 
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CRaceCar::UT_FuelL()
    {
    // Test the initial fuel
    SUT_ASSERT_EQUALS( KInitialFuel, iRaceCar->FuelTankSize() )
    SUT_ASSERT_EQUALS( 0, iRaceCar->FuelLeft() )

    // Add a proper amount of fuel
    SUT_ASSERT_EQUALS( 0, iRaceCar->AddFuel( KInitialFuel ) )
    SUT_ASSERT_EQUALS( KInitialFuel, iRaceCar->FuelLeft() )

    // Add too much fuel
    SUT_ASSERT_EQUALS( 2, iRaceCar->AddFuel( 1 ) )  //expect to leave
    SUT_ASSERT_EQUALS( KInitialFuel, iRaceCar->FuelLeft() )
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CRaceCar::UT_SteeringL()
    {
    // Try to steer beyond the allowed values
    SUT_ASSERT( iRaceCar->Steer( -( KMaxTyreAngle + 1 ) ) != KErrNone )
    SUT_ASSERT_EQUALS( 0, iRaceCar->Direction() )
    SUT_ASSERT( iRaceCar->Steer( KMaxTyreAngle + 1 ) != KErrNone )
    SUT_ASSERT_EQUALS( 0, iRaceCar->Direction() )

    // Steer with the an allowed value 
    SUT_ASSERT_EQUALS( KErrNone, iRaceCar->Steer( KMaxTyreAngle ) )
    SUT_ASSERT_EQUALS( KMaxTyreAngle, iRaceCar->Direction() )
    
    // Try beyond the allowed value
    SUT_ASSERT( iRaceCar->Steer( 1 ) != KErrNone )
    SUT_ASSERT_EQUALS( KMaxTyreAngle, iRaceCar->Direction() )

    // Steer to the opposite maximum
    SUT_ASSERT_EQUALS( KErrNone, iRaceCar->Steer( -2 * KMaxTyreAngle ) )
    SUT_ASSERT_EQUALS( -KMaxTyreAngle, iRaceCar->Direction() )

    // Try beyond the allowed value
    SUT_ASSERT( iRaceCar->Steer( -1 ) != KErrNone )
    SUT_ASSERT_EQUALS( -KMaxTyreAngle, iRaceCar->Direction() )
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CRaceCar::UT_SpeedL()
    {
    // Try to accelerate without fuel
    SUT_ASSERT_EQUALS( 0, iRaceCar->Speed() )
    SUT_ASSERT( iRaceCar->Accelerate( 1 ) != KErrNone )
    
    // Add fuel and accelerate
    iRaceCar->AddFuel( 2 );
    SUT_ASSERT_EQUALS( 2, iRaceCar->FuelLeft() )
    SUT_ASSERT_EQUALS( KErrNone, iRaceCar->Accelerate( 100 ) )
    SUT_ASSERT_EQUALS( 100, iRaceCar->Speed() )
    SUT_ASSERT_EQUALS( 1, iRaceCar->FuelLeft() )
    SUT_ASSERT_EQUALS( KErrNone, iRaceCar->Accelerate( 50 ) )
    SUT_ASSERT_EQUALS( 150, iRaceCar->Speed() )
    SUT_ASSERT_EQUALS( 0, iRaceCar->FuelLeft() )

    // Try braking
    iRaceCar->Brake( 70 );
    SUT_ASSERT_EQUALS( 80, iRaceCar->Speed() )
    iRaceCar->Brake( 90 );
    SUT_ASSERT_EQUALS( 0, iRaceCar->Speed() )    
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CRaceCar::UT_ColorL()
    {
    // Check the initial color
    SUT_ASSERT_EQUALS( KRed, iRaceCar->Color() )

    // Try to paint the car, color allowed
    iRaceCar->ChangeColorL( KBlue );
    SUT_ASSERT_EQUALS( KBlue, iRaceCar->Color() )
    
    // Try to paint the car, color not allowed
    SUT_ASSERT_LEAVE( iRaceCar->ChangeColorL( KNullDesC ) )    
    
    SUT_ASSERT_LEAVE_WITH( 
        iRaceCar->ChangeColorL( KNullDesC ), KErrArgument )
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CRaceCar::UT_TimeoutL()
    {
    //the car is too slow (:
    User::After(7 * 1000000);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void UT_CRaceCar::UT_CrashL()
    {
    //the car is totaled (:
    User::Panic(_L("Crash"), -100);
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void UT_CRaceCar::UT_QuitL()
    {
    User::Leave(-100);
    }
