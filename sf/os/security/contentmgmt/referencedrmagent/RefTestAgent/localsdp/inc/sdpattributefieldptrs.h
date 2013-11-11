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
* Name          : SdpAttributeFieldPtrs.h
* Part of       : Local SDP Codec
* Interface     : -
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPATTRIBUTEFIELDPTRS_H
#define CSDPATTRIBUTEFIELDPTRS_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSdpFmtAttributeField;

// CLASS DECLARATION
/**
 *	This class encapsulates the data of CSdpAttributeField.
 *
 *  @lib localsdpcodec.lib
 */
class CSdpAttributeFieldPtrs : public CBase
    {
    public:
    
        static CSdpAttributeFieldPtrs* NewL();
	    ~CSdpAttributeFieldPtrs();

        void SetValueL( const TDesC8& aValue );
        const TDesC8& Value() const;
        
        void SetFmtAttribute( const CSdpFmtAttributeField* aFmtAttribute );
        const CSdpFmtAttributeField* FmtAttribute() const;

    private: // Second phase constructors
    
        CSdpAttributeFieldPtrs();
        void ConstructL();
    
    private: // Data

        HBufC8* iValue; // Owned
        const CSdpFmtAttributeField* iFmtAttributeField; // Not owned	    
    };

#endif // CSDPATTRIBUTEFIELDPTRS_H

// End of File
