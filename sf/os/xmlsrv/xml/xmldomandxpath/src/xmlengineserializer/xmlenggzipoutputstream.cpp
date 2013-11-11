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

#include "xmlenggzipoutputstream.h"
#include <ezstream.h>
#include <ezcompressor.h>

/**
  * Default input buffers size.
  */
static const TInt KDefaultInputBufferSize = 4096;
	    
/**
  * Output buffer size. It determines, how often output data will be written
  * to disk. (it's written to disk, when buffer is full.
  *
  */   
static const TInt KOutputSize = 0x1000;


CXmlEngGZIPOutputStream* CXmlEngGZIPOutputStream::NewLC(MXmlEngOutputStream& aOutputBuffer )
    {
    CXmlEngGZIPOutputStream* gzbos = new (ELeave) CXmlEngGZIPOutputStream( aOutputBuffer );
    CleanupStack::PushL( gzbos );
    gzbos->ConstructL();
    return gzbos;	
    }

CXmlEngGZIPOutputStream* CXmlEngGZIPOutputStream::NewL( MXmlEngOutputStream& aOutputBuffer )
    {
    CXmlEngGZIPOutputStream* gzbos = CXmlEngGZIPOutputStream::NewLC( aOutputBuffer );
    CleanupStack::Pop( gzbos );
    return gzbos;	
    }

CXmlEngGZIPOutputStream::CXmlEngGZIPOutputStream( MXmlEngOutputStream& aOutputBuffer )
    {
    iOutputString = &aOutputBuffer;
    iKeepGoing = ETrue;
    }

void CXmlEngGZIPOutputStream::ConstructL()
    {
    //we can't create compressor here, because we heve no input at this moment
    iOutputDescriptor.CreateL( KOutputSize );
    iInputDescriptor.CreateL( KDefaultInputBufferSize );
    iOldInputDescriptor.CreateL( KDefaultInputBufferSize );
    }

CXmlEngGZIPOutputStream::~CXmlEngGZIPOutputStream()
    {
    delete iCompressor;
    iOutputDescriptor.Close();
    iInputDescriptor.Close();
    iOldInputDescriptor.Close();
    }

TInt CXmlEngGZIPOutputStream::Write( const TDesC8& aBuffer )
	{
	TRAP ( iError, WriteL( aBuffer ) );
	if ( iError )
	    {
        return -1;
	    }
	
	return aBuffer.Size();
	}

void CXmlEngGZIPOutputStream::WriteL( const TDesC8& aBuffer )
	{
	iNoInputNeeded = ETrue;
	if ( !iCompressor )
	    {
	    const TInt newLength = aBuffer.Size();
	    if ( newLength > iInputDescriptor.Size() )
	        {
	        iInputDescriptor.ReAllocL( newLength );
	        }
	    iInputDescriptor.Copy( aBuffer );
	    iCompressor = CEZCompressor::NewL( *this );
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
	
TInt CXmlEngGZIPOutputStream::Close()
	{
	iCloseInvoked = ETrue;
	TRAP ( iError, CloseL() );
	if ( iError )
	    {
        return -1;
	    }		
	return KErrNone;
	}

void CXmlEngGZIPOutputStream::CloseL()
	{
	while ( iKeepGoing )
	    {
	    iKeepGoing = iCompressor->DeflateL();
	    }
	}
		
TInt CXmlEngGZIPOutputStream::CheckError()
    {
    return iError;
    }
   
void CXmlEngGZIPOutputStream::InitializeL( CEZZStream& aZStream )
   	{
   	aZStream.SetInput( iInputDescriptor );
   	aZStream.SetOutput( iOutputDescriptor );
   	iNoInputNeeded = EFalse;
   	}

void CXmlEngGZIPOutputStream::NeedInputL( CEZZStream& aZStream )
   	{
    if ( iCloseInvoked )
        {
        aZStream.SetInput( KNullDesC8 );
        }
    else        
        {
       	aZStream.SetInput( iInputDescriptor );
       	iNoInputNeeded = EFalse;
       	}
   	}

void CXmlEngGZIPOutputStream::NeedOutputL( CEZZStream& aZStream )
   	{
   	TPtrC8 od = aZStream.OutputDescriptor();
	TInt res = iOutputString->Write(od);
   	if (res < 0)
   		{
		User::Leave(res);
    	}
   	aZStream.SetOutput( iOutputDescriptor );
   	}

void CXmlEngGZIPOutputStream::FinalizeL( CEZZStream& aZStream )
    {
    TPtrC8 od = aZStream.OutputDescriptor();
	TInt res = iOutputString->Write(od);
   	if (res < 0)
   		{
		User::Leave(res);
    	}
   	res = iOutputString->Close();
   	if (res < 0)
   		{
		User::Leave(res);
    	}
   	}
 
