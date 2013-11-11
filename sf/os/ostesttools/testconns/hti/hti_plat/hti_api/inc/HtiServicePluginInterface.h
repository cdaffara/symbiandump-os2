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
* Description:  Interface defenition for HTI services
*
*/


#ifndef SERVICEPLUGININTERFACE_H__
#define SERVICEPLUGININTERFACE_H__

#include <ecom/ecom.h>

#include <HtiDispatcherInterface.h>

const TUid KHTIServiceInterfaceUid = {0x1020DEB7};


class CHTIServicePluginInterface : public CBase, public MHtiMemoryObserver
    {
public: //ECom specific methods (implemented as inline)

    /**
    * Wraps ECom object instantitation
    * plug-in loaded by its UID
    *
    * @param aUID service plugin implementation UDI
    */
    static CHTIServicePluginInterface* NewL(const TUid aUID);

    /**
    * Wraps ECom object instantitation
    * default resovler is used
    *
    * @param aMatchString service plugin name
    */
    static CHTIServicePluginInterface* NewL(const TDesC8& aMatchString);

    /**
    * Wraps ECom object destruction
    */
    virtual ~CHTIServicePluginInterface();

public: //Comm plugin interface methods

    /**
    * This method is called by framework after the plugin was loaded and
    * iDispatcher set.
    */
    virtual void InitL(){};

    /**
    * Result indicates eiterh plug-in is able to process request
    * (e.g. a plug-in doesn't have an outstanding request to async service).
    * Framework calls this method before calling ProcessMessageL().
    * If plug-in is busy, request will be dispatched later.
    * Typically, AO's CActive::IsActive() can be used
    * in a overriden implementation.
    * Default implementation returns EFalse.
    */
    virtual TBool IsBusy();

    /**
    * Interface service
    * implementation of service, should return ASAP.
    * aMessage contan is service specific.
    *
    * @param aMessage message body destinated to a servive
    * @param aPriority message priority
    */
    virtual void ProcessMessageL(const TDesC8& aMessage,
                                THtiMessagePriority aPriority) = 0;

    virtual void Error(TInt /*aErrorCode*/, const TDesC8& /*aErrorDescription*/){};

    /**
    * Set dispatcher that used by services to send messages.
    *
    */
    virtual void SetDispatcher(MHtiDispatcher* aDispatcher);

public: //from MHtiMemoryObserver
    /**
    * Empty implementation.
    */
    virtual void NotifyMemoryChange(TInt /*aAvailableMemory*/){};

protected:
    /**
    * Pointer to a dispatcher. It is not availble during construction.
    * If it should be used before starting process messages, InitL()
    * must be overriden.
    */
    MHtiDispatcher* iDispatcher;

private:
    /**
    * Instance identifier key
    */
    TUid iDtor_ID_Key;
    };

#include <HtiServicePluginInterface.inl>

#endif
