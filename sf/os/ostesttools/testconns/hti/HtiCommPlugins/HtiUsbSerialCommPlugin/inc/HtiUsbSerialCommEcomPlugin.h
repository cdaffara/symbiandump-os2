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
* Description:  ECOM plugin to communicate over USB serial emulation
*
*/


#ifndef CHTIUSBSERIALCOMMECOMPLUGIN_H
#define CHTIUSBSERIALCOMMECOMPLUGIN_H

// INCLUDES
#include <c32comm.h>
#include <HtiCommPluginInterface.h> // defined in HtiFramework project

// FORWARD DECLARATIONS
class CHtiCfg;

// CLASS DECLARATION
/**
* ECOM plugin module for communicating with PC using USB serial emulation.
*
*/
class CHtiUsbSerialCommEcomPlugin :
    public CHTICommPluginInterface
    {
public:

    /**
    * Create instance of plugin.
    * Reads configuration, opens and initializes USB port.
    * @return Plugin instance.
    */
    static CHtiUsbSerialCommEcomPlugin* NewL();

    /**
    * Destructor.
    */
    ~CHtiUsbSerialCommEcomPlugin();

public: // Interface implementation

    /**
    * Receive data from USB serial port.
    * The size of supplied buffer must equal to size given
    * by GetReceiveBufferSize.
    * The number of received bytes may be anything between 1
    * and GetReceiveBufferSize.
    *
    * @param aRawdataBuf Buffer where the result is written
    * @param aStatus Request status
    */
    void Receive( TDes8& aRawdataBuf, TRequestStatus& aStatus );

    /**
    * Send data to USB serial port. The size of data must not
    * exceed GetSendBufferSize.
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
    CHtiUsbSerialCommEcomPlugin();

    /**
    * Second phase construction.
    */
    void ConstructL();

    /**
    * Load needed drivers and start comm server.
    */
    void InitCommServerL();

    /**
    * Setup USB serial port.
    */
    void InitCommPortL();

    /**
    * Load configuration file.
    */
    void LoadConfigL();

    /**
    * Read and parse the configuration values.
    */
    void ReadConfig();

    /**
    * Helper to get data rate capability bitmask.
    */
    TUint RateCapsBitmaskFromRate( TBps aDataRate );

    /**
    * Show error notifier dialog with text and error code.
    */
    void ShowErrorNotifierL( const TDesC& aText, TInt aErr );

private: // Data

    CHtiCfg* iCfg;

    // port settings
    TInt      iPortNumber;
    TBps      iDataRate;
    TParity   iParity;
    TDataBits iDataBits;
    TStopBits iStopBits;

    // bitmask for handshake
    TInt iHandshake;

    RCommServ iCommServ;
    RComm iCommPort;
    };

#endif // CHTIUSBSERIALCOMMECOMPLUGIN_H

// End of file

