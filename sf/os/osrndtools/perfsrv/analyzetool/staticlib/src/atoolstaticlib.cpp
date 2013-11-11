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
* Description:  The file contains declarations of extern methods and definitions
*                of overloaded User methods.
*
*/


#include <analyzetool/customuser.h>
#include "atlog.h"

// -----------------------------------------------------------------------------
// GetInt()
// Extern function for acquiring all integer values
// -----------------------------------------------------------------------------
// 
GLREF_C TInt GetInt( const TUint8 aType );

// -----------------------------------------------------------------------------
// GetString()
// Extern function for acquiring all string values
// -----------------------------------------------------------------------------
//
GLREF_C TPtrC GetString( const TUint8 aType );

// -----------------------------------------------------------------------------
// User::Exit()
// Overloaded User::Exit() function
// -----------------------------------------------------------------------------
// 
void User::Exit( TInt aReason )
    {   
    LOGSTR2( "ATMH User::Exit %i", aReason ); 
    CustomUser::Exit( aReason );
    }

// -----------------------------------------------------------------------------
// User::Panic()
// Overloaded User::Panic() function
// -----------------------------------------------------------------------------
//
void User::Panic( const TDesC& aCategory, TInt aReason )
    {
    LOGSTR3( "ATMH User::Panic() %S %i", &aCategory, aReason );
    CustomUser::Panic( aCategory, aReason );
    }
 
// -----------------------------------------------------------------------------
// User::SetCritical()
// Overloaded User::SetCritical() function which returns
// KErrNone, if successful; KErrArgument, if EAllThreadsCritical is 
// passed - this is a state associated with a process, and you use 
// User::SetProcessCritical() to set it.
// -----------------------------------------------------------------------------
// 
TInt User::SetCritical( TCritical aCritical )
    {
    LOGSTR2( "ATMH User::SetCritical() %i", aCritical );
    return CustomUser::SetCritical( aCritical );
    }

// -----------------------------------------------------------------------------
// User::SetProcessCritical()
// Overloaded User::SetProcessCritical() function
// KErrNone, if successful; KErrArgument, if either EProcessCritical or 
// EProcessPermanent is passed - these are states associated with a 
// thread, and you use User::SetCritical() to set them.
// -----------------------------------------------------------------------------
// 
TInt User::SetProcessCritical( TCritical aCritical )
    {
    LOGSTR2( "ATMH User::SetProcessCritical() %i", aCritical );
    return CustomUser::SetProcessCritical( aCritical );
    }
    
// -----------------------------------------------------------------------------
// UserHeap::SetupThreadHeap()
// Overloaded UserHeap::SetupThreadHeap function
// -----------------------------------------------------------------------------
//  
TInt UserHeap::SetupThreadHeap( TBool aNotFirst, 
                                SStdEpocThreadCreateInfo& aInfo )
    {
    LOGSTR1( "ATMH UserHeap::SetupThreadHeap()" );
        
    // Check validity of parameters 
    TInt logOption( GetInt( (TUint8) CustomUser::ELogOption ) );
    if ( logOption < 0 )
        logOption = KATDefaultLogOption;
    
    TInt debug( GetInt( (TUint8) CustomUser::EDebug ) );
    if ( debug < 0 )
        debug = KATDefaultDebug;
    
    TInt allocCallStack( GetInt( (TUint8) CustomUser::EAllocCallStackSize ) );
    if ( allocCallStack < 0 )
        allocCallStack = KATDefaultAllocCallStackSize;
    
    TInt freeCallStack( GetInt( (TUint8) CustomUser::EFreeCallStackSize ) );
    if ( freeCallStack < 0 )
        freeCallStack = KATDefaultFreeCallStackSize;
        
    return CustomUser::SetupThreadHeap( aNotFirst, aInfo, 
            GetString( (TUint8) CustomUser::ELogFileName ),
            GetString( (TUint8) CustomUser::ELogFilePath ),
            (TUint32) logOption, (TUint32) debug,
            GetString( (TUint8) CustomUser::EVersion ),
            (TUint32) allocCallStack, (TUint32) freeCallStack,
            KATArgumentList );
    }
  
// End of File
