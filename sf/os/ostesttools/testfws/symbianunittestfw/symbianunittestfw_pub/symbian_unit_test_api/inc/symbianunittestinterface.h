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

#ifndef SYMBIANUNITTESTINTERFACE_H
#define SYMBIANUNITTESTINTERFACE_H

// INCLUDES
#include <badesca.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MSymbianUnitTestObserver;
class CSymbianUnitTestResult;

// CONSTANTS
/**
* All the DLLs containing Symbian unit tests should use the following UID
*/
const TUid KSymbianUnitTestDllUid = { 0x20022E76 };

// CLASS DECLARATION
/**
* The interface that is implemented by all 
* the unit tests and collections of unit tests.
* Provides a common interface 
* for Symbian unit test framework to execute the unit tests.
*
* @lib symbianunittestfw.lib
*/
class MSymbianUnitTestInterface
    {
    public: // Enumerations
    
        /** Test failure simulation types */
        enum TFailureSimulation 
            {
            /** No failure simulation */
            ENoFailureSimulation,
            /** Memory allocation failure simulation */
            EMemAllocFailureSimulation
            };     
    
    public: // New functions
    
        /**
        * Virtual destructor to enable the framework to delete
        * the unit test or the unit test collection through this interface
        */       
        virtual ~MSymbianUnitTestInterface() { }       

        /**
        * Function called by the framework to execute 
        * the unit tests contained in this object.
        * 
        * @param aObserver an observer for the test progress 
        * @param aResult the test result
        * @param aFailureSimulation the type of failure simulation to be used
        * @param aTestCaseNames the test cases to run
	    * @param aTimeout the time out value for test execution
        */        
        virtual void ExecuteL( 
            MSymbianUnitTestObserver& aObserver,
            CSymbianUnitTestResult& aResult,
            TFailureSimulation aFailureSimulation,
	    const CDesCArray& aTestCaseNames,
	    TInt aTimeout) = 0;

        /**
        * Returns the number of unit tests contained in this object
        * @return The number of unit tests in this object
        */        
        virtual TInt TestCaseCount() = 0;

        /**
        * @return The name of this unit test or unit test collection
        */        
        virtual const TDesC& Name() const = 0;
        
        /**
         * @return the array of test case names
         */
        virtual void TestCaseNamesL(CDesCArray& aTestCaseNames) = 0;
    };

#endif // SYMBIANUNITTESTINTERFACE_H
