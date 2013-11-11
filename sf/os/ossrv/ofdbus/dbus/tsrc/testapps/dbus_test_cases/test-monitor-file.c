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



#include "test-utils.h"
#include <unistd.h>
int write_to_configfile()
{
	FILE *fp;
	char buff[10];
	fp = fopen("c:\\data\\dbus\\system.conf", "r+");
	if(fp)
	{
	    fread(buff,1,10,fp);
		fwrite(buff,1,10,fp);
		fclose(fp);
		return 0;
	}
	return 1;
} 

int reset_case_passed()
{
   FILE *fp; 
   fp = fopen("C:\\data\\dbus\\test_result.txt","w+");
   if(fp)
      {
        //we simply create empty file
      	fclose(fp);
      }
}
int check_case_passed()
{
   FILE *fp; 
   char buf[50];
   int n;
   fp = fopen("C:\\data\\dbus\\test_result.txt","r");
   if(fp)
      {
        n = fread(buf,1,30,fp);
        if(n>0)
        {
        //case passed
        fclose(fp);
        return 0;	
        }
      	
      }
   return 1;   
}
int
main ()
{
  DBusError error;
  DBusConnection *connection;
  
 
  dbus_error_init (&error);
  connection = dbus_bus_get (DBUS_BUS_SESSION, &error);
  if (connection == NULL)
    {
      fprintf (stderr, "*** Failed to open connection to system bus: %s\n",
               error.message);
      dbus_error_free (&error);
      getchar();
      return 1;
    }
  if(!reset_case_passed())
  {
  write_to_configfile();
  	
  }
  sleep(10);
  return check_case_passed();
  
}
