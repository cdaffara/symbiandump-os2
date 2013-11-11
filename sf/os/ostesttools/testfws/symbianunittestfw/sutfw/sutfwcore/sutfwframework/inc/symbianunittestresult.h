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
#ifndef SYMBIANUNITTESTRESULT_H
#define SYMBIANUNITTESTRESULT_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <symbianunittestmacros.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestFailure;

// CLASS DECLARATION
class CSymbianUnitTestResult : public CBase
    {
    public: // Constructors and destructor
    
        static CSymbianUnitTestResult* NewLC();
        static CSymbianUnitTestResult* NewL();
        ~CSymbianUnitTestResult();
    
    public: // New functions
        
        TInt TestCount() const;
        
        TInt PassedTestCount() const;
        
        RPointerArray< CSymbianUnitTestFailure >& Failures();    
        
        void StartTestL( const TDesC& aTestName );    
        void EndTestL();    
        
        TBool CurrentResult();
 
        void AddSetupErrorL( 
            TInt aError );        
        
        void AddLeaveFromTestL( 
            TInt aLeaveCode,
            TInt aAllocFailureRate );
        
        void AddMemoryLeakInfoL(
            TInt aLeakedMemory,
            TInt aAllocFailureRate );
        
        void AddResourceLeakInfoL(
            TInt aLeakedResource,
            TInt aAllocFailureRate );
        
        void AddRequestLeakInfoL(
            TInt aLeakedRequest,
            TInt aAllocFailureRate );
        
        void AddPanicInfoL(
            const TExitCategoryName& aPanicCategory,
            TInt aPanicReason,
            TInt aAllocFailureRate );
    
        TInt AddAssertFailure(
            const TDesC8& aFailureMessage,
            TInt aLineNumber,
            const TDesC8& aFileName );        

	void AddTimeOutErrorL(
	    TInt aTimeOut);

	const CDesCArray& TestCaseNames() const;
        
    private: // Constructors
    
        CSymbianUnitTestResult();
        void ConstructL();
        
    private: // New functions
   
        void AddAssertFailureL(
            const TDesC8& aFailureMessage,
            TInt aLineNumber,
            const TDesC8& aFileName );        
        
        TInt AddFailure( 
            const TDesC8& aFailureText,
            TInt aAllocFailureRate );        
        
        void AddFailureL( 
            const TDesC8& aFailureText,
            TInt aAllocFailureRate );

    private: // Data
        
        RHeap& iOriginalHeap;
        RPointerArray< CSymbianUnitTestFailure > iFailures;
        TInt iTestCount;
        HBufC* iCurrentTestName;
    	TTime iTime;
    	TBool iCurrentResult;
    	CDesCArray * iTestCaseNames;
        
    private: // Test
        
        SYMBIAN_UNIT_TEST_CLASS( UT_CSymbianUnitTestResult )        
    };

#endif // SYMBIANUNITTESTRESULT_H


