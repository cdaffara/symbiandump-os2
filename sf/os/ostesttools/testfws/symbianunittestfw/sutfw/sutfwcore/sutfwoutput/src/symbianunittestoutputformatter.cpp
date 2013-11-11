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

#include "symbianunittestoutputformatter.h"
#include "symbianunittestfileoutputwriter.h"
#include "symbianunittestresult.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputFormatter::CSymbianUnitTestOutputFormatter()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputFormatter::ConstructL( const TDesC& aFileName ) 
    {
    TInt fileNameLength = aFileName.Length();
    fileNameLength += 1; // dot length
    fileNameLength += FileExtension().Length();
    HBufC* fileName = HBufC::NewLC( fileNameLength );
    fileName->Des() = aFileName;
    fileName->Des().Append('.');
    fileName->Des().Append( FileExtension() );
    iOutputWriter = CSymbianUnitTestFileOutputWriter::NewL( *fileName );
    CleanupStack::PopAndDestroy( fileName );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputFormatter::~CSymbianUnitTestOutputFormatter()
    {
    delete iOutputWriter;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputFormatter::PrintL( CSymbianUnitTestResult& aResult ) 
    {  
    PrintHeaderL( aResult );
    PrintPassedTestsL( aResult );
    PrintFailedTestsL( aResult );
    PrintFooterL();
    }
