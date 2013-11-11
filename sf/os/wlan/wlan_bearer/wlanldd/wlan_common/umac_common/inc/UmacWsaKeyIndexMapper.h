/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanWsaKeyIndexMapper class
*
*/

/*
* %version: 12 %
*/

#ifndef WLANWSAKEYINDEXMAPPER_H
#define WLANWSAKEYINDEXMAPPER_H

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif

/**
*  Maps an encryption key index to WHA key entry index.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanWsaKeyIndexMapper
    {
    enum { KWepPairWiseKey   = 4 };
    enum { KTkipPairWiseKey  = 5 };
    enum { KTkipGroupKey     = 6 };
    enum { KAesGroupKey      = 7 };
    enum { KAesPairWiseKey   = 8 };
    enum { KWapiPairWiseKey  = KTkipPairWiseKey };
    enum { KWapiGroupKey     = KTkipGroupKey };

    public:

        static TUint8 Extract( 
            WHA::TKeyType aKeyType, 
            TUint32 aDefaultKeyNumber = 0 );

    private:

        static inline TUint32 HandleWepPairwiseKey();
        static inline TUint32 HandleWepGroupKey( TUint32 aDefaultKeyNumber );
        static inline TUint32 HandleTkipGroupKey();
        static inline TUint32 HandleTkipPairWiseKey();
        static inline TUint32 HandleAesPairwiseKey();
        static inline TUint32 HandleAesGroupKey();
        static inline TUint32 HandleWapiPairwiseKey();
        static inline TUint32 HandleWapiGroupKey();
    };

#endif  // WLANWSAKEYINDEXMAPPER_H
