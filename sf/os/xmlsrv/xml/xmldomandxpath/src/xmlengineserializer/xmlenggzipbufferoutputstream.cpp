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

#include "xmlenggzipbufferoutputstream.h"
#include <ezstream.h>
#include <ezcompressor.h>


    CXmlEngGZIPBufferOutputStream* CXmlEngGZIPBufferOutputStream::NewLC( RBuf8& aOutputBuffer )
	    {
	    CXmlEngGZIPBufferOutputStream* gzbos = new (ELeave) CXmlEngGZIPBufferOutputStream( aOutputBuffer );
	    CleanupStack::PushL( gzbos );
	    gzbos->ConstructL();
	    return gzbos;	
	    }

    CXmlEngGZIPBufferOutputStream* CXmlEngGZIPBufferOutputStream::NewL( RBuf8& aOutputBuffer )
	    {
	    CXmlEngGZIPBufferOutputStream* gzbos = CXmlEngGZIPBufferOutputStream::NewLC( aOutputBuffer );
	    CleanupStack::Pop( gzbos );
	    return gzbos;	
	    }

   CXmlEngGZIPBufferOutputStream::CXmlEngGZIPBufferOutputStream( RBuf8& aOutputBuffer ) : iOutputString(aOutputBuffer)
        {
        iKeepGoing = ETrue;
        }

   void CXmlEngGZIPBufferOutputStream::ConstructL()
        {
        if(!iOutputString.MaxSize())
            {
            iOutputString.ReAllocL(KOutputBufferSize);
            }
        //we can't create compressor here, because we heve no input at this moment
        iOutputDescriptor.CreateL( KOutputBufferSize );
        iInputDescriptor.CreateL( KDefaultInputBufferSize );
        iOldInputDescriptor.CreateL( KDefaultInputBufferSize );
        }

   CXmlEngGZIPBufferOutputStream::~CXmlEngGZIPBufferOutputStream()
        {
        delete iCompressor;
        iOutputDescriptor.Close();
        iInputDescriptor.Close();
        iOldInputDescriptor.Close();
        }

	TInt CXmlEngGZIPBufferOutputStream::Write( const TDesC8& aBuffer )
		{
		TRAP ( iError, WriteL( aBuffer ) );
		if ( iError )
		    {
	        return -1;
		    }
		
		return aBuffer.Size();
		}

	void CXmlEngGZIPBufferOutputStream::WriteL( const TDesC8& aBuffer )
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
		
	TInt CXmlEngGZIPBufferOutputStream::Close()
		{
		iCloseInvoked = ETrue;
        if ( iError )
            {
            return -1;
            }		
		TRAP ( iError, CloseL() );
		if ( iError )
		    {
	        return -1;
		    }		
		return KErrNone;
		}

	void CXmlEngGZIPBufferOutputStream::CloseL()
		{
		while ( iKeepGoing )
		    {
		    iKeepGoing = iCompressor->DeflateL();
		    }
		}
		
	TInt CXmlEngGZIPBufferOutputStream::CheckError()
	    {
	    return iError;
	    }
    
    void CXmlEngGZIPBufferOutputStream::InitializeL( CEZZStream& aZStream )
    	{
    	aZStream.SetInput( iInputDescriptor );
    	aZStream.SetOutput( iOutputDescriptor );
    	iNoInputNeeded = EFalse;
    	}

    void CXmlEngGZIPBufferOutputStream::NeedInputL( CEZZStream& aZStream )
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

    void CXmlEngGZIPBufferOutputStream::NeedOutputL( CEZZStream& aZStream )
    	{
    	TPtrC8 od = aZStream.OutputDescriptor();
    	while(TRUE)
    	    {
    	    if (od.Size() > (iOutputString.MaxSize() - iOutputString.Size()))
		        {
		        iOutputString.ReAllocL( iOutputString.MaxSize()*2 );
		        }
		    else
		        {
		        break;
		        }    	    
    	    }
    	iOutputString.Append( od );
    	aZStream.SetOutput( iOutputDescriptor );
    	}

    void CXmlEngGZIPBufferOutputStream::FinalizeL( CEZZStream& aZStream )
        {
        TPtrC8 od = aZStream.OutputDescriptor();
    	while(TRUE)
    	    {
    	    if (od.Size() > (iOutputString.MaxSize() - iOutputString.Size()))
		        {
		        iOutputString.ReAllocL( iOutputString.MaxSize()*2 );
		        }
		    else
		        {
		        break;
		        }    	    
    	    }
    	iOutputString.Append( od );
    	}

  
