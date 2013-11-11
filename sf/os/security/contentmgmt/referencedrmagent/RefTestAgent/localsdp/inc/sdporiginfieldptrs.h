/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name          : SdpOriginFieldPtrs.h
* Part of       : Local SDP Codec
* Interface     : -
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPORIGINFIELDPTRS_H
#define CSDPORIGINFIELDPTRS_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TUint KMaxTInt64Digits = 30;

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 *	This class encapsulates the data of CSdpAttributeField.
 *
 *  @lib localsdpcodec.lib
 */
class CSdpOriginFieldPtrs : public CBase
    {
    public:
    
        static CSdpOriginFieldPtrs* NewL( TInt64 aSessionId,
        								  TInt64 aSessionVersion );
	    ~CSdpOriginFieldPtrs();

        void SetUserNameL( const TDesC8& aUserName );        
        const TDesC8& UserName() const;
        
        void SetSessionIdL( const TDesC8& aSessionId );
        void SetSessionIdL( TInt64 aSessionId );
        const TDesC8& SessionId() const;
        
        void SetSessionVersionL( const TDesC8& aSessionVersion );
        void SetSessionVersionL( TInt64 aSessionVersion );
        const TDesC8& SessionVersion() const;

    private: // Second phase constructors

        CSdpOriginFieldPtrs();
        void ConstructL( TInt64 aSessionId, TInt64 aSessionVersion );
    
    private: // Data

		// Owned
		HBufC8* iUserName;
		// Owned
		HBufC8* iSessionId;
		// Owned
		HBufC8* iSessionVersion;
    };

#endif // CSDPORIGINFIELDPTRS_H

// End of File
