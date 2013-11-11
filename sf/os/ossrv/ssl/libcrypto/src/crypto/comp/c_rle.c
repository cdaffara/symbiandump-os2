/*
Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation nor the names of its contributors 
  may be used to endorse or promote products derived from this software 
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Description:
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/objects.h>
#include <openssl/comp.h>
#if (defined(SYMBIAN) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libcrypto_wsd_macros.h"
#include "libcrypto_wsd.h"
#endif

static int rle_compress_block(COMP_CTX *ctx, unsigned char *out,
	unsigned int olen, unsigned char *in, unsigned int ilen);
static int rle_expand_block(COMP_CTX *ctx, unsigned char *out,
	unsigned int olen, unsigned char *in, unsigned int ilen);

#ifndef EMULATOR
static COMP_METHOD rle_method={
	NID_rle_compression,
	LN_rle_compression,
	NULL,
	NULL,
	rle_compress_block,
	rle_expand_block,
	NULL,
	NULL,
	};
#else
  GET_STATIC_VAR_FROM_TLS(rle_method,c_rle,COMP_METHOD)
  #define rle_method (*GET_WSD_VAR_NAME(rle_method,c_rle,s)())
  const COMP_METHOD temp_s_rle_method={
	NID_rle_compression,
	LN_rle_compression,
	NULL,
	NULL,
	rle_compress_block,
	rle_expand_block,
	NULL,
	NULL,
	};
#endif

EXPORT_C COMP_METHOD *COMP_rle(void)
	{
	return(&rle_method);
	}

static int rle_compress_block(COMP_CTX *ctx, unsigned char *out,
	     unsigned int olen, unsigned char *in, unsigned int ilen)
	{
	/* int i; */

	if (olen < (ilen+1))
		{
		/* ZZZZZZZZZZZZZZZZZZZZZZ */
		return(-1);
		}

	*(out++)=0;
	memcpy(out,in,ilen);
	return(ilen+1);
	}

static int rle_expand_block(COMP_CTX *ctx, unsigned char *out,
	     unsigned int olen, unsigned char *in, unsigned int ilen)
	{
	int i;

	if (olen < (ilen-1))
		{
		/* ZZZZZZZZZZZZZZZZZZZZZZ */
		return(-1);
		}

	i= *(in++);
	if (i == 0)
		{
		memcpy(out,in,ilen-1);
		}
	return(ilen-1);
	}

