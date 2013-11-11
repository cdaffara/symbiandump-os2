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
CRaceCar* CRaceCar::NewL( const TDesC& aColor, TUint aFuelTankSize )
    {
    CRaceCar* self = NewLC( aColor, aFuelTankSize );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRaceCar* CRaceCar::NewLC( const TDesC& aColor, TUint aFuelTankSize )
    {
    CRaceCar* self = new( ELeave )CRaceCar( aFuelTankSize );
    CleanupStack::PushL( self );
    self->ConstructL( aColor );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRaceCar::CRaceCar( TUint aFuelTankSize )
 : iFuelTankSize( aFuelTankSize )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRaceCar::ConstructL( const TDesC& aColor )
    {
    iColor = aColor.AllocL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRaceCar::~CRaceCar()
    {
    delete iColor;
    } 
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TUint CRaceCar::FuelTankSize() const
    {
    return iFuelTankSize;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//    
TUint CRaceCar::FuelLeft() const
    {
    return iFuel;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TUint CRaceCar::AddFuel( TUint aFuel )
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
TInt CRaceCar::Steer( TInt aAngle )
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
TInt CRaceCar::Direction() const
    {
    return iDirection;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CRaceCar::Accelerate( TUint aForce )
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
TUint CRaceCar::Speed() const
    {
    return iSpeed;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRaceCar::Brake( TUint aForce )
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
const TDesC& CRaceCar::Color() const
    {
    return *iColor;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRaceCar::ChangeColorL( const TDesC& aNewColor )
    {
    __ASSERT_ALWAYS ( aNewColor.Length() > 0, User::Leave( KErrArgument ) );
    
    HBufC* tmpColor = aNewColor.AllocL();
    delete iColor;
    iColor = tmpColor;
    }
