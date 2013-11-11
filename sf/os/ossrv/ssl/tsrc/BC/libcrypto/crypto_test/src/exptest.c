/* crypto/bn/exptest.c */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */
/*
 © Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef SYMBIAN
#include "../e_os.h"
#else
#include "e_os.h"
#endif

#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/err.h>
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


#define NUM_BITS	(BN_BITS*2)

static const char rnd_seed[] = "string to make the random number generator think it has entropy";
#ifndef SYMBIAN
int main(int argc, char *argv[])
#else
int exp_main(int argc, char *argv[])
#endif
	{
	BN_CTX *ctx;
	BIO *out=NULL;
	int i,ret;
	unsigned char c;
	BIGNUM *r_mont,*r_mont_const,*r_recp,*r_simple,*a,*b,*m;
	//	FILE* temp;
  
    
	RAND_seed(rnd_seed, sizeof rnd_seed); /* or BN_rand may fail, and we don't
	                                       * even check its return value
	                                       * (which we should) */
  if(errno==ENOMEM)
  {
	  return 1;
  }	                                       

	ERR_load_BN_strings();
	if(errno==ENOMEM)
    {
	  return 1;
	}	    

	ctx=BN_CTX_new();
	if (ctx == NULL)
	{
	 if(errno==ENOMEM)
     {
	  return 1;
	 }
	 return 1;	    
	} 
	r_mont=BN_new();
	if(r_mont==NULL&&errno==ENOMEM)
    {
	  return 1;
	}	    
	r_mont_const=BN_new();
	if(r_mont_const==NULL&&errno==ENOMEM)
  {
	  return 1;
	}	    

	r_recp=BN_new();
	if(r_recp==NULL&&errno==ENOMEM)
   {
	  return 1;
	}	    

	r_simple=BN_new();
	if(r_simple==NULL&&errno==ENOMEM)
  {
	  return 1;
	}	    

	a=BN_new();
	if(a==NULL&&errno==ENOMEM)
  {
	  return 1;
	}	    

	b=BN_new();
	if(b==NULL&&errno==ENOMEM)
  {
	  return 1;
	}	    
	
	m=BN_new();
	if(m==NULL&&errno==ENOMEM)
  {
	  return 1;
	}	    
	
	if (	(r_mont == NULL) || (r_recp == NULL) ||
		(a == NULL) || (b == NULL))
		goto err;

	out=BIO_new(BIO_s_file());
	if(out==NULL&&errno==ENOMEM)
  {
	  return 1;
	}	    
	if (out == NULL) 
	return 1;
	BIO_set_fp(out,stdout,BIO_NOCLOSE);
	if(errno==ENOMEM)
  {
	  return 1;
	}	    

// temp = fopen("sanjeev.txt", "w");
	
	for (i=0; i<200; i++)
		{
	//	fputc(i,temp);
		RAND_bytes(&c,1);
  	if(errno==ENOMEM)
    {
	    return 1;
	  }	    

		c=(c%BN_BITS)-BN_BITS2;
		BN_rand(a,NUM_BITS+c,0,0);
		if(errno==ENOMEM)
    {
	   return 1;
	  }	    
		RAND_bytes(&c,1);
  	if(errno==ENOMEM)
    {
	    return 1;
	  }	    
		c=(c%BN_BITS)-BN_BITS2;
		BN_rand(b,NUM_BITS+c,0,0);
  	if(errno==ENOMEM)
    {
	    return 1;
	  }	    
		RAND_bytes(&c,1);
		if(errno==ENOMEM)
    {
	   return 1;
	  }	    

		c=(c%BN_BITS)-BN_BITS2;
		BN_rand(m,NUM_BITS+c,0,1);
		if(errno==ENOMEM)
    {
	   return 1;
	  }	    

		BN_mod(a,a,m,ctx);
		if(errno==ENOMEM)
    {
	   return 1;
	  }	    
	
		BN_mod(b,b,m,ctx);
		if(errno==ENOMEM)
      {
	   return 1;
	  }	    

		ret=BN_mod_exp_mont(r_mont,a,b,m,ctx,NULL);
		if (ret <= 0)
			{
			if(errno==ENOMEM)
      {
	     return 1;
	    }	    
			fprintf(stdout,"BN_mod_exp_mont() problems\n");
			ERR_print_errors(out);
  		if(errno==ENOMEM)
      {
	     return 1;
	    }	    
			return 1;
			}

		ret=BN_mod_exp_recp(r_recp,a,b,m,ctx);
		if (ret <= 0)
			{
  		if(errno==ENOMEM)
       {
	       return 1;
	     }	    
			fprintf(stdout,"BN_mod_exp_recp() problems\n");
			ERR_print_errors(out);
			if(errno==ENOMEM)
      {
	    return 1;
	    }	    
			return 1;
			}

		ret=BN_mod_exp_simple(r_simple,a,b,m,ctx);
		if (ret <= 0)
			{
  		if(errno==ENOMEM)
       {
	       return 1;
	     }	    
			fprintf(stdout,"BN_mod_exp_simple() problems\n");
			ERR_print_errors(out);
  		if(errno==ENOMEM)
       {
	       return 1;
	     }	    
			return 1;
			}

		ret=BN_mod_exp_mont_consttime(r_mont_const,a,b,m,ctx,NULL);
		if (ret <= 0)
			{
  		if(errno==ENOMEM)
       {
	       return 1;
	     }	    
			fprintf(stdout,"BN_mod_exp_mont_consttime() problems\n");
			ERR_print_errors(out);
  		if(errno==ENOMEM)
       {
	       return 1;
	     }	    
			return 1;
			}

		if (BN_cmp(r_simple, r_mont) == 0
		    && BN_cmp(r_simple,r_recp) == 0
			&& BN_cmp(r_simple,r_mont_const) == 0)
			{
  		if(errno==ENOMEM)
       {
	       return 1;
	     }	    
      fprintf(stdout,".");
			fflush(stdout);
			}
		else
		  	{
			if (BN_cmp(r_simple,r_mont) != 0)
       {
 	  		if(errno==ENOMEM)
        {
	        return 1;
	      }	    
				fprintf(stdout,"\nsimple and mont results differ\n");
       }				
			if (BN_cmp(r_simple,r_mont) != 0)
			{
 	  		if(errno==ENOMEM)
        {
	        return 1;
	      }	    
				fprintf(stdout,"\nsimple and mont const time results differ\n");
			}	
			if (BN_cmp(r_simple,r_recp) != 0)
			{
 	  		if(errno==ENOMEM)
        {
	        return 1;
	      }	    
				fprintf(stdout,"\nsimple and recp results differ\n");
      }
			fprintf(stdout,"a (%3d) = ",BN_num_bits(a));   BN_print(out,a);
  		if(errno==ENOMEM)
      {
	       return 1;
      }	    
			fprintf(stdout,"\nb (%3d) = ",BN_num_bits(b)); BN_print(out,b);
 	  	if(errno==ENOMEM)
      {
	       return 1;
      }	    
			fprintf(stdout,"\nm (%3d) = ",BN_num_bits(m)); BN_print(out,m);
 	  	if(errno==ENOMEM)
      {
	       return 1;
      }	    
			fprintf(stdout,"\nsimple   =");	BN_print(out,r_simple);
 	  	if(errno==ENOMEM)
      {
	       return 1;
      }	    
			fprintf(stdout,"\nrecp     =");	BN_print(out,r_recp);
 	  	if(errno==ENOMEM)
      {
	       return 1;
      }	    
			fprintf(stdout,"\nmont     ="); BN_print(out,r_mont);
 	  	if(errno==ENOMEM)
      {
	       return 1;
      }	    
			fprintf(stdout,"\nmont_ct  ="); BN_print(out,r_mont_const);
 	  	if(errno==ENOMEM)
      {
	       return 1;
      }	    
			fprintf(stdout,"\n");
			return 1;
			}
		}
	BN_free(r_mont);
	BN_free(r_mont_const);
	BN_free(r_recp);
	BN_free(r_simple);
	BN_free(a);
	BN_free(b);
	BN_free(m);
	BN_CTX_free(ctx);
	ERR_remove_state(0);
	if(errno==ENOMEM)
  {
    return 1;
  }	    
	CRYPTO_mem_leaks(out);
	if(errno==ENOMEM)
  {
    return 1;
  }	    
	BIO_free(out);
	if(errno==ENOMEM)
   {
	  return 1;
	 }    
 
    CRYPTO_cleanup_all_ex_data();
   if(errno==ENOMEM)
   {
	  return 1;
	 }    
 
	fprintf(stdout," done\n");
	fprintf(stdout," Test case passed\n");
	return 0;
err:
	ERR_load_crypto_strings();
	if(errno==ENOMEM)
  {
    return 1;
  }	    
	
	ERR_print_errors(out);
	if(errno==ENOMEM)
  {
    return 1;
  }	    
	
	
#ifdef OPENSSL_SYS_NETWARE
    fprintf(stdout,"ERROR\n");
#endif
	return(1);
	}

