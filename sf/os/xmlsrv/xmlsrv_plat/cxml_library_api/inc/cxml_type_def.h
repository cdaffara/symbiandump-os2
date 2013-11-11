/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CXML_TYPE_DEF_H
#define CXML_TYPE_DEF_H

#warning The CXML API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

/*
 *  ANSI/ISO C header files needed to define basic data types
 * (ie. NULL, etc)
 */
#include <stdlib.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Data types in cXML */
//typedef unsigned char           bool8;
typedef unsigned char           byte;
typedef unsigned short int      word;
typedef unsigned long int       dword;

typedef unsigned char           uint8;
typedef unsigned short int      uint16;
typedef unsigned long int       uint32;

typedef signed   char           int8;
typedef signed   short int      int16;
typedef signed   long int       int32;

typedef unsigned char         CXML_Bool;        /* NW_FALSE, NW_TRUE  */
typedef byte          CXML_Byte;        /* 0 ... 0xFF         */
typedef word          CXML_Word;        /* 0 ... 0xFFFF       */
typedef dword         CXML_Dword;       /* 0 ... 0xFFFFFFFF   */

typedef uint8         CXML_Uint8;       /* 0 ... 255          */
typedef uint16        CXML_Uint16;      /* 0 ... 65535        */
typedef uint32        CXML_Uint32;      /* 0 ... 4294967295   */

typedef int8          CXML_Int8;        /* -128 ... +127      */
typedef int16         CXML_Int16;       /* -32768 ... +32767  */
typedef int32         CXML_Int32;       /* -2147483648 ... +2147483647 */

typedef unsigned short CXML_Ucs2;       /* 0 ... 65535        */

typedef CXML_Uint32 NW_TinyTree_Offset_t;

#define CXML_ASSERT(expr) assert(((CXML_Uint32)(expr)) != 0)
/* Use following to catch the ASSERT in Wins
#if defined(__WINS__) && !defined(__WINSCW__)
int kimono_assert ();
#define CXML_ASSERT(expr) ((((CXML_Uint32)(expr)) != 0) || kimono_assert())
#else
#define CXML_ASSERT(expr) assert(((CXML_Uint32)(expr)) != 0)
#endif
*/

#define CXML_TRUE         ((CXML_Bool)  0x01U)
#define CXML_FALSE        ((CXML_Bool)  0x00U)

#define CXML_UINT16_MAX   ((NW_Uint16) 0xFFFFU)
#define CXML_UINT32_MAX   ((NW_Uint32) 0xFFFFFFFFU)
#define CXML_INT32_MIN    ((NW_Int32)  0x80000000U)
#define CXML_INT32_MAX    ((NW_Int32)  0x7FFFFFFFU)

#define CXML_UINT16_CAST(expr) \
        ( (CXML_ASSERT((-(CXML_Int16)(expr)) <= 0 && \
                     ((CXML_Uint16)(expr)) <= CXML_UINT16_MAX)), \
          ((CXML_Uint16)(expr)) )  

#define CXML_UINT32_CAST(expr) \
        ( (CXML_ASSERT((-(CXML_Int32)(expr)) <= 0 && \
                     ((CXML_Uint32)(expr)) <= CXML_UINT32_MAX)), \
          ((CXML_Uint32)(expr)) )  

#define CXML_INT32_CAST(expr) \
        ( (CXML_ASSERT(((CXML_Int32)(expr)) >= CXML_INT32_MIN && \
                     ((CXML_Int32)(expr)) <= CXML_INT32_MAX)), \
          ((CXML_Int32)(expr)) )


/* Vector types */
typedef CXML_Uint32 CXML_Vector_Metric_t;
#define CXML_Vector_AtEnd   ((CXML_Vector_Metric_t) ~0)
#define CXML_Vector_AtFront ((CXML_Vector_Metric_t) 0)
#define CXML_SEGMENT_LIST_INCREMENT 4


#define CXML_ASCII_NULL                0x00
#define CXML_ASCII_UPPER_A             0x41
#define CXML_ASCII_UPPER_F             0x46
#define CXML_ASCII_UPPER_Z             0x5A

#define CXML_ASCII_LOWER_A             0x61
#define CXML_ASCII_0                   0x30
#define CXML_ASCII_LOWER_F             0x66
#define CXML_ASCII_9                   0x39
#define CXML_ASCII_LOWER_Z             0x7A

#define CXML_ASCII_SPACE               0x20
#define CXML_ASCII_HT                  0x09
#define CXML_ASCII_LF                  0x0a
#define CXML_ASCII_VT                  0x0b
#define CXML_ASCII_FF                  0x0c
#define CXML_ASCII_CR                  0x0d

/* 
 * Used to supress compiler warnings about required, 
 * but unused function parameters 
 */
#define CXML_REQUIRED_PARAM(param) if (param) {};

/*
** Type Declarations
*/
typedef struct {
  CXML_Uint32  length;           /* length of the used part of the buffer */
  CXML_Uint32  allocatedLength;  /* How much space was really allocated */
  CXML_Byte    *data;            /* the real data */
} CXML_Buffer_t;




/* Forward declarations */
typedef struct CXML_Vector_s CXML_Vector_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CXML_TYPE_DEF_H */
