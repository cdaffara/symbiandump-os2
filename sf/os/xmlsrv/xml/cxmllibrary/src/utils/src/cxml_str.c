/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/***************************************************************************
**   File: cxml_str.c
**   Purpose:  Provides the implementation to CXML string API
**************************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/

#include "cxml_internal.h"
#include <xml/cxml/cxml_nw2cxmlTypes.h>


static NW_Uint16 CXML_Str_Strlen(const CXML_Ucs2 *string)
{
  NW_Uint16 len = 0;
  NW_ASSERT(string != NULL);

  while (*string != NULL) 
  {
    string++;
    len++;
  }
  return len;
}

static NW_Uint32 NW_Asc_strlen(const char *s)
{
  NW_Uint32 i = 0;
  while (*s != '\0')
  {
    s++;
    i++;
  }
  return i;
}

/*****************************************************************
**  Name:  CXML_Str_ToLower
**  Description:  Converts a CXML_Ucs2 to lower case
**  Parameters:   ch - the character to convert
**  Return Value: the converted character
******************************************************************/
CXML_Ucs2 CXML_Str_ToLower(const CXML_Ucs2 ch)
{
  if((ch >= CXML_ASCII_UPPER_A) && (ch <= CXML_ASCII_UPPER_Z)) {
    return (CXML_Ucs2)(ch - CXML_ASCII_UPPER_A + CXML_ASCII_LOWER_A);
  } else {
    return ch;
  }
}

/*****************************************************************
**  Name:  CXML_Str_ToUpper
**  Description:  Converts a NW_Ucs2 to upper case
**  Parameters:   ch - the character to convert
**  Return Value: the converted character
******************************************************************/
CXML_Ucs2 CXML_Str_ToUpper(const NW_Ucs2 ch)
{
  if((ch >= CXML_ASCII_LOWER_A) && (ch <= CXML_ASCII_LOWER_Z)) {
    return (NW_Ucs2)(ch - CXML_ASCII_LOWER_A + CXML_ASCII_UPPER_A);
  } else {
    return ch;
  }
}

/*****************************************************************
**  Name:  CXML_Str_Isspace
**  Description:  Checks if the character is a particular 
**                representation of a space character
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a white-space character, else 0
******************************************************************/
CXML_Int32 CXML_Str_Isspace(const CXML_Ucs2 ch)
{
  return ((ch == CXML_ASCII_SPACE) || (ch == CXML_ASCII_CR) ||
          (ch == CXML_ASCII_LF) || (ch == CXML_ASCII_FF) ||
          (ch == CXML_ASCII_HT) || (ch == CXML_ASCII_VT));
}


/*****************************************************************
**  Name:  CXML_Str_Isdigit
**  Description:  Checks if the character is a particular 
**                representation of a decimal-digit character.
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a decimal-digit character, else 0
******************************************************************/
CXML_Int32 CXML_Str_Isdigit(const CXML_Ucs2 ch)
{
  return ( (ch >= CXML_ASCII_0) && (ch <= CXML_ASCII_9) );
}


/*****************************************************************
**  Name:  CXML_Str_Isxdigit
**  Description:  Checks if the character is a particular 
**                representation of a hexadecimal-digit character.
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a hexadecimal-digit character, else 0
******************************************************************/
CXML_Int32 CXML_Str_Isxdigit(const CXML_Ucs2 ch)
{
  return (CXML_Str_Isdigit(ch) ||
          ( (ch >= CXML_ASCII_UPPER_A) && (ch <= CXML_ASCII_UPPER_F) ) ||
          ( (ch >= CXML_ASCII_LOWER_A) && (ch <= CXML_ASCII_LOWER_F) ) );
}

/*****************************************************************
**  Name:  CXML_Str_CvtToAscii
**  Description:  Convert ucs2 string to new null-terminated ascii string
**  Parameters:   *string - Null-terminated ucs2 string
**  Return Value: pointer to the new ascii string or NULL
******************************************************************/
CXML_Int8 *CXML_Str_CvtToAscii(const CXML_Ucs2 *string)
{
  NW_Int32 i, size;
  CXML_Int8* res = NULL;

  NW_ASSERT(string);

  size = CXML_Str_Strlen(string) + 1;
  res = (CXML_Int8*) NW_Mem_Malloc((NW_Uint32)size * sizeof(CXML_Int8));
  /* this is a really naive conversion to ascii */
  if (res != NULL) {
    for (i=0; i < size; i++) {
      res[i] = (CXML_Int8)(string[i] & 0xFF);
    }
  }
  return res;
}


CXML_Uint32 CXML_Asc_strlen(const CXML_Int8 *s)
{
  NW_Uint32 i = 0;
  while (*s != '\0')
  {
    s++;
    i++;
  }
  return i;
}


/*****************************************************************
**  Name:  NW_Str_CvtFromAscii
**  Description:  Convert null-terminated ascii string to new ucs2 string
**  Parameters:   *string - Null-terminated ascii string
**  Return Value: pointer to new ucs2 string or NULL
******************************************************************/
CXML_Ucs2 *CXML_Str_CvtFromAscii(const CXML_Int8 *string)
{
  NW_Uint32 i, size;
  NW_Ucs2* res = NULL;

  NW_ASSERT(string);

  size = NW_Asc_strlen((const char*) string) + 1;
  res = (NW_Ucs2*) NW_Mem_Malloc(size * sizeof(NW_Ucs2));
  /* this is a really naive conversion from ascii */
  if (res != NULL) {
    for (i=0; i < size; i++) {
      res[i] = string[i];
    }
  }
  return res;
}


/*****************************************************************
**  Name:  CXML_Str_Strsize
**  Description:  Get the size of the string
**  Parameters:   *string - Null-terminated string
**  Return Value: number of bytes in string including terminator
******************************************************************/
CXML_Uint16 CXML_Str_Strsize(const CXML_Ucs2 *string)
{
  NW_Uint16 len;

  NW_ASSERT(string != NULL);

  len = CXML_Str_Strlen(string);
  return NW_UINT16_CAST((len + 1) * sizeof(NW_Ucs2));
}

/*****************************************************************
**  Name:  CXML_Str_StrcmpConst
**  Description:  Compare a string to an Ascii string constant
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**  Return Value: <0 if string1 < string2, 0 if equal, >0 if string1 > string2
******************************************************************/

CXML_Int32 CXML_Str_StrcmpConst(const CXML_Ucs2 *string1,const CXML_Ucs2 *string2)
{
  NW_Int32 i = 0;
  NW_Int32 ch1;
  NW_Int32 ch2;

  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  do {
    ch1 = string1[i];
    ch2 = string2[i];
    i++;
  } while (((ch1 - ch2) == 0) && (ch1 != CXML_ASCII_NULL));

  return (ch1 - ch2);
}

/*****************************************************************
**  Name:  CXML_Str_Stricmp
**  Description:  Compare two strings, case-insensitve
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**  Return Value: <0 if string1 < string2, 0 if equal, >0 if string1 > string2
******************************************************************/
CXML_Int32 CXML_Str_Stricmp(const CXML_Ucs2 *string1, 
                             const CXML_Ucs2 *string2)
{
  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);


  while ((CXML_Str_ToUpper(*string1) == CXML_Str_ToUpper(*string2)) &&
         (*string1 != CXML_ASCII_NULL) && (*string2 != CXML_ASCII_NULL))
  {
    string1++;
    string2++;
  }
  return (CXML_Str_ToUpper(*string1) - CXML_Str_ToUpper(*string2));
}

/*****************************************************************
**  Name:  CXML_Asc_strcpy
**  Description:  Copy null terminated string 
**  Parameters:   *s2 - Null-terminated string to copy
**                *s1 - Resultant String
**  Return Value: Copied string
******************************************************************/

CXML_Int8 *CXML_Asc_strcpy(CXML_Int8 *s1, const CXML_Int8 *s2)
{
  while (*s2 != '\0') {
    *s1 = *s2;
    s1++;
    s2++;
  }
  *s1 = *s2;
  return s1;
}

/*****************************************************************
**  Name:  CXML_Asc_strcat
**  Description:  Concatenate null terminated string 
**  Parameters:   *s2 - Null-terminated string to copy
**                *s1 - Resultant String Null terminated
**  Return Value: Resultant string
******************************************************************/

CXML_Int8 *CXML_Asc_strcat(CXML_Int8 *s1, const CXML_Int8 *s2)
{
  CXML_Int8 *myDest = s1;
  while (*myDest != '\0') {
   myDest++;
  }
  CXML_Asc_strcpy(myDest, s2);
  return s1;
}

/*****************************************************************
**  Name:  CXML_Asc_Strncpy
**  Description:  Copy characters of one string to another
**  Parameters:   *destination - Destination string
**                *source - Source string
**                count -  Maximum number of characters to be copied
**  Return Value: pointer to destination string 
******************************************************************/
CXML_Int8 *CXML_Asc_strncpy(CXML_Int8 *destination,
                          const CXML_Int8 *source,
                          const CXML_Uint32 count )
{
  CXML_Uint32 nCopied = 0;
  
  NW_ASSERT(destination != NULL);
  NW_ASSERT(source != NULL);

  while ( (nCopied < count) && (*source != CXML_ASCII_NULL ) ) {
    *destination = *source;
    destination++;
    source++;
    nCopied++;
  }

  /*
  ** ANSI 7.11.2.4 says nulls are appended till count characters in all have
  ** been copied
  */
  while (nCopied < count) {
    *destination = CXML_ASCII_NULL;
    destination++;
    nCopied++;
  }
  return destination;
}

/*****************************************************************
**  Name:  CXML_Asc_strncat
**  Description: Concatenate characters of one string to another
**  Parameters:   *destination - Destination string
**                *source - Source string
**                count -  Maximum number of characters to be copied
**  Return Value: pointer to destination string
******************************************************************/
CXML_Int8 *CXML_Asc_strncat(CXML_Int8 *destination,
                          const CXML_Int8 *source,
                          const NW_Uint32 count )
{
  CXML_Uint32 nCount = 0;
  
  NW_ASSERT(destination != NULL);
  NW_ASSERT(source != NULL);

  while(*destination != '\0')
  {
   destination++;
  }


  while ( (nCount < count) && (*source != CXML_ASCII_NULL ) ) {
    *destination = *source;
    destination++;
    source++;
    nCount++;
  }

  /*
  ** ANSI 7.11.2.4 says nulls are appended till count characters in all have
  ** been copied
  */
  while (nCount < count) {
    *destination = CXML_ASCII_NULL;
    destination++;
    nCount++;
  }
  return destination;
}




