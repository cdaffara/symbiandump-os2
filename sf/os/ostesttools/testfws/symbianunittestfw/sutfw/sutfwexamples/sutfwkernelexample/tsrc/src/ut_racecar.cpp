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

#include <kernel/kern_priv.h>
#include "ut_racecar.h"
#include "racecar.h"
#include <symbianunittestmacros.h>

_LIT( KDriveName, "ut_racecar.ldd");
_LIT( KRed, "red" );
_LIT( KBlue, "blue" );
const TInt KInitialFuel( 100 );


DECLARE_STANDARD_LDD()
    {
    //create factory here.
    //Notice: the driver name should set to ldd name
    return new DRaceCarTestFactory(KDriveName);
    }

TInt DRaceCarTestFactory::Create(DLogicalChannelBase*& aChannel)
    {
    aChannel=new UT_DRaceCar();
    return aChannel?KErrNone:KErrNoMemory;
    }

UT_DRaceCar::UT_DRaceCar()
    {
    Construct();//setup test cases
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_DRaceCar::Construct()
    {
    DEFINE_TEST_CLASS( UT_DRaceCar )

    ADD_SUT( UT_Fuel )
    ADD_SUT( UT_Steering )
    ADD_SUT( UT_Speed )
    // Setup and teardown functions can be changed for each test function
    // Usually this is not needed, but this is possible.
    ADD_SUT_WITH_SETUP_AND_TEARDOWN( Setup, UT_Color, Teardown )

    //ADD_SUT(UT_Crash )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_DRaceCar::~UT_DRaceCar()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt UT_DRaceCar::Setup()
    {
    iRaceCar = new DRaceCar( KRed, KInitialFuel );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt UT_DRaceCar::Teardown()
    {
    delete iRaceCar;
    iRaceCar = NULL;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt UT_DRaceCar::UT_Fuel()
    {
    Kern::Printf("test case UT_Fuel");
    
    // Test the initial fuel
    SUT_ASSERT_EQUALS( KInitialFuel, iRaceCar->FuelTankSize() )
    SUT_ASSERT_EQUALS( 0, iRaceCar->FuelLeft() )

    // Add a proper amount of fuel
    SUT_ASSERT_EQUALS( 0, iRaceCar->AddFuel( KInitialFuel ) )
    SUT_ASSERT_EQUALS( KInitialFuel, iRaceCar->FuelLeft() )

    // Add too much fuel
    SUT_ASSERT_EQUALS( 2, iRaceCar->AddFuel( 1 ) )  //expect to leave
    SUT_ASSERT_EQUALS( KInitialFuel, iRaceCar->FuelLeft() )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt UT_DRaceCar::UT_Steering()
    {
    Kern::Printf("test case UT_Steering");
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
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt UT_DRaceCar::UT_Speed()
    {
    Kern::Printf("test case UT_Speed");
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
    return KErrNone;
    }


TInt UT_DRaceCar::UT_Color()
    {
    Kern::Printf("test case UT_Color");
    // Check the initial color
    SUT_ASSERT_EQUALS( KRed, iRaceCar->Color() )

    // Try to paint the car, color allowed
    iRaceCar->ChangeColorL( KBlue );
    SUT_ASSERT_EQUALS( KBlue, iRaceCar->Color() )
    
//leave assertion is not support in kernel test
//    SUT_ASSERT_LEAVE( iRaceCar->ChangeColorL( KNullDesC ) )    
//    
//    SUT_ASSERT_LEAVE_WITH( 
//        iRaceCar->ChangeColorL( KNullDesC ), KErrArgument )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt UT_DRaceCar::UT_Crash()
    {
    //the car is totaled (:
    Kern::Printf("test case UT_Crash enter");
    
//    _LIT( KPanicString, "kkkkkkkkkkkk");
//    TBufC8<3> buf( KPanicString );
        
    Kern::PanicCurrentThread(_L("Crash"), -100);
    //Kern::Printf("test case UT_Crash exits");
    return KErrNone;
    }
