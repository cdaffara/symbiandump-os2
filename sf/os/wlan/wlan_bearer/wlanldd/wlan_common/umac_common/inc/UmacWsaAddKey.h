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
* Description:   Declaration of the WlanWsaAddKey class.
*
*/

/*
* %version: 10 %
*/

#ifndef WLANWSAADDKEY_H
#define WLANWSAADDKEY_H

#include "UmacWsaCommand.h"

/**
*  Encapsulates the execution of Release WHA command.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanWsaAddKey : public WlanWsaCommand
    {
    
public:

    /**
    * C++ default constructor.
    */
    WlanWsaAddKey() : 
        iKeyType( static_cast<WHA::TKeyType>(0) ),
        iKey( 0 ),
        iEntryIndex( 0 ) {};

    /**
    * Destructor.
    */
    virtual inline ~WlanWsaAddKey();

    inline void Set( 
        WlanContextImpl& aCtxImpl, 
        WHA::TKeyType aKeyType,
        const TAny* aKey,
        TUint8 aEntryIndex );

private:

    virtual void Entry( WlanContextImpl& aCtxImpl );
#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    // Prohibit copy constructor.
    WlanWsaAddKey( const WlanWsaAddKey& );
    // Prohibit assigment operator.
    WlanWsaAddKey& operator= ( const WlanWsaAddKey& );

    virtual void CommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams );

private:    // Data

    WHA::TKeyType       iKeyType;
    /**
    * cipher key to add
    * Not own
    */
    const TAny*         iKey;
    TUint8              iEntryIndex;

#ifndef NDEBUG 
    static const TInt8  iName[];
#endif
    };

#include "UmacWsaAddKey.inl"

#endif      // WLANWSAADDKEY_H
