/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanWhaConfigureAc class.
*
*/

/*
* %version: 10 %
*/

#ifndef WLANWHACONFIGUREAC_H
#define WLANWHACONFIGUREAC_H

#include "UmacWsaCommand.h"
#include "umacinternaldefinitions.h"
#include "UmacContextImpl.h"

/**
 *  Encapsulates the execution of ConfigureAc WHA command.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanWhaConfigureAc : public WlanWsaCommand
    {

public:
    
    WlanWhaConfigureAc();

    virtual ~WlanWhaConfigureAc() {};

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    void Set( 
        const TCwMinVector& aCwMin,
        const TCwMaxVector& aCwMax,
        const TAifsVector& aAIFS,
        const TTxOplimitVector& aTxOplimit );

private:

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void CommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams );

    // Prohibit copy constructor.
    WlanWhaConfigureAc( const WlanWhaConfigureAc& );
    // Prohibit assigment operator.
    WlanWhaConfigureAc& operator= ( const WlanWhaConfigureAc& );

private: // data

#ifndef NDEBUG
    /**
     * name of the state
     */
    static const TInt8  iName[];
#endif

    TUint16 iCwMin[WHA::Wha::KNumOfEdcaQueues];
    TUint16 iCwMax[WHA::Wha::KNumOfEdcaQueues];
    TUint8  iAIFS[WHA::Wha::KNumOfEdcaQueues];
    TUint16 iTxOplimit[WHA::Wha::KNumOfEdcaQueues];    
    };

#endif // WLANWHACONFIGUREAC_H
