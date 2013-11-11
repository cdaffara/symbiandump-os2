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

 

 #include <f32file.h>
 #include <e32des16.h>
 #include "dbus-sysdeps-unix-helper.h"
 #include "dbus-list.h"
 #include "dbus-string.h"
 

#define DBUS_STANDARD_SESSION_SERVICEDIR "\\dbus1\\services"
#define DBUS_DATADIR_HARWARE ":\\data\\dbus"


 int AppendDir(DBusList **dirs)
 { 
         // Show the Input List
 RFs fs;
 User::LeaveIfError(fs.Connect());
 TFindFile findFile(fs);
 CDir *aDir = NULL;
 char *cpath = NULL;
  
 // char *cpath;
  //char *driveletter = (char *)malloc(2*sizeof(char));
  
  

 if (findFile.FindWildByDir(_L("*.service"),_L("\\data\\dbus\\dbus1\\services\\"),aDir) == KErrNone   )
{

  /*         if (!_dbus_string_append (&servicedir_path,DBUS_DATADIR))
        goto oom;
  
   if (!_dbus_string_append (&servicedir_path,DBUS_UNIX_STANDARD_SESSION_SERVICEDIR))
        goto oom;
  
    if (!_dbus_string_copy_data(&servicedir_path, &cpath))
        {
          goto oom;
        }*/
//TInt count = aDir->Count();
char *driveletter = new char[2];
//TEntry entry;
do
{

	TBuf16<256> buf16(findFile.File());
	DBusString servicedir_path;
  if (!_dbus_string_init (&servicedir_path))
    return 0;

		 cpath = (char *)(buf16.PtrZ());
		driveletter[0] =	cpath[0];
		cpath = NULL;
//	driveletter[0] = AppendDir(dirs);
        driveletter[1] = '\0';
        if (!_dbus_string_append (&servicedir_path,driveletter))
         return 0;
        
        if (!_dbus_string_append (&servicedir_path,DBUS_DATADIR_HARWARE))
         return 0;
  
   if (!_dbus_string_append (&servicedir_path,DBUS_STANDARD_SESSION_SERVICEDIR))
         return 0;
  
    if (!_dbus_string_copy_data(&servicedir_path, &cpath))
        {
          return 0;;
        }

      if (!_dbus_list_append (dirs, cpath))
        {
                     
          dbus_free (cpath);
           return 0;
        }

_dbus_string_free (&servicedir_path);
        
}while(findFile.FindWild(aDir)!= KErrNotFound);
}
    return 1;
}



char getDaemonDriveLetter()
{
	 RFs fs;
 User::LeaveIfError(fs.Connect());
 TFindFile findFile(fs);
 
 char *cpath = NULL;
  char *driveletter = new char[2];
  

 if (findFile.FindByDir(_L("dbusdaemon.exe"),_L("\\sys\\bin\\")) == KErrNone   )
{





	TBuf16<256> buf16(findFile.File());


		 cpath = (char *)(buf16.PtrZ());
		driveletter[0] =	cpath[0];
		cpath = NULL;

        driveletter[1] = '\0';
	
}
delete cpath;
return driveletter[0];
}
