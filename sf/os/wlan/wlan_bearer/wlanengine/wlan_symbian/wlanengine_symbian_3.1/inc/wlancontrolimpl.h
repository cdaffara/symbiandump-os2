/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class implementing MWlanControlInterface.
*
*/


#ifndef WLANCONTROLIMPL_H
#define WLANCONTROLIMPL_H

#include "rwlmserver.h"
#include "wlancontrolclient.h"

/**
 * Implementation for MWlanControlInterface interface.
 *
 * @lib wlancontrolimpl.dll
 * @since S60 v3.2
 */
class CWlanControlImpl : public CWlanControlClient
    {

public:

    /**
     * Static constructor.
     */
    static CWlanControlImpl* NewL();
        
    /**
     * Destructor.
     */
    virtual ~CWlanControlImpl();

// from base class CWlanControlClient

    /**
     * From CWlanControlImpl.
     * Get packet statistics of the current connection.
     *
     * @since S60 v3.2
     * @param aStatistics Packet statistics of the current connection.
     * @return KErrNone if statistics were read successfully, an error otherwise.
     */
    virtual TInt GetPacketStatistics(
        TWlanPacketStatistics& aStatistics );

    /**
     * From CWlanControlImpl.
     * Clear packet statistics of the current connection.
     *
     * @since S60 v3.2
     */        
    virtual void ClearPacketStatistics();

    /**
     * From CWlanControlImpl.
     * Get the current U-APSD settings.
     *
     * @since S60 v3.2
     * @param aSettings Current U-APSD settings.
     * @return KErrNone if settings were read successfully, an error otherwise.
     */
    virtual TInt GetUapsdSettings(
        TWlanUapsdSettings& aSettings );

    /**
     * From CWlanControlImpl.
     * Set the U-APSD settings.
     *
     * @since S60 v3.2
     * @param aSettings Current U-APSD settings to be set.
     * @return KErrNone if settings were set successfully, an error otherwise.
     */
    virtual TInt SetUapsdSettings(
        const TWlanUapsdSettings& aSettings );

    /**
     * Get the current power save settings.
     *
     * @since S60 v3.2
     * @param aSettings power save settings.
     * @return KErrNone if settings were read successfully, an error otherwise.
     */
    virtual TInt GetPowerSaveSettings(
        TWlanPowerSaveSettings& aSettings );

    /**
     * Set the power save settings.
     *
     * @since S60 v3.2
     * @param aSettings Current power save settings to be set.
     * @return KErrNone if settings were set successfully, an error otherwise.
     */      
    virtual TInt SetPowerSaveSettings(
        const TWlanPowerSaveSettings& aSettings );

    /**
     * Get information about the current AP.
     *
     * @since S60 v3.2
     * @param aInfo Information about the current AP.
     * @return KErrNone if information is available, an error otherwise.
     */
    virtual TInt GetAccessPointInfo(
        TWlanAccessPointInfo& aInfo );

    /**
     * Get roam metrics of the current connection.
     *
     * @since S60 v3.2
     * @param aRoamMetrics Roam metrics of the current connection.
     * @return KErrNone if settings were read successfully, an error otherwise.
     */
    virtual TInt GetRoamMetrics(
        TWlanRoamMetrics& aRoamMetrics );

    /**
     * Get a list of BSSIDs on the rogue list.
     *
     * @since S60 v3.2
     * @param aRogueList List of BSSIDs on the rogue list.
     * @return KErrNone if list were read successfully, an error otherwise.
     */
    virtual TInt GetRogueList(
        CArrayFixSeg<TWlanBssid>& aRogueList );

    /**
     * Get the current regulatory domain.
     *
     * @since S60 v3.2
     * @param aRegion current region.
     * @return KErrNone if value was read successfully, an error otherwise.
     */
    virtual TInt GetRegulatoryDomain(
        TWlanRegion& aRegion );
    
    /**
     * Get the current power save mode.
     *
     * @since S60 v3.2
     * @param aPowerSaveMode current power save mode.
     * @return KErrNone if value was read successfully, an error otherwise.
     */
    virtual TInt GetPowerSaveMode(
        TWlanPowerSave& aPowerSaveMode );
    
    /**
     * Set the power save mode.
     *
     * @note This method is meant for overriding the dynamic power save mode selection
     *       for testing purposes. Overriding the power save mode may have an adverse
     *       effect on throughput and/or power consumption. Dynamic selection can be
     *       re-enabled by setting the mode to EWlanPowerSaveModeAutomatic.
     *
     * @since S60 v5.0.1
     * @param aMode Power save mode to be set.
     * @return KErrNone if the mode was set successfully, an error otherwise.
     */
    virtual TInt SetPowerSaveMode(
        TWlanPowerSaveMode aMode );    

private:

    /**
     * C++ default constructor.
     */
    CWlanControlImpl();

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

private: // data

    /**
     * Handle to WLAN engine.
     */
    RWLMServer iServer;

    };

#endif // WLANCONTROLIMPL_H
