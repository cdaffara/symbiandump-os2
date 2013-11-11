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
* Description:  
*
*/




#include "stdafx.h"
#include <string.h>
#include <malloc.h>
#include <winbase.h>
#include "cReporter.h"

CRITICAL_SECTION aCriticalSection;

/*****************************************************************************
 * Basic constructor
 *****************************************************************************/

Reporter::Reporter()
: pCurrent((va_list)0), state(0), prefix(0), hFile((HANDLE)0), fileRead(0)
{
	memset(filename, 0, sizeof(filename));
	memset(progname, 0, sizeof(progname));
	memset(buffer, 0, sizeof(buffer));

	int i;
	for (i=0;i<RPT_MAX_LIST_ENTRIES;i++)
		messageList[i] = NULL;
}

/***************************************************************************
 * Destructor to free any memory we've allocated.
 ***************************************************************************/

Reporter::~Reporter ( void )
{
  kill();
}

/*****************************************************************************
 * Initialiser for Reporter using automatic filename generator.
 * 
 * Generates a filename if requested. If file doesn't exist, it is created.
 *****************************************************************************/

CPO_BOOL
Reporter::init ( const char * appName,
                 const BITFIELD prefixMask,
                 const BITFIELD destination )
{
  // must not be initialised
  if ( state & CPO_CONNECTED )
  {
    return ( CPO_FALSE );
  }

  // initialise variables
  prepare ( );
  
  // set up logging file
  state = destination;
  if ( state & RPT_FILE )
  {
    generateFilename ( filename, appName );
  }
  
  // final preparations
  if ( ! ( setup ( appName, prefixMask ) ) )
  {
    return ( CPO_FALSE );
  }

#ifdef RPT_DEBUG_PROG
    (void) fprintf ( stdout, 
                     "progname [%s] prefix %ld dest %ld file [%s] state %ld\n",
                     progname, prefixMask, destination, filename, state );
    (void) fflush ( stdout );
#endif  

  InitializeCriticalSection(&aCriticalSection);
  return ( CPO_TRUE );
}

/*****************************************************************************
 * Initialiser for Reporter using specific path for file output.
 * 
 * Opens an output file if requested at the supplied path.  
 * If the requested file doesn't exist, it is created.
 * If the path is invalid, initialisation will fail.
 *****************************************************************************/

CPO_BOOL
Reporter::init ( const char * appName,
                 const BITFIELD prefixMask,
                 const BITFIELD destination,
                 const char * path )
{
  char tmpFile [ CPO_MAX_FILENAME_LEN + 1 ];
  char tmpPath [ CPO_MAX_FILENAME_LEN + 1 ];
  
  // must not be initialised
  if ( state & CPO_CONNECTED )
  {
    return ( CPO_FALSE );
  }

// initialise variables
  prepare ( );
  
  // because we supplied a path, make sure we are setup for file output
  state = destination;
  if ( !(state & RPT_FILE) )
  {
    state += RPT_FILE;
  }

  // generate the output filename
  generateFilename ( tmpFile, appName );

  // add a backslash if there isn't one
  strcpy(tmpPath, path);
  if ((*(tmpPath + strlen(tmpPath) - 1)) != '\\')
	strcat(tmpPath, "\\");

  (void) sprintf ( filename, "%s%s", tmpPath, tmpFile );
  
  // final preparations
  if ( ! ( setup ( appName, prefixMask ) ) )
  {
    return ( CPO_FALSE );
  }

#ifdef RPT_DEBUG_PROG
    (void) fprintf ( stdout, 
                     "appname [%s] prefix %ld dest %ld file [%s] state %ld\n", 
                     appName, prefixMask, destination, filename, state );
    (void) fflush ( stdout );
#endif  
    
  InitializeCriticalSection(&aCriticalSection);
  return ( CPO_TRUE );
}

/*****************************************************************************
 * Initialiser for Reporter using specific path and filename for output.
 * 
 * Opens an output file using supplied name at supplied path.  
 * If the requested file doesn't exist, it is created.
 * If the path is invalid, initialisation will fail.
 *****************************************************************************/

CPO_BOOL
Reporter::init ( const char * appName,
                 const BITFIELD prefixMask,
                 const BITFIELD destination,
                 const char * path,
                 const char * file,
				 const unsigned long ulOpenAttributes)
{
  char tmpFile [ CPO_MAX_FILENAME_LEN + 1 ];
  char tmpPath [ CPO_MAX_FILENAME_LEN + 1 ];

  // must not be initialised
  if ( state & CPO_CONNECTED )
  {
    return ( CPO_FALSE );
  }

 // initialise variables
  prepare ( );
  
  // because we supplied a filename, make sure we are setup for file output
  state = destination;
  if ( ! ( state & RPT_FILE ) )
  {
    state += RPT_FILE;
  }

  // get the actual filename if a full path has been supplied
  extractName ( tmpFile, file, CPO_MAX_FILENAME_LEN );

  // add a backslash if there isn't one
  strcpy(tmpPath, path);
  if ((*(tmpPath + strlen(tmpPath) - 1)) != '\\')
	strcat(tmpPath, "\\");

  (void) sprintf ( filename, "%s%s", tmpPath, tmpFile );
  
  // final preparations
  if ( ! ( setup ( appName, prefixMask, ulOpenAttributes ) ) )
  {
    return ( CPO_FALSE );
  }

#ifdef RPT_DEBUG_PROG
    (void) fprintf ( stdout, 
                     "appname [%s] prefix %ld dest %ld file [%s] state %ld\n", 
                     appName, prefixMask, destination, filename, state );
    (void) fflush ( stdout );
#endif  
    
  InitializeCriticalSection(&aCriticalSection);
  return ( CPO_TRUE );
}

/*****************************************************************************
 * Append a 'Warning' message to file
 *****************************************************************************/

void
Reporter::warn ( char * message, ... )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    va_start ( pCurrent, message );
    buildMessage ( message, RPT_WARN );
    va_end ( pCurrent );
	LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append an 'Error' message to file
 *****************************************************************************/

void
Reporter::error ( char * message, ... )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    va_start ( pCurrent, message );
    buildMessage ( message, RPT_ERROR );
    va_end ( pCurrent );
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append a 'Info' message to file
 *****************************************************************************/

void
Reporter::info ( char * message, ... )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    va_start ( pCurrent, message );
    buildMessage ( message, RPT_INFO );
    va_end ( pCurrent );
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append a 'Debug' message to file
 *****************************************************************************/

void
Reporter::debug ( char * message, ... )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && 
       ( state & CPO_ACTIVATED ) && 
       ( state & RPT_DEBUG_ON ) )
  {
    EnterCriticalSection(&aCriticalSection);
    va_start ( pCurrent, message );
    buildMessage ( message, RPT_DEBUG );
    va_end ( pCurrent );
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append a message to output stream with no accompanying information.
 *****************************************************************************/

void
Reporter::msg ( char * message, ... )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    va_start ( pCurrent, message );
    buildMessage ( message, RPT_MSG );
    va_end ( pCurrent );
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Write a special message containing day, mth, year, and time with the text.
 *****************************************************************************/

void
Reporter::header ( char * message, ... )
{
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    va_start ( pCurrent, message );
    buildMessage ( message, RPT_HEADER );
    va_end ( pCurrent );
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Return the text associated with a particular code.
 *****************************************************************************/

char *
Reporter::text ( char * message, ... )
{
  *( buffer ) = '\0';
  
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    va_start ( pCurrent, message );
    buildMessage ( message, RPT_TEXT );
    va_end ( pCurrent );
    LeaveCriticalSection(&aCriticalSection);
  }

  return ( buffer );
}

/*****************************************************************************
 * Append a 'Warning' message to file from message list.
 *****************************************************************************/

void
Reporter::warn ( int code, ... )
{
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
	  EnterCriticalSection(&aCriticalSection);

	  va_start ( pCurrent, code );
	  actOnCode ( code, RPT_WARN );
	  va_end ( pCurrent );

	  LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append an 'Error' message to file from message list.
 *****************************************************************************/

void
Reporter::error ( int code, ... )
{
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
	  EnterCriticalSection(&aCriticalSection);

	  va_start ( pCurrent, code );
	  actOnCode ( code, RPT_ERROR );
	  va_end ( pCurrent );

	  LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append a 'Info' message to file from message list.
 *****************************************************************************/

void
Reporter::info ( int code, ... )
{
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
	  EnterCriticalSection(&aCriticalSection);

	  va_start ( pCurrent, code );
	  actOnCode ( code, RPT_INFO );
	  va_end ( pCurrent );

	  LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append a 'Debug' message to file from message list.
 *****************************************************************************/

void
Reporter::debug ( int code, ... )
{
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
	  EnterCriticalSection(&aCriticalSection);

	  if ( state & RPT_DEBUG_ON )
	  {
		va_start ( pCurrent, code );
		actOnCode ( code, RPT_DEBUG );
		va_end ( pCurrent );
	  }

	  LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append a message to output stream with no accompanying information.
 *****************************************************************************/

void
Reporter::msg ( int code, ... )
{
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
	  EnterCriticalSection(&aCriticalSection);

	  va_start ( pCurrent, code );
	  actOnCode ( code, RPT_MSG );
	  va_end ( pCurrent );

	  LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Write a special message containing day, mth, year, and time with the text.
 *****************************************************************************/

void
Reporter::header ( int code, ... )
{
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
	  EnterCriticalSection(&aCriticalSection);

	  va_start ( pCurrent, code );
	  actOnCode ( code, RPT_HEADER );
	  va_end ( pCurrent );

	  LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Return the text associated with a particular code.
 *****************************************************************************/

char *
Reporter::text ( int code, ... )
{
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
	  EnterCriticalSection(&aCriticalSection);

	  *( buffer ) = '\0';
  
	  va_start ( pCurrent, code );
	  actOnCode ( code, RPT_TEXT );
	  va_end ( pCurrent );

	  LeaveCriticalSection(&aCriticalSection);
  }
  return ( buffer );
}

/*****************************************************************************
 * Append a dashed line to file
 *****************************************************************************/

void
Reporter::dash ( void )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    writeToStream ( "----------------------------------------"
                    "---------------------------------------" );
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append a star line to file
 *****************************************************************************/

void
Reporter::star ( void )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    writeToStream ( "****************************************"
                    "***************************************" );
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Append a blank line to file
 *****************************************************************************/

void
Reporter::blank ( void )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    writeToStream ( " " );
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Shutdown the Reporter
 * Puts a message into the logging file that the Reporter has shutdown.
 * Closes the logging file.
 *****************************************************************************/

void
Reporter::kill ( void )
{
  // must be initialised
  if ( state & CPO_CONNECTED )
  {
    EnterCriticalSection(&aCriticalSection);
    if ( state & RPT_FILE )
    {
      closeFile ( );
    }

    // clear in case actions are tried on an uninitialised Reporter
    state = CPO_CLEAR;

    // free any memory allocated
    if ( fileRead )
    {
      for (int i=0;i<RPT_MAX_LIST_ENTRIES;i++)
      {
        if ( messageList [ i ] )
        {
          //delete ( messageList [ i ] );
          free ( messageList [ i ] );
        }
      }
    }
    LeaveCriticalSection(&aCriticalSection);
  }

  DeleteCriticalSection(&aCriticalSection);
}

/*****************************************************************************
 * Return whether the Reporter is currently initialised or not.
 *****************************************************************************/

CPO_BOOL
Reporter::connected ( void )
{
  return ( (BOOLEAN) state & CPO_CONNECTED );
}

/*****************************************************************************
 * Return whether the Reporter is currently active or not.
 *****************************************************************************/

CPO_BOOL
Reporter::active ( void )
{
  return ( (BOOLEAN) state & CPO_ACTIVATED );
}

/*****************************************************************************
 * Suspend the Reporter
 *****************************************************************************/

void
Reporter::sleep ( void )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    state -= CPO_ACTIVATED;
    
    if ( state & RPT_FILE )
    {
      closeFile ( );
    }
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Re-activate the Reporter
 *****************************************************************************/

void
Reporter::wake ( void )
{
  // must be initialised but not activated
  if ( ( state & CPO_CONNECTED ) && ! ( state & CPO_ACTIVATED ) )
  {
    EnterCriticalSection(&aCriticalSection);
    state += CPO_ACTIVATED;
    
    if ( state & RPT_FILE && !openFile ( ) )
    {
      // couldn't reopen the file so stop outputting to it
      state -= RPT_FILE;

      // no output left so shutdown Reporter
      if ( !( state & RPT_OUTPUT_MASK ) )
      {
        kill ( );
      }
    }
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Allow debug messages.
 *****************************************************************************/

void
Reporter::debugON ( void )
{
  if ( state & CPO_CONNECTED )
  {
    EnterCriticalSection(&aCriticalSection);
    if ( !( state & RPT_DEBUG_ON ) )
    {
      state += RPT_DEBUG_ON;
      debug ( "DEBUG messages will be processed" );
    }
    LeaveCriticalSection(&aCriticalSection);
  }
}

/*****************************************************************************
 * Refuse debug messages.
 *****************************************************************************/

void
Reporter::debugOFF ( void )
{
  if ( state & CPO_CONNECTED )
  {
    EnterCriticalSection(&aCriticalSection);
    if ( state & RPT_DEBUG_ON )
    {
      debug ( "DEBUG messages will not be processed" );
      state -= RPT_DEBUG_ON;
    }
    LeaveCriticalSection(&aCriticalSection);
  }
}

/***************************************************************************
 * Loads the contents of a message file into memory.
 ***************************************************************************/

void
Reporter::load ( const char * file )
{
  if ( state & CPO_CONNECTED )
  {
    EnterCriticalSection(&aCriticalSection);
    readMessageFile ( file );
    if ( fileRead )
    {
      debug ( "Messages loaded from %s", file );
    }
    else
    {
      debug ( "No messages loaded from %s", file );
    }
    
#ifdef RPT_DEBUG_PROG
    if ( fileRead )
    {
      for (int i=0;i<RPT_MAX_LIST_ENTRIES;i++)
      {
        if ( messageList [ i ] )
        {
          (void) fprintf ( stdout, "%d : [%s] %d\n", 
                           i, messageList [ i ], 
                           strlen ( messageList [ i ] ) );
        }
      }
    }
#endif
    LeaveCriticalSection(&aCriticalSection);
  }
}

/***************************************************************************/
