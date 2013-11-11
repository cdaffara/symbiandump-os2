/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/




#ifndef _STD_LOG_FILE_H__
#define _STD_LOG_FILE_H__

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#ifdef __SYMBIAN32__
//#define LOG_FILE "c:\\logs\\std_test_log.txt"
#define LOG_DIR "c:\\logs\\"
#define LOG_FILE_EXT "xml"
int assert_failed = 0;
#else
#define LOG_DIR ""
#define LOG_FILE_EXT "xml"
#define LOG_FILE "std_test_log.txt"
int assert_failed = 0;
#endif
FILE *fp;

int gnutest = 1;

# define VERIFY(fn) gnutest &= (fn)


void std_log(const char *filename,const int lineno,const char* aformat,...)
{
	va_list va;
	if(fp==NULL)
	{
		fp = fopen(LOG_FILE,"a");
	}
	
	va_start(va,aformat);    
    {
		fprintf(fp,"%s - [%d] : ",filename,lineno);
		vfprintf(fp,aformat,va);
		fprintf(fp,"\n");
	}
	va_end(va);
}


void init_log_file()
{
	if(fp == NULL)
	{
		fp = fopen(LOG_FILE, "a");
	}
}

void close_log_file()
{
   fclose(fp);
}

// This function is used to generate the xml file used bt ATS
void testResultXml(char *filename)
{
	char time_buf[50];

	char result[10];

	char xmlfilename[256];

	time_t t = time(NULL);

	struct tm *tm1 = localtime(&t);

	char *atsinitmsg        =   "<test-report>\n\t<test-batch>";

	char *atsbatchinit1     =   \
								"\n\t\t<batch-init>\
								\n\t\t\t<description></description>\
								\n\t\t\t<date>";
	char *atsbatchinit2 =   "</date>\
							 \n\t\t\t<factory>NA</factory>\
							 \n\t\t\t<component>\
							 \n\t\t\t\t<name>NA</name>\
							 \n\t\t\t\t<version>NA</version>\
							 \n\t\t\t</component>\
							 \n\t\t</batch-init>";

	char *atsbatchresult=   \
							"\n\t\t<batch-result>\
							\n\t\t\t<run-time>00:00:00</run-time>\
							\n\t\t</batch-result>";

	char *atsclosemsg       =   \
								"\n\t</test-batch>\
								\n</test-report>\n ";

	char *atstestinit       =       "\n\t\t<test-case time-stamp=\"00:00:00\">";


	char *atscaseinit1      =       \
									"\n\t\t\t<case-init>\
									\n\t\t\t\t<version></version>\
									\n\t\t\t\t<id>";

	char *atscaseinit2 =    "</id>\
							 \n\t\t\t\t<expected-result description=\"\">0</expected-result>\
							 \n\t\t\t</case-init>";

	char *atscaseresult1=   \
							"\n\t\t\t<case-result status=\"";

	char *atscaseresult2=   "\">\
							 \n\t\t\t\t<actual-result>0</actual-result>\
							 \n\t\t\t\t<run-time>00:00:00</run-time>\
							 \n\t\t\t</case-result>";

	char *atstestclose      =       "\n\t\t</test-case>";

	// create the xml file name
	FILE *fp_result;
	sprintf(xmlfilename, "%s%s.%s", LOG_DIR, filename, LOG_FILE_EXT);
	strftime(time_buf,50,"%c",tm1);

	if(assert_failed )
		strcpy(result,"FAILED");
	else
		strcpy(result,"PASSED");

	fp_result = fopen(xmlfilename,"w");

	if(fp_result)
	{
		fprintf(fp_result,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s",atsinitmsg,atsbatchinit1,time_buf,atsbatchinit2,atstestinit,
				atscaseinit1,filename,atscaseinit2,atscaseresult1,result,atscaseresult2,
				atstestclose,atsbatchresult,atsclosemsg);

		fclose(fp_result);
	}
}

#endif

