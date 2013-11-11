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

#include <symbianunittestsuite.h>
#include "symbianunittestresult.h"
#include "sutlogger.h"
#include <e32debug.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CSymbianUnitTestSuite* CSymbianUnitTestSuite::NewLC( 
    const TDesC& aName )
    {
    CSymbianUnitTestSuite* self = new( ELeave )CSymbianUnitTestSuite();
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CSymbianUnitTestSuite* CSymbianUnitTestSuite::NewL( 
    const TDesC& aName )
    {
    CSymbianUnitTestSuite* self = CSymbianUnitTestSuite::NewLC( aName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTestSuite::ConstructL( const TDesC& aName )
    {
    iName = aName.AllocL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CSymbianUnitTestSuite::CSymbianUnitTestSuite() 
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CSymbianUnitTestSuite::~CSymbianUnitTestSuite() 
    {
    iTests.ResetAndDestroy();
    delete iName;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTestSuite::AddL( 
    MSymbianUnitTestInterface* aTest )
    {
    iTests.AppendL( aTest );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTestSuite::ExecuteL(
    MSymbianUnitTestObserver& aObserver,
    CSymbianUnitTestResult& aResult,
    MSymbianUnitTestInterface::TFailureSimulation aFailureSimulation,
    const CDesCArray& aTestCaseNames,
    TInt aTimeout )
    {
    for ( TInt i=0; i < iTests.Count(); i++ )
        {
        iTests[i]->ExecuteL( aObserver, aResult, aFailureSimulation, aTestCaseNames, aTimeout);
        }
    }


// -----------------------------------------------------------------------------
// From MSymbianUnitTestInterface
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTestSuite::TestCaseNamesL(CDesCArray& aTestCaseNames)
    {
    for ( TInt i=0; i < iTests.Count(); i++ )
        {
        MSymbianUnitTestInterface* test = iTests[i];
        test->TestCaseNamesL(aTestCaseNames);
        }
    
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSymbianUnitTestSuite::TestCaseCount()
    {
    TInt count = 0;
    for ( TInt i=0; i < iTests.Count(); i++ )
        {
        count += iTests[i]->TestCaseCount();
        }
    return count;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CSymbianUnitTestSuite::Name() const
    {
    return *iName;
    }
