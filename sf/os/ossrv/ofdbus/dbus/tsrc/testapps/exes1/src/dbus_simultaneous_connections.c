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


#include<stdio.h> 
#include <dbus/dbus.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define LOG_FILE "c:\\logs\\dbus_simultaneous_connections_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

int make_fifo(char* fifopath)
{
	int err =0;
	err = mkfifo (fifopath, 0666);
	   if(err != 0)
	   {
	       // probably file already exists, delete the file
	       unlink(fifopath); 
	       // try once more..
	       err = mkfifo (fifopath, 0666);
	       if(err != 0)
	       {
	            return 1;
	       }
	   }
	   return 0;
}

int read_fifo(char* fifopath, char* str)
{
	int fd = open(fifopath, O_RDONLY);
	int err;
	char buf[180];
	
	if(fd > 0)
	{
		err = read (fd, buf, 80);
		close(fd);
	}
	else
		{
		return 1;
		}
	unlink(fifopath);
	if(strcmp(buf, "done"))
	{
		strcpy(str, buf);
		return 1;
	}
	
	return 0;
}

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_simultaneous_connections");
	close_log_file();
}

int main()
{

	
	FILE* fp[3];
	int cnt;
	char buf[180];
	char res[80];
	
#if defined(__WINSCW__) || defined(__WINS__)
	char* exe_names[] = { 	"z:\\sys\\bin\\get_connection_1.exe",
							"z:\\sys\\bin\\get_connection_2.exe",
							"z:\\sys\\bin\\get_connection_3.exe"
						};
#else
	char* exe_names[] = { 	"c:\\sys\\bin\\get_connection_1.exe",
							"c:\\sys\\bin\\get_connection_2.exe",
							"c:\\sys\\bin\\get_connection_3.exe"
						};
#endif
	
	char* fifopath[3] = {	"C:\\mkfifo001.file",
								"C:\\mkfifo002.file",
								"C:\\mkfifo003.file"};
	for(cnt=0; cnt<3; cnt++)
	{
		if(make_fifo(fifopath[cnt]))
			{
			std_log(LOG_FILENAME_LINE,"Failed to open FIFO for %d count", cnt);
			create_xml(1);
			return 1;
			}
	}
	

	
	
	for(cnt=0; cnt<3; cnt++)
		{
			fp[cnt] = popen(exe_names[cnt], "r");
			if(!fp[cnt])
				{
				std_log(LOG_FILENAME_LINE,"Failed to open %d th EXE", cnt);
				create_xml(1);
				return 1;
				}
		}
	
	for(cnt=0; cnt<3; cnt++)
	{
		if(read_fifo(fifopath[cnt], res))
			{
			std_log(LOG_FILENAME_LINE,"Failed to read FIFO for %d exe and %s", cnt, res);
			create_xml(1);
			return 1;
			}
	}	
	
	std_log(LOG_FILENAME_LINE,"Test Successful");
	create_xml(0);
	return 0;
}
