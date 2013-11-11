// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : iconv.CPP
// Part of     : LIBC
// Contains the source for character set conversion
// Version     : 
//



#include "sysreent.h"
#include <errno.h>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <charconv.h>
#include <f32file.h>
#include <_ansi.h>
#include "sysif.h"


extern "C" {

EXPORT_C iconv_t iconv_open(const char *tocode, const char *fromcode)
{
	return _iconv_open_r(&errno, tocode, fromcode);
	}


EXPORT_C int iconv_close(iconv_t cd)
	{
	return _iconv_close_r(&errno, cd);
	}


EXPORT_C size_t iconv(iconv_t cd,  const char **inbuf, size_t *inbytesleft,
	    char **outbuf, size_t *outbytesleft)
	{
	return _iconv_r(&errno, cd, inbuf, inbytesleft, outbuf, outbytesleft);
	}

} //extern "C"
