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
* Description: This file contains the CPrintQueue and 
* TPrintQueueItem class definitions.
*
*/

#ifndef PRINTQUEUE_H
#define PRINTQUEUE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifTestInterface.h>


// CONSTANTS

// The print queue maximum length. If queue overflows, the lowest
// priority item is discarded from queue.
const TInt KQueueMaxLen = 20;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

// DESCRIPTION
// TPrintQueueItem is a print queue item class.
class TPrintQueueItem 
        :public CBase
    {
    public: // Enumerations
         // None
    
    protected: // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        /** 
        * C++ default constructor.
        */
        TPrintQueueItem( const TInt aPriority,
                         const TStifInfoName& aText1,
                         const TName& aText2 );

    public: // New functions
        // None

    protected:  // New functions
        // None

    private:    // New functions

    public:  // Functions from base classes        
        
    protected:  // Functions from base classes
        // None

    private:
       // None
       
    public:   //Data
        TStifInfoName     iData1;                    // Description
        TName         iData2;                    // Text
        TInt          iPriority;                 // Priority
        static const  TInt iOffset;              // Queue link offset
    
    protected:  // Data
        // None

    private:    // Data
        TSglQueLink   iSlink;                    // Queue link
    
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None 

    };

// DESCRIPTION
// CPrintQueue is a queue for test module prints
class CPrintQueue
        :public CBase
    {
    public: // Enumerations
        // None

    protected: // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */        
        static CPrintQueue* NewL();

        /**
        * Destructor of CPrintQueue.
        */
        virtual ~CPrintQueue();


    public: // New functions

        /**
        * Pops item from queue
        */
        TPrintQueueItem*  Pop();
    

        /**
        * Push item to queue
        */
        TInt Push( TPrintQueueItem& anItem );

        /**
        * Returns the count
        */
        TInt Count();
        

    public: // Functions from base classes
        // None

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CPrintQueue();
        
    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        TSglQue<TPrintQueueItem>     iQueue;
        TSglQueIter<TPrintQueueItem> iQueueIterator;
        TInt                         iQueueLen;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

#endif  // PRINTQUEUE_H

// End of File
