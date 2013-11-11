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
* Description:   Declaration of the WlanWsaComplexCommand class.
*
*/

/*
* %version: 13 %
*/

#ifndef WLANWSACOMPLEXCOMMAND_H
#define WLANWSACOMPLEXCOMMAND_H

#include "UmacMacActionState.h"


/**
*  Base class for complex WHA command objects
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanWsaComplexCommand : public WlanMacActionState
    {

public:

    /**
    * Destructor.
    */
    virtual ~WlanWsaComplexCommand() {};

    void Act( WlanContextImpl& aCtxImpl, TUint32 aAct ) const;
    
protected:

    /**
    * C++ default constructor.
    */
    WlanWsaComplexCommand() {};

    virtual TBool DoErrorIndication( 
        WlanContextImpl& aCtxImpl, 
        WHA::TStatus aStatus );

    virtual void OnWhaCommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams,
        TUint32 aAct );

    inline void TraverseToHistoryState( WlanContextImpl& aCtxImpl );        
    };

#include "UmacWsaComplexCommand.inl"

#endif      // WLANWSACOMPLEXCOMMAND_H
