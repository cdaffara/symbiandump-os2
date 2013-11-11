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

#include "symbianunittestfileoutputwriter.h"
#include "symbianunittestuicallback.h"
#include <utf.h>

_LIT( KResultDirectory, "\\sut\\" );
_LIT8( KDateAndTimeFormat, "%02d.%02d.%04d @ %02d:%02d:%02d" );

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestFileOutputWriter* CSymbianUnitTestFileOutputWriter::NewLC( 
    const TDesC& aFileName )
    {
    CSymbianUnitTestFileOutputWriter* self = 
        new( ELeave )CSymbianUnitTestFileOutputWriter();
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestFileOutputWriter* CSymbianUnitTestFileOutputWriter::NewL( 
    const TDesC& aFileName )
    {
    CSymbianUnitTestFileOutputWriter* self = 
        CSymbianUnitTestFileOutputWriter::NewLC( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestFileOutputWriter::ConstructL( const TDesC& aFileName )
    {
    User::LeaveIfError( iRFs.Connect() );
    TInt err = iRFs.MkDir( KResultDirectory );
    if ( err == KErrNoMemory )
        {
        // Ignore the other errors. The directory may already exist.
        User::Leave( err );
        }
    User::LeaveIfError( iRFs.SetSessionPath( KResultDirectory ) );
    User::LeaveIfError( iFile.Replace( iRFs, aFileName, EFileWrite ) );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestFileOutputWriter::CSymbianUnitTestFileOutputWriter()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestFileOutputWriter::~CSymbianUnitTestFileOutputWriter()
    {
    iFile.Close();
    iRFs.Close();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestFileOutputWriter::WriteDateAndTimeL()
    {
    TTime time;
    time.HomeTime();   
    TDateTime dateTime( time.DateTime() );
    HBufC8* buf = HBufC8::NewLC( KDateAndTimeFormat().Length() );
    buf->Des().Format( KDateAndTimeFormat(), dateTime.Day()+1, 
                       dateTime.Month()+1, dateTime.Year(), dateTime.Hour(), 
                       dateTime.Minute(), dateTime.Second() );
    User::LeaveIfError( iFile.Write( *buf ) );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestFileOutputWriter::WriteL( const TDesC8& aValue )
    {
    User::LeaveIfError( iFile.Write( aValue ) );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestFileOutputWriter::WriteL( const TDesC& aValue )
    {
    HBufC8* value = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aValue);
    CleanupStack::PushL(value);
    User::LeaveIfError( iFile.Write( *value ) );
    CleanupStack::PopAndDestroy( value );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestFileOutputWriter::WriteL( 
    const TDesC8& aFormat, 
    TInt aValue )
    {
    const TInt KIntAsTextMaxLength( 20 );
    HBufC8* buf = HBufC8::NewLC( aFormat.Length() + KIntAsTextMaxLength );
    buf->Des().Format( aFormat, aValue );
    User::LeaveIfError( iFile.Write( *buf ) );
    CleanupStack::PopAndDestroy( buf );
    }
