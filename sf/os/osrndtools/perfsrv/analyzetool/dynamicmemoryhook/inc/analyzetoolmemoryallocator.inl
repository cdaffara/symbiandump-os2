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
* Description:  Definition for the inline functions of RAnalyzeToolMemoryAllocator.
*
*/



#include <analyzetool/atcommon.h>

// -----------------------------------------------------------------------------
// RAnalyzeToolMemoryAllocator::IsAddressLoadedCode()
// Checks is the given address in loaded code memory area.
// -----------------------------------------------------------------------------
//
inline bool RAnalyzeToolMemoryAllocator::IsAddressLoadedCode( TUint32& aAddress )
    {
    // Debug log strings in this function are not used because
    // this is called so many times.
    /*
     * TMemModelAttributes models.
     * EMemModelTypeDirect      // direct memory model on hardware
     * EMemModelTypeMoving=1    // moving memory model on hardware
     * EMemModelTypeMultiple=2  // multiple memory model on hardware
     * EMemModelTypeEmul=3      // emulation using single host process
     * Flexible ?
     */
    switch( iMemoryModel )
        {
        case EMemModelTypeMultiple:
            // Use low & high limits which define rofs loading->rom area
            // in multiple memory model.
            if ( aAddress < KATMultipleMemoryModelLowLimit 
              || aAddress > KATMultipleMemoryModelHighLimit )
                return false;
            return true;
        default:
            return true;
        }
    }

// End of File

