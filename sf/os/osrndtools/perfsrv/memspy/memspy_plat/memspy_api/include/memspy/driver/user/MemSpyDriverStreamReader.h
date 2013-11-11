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

#ifndef MEMSPYDRIVERSTREAMREADER_H
#define MEMSPYDRIVERSTREAMREADER_H

// System includes
#include <e32base.h>

// Classes referenced
class CMemSpyMemStreamReader;


class RMemSpyMemStreamReader
    {
friend class RMemSpyDriverClient;

public:
    IMPORT_C void Close();

private:
    RMemSpyMemStreamReader( CMemSpyMemStreamReader* aImp );

public: // API
    IMPORT_C TInt32 ReadInt32L();
    IMPORT_C TUint32 ReadUint32L();
    IMPORT_C void ReadL( TDes8& aDes );
    IMPORT_C void ReadL( TDes16& aDes );

private:
    CMemSpyMemStreamReader* iImp;
    };


#endif
