/* Written by Ben Laurie, 2001 */
/*
 * Copyright (c) 2001 The OpenSSL Project.  All rights reserved.
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
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
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
 */
/*
 © Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 */
#include <stdio.h>
#include <string.h>

#ifndef SYMBIAN
#include "../e_os.h"
#else
#include "e_os.h"
#endif

#include <openssl/opensslconf.h>
#include <openssl/evp.h>
#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif
#include <openssl/err.h>
#include <openssl/conf.h>
#ifdef SYMBIAN
#ifdef stdin
#undef stdin
#endif
#ifdef stdout
#undef stdout
#endif
#ifdef stderr
#undef stderr
#endif

#define stdin fp_stdin
#define stdout fp_stdout
#define stderr fp_stderr

extern FILE *fp_stdout;
extern FILE *fp_stderr;
#endif

static void hexdump(FILE *f,const char *title,const unsigned char *s,int l)
    {
    int n=0;

    fprintf(f,"%s",title);
    for( ; n < l ; ++n)
	{
	if((n%16) == 0)
	    fprintf(f,"\n%04x",n);
	fprintf(f," %02x",s[n]);
	}
    fprintf(f,"\n");
    }

static int convert(unsigned char *s)
    {
    unsigned char *d;

    for(d=s ; (*s)&&(*s!='\r') ; s+=2,++d)
	{
	unsigned int n;

	if(!s[1])
	    {
	    fprintf(stderr,"Odd number of hex digits!");
	    return 4;
	    //EXIT(4);
	    }
	sscanf((char *)s,"%2x",&n);
	*d=(unsigned char)n;
	}
    return s-d;
    }

static char *sstrsep(char **string, const char *delim)
    {
    char isdelim[256];
    char *token = *string;

    if (**string == 0)
        return NULL;

    memset(isdelim, 0, 256);
    isdelim[0] = 1;

    while (*delim)
        {
        isdelim[(unsigned char)(*delim)] = 1;
        delim++;
        }

    while (!isdelim[(unsigned char)(**string)])
        {
        (*string)++;
        }

    if (**string)
        {
        **string = 0;
        (*string)++;
        }

    return token;
    }

static unsigned char *ustrsep(char **p,const char *sep)
    { return (unsigned char *)sstrsep(p,sep); }

static int test1_exit(int ec)
	{
	
	return(0);		/* To keep some compilers quiet */
	}

static void test1(const EVP_CIPHER *c,const unsigned char *key,int kn,
		  const unsigned char *iv,int in,
		  const unsigned char *plaintext,int pn,
		  const unsigned char *ciphertext,int cn,
		  int encdec)
    {
    EVP_CIPHER_CTX ctx;
#ifndef SYMBIAN    
    unsigned char out[4096];
#else
    unsigned char out[400];
#endif    
    int outl,outl2;

    fprintf(stdout,"Testing cipher %s%s\n",EVP_CIPHER_name(c),
	   (encdec == 1 ? "(encrypt)" : (encdec == 0 ? "(decrypt)" : "(encrypt/decrypt)")));
    hexdump(stdout,"Key",key,kn);
    if(in)
	hexdump(stdout,"IV",iv,in);
    hexdump(stdout,"Plaintext",plaintext,pn);
    hexdump(stdout,"Ciphertext",ciphertext,cn);
    
    if(kn != c->key_len)
	{
	fprintf(stderr,"Key length doesn't match, got %d expected %d\n",kn,
		c->key_len);
	test1_exit(5);
	}
    EVP_CIPHER_CTX_init(&ctx);
 if(errno==ENOMEM)
   {
	  return ;
	 }    
    if (encdec != 0)
        {
	if(!EVP_EncryptInit_ex(&ctx,c,NULL,key,iv))
	    {
	    if(errno==ENOMEM)
     {
	    return ;
	    }    
 	    fprintf(stderr,"EncryptInit failed\n");
	    ERR_print_errors_fp(stderr);
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
 
	    test1_exit(10);
	    }
	EVP_CIPHER_CTX_set_padding(&ctx,0);

	if(!EVP_EncryptUpdate(&ctx,out,&outl,plaintext,pn))
	    {
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
 	    fprintf(stderr,"Encrypt failed\n");
	    ERR_print_errors_fp(stderr);
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
	    test1_exit(6);
	    }
	if(!EVP_EncryptFinal_ex(&ctx,out+outl,&outl2))
	    {
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
      fprintf(stderr,"EncryptFinal failed\n");
	    ERR_print_errors_fp(stderr);
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
      test1_exit(7);
	    }

	if(outl+outl2 != cn)
	    {
	    fprintf(stderr,"Ciphertext length mismatch got %d expected %d\n",
		    outl+outl2,cn);
	    test1_exit(8);
	    }

	if(memcmp(out,ciphertext,cn))
	    {
	    fprintf(stderr,"Ciphertext mismatch\n");
	    hexdump(stderr,"Got",out,cn);
	    hexdump(stderr,"Expected",ciphertext,cn);
	    test1_exit(9);
	    }
	}

    if (encdec <= 0)
        {
	if(!EVP_DecryptInit_ex(&ctx,c,NULL,key,iv))
	    {
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
	    fprintf(stderr,"DecryptInit failed\n");
	    ERR_print_errors_fp(stderr);
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
	    test1_exit(11);
	    }
	EVP_CIPHER_CTX_set_padding(&ctx,0);
   if(errno==ENOMEM)
   {
	    return ;
   }    


	if(!EVP_DecryptUpdate(&ctx,out,&outl,ciphertext,cn))
	    {
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
      fprintf(stderr,"Decrypt failed\n");
	    ERR_print_errors_fp(stderr);
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
	    test1_exit(6);
	    }
	if(!EVP_DecryptFinal_ex(&ctx,out+outl,&outl2))
	    {
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
      fprintf(stderr,"DecryptFinal failed\n");
	    ERR_print_errors_fp(stderr);
	    if(errno==ENOMEM)
      {
	    return ;
	    }    
	    test1_exit(7);
	    }

	if(outl+outl2 != cn)
	    {
	    fprintf(stderr,"Plaintext length mismatch got %d expected %d\n",
		    outl+outl2,cn);
	    test1_exit(8);
	    }

	if(memcmp(out,plaintext,cn))
	    {
	    fprintf(stderr,"Plaintext mismatch\n");
	    hexdump(stderr,"Got",out,cn);
	    hexdump(stderr,"Expected",plaintext,cn);
	    test1_exit(9);
	    }
	}

    EVP_CIPHER_CTX_cleanup(&ctx);
	    if(errno==ENOMEM)
      {
	    return ;
	    }    

    fprintf(stdout,"Test Case passed!\n");
    
    }

static int test_cipher(const char *cipher,const unsigned char *key,int kn,
		       const unsigned char *iv,int in,
		       const unsigned char *plaintext,int pn,
		       const unsigned char *ciphertext,int cn,
		       int encdec)
    {
    const EVP_CIPHER *c;

    c=EVP_get_cipherbyname(cipher);
   if(c==NULL&&errno==ENOMEM)
   {
	    return 0;
	 }  
    if(!c)
	 return 0;

    test1(c,key,kn,iv,in,plaintext,pn,ciphertext,cn,encdec);

    return 1;
    }

static int test_digest(const char *digest,
		       const unsigned char *plaintext,int pn,
		       const unsigned char *ciphertext, unsigned int cn)
    {
    const EVP_MD *d;
    EVP_MD_CTX ctx;
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int mdn;

    d=EVP_get_digestbyname(digest);
    	if(d==0&&errno==ENOMEM)
  {
	    return 0;
	}  

    if(!d)
	return 0;

    fprintf(stdout,"Testing digest %s\n",EVP_MD_name(d));
    hexdump(stdout,"Plaintext",plaintext,pn);
    hexdump(stdout,"Digest",ciphertext,cn);

    EVP_MD_CTX_init(&ctx);
   if(errno==ENOMEM)
   {
	    return 0;
	 }  

    if(!EVP_DigestInit_ex(&ctx,d, NULL))
	{
		if(errno==ENOMEM)
  {
	    return 0;
	}  
	
	fprintf(stderr,"DigestInit failed\n");
	ERR_print_errors_fp(stderr);
	return 100;
	//EXIT(100);
	}
    if(!EVP_DigestUpdate(&ctx,plaintext,pn))
	{
		if(errno==ENOMEM)
  {
	    return 0;
	}  
	
	fprintf(stderr,"DigestUpdate failed\n");
	ERR_print_errors_fp(stderr);
  if(errno==ENOMEM)
  {
	    return 0;
	}  

	return 101;
	//EXIT(101);
	}
    if(!EVP_DigestFinal_ex(&ctx,md,&mdn))
	{
		if(errno==ENOMEM)
  {
	    return 0;
	}  
	fprintf(stderr,"DigestFinal failed\n");
	ERR_print_errors_fp(stderr);
  if(errno==ENOMEM)
  {
	    return 0;
	}  

	return 101;
	//EXIT(101);
	}
    EVP_MD_CTX_cleanup(&ctx);
	if(errno==ENOMEM)
  {
	    return 0;
	}  

    if(mdn != cn)
	{
	fprintf(stderr,"Digest length mismatch, got %d expected %d\n",mdn,cn);
	return 102;
	//EXIT(102);
	}

    if(memcmp(md,ciphertext,cn))
	{
	fprintf(stderr,"Digest mismatch\n");
	hexdump(stderr,"Got",md,cn);
	hexdump(stderr,"Expected",ciphertext,cn);
	return 103;
		//EXIT(103);
	}

    fprintf(stdout,"done\n");
    
    EVP_MD_CTX_cleanup(&ctx);
	if(errno==ENOMEM)
  {
	    return 0;
	}  

    return 1;
    }
#ifndef SYMBIAN
int main(int argc,char **argv)
#else
int evp_main(int argc,char **argv)
#endif
    {
    const char *szTestFile;
    FILE *f;
    if(argc != 2)
	{
	fprintf(stderr,"%s <test file>\n",argv[0]);
	return 1;
	//EXIT(1);
	}
	
    /*CRYPTO_malloc_debug_init();
    if(errno==ENOMEM)
   {
	  return 1;
	 }
    CRYPTO_set_mem_debug_options(V_CRYPTO_MDEBUG_ALL);
     if(errno==ENOMEM)
   {
	  return 1;
	 }
    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
     if(errno==ENOMEM)
   {
	  return 1;
	 }*/

    szTestFile=argv[1];

    f=fopen(szTestFile,"r");
    if(!f)
	{
		 if(errno==ENOMEM)
   {
	  return 1;
	 }
	fprintf(stderr,"Couldn't open file");
	//perror(szTestFile);
	return 1;
	//EXIT(2);
	}
    fprintf(stderr,"Opened file sucessfully\n");
    /* Load up the software EVP_CIPHER and EVP_MD definitions */
    OpenSSL_add_all_ciphers();
     if(errno==ENOMEM)
   {
	  return 1;
	 }
    fprintf(stderr,"OpenSSL_add_all_ciphers(): done\n");
    OpenSSL_add_all_digests();
     if(errno==ENOMEM)
   {
	  return 1;
	 }
    fprintf(stderr,"OpenSSL_add_all_digests() : done\n");
#ifndef OPENSSL_NO_ENGINE
    /* Load all compiled-in ENGINEs */
    ENGINE_load_builtin_engines();
     if(errno==ENOMEM)
   {
	  return 1;
	 }
#endif
#if 0
    OPENSSL_config();
#endif
#ifndef OPENSSL_NO_ENGINE
    /* Register all available ENGINE implementations of ciphers and digests.
     * This could perhaps be changed to "ENGINE_register_all_complete()"? */
    ENGINE_register_all_ciphers();
     if(errno==ENOMEM)
   {
	  return 1;
	 }
    ENGINE_register_all_digests();
     if(errno==ENOMEM)
   {
	  return 1;
	 }
    /* If we add command-line options, this statement should be switchable.
     * It'll prevent ENGINEs being ENGINE_init()ialised for cipher/digest use if
     * they weren't already initialised. */
    /* ENGINE_set_cipher_flags(ENGINE_CIPHER_FLAG_NOINIT); */
#endif

    for( ; ; )
	{
#ifndef SYMBIAN		
	char line[4096];
#else
  char line[400];
#endif	
	char *p;
	char *cipher;
	unsigned char *iv,*key,*plaintext,*ciphertext;
	int encdec;
	int kn,in,pn,cn;

	if(!fgets((char *)line,sizeof line,f))
	    break;
	if(line[0] == '#' || line[0] == '\n'||line[0] == '\r')
	    continue;
	
	p=line;
	cipher=sstrsep(&p,":");	
	key=ustrsep(&p,":");
	iv=ustrsep(&p,":");
	plaintext=ustrsep(&p,":");
	ciphertext=ustrsep(&p,":");
	if (p[-1] == '\n') {
	    p[-1] = '\0';
	    encdec = -1;
	} else {
	    encdec = atoi(sstrsep(&p,"\n"));
	}
	      
    	kn=convert(key);
	
	in=convert(iv);
	pn=convert(plaintext);
	cn=convert(ciphertext);
    	if(!test_cipher(cipher,key,kn,iv,in,plaintext,pn,ciphertext,cn,encdec)
	   && !test_digest(cipher,plaintext,pn,ciphertext,cn))
	    {
 if(errno==ENOMEM)
   {
	  return 1;
	 }	    	
#ifdef OPENSSL_NO_AES
	    if (strstr(cipher, "AES") == cipher)
		{
		fprintf(stdout, "Cipher disabled, skipping %s\n", cipher); 
		continue;
		}
#endif
#ifdef OPENSSL_NO_DES
	    if (strstr(cipher, "DES") == cipher)
		{
		fprintf(stdout, "Cipher disabled, skipping %s\n", cipher); 
		continue;
		}
#endif
#ifdef OPENSSL_NO_RC4
	    if (strstr(cipher, "RC4") == cipher)
		{
		fprintf(stdout, "Cipher disabled, skipping %s\n", cipher); 
		continue;
		}
#endif
	    fprintf(stderr,"Can't find %s\n",cipher);
	    return 3;
	    //EXIT(3);
	    }
	}

#ifndef OPENSSL_NO_ENGINE
    ENGINE_cleanup();
    if(errno==ENOMEM)
   {
	  return 1;
	 }
#endif
    EVP_cleanup();
    if(errno==ENOMEM)
   {
	  return 1;
	 }
    CRYPTO_cleanup_all_ex_data();
   if(errno==ENOMEM)
   {
	  return 1;
	 }    
    ERR_remove_state(0);
     if(errno==ENOMEM)
   {
	  return 1;
	 }
    ERR_free_strings();
   if(errno==ENOMEM)
   {
	  return 1;
	 }    
    CRYPTO_mem_leaks_fp(stderr);
     if(errno==ENOMEM)
   {
	  return 1;
	 }
    fprintf(stderr,"*************END OF THE TEST CASE\n************");
    
    return 0;
    }
