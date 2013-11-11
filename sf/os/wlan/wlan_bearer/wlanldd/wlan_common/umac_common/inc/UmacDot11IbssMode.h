/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11IbssMode class.
*
*/

/*
* %version: 17 %
*/

#ifndef C_WLANDOT11IBSSMODE_H
#define C_WLANDOT11IBSSMODE_H

#include "UmacDot11Associated.h"

/**
 *  IBSS mode base class
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanDot11IbssMode : public WlanDot11Associated
    {

public:

    virtual ~WlanDot11IbssMode() {};

protected:

    WlanDot11IbssMode() {};

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void DoSetTxMpduDaAddress( 
        SDataFrameHeader& aDataFrameHeader, 
        const TMacAddress& aMac ) const;  
       
    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */    
    virtual TBool DoIsRxFrameSAourAddress( 
        WlanContextImpl& aCtxImpl,
        const SDataFrameHeader& aFrameHeader,
        const SAmsduSubframeHeader* aSubFrameHeader ) const;

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual TBool DoIsValidAddressBitCombination(
        const SDataFrameHeader& aFrameHeader ) const;

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void DoBuildEthernetFrame(
        TDataBuffer& aBuffer,
        const SDataMpduHeader& aDot11DataMpdu,
        const TUint8* aStartOfEtherPayload,
        TUint aEtherPayloadLength,
        TBool aAmsdu,
        TUint8* aCopyBuffer );

private:           

    // Prohibit copy constructor
    WlanDot11IbssMode( const WlanDot11IbssMode& );
    // Prohibit assigment operator
    WlanDot11IbssMode& operator= ( const WlanDot11IbssMode& );  

    };

#endif // C_WLANDOT11IBSSMODE_H
