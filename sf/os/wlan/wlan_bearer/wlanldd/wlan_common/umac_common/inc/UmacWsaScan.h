/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanWsaScan class.
*
*/

/*
* %version: 10 %
*/

#ifndef WLANWSASCAN_H
#define WLANWSASCAN_H

#include "UmacWsaCommand.h"

/**
*  Encapsulates the execution of Scan WHA command.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanWsaScan : public WlanWsaCommand
    {

public:

    /**
    * C++ default constructor.
    */
    WlanWsaScan() :
        iMaxTransmitRate( static_cast<WHA::TRate>(0)),
        iBand( static_cast<WHA::TBand>(0)),
        iNumOfChannels( 0 ),
        iChannels( NULL ),
        iScanType( static_cast<WHA::TScanType>(0)),
        iNumOfProbeRequests( 0 ),
        iSplitScan( EFalse ),
        iNumOfSSID( 0 ),
        iSsid( NULL )
        {};

    /**
    * Destructor.
    */
    virtual ~WlanWsaScan();

    void Set(             
        WlanContextImpl& aCtxImpl,
        TUint32 aMaxTransmitRate, 
        WHA::TBand aBand,
        TUint8 aNumOfChannels,
        const WHA::SChannels* aChannels, 
        WHA::TScanType aScanType,
        TUint8 aNumOfProbeRequests,
        TBool aSplitScan,
        TUint8 aNumOfSSID,
        const WHA::SSSID* aSSID );

private:

    virtual void Entry( WlanContextImpl& aCtxImpl );
#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    virtual void CommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams );

    // Prohibit copy constructor.
    WlanWsaScan( const WlanWsaScan& );
    // Prohibit assigment operator.
    WlanWsaScan& operator= ( const WlanWsaScan& );

private:    // Data

#ifndef NDEBUG 
    static const TInt8  iName[];
#endif

    WHA::TRate              iMaxTransmitRate;
    WHA::TBand              iBand;
    TUint8                  iNumOfChannels;
    /**
    * channels to scan
    * Not own
    */
    const WHA::SChannels*   iChannels;
    WHA::TScanType          iScanType;
    TUint8                  iNumOfProbeRequests;
    TBool                   iSplitScan;
    TUint8                  iNumOfSSID;
    /**
    * SSID to scan
    * Not own
    */
    const WHA::SSSID*       iSsid;        
    };

#endif      // WLANWSASCAN_H
