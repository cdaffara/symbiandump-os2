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
* Description:   Declaration of the WlanDot11SharedAuthPending class
*
*/

/*
* %version: 14 %
*/

#ifndef WLANDOT11SHAREDAUTHPENDING_H
#define WLANDOT11SHAREDAUTHPENDING_H

#include "UmacDot11AuthenticatePending.h"

/**
*  A class that implements dot11 shared mode authentication frame exchange.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11SharedAuthPending : public WlanDot11AuthenticatePending
    {
public:

    /**
    * C++ default constructor.
    */
    WlanDot11SharedAuthPending() : 
        iLatestRxAuthRespPtr ( NULL ),
        iLatestRxAuthRespFlags ( 0 )
    {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11SharedAuthPending() { iLatestRxAuthRespPtr = NULL; };
    
private:    

    /**
    * Sends authentication sequence number 3 message
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @return ETrue upon success, 
    * EFalse if packet scheduler discarded the frame
    */
    TBool SendAuthSeqNbr3Frame( WlanContextImpl& aCtxImpl ) const;

    // Functions from base classes

    /**
    * From ?base_class ?member_description.
    * Sets the used authentication number to authenticate message
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void OnSetAlgorithmNumber( WlanContextImpl& aCtxImpl );

    /**
    * From ?base_class ?member_description.
    * Returns the states name
    * @since S60 3.1
    * @param aLength (OUT) length of the name of the state
    * @return name of the state
    */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    /**
    * From ?base_class ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @param aRcpi RCPI of the frame
    * @param aFlags flags from WHA frame receive method
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
    virtual void OnReceiveFrameSuccess(
        WlanContextImpl& aCtxImpl,
        const void* aFrame,
        TUint16 aLength,
        WHA::TRcpi aRcpi,
        TUint32 aFlags,
        TUint8* aBuffer );

    /**
    * From ?base_class ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    /**
    * From ?base_class ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void OnRxAuthResponseEvent( WlanContextImpl& aCtxImpl );

    // Prohibit copy constructor.
    WlanDot11SharedAuthPending( 
        const WlanDot11SharedAuthPending& );
    // Prohibit assigment operator.
    WlanDot11SharedAuthPending& operator= 
        ( const WlanDot11SharedAuthPending& );
    
private:   // Data

#ifndef NDEBUG 
    /** name of the state */
    static const TInt8  iName[];        
#endif
    /** 
    * pointer to the beginning of the latest received valid shared 
    * authentication response frame. Not own.
    */
    const TUint8* iLatestRxAuthRespPtr;
    
    /** 
    * value of WHA::ReceivePacket() aFlags for the latest received
    * valid shared authentication response frame
    */
    TUint32 iLatestRxAuthRespFlags;
    };

#endif  // WLANDOT11SHAREDAUTHPENDING_H
