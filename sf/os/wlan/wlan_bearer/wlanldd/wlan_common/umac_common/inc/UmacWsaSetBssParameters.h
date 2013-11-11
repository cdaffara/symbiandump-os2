/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanWsaSetBssParameters class.
*
*/

/*
* %version: 8 %
*/

#ifndef WLANWSASETBSSPARAMETERS_H
#define WLANWSASETBSSPARAMETERS_H

#include "UmacWsaCommand.h"

/**
*  Encapsulates the execution of SetBssParameters WHA command.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanWsaSetBssParameters : public WlanWsaCommand
    {
    
public:

    /**
    * C++ default constructor.
    */
    WlanWsaSetBssParameters() : iDtim( 0 ), iAid( 0 ) {};

    /**
    * Destructor.
    */
    virtual ~WlanWsaSetBssParameters() {};

    void Set( 
        WlanContextImpl& aCtxImpl,
        TUint32 aDtim,
        TUint32 aAid );

private:

    virtual void Entry( WlanContextImpl& aCtxImpl );
#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    virtual void CommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams );

    // Prohibit copy constructor.
    WlanWsaSetBssParameters( const WlanWsaSetBssParameters& );
    // Prohibit assigment operator.
    WlanWsaSetBssParameters& operator= ( 
        const WlanWsaSetBssParameters& );

private:    // Data

#ifndef NDEBUG 
    static const TInt8  iName[];
#endif

    TUint32             iDtim;
    TUint32             iAid;
    };

#endif      // WLANWSASETBSSPARAMETERS_H
