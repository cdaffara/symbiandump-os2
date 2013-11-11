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
#include <symbianunittest.h>

// FORWARD DECLARATIONS
class CRaceCar;

// CLASS DECLARATION
class UT_CRaceCar : public CSymbianUnitTest
    {
    public: // Constructors and destructor
    
        static UT_CRaceCar* NewL();
        static UT_CRaceCar* NewLC();
        ~UT_CRaceCar();
   
    protected: // From CSymbianUnitTest
        
        void SetupL();
        void Teardown();
        
    protected: // Test functions
    
        void UT_FuelL();
        void UT_SteeringL();
        void UT_SpeedL();
        void UT_ColorL();
        void UT_TimeoutL();
	void UT_CrashL();
	void UT_QuitL();
    
    private: // Contructors
        
        UT_CRaceCar();
        void ConstructL();  

    private: // Data
    
        // The object to be tested as a member variable:
        CRaceCar *iRaceCar;
    };

#endif // UT_RACECAR_H
