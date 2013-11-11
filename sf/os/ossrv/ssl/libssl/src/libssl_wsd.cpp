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



#include <openssl/bio.h>
#include <openssl/err.h>
#include <e32std.h>


#include "libssl_wsd.h"
/*
#include "wsd_solution.h"
*/

#ifdef __WINSCW__ 
#include <emulator.h> //GetProcAddress, GetModuleHandle
#include "pls.h" // For emulator WSD API 
const TUid KLibdlUid3 = {0x10281F34}; // This is the UID of the library
#endif//defined(__WINSCW__) || defined(__WINS__)

#ifdef __cplusplus
extern "C" 
{
#endif

#ifdef EMULATOR


extern const SSL_METHOD temp_methods_sslp;								
extern const SSL_METHOD temp_DTLSv1_method_data;
extern const SSL_METHOD temp_DTLSv1_client_method_data;
extern const SSL_METHOD temp_dtlsv1_base_method_data;
extern const SSL_METHOD temp_DTLSv1_server_method_data;
extern const SSL_METHOD temp_SSLv23_client_method_data;
extern const SSL_METHOD temp_sslv23_base_method_data;
extern const SSL_METHOD temp_SSLv23_method_data;
extern const SSL_METHOD temp_SSLv23_server_method_data;
extern const SSL_METHOD temp_SSLv2_client_method_data;
extern const SSL_METHOD temp_sslv2_base_method_data;
extern const SSL_METHOD temp_SSLv2_method_data;
extern const SSL_METHOD temp_SSLv2_server_method_data;
extern const SSL_METHOD temp_SSLv3_client_method_data;
extern const SSL_METHOD temp_sslv3_base_method_data;
extern const SSL_METHOD temp_SSLv3_method_data;
extern const SSL_METHOD temp_SSLv3_server_method_data;
extern const SSL_METHOD temp_TLSv1_client_method_data;
extern const SSL_METHOD temp_tlsv1_base_method_data;
extern const SSL_METHOD temp_TLSv1_server_method_data;
extern const SSL_METHOD temp_TLSv1_method_data;
extern const ERR_STRING_DATA temp_SSL_str_functs[172];
extern const ERR_STRING_DATA temp_SSL_str_reasons[238];
extern const SSL_CIPHER temp_ssl3_ciphers[87];
extern const SSL_CIPHER temp_ssl2_ciphers[10];
extern const SSL3_ENC_METHOD temp_ssl3_undef_enc_method;
extern const SSL3_ENC_METHOD temp_DTLSv1_enc_data;
extern const SSL3_ENC_METHOD temp_SSLv3_enc_data;
extern const SSL3_ENC_METHOD temp_TLSv1_enc_data;

int LibSSLWSD_Init(_libssl_wsd *p);

_libssl_wsd *Libssl_ImpurePtr()
 	{	
/* 		// Access the PLS area of this process.
		struct _libssl_wsd *p = (struct _libssl_wsd*)Pls();

		if(p)
			return p;

		// To allocate storage for the global object within the
		// VAS of the emulator.
		p = (struct _libssl_wsd *)AllocatePls(sizeof(struct _libssl_wsd));
		
		if(!p)
			User::Panic(_L("LIBSSL-INIT"),KErrNoMemory);	
		
		// Placement new to construct the WSD object at 'p'
		new (p) struct _libssl_wsd;
		
		// WSD object initialization
		Mem::FillZ(p,sizeof(struct _libssl_wsd));
		InitSSLWsdVar(p);
		
		// Set the Pls with the new value
		TInt err = SetPls(p);
		
		if(err != KErrNone)
		{
			User::Panic(_L("LIBSSL-INIT"),err);
		}
*/
		_libssl_wsd *p = Pls<_libssl_wsd>(KLibdlUid3, LibSSLWSD_Init);
		return p;
 	}
 	
int LibSSLWSD_Init(_libssl_wsd *p)
{
	if(p)
		{
			
			memcpy(&(p->GET_WSD_VAR_NAME(methods_sslp,bio_ssl,s)),&temp_methods_sslp,sizeof(SSL_METHOD));
			
			
			memcpy(&(p->GET_WSD_VAR_NAME(ssl3_undef_enc_method,ssl_lib,g)),&temp_ssl3_undef_enc_method,sizeof(SSL3_ENC_METHOD));
			
			memcpy(&(p->GET_WSD_VAR_NAME(DTLSv1_enc_data,d1_lib,g)),&temp_DTLSv1_enc_data,sizeof(SSL3_ENC_METHOD));
			
			memcpy(&(p->GET_WSD_VAR_NAME(TLSv1_enc_data,t1_lib,g)),&temp_TLSv1_enc_data,sizeof(SSL3_ENC_METHOD));
			
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv3_enc_data,s3_lib,g)),&temp_SSLv3_enc_data,sizeof(SSL3_ENC_METHOD));
			
			
			memcpy(&(p->GET_WSD_VAR_NAME(DTLSv1_method_data,d1_meth,s)),&temp_DTLSv1_method_data,sizeof(SSL_METHOD));			
			(p->GET_WSD_VAR_NAME(DTLSv1_method_data,d1_meth,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(DTLSv1_enc_data,d1_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(DTLSv1_client_method_data,d1_clnt,s)),&temp_DTLSv1_client_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(DTLSv1_client_method_data,d1_clnt,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(DTLSv1_enc_data,d1_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(dtlsv1_base_method_data,d1_lib,s)),&temp_dtlsv1_base_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(dtlsv1_base_method_data,d1_lib,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(DTLSv1_enc_data,d1_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(DTLSv1_server_method_data,d1_srvr,s)),&temp_DTLSv1_server_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(DTLSv1_server_method_data,d1_srvr,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(DTLSv1_enc_data,d1_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv23_client_method_data,s23_clnt,s)),&temp_SSLv23_client_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(SSLv23_client_method_data,s23_clnt,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(ssl3_undef_enc_method,ssl_lib,g));			
			
			memcpy(&(p->GET_WSD_VAR_NAME(sslv23_base_method_data,s23_lib,s)),&temp_sslv23_base_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(sslv23_base_method_data,s23_lib,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(ssl3_undef_enc_method,ssl_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv23_method_data,s23_meth,s)),&temp_SSLv23_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(SSLv23_method_data,s23_meth,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(ssl3_undef_enc_method,ssl_lib,g));
						
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv23_server_method_data,s23_srvr,s)),&temp_SSLv23_server_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(SSLv23_server_method_data,s23_srvr,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(ssl3_undef_enc_method,ssl_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv2_client_method_data,s2_clnt,s)),&temp_SSLv2_client_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(SSLv2_client_method_data,s2_clnt,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(ssl3_undef_enc_method,ssl_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(sslv2_base_method_data,s2_lib,s)),&temp_sslv2_base_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(sslv2_base_method_data,s2_lib,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(ssl3_undef_enc_method,ssl_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv2_method_data,s2_meth,s)),&temp_SSLv2_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(SSLv2_method_data,s2_meth,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(ssl3_undef_enc_method,ssl_lib,g));
				
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv2_server_method_data,s2_srvr,s)),&temp_SSLv2_server_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(SSLv2_server_method_data,s2_srvr,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(ssl3_undef_enc_method,ssl_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv3_client_method_data,s3_clnt,s)),&temp_SSLv3_client_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(SSLv3_client_method_data,s3_clnt,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(SSLv3_enc_data,s3_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(sslv3_base_method_data,s3_lib,s)),&temp_sslv3_base_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(sslv3_base_method_data,s3_lib,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(SSLv3_enc_data,s3_lib,g));
									
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv3_method_data,s3_meth,s)),&temp_SSLv3_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(SSLv3_method_data,s3_meth,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(SSLv3_enc_data,s3_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(SSLv3_server_method_data,s3_srvr,s)),&temp_SSLv3_server_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(SSLv3_server_method_data,s3_srvr,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(SSLv3_enc_data,s3_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(TLSv1_client_method_data,t1_clnt,s)),&temp_TLSv1_client_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(TLSv1_client_method_data,t1_clnt,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(TLSv1_enc_data,t1_lib,g));
			
			memcpy(&(p->GET_WSD_VAR_NAME(tlsv1_base_method_data,t1_lib,s)),&temp_tlsv1_base_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(tlsv1_base_method_data,t1_lib,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(TLSv1_enc_data,t1_lib,g));	
			
			memcpy(&(p->GET_WSD_VAR_NAME(TLSv1_server_method_data,t1_srvr,s)),&temp_TLSv1_server_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(TLSv1_server_method_data,t1_srvr,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(TLSv1_enc_data,t1_lib,g));	
			
			memcpy(&(p->GET_WSD_VAR_NAME(TLSv1_method_data,t1_meth,s)),&temp_TLSv1_method_data,sizeof(SSL_METHOD));
			(p->GET_WSD_VAR_NAME(TLSv1_method_data,t1_meth,s)).ssl3_enc = &(p->GET_WSD_VAR_NAME(TLSv1_enc_data,t1_lib,g));	
			
			p->GET_WSD_VAR_NAME(ssl_x509_store_ctx_idx,ssl_cert,s) = -1;
			
			p->GET_WSD_VAR_NAME(ssl_comp_methods,ssl_ciph,s) = NULL;	
			
			
			memcpy(&(p->GET_WSD_VAR_NAME(SSL_str_functs,ssl_err,s)),&temp_SSL_str_functs,sizeof(ERR_STRING_DATA) * SSL_STR_FUNCTS_NUM);
			
			memcpy(&(p->GET_WSD_VAR_NAME(SSL_str_reasons,ssl_err,s)),&temp_SSL_str_reasons,sizeof(ERR_STRING_DATA) * SSL_STR_REASONS_NUM);
			
			memcpy(&(p->GET_WSD_VAR_NAME(ssl3_ciphers,s3_lib,g)),&temp_ssl3_ciphers,sizeof(SSL_CIPHER) * SSL3_NUM_CIPHERS);
			
			memcpy(&(p->GET_WSD_VAR_NAME(ssl2_ciphers,s2_lib,g)),&temp_ssl2_ciphers,sizeof(SSL_CIPHER) * SSL2_NUM_CIPHERS);

		
																	
		}
		return KErrNone;
}
#endif  /*EMULATOR*/

#ifdef __cplusplus
}
#endif

