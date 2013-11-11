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

#include "racecar.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
DRaceCar::DRaceCar( const TDesC8& aColor, TUint aFuelTankSize )
 : iFuelTankSize( aFuelTankSize )
    {
    iColor = HBuf8::New(aColor);
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
DRaceCar::~DRaceCar()
    {
    delete iColor;
    } 
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TUint DRaceCar::FuelTankSize() const
    {
    return iFuelTankSize;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//    
TUint DRaceCar::FuelLeft() const
    {
    return iFuel;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TUint DRaceCar::AddFuel( TUint aFuel )
    {
    TUint extraFuel = 0;
    if ( aFuel > iFuelTankSize - iFuel )
        {
        extraFuel = aFuel- ( iFuelTankSize - iFuel );
        iFuel = iFuelTankSize;
        }
    else
        {
        iFuel += aFuel;
        }
    return extraFuel;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//    
TInt DRaceCar::Steer( TInt aAngle )
    {
    if ( ( iDirection + aAngle ) < -KMaxTyreAngle || 
         ( iDirection + aAngle ) > KMaxTyreAngle )
        {
        return KErrArgument;
        }
    iDirection += aAngle;
    return KErrNone;
    }    
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt DRaceCar::Direction() const
    {
    return iDirection;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt DRaceCar::Accelerate( TUint aForce )
    {
    if ( iFuel == 0 )
        {
        return KErrGeneral;
        }
    else
        {
        iFuel--;
        iSpeed += aForce;
        }
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TUint DRaceCar::Speed() const
    {
    return iSpeed;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void DRaceCar::Brake( TUint aForce )
    {
    if ( iSpeed < aForce )
        {
        iSpeed = 0;
        }
    else
        {
        iSpeed -= aForce;
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& DRaceCar::Color() const
    {
    return *iColor;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void DRaceCar::ChangeColorL( const TDesC8& aNewColor )
    {
    HBuf* tmpColor = HBuf8::New(aNewColor);
    delete iColor;
    iColor = tmpColor;
    }
