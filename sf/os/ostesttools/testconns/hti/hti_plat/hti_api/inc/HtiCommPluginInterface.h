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
* Description:  Interface defenition for communication plugins.
*        Implementation is constructed by matching string.
*        The default implementation is FBus plugin.
*
*/


#ifndef HTICOMMPLUGININTERFACE_H__
#define HTICOMMPLUGININTERFACE_H__

#include <ecom/ecom.h>

#ifndef __WINS__
_LIT8( KCommDefaultImplementation, "USBSERIAL" );
#else
_LIT8( KCommDefaultImplementation, "SERIAL" );
#endif

const TUid KHTICommInterfaceUid = { 0x1020DEB8 };

/**
*  Completion code for Receive() and Send(), when underlying communication
*  module has reinitialized, i.e. cancelled any send and receive operations.
*/
const TInt KErrComModuleReset = 100;


class CHTICommPluginInterface : public CBase
    {
public: //ECom specific methods (implemented as inline)

    /**
    * Wraps ECom object instantitation
    * default resovler is used.
    * Plug-in specified by KCommDefaultImplementation
    * is loaded.
    */
    static CHTICommPluginInterface* NewL();

    /**
    * Wraps ECom object instantitation
    * default resovler is used
    *
    * @param aMatchString plug-in name as specified in
    *        in plug-in implementation resource file
    */
    static CHTICommPluginInterface* NewL(const TDesC8& aMatchString);

    /**
    * Wraps ECom object destruction
    */
    virtual ~CHTICommPluginInterface();


public: //Service plugin interface methods

    /**
    * Receive a message to the provided descriptor.
    * It is possible that the descriptor will not be full on request
    *   complition.
    *
    * @param aMessage Buffer for receiving incoming message
    * @param aStatus AO's status
    */
    virtual void Receive( TDes8& aMessage, TRequestStatus& aStatus ) = 0;

    /**
    * Send a message.
    *
    * @param aMessage Buffer with data for sending
    * @param aStatus AO's status
    */
    virtual void Send(const TDesC8& aMessage, TRequestStatus& aStatus ) = 0;

    /**
    * Cancels receive operation
    */
    virtual void CancelReceive() = 0;
    /**
    * Cancels send operation
    */
    virtual void CancelSend() = 0;

    /**
    *   Retruns the size of a buffer used for
    *   Send() function.
    **/
    virtual TInt GetSendBufferSize() = 0;

    /**
    *   Retruns the size of a buffer used for
    *   Receive() function.
    **/
    virtual TInt GetReceiveBufferSize() = 0;

private:
    /**
    * Instance identifier key
    */
    TUid iDtor_ID_Key;
    };

#include <HtiCommPluginInterface.inl>

#endif
