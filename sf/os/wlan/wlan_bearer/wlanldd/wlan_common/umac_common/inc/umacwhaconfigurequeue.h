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
* Description:   Declaration of the WlanWhaConfigureQueue class.
*
*/

/*
* %version: 9 %
*/

#ifndef T_WLANWHACONFIGUREQUEUE_H
#define T_WLANWHACONFIGUREQUEUE_H

#include "UmacWsaCommand.h"

/**
 *  Encapsulates the execution of ConfigureQueue WHA command.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanWhaConfigureQueue : public WlanWsaCommand
    {

public:
    
    WlanWhaConfigureQueue() : 
        iQueueId( static_cast<WHA::TQueueId>(0) ), 
        iMaxLifeTime( 0 ), 
        iPsScheme( static_cast<WHA::TPsScheme>(0) ), 
        iAckPolicy( static_cast<WHA::TAckPolicy>(0) ),
        iMediumTime( 0 ) {};

    virtual ~WlanWhaConfigureQueue() {};

    /**
     * ?description
     *
     * @since S60 3.1
     * @param aQueueId
     * @param aMaxLifeTime
     * @param aPsScheme
     * @param aAckPolicy
     * @param aMediumTime
     */
    void Set( 
        WHA::TQueueId aQueueId,
        TUint32 aMaxLifeTime,
        WHA::TPsScheme aPsScheme,
        WHA::TAckPolicy aAckPolicy,
        TUint16 aMediumTime );

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
    WlanWhaConfigureQueue( const WlanWhaConfigureQueue& );
    // Prohibit assigment operator.
    WlanWhaConfigureQueue& operator= ( const WlanWhaConfigureQueue& );

private: // data

#ifndef NDEBUG
    /**
     * ?description_of_member
     */
    static const TInt8  iName[];
#endif

    WHA::TQueueId   iQueueId;
    TUint32         iMaxLifeTime;
    WHA::TPsScheme  iPsScheme;
    WHA::TAckPolicy iAckPolicy;
    TUint16         iMediumTime;
    };

#endif // T_WLANWHACONFIGUREQUEUE_H
