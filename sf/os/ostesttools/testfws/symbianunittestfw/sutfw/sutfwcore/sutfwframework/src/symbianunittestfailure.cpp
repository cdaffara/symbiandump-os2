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

#include "symbianunittestfailure.h"
#include <utf.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestFailure* CSymbianUnitTestFailure::NewL(
    const TDesC& aTestName,
    const TDesC8& aFailureMessage,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    CSymbianUnitTestFailure* self = 
        CSymbianUnitTestFailure::NewLC( 
            aTestName, aFailureMessage, aLineNumber, aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestFailure* CSymbianUnitTestFailure::NewLC(
    const TDesC& aTestName,
    const TDesC8& aFailureMessage,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    CSymbianUnitTestFailure* self = 
        new( ELeave )CSymbianUnitTestFailure( aLineNumber );
    CleanupStack::PushL( self );
    self->ConstructL( aTestName, aFailureMessage, aFileName );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestFailure::CSymbianUnitTestFailure( TInt aLineNumber )
    : iLineNumber( aLineNumber )
	{
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestFailure::ConstructL(
    const TDesC& aTestName,
    const TDesC8& aFailureMessage,
    const TDesC8& aFileName )
    {
    iTestName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aTestName );   
    iFailureMessage = aFailureMessage.AllocL();
    iFileName = aFileName.AllocL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestFailure::~CSymbianUnitTestFailure()
    {
    delete iFileName;
    delete iFailureMessage;
    delete iTestName;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& CSymbianUnitTestFailure::TestName()
    {
    return *iTestName;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& CSymbianUnitTestFailure::FailureMessage()
    {
    return *iFailureMessage;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestFailure::LineNumber()
    {
    return iLineNumber;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& CSymbianUnitTestFailure::FileName()
    {
    return *iFileName;
    }
