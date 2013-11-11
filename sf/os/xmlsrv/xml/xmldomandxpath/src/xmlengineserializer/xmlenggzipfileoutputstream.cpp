// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// GZIP output stream functions
//

#include "xmlenggzipfileoutputstream.h"
#include <ezstream.h>
#include <ezcompressor.h>

/**
 * Default input buffers size.
 */
const TInt CXmlEngGZIPFileOutputStream::KDefaultInputBufferSize = 4096;
	    
/**
 * Output buffer size. It determines, how often output data will be written
 * to disk. (it's written to disk, when buffer is full.
 *
 */
const TInt CXmlEngGZIPFileOutputStream::KOutputBufferSize = 0x8000;

CXmlEngGZIPFileOutputStream* CXmlEngGZIPFileOutputStream::NewLC( RFile& aOutputFile, 
                                                                RFs& aRFs )
    {
    CXmlEngGZIPFileOutputStream* gzfos = new (ELeave) CXmlEngGZIPFileOutputStream( aOutputFile, 
	                                                                                aRFs );
    CleanupStack::PushL( gzfos );
    gzfos->ConstructL();
    return gzfos;	
    }

CXmlEngGZIPFileOutputStream* CXmlEngGZIPFileOutputStream::NewL( RFile& aOutputFile, 
                                                                RFs& aRFs )
    {
    CXmlEngGZIPFileOutputStream* gzfos = CXmlEngGZIPFileOutputStream::NewLC( aOutputFile, 
	                                                                                aRFs );
    CleanupStack::Pop( gzfos );
    return gzfos;	
    }

CXmlEngGZIPFileOutputStream::CXmlEngGZIPFileOutputStream( RFile& aOutputFile, RFs& aRFs )
    {
    iKeepGoing = ETrue;
    iOutputFile = aOutputFile;
    iRFs = aRFs;
    iCrc = crc32( iCrc, NULL, 0 );
    }

void CXmlEngGZIPFileOutputStream::ConstructL()
    {
    EZGZipFile::WriteHeaderL( iOutputFile, iHeader );
    //we can't create compressor here, because we heve no input at this moment
    iOutputDescriptor.CreateL( KOutputBufferSize );
    iInputDescriptor.CreateL( KDefaultInputBufferSize );
    iOldInputDescriptor.CreateL( KDefaultInputBufferSize );
    }

CXmlEngGZIPFileOutputStream::~CXmlEngGZIPFileOutputStream()
    {
    delete iCompressor;
    iOutputDescriptor.Close();
    iInputDescriptor.Close();
    iOldInputDescriptor.Close();
    }

TInt CXmlEngGZIPFileOutputStream::Write( const TDesC8& aBuffer )
    {
    TRAP ( iError, WriteL( aBuffer ) );
    if ( iError )
        {
        return KErrNotFound;
        }
    
    TInt bufferSize = aBuffer.Size();
    iUncompressedDataSize += bufferSize;
						
    return bufferSize;
    }

void CXmlEngGZIPFileOutputStream::WriteL( const TDesC8& aBuffer )
    {
    iNoInputNeeded = ETrue;
    if ( !iCompressor )
        {
        const TInt newLength = aBuffer.Length();
        if ( newLength > iInputDescriptor.MaxLength() )
            {
            iInputDescriptor.ReAllocL( newLength );
            }
        iInputDescriptor.Copy( aBuffer );
        iCompressor = CEZCompressor::NewL( *this, CEZCompressor::EDefaultCompression,
		                                                             -CEZCompressor::EMaxWBits );    
        }
    else
        {
        iInputDescriptor.Swap( iOldInputDescriptor );
        const TInt newLength = aBuffer.Length();
        if ( newLength > iInputDescriptor.MaxLength() )
            {
            iInputDescriptor.ReAllocL( newLength );
            }
        iInputDescriptor.Copy( aBuffer );		    
		    
        while ( iNoInputNeeded )
            {
            iCompressor->DeflateL();
            }		        
        }       		
    }
	
TInt CXmlEngGZIPFileOutputStream::Close()
    {
    iCloseInvoked = ETrue;
    if ( iError )
		    {
	        return -1;
		    }
    TRAP ( iError, CloseL() );
    if ( iError )
        {
        return KErrNotFound;
        }		
    return KErrNone;
    }

void CXmlEngGZIPFileOutputStream::CloseL()
    {
    while ( iKeepGoing )
        {
        iKeepGoing = iCompressor->DeflateL();
        }
    TEZGZipTrailer trailer( iCrc, iUncompressedDataSize );
    EZGZipFile::WriteTrailerL( iOutputFile, trailer );
    }
		
TInt CXmlEngGZIPFileOutputStream::CheckError()
    {
    return iError;
    }
    
void CXmlEngGZIPFileOutputStream::InitializeL( CEZZStream& aZStream )
    {
    aZStream.SetInput(iInputDescriptor);
    aZStream.SetOutput(iOutputDescriptor);
    iCrc = crc32(iCrc,iInputDescriptor.Ptr(),iInputDescriptor.Size());
    iNoInputNeeded = EFalse;
    }

void CXmlEngGZIPFileOutputStream::NeedInputL( CEZZStream& aZStream )
    {
    if ( iCloseInvoked )
        {
        aZStream.SetInput( KNullDesC8 );
        iCrc = crc32( iCrc, (unsigned char*)18, 0 );    
        //second argument can't be null, and third should be 0 !!!
        }
    else        
        {
        aZStream.SetInput( iInputDescriptor );
        iCrc = crc32( iCrc, iInputDescriptor.Ptr(), iInputDescriptor.Size() );
        iNoInputNeeded = EFalse;
        }
    }

void CXmlEngGZIPFileOutputStream::NeedOutputL( CEZZStream& aZStream )
    {
    TPtrC8 od = aZStream.OutputDescriptor();    	
    User::LeaveIfError( iOutputFile.Write(od) );
    aZStream.SetOutput( iOutputDescriptor );
    }

void CXmlEngGZIPFileOutputStream::FinalizeL( CEZZStream& aZStream )
    {
    TPtrC8 od = aZStream.OutputDescriptor();    	
    User::LeaveIfError( iOutputFile.Write(od) );    
    }
  
