/* ====================================================================
 * Copyright (c) 2000 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */
 /*
 © Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 */

#include "eng_int.h"
#if (defined(SYMBIAN) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libcrypto_wsd_macros.h"
#include "libcrypto_wsd.h"
#endif

/* If this symbol is defined then ENGINE_get_cipher_engine(), the function that
 * is used by EVP to hook in cipher code and cache defaults (etc), will display
 * brief debugging summaries to stderr with the 'nid'. */
/* #define ENGINE_CIPHER_DEBUG */

#ifndef EMULATOR
static ENGINE_TABLE *cipher_table = NULL;
#else
GET_STATIC_VAR_FROM_TLS(cipher_table,tb_cipher,ENGINE_TABLE *)
#define cipher_table (*GET_WSD_VAR_NAME(cipher_table,tb_cipher, s)())
#endif


EXPORT_C void ENGINE_unregister_ciphers(ENGINE *e)
	{
	engine_table_unregister(&cipher_table, e);
	}

static void engine_unregister_all_ciphers(void)
	{
	engine_table_cleanup(&cipher_table);
	}

EXPORT_C int ENGINE_register_ciphers(ENGINE *e)
	{
	if(e->ciphers)
		{
		const int *nids;
		int num_nids = e->ciphers(e, NULL, &nids, 0);
		if(num_nids > 0)
			return engine_table_register(&cipher_table,
					engine_unregister_all_ciphers, e, nids,
					num_nids, 0);
		}
	return 1;
	}

EXPORT_C void ENGINE_register_all_ciphers()
	{
	ENGINE *e;

	for(e=ENGINE_get_first() ; e ; e=ENGINE_get_next(e))
		ENGINE_register_ciphers(e);
	}

EXPORT_C int ENGINE_set_default_ciphers(ENGINE *e)
	{
	if(e->ciphers)
		{
		const int *nids;
		int num_nids = e->ciphers(e, NULL, &nids, 0);
		if(num_nids > 0)
			return engine_table_register(&cipher_table,
					engine_unregister_all_ciphers, e, nids,
					num_nids, 1);
		}
	return 1;
	}

/* Exposed API function to get a functional reference from the implementation
 * table (ie. try to get a functional reference from the tabled structural
 * references) for a given cipher 'nid' */
EXPORT_C ENGINE *ENGINE_get_cipher_engine(int nid)
	{
	return engine_table_select(&cipher_table, nid);
	}

/* Obtains a cipher implementation from an ENGINE functional reference */
EXPORT_C const EVP_CIPHER *ENGINE_get_cipher(ENGINE *e, int nid)
	{
	const EVP_CIPHER *ret;
	ENGINE_CIPHERS_PTR fn = ENGINE_get_ciphers(e);
	if(!fn || !fn(e, &ret, NULL, nid))
		{
		ENGINEerr(ENGINE_F_ENGINE_GET_CIPHER,
				ENGINE_R_UNIMPLEMENTED_CIPHER);
		return NULL;
		}
	return ret;
	}

/* Gets the cipher callback from an ENGINE structure */
EXPORT_C ENGINE_CIPHERS_PTR ENGINE_get_ciphers(const ENGINE *e)
	{
	return e->ciphers;
	}

/* Sets the cipher callback in an ENGINE structure */
EXPORT_C int ENGINE_set_ciphers(ENGINE *e, ENGINE_CIPHERS_PTR f)
	{
	e->ciphers = f;
	return 1;
	}
