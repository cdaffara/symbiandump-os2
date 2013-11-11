/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECOM plugin to communicate over BT serial port
*
*/


#ifndef CHTIBTCOMMECOMPLUGIN_H
#define CHTIBTCOMMECOMPLUGIN_H

class CBtSerialClient;

// INCLUDES
#include <c32comm.h>
#include <HtiCommPluginInterface.h> // defined in HtiFramework project

#include "HtiBtCommInterface.h" // RHtiBtCommInterface

// FORWARD DECLARATIONS
class CHtiCfg;

// CLASS DECLARATION
/**
* ECOM plugin module for communicating with PC using serial port.
*
*/
class CHtiBtCommEcomPlugin :
    public CHTICommPluginInterface
    {
public:

    /**
    * Create instance of plugin.
    * @return Connected plugin instance.
    */
    static CHtiBtCommEcomPlugin* NewL();

    /**
    * Destructor
    */
    ~CHtiBtCommEcomPlugin();

public: // CHTICommPluginInterface interface implementation

    /**
    * Receive data from Bluetooth.
    * The size of supplied buffer must equal to size given by GetBufferSize.
    * The number of received bytes may be anything between 1 and GetBufferSize.
    *
    * @param aRawdataBuf Buffer where the result is written
    * @param aStatus Request status
    */
    void Receive( TDes8& aRawdataBuf, TRequestStatus& aStatus );

    /**
    * Send data to Bluetooth. The size of data must not exceed GetBufferSize.
    * @param aRawdataBuf Buffer where the data is read from.
    * @param aStatus Request status
    */
    void Send( const TDesC8& aRawdataBuf, TRequestStatus& aStatus );

    /**
    * Cancel read operation
    */
    void CancelReceive();

    /**
    * Cancel send operation
    */
    void CancelSend();

    /**
     *  Return required buffer size for Send operation.
     */
    TInt GetSendBufferSize();

    /**
     *  Return required buffer size for Receive operation.
     */
    TInt GetReceiveBufferSize();

private:

    /**
    * Constructor of this plugin.
    */
    CHtiBtCommEcomPlugin();

    /**
    * Second phase construction. Connects the Bluetooth.
    */
    void ConstructL();

    /**
    * Load configuration file.
    */
    void LoadConfigL();

    /**
    * Read and parse the configuration values.
    */
    void ReadConfig();

    /**
     * Parse the possible port (channel) number value from name or address param
     */
     TInt ParsePortNumber();

     /**
     * StorePortNumberL()
     * Store the given port number to configuration file
     * @param aPortNumber the port number to store
     */
     void StorePortNumberL( TInt aPortNumber );

private: // Data

    RHtiBtCommInterface iBtCommInterface;

    CHtiCfg* iCfg;
    HBufC8*  iBtDeviceNameOrAddress;
    TInt     iPort;

    };

#endif // CHTIBTCOMMECOMPLUGIN_H
