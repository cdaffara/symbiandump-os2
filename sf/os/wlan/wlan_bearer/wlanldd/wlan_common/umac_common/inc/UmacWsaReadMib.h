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
* Description:   Declaration of the WlanWsaReadMib class.
*
*/

/*
* %version: 8 %
*/

#ifndef WLANWSAREADMIB_H
#define WLANWSAREADMIB_H

#include "UmacWsaCommand.h"

/**
*  Encapsulates the execution of ReadMib WHA command.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanWsaReadMib : public WlanWsaCommand
    {
    
public:

    /**
    * C++ default constructor.
    */
    WlanWsaReadMib() : iMib( static_cast<WHA::TMib>(0) ) {};

    /**
    * Destructor.
    */
    virtual ~WlanWsaReadMib() {};

public:

    inline void Set( 
        WlanContextImpl& aCtxImpl, 
        WHA::TMib aMib );

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
    WlanWsaReadMib( const WlanWsaReadMib& );
    // Prohibit assigment operator.
    WlanWsaReadMib& operator= ( const WlanWsaReadMib& );

private:    // Data

    WHA::TMib               iMib;
#ifndef NDEBUG 
    static const TInt8  iName[];
#endif
    };

#include "UmacWsaReadMib.inl"

#endif      // WLANWSAREADMIB_H

