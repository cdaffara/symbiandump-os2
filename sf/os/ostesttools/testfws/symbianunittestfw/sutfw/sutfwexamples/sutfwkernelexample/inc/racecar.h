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
#ifndef DRACECAR_H
#define DRACECAR_H

// INCLUDES
#include <kernel/kernel.h>
#include "symbianunittestmacros.h"

// CONSTANTS
const TInt KMaxTyreAngle( 45 );

// CLASS DECLARATION
class DRaceCar : public DBase 
    {
    public: // Constructors and Destructor
        
        DRaceCar( const TDesC8& aColor, TUint aFuelTankSize );
        ~DRaceCar();
        
    public: // New functions
        
        TUint FuelTankSize() const;
        
        TUint FuelLeft() const;
        
        TUint AddFuel( TUint aFuel );

        TInt Steer( TInt aAngle );
        
        TInt Direction() const;
    
        TInt Accelerate( TUint aForce );
        
        TUint Speed() const;
        
        void Brake( TUint aForce );
    
        const TDesC8& Color() const;
        
        void ChangeColorL( const TDesC8& aNewColor );
    
    private: // Constructors
    
        void ConstructL( const TDesC8& aColor );
        DRaceCar( TUint aFuelTankSize );
    
    private: // Data
        
        TUint iFuelTankSize;
        HBuf* iColor;
        TUint iFuel;
        TInt iDirection;
        TUint iSpeed;

    private: // Test class
        
        SYMBIAN_UNIT_TEST_CLASS( UT_DRaceCar )
	};

#endif // DRACECAR_H
