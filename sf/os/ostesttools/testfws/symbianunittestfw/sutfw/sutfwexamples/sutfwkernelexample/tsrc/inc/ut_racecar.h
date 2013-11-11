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

#ifndef UT_RACECAR_H
#define UT_RACECAR_H

// INCLUDES
#include <kernel/kernel.h>
#include <symbianunittestldd.h>

// FORWARD DECLARATIONS
class DRaceCar;


class DRaceCarTestFactory : public DSymbianUnitTestFactory
    {
    public:
        DRaceCarTestFactory(const TDesC& aName) :
            DSymbianUnitTestFactory(aName){};
        TInt Create(DLogicalChannelBase*& aChannel);
    };

// CLASS DECLARATION
class UT_DRaceCar : public DSymbianUnitTest
    {
    public: // Constructors and destructor
    
        UT_DRaceCar();
        ~UT_DRaceCar();
        
    protected: // From DSymbianUnitTest
        
        TInt Setup();
        TInt Teardown();
        void Construct();
        
    protected: // Test functions
    
        TInt UT_Fuel();
        TInt UT_Steering();
        TInt UT_Speed();
        TInt UT_Color();
        TInt UT_Crash();
        

    private: // Data
    
        // The object to be tested as a member variable:
        DRaceCar *iRaceCar;
    };

#endif // UT_RACECAR_H
