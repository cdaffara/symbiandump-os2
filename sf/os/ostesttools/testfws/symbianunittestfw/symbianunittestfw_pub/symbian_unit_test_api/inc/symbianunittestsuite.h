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

#ifndef SYMBIANUNITTESTSUITE_H
#define SYMBIANUNITTESTSUITE_H

// INCLUDES
#include <symbianunittestinterface.h>
#include <e32base.h>

// CLASS DECLARATION
/**
* A class to collect a number of unit tests to be executed 
* during a particular test run.
*
* @lib symbianunittestfw.lib
*/
class CSymbianUnitTestSuite : public CBase, public MSymbianUnitTestInterface
    {
    public: // Constructors and destructor

        /**
        * Creates an empty unit test suite.
        * @param aName the name of the unit test suite
        * @return a new instance of CSymbianUnitTestSuite, 
        *         the ownership is transferred
        */        
        IMPORT_C static CSymbianUnitTestSuite* NewL( const TDesC& aName );
        
        /**
        * Creates an empty unit test suite and pushes it to the CleanupStack.
        * @param aName the name of the unit test suite
        * @return a new instance of CSymbianUnitTestSuite, 
        *         the ownership is transferred
        */        
        IMPORT_C static CSymbianUnitTestSuite* NewLC( const TDesC& aName );
        
        /**
        * Destructor
        */        
        IMPORT_C ~CSymbianUnitTestSuite();
    
    public: // From MSymbianUnitTestInterface

        /**
        * Function called by the framework to execute 
        * the unit tests contained in this test suite.
        * There is no need to call this function directly from the unit tests.
        * 
        * @param aObserver an observer for the test progress 
        * @param aResult the test result
        * @param aFailureSimulation the type of failure simulation to be used
	* @param aTestCaseNames the test cases to run
	* @param aTimeout the time out value for test execution
        */        
        IMPORT_C void ExecuteL( 
            MSymbianUnitTestObserver& aObserver,
            CSymbianUnitTestResult& aResult,
            MSymbianUnitTestInterface::TFailureSimulation aFailureSimulation,
	    const CDesCArray& aTestCaseNames,
	    TInt aTimeout );

        /**
        * Returns the number of unit tests contained in this unit test suite
        * @return The number of unit tests in this unit test suite
        */        
        IMPORT_C TInt TestCaseCount();
 
        /**
        * @return The name of this unit test suite
        */        
        IMPORT_C const TDesC& Name() const;
        
        /**
        * @return the array of test case names
        */
        IMPORT_C void TestCaseNamesL(CDesCArray& aTestCaseNames);
  
    public: // New functions

        /**
        * Adds a new unit test to this unit test suite.
        * @param aTest the test to be added, the ownership is transferred. 
        */        
        IMPORT_C void AddL( MSymbianUnitTestInterface* aTest );        
        
    protected: // Constructors
    
        IMPORT_C CSymbianUnitTestSuite();
        IMPORT_C void ConstructL( const TDesC& aName );
    
    private: // Data
        
        RPointerArray< MSymbianUnitTestInterface > iTests;
        HBufC* iName;
    };

#endif // SYMBIANUNITTESTSUITE_H
