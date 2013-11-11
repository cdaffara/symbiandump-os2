/* apps/openssl.c */
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
/* ====================================================================
 * Copyright (c) 1998-2001 The OpenSSL Project.  All rights reserved.
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
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#define OPENSSL_C /* tells apps.h to use complete apps_startup() */
#include "apps.h"
#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/lhash.h>
#include <openssl/conf.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <sys/stat.h>
#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif
#define USE_SOCKETS /* needed for the _O_BINARY defs in the MS world */
#include "progs.h"
#include "s_apps.h"
#include <openssl/err.h>
#ifdef SYMBIAN
#include "topenssl.h"
#endif

/* The LHASH callbacks ("hash" & "cmp") have been replaced by functions with the
 * base prototypes (we cast each variable inside the function to the required
 * type of "FUNCTION*"). This removes the necessity for macro-generated wrapper
 * functions. */

/* static unsigned long MS_CALLBACK hash(FUNCTION *a); */
static unsigned long MS_CALLBACK hash(const void *a_void);
/* static int MS_CALLBACK cmp(FUNCTION *a,FUNCTION *b); */
static int MS_CALLBACK cmp(const void *a_void,const void *b_void);
static LHASH *prog_init(void );
static int do_cmd(LHASH *prog,int argc,char *argv[]);
char *default_config_file=NULL;

/* Make sure there is only one when MONOLITH is defined */
#ifdef MONOLITH
CONF *config=NULL;
BIO *bio_err=NULL;
#endif

#ifdef SYMBIAN
FILE *fp_stdout;
FILE *fp_stderr;
FILE *fp_stdin;
#endif


static void lock_dbg_cb(int mode, int type, const char *file, int line)
	{
	static int modes[CRYPTO_NUM_LOCKS]; /* = {0, 0, ... } */
	const char *errstr = NULL;
	int rw;
	
	rw = mode & (CRYPTO_READ|CRYPTO_WRITE);
	if (!((rw == CRYPTO_READ) || (rw == CRYPTO_WRITE)))
		{
		errstr = "invalid mode";
		goto err;
		}

	if (type < 0 || type >= CRYPTO_NUM_LOCKS)
		{
		errstr = "type out of bounds";
		goto err;
		}

	if (mode & CRYPTO_LOCK)
		{
		if (modes[type])
			{
			errstr = "already locked";
			/* must not happen in a single-threaded program
			 * (would deadlock) */
			goto err;
			}

		modes[type] = rw;
		}
	else if (mode & CRYPTO_UNLOCK)
		{
		if (!modes[type])
			{
			errstr = "not locked";
			goto err;
			}
		
		if (modes[type] != rw)
			{
			errstr = (rw == CRYPTO_READ) ?
				"CRYPTO_r_unlock on write lock" :
				"CRYPTO_w_unlock on read lock";
			}

		modes[type] = 0;
		}
	else
		{
		errstr = "invalid mode";
		goto err;
		}

 err:
	if (errstr)
		{
		/* we cannot use bio_err here */
#ifndef SYMBIAN		
		fprintf(stderr, "openssl (lock_dbg_cb): %s (mode=%d, type=%d) at %s:%d\n",
			errstr, mode, type, file, line);
#else
        fprintf(fp_stderr, "openssl (lock_dbg_cb): %s (mode=%d, type=%d) at %s:%d\n",
			errstr, mode, type, file, line);
        fclose(fp_stderr)			;
#endif			
		}
	}



#ifdef SYMBIAN
void testResultXml(char *filename,int retval)
{
	char time_buf[50];
	
	char result[10];
	
	char xmlfilename[256];
		
	time_t t = time(NULL);
	
	struct tm *tm1 = localtime(&t);
	
	char *atsinitmsg 	= 	"<test-report>\n\t<test-batch>";
	
	char *atsbatchinit1	=   \
							"\n\t\t<batch-init>\
							\n\t\t\t<description></description>\
							\n\t\t\t<date>";						
							
	char *atsbatchinit2 =	"</date>\
							\n\t\t\t<factory>NA</factory>\
							\n\t\t\t<component>\
							\n\t\t\t\t<name>NA</name>\
							\n\t\t\t\t<version>NA</version>\
							\n\t\t\t</component>\
							\n\t\t</batch-init>";
							
	char *atsbatchresult=	\
							"\n\t\t<batch-result>\
							\n\t\t\t<run-time>00:00:00</run-time>\
							\n\t\t</batch-result>";
							
	char *atsclosemsg	=   \
							"\n\t</test-batch>\
							\n</test-report>\n ";

	char *atstestinit	=	"\n\t\t<test-case time-stamp=\"00:00:00\">";

	
	char *atscaseinit1	=	\
							"\n\t\t\t<case-init>\
							\n\t\t\t\t<version></version>\
							\n\t\t\t\t<id>";
												
	char *atscaseinit2 = 	"</id>\
							\n\t\t\t\t<expected-result description=\"\">0</expected-result>\
							\n\t\t\t</case-init>";
							
	char *atscaseresult1=	\
							"\n\t\t\t<case-result status=\"";
							
	char *atscaseresult2=	"\">\
							\n\t\t\t\t<actual-result>0</actual-result>\
							\n\t\t\t\t<run-time>00:00:00</run-time>\
							\n\t\t\t</case-result>";

	char *atstestclose	=	"\n\t\t</test-case>";
	
	DIR *dir;
	FILE *fp;
	
	dir = opendir("c:\\spd_logs");
	if(!dir)
		mkdir("c:\\spd_logs",0777);
	
	dir = opendir("c:\\spd_logs\\xml");
	if(!dir)
		mkdir("c:\\spd_logs\\xml",0777);
	
	// create the xml file name
	strcpy(xmlfilename,"c:/spd_logs/xml/");
	strcat(xmlfilename,filename);
	strcat(xmlfilename,".xml");
	
	strftime(time_buf,50,"%c",tm1);

	if(retval)
		strcpy(result,"FAILED");
	else
		strcpy(result,"PASSED");
	
	fp = fopen(xmlfilename,"w");
	
	if(fp)
	{
		fprintf(fp,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s",atsinitmsg,atsbatchinit1,time_buf,atsbatchinit2,atstestinit,
			atscaseinit1,filename,atscaseinit2,atscaseresult1,result,atscaseresult2,
			atstestclose,atsbatchresult,atsclosemsg);
			
		fclose(fp);	
	}
}


int openssl_init(void)
{
	
	fp_stdout = freopen(LOG_STDOUT,"w+",stdout);
	if(!fp_stdout)
	return 1;
	fp_stderr = freopen(LOG_STDERR,"w+",stderr);
	if(!fp_stderr)
	return 1;
	fp_stdin = freopen(LOG_STDIN,"w+",stdin);
	if(!fp_stdin)
	return 1;
	return 0;
}

void openssl_deinit(void)
{
    fclose(fp_stdout);
    fclose(fp_stderr);
    fclose(fp_stdin);
    fp_stdout=NULL;
    fp_stdin=NULL;
    fp_stderr=NULL;
    
}

int append(char *file1,char *file2, char *dst_file )
{
   FILE *fp1 = NULL;
   FILE *fp2 = NULL;
   FILE *fp3 = NULL;
				
   long file1_size;
   long file2_size;
   char *file1_data = NULL;
   char *file2_data = NULL;
	
   int ret;				
	
   if(!(fp1 = fopen(file1,"r+")))
   {
	 fprintf(stdout,"Faliure opening file %s.",file1);
     ret = -1;
	 goto err;
   }
					
   if(!(fp2 = fopen(file2,"r+")))
   {
	 fprintf(stdout,"Faliure opening file %s.",file2);
	 ret = -1;
	 goto err;
   }
 
   if(!(fp3= fopen(dst_file,"w+")))
   {
	 fprintf(stdout,"Faliure opening file %s.",dst_file);
	 ret = -1;
	 goto err;
   }

   if((ret = fseek(fp1, 0, SEEK_END)) < 0)
   {
   	  ret = -1;
	  goto err;
   }
   
			
   if((ret = fseek(fp2, 0, SEEK_END)) < 0)
   {
   	 ret = -1;
	 goto err;
   }
   
			
   if((file1_size = ftell(fp1)) < 0)
   {	
   	 ret = -1;
	 goto err;
   }
	
   if((file2_size = ftell(fp2)) < 0)
   {
     ret = -1;
	 goto err;
   }
									
   if((ret = fseek(fp1, 0, SEEK_SET)) < 0)
   {
   	 ret = -1;
	 goto err;
   }
				
   if((ret = fseek(fp2, 0, SEEK_SET)) < 0)
   {
   	 ret = -1;
	 goto err;
   }
   				
   file1_data = (char *)malloc(file1_size);
   file2_data = (char *)malloc(file2_size);
				
   if(!file1_data||!file2_data)
   {
   	 ret = -1;
	 goto err;
   }
   memset(file1_data,0,file1_size);
   
   memset(file2_data,0,file2_size);
				
   ret = fread(file1_data,sizeof(char),file1_size,fp1);
   if(ret < file1_size)
   {
	fprintf(stdout,"Failed reading file %s.", file1);
	ret = -1;
    goto err;
   }

  ret = fread(file2_data,sizeof(char),file2_size,fp2);
  if(ret < file2_size)
  {
	fprintf(stdout,"Failed reading file %s.",file2);
	ret = -1;
	goto err;
  }
  
  ret = fwrite(file1_data,sizeof(char),file1_size,fp3);
  if(ret < file1_size)
  {
	fprintf(stdout,"Failed wrting from %s file to file %s.",file1,dst_file);
	ret = -1;
	goto err;
  }
  ret = fwrite(file2_data,sizeof(char),file2_size,fp3);
  if(ret < file2_size)
  {
	fprintf(stdout,"Failed wrting from %s file to file %s.",file2,dst_file);
	ret = -1;
	goto err;
  }
  ret=0;

err:  if(file1_data)
  free(file1_data);
  if(file2_data)
  free(file2_data);				
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  return ret;
 				
}
void print_logs(int argc, char *argv[])
{
	int i=0;
	for(i=0;i<argc-3;i++)
    {
    	fprintf(stdout,"%s ",argv[i+3]);
    }
   fprintf(stdout,"\n");
	
}

char** create_params(char *ip_str,int *argc)
{
   char *paramets[50];
   char **cmd_line=NULL;
   char *str=NULL,*ptr=NULL;
   int len=0,cnt=0,i=0;
  
  
   
   str=ip_str;
    
   while(str)
   {
      ptr=strchr(str,' ');
      if(ptr)
      {
        len = ptr-str;
        paramets[cnt]=(char *)malloc(sizeof(char)*len+1);
        if(paramets[cnt])
        {
          strncpy(paramets[cnt],str,len);
      	  paramets[cnt][len]='\0';
      	}
      	cnt++;
      	str+= len + 1;
      }
      else
      {
        len = strlen(str);
        paramets[cnt]=(char *)malloc(sizeof(char)*len+1);
        if(paramets[cnt])
       	strcpy(paramets[cnt],str);
        cnt++;
      	break;
      }
   	
   }
   
   if(cnt)
   cmd_line=(char **)malloc(cnt*sizeof(char *));
   for(i=0;i<cnt;i++) 
   cmd_line[i]=paramets[i];
   *argc=cnt;
   return cmd_line;
 
 	
}



int compare_files(const char *file1, const char *file2)
{
			
   			FILE *fp1 = NULL;
			FILE *fp2 = NULL;
				
				long file1_size;
				long file2_size;
				
				char *file1_data = NULL;
				char *file2_data = NULL;
				
				int ret;				
				int argc=0;
   			char** argv = NULL;
   		
   			
				if(!(fp1 = fopen(file1,"r+"))){
					fprintf(stdout,"Faliure opening file %s.",file1);
					ret = -1;
					goto err;
				}
					
				if(!(fp2 = fopen(file2,"r+"))){
					fprintf(stdout,"Faliure opening file %s.",file2);
					ret = -1;
					goto err;
				}
				
				if((ret = fseek(fp1, 0, SEEK_END)) < 0)
					goto err;
				
				if((ret = fseek(fp2, 0, SEEK_END)) < 0)
					goto err;
								
 				if((file1_size = ftell(fp1)) < 0)
				{	
					ret = file1_size;
					goto err;
				}
				if((file2_size = ftell(fp1)) < 0)
					{
						ret = file2_size;
						goto err;
					}
									
				if(file1_size != file2_size)
				{
					fprintf(stdout,"File size of  %s is not same as that of %s.",file1,file2);
					ret = -1;
					goto err;
				}
							
                if((ret = fseek(fp1, 0, SEEK_SET)) < 0)
					goto err;
				
				if((ret = fseek(fp2, 0, SEEK_SET)) < 0)
					goto err;							
				
				file1_data = (char *)malloc(file1_size);
				file2_data = (char *)malloc(file2_size);
				
				memset(file1_data,0,file1_size);
				memset(file2_data,0,file2_size);
				
				ret = fread(file1_data,sizeof(char),file1_size,fp1);
				if(ret < file1_size)
				{
				    ret = ferror(fp1);
					fprintf(stdout,"Failed reading file %s.", file1);
					ret = -1;
					goto err1;
				}
				
				ret = fread(file2_data,sizeof(char),file2_size,fp2);
				if(ret < file2_size)
				{
					fprintf(stdout,"Failed reading file %s.",file2);
					ret = -1;
					goto err2;
				}
				
				if(memcmp(file1_data,file2_data,file1_size))
				{
						
				   fprintf(stdout,"File file1 is not similar to file2.");
				   ret = -1;
				   goto err2;
				}
				else
				ret=0; //file are smae
		
err2:		
				fclose(fp2);
err1:
				fclose(fp1);
err:

				if(file1_data)
					free(file1_data);
					
				if(file2_data)
					free(file2_data);
					
				return ret;
}

void delete_file(char *file)
{
	unlink(file);
}

void delete_params(char ** cmd_line,int argc)
{
	int i;
	for(i=0;i<argc;i++) 
	if(cmd_line[i])	free(cmd_line[i]);
	if(cmd_line)
	free(cmd_line);
	cmd_line=NULL;
}

void strip(char *str)
{
	int len=0;
	char *ptr=NULL;
	char *ptr1=NULL;
	len=strlen(str);
	ptr=strchr(str,'\r');
	ptr1=strchr(str,'\n');
	if((ptr) || (ptr1))
	{
		if(ptr)
		*ptr='\0'; //strip \r and \n	
		else
		*ptr1='\0';
	}
	return;
}


int main(int argc_m, char *argv_m[])
{
	
   int ret,argc=0;
   char argstr[200];
   int total_cases,cur_case=0,fail=0;
   char **argv=NULL;
   char openssltestnum[20];
   FILE *arg_fp;
   
   	
  if(openssl_init())
  {
    return 1;	
  }
    
    if(argc_m > 1)
    	arg_fp = fopen(argv_m[1],"r+");
    else
    	return 1; // filename needed as argument.
    
  
    if(!arg_fp)
    {
      return 1;	
    }
  
    while(!feof(arg_fp)&&!fail)
    {
  	  memset(argstr,0,200);
      fgets(argstr, 200, arg_fp);
      strip(argstr);
      argv = create_params(argstr,&argc);
	  strcpy(openssltestnum,argv[1]);
	
	  if(!strcmp(argv[2],"test_log"))
      {
        print_logs(argc,argv);
      }
      else if(!strcmp(argv[2],"compare"))
      {
        ret = compare_files(argv[3],argv[4]);	
        if(ret)
        {
        testResultXml(openssltestnum, ret);
        fail=1;
        }
        
      }
      else if(!strcmp(argv[2],"append"))
      {
        ret = append(argv[3],argv[4],argv[5]);	
        if(ret)
        {
         testResultXml(openssltestnum, ret);
         fail=1;
        }
        
      }
       else if(!strcmp(argv[2],"delete"))
      {
        delete_file(argv[3]);	
    
      }
      else
      {
        ret = openssl_main(argc-2,argv+2);
        if(ret)
        {
        testResultXml(openssltestnum, ret);
        fail=1;	
        }
        
      }   
    
     //delete_params(argv,argc);  
   }//while
   
   if(!fail&&!ret)
   testResultXml(openssltestnum, fail);
   fclose(arg_fp);
 
  
 openssl_deinit(); 
 return ret;
}

#endif  /* IF_DEF SYMBIAN */
#ifndef SYMBIAN
int main(int Argc, char *Argv[])
#else
int openssl_main(int Argc, char *Argv[])
#endif
	{
	ARGS arg;
#define PROG_NAME_SIZE	39
	char pname[PROG_NAME_SIZE+1];
	FUNCTION f,*fp;
	MS_STATIC const char *prompt;
	MS_STATIC char buf[1024];
	char *to_free=NULL;
	int n,i,ret=0;
	int argc;
	char **argv,*p;
	LHASH *prog=NULL;
	long errline;
 
	arg.data=NULL;
	arg.count=0;
	
	if (bio_err == NULL)
		if ((bio_err=BIO_new(BIO_s_file())) != NULL)
#ifdef SYMBIAN		
        BIO_set_fp(bio_err,fp_stderr,BIO_NOCLOSE|BIO_FP_TEXT);
#else
        BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
#endif			
	if (getenv("OPENSSL_DEBUG_MEMORY") != NULL) /* if not defined, use compiled-in library defaults */
		{
		if (!(0 == strcmp(getenv("OPENSSL_DEBUG_MEMORY"), "off")))
			{
			CRYPTO_malloc_debug_init();
			CRYPTO_set_mem_debug_options(V_CRYPTO_MDEBUG_ALL);
			}
		else
			{
			/* OPENSSL_DEBUG_MEMORY=off */
			CRYPTO_set_mem_debug_functions(0, 0, 0, 0, 0);
			}
		}
	CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);

#if 0
	if (getenv("OPENSSL_DEBUG_LOCKING") != NULL)
#endif
		{
		CRYPTO_set_locking_callback(lock_dbg_cb);
		}

	apps_startup();

	/* Lets load up our environment a little */
	p=getenv("OPENSSL_CONF");
	if (p == NULL)
		p=getenv("SSLEAY_CONF");
	if (p == NULL)
		p=to_free=make_config_name();

	default_config_file=p;

	config=NCONF_new(NULL);
	i=NCONF_load(config,p,&errline);
	if (i == 0)
		{
		NCONF_free(config);
		config = NULL;
		ERR_clear_error();
		}

	prog=prog_init();

	/* first check the program name */
	program_name(Argv[0],pname,sizeof pname);

	f.name=pname;
	fp=(FUNCTION *)lh_retrieve(prog,&f);
	if (fp != NULL)
		{
		Argv[0]=pname;
		ret=fp->func(Argc,Argv);
		goto end;
		}

	/* ok, now check that there are not arguments, if there are,
	 * run with them, shifting the ssleay off the front */
	if (Argc != 1)
		{
		Argc--;
		Argv++;
		ret=do_cmd(prog,Argc,Argv);
		if (ret < 0) ret=0;
		goto end;
		}

	/* ok, lets enter the old 'OpenSSL>' mode */
	
	for (;;)
		{
		ret=0;
		p=buf;
		n=sizeof buf;
		i=0;
		for (;;)
			{
			p[0]='\0';
			if (i++)
				prompt=">";
			else	prompt="OpenSSL> ";
#ifndef SYMBIAN			
			fputs(prompt,stdout);
			fflush(stdout);
			fgets(p,n,stdin);
#else
            fputs(prompt,stdout);
			fflush(stdout);
			fgets(p,n,stdin);

#endif			
			if (p[0] == '\0') goto end;
			i=strlen(p);
			if (i <= 1) break;
			if (p[i-2] != '\\') break;
			i-=2;
			p+=i;
			n-=i;
			}
		if (!chopup_args(&arg,buf,&argc,&argv)) break;

		ret=do_cmd(prog,argc,argv);
		if (ret < 0)
			{
			ret=0;
			goto end;
			}
		if (ret != 0)
			BIO_printf(bio_err,"error in %s\n",argv[0]);
		(void)BIO_flush(bio_err);
		}
	BIO_printf(bio_err,"bad exit\n");
	ret=1;
end:
	if (to_free)
		OPENSSL_free(to_free);
	if (config != NULL)
		{
		NCONF_free(config);
		config=NULL;
		}
	if (prog != NULL) lh_free(prog);
	if (arg.data != NULL) OPENSSL_free(arg.data);

	apps_shutdown();

	CRYPTO_mem_leaks(bio_err);
	if (bio_err != NULL)
		{
		BIO_free(bio_err);
		bio_err=NULL;
		}
	
	return ret;
//	OPENSSL_EXIT(ret);
	}

#define LIST_STANDARD_COMMANDS "list-standard-commands"
#define LIST_MESSAGE_DIGEST_COMMANDS "list-message-digest-commands"
#define LIST_CIPHER_COMMANDS "list-cipher-commands"

static int do_cmd(LHASH *prog, int argc, char *argv[])
	{
	FUNCTION f,*fp;
	int i,ret=1,tp,nl;

	if ((argc <= 0) || (argv[0] == NULL))
		{ ret=0; goto end; }
	f.name=argv[0];
	fp=(FUNCTION *)lh_retrieve(prog,&f);
	if (fp != NULL)
		{
		ret=fp->func(argc,argv);
		}
	else if ((strncmp(argv[0],"no-",3)) == 0)
		{

		BIO *bio_stdout = BIO_new_fp(stdout,BIO_NOCLOSE);
#ifdef OPENSSL_SYS_VMS
		{
		BIO *tmpbio = BIO_new(BIO_f_linebuffer());
		bio_stdout = BIO_push(tmpbio, bio_stdout);
		}
#endif
		f.name=argv[0]+3;
		ret = (lh_retrieve(prog,&f) != NULL);
		if (!ret)
			BIO_printf(bio_stdout, "%s\n", argv[0]);
		else
			BIO_printf(bio_stdout, "%s\n", argv[0]+3);
		BIO_free_all(bio_stdout);
		goto end;
		}
	else if ((strcmp(argv[0],"quit") == 0) ||
		(strcmp(argv[0],"q") == 0) ||
		(strcmp(argv[0],"exit") == 0) ||
		(strcmp(argv[0],"bye") == 0))
		{
		ret= -1;
		goto end;
		}
	else if ((strcmp(argv[0],LIST_STANDARD_COMMANDS) == 0) ||
		(strcmp(argv[0],LIST_MESSAGE_DIGEST_COMMANDS) == 0) ||
		(strcmp(argv[0],LIST_CIPHER_COMMANDS) == 0))
		{
		int list_type;
		BIO *bio_stdout;

		if (strcmp(argv[0],LIST_STANDARD_COMMANDS) == 0)
			list_type = FUNC_TYPE_GENERAL;
		else if (strcmp(argv[0],LIST_MESSAGE_DIGEST_COMMANDS) == 0)
			list_type = FUNC_TYPE_MD;
		else /* strcmp(argv[0],LIST_CIPHER_COMMANDS) == 0 */
			list_type = FUNC_TYPE_CIPHER;

		bio_stdout = BIO_new_fp(stdout,BIO_NOCLOSE);
#ifdef OPENSSL_SYS_VMS
		{
		BIO *tmpbio = BIO_new(BIO_f_linebuffer());
		bio_stdout = BIO_push(tmpbio, bio_stdout);
		}
#endif
		
		for (fp=functions; fp->name != NULL; fp++)
			if (fp->type == list_type)
				BIO_printf(bio_stdout, "%s\n", fp->name);
		BIO_free_all(bio_stdout);
		ret=0;
		goto end;
		}
	else
		{
		BIO_printf(bio_err,"openssl:Error: '%s' is an invalid command.\n",
			argv[0]);
		BIO_printf(bio_err, "\nStandard commands");
		i=0;
		tp=0;
		for (fp=functions; fp->name != NULL; fp++)
			{
			nl=0;
			if (((i++) % 5) == 0)
				{
				BIO_printf(bio_err,"\n");
				nl=1;
				}
			if (fp->type != tp)
				{
				tp=fp->type;
				if (!nl) BIO_printf(bio_err,"\n");
				if (tp == FUNC_TYPE_MD)
					{
					i=1;
					BIO_printf(bio_err,
						"\nMessage Digest commands (see the `dgst' command for more details)\n");
					}
				else if (tp == FUNC_TYPE_CIPHER)
					{
					i=1;
					BIO_printf(bio_err,"\nCipher commands (see the `enc' command for more details)\n");
					}
				}
			BIO_printf(bio_err,"%-15s",fp->name);
			}
		BIO_printf(bio_err,"\n\n");
		ret=0;
		}
end:
	return(ret);
	}

static int SortFnByName(const void *_f1,const void *_f2)
    {
    const FUNCTION *f1=_f1;
    const FUNCTION *f2=_f2;

    if(f1->type != f2->type)
	return f1->type-f2->type;
    return strcmp(f1->name,f2->name);
    }

static LHASH *prog_init(void)
	{
	LHASH *ret;
	FUNCTION *f;
	size_t i;

	/* Purely so it looks nice when the user hits ? */
	for(i=0,f=functions ; f->name != NULL ; ++f,++i)
	    ;
	qsort(functions,i,sizeof *functions,SortFnByName);

	if ((ret=lh_new(hash, cmp)) == NULL)
		return(NULL);

	for (f=functions; f->name != NULL; f++)
		lh_insert(ret,f);
	return(ret);
	}

/* static int MS_CALLBACK cmp(FUNCTION *a, FUNCTION *b) */
static int MS_CALLBACK cmp(const void *a_void, const void *b_void)
	{
	return(strncmp(((const FUNCTION *)a_void)->name,
			((const FUNCTION *)b_void)->name,8));
	}

/* static unsigned long MS_CALLBACK hash(FUNCTION *a) */
static unsigned long MS_CALLBACK hash(const void *a_void)
	{
	return(lh_strhash(((const FUNCTION *)a_void)->name));
	}
