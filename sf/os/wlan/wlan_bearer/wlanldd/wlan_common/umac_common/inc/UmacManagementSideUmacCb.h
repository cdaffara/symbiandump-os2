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
* Description:   Declaration of the WlanManagementSideUmacCb class.
*
*/

/*
* %version: 18 %
*/

#ifndef WLANMANAGEMENTSIDEUMACB_H
#define WLANMANAGEMENTSIDEUMACB_H

class TDataBuffer;
struct TMacAddress;

/**
* Callback interface class for oid completion and stuff
*/
class WlanManagementSideUmacCb
    {
public:  // Types

    struct SOidOutputData
        {
        public:

            TUint32 iOidId;
            TAny*   iBufferSupplied;
            TUint32 iLengthOfBufferSupplied;

            const TAny* iOidData;
            TUint32     iLengthOfDataInBytes;

            SOidOutputData() 
                : iOidId( 0 ), 
                iBufferSupplied( NULL ),
                iLengthOfBufferSupplied( 0 ),
                iOidData( NULL ),
                iLengthOfDataInBytes( 0 ) {};

        private:
            
            /** Prohibit assignment operator */
            SOidOutputData& operator= ( const SOidOutputData& aObj );    
            /** Prohibit copy constructor */
            SOidOutputData( const SOidOutputData& );
        };

public:
    
    // dtor
    virtual ~WlanManagementSideUmacCb();

    void Attach( WlanManagementSideUmacCb& aSelf ); 
    void Detach(); 

    /**
    * Called from statemachine when oid has been completed. 
    * Triggers handling of a new oid
    * @param aOid OID handled
    * @param aReason completion code
    */
    virtual void OnOidCompleted( TInt aReason, SOidOutputData& OidOutputData );

    /**
    * Sets the Tx offset for every frame type which can be transmitted
    *
    * @since S60 5.0
    * @param aEthernetFrameTxOffset Tx offset for Ethernet frames and Ethernet
    *        Test frames
    * @param aDot11FrameTxOffset Tx offset for 802.11 frames
    * @param aSnapFrameTxOffset Tx offset for SNAP frames
    */
    virtual void SetMgmtSideTxOffsets( 
        TUint32 aEthernetFrameTxOffset,
        TUint32 aDot11FrameTxOffset,
        TUint32 aSnapFrameTxOffset ); 

    /**
    * Gets buffer for Rx data
    * @param aLengthinBytes Requested buffer length
    * @return buffer for Rx data upon success
    *         NULL otherwise
    */
    virtual TUint8* GetBufferForRxData( TUint aLengthinBytes );

    /**
     * Get DMA xfer suitable Tx buffer for UMAC's internal use
     *
     * @since S60 3.1
     * @returns begin of the memory that can be used for DMA xfer 
     */
    virtual TUint8* DmaPrivateTxMemory();
    
    /**
    * Completes a data read operation from management side
    *
    * @param aBufferStart first element of the array that holds pointers to
    *        Rx frame meta headers
    * @param aNumOfBuffers number of meta header pointers in the array
    */
    virtual void MgmtDataReceiveComplete( 
        TDataBuffer*& aBufferStart, 
        TUint32 aNumOfBuffers );

    /**
    * Completes a data write operation from management side
    * @param aErr completion code 
    */
    virtual void MgmtPathWriteComplete( TInt aErr );

    /**
    * Sends a indication to the management side
    * @param aIndication indication code 
    */
    virtual void OnInDicationEvent( TIndication aIndication );

    /**
    * Frees the specified Rx frame buffer
    *
    * @param aBufferToFree The buffer to free
    */
    virtual void MarkRxBufFree( TUint8* aBufferToFree );

    virtual void RegisterTimeout( 
        TUint32 aTimeoutInMicroSeconds, 
        TWlanTimer aTimer = EWlanDefaultTimer );
    virtual void CancelTimeout( TWlanTimer aTimer = EWlanDefaultTimer );

    virtual void RegisterDfc( TAny* aCntx );
    virtual void CancelDfc();

    /**
    * Allocates Rx frame meta header
    * @return Rx frame meta header upon success
    *         NULL otherwise
    */
    virtual TDataBuffer* GetRxFrameMetaHeader();

    /**
    * Deallocates Rx frame meta header
    * @param aMetaHeader Meta header to deallocate
    */
    virtual void FreeRxFrameMetaHeader( TDataBuffer* aMetaHeader );
    
protected:

    WlanManagementSideUmacCb() : iSelf( NULL ) {};

private:
    
    // Prohibit copy constructor 
    WlanManagementSideUmacCb( const WlanManagementSideUmacCb& );
    // Prohibit assigment operator 
    WlanManagementSideUmacCb& operator= ( const WlanManagementSideUmacCb& );   

private: // Data

    /** pointer to self */
    WlanManagementSideUmacCb* iSelf;
    };

#endif      // WLANMANAGEMENTSIDEUMACB_H
