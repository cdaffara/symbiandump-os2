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


#ifndef CXML_NW2CXMLTYPES_H
#define CXML_NW2CXMLTYPES_H

#include <xml/cxml/cxml_type_def.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/* Data types */
#define NW_Bool   CXML_Bool
#define NW_Byte   CXML_Byte
#define NW_Word   CXML_Word
#define NW_Dword  CXML_Dword
#define NW_Uint8  CXML_Uint8
#define NW_Uint16 CXML_Uint16
#define NW_Uint32 CXML_Uint32
#define NW_Int8   CXML_Int8
#define NW_Int16  CXML_Int16
#define NW_Int32  CXML_Int32
#define NW_Ucs2   CXML_Ucs2

#define NW_TRUE              CXML_TRUE
#define NW_FALSE             CXML_FALSE
#define NW_ASSERT(x)         CXML_ASSERT(x)
#define NW_REQUIRED_PARAM(x) CXML_REQUIRED_PARAM(x)

#define NW_UINT16_MAX        CXML_UINT16_MAX
#define NW_UINT32_MAX        CXML_UINT32_MAX
#define NW_INT32_MIN         CXML_INT32_MIN
#define NW_INT32_MAX         CXML_INT32_MAX
#define NW_UINT16_CAST(expr) CXML_UINT16_CAST(expr)
#define NW_UINT32_CAST(expr) CXML_UINT32_CAST(expr)  
#define NW_INT32_CAST(expr)  CXML_INT32_CAST(expr)
#define NW_Buffer_t          CXML_Buffer_t


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif /* CXML_NW2CXMLTYPES_H */

