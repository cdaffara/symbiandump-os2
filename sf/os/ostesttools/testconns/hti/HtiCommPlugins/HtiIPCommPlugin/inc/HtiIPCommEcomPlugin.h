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
* Description:  ECOM plugin to communicate over IP port
*
*/


#ifndef CHTIIPCOMMECOMPLUGIN_H
#define CHTIIPCOMMECOMPLUGIN_H


// INCLUDES
#include <HtiCommPluginInterface.h> // defined in HtiFramework project
#include "HtiIPCommServerClient.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* ECOM plugin module for communicating with PC using IP port.
*
*/
class CHtiIPCommEcomPlugin :
    public CHTICommPluginInterface
    {
public:

    /**
    * Create instance of plugin.
    * Reads configuration, opens and initializes IP connection.
    * @return Plugin instance.
    */
    static CHtiIPCommEcomPlugin* NewL();

    /**
    * Destructor.
    */
    ~CHtiIPCommEcomPlugin();

public: // Interface implementation

    /**
    * Receive data from IP port.
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
    * Send data to IP port. The size of data must not
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
    CHtiIPCommEcomPlugin();

    /**
    * Second phase construction.
    */
    void ConstructL();

    /**
    * Load configuration file.
    */
    void LoadConfigL();

private: // Data
    RHtiIPCommServer iHtiIPCommServer;
    };

#endif // CHTIIPCOMMECOMPLUGIN_H
