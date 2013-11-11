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
* Description:   Declaration of the WlanWhaRelease class.
*
*/

/*
* %version: 8 %
*/

#ifndef WLAN_WHA_RELEASE_H
#define WLAN_WHA_RELEASE_H

#include "UmacWsaCommand.h"

/**
 *  Encapsulates the execution of Release WHA command.
 *
 *  @lib ?library
 *  @since S60 3.1
 */
class WlanWhaRelease : public WlanWsaCommand
    {

public:
    
    WlanWhaRelease() {};

    virtual ~WlanWhaRelease() {};

private:

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void CommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams );

    // Prohibit copy constructor.
    WlanWhaRelease( const WlanWhaRelease& );
    // Prohibit assigment operator.
    WlanWhaRelease& operator= ( const WlanWhaRelease& );

private: // data

#ifndef NDEBUG
    /**
     * name of the state
     */
    static const TInt8  iName[];
#endif 
    };

#endif // WLAN_WHA_RELEASE_H
