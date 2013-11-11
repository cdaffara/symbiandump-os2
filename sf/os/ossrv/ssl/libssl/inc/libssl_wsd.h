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

 
#ifndef _LIBSSL_WSD_H_
#define _LIBSSL_WSD_H_


#include <openssl/bio.h>
#include <openssl/ssl.h>
#include "libssl_wsd_defs.h"

#ifdef EMULATOR


#define SSL_MD_NUM_IDX	2
#define SSL_ENC_NUM_IDX	9

#define SSL_STR_FUNCTS_NUM 172
#define SSL_STR_REASONS_NUM 238

#define SSL3_MAX_NUM_CIPHERS 87 /* Must be updated once new ciphers are added. */    
#define SSL2_MAX_NUM_CIPHERS 10 /* Must be updated once new ciphers are added. */	

#define SSL3_NUM_CIPHERS (sizeof(temp_ssl3_ciphers)/sizeof(SSL_CIPHER))
#define SSL2_NUM_CIPHERS (sizeof(temp_ssl2_ciphers)/sizeof(SSL_CIPHER))

/* This is for the SSLv3/TLSv1.0 differences in crypto/hash stuff
 * It is a bit of a mess of functions, but hell, think of it as
 * an opaque structure :-) */
typedef struct ssl3_enc_method
	{
	int (*enc)(SSL *, int);
	int (*mac)(SSL *, unsigned char *, int);
	int (*setup_key_block)(SSL *);
	int (*generate_master_secret)(SSL *, unsigned char *, unsigned char *, int);
	int (*change_cipher_state)(SSL *, int);
	int (*final_finish_mac)(SSL *, EVP_MD_CTX *, EVP_MD_CTX *, const char *, int, unsigned char *);
	int finish_mac_length;
	int (*cert_verify_mac)(SSL *, EVP_MD_CTX *, unsigned char *);
	const char *client_finished_label;
	int client_finished_label_len;
	const char *server_finished_label;
	int server_finished_label_len;
	int (*alert_value)(int);
	} SSL3_ENC_METHOD;

typedef struct{

			/* bio_ssl.c */
			VARIABLE_DECL(methods_sslp,s,bio_ssl,BIO_METHOD) //static
			
			/* d1_meth.c */
			VARIABLE_DECL(DTLSv1_method_data,s,d1_meth,SSL_METHOD) //static
			
			/* d1_clnt.c */
			VARIABLE_DECL(DTLSv1_client_method_data,s,d1_clnt,SSL_METHOD) //static
			
			/* d1_lib.c */
			VARIABLE_DECL(dtlsv1_base_method_data,s,d1_lib,SSL_METHOD) //static
			
			/* d1_srvr.c */
			VARIABLE_DECL(DTLSv1_server_method_data,s,d1_srvr,SSL_METHOD) //static
			
			/* s23_clnt.c */
			VARIABLE_DECL(SSLv23_client_method_data,s,s23_clnt,SSL_METHOD) //static
			
			/* s23_lib.c */
			VARIABLE_DECL(sslv23_base_method_data,s,s23_lib,SSL_METHOD) //static
			
			/* s23_meth.c */
			VARIABLE_DECL(SSLv23_method_data,s,s23_meth,SSL_METHOD) //static
			
			/* s23_srvr.c */
			VARIABLE_DECL(SSLv23_server_method_data,s,s23_srvr,SSL_METHOD) //static
			
			/* s2_clnt.c */
			VARIABLE_DECL(SSLv2_client_method_data,s,s2_clnt,SSL_METHOD) //static
			
			/* s2_lib.c */
			VARIABLE_DECL(sslv2_base_method_data,s,s2_lib,SSL_METHOD) //static
			
			/* s2_meth.c */
			VARIABLE_DECL(SSLv2_method_data,s,s2_meth,SSL_METHOD) //static
			
			/* s2_srvr.c */
			VARIABLE_DECL(SSLv2_server_method_data,s,s2_srvr,SSL_METHOD) //static
			
			/* s3_clnt.c */
			VARIABLE_DECL(SSLv3_client_method_data,s,s3_clnt,SSL_METHOD) //static
			
			/* s3_lib.c */
			VARIABLE_DECL(sslv3_base_method_data,s,s3_lib,SSL_METHOD) //static
			
			/* s3_meth.c */
			VARIABLE_DECL(SSLv3_method_data,s,s3_meth,SSL_METHOD) //static
			
			/* s3_srvr.c */
			VARIABLE_DECL(SSLv3_server_method_data,s,s3_srvr,SSL_METHOD) //static
			
			/* t1_clnt.c */
			VARIABLE_DECL(TLSv1_client_method_data,s,t1_clnt,SSL_METHOD) //static
			
			/* t1_lib.c */
			VARIABLE_DECL(tlsv1_base_method_data,s,t1_lib,SSL_METHOD) //static
			
			/* t1_srvr.c */
			VARIABLE_DECL(TLSv1_server_method_data,s,t1_srvr,SSL_METHOD) //static
			
			/* t1_meth.c */
			VARIABLE_DECL(TLSv1_method_data,s,t1_meth,SSL_METHOD) //static
			
			/* ssl_cert.c */
			VARIABLE_DECL(ssl_x509_store_ctx_idx,s,ssl_cert,volatile int) //static
			
			/* ssl_ciph.c */
			VARIABLE_DECL(ssl_comp_methods,s,ssl_ciph,STACK_OF(SSL_COMP) *) //static
			
			/* ssl_ciph.c */
			VARIABLE_DECL_ARRAY(ssl_cipher_methods,s,ssl_ciph,const EVP_CIPHER *,SSL_ENC_NUM_IDX) //static
			
			/* ssl_ciph.c */
			VARIABLE_DECL_ARRAY(ssl_digest_methods,s,ssl_ciph,const EVP_MD *,SSL_MD_NUM_IDX) //static
						
			
			/* ssl_err.c */
			VARIABLE_DECL_ARRAY(SSL_str_functs,s,ssl_err,ERR_STRING_DATA,SSL_STR_FUNCTS_NUM) //static
						
			/* ssl_err.c */
			VARIABLE_DECL_ARRAY(SSL_str_reasons,s,ssl_err,ERR_STRING_DATA,SSL_STR_REASONS_NUM) //static
			
			/* s3_lib.c */
			VARIABLE_DECL_ARRAY(ssl3_ciphers,g,s3_lib,SSL_CIPHER,SSL3_MAX_NUM_CIPHERS) //global
			
			/* s3_lib.c */
			VARIABLE_DECL_ARRAY(ssl2_ciphers,g,s2_lib,SSL_CIPHER,SSL2_MAX_NUM_CIPHERS) //global

			/* ssl_lib.c */
			VARIABLE_DECL(ssl3_undef_enc_method,g,ssl_lib,SSL3_ENC_METHOD) //static
			
			/* d1_lib.c */
			VARIABLE_DECL(DTLSv1_enc_data,g,d1_lib,SSL3_ENC_METHOD) //static
			
			/* s3_lib.c */
			VARIABLE_DECL(SSLv3_enc_data,g,s3_lib,SSL3_ENC_METHOD) //static

			/* t1_lib.c */
			VARIABLE_DECL(TLSv1_enc_data,g,t1_lib,SSL3_ENC_METHOD) //static
			
			
}_libssl_wsd;



#ifdef __cplusplus
extern "C" {
#endif


void InitSSLWsdVar(struct _libssl_wsd *p);

//void Init_methods_sslp(/*_libssl_wsd *ptr*/);

_libssl_wsd* Libssl_ImpurePtr();

#ifdef __cplusplus
}
#endif

#endif /* EMULATOR */
#endif /* _LIBSSL_WSD_H_ */
