/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Header file for functions for the Reporter logging mechanism*
*/




#ifndef CPO_REPORT_HPP
#define CPO_REPORT_HPP

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

// Comment out this line for release version ...
// #define RPT_DEBUG_PROG

#define CPO_GARBAGE_CHAR               0xDB    // decimal value ( 3339 )
#define CPO_TRUE                       1
#define CPO_FALSE                      0
#define CPO_BOOL                        unsigned int

#define BITFIELD                       unsigned long
#define CPO_MAX_FILENAME_LEN           255

// status values
#define CPO_CLEAR                      0
#define CPO_CONNECTED                  1  // component currently initiated
#define CPO_ACTIVATED                  2  // component currently active


// Max values for messages
#define RPT_OPEN_MSG_LEN               2048
#define RPT_MAX_MSG_LEN                  256
#define RPT_MAX_PROGRAM_LEN              15

// Max values for message file contents
#define RPT_MAX_LIST_ENTRIES           1000
#define RPT_MAX_LINE_LEN                200

// Bit values for the current state 
#define RPT_CLEAR                         0
#define RPT_FILE                          4   // Output to file
#define RPT_STDOUT                        8   // Output to stdout
#define RPT_STDERR                       16   // Output to stderr
#define RPT_DEBUG_ON                     32   // Are debug messages processed ?
#define RPT_OUTPUT_MASK        ( RPT_FILE | \
                               RPT_STDOUT | \
                               RPT_STDERR )

// Bit values for the prefix mask
#define RPT_MSG_ONLY                      1
#define RPT_MSG_TYPE                      2
#define RPT_APPNAME                       4
#define RPT_DATE                          8
#define RPT_TIME                         16
#define RPT_ALL            ( RPT_MSG_TYPE | \
                              RPT_APPNAME | \
                                 RPT_DATE | \
                                 RPT_TIME )

// Different kinds of Reporter messages
#define RPT_DEBUG                       'D'
#define RPT_INFO                        'I'
#define RPT_WARN                        'W'
#define RPT_ERROR                       'E'
#define RPT_MSG                         'M'
#define RPT_HEADER                      'H'
#define RPT_TEXT                        'T'

class Reporter
{
public:
  Reporter ();
   ~Reporter ();
    
  // connection methods
  CPO_BOOL  init ( const char * appName,
                             const BITFIELD prefixMask,
                             const BITFIELD destination );
  CPO_BOOL  init ( const char * appName,
                             const BITFIELD prefixMask,
                             const BITFIELD destination,
                             const char * path );
  CPO_BOOL  init ( const char * appName,
                             const BITFIELD prefixMask,
                             const BITFIELD destination,
                             const char * path,
                             const char * file,
							 const unsigned long ulOpenAttributes = OPEN_ALWAYS);

  CPO_BOOL  connected ( void );
  CPO_BOOL  active ( void );
  void     debugON ( void );
  void     debugOFF ( void );
  void     sleep ( void );
  void     wake ( void );
  void     load ( const char * filename );
  void     kill ( void );
  
  // display methods
  void     dash ( void );
  void     star ( void );
  void     blank ( void );
  void     header ( char * message, ... );
  void     msg ( char * message, ... );
  void     debug ( char * message, ... );
  void     info  ( char * message, ... );
  void     warn  ( char * message, ... );
  void     error ( char * message, ... );
  char *   text ( char * message, ... );

  void     header ( int code, ... );
  void     msg ( int code, ... );
  void     debug ( int code, ... );
  void     info  ( int code, ... );
  void     warn  ( int code, ... );
  void     error ( int code, ... );
  char *   text ( int code, ... );

private:

  // variables
  va_list pCurrent;                               // pointer to arg list
  BITFIELD state;                                 // current state flags
  BITFIELD prefix;                                // info to prepend to message
  HANDLE hFile;		                              // handle to log file
  char filename [ CPO_MAX_FILENAME_LEN + 1 ];     // name of log file
  char progname [ RPT_MAX_PROGRAM_LEN + 1 ];      // name of calling process
  int fileRead;                                   // valid list entries ?
  char *messageList [ RPT_MAX_LIST_ENTRIES ]; // list of messages
  char buffer [ RPT_OPEN_MSG_LEN ];               // common temp buffer

  // methods
  void        prepare ( void );
  CPO_BOOL     setup ( const char * appName, 
                       const BITFIELD prefixMask,
					   const unsigned long ulOpenAttributes = OPEN_ALWAYS );
  void        actOnCode ( int code, char messType );
  void        extractName ( char * file, const char * path, 
                                       const int length );
  void        generateFilename ( char * validName, 
                                            const char * appName );
  int         openFile ( unsigned long ulOpenAttributes = OPEN_ALWAYS );
  void        closeFile ( void );
  void        validateText ( char * text, const int length );
  void        addHeaderInfo ( char * prefixInfo );
  void        addLogInfo ( char * prefixInfo, const char msgType );
  void        buildMessage ( const char * message, 
                                        const char messType );
  void        writeToStream ( const char * str );
  void        readMessageFile ( const char * file );
  void        addMessage ( void );
};

#endif
