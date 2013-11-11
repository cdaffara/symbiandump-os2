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




#ifndef __G_LOG_MRT2_GLIB2_TEST
#define __G_LOG_MRT2_GLIB2_TEST
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

gboolean assert_failed = FALSE;

#undef g_assert
#define g_assert(expr)			G_STMT_START{	if (expr) { } else	 {g_log (G_LOG_DOMAIN,G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, "file %s: line %d: assertion failed: (%s)",    __FILE__,	 __LINE__,	 #expr);assert_failed  = TRUE;			}}G_STMT_END

	
void mrtLogHandler(const gchar* log_domain, GLogLevelFlags log_level,
					const gchar* message, gpointer user_data)
{
	FILE *fp;
	fp = fopen("c:\\logtests.txt","a");
	
	if(fp)
	{
		fprintf(fp,message);
		fprintf(fp,"\n");
		fclose(fp);
	}
}

void mrtPrintHandler(const gchar *message)
{
	FILE *fp;
	fp = fopen("c:\\logtests.txt","a");
	
	if(fp)
	{
		fprintf(fp,message);
		fprintf(fp,"\n");
		fclose(fp);
	}
}


// This function is used to generate the xml file used bt ATS
void testResultXml(char *filename)
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
	
	/* Check and see if spd_logs/xml is existent or not. If not present create it */
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
	
	if(assert_failed )
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
	else
	{
		g_assert(FALSE && "Failed to create the xml file");
	}
}

#endif
