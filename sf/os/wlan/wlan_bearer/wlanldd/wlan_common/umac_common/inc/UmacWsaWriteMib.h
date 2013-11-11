/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanWsaWriteMib class.
*
*/

/*
* %version: 8 %
*/

#ifndef WLANWSAWRITEMIB_H
#define WLANWSAWRITEMIB_H

#include "UmacWsaCommand.h"

/**
*  WHA WriteMib command object encapsulation
*  
*
*  @lib wlanumac.lib
*  @since S60 3.1
*/
class WlanWsaWriteMib : public WlanWsaCommand
    {

public:

    /**
    * C++ default constructor.
    */
    WlanWsaWriteMib() : 
        iDfcPending( EFalse ),
        iMib( static_cast<WHA::TMib>(0) ),
        iLength( 0 ),
        iData( NULL ) {};

    /**
    * Destructor.
    */
    virtual inline ~WlanWsaWriteMib();

    inline void Set( 
        WlanContextImpl& aCtxImpl, 
        WHA::TMib aMib,
        TUint32 aLength,
        const TAny* aData );

private:

    virtual void Entry( WlanContextImpl& aCtxImpl );
    virtual void Exit( WlanContextImpl& aCtxImpl);

    virtual void CommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams );

    virtual void OnDfc( TAny* aCtx );

#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    inline void RegisterDfc();
    inline void CancelDfc();
    inline void DfcPending( TBool aValue );
    inline TBool DfcPending() const;

    // Prohibit copy constructor.
    WlanWsaWriteMib( const WlanWsaWriteMib& );
    // Prohibit assigment operator.
    WlanWsaWriteMib& operator= ( const WlanWsaWriteMib& );

private:    // Data

#ifndef NDEBUG 
    static const TInt8  iName[];
#endif

    TBool               iDfcPending;
    WHA::TMib           iMib;
    TUint32             iLength;
    /**
    * Pointer to MIB data
    * Not own
    */
    const TAny*         iData;
    };

#include "UmacWsaWriteMib.inl"

#endif      // WLANWSAWRITEMIB_H
