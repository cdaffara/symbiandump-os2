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



#include <string.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include<dbus_glib3_stringsend-bindings.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_TRY 10
#define MAX_THREAD 75

int handle_error(GError* error)
	{
	g_printerr ("Failed to open connection to bus: %s\n",
            error->message);
	g_error_free (error);
	return 1;
	}
	
int main(int argc, char* argv[])
{
	
	 
		DBusGConnection *connection;
		GError *error;
		DBusGProxy *proxy;
		FILE *File;
		char buffer_F[]= "FAIL to open the file ";
		
		gchar* 		st = "StringPassing";
		gchar*      nxt_loop = "entering the next loop for debug";
		gint		 ret = 1;
		int i;
		
		g_type_init ();
		
		error = NULL;
		
		connection = dbus_g_bus_get (DBUS_BUS_SESSION,&error);
		
		if (connection == NULL)
		{
		  g_printerr ("Failed to open connection to bus: %s\n",error->message);
		  g_error_free (error);
		  exit (1);
		}
		
		proxy = dbus_g_proxy_new_for_name (connection,
		                                 "com.example.DBusGlibObject2",
		                                 "/com/example/DBusGlibObject2",
		                                 "com.example.DBusGlibObject2");		  
		  
		  if (File==NULL)
		  {
		      exit(0);
		  }
		for( i = 0;i<20000;i++)
	{
		if(!com_example_DBusGlibObject2_send_message0(proxy, st, &ret, &error))
			{
					File = fopen ("c:\\simple_message.txt","a+");
					
					fprintf (File, "%s\n",error->message); 
					
					fclose (File);
						return 1;
			}
		
		if(ret!=9090)
		{
			return 1;
		}
		File = fopen ("c:\\simple_message.txt","a+");
		
		fprintf (File, "%d\n",i); 
	
	fclose (File);
	if(i == 8187)
		break;
		
	}


//code added for debug
			for(;i<20000;i++)
			{
			
			File = fopen ("c:\\simple_message.txt","a+");
			
			fprintf (File, "%s\n",nxt_loop); 
			
			fclose (File);
			if(!com_example_DBusGlibObject2_send_message0(proxy, st, &ret, &error))
				{
				File = fopen ("c:\\simple_message.txt","a+");
				
				fprintf (File, "%s\n",error->message); 
				
				fclose (File);
					return 1;
				}
			
			if(ret!=9090)
			{
				return 1;
			}
			File = fopen ("c:\\simple_message.txt","a+");
			
			fprintf (File, "%d\n",i); 
			
			fclose (File);
			
			}
// code added 4 debug
		File = fopen ("c:\\simple_message.txt","a+");
		dbus_g_connection_unref(connection);
		fprintf(File,"SUCCESS");
		
		fclose (File);
	
		return 0;
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	