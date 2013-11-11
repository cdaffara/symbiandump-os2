/* crypto/engine/enginetest.c */
/* Written by Geoff Thorpe (geoff@geoffthorpe.net) for the OpenSSL
 * project 2000.
 */
/* ====================================================================
 * Copyright (c) 1999-2001 The OpenSSL Project.  All rights reserved.
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
#include <stdio.h>
#include <string.h>

#ifdef OPENSSL_NO_ENGINE
int main(int argc, char *argv[])
{
    printf("No ENGINE support\n");
    return(0);
}
#else
#include <openssl/e_os2.h>
#include <openssl/buffer.h>
#include <openssl/crypto.h>
#include <openssl/engine.h>
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

static void display_engine_list(void)
	{
	ENGINE *h;
	int loop;

	h = ENGINE_get_first();
	if(errno==ENOMEM)
  {
	  return ;
	}
	loop = 0;
	fprintf(stdout,"listing available engine types\n");
	if(errno==ENOMEM)
  {
	  return ;
	}
	while(h)
		{
		fprintf(stdout,"engine %i, id = \"%s\", name = \"%s\"\n",
			loop++, ENGINE_get_id(h), ENGINE_get_name(h));
		if(errno==ENOMEM)
   	{
	  return ;
	  }	
		h = ENGINE_get_next(h);
		if(errno==ENOMEM)
   	{
	  return ;
	  }
		}
	fprintf(stdout,"end of list\n");
	if(errno==ENOMEM)
  {
	  return ;
	}
	/* ENGINE_get_first() increases the struct_ref counter, so we 
           must call ENGINE_free() to decrease it again */
	ENGINE_free(h);
	if(errno==ENOMEM)
  {
	  return ;
	}
	}

#ifndef SYMBIAN
int main(int argc, char *argv[])
#else
int engine_main(int argc, char *argv[])
#endif

	{
	ENGINE *block[512];
	char buf[256];
	const char *id, *name;
	ENGINE *ptr;
	int loop;
	int to_return = 1;
	ENGINE *new_h1 = NULL;
	ENGINE *new_h2 = NULL;
	ENGINE *new_h3 = NULL;
	ENGINE *new_h4 = NULL;
    
	/* enable memory leak checking unless explicitly disabled */

	ERR_load_crypto_strings();
	if(errno==ENOMEM)
  {
	  return 1;
	}

	memset(block, 0, 512 * sizeof(ENGINE *));
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(((new_h1 = ENGINE_new()) == NULL) ||
			!ENGINE_set_id(new_h1, "test_id0") ||
			!ENGINE_set_name(new_h1, "First test item") ||
			((new_h2 = ENGINE_new()) == NULL) ||
			!ENGINE_set_id(new_h2, "test_id1") ||
			!ENGINE_set_name(new_h2, "Second test item") ||
			((new_h3 = ENGINE_new()) == NULL) ||
			!ENGINE_set_id(new_h3, "test_id2") ||
			!ENGINE_set_name(new_h3, "Third test item") ||
			((new_h4 = ENGINE_new()) == NULL) ||
			!ENGINE_set_id(new_h4, "test_id3") ||
			!ENGINE_set_name(new_h4, "Fourth test item"))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Couldn't set up test ENGINE structures\n");
		goto end;
		}
	fprintf(stdout,"\nenginetest beginning\n\n");
	if(errno==ENOMEM)
  {
	  return 1;
	}
	display_engine_list();
	if(!ENGINE_add(new_h1))
		{
		fprintf(stdout,"Add failed!\n");
		goto end;
		}
	display_engine_list();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	ptr = ENGINE_get_first();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(!ENGINE_remove(ptr))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Remove failed!\n");
		goto end;
		}
	if (ptr)
	{
		ENGINE_free(ptr);
		if(errno==ENOMEM)
   	{
	  return 1;
	  }
	}	
	display_engine_list();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(!ENGINE_add(new_h3) || !ENGINE_add(new_h2))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Add failed!\n");
		goto end;
		}
	display_engine_list();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(!ENGINE_remove(new_h2))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Remove failed!\n");
		goto end;
		}
	display_engine_list();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(!ENGINE_add(new_h4))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Add failed!\n");
		goto end;
		}
	display_engine_list();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(ENGINE_add(new_h3))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Add *should* have failed but didn't!\n");
		goto end;
		}
	else
		fprintf(stdout,"Add that should fail did.\n");
	ERR_clear_error();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(ENGINE_remove(new_h2))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Remove *should* have failed but didn't!\n");
		goto end;
		}
	else
		fprintf(stdout,"Remove that should fail did.\n");
	ERR_clear_error();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(!ENGINE_remove(new_h3))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Remove failed!\n");
		goto end;
		}
	display_engine_list();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(!ENGINE_remove(new_h4))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Remove failed!\n");
		goto end;
		}
	display_engine_list();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	/* Depending on whether there's any hardware support compiled
	 * in, this remove may be destined to fail. */
	ptr = ENGINE_get_first();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(ptr)
		if(!ENGINE_remove(ptr))
		{
			if(errno==ENOMEM)
    	{
	      return 1;
	    }
			fprintf(stdout,"Remove failed!i - probably no hardware "
				"support present.\n");
		}		
	if (ptr)
	{
		ENGINE_free(ptr);
		if(errno==ENOMEM)
   	{
	  return 1;
	  }
	}	
	display_engine_list();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	if(!ENGINE_add(new_h1) || !ENGINE_remove(new_h1))
		{
		if(errno==ENOMEM)
   	{
	  return 1;
	  }	
		fprintf(stdout,"Couldn't add and remove to an empty list!\n");
		goto end;
		}
	else
		fprintf(stdout,"Successfully added and removed to an empty list!\n");
	fprintf(stdout,"About to beef up the engine-type list\n");
	for(loop = 0; loop < 512; loop++)
		{
		sprintf(buf, "id%i", loop);
		id = BUF_strdup(buf);
		if(errno==ENOMEM)
   	{
	  return 1;
	  }
		sprintf(buf, "Fake engine type %i", loop);
		name = BUF_strdup(buf);
		if(errno==ENOMEM)
   	{
	  return 1;
	  }
		if(((block[loop] = ENGINE_new()) == NULL) ||
				!ENGINE_set_id(block[loop], id) ||
				!ENGINE_set_name(block[loop], name))
			{
			if(errno==ENOMEM)
     	{
	      return 1;
	    }	
			fprintf(stdout,"Couldn't create block of ENGINE structures.\n"
				"I'll probably also core-dump now, damn.\n");
			goto end;
			}
		}
	for(loop = 0; loop < 512; loop++)
		{
		if(!ENGINE_add(block[loop]))
			{
			if(errno==ENOMEM)
     	{
	     return 1;
	    } 	
			fprintf(stdout,"\nAdding stopped at %i, (%s,%s)\n",
				loop, ENGINE_get_id(block[loop]),
				ENGINE_get_name(block[loop]));
			if(errno==ENOMEM)
     	{
	     return 1;
	    } 	
						
			goto cleanup_loop;
			}
		else
			fprintf(stdout,"."); fflush(stdout);
		}
cleanup_loop:
	fprintf(stdout,"\nAbout to empty the engine-type list\n");
	while((ptr = ENGINE_get_first()) != NULL)
		{
			if(errno==ENOMEM)
   	 {
	    return 1;
	   }
		if(!ENGINE_remove(ptr))
			{
			if(errno==ENOMEM)
     	{
	      return 1;
	    }
			fprintf(stdout,"\nRemove failed!\n");
			goto end;
			}
		ENGINE_free(ptr);
		if(errno==ENOMEM)
   	{
	  return 1;
	  }
		fprintf(stdout,"."); fflush(stdout);
		}
	for(loop = 0; loop < 512; loop++)
		{
		OPENSSL_free((void *)ENGINE_get_id(block[loop]));
		if(errno==ENOMEM)
     	{
	     return 1;
	    } 	
		
		OPENSSL_free((void *)ENGINE_get_name(block[loop]));
			if(errno==ENOMEM)
     	{
	     return 1;
	    } 	
		
		}
	fprintf(stdout,"\nTests completed happily\n");
	to_return = 0;
end:
	if(to_return)
	{
		ERR_print_errors_fp(stderr);
		if(errno==ENOMEM)
   	{
	  return 1;
	  }
	}	
	if(new_h1)
	{ ENGINE_free(new_h1);
		if(errno==ENOMEM)
   	{
	  return 1;
	  }
	}  
	if(new_h2) 
	{
		ENGINE_free(new_h2);
		if(errno==ENOMEM)
   	{
	  return 1;
	  }
	}  
	if(new_h3)
	{ 
		ENGINE_free(new_h3);
		if(errno==ENOMEM)
   	{
	  return 1;
	  }
	}  
	if(new_h4)
	{
		 ENGINE_free(new_h4);
		 if(errno==ENOMEM)
   	{
	  return 1;
	  }
	}  
	for(loop = 0; loop < 512; loop++)
		if(block[loop])
		{
			ENGINE_free(block[loop]);
			if(errno==ENOMEM)
   	 {
	    return 1;
	   }
		}	
	ENGINE_cleanup();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	CRYPTO_cleanup_all_ex_data();
	if(errno==ENOMEM)
  {
	  return 1;
	}
	ERR_free_strings();
	if(errno==ENOMEM)
 	{
	  return 1;
	}
	ERR_remove_state(0);
	if(errno==ENOMEM)
  {
  		  return 1;
	}
	CRYPTO_mem_leaks_fp(stderr);
	if(errno==ENOMEM)
  {
	  return 1;
	}
	return to_return;
	}
#endif
