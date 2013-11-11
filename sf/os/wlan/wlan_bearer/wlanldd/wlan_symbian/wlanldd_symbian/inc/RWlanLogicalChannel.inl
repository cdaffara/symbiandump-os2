/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of RWlanLogicalChannel inline methods.
*
*/

/*
* %version: 19 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TVersion RWlanLogicalChannel::VersionRequired() const
   {
	return TVersion( 
        KWlanDriverMajorVersion, 
        KWlanDriverMinorVersion, 
        KWlanDriverBuildVersion );
   }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TInt RWlanLogicalChannel::Open( 
    TWlanUnit aUnit, 
    TOpenParam& aOpenParam )
    {
    TInt err = DoCreate(
        LDD_NAME, 
        VersionRequired(), 
        aUnit, 
        NULL, 
        NULL,
        EOwnerProcess);

    if ( err == KErrNone )
        {
        // driver load sequence success
        // do system init
        err = InitWlanSystem( aOpenParam  );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void RWlanLogicalChannel::CloseChannel()
    {
    if ( Handle() > KNullHandle )
        {
        // we have a valid handle to the kernel side object
    
        // request WLAN system resources to be released
        //
        TRequestStatus status;
        DoRequest( EWlanFinitSystem, status );
        User::WaitForRequest(status);
        
        // internally call close
        Close();
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TInt RWlanLogicalChannel::InitWlanSystem( 
    TOpenParam& aOpenParam )
    {
    iOpenParam = aOpenParam;
    TRequestStatus status;
    DoRequest( EWlanInitSystem, status, &iOpenParam );
    User::WaitForRequest(status);
    return status.Int();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TInt RWlanLogicalChannel::ManagementCommand(
    const TDesC8& aInBuffer, 
    TDes8* aOutBuffer, 
    TRequestStatus* aStatus)
    {
    TInt ret( KErrNoMemory );
    SOidMsgStorage* pdu( new SOidMsgStorage );
    
    if ( pdu )
        {
        TUint32 input_param_len( aInBuffer.Length() );
        os_memcpy( pdu, aInBuffer.Ptr(), input_param_len );

        SOutputBuffer output = { NULL, 0 };
        if ( aOutBuffer )
            {
            output.iData = const_cast<TUint8*>(aOutBuffer->Ptr());
            output.iLen = aOutBuffer->Length();
            }

        if (aStatus == NULL)
            {
            // Execute command synchronously
            TRequestStatus status;
            
            DoRequest( 
                EWlanCommand, 
                status, 
                pdu, 
                (output.iData ? &output : NULL) );
            
	        User::WaitForRequest(status);
        
            ret = status.Int();
            }
        else
            {
            // Execute command asynchronously
            
            iAsyncOidCommandMsg = *pdu;
            iAsyncOidCommandOutput = output;
            DoRequest(
                EWlanCommand, 
                *aStatus, 
                &iAsyncOidCommandMsg, 
                (output.iData ? &iAsyncOidCommandOutput : NULL) );

            ret = KErrNone;
            }

        // always remember to deallocate
        delete pdu;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void RWlanLogicalChannel::RequestSignal(
    TRequestStatus &aStatus,  
    TIndication &aBuffer)
    {
	DoRequest(EWlanRequestNotify, aStatus, &aBuffer);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void RWlanLogicalChannel::CancelRequestSignal()
    {
    // DoCancel uses mask instead of real values.
    DoCancel(1<<EWlanRequestNotify);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void RWlanLogicalChannel::CancelRxRequests()
    {
    // DoCancel uses mask instead of real values.
	DoCancel(1<<EWlanRequestFrame);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TInt RWlanLogicalChannel::InitialiseBuffers( 
    RFrameXferBlock*& aFrameXferBlock )
    {
    TInt status ( KErrNone );
    
    TSharedChunkInfo info;
    
    status = DoSvControl( EWlanSvControlInitBuffers, 
        static_cast<TAny*>(&info) );

    if ( status == KErrNone )
        {
        // shared memory chunk initialization success

        // Set the handle for the shared memory chunk
        iSharedMemoryChunk.SetHandle( info.iChunkHandle );

        // Set the relevant user mode 
        // addresses as offsets from the chunk base address

        TUint8* baseAddress ( iSharedMemoryChunk.Base() );
        
        const TUint KRxDataChunkSize( 
            info.iSize
            - ( sizeof( TDataBuffer )
                + KMgmtSideTxBufferLength
                + KProtocolStackSideTxDataChunkSize
                + sizeof( RFrameXferBlock ) 
                + sizeof( RFrameXferBlockProtocolStack ) ) );

        aFrameXferBlock = reinterpret_cast<RFrameXferBlock*>(
            baseAddress
            + KRxDataChunkSize
            + sizeof( TDataBuffer )
            + KMgmtSideTxBufferLength
            + KProtocolStackSideTxDataChunkSize );

        aFrameXferBlock->SetTxDataBufferField( reinterpret_cast<TLinAddr>(
            baseAddress
            + KRxDataChunkSize ) );
        
        aFrameXferBlock->UserInitialize( 
            reinterpret_cast<TUint32>(aFrameXferBlock) );
        }
    
    return status;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TInt RWlanLogicalChannel::ReleaseBuffers()
    {
    // close the handle to the shared memory chunk
    iSharedMemoryChunk.Close();
    
    return DoSvControl( EWlanSvControlFreeBuffers );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void RWlanLogicalChannel::WriteFrame( 
    TRequestStatus &aStatus )
    {
    DoRequest( EWlanRequestSend, aStatus );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void RWlanLogicalChannel::RequestFrame( 
    TRequestStatus &aStatus )
    {
    DoRequest( EWlanRequestFrame, aStatus );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TDataBuffer* RWlanLogicalChannel::GetRxFrame(
    TDataBuffer* aFrameToFree )
    {
    return reinterpret_cast<TDataBuffer*>(DoControl( 
        EWlanControlFastGetRxFrame,
        reinterpret_cast<TAny*>(aFrameToFree) ));
    }
