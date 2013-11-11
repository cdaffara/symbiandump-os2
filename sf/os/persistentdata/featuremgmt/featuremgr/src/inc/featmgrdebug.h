/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef FEATMGRDEBUG_H
#define FEATMGRDEBUG_H


//  INCLUDES
#include <e32debug.h>

// Enable macros to support error and/or info logging
#ifdef _DEBUG
    //#define FEATMGR_INFO_LOG_ENABLED
    //#define FEATMGR_ERROR_LOG_ENABLED
    #define FEATMGR_TIMESTAMP_ENABLED
#endif // _DEBUG


/**
* Prefix trace macro to complete tracing with component name.
* Returns TDesC which can be used directly with RDebug or RFileLogger.
*/
 #define PREFIX( aMsg ) TPtrC( (const TText*)L"[EFM]: " L##aMsg )

/**
* Prefix error trace
*/
 #define ERROR_PREFIX( aMsg ) PREFIX( "[ERROR]: " L##aMsg )

/**
* Prefix macro for strings
*/
#define _PREFIX_CHAR( aMsg ) (const char*)"[EFM]: " ##aMsg

// Info logging
#ifdef FEATMGR_INFO_LOG_ENABLED

 #define INFO_LOG( aMsg ) { RDebug::Print( PREFIX( aMsg ) ); }

 #define INFO_LOG1( aMsg, aArg1 )\
    { RDebug::Print( PREFIX( aMsg ), aArg1 ); }

 #define INFO_LOG2( aMsg, aArg1, aArg2 )\
    { RDebug::Print( PREFIX( aMsg ), aArg1, aArg2 ); }

 #define INFO_LOG3( aMsg, aArg1, aArg2, aArg3 )\
    { RDebug::Print( PREFIX( aMsg ), aArg1, aArg2, aArg3 ); }

 #define FUNC( aMsg, aP1 )\
    {\
    RDebug::Printf( aMsg, aP1 );\
    }\
// Function log object
_LIT8( KFuncNameTerminator, "(" );
_LIT8( KFuncLeavePattern, "L" );

class TFuncLog
    {
    public:
    
    static void Cleanup( TAny* aPtr )
        {
        TFuncLog* self = static_cast< TFuncLog* >( aPtr );
        self->iLeft = ETrue;
        FUNC( _PREFIX_CHAR("%S-LEAVE"), &self->iFunc ); // Leave detected
        }

    inline TFuncLog( const char* aFunc ) :
            iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
            iLeft( EFalse ),
            iCleanupItem( Cleanup, this ),
            iCanLeave( EFalse )
        {
        TInt pos( iFunc.Find( KFuncNameTerminator ) );
        if( pos != KErrNotFound )
            {
            iFunc.Set( iFunc.Left( pos ) );
            iCanLeave = !iFunc.Right( KFuncLeavePattern().Length() ).Compare( KFuncLeavePattern );
            if ( iCanLeave )
                {
                CleanupStack::PushL( iCleanupItem ); // Ignore warnings
                }
            }
        FUNC( _PREFIX_CHAR("%S-START"), &iFunc );
        }

    inline ~TFuncLog()
        {
        if ( !iLeft )
            {
            if ( iCanLeave )
                {
                CleanupStack::Pop( this ); // Pop the cleanup item
                }
            FUNC( _PREFIX_CHAR("%S-END"), &iFunc ); // Normally finished
            }
        }

    private: // Data
    
    TPtrC8 iFunc;
    TBool iLeft;
    TCleanupItem iCleanupItem;
    TBool iCanLeave;
    };

 #define FUNC_LOG TFuncLog _fl( __PRETTY_FUNCTION__ );

#else // FEATMGR_INFO_LOG_ENABLED

 #define INFO_LOG( aMsg )

 #define INFO_LOG1( aMsg, aArg1 )

 #define INFO_LOG2( aMsg, aArg1, aArg2 )

 #define INFO_LOG3( aMsg, aArg1, aArg2, aArg3 )

 #define FUNC_LOG

#endif // FEATMGR_INFO_LOG_ENABLED

// Timestamp logging
#ifdef FEATMGR_TIMESTAMP_ENABLED

 #define TIMESTAMP( aCaption )\
    {\
    TTime t;\
    t.HomeTime();\
    TDateTime dt = t.DateTime();\
    _LIT( KCaption, aCaption );\
    RDebug::Print( PREFIX("[TIMESTAMP] %S %d:%02d:%02d.%d us"),\
        &KCaption, dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond() );\
    }

#else

 #define TIMESTAMP( aCaption )

#endif // FEATMGR_TIMESTAMP_ENABLED

// Error logging
#ifdef FEATMGR_ERROR_LOG_ENABLED

 #define ERROR_LOG( aMsg )\
    {RDebug::Print( ERROR_PREFIX( aMsg ) ); }

 #define ERROR_LOG1( aMsg, aArg1 )\
    {RDebug::Print( ERROR_PREFIX( aMsg ), aArg1 ); }

 #define ERROR_LOG2( aMsg, aArg1, aArg2 )\
    { RDebug::Print( ERROR_PREFIX( aMsg ), aArg1, aArg2 ); }

 #define ERROR_LOG3( aMsg, aArg1, aArg2, aArg3 )\
    { RDebug::Print( ERROR_PREFIX( aMsg ), aArg1, aArg2, aArg3 ); }

 #define LOG_IF_ERROR( aErr, aMsg )\
    if ( ( aErr ) != KErrNone )\
        { RDebug::Print( ERROR_PREFIX( aMsg )); }

 #define LOG_IF_ERROR1( aErr, aMsg, aArg1 )\
    if ( ( aErr ) != KErrNone )\
        { RDebug::Print( ERROR_PREFIX( aMsg ), aArg1 ); }
        
 #define LOG_IF_ERROR2( aErr, aMsg, aArg1, aArg2 )\
    if ( ( aErr ) != KErrNone )\
        { RDebug::Print( ERROR_PREFIX( aMsg ), aArg1, aArg2 ); }

 #define LOG_IF_ERROR3( aErr, aMsg, aArg1, aArg2, aArg3 )\
    if ( ( aErr ) != KErrNone )\
        { RDebug::Print( ERROR_PREFIX( aMsg ), aArg1, aArg2, aArg3 ); }

#else // FEATMGR_ERROR_LOG_ENABLED

 #define ERROR_LOG( aMsg )

 #define ERROR_LOG1( aMsg, aArg1 )

 #define ERROR_LOG2( aMsg, aArg1, aArg2 )

 #define ERROR_LOG3( aMsg, aArg1, aArg2, aArg3 )

 // Remove compiler warning
 #define LOG_IF_ERROR( aErr, aMsg ) ( aErr ) = ( aErr );

 #define LOG_IF_ERROR1( aErr, aMsg, aArg1 )  ( aErr ) = ( aErr );

 #define LOG_IF_ERROR2( aErr, aMsg, aArg1, aArg2 )  ( aErr ) = ( aErr );

 #define LOG_IF_ERROR3( aErr, aMsg, aArg1, aArg2, aArg3 ) ( aErr ) = ( aErr );

#endif // FEATMGR_ERROR_LOG_ENABLED


#endif // FEATMGRDEBUG_H

// End of File
