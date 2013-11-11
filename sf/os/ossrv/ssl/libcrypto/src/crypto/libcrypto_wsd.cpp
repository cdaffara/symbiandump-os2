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

#include <e32std.h>
#include <_ansi.h>
#include <libcrypto_wsd.h>
#include <stdlib.h>
#include <openssl/objects.h>
#include <openssl/bn.h>
#include <openssl/buffer.h>
#include <openssl/comp.h>
#include <openssl/conf.h>
#include <openssl/dh.h>
#include <openssl/dsa.h>
#include <openssl/dso.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ocsp.h>
#include <openssl/pem.h>
#include <openssl/pkcs7.h>
#include <openssl/pkcs12.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/ui.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/lhash.h>
#include <openssl/crypto.h>
#include "obj_dat.h"
//#include "libcrypto_wsd_solution.h"
#ifdef __WINSCW__ 
#include <emulator.h> //GetProcAddress, GetModuleHandle
#include "pls.h" // For emulator WSD API 
const TUid KLibdlUid3 = {0x10281F35}; // This is the UID of the library
#endif//defined(__WINSCW__) || defined(__WINS__)

#ifdef __cplusplus
extern "C" 
{
#endif
extern CRYPTO_EX_DATA_IMPL temp_s_impl_default;
extern BIO_METHOD temp_s_mem_method;
extern BIO_METHOD temp_s_null_method;
extern BIO_METHOD temp_s_methods_fdp;
extern BIO_METHOD temp_s_methods_sockp;
extern BIO_METHOD temp_s_methods_filep;
extern BIO_METHOD temp_s_methods_dgramp;
extern BIO_METHOD temp_s_methods_connectp;
extern BIO_METHOD temp_s_methods_biop;
extern BIO_METHOD temp_s_methods_acceptp;
extern BIO_METHOD temp_s_methods_md;
extern BIO_METHOD temp_s_methods_b64;
extern BIO_METHOD temp_s_methods_enc;
extern BIO_METHOD temp_s_methods_ok;
extern COMP_METHOD temp_s_rle_method;
extern COMP_METHOD temp_s_zlib_method_nozlib;
extern COMP_METHOD temp_s_zlib_stateful_method;
extern CONF_METHOD temp_s_default_method;
extern CONF_METHOD temp_s_WIN32_method;
extern DH_METHOD temp_s_dh_ossl;
extern DSA_METHOD temp_s_openssl_dsa_meth;
//extern DSO_METHOD temp_s_dso_meth_dl;
extern DSO_METHOD temp_s_dso_meth_dlfcn;
extern DSO_METHOD temp_s_dso_meth_null;
extern RSA_METHOD temp_s_rsa_pkcs1_eay_meth;
extern STORE_METHOD temp_s_store_memory;
extern UI_METHOD temp_s_ui_openssl;
extern X509V3_CONF_METHOD temp_s_nconf_method;
extern X509V3_CONF_METHOD temp_s_conf_lhash_method;
extern ASN1_METHOD temp_s_meth;
extern ASN1_METHOD temp_s_method;
extern RAND_METHOD temp_g_rand_ssleay_meth;
extern X509_LOOKUP_METHOD temp_g_x509_file_lookup;
extern X509_LOOKUP_METHOD temp_g_x509_dir_lookup;

extern void *default_malloc_ex(size_t num, const char *file, int line);
extern void *default_realloc_ex(void *str, size_t num,
        const char *file, int line);
extern void *default_malloc_locked_ex(size_t num, const char *file, int line);

extern const ERR_STRING_DATA temp_ASN1_str_functs[108];
extern const ERR_STRING_DATA temp_ASN1_str_reasons[99];
extern const ERR_STRING_DATA temp_CRYPTO_str_functs[10];
extern const ERR_STRING_DATA temp_CRYPTO_str_reasons[2];
extern const ERR_STRING_DATA temp_BIO_str_functs[33];
extern const ERR_STRING_DATA temp_BIO_str_reasons[30];	
extern const ERR_STRING_DATA temp_BN_str_functs[39];
extern const ERR_STRING_DATA temp_BN_str_reasons[18];
extern const ERR_STRING_DATA temp_BUF_str_functs[7];
extern const ERR_STRING_DATA temp_BUF_str_reasons[1];
extern const ERR_STRING_DATA temp_COMP_str_functs[1];
extern const ERR_STRING_DATA temp_COMP_str_reasons[1];
extern const ERR_STRING_DATA temp_CONF_str_functs[22];
extern const ERR_STRING_DATA temp_CONF_str_reasons[16];
extern const ERR_STRING_DATA temp_DH_str_functs[8];
extern const ERR_STRING_DATA temp_DH_str_reasons[5];
extern const ERR_STRING_DATA temp_DSA_str_functs[15];
extern const ERR_STRING_DATA temp_DSA_str_reasons[5];
extern const ERR_STRING_DATA temp_DSO_str_functs[38];
extern const ERR_STRING_DATA temp_DSO_str_reasons[18];
extern const ERR_STRING_DATA temp_ENGINE_str_functs[37];
extern const ERR_STRING_DATA temp_ENGINE_str_reasons[40];
extern const ERR_STRING_DATA temp_ERR_str_libraries[25];
extern const ERR_STRING_DATA temp_ERR_str_functs[11];
extern const ERR_STRING_DATA temp_ERR_str_reasons[36];
extern const ERR_STRING_DATA temp_EVP_str_functs[36];
extern const ERR_STRING_DATA temp_EVP_str_reasons[45];
extern const ERR_STRING_DATA temp_OBJ_str_functs[8];
extern const ERR_STRING_DATA temp_OBJ_str_reasons[3];
extern const ERR_STRING_DATA temp_OCSP_str_functs[18];
extern const ERR_STRING_DATA temp_OCSP_str_reasons[31];
extern const ERR_STRING_DATA temp_PEM_str_functs[27];
extern const ERR_STRING_DATA temp_PEM_str_reasons[17];
extern const ERR_STRING_DATA temp_PKCS7_str_functs[29];
extern const ERR_STRING_DATA temp_PKCS7_str_reasons[47];
extern const  ERR_STRING_DATA temp_PKCS12_str_functs[30];
extern const ERR_STRING_DATA temp_PKCS12_str_reasons[23];
extern const ERR_STRING_DATA temp_RAND_str_functs[3];
extern const ERR_STRING_DATA temp_RAND_str_reasons[2];
extern const ERR_STRING_DATA temp_RSA_str_functs[38];
extern const ERR_STRING_DATA temp_RSA_str_reasons[42];
extern const ERR_STRING_DATA temp_UI_str_functs[13];
extern const ERR_STRING_DATA temp_UI_str_reasons[8];
extern const ERR_STRING_DATA temp_X509_str_functs[46];
extern const ERR_STRING_DATA temp_X509_str_reasons[24];
extern const ERR_STRING_DATA temp_X509V3_str_functs[57];
extern const ERR_STRING_DATA temp_X509V3_str_reasons[61];
extern const DES_cblock temp_weak_keys[16];
extern const ASN1_METHOD temp_ia5string_meth;
extern const ASN1_METHOD temp_bit_string_meth;
#ifdef __cplusplus
}
#endif


libcrypto_global_struct *libcrypto_ImpurePtr()
{
	  /*struct libcrypto_global_struct *g = (struct libcrypto_global_struct *)Pls();
	
	  if(g)
		return g;
	
	  g = (struct libcrypto_global_struct *)
	          AllocatePls(sizeof(struct libcrypto_global_struct));
	          
		//new (g) struct libcrypto_global_struct;
		
	  Mem::FillZ(g,sizeof(struct libcrypto_global_struct));
	  libcrypto_Init(g);

		TInt err = SetPls(g);
	
	  if(g == 0 || err)
		User::Panic(_L("LIBCRYPTO-INIT"),err);
	  return g;*/
	  libcrypto_global_struct *p = Pls<libcrypto_global_struct>(KLibdlUid3,libcrypto_Init);
	  return p;
}

int libcrypto_Init(libcrypto_global_struct *g)
{
	  if(g)
	  {
	    memcpy(&(g->GET_WSD_VAR_NAME(ia5string_meth,a_meth,s)),&temp_ia5string_meth,sizeof(ASN1_METHOD));
	    memcpy(&(g->GET_WSD_VAR_NAME(bit_string_meth,a_meth,s)),&temp_bit_string_meth,sizeof(ASN1_METHOD));
		  
		  g->GET_WSD_VAR_NAME(stable,a_strnid,s)= NULL;
      g->GET_WSD_VAR_NAME(global_mask,a_strnid,s)=0xFFFFFFFFL;
      g->GET_WSD_VAR_NAME(app_locks,cryptlib,s)=NULL;			
      g->GET_WSD_VAR_NAME(dyn_locks,cryptlib,s)=NULL;
      g->GET_WSD_VAR_NAME(allow_customize,mem,s)=1;
      g->GET_WSD_VAR_NAME(allow_customize_debug,mem,s)=1;
      g->GET_WSD_VAR_NAME(mh_mode,mem_dbg,s)=0x0;
      g->GET_WSD_VAR_NAME(order,mem_dbg,s)=0x0;
      g->GET_WSD_VAR_NAME(mh,mem_dbg,s)= NULL;
      g->GET_WSD_VAR_NAME(amih,mem_dbg,s)=NULL;
      g->GET_WSD_VAR_NAME(options,mem_dbg,s)=
      #if defined(CRYPTO_MDEBUG_TIME) || defined(CRYPTO_MDEBUG_ALL)
	    V_CRYPTO_MDEBUG_TIME |
      #endif
      #if defined(CRYPTO_MDEBUG_THREAD) || defined(CRYPTO_MDEBUG_ALL)
	    V_CRYPTO_MDEBUG_THREAD |
      #endif
	    0;
      g->GET_WSD_VAR_NAME(num_disable,mem_dbg,s)=0;
      g->GET_WSD_VAR_NAME(disabling_thread,mem_dbg,s)=0;
      g->GET_WSD_VAR_NAME(impl,ex_data,s)=NULL;
      g->GET_WSD_VAR_NAME(ex_class,ex_data,s)=CRYPTO_EX_INDEX_USER;
      g->GET_WSD_VAR_NAME(ex_data,ex_data,s)=NULL;
  		
  		
  			
  	  g->GET_WSD_VAR_NAME(init,bn_lib,s) = 0;		
  		
  		g->GET_WSD_VAR_NAME(data_one,bn_lib,s)=1L;
  		g->GET_WSD_VAR_NAME(const_one,bn_lib,s).d=&(g->GET_WSD_VAR_NAME(data_one,bn_lib,s));
  		g->GET_WSD_VAR_NAME(const_one,bn_lib,s).top = 1;
  		g->GET_WSD_VAR_NAME(const_one,bn_lib,s).dmax = 1;
  		g->GET_WSD_VAR_NAME(const_one,bn_lib,s).neg = 0;
  		g->GET_WSD_VAR_NAME(const_one,bn_lib,s).flags = BN_FLG_STATIC_DATA;
  		g->GET_WSD_VAR_NAME(_is_set_256_data,bn_nist,s)=0;
      g->GET_WSD_VAR_NAME(_is_set_384_data,bn_nist,s)=0;
  		g->GET_WSD_VAR_NAME(zlib_stateful_ex_idx,c_zlib,s)=-1;
  		g->GET_WSD_VAR_NAME(p_compress,c_zlib,s) = NULL;
      g->GET_WSD_VAR_NAME(p_inflateEnd,c_zlib,s) = NULL;
      g->GET_WSD_VAR_NAME(p_inflate,c_zlib,s) = NULL;
      g->GET_WSD_VAR_NAME(p_inflateInit_,c_zlib,s) = NULL;
      g->GET_WSD_VAR_NAME(p_deflateEnd,c_zlib,s)=NULL;
      g->GET_WSD_VAR_NAME(p_deflate,c_zlib,s)=NULL;
      g->GET_WSD_VAR_NAME(p_deflateInit_,c_zlib,s)=NULL;
      g->GET_WSD_VAR_NAME(zlib_loaded ,c_zlib,s)=0;
      g->GET_WSD_VAR_NAME(zlib_dso ,c_zlib,s)=NULL;
      g->GET_WSD_VAR_NAME(supported_modules,conf_mod,s)=NULL;
      g->GET_WSD_VAR_NAME(initialized_modules,conf_mod,s)=NULL;
      g->GET_WSD_VAR_NAME(openssl_configured,conf_sap,s)=0;
      g->GET_WSD_VAR_NAME(init,ecb_enc,s)=1;
      
      memset(&(g->GET_WSD_VAR_NAME(buf,ecb_enc,s)),0,32);

  		g->GET_WSD_VAR_NAME(net,enc_read,s)=NULL;
      g->GET_WSD_VAR_NAME(unnet,enc_read,s)=NULL;
      g->GET_WSD_VAR_NAME(unnet_start,enc_read,s)=0;
      g->GET_WSD_VAR_NAME(unnet_left,enc_read,s)=0;
      g->GET_WSD_VAR_NAME(tmpbuf,enc_read,s)=NULL;
  		g->GET_WSD_VAR_NAME(outbuf,enc_write,s)=NULL;
      g->GET_WSD_VAR_NAME(start,enc_write,s)=1;
      
      memcpy(&(g->GET_WSD_VAR_NAME(weak_keys,set_key_c,s)),&temp_weak_keys,16);
         
      g->GET_WSD_VAR_NAME(cleanup_stack,eng_lib,s)= NULL;
      g->GET_WSD_VAR_NAME(internal_static_hack,eng_lib,s)=0;
      g->GET_WSD_VAR_NAME(engine_list_head,eng_list,s)=NULL;
      g->GET_WSD_VAR_NAME(engine_list_tail,eng_list,s)=NULL;
      g->GET_WSD_VAR_NAME(table_flags,eng_table,s)=0;
      g->GET_WSD_VAR_NAME(rsa_table,tb_rsa,s)=NULL;
      g->GET_WSD_VAR_NAME(dsa_table,tb_dsa,s)=NULL;
      g->GET_WSD_VAR_NAME(ecdh_table,tb_ecdh,s)=NULL;
      g->GET_WSD_VAR_NAME(ecdsa_table,tb_ecdsa,s)=NULL;
      g->GET_WSD_VAR_NAME(rand_table,tb_rand,s)=NULL;
      g->GET_WSD_VAR_NAME(digest_table,tb_digest,s)=NULL;
      g->GET_WSD_VAR_NAME(store_table,tb_store,s)=NULL;
      g->GET_WSD_VAR_NAME(dh_table,tb_dh,s)=NULL;
      g->GET_WSD_VAR_NAME(cipher_table,tb_cipher,s)=NULL;
  		 
  		g->GET_WSD_VAR_NAME(initialized_engines,eng_cnf,s)=NULL;
  		g->GET_WSD_VAR_NAME(dynamic_ex_data_idx,eng_dyn,s)= -1;
  		g->GET_WSD_VAR_NAME(err_fns,err,s)=NULL;
  		g->GET_WSD_VAR_NAME(pbe_algs,evp_pbe,s)=NULL; 
  		g->GET_WSD_VAR_NAME(int_error_hash,err,s)=NULL;
      g->GET_WSD_VAR_NAME(int_thread_hash,err,s)=NULL;
      g->GET_WSD_VAR_NAME(int_thread_hash_references,err,s)=0;
      g->GET_WSD_VAR_NAME(int_err_library_number,err,s)=ERR_LIB_USER;
      g->GET_WSD_VAR_NAME(init,err,s)=1;

      g->GET_WSD_VAR_NAME(names_lh,o_names,s)=NULL;
      g->GET_WSD_VAR_NAME(names_type_num,o_names,s)=OBJ_NAME_TYPE_NUM;
      g->GET_WSD_VAR_NAME(name_funcs_stack,o_names,s)=NULL;
      g->GET_WSD_VAR_NAME(free_type,o_names,s)=0;
      g->GET_WSD_VAR_NAME(new_nid,obj_dat,s)=NUM_NID;
      g->GET_WSD_VAR_NAME(added,obj_dat,s)=NULL;
         
      g->GET_WSD_VAR_NAME(state_num,md_rand,s)=0;
      g->GET_WSD_VAR_NAME(state_index,md_rand,s)=0;
      memset(&(g->GET_WSD_VAR_NAME(state,md_rand,s)),0,1023+MD_DIGEST_LENGTH);
      memset(&(g->GET_WSD_VAR_NAME(md,md_rand,s)),0,MD_DIGEST_LENGTH);
      g->GET_WSD_VAR_NAME(md_count,md_rand,s)[0]=0;
      g->GET_WSD_VAR_NAME(md_count,md_rand,s)[1]=0;
      g->GET_WSD_VAR_NAME(entropy,md_rand,s)=0;
      g->GET_WSD_VAR_NAME(initialized,md_rand,s)=0;
      g->GET_WSD_VAR_NAME(crypto_lock_rand,md_rand,s)=0;
      g->GET_WSD_VAR_NAME(locking_thread,md_rand,s)=0;
      g->GET_WSD_VAR_NAME(default_RAND_meth,rand_lib,s)=NULL;
         
      g->GET_WSD_VAR_NAME(tty_in,ui_openssl,s)=NULL;
      g->GET_WSD_VAR_NAME(tty_out,ui_openssl,s)=NULL;
      g->GET_WSD_VAR_NAME(is_a_tty,ui_openssl,s)=0;
      g->GET_WSD_VAR_NAME(ext_nid_list,x509_req,s)[0]= NID_ext_req;
      g->GET_WSD_VAR_NAME(ext_nid_list,x509_req,s)[1]= NID_ms_ext_req,
      g->GET_WSD_VAR_NAME(ext_nid_list,x509_req,s)[2]= NID_undef;
      g->GET_WSD_VAR_NAME(ext_nids,x509_req,s)=(int *)&(g->GET_WSD_VAR_NAME(ext_nid_list,x509_req,s));
         
      g->GET_WSD_VAR_NAME(trtable,x509_trs,s)=NULL;
      g->GET_WSD_VAR_NAME(param_table,x509_vpm,s)=NULL;
      g->GET_WSD_VAR_NAME(ext_list,v3_lib,s)=NULL;
      g->GET_WSD_VAR_NAME(xptable,v3_purp,s)=NULL;

      g->GET_WSD_VAR_NAME(default_CONF_method,conf_lib,s)=NULL;
      g->GET_WSD_VAR_NAME(default_DH_method,dh_lib,s)=NULL;
      g->GET_WSD_VAR_NAME(default_DSA_method,dsa_lib,s)=NULL;
      g->GET_WSD_VAR_NAME(default_DSO_meth,dso_lib,s)=NULL;
      g->GET_WSD_VAR_NAME(default_RSA_meth,rsa_lib,s)=NULL;
      g->GET_WSD_VAR_NAME(default_UI_meth,ui_lib,s)=NULL;
      g->GET_WSD_VAR_NAME(stirred_pool,md_rand,s)=0;

      g->malloc_func=malloc;
      g->malloc_ex_func=default_malloc_ex;
      g->realloc_func=realloc;
      g->realloc_ex_func=default_realloc_ex;
      g->free_func_openssl= free;
      g->malloc_locked_func= malloc;
      g->malloc_locked_ex_func= default_malloc_locked_ex;
      g->free_locked_func= free;

#ifdef CRYPTO_MDEBUG
      g->malloc_debug_func=CRYPTO_dbg_malloc;
      g->realloc_debug_func=CRYPTO_dbg_realloc;
      g->free_debug_func=CRYPTO_dbg_free;
      g->set_debug_options_func=CRYPTO_dbg_set_options;
      g->get_debug_options_func=CRYPTO_dbg_get_options;
#else
      g->malloc_debug_func=NULL;
      g->realloc_debug_func=NULL;
      g->free_debug_func=NULL;
      g->set_debug_options_func=NULL;
      g->get_debug_options_func=NULL;
#endif
      g->locking_callback=NULL;
      g->add_lock_callback=NULL;
      g->id_callback=NULL;
      g->dynlock_create_callback=NULL;
      g->dynlock_lock_callback=NULL;
      g->dynlock_destroy_callback=NULL;
      g->_shadow_DES_rw_mode=DES_PCBC_MODE;
      g->_shadow_DES_check_key=0;
          		 
  		 memset(&(g->GET_WSD_VAR_NAME(data,bn_lib,s)),0,16);
  		 memset(&(g->GET_WSD_VAR_NAME(buff,fcrypt,s)),0,14);
  		 memset(&(g->GET_WSD_VAR_NAME(buf,err,s)),0,256);
  		 memset(&(g->GET_WSD_VAR_NAME(fallback,err,s)),0,sizeof(ERR_STATE));
  		 memset(&(g->GET_WSD_VAR_NAME(SYS_str_reasons,err,s)),0,128*sizeof(ERR_STRING_DATA));
  		 memset(&(g->GET_WSD_VAR_NAME(prompt_string,evp_key,s)),0,80);
  		 
  		 memset(&(g->GET_WSD_VAR_NAME(m,hmac,s)),0,EVP_MAX_MD_SIZE);
  		 memset(&(g->GET_WSD_VAR_NAME(m,md2,s)),0,EVP_MAX_MD_SIZE);
  		 memset(&(g->GET_WSD_VAR_NAME(m,md5,s)),0,EVP_MAX_MD_SIZE);
  		 memset(&(g->GET_WSD_VAR_NAME(m,sha1_one,s)),0,SHA_DIGEST_LENGTH);
  		 memset(&(g->GET_WSD_VAR_NAME(m,sha_one,s)),0,SHA_DIGEST_LENGTH);
  		 
  		 memset(&(g->GET_WSD_VAR_NAME(buf,x509_txt,s)),0,100);
  		  		  		  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(ASN1_str_functs,asn1_err,s)),&temp_ASN1_str_functs,108*sizeof(ERR_STRING_DATA));
  		 memcpy(&(g->GET_WSD_VAR_NAME(ASN1_str_reasons,asn1_err,s)),&temp_ASN1_str_reasons,99*sizeof(ERR_STRING_DATA));
       memcpy(&(g->GET_WSD_VAR_NAME(CRYPTO_str_functs,cpt_err,s)),&temp_CRYPTO_str_functs,10*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(CRYPTO_str_reasons,cpt_err,s)),&temp_CRYPTO_str_reasons,2*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(BIO_str_functs,bio_err,s)),&temp_BIO_str_functs,33*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(BIO_str_reasons,bio_err,s)),&temp_BIO_str_reasons,30*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(BN_str_functs,bn_err,s)),&temp_BN_str_functs,39*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(BN_str_reasons,bn_err,s)),&temp_BN_str_reasons,19*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(BUF_str_functs,buf_err,s)),&temp_BUF_str_functs,7*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(BUF_str_reasons,buf_err,s)),&temp_BUF_str_reasons,1*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(COMP_str_functs,comp_err,s)),&temp_COMP_str_functs,sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(COMP_str_reasons,comp_err,s)),&temp_COMP_str_reasons,sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(CONF_str_functs,conf_err,s)),&temp_CONF_str_functs,22*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(CONF_str_reasons,conf_err,s)),&temp_CONF_str_reasons,16*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(DH_str_functs,dh_err,s)),&temp_DH_str_functs,8*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(DH_str_reasons,dh_err,s)),&temp_DH_str_reasons,5*sizeof(ERR_STRING_DATA));  		 
		   memcpy(&(g->GET_WSD_VAR_NAME(DSA_str_functs,dsa_err,s)),&temp_DSA_str_functs,15*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(DSA_str_reasons,dsa_err,s)),&temp_DSA_str_reasons,5*sizeof(ERR_STRING_DATA));  		 
		   memcpy(&(g->GET_WSD_VAR_NAME(DSO_str_functs,dso_err,s)),&temp_DSO_str_functs,38*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(DSO_str_reasons,dso_err,s)),&temp_DSO_str_reasons,18*sizeof(ERR_STRING_DATA));  		 
		   memcpy(&(g->GET_WSD_VAR_NAME(ENGINE_str_functs,eng_err,s)),&temp_ENGINE_str_functs,37*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(ENGINE_str_reasons,eng_err,s)),&temp_ENGINE_str_reasons,40*sizeof(ERR_STRING_DATA));  		 
		   memcpy(&(g->GET_WSD_VAR_NAME(ERR_str_libraries,err_err,s)),&temp_ERR_str_libraries,25*sizeof(ERR_STRING_DATA));  		 
		   memcpy(&(g->GET_WSD_VAR_NAME(ERR_str_functs,err_err,s)),&temp_ERR_str_functs,11*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(ERR_str_reasons,err_err,s)),&temp_ERR_str_reasons,36*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(EVP_str_functs,evp_err,s)),&temp_EVP_str_functs,36*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(EVP_str_reasons,evp_err,s)),&temp_EVP_str_reasons,45*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(OBJ_str_functs,obj_err,s)),&temp_OBJ_str_functs,8*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(OBJ_str_reasons,obj_err,s)),&temp_OBJ_str_reasons,3*sizeof(ERR_STRING_DATA));  		 
		   memcpy(&(g->GET_WSD_VAR_NAME(OCSP_str_functs,ocsp_err,s)),&temp_OCSP_str_functs,18*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(OCSP_str_reasons,ocsp_err,s)),&temp_OCSP_str_reasons,31*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(PEM_str_functs,pem_err,s)),&temp_PEM_str_functs,27*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(PEM_str_reasons,pem_err,s)),&temp_PEM_str_reasons,17*sizeof(ERR_STRING_DATA));  		 
		   memcpy(&(g->GET_WSD_VAR_NAME(PKCS7_str_functs,pkcs7_err,s)),&temp_PKCS7_str_functs,29*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(PKCS7_str_reasons,pkcs7_err,s)),&temp_PKCS7_str_reasons,47*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(PKCS12_str_functs,pkcs12_err,s)),&temp_PKCS12_str_functs,30*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(PKCS12_str_reasons,pkcs12_err,s)),&temp_PKCS12_str_reasons,23*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(RAND_str_functs,rand_err,s)),&temp_RAND_str_functs,3*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(RAND_str_reasons,rand_err,s)),&temp_RAND_str_reasons,2*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(RSA_str_functs,rsa_err,s)),&temp_RSA_str_functs,38*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(RSA_str_reasons,rsa_err,s)),&temp_RSA_str_reasons,42*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(UI_str_functs,ui_err,s)),&temp_UI_str_functs,13*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(UI_str_reasons,ui_err,s)),&temp_UI_str_reasons,8*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(X509_str_functs,x509_err,s)),&temp_X509_str_functs,46*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(X509_str_reasons,x509_err,s)),&temp_X509_str_reasons,24*sizeof(ERR_STRING_DATA));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(X509V3_str_functs,x509v3_err,s)),&temp_X509V3_str_functs,57*sizeof(ERR_STRING_DATA));  		 
  		 memcpy(&(g->GET_WSD_VAR_NAME(X509V3_str_reasons,x509v3_err,s)),&temp_X509V3_str_reasons,61*sizeof(ERR_STRING_DATA));  		 
  		 
  		 
  		 
  		 /*START - static funcs*/
 		   memcpy(&(g->GET_WSD_VAR_NAME(impl_default,ex_data,s)),&temp_s_impl_default,sizeof(CRYPTO_EX_DATA_IMPL));
 		   memcpy(&(g->GET_WSD_VAR_NAME(mem_method,bss_mem,s)),&temp_s_mem_method,sizeof(BIO_METHOD));
 		   memcpy(&(g->GET_WSD_VAR_NAME(null_method,bss_null,s)),&temp_s_null_method,sizeof(BIO_METHOD));
       memcpy(&(g->GET_WSD_VAR_NAME(methods_fdp,bss_fd,s)),&temp_s_methods_fdp,sizeof(BIO_METHOD));  		 
       memcpy(&(g->GET_WSD_VAR_NAME(methods_sockp,bss_sock,s)),&temp_s_methods_sockp,sizeof(BIO_METHOD));  
       memcpy(&(g->GET_WSD_VAR_NAME(methods_filep,bss_file,s)),&temp_s_methods_filep,sizeof(BIO_METHOD));  		 		 
       memcpy(&(g->GET_WSD_VAR_NAME(methods_dgramp,bss_dgram,s)),&temp_s_methods_dgramp,sizeof(BIO_METHOD));  		 		 
       memcpy(&(g->GET_WSD_VAR_NAME(methods_connectp,bss_conn,s)),&temp_s_methods_connectp,sizeof(BIO_METHOD));  		 		 
       memcpy(&(g->GET_WSD_VAR_NAME(methods_biop,bss_bio,s)),&temp_s_methods_biop,sizeof(BIO_METHOD));  		 		 
       memcpy(&(g->GET_WSD_VAR_NAME(methods_acceptp,bss_acpt,s)),&temp_s_methods_acceptp,sizeof(BIO_METHOD));  		 		 
         
         
       memcpy(&(g->GET_WSD_VAR_NAME(default_method,conf_def,s)),&temp_s_default_method,sizeof(CONF_METHOD));  		 		 
       memcpy(&(g->GET_WSD_VAR_NAME(WIN32_method,conf_def,s)),&temp_s_WIN32_method,sizeof(CONF_METHOD));  		 		 
         
       memcpy(&(g->GET_WSD_VAR_NAME(rle_method,c_rle,s)),&temp_s_rle_method,sizeof(COMP_METHOD));  		 		 
       memcpy(&(g->GET_WSD_VAR_NAME(zlib_method_nozlib,c_zlib,s)),&temp_s_zlib_method_nozlib,sizeof(COMP_METHOD));  		 		 
#ifdef ZLIB       
       memcpy(&(g->GET_WSD_VAR_NAME(zlib_stateful_method,c_zlib,s)),&temp_s_zlib_stateful_method,sizeof(COMP_METHOD));  		 		 
#endif       
       

       memcpy(&(g->GET_WSD_VAR_NAME(dh_ossl,dh_key,s)),&temp_s_dh_ossl,sizeof(DH_METHOD));  
       memcpy(&(g->GET_WSD_VAR_NAME(openssl_dsa_meth,dsa_ossl,s)),&temp_s_openssl_dsa_meth,sizeof(DSA_METHOD));  		 		          		 		          
         
       //memcpy(&(g->GET_WSD_VAR_NAME(dso_meth_dl,dso_dl,s)),&temp_s_dso_meth_dl,sizeof(DSO_METHOD));  		 		          		 		          
       memcpy(&(g->GET_WSD_VAR_NAME(dso_meth_dlfcn,dso_dlfcn,s)),&temp_s_dso_meth_dlfcn,sizeof(DSO_METHOD));  		 		          		 		          
       memcpy(&(g->GET_WSD_VAR_NAME(dso_meth_null,dso_null,s)),&temp_s_dso_meth_null,sizeof(DSO_METHOD));  		 		          		 		          
         
         
       memcpy(&(g->GET_WSD_VAR_NAME(methods_md,bio_md,s)),&temp_s_methods_md,sizeof(BIO_METHOD));  		 		          		 		          
       memcpy(&(g->GET_WSD_VAR_NAME(methods_b64,bio_b64,s)),&temp_s_methods_b64,sizeof(BIO_METHOD));  		 		          		 		          
       memcpy(&(g->GET_WSD_VAR_NAME(methods_enc,bio_enc,s)),&temp_s_methods_enc,sizeof(BIO_METHOD));  		 		          		 		          
       memcpy(&(g->GET_WSD_VAR_NAME(methods_ok,bio_ok,s)),&temp_s_methods_ok,sizeof(BIO_METHOD));  		 		          		 		          
         
       memcpy(&(g->GET_WSD_VAR_NAME(rsa_pkcs1_eay_meth,rsa_eay,s)),&temp_s_rsa_pkcs1_eay_meth,sizeof(RSA_METHOD));  		 		          		 		                   
         
       memcpy(&(g->GET_WSD_VAR_NAME(store_memory,str_mem,s)),&temp_s_store_memory,sizeof(STORE_METHOD));  		 		          		 		                   
         
       memcpy(&(g->GET_WSD_VAR_NAME(ui_openssl,ui_openssl,s)),&temp_s_ui_openssl,sizeof(UI_METHOD));  		 		          		 		                   
       memcpy(&(g->GET_WSD_VAR_NAME(nconf_method,v3_conf,s)),&temp_s_nconf_method,sizeof(X509V3_CONF_METHOD));  		 		          		 		                   
       memcpy(&(g->GET_WSD_VAR_NAME(conf_lhash_method,v3_conf,s)),&temp_s_conf_lhash_method,sizeof(X509V3_CONF_METHOD));  		 		          		 		                   
         
       memcpy(&(g->GET_WSD_VAR_NAME(meth,x_x509,s)),&temp_s_meth,sizeof(ASN1_METHOD));  		 		          		 		                   
         
       memcpy(&(g->GET_WSD_VAR_NAME(method,rsa_asn1,s)),&temp_s_method,sizeof(ASN1_METHOD));  		 		          		 		                   
               
  		 /*START -global vars*/
  		 g->GET_WSD_VAR_NAME(cleanse_ctr,mem_clr,g)=0;
  		 memcpy(&(g->GET_WSD_VAR_NAME(rand_ssleay_meth,md_rand,g)),&temp_g_rand_ssleay_meth,sizeof(RAND_METHOD));  		 		          		 		                   
  		 memcpy(&(g->GET_WSD_VAR_NAME(x509_file_lookup,by_file,g)),&temp_g_x509_file_lookup,sizeof(X509_LOOKUP_METHOD));  		 		          		 		                   
  		 memcpy(&(g->GET_WSD_VAR_NAME(x509_dir_lookup,by_dir,g)),&temp_g_x509_dir_lookup,sizeof(X509_LOOKUP_METHOD));  		 		          		 		                   
  		 

				
	  }
	  return KErrNone;
}

