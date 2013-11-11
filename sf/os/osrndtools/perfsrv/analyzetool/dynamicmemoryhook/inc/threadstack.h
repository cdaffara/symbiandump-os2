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
* Description:  Declaration of the class TThreadStack.
*
*/


#ifndef THREADSTACK_H
#define THREADSTACK_H

// INCLUDES
#include <u32std.h>

// CLASS DECLARATION

/**
*  Stores thread id and the start of the thread's callstack
*/
class TThreadStack
    {
    public:
    
        /**
        * C++ default constructor.
        * @param aId The thread id
        * @param aStackStart The start of thread's stack
        */
        TThreadStack( TThreadId aId, TUint32 aStackStart );
        
        /**
        * Checks if this is the current thread and if this is the current
        * thread assings value to the given parameter
        * @param aStackStart& A reference to stack start
        * @return TBool The start of thread's stack
        */
        TBool ThreadStackStart( TUint32& aStackStart );
       
        /**
        * Checks if this the the current thread
        * @param aThreadId A thread id
        * @return TBool ETrue it this is the current thread, EFalse otherwise
        */ 
        TBool Match( const TUint aThreadId = 0 );
        
    private: // Member variables
    
        /* The id of the thread */
        TThreadId iId;
        
        /* The start addess of this thread */
        TUint32 iStackStart;
    };


#endif // THREADSTACK_H

// End of File


