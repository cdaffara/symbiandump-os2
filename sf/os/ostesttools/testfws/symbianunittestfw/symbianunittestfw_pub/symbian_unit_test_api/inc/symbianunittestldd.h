// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// 
//

#ifndef __D_SYMBIANUNITTESTLDD_H
#define __D_SYMBIANUNITTESTLDD_H


#include <kernel/kernel.h>
#include "symbianunittestlddif.h"


// CLASS DECLARATION


class DSymbianUnitTestFactory : public DLogicalDevice
{
public:
    IMPORT_C DSymbianUnitTestFactory(const TDesC& aName);
    IMPORT_C virtual ~DSymbianUnitTestFactory();
	IMPORT_C virtual TInt Install();
	IMPORT_C virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel) = 0;

private:
    TName iDriverName;
    
};





/**
* The base class for kernel unit test classes.
* It implements logical driver to run test code in kernel mode
* The classes inheriting from this class should implements Construct
* for adding test case 
* @lib symbianunittestldd.lib
*/
class DSymbianUnitTest : public DLogicalChannelBase
{
    
public: // Typedefs
    
    typedef TInt ( DSymbianUnitTest::*FunctionPtr )();
    

public:

    //Constructor
    IMPORT_C DSymbianUnitTest();
    
    // Destructor
    IMPORT_C virtual ~DSymbianUnitTest();
    
    /**
    * This function should be overidden in the actual unit test classes.
    * It is the default setup function that gets executed 
    * by the framework before each unit test case.
    */        
    IMPORT_C virtual TInt Setup();
    
    /**
    * This function should be overidden in the actual unit test classes.
    * It is the default teardown function that gets executed 
    * by the framework after each unit test case.
    */        
    IMPORT_C virtual TInt Teardown();
    

protected:    

    /**
    * Adds a new unit test case to this unit test.
    * Use macro ADD_SYMBIAN_UT in symbianunittestmacros.h 
    * instead to avoid the need to pass 
    * the name of the unit test case as a parameter.
    * 
    * @param aName the name of the unit test case
    * @param aSetupFunction a function pointer to the setup function 
    *        that will be executed before the actual unit test case
    * @param aTestFunction a function pointer to the unit test case
    * @param aTeardownFunction a function pointer to the teardown function
    *        that will be executed after the actual unit test case
    */        
	IMPORT_C void AddTestCase( 
        const TDesC& aName,
        DSymbianUnitTest::FunctionPtr aSetupFunction,
        DSymbianUnitTest::FunctionPtr aTestFunction,
        DSymbianUnitTest::FunctionPtr aTeardownFunction );

    /**
    * Asserts that two TInt values are equal.
    * Leaves with a Symbian unit test framework specific error code
    * if the values are not equal.
    * In case of a failed assertion, the framework records 
    * the failure reason, line number and file name to the test results.
    * Use macro SYMBIAN_UT_ASSERT_EQUALS in symbianunittestmacros.h 
    * instead to avoid the need to pass the other parameters 
    * than the actual asserted values.  
    * 
    * @param aExpectedValue the expected value
    * @param aActualValue the actual value
    * @param aLineNumber the line number of the assertion
    * @param aFileName the name of the file where the assertion is located
    */        
    IMPORT_C TInt AssertEquals( 
        TInt aExpectedValue,
        TInt aActualValue,
        TInt aLineNumber,
        const TDesC8& aFileName );        

    /**
    * Asserts that two TDesC8 values are equal.
    * Leaves with a Symbian unit test framework specific error code
    * if the values are not equal.
    * In case of a failed assertion, the framework records 
    * the failure reason, line number and file name to the test results.
    * Use macro SYMBIAN_UT_ASSERT_EQUALS in symbianunittestmacros.h 
    * instead to avoid the need to pass the other parameters 
    * than the actual asserted values.  
    * 
    * @param aExpectedValue the expected value
    * @param aActualValue the actual value
    * @param aLineNumber the line number of the assertion
    * @param aFileName the name of the file where the assertion is located
    */ 
    IMPORT_C TInt AssertEquals(
        const TDesC8& aExpectedValue,
        const TDesC8& aActualValue, 
        TInt aLineNumber, 
        const TDesC8& aFileName );


   
    /**
    * Records a failed assertion to the test results and
    * leaves with a Symbian unit test framework specific error code
    * to quit the execution of the test function.
    * Use macro SYMBIAN_UT_ASSERT in symbianunittestmacros.h 
    * to perform the actual assertion.  
    * 
    * @param aFailureMessage the failed assertion as text
    * @param aLineNumber the line number of the assertion
    * @param aFileName the name of the file where the assertion is located
    */        
    IMPORT_C TInt AssertionFailed( 
        const TDesC8& aFailureMessage,
        TInt aLineNumber,
        const TDesC8& aFileName );  
    
    /**
     * Add test cases
     * Although the functions implemented in this class 
     * for unit test assertions and adding test cases 
     * can be called directly from the subclasses,
     * the macros defined in symbianunittestmacros.h should be used instead.
     */        
    virtual void Construct() = 0;

    
private: // Internal class for a unit test function's data
        
        class DSymbianUnitTestCase : public DBase
            {            
            public: // Constructors and destructor
                
                DSymbianUnitTestCase( 
                    const TDesC& aName,
                    DSymbianUnitTest::FunctionPtr aSetupFunction,
                    DSymbianUnitTest::FunctionPtr aTestFunction,
                    DSymbianUnitTest::FunctionPtr aTeardownFunction );
                
                ~DSymbianUnitTestCase();
            
            public: // New functions
                
                const TDesC& Name() const;
                
            public: // Data
                
                DSymbianUnitTest::FunctionPtr iSetupFunction;
                DSymbianUnitTest::FunctionPtr iTestFunction;
                DSymbianUnitTest::FunctionPtr iTeardownFunction;
            
            private: // Data
                
                HBuf* iName;
            };
        
private:
    
	
	TInt TestCount(TInt* aDest);
	
	TInt ExecuteTestCase(TAny* aIndex, TDes8* aResult);
	
	TInt GetTestCaseName(TAny* aIndex, TDes* aCaseName);
	
	TInt ExecuteTestCaseInThread();

	static TInt TestThreadEntryFunction( TAny* aPtr );
	
	void Printf(TDes8& aDes, const char* aFmt, ...);
	
	IMPORT_C virtual TInt Request(TInt aReqNo, TAny* a1, TAny* a2);
	
private: // Data
	
	HBuf* iName;
    RPointerArray< DSymbianUnitTestCase > iTestCases;
    DSymbianUnitTestCase* iCurrentTestCase; // Not owned
  
    DThread* iClient;  //pointer to client thread for read/write data
    TSUTTestCaseResult* iCurrentResult; // Not owned
    NFastSemaphore iExecSem;//semaphore for synchronization between main thread and execworkder thread
    
    //TUint iExecThreadId; //id of the current exec workder thread.
    //DCrashHandler* iCrashHandler;
	
};



#endif //__D_SYMBIANUNITTESTLDD_H

