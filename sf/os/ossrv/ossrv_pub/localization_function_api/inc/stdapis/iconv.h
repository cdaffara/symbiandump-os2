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
* Description:   Contains the source for character set conversion
 *
*/


 
#ifndef _ICONV_H
#define _ICONV_H

#include <stddef.h>
#include <sys/cdefs.h>

#ifdef __SYMBIAN32__
#include <_ansi.h>
#endif

__BEGIN_DECLS
typedef void *iconv_t;


/* Allocate memory for codeset conversion descriptor */
IMPORT_C iconv_t iconv_open(const char *tocharset, const char *fromcharset);


/* Converts characters in the input buffer from source character set to destination charcater set and stores the converted charcaters in output buffer */
IMPORT_C size_t iconv(iconv_t conversion_descriptor, const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft);

/* Deallocates the memory allocated for codeset converison descriptor */
IMPORT_C int iconv_close(iconv_t conversion_descriptor);

__END_DECLS
#endif
