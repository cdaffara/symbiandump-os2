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
#ifndef CRACECAR_H
#define CRACECAR_H

// INCLUDES
#include <e32base.h>
#include <symbianunittestmacros.h>

// CONSTANTS
const TInt KMaxTyreAngle( 45 );

// CLASS DECLARATION
class CRaceCar : public CBase 
    {
    public: // Constructors and Destructor
        
        static CRaceCar* NewL( const TDesC& aColor, TUint aFuelTankSize );
        static CRaceCar* NewLC( const TDesC& aColor, TUint aFuelTankSize );
        ~CRaceCar();
        
    public: // New functions
        
        TUint FuelTankSize() const;
        
        TUint FuelLeft() const;
        
        TUint AddFuel( TUint aFuel );

        TInt Steer( TInt aAngle );
        
        TInt Direction() const;
    
        TInt Accelerate( TUint aForce );
        
        TUint Speed() const;
        
        void Brake( TUint aForce );
    
        const TDesC& Color() const;
        
        void ChangeColorL( const TDesC& aNewColor );
    
    private: // Constructors
    
        void ConstructL( const TDesC& aColor );
        CRaceCar( TUint aFuelTankSize );
    
    private: // Data
        
        TUint iFuelTankSize;
        HBufC* iColor;
        TUint iFuel;
        TInt iDirection;
        TUint iSpeed;

    private: // Test class
        
        SYMBIAN_UNIT_TEST_CLASS( UT_CRaceCar )
	};

#endif // CRACECAR_H
