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


/**
@file
@publishedPartner
*/

#ifndef USBNCM_H
#define USBNCM_H

//INCLUDES
#include <e32std.h>

/** Defines the exported P&S key and data structure of the value. */
namespace UsbNcm
    {
    /** The property's category UID.
     *  This property is to notify the NCM connection state.
     */
    static const TUid  KCatNcmSvrUid = {0x101F7989};

    /** The property's sub-key.*/
    static const TUint KKeyNcmConnectionEvent = 0x2002C33F;


    /** NCM connection state.*/
    enum TNcmConnectionState
      {
      ENcmStateDisconnected = 0,
      ENcmStateConnected,
      ENcmStateMax
      };

    /**
     * Defines the NCM connection event, which contains the IapId, NCM connection state and corresponding error code.
     * The NCM connection is a special one which can not only be used by RConnection, but also needed to monitor the P&S to fetch its state changing.
     * Once the P&S published the ENcmStateConnected, the iIapId is also available for starting this connection.
     * Otherwise, if the P&S published the ENcmStateDisconnected, the iErrCode should be checked to notify the failure reason:
     *            KErrNone, normally disconnected probably because of USB cable unpluged.
     *            KErrInUse, means the other component(currently, BtPan or RNDIS) hold the other P&S key so that the DHCP Provision can't be done by NCM.
     *            system-wide error code, please check Developer Library.
     *            It is also possible that this P&S key is deleted when fetching its value because of the NCM connection closed by its driver.
     *
     * @see below sample code.
     */
    struct TNcmConnectionEvent
      {
      TUint32                iIapId;
      TNcmConnectionState    iState;
      TInt                   iErrCode;
      TUint8                 reserved[20];
      };
    } // namespace UsbNcm

/**
 Example Usage:
 @code
 //header file.
 #include <usb/usbncm.h>
 using namespace UsbNcm;

 CMySampleClass: public CActive
     {
     private:
        RProperty                      iProperty;
        RConnection                    iConnection;
        TPckgBuf<TNcmConnectionEvent>  iNcmEvent;
        RSocket                        iSocket;
     }
 @endcode

 @code
 #include "mysampleclass.h"

 void CMySampleClass::ContructL()
    {
    User::LeaveIfError(iProperty.Attach(KCatNcmSvrUid, KKeyNcmConnectionEvent, EOwnerThread));

    iProperty.Get(iNcmEvent);
    if (ENcmStateConnected == iNcmEvent.iState)
        {
        TRequestStatus* pStatus = &iStatus;
        iStatus = KRequestPending;
        User::RequestComplete(pStatus, KErrNone);
        }
    else
        {
        iProperty.Subscribe(iStatus);
        SetActive();
        }
    }

 void CMySampleClass::RunL()
    {
    if (KErrNone == iStatus.Int())
        {
        iProperty.Get(iNcmEvent);

        switch(iNcmEvent.iState)
            {
            case ENcmStateConnected:
                StartConnection();
                break;

            case ENcmStateDisconnected:
                if (KErrInUse == iNcmEvent.u.iErrCode)
                    {
                    //Show error UI with msg like "BtPan or Rndis is active, pls deactive it then re-try....", etc.
                    }
                else
                    {
                    //Show error UI with other msg as you like.
                    //StopConnection();
                    }
                break;
            }
        }
    }

 //Sample code, not all string in this panic show on.
 LIT(KNotFoundNcm, "NotFoundPanic - The NCM connection was not found!");

 TInt CMySampleClass::StartConnection()
    {
    //the number of connections.
    TInt cnt = 0;
    iConnection.EnumerateConnections(cnt);

    TInt index = 1;
    for (index = 1; index <= cnt; index++)
        {
        TPckgBuf<TConnectionInfo> info;
        iConnection.GetConnectionInfo(index, info);
        if (info().iIapId == iNcmEvent.iIapId)
            {
            iConnection.Attach(info, RConnection::EAttachTypeNormal);
            break;
            }
        }
    __ASSERT_ALWAYS(index <= cnt, User::Panic(KErrNotFound, KNotFoundNcm));

    TInt ret = iSocket.Open(...., iConnection);
    //.....
    return ret;
    }
 @endcode
*/
#endif // USBNCM_H
