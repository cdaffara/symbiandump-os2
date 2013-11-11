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
**   File: cxml_str.h
**   Purpose:  Provides the interfaces to CXML string API
**************************************************************************/

#ifndef CXML_STR_H
#define CXML_STR_H

#warning The CXML API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/
#include <xml/cxml/cxml_type_def.h>

CXML_Ucs2 CXML_Str_ToLower(const CXML_Ucs2 ch);
CXML_Ucs2 CXML_Str_ToUpper(const CXML_Ucs2 ch);
CXML_Int32 CXML_Str_Isspace(const CXML_Ucs2 ch);
CXML_Int32 CXML_Str_Isdigit(const CXML_Ucs2 ch);
CXML_Int32 CXML_Str_Isxdigit(const CXML_Ucs2 ch);
void CXML_Str_Delete(CXML_Ucs2 *string);
CXML_Int8*CXML_Str_CvtToAscii(const CXML_Ucs2 *string);
CXML_Uint32 CXML_Asc_strlen(const CXML_Int8 *s);
CXML_Ucs2 *CXML_Str_CvtFromAscii(const CXML_Int8 *string);
CXML_Int32 CXML_Str_StrcmpConst(const CXML_Ucs2 *string1,const CXML_Ucs2 *string2);
CXML_Int32 CXML_Str_Stricmp(const CXML_Ucs2 *string1,const CXML_Ucs2 *string2);


CXML_Int8 *CXML_Asc_strcpy(CXML_Int8 *s1, const CXML_Int8 *s2);

CXML_Int8 *CXML_Asc_strcat(CXML_Int8 *s1, const CXML_Int8 *s2);

CXML_Int8 *CXML_Asc_strncpy(CXML_Int8 *destination, const CXML_Int8 *source, const CXML_Uint32 count);

CXML_Int8 *CXML_Asc_strncat(CXML_Int8 *s1, const CXML_Int8 *s2,const CXML_Uint32 count);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CXML_STR_H */
