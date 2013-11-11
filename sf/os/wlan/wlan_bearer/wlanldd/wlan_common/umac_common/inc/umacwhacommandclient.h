/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the MWlanWhaCommandClient class.
*
*/

/*
* %version: 8 %
*/

#ifndef M_WLANWHACOMMANDCLIENT_H
#define M_WLANWHACOMMANDCLIENT_H

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif

class WlanContextImpl;

/**
 *  WHA command callback interface.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class MWlanWhaCommandClient
    {

public:

    /**
     * Method called by WHA command object after it has received a correponding
     * commmad response event.
     * It is guarantedd by the frame work that response matches 
     * to the command send by the WHA command client. So it is not required
     * to check the aCommandId parameter for programming error 
     * (matches to the command send).
     * This method is called just prior state object transition is done,
     * so one must not make a state object transition inside this method
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     * @param aCommandId WHA command identifier
     * @param aStatus comamnd specific status code
     * @param aCommandResponseParams command specific response paramaters, if any
     * @param aAct Asynchronous Completion Token used by the WHA command client,
     *        returned by the caller of this method just as it was supplied
     */
    virtual void OnWhaCommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams,
        TUint32 aAct ) = 0;

    };

#endif // M_WLANWHACOMMANDCLIENT_H
