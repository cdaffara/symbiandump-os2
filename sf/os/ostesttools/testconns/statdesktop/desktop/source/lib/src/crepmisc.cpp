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
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cReporter.h"


/*****************************************************************************
 * Reporter preparation
 * 
 * Sets member strings to a known state
 *****************************************************************************/

void
Reporter::prepare ( void )
{
  int i;
  
#ifdef RPT_DEBUG_PROG
  // set filename contents to garbage value
  for (i=0;i<CPO_MAX_FILENAME_LEN;i++)
  {
    filename [ i ] = CPO_GARBAGE_CHAR;
  }
  
  // set program name contents to garbage value
  for (i=0;i<RPT_MAX_PROGRAM_LEN;i++)
  {
    progname [ i ] = CPO_GARBAGE_CHAR;
  }
#endif  

  for (i=0;i<RPT_MAX_LIST_ENTRIES;i++)
  {
    messageList [ i ] = (char *)0;
  }
  
  fileRead = 0;
  pCurrent = (va_list) 0;
  prefix = 0;
  state = 0;
  hFile = (HANDLE) 0;
  filename [ 0 ] = '\0';
  progname [ 0 ] = '\0';
}

/*****************************************************************************
 * Reporter setup
 * 
 * Checks the output streams.
 * Sets the prefix mask.
 * Stores a copy of the application name.
 * Puts a message into the logging file that the Reporter is initialised.
 *****************************************************************************/

CPO_BOOL
Reporter::setup ( const char * appName,
                  const BITFIELD prefixMask,
				  const unsigned long ulOpenAttributes )
{
  // check for valid output destinations
  if ( !( state & RPT_FILE ) && 
       !( state & RPT_STDOUT ) &&
       !( state & RPT_STDERR ) )
  {
    return ( CPO_FALSE );
  }  

  // check for valid mask
  if ( !( prefixMask & RPT_MSG_TYPE ) && 
       !( prefixMask & RPT_APPNAME ) &&
       !( prefixMask & RPT_DATE ) &&
       !( prefixMask & RPT_TIME ) )
  {
    prefix = RPT_MSG_ONLY;
  }  
  else
  {
    prefix = prefixMask;
  }
  
  // get the actual program name and make sure it is valid
  extractName ( progname, appName, RPT_MAX_PROGRAM_LEN );
  validateText ( progname, RPT_MAX_PROGRAM_LEN );
  if ( !( *progname ) )
  {
    return ( CPO_FALSE );
  }

  if ( state & RPT_FILE )
  {
    // open output file
    if ( !openFile ( ulOpenAttributes ) )
    {
      state -= RPT_FILE;
      if ( !( state & RPT_OUTPUT_MASK ) )
      {
        return ( CPO_FALSE );
      }
    }
  }
  
  // set flags to show everything ok
  state |= CPO_CONNECTED;
  state |= CPO_ACTIVATED;
  
  return ( CPO_TRUE );
}

/*****************************************************************************
 * Resolve a code to a message and build it.
 *****************************************************************************/

void
Reporter::actOnCode ( int code, char messType )
{
  // must be initialised and activated
  if ( ( state & CPO_CONNECTED ) && 
       ( state & CPO_ACTIVATED ) &&
       code >= 0 && 
       code < RPT_MAX_LIST_ENTRIES )
  {
    if ( messageList [ code ] )
    {
      buildMessage ( messageList [ code ], messType );
    }
    else
    {
      char tmpMsg [ 128 ];
      (void) sprintf ( tmpMsg, "[ No message text for code %d ]", code );
      buildMessage ( tmpMsg, messType );
    }
  }
}

/*****************************************************************************
 * Generate a filename using the application name and current date.
 *****************************************************************************/

void
Reporter::generateFilename ( char * validName, const char * appName )
{
  /* Construct a filename for the logging to be written to.
   * Filename is of the format x[xxx]9988.rpt where :
   * 
   * xxxx = up to first 4 chars of calling program's name
   * 99 = 2 digit day of month
   * 88 = 2 digit month of year
   */

  const int PROG_LEN = 4;
  time_t curtime;
  struct tm *tminfo;
  
  // get filename from path
  extractName ( buffer, appName, PROG_LEN );
  
  (void) time ( &curtime );
  tminfo = localtime ( &curtime );

  (void) sprintf ( validName, 
                   "%s%02d%02d.RPT", 
                   buffer, 
                   tminfo->tm_mday, 
                   tminfo->tm_mon + 1);

#ifdef RPT_DEBUG_PROG
  (void) fprintf ( stdout, "filename [%s] generated from [%s]\n", 
                   validName, appName );
  (void) fflush ( stdout );
#endif  
}

/*****************************************************************************
 * Extract 'length' characters of the actual filename from the path.
 *****************************************************************************/

void
Reporter::extractName ( char * file, const char * path, const int length )
{
  char *ptr;

  // find the last backslash
  ptr = strrchr ( path, '\\' );

  if ( !ptr )
  {
    strncpy ( file, path, length );
  }
  else
  {
    strncpy ( file, (++ptr), length );
  }
  
  *( file + length ) = '\0';
}

/****************************************************************************
 * Open the file or print a message if we can't
 *****************************************************************************/

int 
Reporter::openFile ( unsigned long ulOpenAttributes )
{
#ifdef UNICODE
	TCHAR uFilename[CPO_MAX_FILENAME_LEN + 1];

    // Convert to UNICODE.
    if (!MultiByteToWideChar(CP_ACP,					// conversion type
							 0,							// flags
							 filename,					// source
							 -1,						// length
							 uFilename,					// dest
							 CPO_MAX_FILENAME_LEN))		// length
    {
		return ( CPO_FALSE );
    }

	hFile = CreateFile(uFilename,
				   GENERIC_WRITE,
				   FILE_SHARE_READ | FILE_SHARE_WRITE,
				   NULL,
				   ulOpenAttributes,
				   FILE_ATTRIBUTE_NORMAL,
				   NULL);
#else
	hFile = CreateFile(filename,
				   GENERIC_WRITE,
				   FILE_SHARE_READ | FILE_SHARE_WRITE,
				   NULL,
				   ulOpenAttributes,
				   FILE_ATTRIBUTE_NORMAL,
				   NULL);
#endif

	if (hFile == INVALID_HANDLE_VALUE)
	{
//		char szTemp[500] = {0};
//		sprintf(szTemp, "WARNING : [%s] could not be opened. File output disabled", filename);
//		MessageBox(NULL, szTemp, NULL, MB_OK);
		return ( CPO_FALSE );
	}

	// move to the end if a file already exists
	SetFilePointer(hFile, 0, 0, FILE_END);

#ifdef RPT_DEBUG_PROG
  (void) fprintf ( stdout, "file [%s] opened\n", filename );
  (void) fflush ( stdout );
#endif  

  return ( CPO_TRUE );
}

/*****************************************************************************
 * Construct the parts of the complete message.
 *****************************************************************************/

void
Reporter::buildMessage ( const char * message, const char messType )
{
  char prefixInfo [ RPT_MAX_MSG_LEN + 1 ];
  char finalStr [ RPT_OPEN_MSG_LEN + RPT_MAX_MSG_LEN + 1 ];
  int maxlen;

  // get prefix information
  switch ( messType )
  {
   case RPT_TEXT:
   case RPT_MSG:
     *( prefixInfo ) = '\0';
     break;
   case RPT_HEADER:
     addHeaderInfo ( prefixInfo );
     break;
   default:
     addLogInfo ( prefixInfo, messType );
     break;
  }
  
  // parse arguments passed with message
  (void) vsprintf ( buffer, message, pCurrent );

  // get max length of output string
  if ( messType == RPT_DEBUG || messType == RPT_MSG || 
       messType == RPT_ERROR || messType == RPT_TEXT )
  {
    maxlen = RPT_OPEN_MSG_LEN - (int) strlen ( prefixInfo ) - 1;
  }
  else
  {
    maxlen = RPT_MAX_MSG_LEN - (int) strlen ( prefixInfo ) - 1;
  }

  // make sure we are dealing with a valid message of correct length
  validateText ( buffer, maxlen );

  // generate the output string
  (void) sprintf ( finalStr,
                   "%s%s",
                   prefixInfo, 
                   buffer );

  if ( messType == RPT_TEXT )
  {
    (void) strcpy ( buffer, finalStr );
  }
  else
  {
    writeToStream ( finalStr );
  }
}

/*****************************************************************************
 * Validate the supplied string for zero length and unprintable chars up to 
 * length bytes.
 * 
 * If unprintable chars are found embedded in the text, the text will be
 * truncated at that point.  
 *
 * If the text is longer than the specified length, it will be truncated at
 * that length.
 *
 * If the text is empty or the 1st char is unprintable, the function will
 * insert replacement 'invalid' text.
 *****************************************************************************/

void
Reporter::validateText ( char * text, const int length )
{
  int len = (int) strlen ( text );
  if ( len > length )
  {
    len = length;
  }

  // check for empty message or unprintable 1st char
  if ( len && isprint ( text [ 0 ] ) )
  {
    // check message for unprintable characters
    for (int i=1;i<len;i++)
    {
		if (text [ i ] != '\t')
		{
		  if ( ! ( isprint ( text [ i ] ) ) )
		  {
			break;
		  }
		}
    }

    text [ i ] = '\0';
  }
  else
  {
    (void) strcpy ( text, "####" );
  }
}

/*****************************************************************************
 * Generate the prefix information to prepend to a 'write' message.
 *
 * NOTE: No checks are done to ensure prefix info is not too small for 
 *       allocated string.  If more information is prepended in the future, 
 *       this will have to be implemented.
 *****************************************************************************/

void
Reporter::addHeaderInfo ( char * prefixInfo )
{
  time_t curTime;
  
  // create the time string
  (void) time ( &curTime );
  (void) strftime ( buffer, 
                    sizeof ( buffer ), 
                    "%a %b %d %Y  %H:%M:%S",
                    localtime ( &curTime ) );

  // append the program name
  (void) sprintf ( prefixInfo,
                   "%s [%s] ",
                   buffer, 
                   progname );
}

/*****************************************************************************
 * Generate the prefix information to prepend to the message.
 * If msg type is RPT_ERROR, info is generated regardless of prefix setting.
 *
 * NOTE: No checks are done to ensure prefix info is not too small for 
 *       allocated string.  If more information is prepended in the future, 
 *       this will have to be implemented.
 *****************************************************************************/

void
Reporter::addLogInfo ( char * prefixInfo, const char msgType )
{
  int bytesUsed = 0;
  time_t curTime;
  *( prefixInfo ) = '\0';

  if ( prefix == RPT_MSG_ONLY)
  {
    return;
  }
  
#ifdef RPT_DEBUG_PROG
  // fill prefix string to check for memory leaks
  for (int i=1;i<RPT_MAX_MSG_LEN;i++)
  {
    *( prefixInfo + i ) = CPO_GARBAGE_CHAR;
  }
#endif  

  if ( ( prefix & RPT_MSG_TYPE ) )
  {
    *( prefixInfo ) = msgType;
    *( prefixInfo + 1 ) = ' ';
    *( prefixInfo + 2 ) = '\0';

    bytesUsed += 2;
  }
  
  if ( ( prefix & RPT_APPNAME ) )
  {
    (void) sprintf ( prefixInfo + bytesUsed, "%s ", progname );
    bytesUsed += (int) strlen ( progname ) + 1;
  }
  
  if ( ( prefix & RPT_DATE ) )
  {
    const int sublen = 5;

    // create the date string
    (void) time ( &curTime );
    (void) strftime ( buffer, 
                      sizeof ( buffer ), 
                      "%d/%m",
                      localtime ( &curTime ) );

    (void) sprintf ( prefixInfo + bytesUsed, "%s ", buffer );
    bytesUsed += ( sublen + 1 );
  }
  
  if ( ( prefix & RPT_TIME ) )
  {
    const int sublen = 8;

    // create the time string
    (void) time ( &curTime );
    (void) strftime ( buffer, 
                      sizeof ( buffer ), 
                      "%H:%M:%S",
                      localtime ( &curTime ) );

    (void) sprintf ( prefixInfo + bytesUsed, "%s ", buffer );
    bytesUsed += ( sublen + 1 );
  }

  // null terminate to be sure
  *( prefixInfo + bytesUsed ) = '\0';
}

/*****************************************************************************
 * Send text to appropriate stream(s).
 *****************************************************************************/

void
Reporter::writeToStream ( const char * str )
{
#ifdef RPT_DEBUG_PROG
  (void) fprintf ( stdout, "output [%s]\n", str );
  (void) fflush ( stdout );
#endif  

  // write output string to file
  if ( state & RPT_FILE )
  {
	  unsigned long lBytesWritten;

	  // need to seek to end of file in case other handles have written in the meantime
	  SetFilePointer(hFile, 0, NULL, FILE_END);

	  WriteFile(hFile, str, strlen(str), &lBytesWritten, NULL);
	  WriteFile(hFile, "\r\n", 2, &lBytesWritten, NULL);

	  // in case other processes are using this file to log
	  FlushFileBuffers(hFile);
  }

  // write output string to standard output
  if ( state & RPT_STDOUT )
  {
    (void) fprintf ( stdout, "%s\n", str );
    (void) fflush ( stdout );
  }
  
  // write output string to standard error
  if ( state & RPT_STDERR )
  {
    (void) fprintf ( stderr, "%s\n", str );
    (void) fflush ( stderr );
  }
}

/*****************************************************************************
 * Close the output file stream.
 *****************************************************************************/

void 
Reporter::closeFile ( void )
{
	CloseHandle(hFile);

#ifdef RPT_DEBUG_PROG
  (void) fprintf ( stdout, "file [%s] closed\n", filename );
  (void) fflush ( stdout );
#endif  
}

/***************************************************************************
 * Open the message file, then read in contents, processing a line at a
 * time, putting the current line into global buffer string.
 ***************************************************************************/

void
Reporter::readMessageFile ( const char * file )
{
  FILE * finptr;
  
  // open initialisation file
  if ( ( finptr = fopen ( file, "r" ) ) == NULL )
  {
    return;
  }
  
  // read each line
  while ( ( fgets ( buffer, RPT_MAX_LINE_LEN, finptr ) ) != NULL )
  {
    if ( *( buffer ) != '\n' && *( buffer ) != '#' )
    {
      addMessage ( );
    }
  }

  (void) fclose ( finptr );
}

/***************************************************************************
 * Split the line of text into code and message and allocate memory in
 * list and store it.
 ***************************************************************************/

void
Reporter::addMessage ( void )
{
  char *pCode;
  char *pText;
  int code;

  // extract the code and text
  pCode = strtok ( buffer, ":" );
  pText = strtok ( NULL, "\n" );

  // check for legitimate values
  if ( !pCode || !pText )
  {
    return;
  }

  code = atoi ( pCode );
  if ( code > RPT_MAX_LIST_ENTRIES || messageList [ code ] )
  {
    return;
  }
  
  // jump any spaces in front of text
  while ( *( pText ) == ' ' || *( pText ) == '\t' )
  {
    pText++;
  }
  
  // copy string
  // messageList [ code ] = new char ( strlen ( pText ) + 1 );
  messageList [ code ] = 
    (char *) malloc ( sizeof ( char ) * ( strlen ( pText ) + 1 ) );

  (void) strcpy ( messageList [ code ], pText );

#ifdef RPT_DEBUG_PROG
//   (void) fprintf ( stdout, "code [%s] text [%s] entry [%s] len %d\n", 
//                    pCode, pText, messageList [ code ], strlen ( pText ) );
//   (void) fflush ( stdout );
#endif

  // let object know there is at least one message entry
  fileRead = 1;
}

/****************************************************************************/

