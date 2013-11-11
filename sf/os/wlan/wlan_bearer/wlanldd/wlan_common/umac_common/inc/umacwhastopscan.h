/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanWhaStopScan class.
*
*/

/*
* %version: 3 %
*/

#ifndef WLAN_WHA_STOP_SCAN_H
#define WLAN_WHA_STOP_SCAN_H

#include "UmacWsaCommand.h"

/**
 *  Encapsulates the execution of StopScan WHA command.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.2
 */
class WlanWhaStopScan : public WlanWsaCommand
    {

public:
    
    WlanWhaStopScan() {};

    virtual ~WlanWhaStopScan() {};

private:

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.2
     * @param ?arg1 ?description
     */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.2
     * @param ?arg1 ?description
     */
    virtual void CommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams );

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.2
     * @param ?arg1 ?description
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    // Prohibit copy constructor.
    WlanWhaStopScan( const WlanWhaStopScan& );
    // Prohibit assigment operator.
    WlanWhaStopScan& operator= ( const WlanWhaStopScan& );

private: // data

#ifndef NDEBUG
    /**
     * name of the state
     */
    static const TInt8  iName[];
#endif 
    };

#endif // WLAN_WHA_STOP_SCAN_H
