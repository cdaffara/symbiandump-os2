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
* Description:  Declaration of the class TCodeblock.
*
*/


#ifndef CODEBLOCK_H
#define CODEBLOCK_H

// INCLUDES
#include <u32std.h>

/**
*  Stores information of process loaded code segments
*/
class TCodeblock
    {
    
    public: // Constructors
        
        /**
        * C++ default constructor.
        * @param aRunAddress Start address of the memory block. 
        * @param aSize The size of the memory block.
        * @param aName The name of the library
        */
        TCodeblock( TLinAddr aRunAddress, TUint32 aSize, TBuf8<KMaxLibraryName>& aName );

    public: // New functions

        /**
        * Checks if the given address is in this memory block area
        * @param aAddress A address to be checked. 
        * @return TBool Returns ETrue if the given address is in this
        *               memory block area, EFalse otherwise
        */
        TBool CheckAddress( TUint32 aAddress );

        /**
        * Matches if the given parameters represents this memory block 
        * @param aName The name of the library
        * @return TBool Returns ETrue if the given parameters represents
        *               this memory block, EFalse otherwise
        */
        TBool Match( TBuf8<KMaxLibraryName>& aName );
        
    private: // Member variables

        /* Start address of the memory block */
        TLinAddr iStartAddress;

        /* End address of the memory block */
        TLinAddr iEndAddress;

        /* End address of the memory block */
        TBuf8<KMaxLibraryName> iName;
    };

#endif // CODEBLOCK_H

// End of File

