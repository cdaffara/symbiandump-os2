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
* Description:   Implementation of RPcmNetCardIf inline methods.
*
*/

/*
* %version: 10 %
*/

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TInt RPcmNetCardIf::Open()
	{
	return DoCreate( 
             LDD_NAME, 
             VersionRequired(), 
             KUnitEthernet, 
             NULL, 
             NULL );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TVersion RPcmNetCardIf::VersionRequired() const
    {
	return TVersion( 
	            KWlanDriverMajorVersion, 
	            KWlanDriverMinorVersion, 
	            KWlanDriverBuildVersion );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void RPcmNetCardIf::ReadCancel()
    { 
    DoCancel( 1 << EControlReadCancel );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void RPcmNetCardIf::ResumeTxCancel()
    { 
    DoCancel( 1 << EControlResumeTxCancel );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TInt RPcmNetCardIf::GetConfig( TDes8& aConfig )
    {
    return DoControl( 
                EControlGetConfig, 
                const_cast<TUint8*>(aConfig.Ptr()) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TInt RPcmNetCardIf::InitialiseBuffers()
    {
    TInt status ( KErrNone );
    TSharedChunkInfo info;
    
    status = DoSvControl( 
        ESvControlInitBuffers, 
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

        RFrameXferBlockProtocolStack* frameXferBlock = 
            reinterpret_cast<RFrameXferBlockProtocolStack*>(
                baseAddress
                + KRxDataChunkSize
                + sizeof( TDataBuffer )
                + KMgmtSideTxBufferLength
                + KProtocolStackSideTxDataChunkSize
                + sizeof( RFrameXferBlock ) );

        frameXferBlock->UserInitialize( 
            reinterpret_cast<TUint32>(frameXferBlock) );
        }
    
    return status;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TInt RPcmNetCardIf::ReleaseBuffers()
    {
    // close the handle to the shared memory chunk
    iSharedMemoryChunk.Close();
    
    return DoSvControl( ESvControlFreeBuffers );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void RPcmNetCardIf::ResumeTx( TRequestStatus& aStatus )
    {
    DoRequest( EResumeTx, aStatus );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void RPcmNetCardIf::RequestFrame( TRequestStatus& aStatus )
    {
    DoRequest( ERequestRead, aStatus );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TDataBuffer* RPcmNetCardIf::AllocTxBuffer( TUint aLength )
    {
    return reinterpret_cast<TDataBuffer*>(DoControl( 
        EControlFastAllocTxBuffer,
        reinterpret_cast<TAny*>(aLength) ));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool RPcmNetCardIf::AddTxFrame( TDataBuffer* aPacket )
    {
    return static_cast<TBool>(DoControl( 
        EControlFastAddTxFrame, 
        reinterpret_cast<TAny*>(aPacket) ));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TDataBuffer* RPcmNetCardIf::GetRxFrame( TDataBuffer* aFrameToFree )
    {
    return reinterpret_cast<TDataBuffer*>(DoControl( 
        EControlFastGetRxFrame,
        reinterpret_cast<TAny*>(aFrameToFree) ));
    }
