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
* Description:  Definitions for the class TThreadStack.
*
*/


#include "threadstack.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// TThreadStack::TThreadStack()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
TThreadStack::TThreadStack( TThreadId aId, TUint32 aStackStart ) :
    iId( aId ),
    iStackStart( aStackStart )
    {
    LOGSTR2( "ATMH TThreadStack::TThreadStack() aStackStart: %i", aStackStart );
    }

// -----------------------------------------------------------------------------
// TThreadStack::ThreadStackStart()
// Checks if this is the current thread and if this is the current
// thread assings value to the given parameter
// -----------------------------------------------------------------------------
//
TBool TThreadStack::ThreadStackStart( TUint32& aStackStart )
    {
    LOGSTR1( "ATMH TThreadStack::ThreadStackStart");
    
    if ( RThread().Id() == iId )
        {
        aStackStart = iStackStart;
        return ETrue;
        }
    else
        {
        aStackStart = 0;
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// TThreadStack::Match()
// Checks if this is the the current thread.
// -----------------------------------------------------------------------------
//
TBool TThreadStack::Match( const TUint aThreadId )
    {
    LOGSTR2( "ATMH TThreadStack::Match( %d )", aThreadId );
    
    if ( aThreadId != 0 )
    	{
    	LOGSTR2( "ATMH > iId.operator TUint() = %d", iId.operator TUint() );
    	return ( aThreadId == iId.operator TUint() ) ? ETrue : EFalse;
    	}
    else
    	{
    	return ( RThread().Id() == iId ) ? ETrue : EFalse;
    	}
    }
    
// End of File
