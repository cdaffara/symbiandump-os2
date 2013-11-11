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
* Description:   Declaration of the WlanDot11Connecting class
*
*/

/*
* %version: 15 %
*/

#ifndef WLANDOT11CONNECTING_H
#define WLANDOT11CONNECTING_H

#include "UmacDot11State.h"

/**
*  Infrastructure mode connection establishment base class.
*  A composite state, which holds the common behaviour of its 
*  substates. In this state STA is locked to a specific radio channel and 
*  is performing authentication and association procedures.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11Connecting : public WlanDot11State
    {    
public:

    /**
    * Destructor.
    */
    virtual ~WlanDot11Connecting() {};
    
protected:

    /**
    * C++ default constructor.
    */
    WlanDot11Connecting() :iFlags( 0 ) {};

    /**
    * Checks is a class 2 frame
    * @param aFrameType first byte from FrameControl field
    * @return ETrue is class 2 frame, EFalse is not a class 2 frame
    */
    static TBool IsClass2Frame( const TUint8 aFrameType );

    /**
    * Checks is a class 3 frame
    * @param aFrameType first byte from FrameControl field
    * @return ETrue is class 3 frame, EFalse is not a class 3 frame
    */
    static TBool IsClass3Frame( const TUint8 aFrameType );

    /**
    * ?member_description.
    *
    * @since Series 60 3.1
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
        TUint8* aBuffer ) = 0;
        
private:  
    
    /**
    * From ?base_class ?member_description.
    * Timer timeout function
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual TAny* RequestForBuffer ( 
        WlanContextImpl& aCtxImpl,
        TUint16 aLength );
           
    // Prohibit copy constructor 
    WlanDot11Connecting( const WlanDot11Connecting& );
    // Prohibit assigment operator
    WlanDot11Connecting& operator= ( const WlanDot11Connecting& ); 
    
protected: // Data

    /** mask for local deferred events */
    TUint32 iFlags;
    };

#endif      // WLANDOT11CONNECTING_H
            
// End of File
