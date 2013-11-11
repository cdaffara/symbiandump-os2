/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

/** @file
@internalComponent
*/


#ifndef NCMCLIENTMANAGER_H
#define NCMCLIENTMANAGER_H

#include <e32std.h>
#include <e32base.h>

#ifdef OVERDUMMY_NCMCC
#include <dummyusbsclddapi.h>
#else
#include <d32usbcsc.h>
#endif // OVERDUMMY_NCMCC

#include "ncmcommon.h"

class RNcmInternalSrv;

/**
 * A class which encapsulates the operations related to USB shared chunk LDD
 */
class CNcmClientManager : public CBase
    {
public:
    /**
     * Constructor
     * @param[in]  aHostMacAddress, the NCM interface MAC address 
     *             which will be used to config.
     */
    CNcmClientManager(const TNcmMacAddress& aHostMacAddress);
    
    ~CNcmClientManager();

    /**
     * Setup NCM interfaces
     * @param[out]   aDataEpBufferSize, NCM data interface EP buffer size
     */
    void SetNcmInterfacesL(TUint& aDataEpBufferSize);
    /**
     * Transfer ownership of NCM interface handles to NCM internal 
     * server which is located in C32 process space
     * @param[in]   aServer, NCM internal server. 
     */
    void TransferInterfacesL(RNcmInternalSrv& aServer);
        
private:
    /**
     * Setup NCM communication interface
     */
    void SetCommunicationInterfaceL();
    /**
     * Setup NCM data interface
     * @param   aDataEpBufferSize, NCM data interface EP buffer size
     */
    void SetDataInterfaceL(TUint& aDataEpBufferSize);
    /**
     * Setup NCM class specific descriptors
     * @param   aDataInterfaceNumber, NCM data interface number
     */
    TInt SetupClassSpecificDescriptor(TUint8 aControlInterfaceNumber, TUint8 aDataInterfaceNumber);
    /**
     * Set NCM interface MAC address into NCM class specific descriptor
     * @param [out]   aStrIndex, the index of string which contains NCM 
     *               interface MAC address 
     */
    TInt SetMacAddressString(TUint8& aStrIndex);
    
    /**
     * Get interface number
     * @param aLdd The logic device driver which can be used to query for.
     * @param aSettingsNumber The alter settings for the interface
     * @param [out] aInterfaceNumber Carry out the interface bumber back to caller.
     * @return KErrNone If interface number is return via aInterfaceNumber.
     *                  Other system wide error code if anything went wrong.
     */
    TInt InterfaceNumber(RDevUsbcScClient& aLdd,TInt aSettingsNumber,TUint8& aInterfaceNumber);
    
private:
    const TNcmMacAddress&   iHostMacAddress;
    
    TBool                   iCommLddInitiated;
    RDevUsbcScClient        iCommLdd;
    TBool                   iDataLddInitiated;
    RDevUsbcScClient        iDataLdd;
    };

#endif // NCMCLIENTMANAGER_H
