/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementing a file-based storage for SSID lists.
*
*/


#ifndef WLANSSIDLISTDB_H
#define WLANSSIDLISTDB_H

#include <e32std.h>
#include <e32base.h>
#include <s32std.h>
#include "wlanssidlist.h"

/**
 * This class implements a file-based storage for SSID lists.
 *
 * @since S60 v5.1
 */
NONSHARABLE_CLASS( CWlanSsidListDb ) : public CBase
    {

public:

    /**
     * This class stores information about SSID list mappings.
     */
    class TWlanSsidListMapEntry
        {
    public:

        /**
         * Constructor.
         */
        TWlanSsidListMapEntry(
            TUint aIap ) : iIap( aIap ), iStreamId( KNullStreamIdValue ) { }

    public: // data

        /** 
         * IAP ID this list is attached to.
         * */
        TUint iIap;

        /** 
         * Stream ID for file storage. 
         */
        TStreamId iStreamId;

        };

    /**
     * Two-phased constructor.
     */
    static CWlanSsidListDb* NewL();

    /**
     * Destructor.
     */
    virtual ~CWlanSsidListDb();

    /**
     * Store the given SSID list to disk.
     *
     * @since S60 v5.1
     * @param aIap IAP ID the list is attached to.
     * @param aSsidList SSID list to store.
     */
    void WriteListL(
        TUint aIap,
        const CWlanSsidList& aSsidList );

    /**
     * Read a SSID list from disk.
     * 
     * @since S60 v5.1
     * @param aIap IAP ID the list is attached to.
     * @param aSsidList SSID list to store contents into.
     */    
    void ReadListL(
        TUint aIap,        
        CWlanSsidList& aSsidList );     

    /**
     * Delete an SSID list from disk.
     * 
     * @since S60 v5.1
     * @param aIap IAP ID the list is attached to.
     */
    void DeleteListL(
        TUint aIap );

    /**
     * Check whether an IAP has an SSID list attached.
     *
     * @since S60 v5.1
     * @param aIap IAP ID the list is attached to.
     * @return ETrue if an list exists, EFalse otherwise.
     */
    TBool IsListAttached(
        TUint aIap );    

private:

    /**
     * Constructor.
     */
    CWlanSsidListDb();

    /**
     * 2nd state constructor.
     */
    void ConstructL();

private: // data

    /**
     * List of SSID list mapping entries.
     */
    RArray<TWlanSsidListMapEntry> iSsidListMap;    

    };

#endif // WLANSSIDLISTDB_H
