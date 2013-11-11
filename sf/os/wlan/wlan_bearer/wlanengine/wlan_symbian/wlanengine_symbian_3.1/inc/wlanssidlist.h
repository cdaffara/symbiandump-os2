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
* Description:  Class implementing storage for an SSID list.
*
*/


#ifndef WLANSSIDLIST_H
#define WLANSSIDLIST_H

#include <e32std.h>
#include <e32base.h>
#include <s32strm.h>
#include <wlanmgmtcommon.h>

/**
 * This class implements storage for an SSID list.
 *
 * @since S60 v5.1
 */
NONSHARABLE_CLASS( CWlanSsidList ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * 
     * @param aGranularity Granularity to use when allocating memory.
     */
    static CWlanSsidList* NewL(
        TUint aGranularity );

    /**
     * Destructor.
     */
    virtual ~CWlanSsidList();

    /**
     * Add an SSID to the list.
     * 
     * @since S60 v5.1
     * @param aSsid SSID to add.
     * @return KErrNone on success, an error code otherwise.
     */
    TInt AddSsid(
        const TWlanSsid& aSsid );

    /**
     * Check whether the given SSID is in the list.
     *
     * @since S60 v5.1
     * @param aSsid SSID to search for.
     * @return ETrue if SSID in the list, EFalse otherwise.
     */
    TBool IsInList(
        const TWlanSsid& aSsid ) const;    

    /**
     * Return the amount SSIDs in the list.
     * 
     * @return The amount SSIDs in the list.
     */
    TUint Count() const;

    /**
     * Write the contents of the SSID list to the given stream.
     *
     * @since S60 v5.1
     * @param aStream Stream to write to.
     */
    void ExternalizeL(
        RWriteStream& aStream ) const;

    /**
     * Read the contents of the SSID list from the given stream.
     * 
     * @since S60 v5.1
     * @param aStream Stream to read from.
     */
    void InternalizeL(
        RReadStream& aStream );    

private:

    /**
     * Constructor.
     *
     * @param aGranularity Granularity to use when allocating memory.
     */
    CWlanSsidList(
        TUint aGranularity );

    /**
     * 2nd state constructor.
     */
    void ConstructL();

    /**
     * Determine the linear order of the two SSIDs.
     * 
     * @param aLeft First SSID to compare.
     * @param aRight Second SSID to compare.
     * @return Zero if SSIDs are equal, negative if the first one is less,
     *         posive if the first one is greater.
     */
    static TInt LinearOrder(
        const TWlanSsid& aLeft,
        const TWlanSsid& aRight );

private: // data

    /**
     * List of SSIDs.
     */
    RArray<TWlanSsid> iSsidList;

    /**
     * Function used to determine the order of two SSIDs.
     */
    TLinearOrder<TWlanSsid> iLinearOrder; 

    };

#endif // WLANSSIDLIST_H
