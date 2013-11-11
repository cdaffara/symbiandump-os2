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

#ifndef _LIBCRYPTO_WSD_H
#define _LIBCRYPTO_WSD_H

#include <_ansi.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <libcrypto_wsd_macros.h>
#include "cryptlib.h"
#include <openssl/asn1.h>
#include <openssl/lhash.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/comp.h>
#include <openssl/dso.h>
#include <openssl/des.h>
#include <openssl/engine.h>
#include <openssl/lhash.h>
#include <openssl/ossl_typ.h>
#include "eng_int.h"
#include "rand_lcl.h"
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/dh.h>
#include <openssl/dso.h>
#include <openssl/dsa.h>
#include <openssl/rsa.h>
#include <openssl/store.h>
#include <openssl/x509v3.h>
#include <openssl/ui.h>
#include <openssl/rand.h>
#include "str_locl.h"
#include "ui_locl.h"
#include <zlib.h>

#ifdef EMULATOR
#define BN_NIST_256_TOP	(256+BN_BITS2-1)/BN_BITS2
#define BN_NIST_384_TOP	(384+BN_BITS2-1)/BN_BITS2

# define Z_CALLCONV

typedef int (Z_CALLCONV *compress_ft)(Bytef *dest,uLongf *destLen,
	const Bytef *source, uLong sourceLen);
typedef int (Z_CALLCONV *inflateEnd_ft)(z_streamp strm);
typedef int (Z_CALLCONV *inflate_ft)(z_streamp strm, int flush);
typedef int (Z_CALLCONV *inflateInit__ft)(z_streamp strm,
	const char * version, int stream_size);
typedef int (Z_CALLCONV *deflateEnd_ft)(z_streamp strm);
typedef int (Z_CALLCONV *deflate_ft)(z_streamp strm, int flush);
typedef int (Z_CALLCONV *deflateInit__ft)(z_streamp strm, int level,
	const char * version, int stream_size);

/* What an "implementation of ex_data functionality" looks like */
struct st_CRYPTO_EX_DATA_IMPL
	{
	/*********************/
	/* GLOBAL OPERATIONS */
	/* Return a new class index */
	int (*cb_new_class)(void);
	/* Cleanup all state used by the implementation */
	void (*cb_cleanup)(void);
	/************************/
	/* PER-CLASS OPERATIONS */
	/* Get a new method index within a class */
	int (*cb_get_new_index)(int class_index, long argl, void *argp,
			CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func,
			CRYPTO_EX_free *free_func);
	/* Initialise a new CRYPTO_EX_DATA of a given class */
	int (*cb_new_ex_data)(int class_index, void *obj,
			CRYPTO_EX_DATA *ad);
	/* Duplicate a CRYPTO_EX_DATA of a given class onto a copy */
	int (*cb_dup_ex_data)(int class_index, CRYPTO_EX_DATA *to,
			CRYPTO_EX_DATA *from);
	/* Cleanup a CRYPTO_EX_DATA of a given class */
	void (*cb_free_ex_data)(int class_index, void *obj,
			CRYPTO_EX_DATA *ad);
	};

struct st_ERR_FNS
	{
	/* Works on the "error_hash" string table */
	LHASH *(*cb_err_get)(int create);
	void (*cb_err_del)(void);
	ERR_STRING_DATA *(*cb_err_get_item)(const ERR_STRING_DATA *);
	ERR_STRING_DATA *(*cb_err_set_item)(ERR_STRING_DATA *);
	ERR_STRING_DATA *(*cb_err_del_item)(ERR_STRING_DATA *);
	/* Works on the "thread_hash" error-state table */
	LHASH *(*cb_thread_get)(int create);
	void (*cb_thread_release)(LHASH **hash);
	ERR_STATE *(*cb_thread_get_item)(const ERR_STATE *);
	ERR_STATE *(*cb_thread_set_item)(ERR_STATE *);
	void (*cb_thread_del_item)(const ERR_STATE *);
	/* Returns the next available error "library" numbers */
	int (*cb_get_next_lib)(void);
	};


#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct
{
	
	
	VARIABLE_DECL(tntmp,s,asn1_gen,struct tag_name_st*)
    VARIABLE_DECL(ia5string_meth,s,a_meth,ASN1_METHOD)
    VARIABLE_DECL(bit_string_meth,s,a_meth,ASN1_METHOD)
    VARIABLE_DECL(stable,s,a_strnid,STACK_OF(ASN1_STRING_TABLE)*)
    VARIABLE_DECL(global_mask,s,a_strnid,unsigned long)
    VARIABLE_DECL(app_locks,s,cryptlib,STACK *)
    VARIABLE_DECL(dyn_locks,s,cryptlib,STACK_OF(CRYPTO_dynlock)*)
    VARIABLE_DECL(allow_customize,s,mem,int)
    VARIABLE_DECL(allow_customize_debug,s,mem,int)
    VARIABLE_DECL(mh_mode,s,mem_dbg,int)
    VARIABLE_DECL(order,s,mem_dbg,unsigned long)
    VARIABLE_DECL(mh,s,mem_dbg,LHASH *)
    VARIABLE_DECL(amih,s,mem_dbg,LHASH *)
    VARIABLE_DECL(options,s,mem_dbg,unsigned long)
    VARIABLE_DECL(num_disable,s,mem_dbg,unsigned int)
    VARIABLE_DECL(disabling_thread,s,mem_dbg,unsigned long)
    VARIABLE_DECL(impl,s,ex_data,CRYPTO_EX_DATA_IMPL *)
    VARIABLE_DECL(ex_class,s,ex_data,int)
    VARIABLE_DECL(ex_data,s,ex_data,LHASH *)
    VARIABLE_DECL(init,s,bn_lib,int)
    VARIABLE_DECL_ARRAY(data,s,bn_lib,char,16)
    VARIABLE_DECL(data_one,s,bn_lib,BN_ULONG)
    VARIABLE_DECL(const_one,s,bn_lib,BIGNUM)
    VARIABLE_DECL_ARRAY(_256_data,s,bn_nist,BN_ULONG,BN_NIST_256_TOP*6)
    VARIABLE_DECL(_is_set_256_data,s,bn_nist,int)
    VARIABLE_DECL_ARRAY(_384_data,s,bn_nist,BN_ULONG,BN_NIST_384_TOP*8)
    VARIABLE_DECL(_is_set_384_data,s,bn_nist,int)
    VARIABLE_DECL(zlib_stateful_ex_idx,s,c_zlib,int)
    VARIABLE_DECL(p_compress,s,c_zlib,compress_ft)
    VARIABLE_DECL(p_inflateEnd,s,c_zlib,inflateEnd_ft)
    VARIABLE_DECL(p_inflate,s,c_zlib,inflate_ft)
    VARIABLE_DECL(p_inflateInit_,s,c_zlib,inflateInit__ft)
    VARIABLE_DECL(p_deflateEnd,s,c_zlib,deflateEnd_ft)
    VARIABLE_DECL(p_deflate,s,c_zlib,deflate_ft)
    VARIABLE_DECL(p_deflateInit_,s,c_zlib,deflateInit__ft)
    VARIABLE_DECL(zlib_loaded,s,c_zlib,int)
    VARIABLE_DECL(zlib_dso,s,c_zlib,DSO *)
 	  VARIABLE_DECL(supported_modules,s,conf_mod,STACK_OF(CONF_MODULE)*)
    VARIABLE_DECL(initialized_modules,s,conf_mod,STACK_OF(CONF_MODULE)*)
    VARIABLE_DECL(openssl_configured,s,conf_sap,int)
    VARIABLE_DECL(init,s,ecb_enc,int)
    VARIABLE_DECL_ARRAY(buf,s,ecb_enc,char,32)
    VARIABLE_DECL(net,s,enc_read,unsigned char *)
    VARIABLE_DECL(unnet,s,enc_read,unsigned char *)
    VARIABLE_DECL(unnet_start,s,enc_read,int)
    VARIABLE_DECL(unnet_left,s,enc_read,int)
    VARIABLE_DECL(tmpbuf,s,enc_read,unsigned char *)
    VARIABLE_DECL(outbuf,s,enc_write,unsigned char *)
    VARIABLE_DECL(start,s,enc_write,int)
    VARIABLE_DECL_ARRAY(weak_keys,s,set_key_c,DES_cblock,16)
    VARIABLE_DECL(cleanup_stack,s,eng_lib,STACK_OF(ENGINE_CLEANUP_ITEM) *)
    VARIABLE_DECL(internal_static_hack,s,eng_lib,int)
    VARIABLE_DECL(engine_list_head,s,eng_list,ENGINE *)
    VARIABLE_DECL(engine_list_tail,s,eng_list,ENGINE *)
    VARIABLE_DECL(table_flags,s,eng_table,unsigned int)
    VARIABLE_DECL(initialized_engines,s,eng_cnf,STACK_OF(ENGINE) *)
    VARIABLE_DECL(dynamic_ex_data_idx,s,eng_dyn,int)
    VARIABLE_DECL(rsa_table,s,tb_rsa,ENGINE_TABLE *)
    VARIABLE_DECL(dsa_table,s,tb_dsa,ENGINE_TABLE *)
    VARIABLE_DECL(ecdh_table,s,tb_ecdh,ENGINE_TABLE *)
    VARIABLE_DECL(ecdsa_table,s,tb_ecdsa,ENGINE_TABLE *)
    VARIABLE_DECL(rand_table,s,tb_rand,ENGINE_TABLE *)
    VARIABLE_DECL(digest_table,s,tb_digest,ENGINE_TABLE *)
    VARIABLE_DECL(store_table,s,tb_store,ENGINE_TABLE *)
    VARIABLE_DECL(dh_table,s,tb_dh,ENGINE_TABLE *)
    VARIABLE_DECL(cipher_table,s,tb_cipher,ENGINE_TABLE *)
    VARIABLE_DECL(err_fns,s,err,const ERR_FNS *)
    VARIABLE_DECL(int_error_hash,s,err,LHASH *)
    VARIABLE_DECL(int_thread_hash,s,err,LHASH *)
    VARIABLE_DECL(int_thread_hash_references,s,err,int)
    VARIABLE_DECL(int_err_library_number,s,err,int) 
    VARIABLE_DECL(init,s,err,int)
    VARIABLE_DECL(pbe_algs,s,evp_pbe,STACK *) 
    VARIABLE_DECL(names_lh,s,o_names,LHASH *)
    VARIABLE_DECL(names_type_num,s,o_names,int)
    VARIABLE_DECL(name_funcs_stack,s,o_names,STACK_OF(NAME_FUNCS)*)
    VARIABLE_DECL(free_type,s,o_names,int)     
    VARIABLE_DECL(new_nid,s,obj_dat,int)
    VARIABLE_DECL(added,s,obj_dat,LHASH *)
    VARIABLE_DECL(state_num,s,md_rand,int)
    VARIABLE_DECL(state_index,s,md_rand,int)
    VARIABLE_DECL_ARRAY(state,s,md_rand,unsigned char,1023+MD_DIGEST_LENGTH)
    VARIABLE_DECL_ARRAY(md,s,md_rand,unsigned char,MD_DIGEST_LENGTH)
    VARIABLE_DECL_ARRAY(md_count,s,md_rand,unsigned char,2)
    VARIABLE_DECL(entropy,s,md_rand,double)
    VARIABLE_DECL(initialized,s,md_rand,int)
    VARIABLE_DECL(crypto_lock_rand,s,md_rand,unsigned int)
    VARIABLE_DECL(locking_thread,s,md_rand,unsigned long)
    VARIABLE_DECL(default_RAND_meth,s,rand_lib,const RAND_METHOD *)
    VARIABLE_DECL(tty_in,s,ui_openssl,FILE *)
    VARIABLE_DECL(tty_out,s,ui_openssl,FILE *)
    VARIABLE_DECL(is_a_tty,s,ui_openssl,int)
    VARIABLE_DECL(ext_nids,s,x509_req,int*)
    VARIABLE_DECL_ARRAY(ext_nid_list,s,x509_req,int,3)
    VARIABLE_DECL(trtable,s,x509_trs,STACK_OF(X509_TRUST) *)
    VARIABLE_DECL(param_table,s,x509_vpm,STACK_OF(X509_VERIFY_PARAM) *)
    VARIABLE_DECL(ext_list,s,v3_lib,STACK_OF(X509V3_EXT_METHOD) *)
    VARIABLE_DECL(xptable,s,v3_purp,STACK_OF(X509_PURPOSE)*)
    VARIABLE_DECL(from,s,b_sock,struct sockaddr_in)    
    VARIABLE_DECL_ARRAY(buff,s,fcrypt,char,14)
    VARIABLE_DECL_ARRAY(buf,s,err,char,256)
    VARIABLE_DECL(fallback,s,err,ERR_STATE)
    VARIABLE_DECL_ARRAY(SYS_str_reasons,s,err,ERR_STRING_DATA,128)//NUM_SYS_STR_REASONS + 1
    VARIABLE_DECL_ARRAY(prompt_string,s,evp_key,char,80)
    VARIABLE_DECL_ARRAY(m,s,hmac,unsigned char,EVP_MAX_MD_SIZE)
    VARIABLE_DECL_ARRAY(m,s,md2,unsigned char,EVP_MAX_MD_SIZE)
    VARIABLE_DECL_ARRAY(m,s,md5,unsigned char,EVP_MAX_MD_SIZE)
    VARIABLE_DECL_ARRAY(m,s,sha1_one,unsigned char,SHA_DIGEST_LENGTH)
    VARIABLE_DECL_ARRAY(m,s,sha_one,unsigned char,EVP_MAX_MD_SIZE)
    VARIABLE_DECL_ARRAY(buf,s,x509_txt,char,100)
    VARIABLE_DECL_ARRAY(ASN1_str_functs,s,asn1_err, ERR_STRING_DATA,108)
    VARIABLE_DECL_ARRAY(ASN1_str_reasons,s,asn1_err,ERR_STRING_DATA,99)
    VARIABLE_DECL_ARRAY(CRYPTO_str_functs,s,cpt_err,ERR_STRING_DATA,10)
    VARIABLE_DECL_ARRAY(CRYPTO_str_reasons,s,cpt_err,ERR_STRING_DATA,2)
    VARIABLE_DECL_ARRAY(BIO_str_functs,s,bio_err,ERR_STRING_DATA,33)
    VARIABLE_DECL_ARRAY(BIO_str_reasons,s,bio_err,ERR_STRING_DATA,30)
    VARIABLE_DECL_ARRAY(BN_str_functs,s,bn_err,ERR_STRING_DATA,39)
    VARIABLE_DECL_ARRAY(BN_str_reasons,s,bn_err,ERR_STRING_DATA,18)
    VARIABLE_DECL_ARRAY(BUF_str_functs,s,buf_err,ERR_STRING_DATA,7)
    VARIABLE_DECL_ARRAY(BUF_str_reasons,s,buf_err,ERR_STRING_DATA,1)
    VARIABLE_DECL_ARRAY(COMP_str_functs,s,comp_err,ERR_STRING_DATA,1)
    VARIABLE_DECL_ARRAY(COMP_str_reasons,s,comp_err,ERR_STRING_DATA,1)
    VARIABLE_DECL_ARRAY(CONF_str_functs,s,conf_err,ERR_STRING_DATA,22)
    VARIABLE_DECL_ARRAY(CONF_str_reasons,s,conf_err,ERR_STRING_DATA,16)
    VARIABLE_DECL_ARRAY(DH_str_functs,s,dh_err,ERR_STRING_DATA,8)
    VARIABLE_DECL_ARRAY(DH_str_reasons,s,dh_err,ERR_STRING_DATA,4)
    VARIABLE_DECL_ARRAY(DSA_str_functs,s,dsa_err,ERR_STRING_DATA,15)
    VARIABLE_DECL_ARRAY(DSA_str_reasons,s,dsa_err,ERR_STRING_DATA,3)
    VARIABLE_DECL_ARRAY(DSO_str_functs,s,dso_err,ERR_STRING_DATA,38)
    VARIABLE_DECL_ARRAY(DSO_str_reasons,s,dso_err,ERR_STRING_DATA,18)
    VARIABLE_DECL_ARRAY(ENGINE_str_functs,s,eng_err,ERR_STRING_DATA,37)
    VARIABLE_DECL_ARRAY(ENGINE_str_reasons,s,eng_err,ERR_STRING_DATA,40)
    VARIABLE_DECL_ARRAY(ERR_str_libraries,s,err_err,ERR_STRING_DATA,25)
    VARIABLE_DECL_ARRAY(ERR_str_functs,s,err_err,ERR_STRING_DATA,11)
    VARIABLE_DECL_ARRAY(ERR_str_reasons,s,err_err,ERR_STRING_DATA,36)
    VARIABLE_DECL_ARRAY(EVP_str_functs,s,evp_err,ERR_STRING_DATA,36)
    VARIABLE_DECL_ARRAY(EVP_str_reasons,s,evp_err,ERR_STRING_DATA,45)
    VARIABLE_DECL_ARRAY(OBJ_str_functs,s,obj_err,ERR_STRING_DATA,8)
    VARIABLE_DECL_ARRAY(OBJ_str_reasons,s,obj_err,ERR_STRING_DATA,3)
    VARIABLE_DECL_ARRAY(OCSP_str_functs,s,ocsp_err,ERR_STRING_DATA,18)
    VARIABLE_DECL_ARRAY(OCSP_str_reasons,s,ocsp_err,ERR_STRING_DATA,31)
    VARIABLE_DECL_ARRAY(PEM_str_functs,s,pem_err,ERR_STRING_DATA,27)
    VARIABLE_DECL_ARRAY(PEM_str_reasons,s,pem_err,ERR_STRING_DATA,17)
    VARIABLE_DECL_ARRAY(PKCS7_str_functs,s,pkcs7_err,ERR_STRING_DATA,29)
    VARIABLE_DECL_ARRAY(PKCS7_str_reasons,s,pkcs7_err,ERR_STRING_DATA,47)
    VARIABLE_DECL_ARRAY(PKCS12_str_functs,s,pkcs12_err,ERR_STRING_DATA,30)
    VARIABLE_DECL_ARRAY(PKCS12_str_reasons,s,pkcs12_err,ERR_STRING_DATA,23)
    VARIABLE_DECL_ARRAY(RAND_str_functs,s,rand_err,ERR_STRING_DATA,3)
    VARIABLE_DECL_ARRAY(RAND_str_reasons,s,rand_err,ERR_STRING_DATA,2)
    VARIABLE_DECL_ARRAY(RSA_str_functs,s,rsa_err,ERR_STRING_DATA,38)
    VARIABLE_DECL_ARRAY(RSA_str_reasons,s,rsa_err,ERR_STRING_DATA,41)
    VARIABLE_DECL_ARRAY(UI_str_functs,s,ui_err,ERR_STRING_DATA,13)
    VARIABLE_DECL_ARRAY(UI_str_reasons,s,ui_err,ERR_STRING_DATA,8)
    VARIABLE_DECL_ARRAY(X509_str_functs,s,x509_err,ERR_STRING_DATA,46)
    VARIABLE_DECL_ARRAY(X509_str_reasons,s,x509_err,ERR_STRING_DATA,24)
    VARIABLE_DECL_ARRAY(X509V3_str_functs,s,x509v3_err,ERR_STRING_DATA,57)
    VARIABLE_DECL_ARRAY(X509V3_str_reasons,s,x509v3_err,ERR_STRING_DATA,61)
    VARIABLE_DECL(default_CONF_method,s,conf_lib,const CONF_METHOD *)
    VARIABLE_DECL(default_DH_method,s,dh_lib,const DH_METHOD *)
    VARIABLE_DECL(default_DSA_method,s,dsa_lib,const DSA_METHOD *)
    VARIABLE_DECL(default_DSO_meth,s,dso_lib,DSO_METHOD *)
    VARIABLE_DECL(default_RSA_meth,s,rsa_lib,const RSA_METHOD *)
    VARIABLE_DECL(default_UI_meth,s,ui_lib,const UI_METHOD *)
    
    void *(*malloc_func)(size_t);
    void *(*malloc_ex_func)(size_t, const char *file, int line);
    void *(*realloc_func)(void *, size_t);
    void *(*realloc_ex_func)(void *, size_t, const char *file, int line);
    void (*free_func_openssl)(void *);
    void *(*malloc_locked_func)(size_t);
    void *(*malloc_locked_ex_func)(size_t, const char *file, int line);
    void (*free_locked_func)(void *);
    void (*malloc_debug_func)(void *,int,const char *,int,int);
	  void (*realloc_debug_func)(void *,void *,int,const char *,int,int);
    void (*free_debug_func)(void *,int);
    void (*set_debug_options_func)(long);
    long (*get_debug_options_func)(void);
    void (MS_FAR *locking_callback)(int mode,int type,
    const char *file,int line);
    int (MS_FAR *add_lock_callback)(int *pointer,int amount,
   	int type,const char *file,int line);
    unsigned long (MS_FAR *id_callback)(void);
    struct CRYPTO_dynlock_value *(MS_FAR *dynlock_create_callback)
	  (const char *file,int line);
    void (MS_FAR *dynlock_lock_callback)(int mode,
	  struct CRYPTO_dynlock_value *l, const char *file,int line);
    void (MS_FAR *dynlock_destroy_callback)(struct CRYPTO_dynlock_value *l,
	  const char *file,int line);
    
    /*START-static structs*/
   	VARIABLE_DECL(impl_default,s,ex_data,CRYPTO_EX_DATA_IMPL)
   	VARIABLE_DECL(mem_method,s,bss_mem,BIO_METHOD)
   	VARIABLE_DECL(null_method,s,bss_null,BIO_METHOD)
    VARIABLE_DECL(methods_fdp,s,bss_fd,BIO_METHOD)
	  VARIABLE_DECL(methods_sockp,s,bss_sock,BIO_METHOD)
	  VARIABLE_DECL(methods_filep,s,bss_file,BIO_METHOD)
	  VARIABLE_DECL(methods_dgramp,s,bss_dgram,BIO_METHOD)
	  VARIABLE_DECL(methods_connectp,s,bss_conn,BIO_METHOD)
	  VARIABLE_DECL(methods_biop,s,bss_bio,BIO_METHOD)
	  VARIABLE_DECL(methods_acceptp,s,bss_acpt,BIO_METHOD)
	  VARIABLE_DECL(rle_method,s,c_rle,COMP_METHOD)
	  VARIABLE_DECL(zlib_method_nozlib,s,c_zlib,COMP_METHOD)
#ifdef ZLIB	  
    VARIABLE_DECL(zlib_stateful_method,s,c_zlib,COMP_METHOD)
#endif    
    VARIABLE_DECL(default_method,s,conf_def,CONF_METHOD)
    VARIABLE_DECL(WIN32_method,s,conf_def,CONF_METHOD)
    VARIABLE_DECL(dh_ossl,s,dh_key,DH_METHOD)
    VARIABLE_DECL(openssl_dsa_meth,s,dsa_ossl,DSA_METHOD)
    //VARIABLE_DECL(dso_meth_dl,s,dso_dl,DSO_METHOD)
    VARIABLE_DECL(dso_meth_dlfcn,s,dso_dlfcn,DSO_METHOD)
    VARIABLE_DECL(dso_meth_null,s,dso_null,DSO_METHOD)
    VARIABLE_DECL(methods_md,s,bio_md,BIO_METHOD)
    VARIABLE_DECL(methods_b64,s,bio_b64,BIO_METHOD)
    VARIABLE_DECL(methods_enc,s,bio_enc,BIO_METHOD)
    VARIABLE_DECL(methods_ok,s,bio_ok,BIO_METHOD)
    VARIABLE_DECL(rsa_pkcs1_eay_meth,s,rsa_eay,RSA_METHOD )
    VARIABLE_DECL(store_memory,s,str_mem,STORE_METHOD)
    VARIABLE_DECL(ui_openssl,s,ui_openssl,UI_METHOD)
    VARIABLE_DECL(nconf_method,s,v3_conf,X509V3_CONF_METHOD)
    VARIABLE_DECL(conf_lhash_method,s,v3_conf,X509V3_CONF_METHOD)
    VARIABLE_DECL(meth,s,x_x509,ASN1_METHOD)
    VARIABLE_DECL(method,s,rsa_asn1,ASN1_METHOD)
    VARIABLE_DECL(stirred_pool,s,md_rand,volatile int)
 
    char strerror_tab[127][32];
    int _shadow_DES_rw_mode;
    int _shadow_DES_check_key;

    /*END-static structs*/   
    /*START-global vars*/
    VARIABLE_DECL(cleanse_ctr,g,mem_clr,unsigned char)
    VARIABLE_DECL(rand_ssleay_meth,g,md_rand, RAND_METHOD)
    VARIABLE_DECL(x509_file_lookup,g,by_file,X509_LOOKUP_METHOD)
    VARIABLE_DECL(x509_dir_lookup,g,by_dir,X509_LOOKUP_METHOD)
   
    /*END-global vars*/
 
    }libcrypto_global_struct;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" 
{
#endif
libcrypto_global_struct * libcrypto_ImpurePtr();
int libcrypto_Init(libcrypto_global_struct *);
#ifdef __cplusplus
}
#endif

#endif /* EMULATOR */
#endif //_LIBCRYPTO_WSD_H