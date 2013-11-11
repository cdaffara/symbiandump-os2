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
* Description:  Declaration of the class RAnalyzeToolMemoryAllocator.
*
*/


#ifndef ANALYZETOOLMEMORYALLOCATOR_H
#define ANALYZETOOLMEMORYALLOCATOR_H

// INCLUDES
#include <u32std.h>
#include "threadstack.h"
#include "../../symbian_version.hrh"

// CONSTANTS
#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_3 )
    #ifndef __WINS__
    const TInt KDummyHandle = -1000;
    #endif
#endif

// CLASS DECLARATION

/**
*  Abstract class for basic RAnalyzeToolMemoryAllocator funtions
*/
class RAnalyzeToolMemoryAllocator : public RAllocator
    {
    public:
    
        /**
        * C++ default constructor.
        * @param aNotFirst Is this first thread using this heap
        */   
        RAnalyzeToolMemoryAllocator( TBool aNotFirst );
        
        /**
        * Destructor.
        */  
        //lint -e{1510} suppress "base class 'RAllocator' has no destructor"    
        virtual ~RAnalyzeToolMemoryAllocator();
        
        /**
        * Uninstall the RAnalyzeToolMemoryAllocator
        */
        virtual void Uninstall() = 0;
        
        /**
        * Shares the heap for another thread
        */
        virtual void ShareHeap() = 0;

    protected:

        /**
         * Switch original allocator in use.
         * Switches original allocator in use if not already.
         */
        void SwitchOriginalAllocator();
        
        /**
         * Checks is the given address in loaded code memory area.
         */
        inline bool IsAddressLoadedCode( TUint32& aAddress );
        
        /* The original thread RAllocator */
        RAllocator* iAllocator;
        
        /* Is this the first thread using this heap */
        TBool iNotFirst;   

        /* Memorymodel */
        TUint32 iMemoryModel;
        
    };

// INLINES
#include "analyzetoolmemoryallocator.inl"

#endif // ANALYZETOOLMEMORYALLOCATOR_H

// End of File
