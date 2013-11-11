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
* Description:  Definitions of logging macros for Analyze Tool S60 modules.
*
*/

#ifndef __ATLOG_H__
#define __ATLOG_H__

#ifdef _DEBUG
//To enable/disable logging uncomment/comment the next line
// #define LOGGING_ENABLED  //Enable logging
//Uncomment next to enable logging only LOGSTR1 lines
// #define LOGGING_ENABLED_FAST //Logging only function names

//Disable next to use file logging
 #define USE_RDEBUG
#endif

//Do not alter the code below this

#ifdef __KERNEL_MODE__

 #ifdef LOGGING_ENABLED
  #include <kernel.h> 
  #define LOGSTR1( AAA ) 			{ Kern::Printf( AAA ); }
  #define LOGSTR2( AAA, BBB ) 			{ Kern::Printf( AAA, BBB ); }
 #else
  #define LOGSTR1( AAA )
  #define LOGSTR2( AAA, BBB )
 #endif

#else // #ifdef __KERNEL_MODE__

 #ifdef LOGGING_ENABLED

//  INCLUDES
  #include <flogger.h>
  #ifdef USE_RDEBUG
   #include <e32svr.h>
  #endif

// CONSTANTS
  _LIT( KAtLoggingFolder, "atlog" );
  _LIT( KAtLoggingFile  , "LOG.TXT" );
  #define ONE_SPACE_MARGIN	_S(" ")

// ---------------------------------------------------------------------------------
// Internal MACROS. Do not call these directly, use the External MACROS instead
// ---------------------------------------------------------------------------------

  #ifdef LOGGING_ENABLED_FAST

   #ifdef USE_RDEBUG
    #define INTERNAL_LOGSTR1( AAA )				{ _LIT( logdes, AAA ); RDebug::Print( logdes() ); }
   #else  // RFileLogger is used
    #define INTERNAL_LOGSTR1( AAA )				{ _LIT( logdes, AAA ); RFileLogger::Write( KAtLoggingFolder(), KAtLoggingFile(), EFileLoggingModeAppend, logdes() ); }
   #endif // USE_RDEBUG
   #define INTERNAL_LOGSTR2( AAA, BBB )
   #define INTERNAL_LOGSTR3( AAA, BBB, CCC )
   #define INTERNAL_LOGMEM
   #define INTERNAL_LOG( AAA )

  #else
  
   #ifdef USE_RDEBUG
    #define INTERNAL_LOGDESC8( AAA )				{ TBuf<100> tmp; if( sizeof( AAA ) < 100 ) tmp.Copy( AAA ); RDebug::Print( tmp );}
    #define INTERNAL_LOGSTR1( AAA )				{ _LIT( logdes, AAA ); RDebug::Print( logdes() ); }
    #define INTERNAL_LOGSTR2( AAA, BBB )			{ _LIT( logdes, AAA ); RDebug::Print( TRefByValue<const TDesC>( logdes()), BBB ); }
    #define INTERNAL_LOGSTR3( AAA, BBB, CCC )	{ _LIT( logdes, AAA ); RDebug::Print( TRefByValue<const TDesC>(logdes()), BBB, CCC ); }
    #define INTERNAL_LOGMEM						{ _LIT( logdes, "Heap size: %i" );  TBuf<50> buf; TInt a; User::AllocSize( a ); buf.Format( logdes, a ); RDebug::Print( buf ); }
    #define INTERNAL_LOG( AAA )					AAA
   #else  // RFileLogger is used
    #define INTERNAL_LOGSTR1( AAA )				{ _LIT( logdes, AAA ); RFileLogger::Write( KAtLoggingFolder(), KAtLoggingFile(), EFileLoggingModeAppend, logdes() ); }
    #define INTERNAL_LOGSTR2( AAA, BBB ) 		{ _LIT( logdes, AAA ); RFileLogger::WriteFormat( KAtLoggingFolder(), KAtLoggingFile(), EFileLoggingModeAppend, TRefByValue<const TDesC>( logdes()), BBB ); }
    #define INTERNAL_LOGSTR3( AAA, BBB, CCC) 	{ _LIT( logdes, AAA ); RFileLogger::WriteFormat( KAtLoggingFolder(), KAtLoggingFile(), EFileLoggingModeAppend, TRefByValue<const TDesC>(logdes()), BBB, CCC ); }
    #define INTERNAL_LOGMEM 					{ _LIT( logdes, "Heap size: %i" ); TMemoryInfoV1Buf info; UserHal::MemoryInfo(info); TInt freeMemory = info().iFreeRamInBytes; TBuf<50> buf; buf.Format( logdes, freeMemory ); RFileLogger::WriteFormat( KAtLoggingFolder(), KAtLoggingFile(), EFileLoggingModeAppend, buf ); }
    #define INTERNAL_LOG( AAA )					AAA
   #endif // USE_RDEBUG

  #endif

 #else

  #define INTERNAL_LOGSTR1( AAA )
  #define INTERNAL_LOGSTR2( AAA, BBB )
  #define INTERNAL_LOGSTR3( AAA, BBB, CCC )
  #define INTERNAL_LOGMEM
  #define INTERNAL_LOG( AAA )

 #endif

// ---------------------------------------------------------------------------------
// External MACROS. Use these in code
// ---------------------------------------------------------------------------------
// Logging of string
// i.e.: LOGSTR1( "Whoo-haa!" );
 #define LOGSTR1( AAA ) { INTERNAL_LOGSTR1( AAA ); }

// Logging of string + integer value
// i.e.: LOGSTR2( "CHttpd status %i:", iStatus );
 #define LOGSTR2( AAA, BBB ) { INTERNAL_LOGSTR2( AAA, BBB ); }

// Logging of descriptor + 2 integers
// i.e.: LOGSTR3( "Jippii %i %i", val1, val2 );
 #define LOGSTR3( AAA, BBB, CCC ) { INTERNAL_LOGSTR3( AAA, BBB, CCC ); }

// Log heap size
 #define LOGMEM         { INTERNAL_LOGMEM }

// Logging variable operations  
 #define LOG( AAA )     INTERNAL_LOG( AAA )

#endif // #ifdef __KERNEL_MODE__

#endif // __ATLOG_H__
// End of File
