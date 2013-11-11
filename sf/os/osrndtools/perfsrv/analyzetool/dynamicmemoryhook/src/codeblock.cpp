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
* Description:  Definitions for the class TCodeblock.
*
*/


#include "codeblock.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// TCodeblock::TCodeblock
// C++ default constructor.
// -----------------------------------------------------------------------------
//
TCodeblock::TCodeblock( TLinAddr aRunAddress, 
                        TUint32 aSize, 
                        TBuf8<KMaxLibraryName>& aName ) :
                        iStartAddress( aRunAddress ),
                        iEndAddress( aRunAddress + aSize )
    {
    LOGSTR3( "ATMH TCodeblock::TCodeblock() %x - %x", iStartAddress, iEndAddress );
    iName.Copy( aName );
    }

// -----------------------------------------------------------------------------
// TCodeblock::CheckAddress
// Checks if the given address is in this memory block area
// -----------------------------------------------------------------------------
//
TBool TCodeblock::CheckAddress( TUint32 aAddress )
    {
    if ( iStartAddress <= aAddress && aAddress <= iEndAddress )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TCodeblock::Match
// Matches if the given parameters represents this memory block 
// -----------------------------------------------------------------------------
//
TBool TCodeblock::Match( TBuf8<KMaxLibraryName>& aName )
    {
    LOGSTR1( "ATMH TCodeblock::Match()" );
    return ( 0 == iName.CompareF( aName ) ) ? ETrue : EFalse;
    }

// End of File
