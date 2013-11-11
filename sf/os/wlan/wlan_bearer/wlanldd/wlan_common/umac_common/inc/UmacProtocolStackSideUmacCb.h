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
* Description:   Declaration of the WlanProtocolStackSideUmacCb class.
*
*/

/*
* %version: 15 %
*/

#ifndef WLANPROTOCOLSTACKSIDEUMACCB_H
#define WLANPROTOCOLSTACKSIDEUMACCB_H

class TDataBuffer;

/**
*  
*/
class WlanProtocolStackSideUmacCb
    {
public: 
    
    // dtor
    virtual inline ~WlanProtocolStackSideUmacCb();

    /**
    * 
    * UMAC adaptation needs to call this method!
    */
    inline void AttachProtocolStackSideUmacCb( 
        WlanProtocolStackSideUmacCb& aSelf );
    
    /**
    * 
    * UMAC adaptation needs to call this method!
    */
    inline void DetachProtocolStackSideUmacCb(); 

    /**
    * Sets the Tx offset for every frame type which can be transmitted
    * UMAC adaptation needs to implement this method!
    *
    * @param aEthernetFrameTxOffset Tx offset for Ethernet frames
    */
    virtual void SetProtocolStackTxOffset( 
        TUint32 aEthernetFrameTxOffset );

    /**
    * Called when the Tx packet in question has been trasferred to the WLAN
    * device.
    * 
    * UMAC adaptation needs to implement this method!
    * @param aCompletionCode Status of the operation.
    * @param aMetaHeader Meta header associated with the related Tx packet
    */
    virtual void OnTxProtocolStackDataComplete( 
        TInt aCompletionCode,
        TDataBuffer* aMetaHeader );

    /**
    * Called when a Tx packet - submitted by someone else than the Protocol
    * Stack Side Client - has been trasferred to the WLAN device.
    * 
    * Note! TxProtocolStackData() must not be called in the same context in 
    * which this method is called (but instead e.g. via a DFC)!
    *  
    * UMAC adaptation needs to implement this method!
    */
    virtual void OnOtherTxDataComplete();
    
    /**
    * Called when the Tx of a frame has completed (either successfully or
    * unsuccessfully).
    * 
    * Note! TxProtocolStackData() must not be called in the same context in 
    * which this method is called (but instead e.g. via a DFC)!
    *  
    * UMAC adaptation needs to implement this method!
    */
    virtual void OnTxDataSent();

    /**
    * Completes a data read operation from protocol stack side
    * UMAC adaptation needs to implement this method!
    *
    * @param aBufferStart first element of the array that holds pointers to
    *        Rx frame meta headers
    * @param aNumOfBuffers number of meta header pointers in the array
    * @return ETrue if this event was processed successfully
    *         EFalse otherwise
    */
    virtual TBool ProtocolStackDataReceiveComplete( 
        TDataBuffer*& aBufferStart, 
        TUint32 aNumOfBuffers );

    /**
    * Determines if the Protocol Stack Side client is ready to handle any
    * callbacks from UMAC
    * UMAC adaptation needs to implement this method!
    *
    * @return ETrue if the client is ready
    *         EFalse if the client is not ready
    */
    virtual TBool ProtocolStackSideClientReady() const;
    
    /**
    * Indicates that WLAN Mgmt Client has (re-)enabled protocol stack side
    * user data Tx.
    * UMAC adaptation needs to implement this method!
    * To quickly resume user data Tx after roaming, UMAC adaptation should call 
    * MWlanProtocolStackSideUmac::TxProtocolStackData() as soon as possible 
    * after receiving this indication. Note! TxProtocolStackData() must not be 
    * called in the same context in which this method is called (but instead
    * e.g. via a DFC)!
    */
    virtual void UserDataReEnabled();

protected:

    WlanProtocolStackSideUmacCb() : iSelf( NULL ) {};
       
private:
    
    // Prohibit copy constructor 
    WlanProtocolStackSideUmacCb( const WlanProtocolStackSideUmacCb& );
    // Prohibit assigment operator 
    WlanProtocolStackSideUmacCb& operator= ( 
        const WlanProtocolStackSideUmacCb& );   

private: // Data

    /** pointer to self */
    WlanProtocolStackSideUmacCb* iSelf;
    };

#include "UmacProtocolStackSideUmacCb.inl"

#endif      // WLANPROTOCOLSTACKSIDEUMACCB_H
