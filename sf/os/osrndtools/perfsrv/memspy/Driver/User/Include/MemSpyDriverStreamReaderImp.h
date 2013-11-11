/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#ifndef MEMSPYDRIVERSTREAMREADERIMP_H
#define MEMSPYDRIVERSTREAMREADERIMP_H

// System includes
#include <e32base.h>

// Classes referenced


class CMemSpyMemStreamReader : public CBase
    {
public:
    CMemSpyMemStreamReader( const TDesC8& aData );
    void ConstructL();
    
public: // API
    TInt32 ReadInt32L();
    TUint32 ReadUint32L();
    void ReadL( TDes8& aDes );
    void ReadL( TDes16& aDes );

private: // Internal methods
    void IsAvailableL( TInt aRequired ) const;
    void IncrementPos( TInt aAmount );

private: // Data members
    const TUint8* iCurrent;
    const TUint8* iMax;
    };


#endif
