/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanWsaJoin class.
*
*/

/*
* %version: 10 %
*/

#ifndef WLANWSAJOIN_H
#define WLANWSAJOIN_H

#include "UmacWsaCommand.h"

class WlanContextImpl;

/**
*  Encapsulates the execution of Join WHA command.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanWsaJoin : public WlanWsaCommand
    {
    
public:

    /**
    * C++ default constructor.
    */
    WlanWsaJoin();

    /**
    * Destructor.
    */
    virtual ~WlanWsaJoin() {};

public:

    void Set( 
        WlanContextImpl& aCtxImpl,
        WHA::TOperationMode aMode,
        WHA::TMacAddress& aBSSID,
        WHA::TBand aBand,
        WHA::SSSID& aSSID,
        WHA::TChannelNumber aChannel,
        TUint32 aBeaconInterval,
        WHA::TRate aBasicRateSet,
        TUint16 aAtimWindow,
        WHA::TPreamble aPreambleType,
        TBool aProbeForJoin );                                   

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
    WlanWsaJoin( const WlanWsaJoin& );
    // Prohibit assigment operator.
    WlanWsaJoin& operator= ( const WlanWsaJoin& );

private:    // Data

#ifndef NDEBUG 
    static const TInt8  iName[];
#endif

    WHA::TOperationMode     iMode;
    WHA::TMacAddress        iBSSID;
    WHA::SSSID              iSSID;
    WHA::TBand              iBand;
    WHA::TChannelNumber     iChannel;
    TUint32                 iBeaconInterval;
    WHA::TRate              iBasicRateSet;
    TUint16                 iAtimWindow;
    WHA::TPreamble          iPreambleType;
    TBool                   iProbeForJoin;
    };

#endif  // WLANWSAJOIN_H
