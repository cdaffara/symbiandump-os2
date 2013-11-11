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

#include <memspy/driver/memspydriverstreamreader.h>

// User includes
#include "MemSpyDriverStreamReaderImp.h"




RMemSpyMemStreamReader::RMemSpyMemStreamReader( CMemSpyMemStreamReader* aImp )
:   iImp( aImp )
    {
    ASSERT( iImp );
    }


EXPORT_C void RMemSpyMemStreamReader::Close()
    {
    delete iImp;
    iImp = NULL; // To pacify code scanner
    }


EXPORT_C TInt32 RMemSpyMemStreamReader::ReadInt32L()
    {
    return iImp->ReadInt32L();
    }


EXPORT_C TUint32 RMemSpyMemStreamReader::ReadUint32L()
    {
    return iImp->ReadUint32L();
    }


EXPORT_C void RMemSpyMemStreamReader::ReadL( TDes8& aDes )
    {
    iImp->ReadL( aDes );
    }


EXPORT_C void RMemSpyMemStreamReader::ReadL( TDes16& aDes )
    {
    iImp->ReadL( aDes );
    }





