/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11IbssNormalMode class.
*
*/

/*
* %version: 13 %
*/

#ifndef C_WLANDOT11IBSSNORMALMODE_H
#define C_WLANDOT11IBSSNORMALMODE_H


#include "UmacDot11IbssMode.h"

/**
 *  Normal IBSS mode operation state
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanDot11IbssNormalMode : public WlanDot11IbssMode
    {
public:  

    WlanDot11IbssNormalMode() {};


    virtual ~WlanDot11IbssNormalMode() {};

private:

    // from base class WlanDot11Associated
        
    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void Exit( WlanContextImpl& aCtxImpl);

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    /**
    * Connect (authenticate and associate) to a WLAN network
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aSSID Name of the network
    * @param aBSSID BSSID of the access point
    * @param aAuthAlgorithmNbr Authentication algorithm number to be used
    * @param aEncryptionStatus Used encryption mode
    * @param aIsInfra ETrue when connecting to infrastructure network
    * @param aScanResponseFrameBodyLength length of the scan response frame body
    * @param aScanResponseFrameBody scan response frame body
    * @param aIeData The IE(s) to be included into the (re-)association request.
    *              NULL, if none to be included
    * @param aIeDataLength Length of the IE(s) to be included into the 
    *                    (re-)association request
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool Connect(
        WlanContextImpl& aCtxImpl,
        const TSSID& aSSID,                 
        const TMacAddress& aBSSID,          
        TUint16 aAuthAlgorithmNbr,      
        TEncryptionStatus aEncryptionStatus,
        TBool aIsInfra,
        TUint16 aScanResponseFrameBodyLength,
        const TUint8* aScanResponseFrameBody,
        const TUint8* aIeData,
        TUint16 aIeDataLength );

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual TBool Disconnect( WlanContextImpl& aCtxImpl );
    
    /**
    * Scan all available networks. 
    * This SW module decides is background scan or foreground scan used
    * @param aCtxImpl statemachine context
    * @param aMode passive or active scan see #TScanMode
    * @param aSSID Network that APs are scanned. Can be broadcast SSID.
    * @param aScanRate Rate that is used in active scanning 
    * @param aMinChannelTime min. time to listen beacons/probe responses on 
    *                        a channel
    * @param aMaxChannelTime max. time to listen beacons/probe responses on 
    *                        a channel
    * @param aSplitScan if ETrue, use split scan
    * @return KErrNone = command completed successfully, 
    * any other = failure
    */
    virtual TBool RealScan(
        WlanContextImpl& aCtxImpl,
        TScanMode aMode,                    
        const TSSID& aSSID,                 
        TUint32 aScanRate,                    
        SChannels& aChannels,
        TUint32 aMinChannelTime,            
        TUint32 aMaxChannelTime,
        TBool aSplitScan );                                  

    /**
    * Called upon receiving a beacon type frame
    * @param aCtxImpl statemachine context
    * @param aRcpi RCPI of the frame
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
    virtual void OnBeaconFrameRx( 
        WlanContextImpl& aCtxImpl,
        const TAny* aFrame,
        const TUint32 aLength,
        WHA::TRcpi aRcpi,
        TUint8* aBuffer );
                
    // Prohibit copy constructor.
    WlanDot11IbssNormalMode( const WlanDot11IbssNormalMode& );
    // Prohibit assigment operator.
    WlanDot11IbssNormalMode& operator= ( const WlanDot11IbssNormalMode& );

private: // data
    
    /**
     * name of the state
     */
    static const TInt8  iName[];
    };

#endif      // C_WLANDOT11IBSSNORMALMODE_H
