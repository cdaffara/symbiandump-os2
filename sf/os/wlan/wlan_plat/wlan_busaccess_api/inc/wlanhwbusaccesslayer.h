/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  The file contains a structure definition for accessing
*               HW bus adaptation modules (SPIA or SDIOA) in WLAN
*               adaptation code.
*
*/

/*
* %version: 5 %
*/

#ifndef WLANHWBUSACCESSLAYER_H
#define WLANHWBUSACCESSLAYER_H

#include <wlanosaplatform.h>

class WlanSpia;
class WlanSdioa;

/**
 *  
 *
 *
 *  @lib wlanpdd.pdd
 *  @since S60 v3.2
 */
struct SHwBusAccessLayer
    {
    WlanSpia* iSpia;
    WlanSdioa* iSdioa;

    SHwBusAccessLayer() : iSpia( NULL ), iSdioa( NULL ) {};
    
private:

    // Prohibit copy constructor.
    SHwBusAccessLayer( const SHwBusAccessLayer& );
    // Prohibit assigment operator.
    SHwBusAccessLayer& operator= ( const SHwBusAccessLayer& );
    };

#endif // WLANHWBUSACCESSLAYER_H
