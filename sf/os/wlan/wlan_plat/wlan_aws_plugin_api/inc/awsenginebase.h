/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Base class for the Adaptive WLAN Scan Engine.
*
*/

/*
* %version: 6 %
*/


#ifndef AWSENGINEBASE_H
#define AWSENGINEBASE_H


#include <ecom/ecom.h>
#include <e32std.h>
#include "awsinterface.h"

/** Value for expiry when background scan should expire immediately. */
const TUint32 KWlanBgScanMaxDelayExpireImmediately = 0;
/** Value for background scan interval when background scan is off. */
const TUint32 KWlanBgScanIntervalNever = 0;
/** Value for Automatic background scan interval. */
const TUint32 KWlanBgScanIntervalAutomatic = 0xFFFFFFFF;
/** Value for AWS ECOM interface UID. */
const TInt KAwsClientInterfaceUid = 0x2002700C;

/**
 *  AWS Engine base class
 *  This class implements base class for AWS Engine.
 *
 *  @since S60 v5.2
 */
class CAwsEngineBase : public CBase, public MAws
    {

public:
    
    struct TAwsEngineConstructionParameters
        {
        MAwsBgScanProvider* aServiceProvider;
        TUint aServiceProviderInterfaceVersion;
        TUint& aAwsEngineInterfaceVersion;
        };

    /**
     * Two-phased constructor.
     * @param aUid UID of implementation to instantiate.
     * @param aConstructionParameters Pointer to construction parameters,
     *                                must not be NULL.
     * @return Pointer to the constructed instance.
     */
    inline static CAwsEngineBase* NewL(
            TInt aUid,
            TAwsEngineConstructionParameters* aConstructionParameters );
    
    /**
    * Destructor.
    */
    virtual ~CAwsEngineBase();
    
    /**
     * List implementations for AWS Plugin interface.
     * @param aImplInfoArray Array of implementation informations.
     */
    inline static void ListImplementationsL(
            RImplInfoPtrArray& aImplInfoArray);

private: // data

    /**
     * Identifies the instance of an implementation created by
     * the ECOM framework.
     */
    TUid iInstanceIdentifier;

    };

#include "awsenginebase.inl"

#endif // AWSENGINEBASE_H
