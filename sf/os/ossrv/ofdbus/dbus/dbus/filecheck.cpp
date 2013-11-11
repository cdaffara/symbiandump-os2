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
#include <fcntl.h>
#include <errno.h>
#include "dbus-bus.h"
#include <dbus/config.h>
#include <unistd.h> 
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include "dbus-internals.h"
#include <string.h>
#include <dbus-userdb.h>
#include <dbus-string.h>
#include <sys/stat.h>

#define SEM_FILE_PATH "c:\\data\\dbus"
extern "C"
{
int check_lock()
{
   		 	int fd;
   		 	TBuf8<30>buf;
   		 	buf.Copy((const TUint8 *)DBUS_LOCK_FILE);	
   			TBuf16<30>buf16;
   			buf16.Copy(buf);	
    		if ((fd = open(DBUS_LOCK_FILE, O_EXCL | O_CREAT )) == -1) 
    		{
   
    			if(errno==EEXIST)
    			{
    				RFs rf;
    				User::LeaveIfError(rf.Connect());
    				RFile fds;	
    				User::LeaveIfError(fds.Open(rf,buf16,EFileShareAny));			
    				if(fds.Lock(0,10) == KErrNone)				//file exists and not locked
    				{
    					_dbus_verbose ("c:\\data\\dbus\\dbus_lock file exist and is lock-free - dbusdaemon not running\n");
						fds.UnLock(0,10);
						fds.Close();			
    					return 1;
    				}
    				else 										//file exists and is locked 
    				{
    					_dbus_verbose ("c:\\data\\dbus\\dbus_lock file exist and is locked - dbusdaemon running\n");
    					fds.Close();
    					return 0;
    				}
    			}
    			else                                             //file is created 
    			{
                    _dbus_verbose ("c:\\data\\dbus\\dbus_lock file doesnt exist and is lockfree - dbusdaemon not running\n");
    			    close(fd);  
    			    return 1;           
    			}
    			
    		}
    	
    		else												//file is created 
    		{
	    			_dbus_verbose ("c:\\data\\dbus\\dbus_lock file doesnt exist and is lockfree - dbusdaemon not running\n");
	    			close(fd);  
	    			return 1;			
    		}

}

int _dbus_semaphore_init()
{

	union semun sem_val;
	int sem_set_id,perm;
	int proj_id = 100;
    key_t fkey;
    struct stat sb;
    if(stat (SEM_FILE_PATH, &sb)<0)
    {
    			_dbus_verbose ("c:\\data\\dbus directory doesnt exist\n");	
    			if(mkdir(SEM_FILE_PATH , 0666) < 0 )  
  				{
      				_dbus_verbose ("failed to create c:\\data\\dbus directory\n");
      				return -1;
  				}
    }
    
    if ((fkey = ftok(SEM_FILE_PATH, proj_id)) == -1) {
        _dbus_verbose ("ftok() failed with error : %d\n",errno);
    	return FALSE;
    }


	perm = SEM_R | SEM_A | SEM_A>>6 | SEM_R>>6;


	if(((sem_set_id = semget(fkey, 1,  perm)))== -1) {
		if((sem_set_id = semget(fkey, 1, IPC_CREAT | perm))==-1)
		{
		    _dbus_verbose ("Failed to create the semaphore with error : %d\n",errno);
			return FALSE;
		}
	}
	else
		return sem_set_id;
	
    _dbus_verbose ("semaphore created successfully with key : %d\n",fkey);
	
	sem_val.array = NULL;
    sem_val.buf = NULL;
    sem_val.val = 1;
    
    if(semctl(sem_set_id, 0, SETVAL, sem_val) == -1) {
       _dbus_verbose ("Failed to initialize the semaphore with error : %d\n",errno);
      
       if(semctl(sem_set_id, 1, IPC_RMID) == -1) {
       		_dbus_verbose ("Failed to delete the semaphore with error : %d\n",errno);
       }
	   return FALSE;	
    }
    
    _dbus_verbose ("semaphore value successfully initialized to 1\n");
	
	return sem_set_id;
}

int _dbus_semaphore_dec()
{
	
	struct sembuf sem_op;
    /*
     * Wait on the semaphore till the value is non-negative.
     */
    int sem_set_id;
    
    sem_set_id = _dbus_semaphore_init(); 
     
    if(sem_set_id == FALSE)
    	 return FALSE;
    
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    if( semop(sem_set_id, &sem_op, 1) <0)
    {
    	_dbus_verbose ("Failed to decrement the semaphore with error : %d\n",errno);
    	return FALSE;
    }

	_dbus_verbose ("semaphore value decremented by 1\n");
	return TRUE;
}


int _dbus_semaphore_inc()
{

	struct sembuf sem_op;
	int sem_set_id;
	
	sem_set_id = _dbus_semaphore_init(); 
	sem_op.sem_num = 0;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    if(semop(sem_set_id, &sem_op, 1)<0)
    {
    	_dbus_verbose ("Failed to increment the semaphore with error : %d\n",errno);
    	return FALSE;
    }
    
	_dbus_verbose ("semaphore value incremented by 1\n");
	return TRUE;
}
int dbus_create_private_directory(const char* dir)
{
	RFs fs;
	
	if(mkdir(dir,0666) < 0)
		return FALSE;
	
	return TRUE;;
}
int _dbus_file_open_and_write(unsigned const char* sid_directory)
{
	TBuf8<100>Filename8 = _L8("C:\\private\\");
	struct stat sb;
	Filename8.Append(sid_directory,strlen((const char*)sid_directory));
    if(stat ((const char*)Filename8.PtrZ(), &sb)<0)
    {
    			_dbus_verbose ("c:\\private\\<SID> directory doesnt exist\n");	
    			if(!dbus_create_private_directory((const char*)Filename8.PtrZ()))
						return FALSE;
    }
	Filename8.Append(_L8("\\foobar_"));
	Filename8.Append(sid_directory,strlen((const char*)sid_directory));
	
	TBuf16<100>Filename16;
	Filename16.Copy(Filename8);
	
	RFs fs;
	if(fs.Connect()!= KErrNone)
	{
		return FALSE;
	}
	
	RFile file;
	if(file.Replace(fs,Filename16,EFileWrite) != KErrNone)
	{
		fs.Close();
		return FALSE;
	}
	if(file.SetAtt(KEntryAttHidden,0) != KErrNone)
	{
		file.Close();
		fs.Close();
		return FALSE;
	}
	if((file.Flush())!=KErrNone) 
	{
		file.Close();
		fs.Close();
		return FALSE;
	}
	
	file.Close();
	fs.Close();
	
	return TRUE;
}

int _dbus_file_open_for_write(const unsigned char* data)
{
	
  	const unsigned char* sid_directory;
  	const DBusString *homedir;
  	dbus_bool_t retval;
	DBusString dirname;

 	retval = FALSE;
 	 
 	TBuf8<100>Filename8;
 	TBuf16<50>Filename16;
 	RFs fs;
 	RFile file;
 	TBuf8<100>pid_data(data);
 	
  	if(!_dbus_homedir_from_current_process(&homedir))		      //getting the private or homedir directory of the process
  	{
  	 goto out_0;	
  	}
  
 	//*(char*)(homedir->dummy1) = 'C'; 
  	if(!_dbus_string_init(&dirname))
  		return FALSE;
  	
  	_dbus_string_copy(homedir,0,&dirname,0);
  	*(char*)(dirname.dummy1) = 'C'; 
  	
  	//sid_directory = (const unsigned char*)_dbus_string_get_const_data(homedir);
    sid_directory = (const unsigned char*)_dbus_string_get_const_data(&dirname);
	
	Filename8.Append(sid_directory,strlen((const char*)sid_directory));
	Filename8.Append(_L8("\\foobar_"));
	Filename8.Append(sid_directory + 11,strlen((const char*)sid_directory + 11));
	
	
	Filename16.Copy(Filename8);
	
	User::LeaveIfError(fs.Connect());
	
	User::LeaveIfError(file.Open(fs,Filename16,EFileRead));
	file.Close();
	
	User::LeaveIfError(file.Open(fs,Filename16,EFileWrite));
	 
	User::LeaveIfError(file.Write(pid_data)); 
	
	retval = TRUE;
	
out_0 : 
	_dbus_string_free(&dirname);
	file.Close();
	fs.Close();
 	return retval;
}

int _dbus_open_file_for_read(const unsigned char* pid,const unsigned char* hash,const unsigned char* sid_directory)
{
	
	dbus_bool_t retval;
	retval = FALSE;
 	 
	TBuf8<100>Filename8 = _L8("c:\\private\\");
	Filename8.Append(sid_directory,strlen((const char*)sid_directory));
	//Filename8.Append(_L8("\\foobar"));
	Filename8.Append(_L8("\\foobar_"));
	Filename8.Append(sid_directory,strlen((const char*)sid_directory));
	
	TBuf16<100>Filename16;
	Filename16.Copy(Filename8);
	
	RFs fs;
	if(fs.Connect()!=KErrNone)
	{
		return FALSE;
	}
	
	RFile file;
	if(file.Open(fs,Filename16,EFileRead) != KErrNone)
	{
		fs.Close();
		return FALSE;
	}
	
	TBuf8<200>read_data; 
	if(file.Read(read_data) != KErrNone)
	{
		fs.Close();
		return FALSE;
	}
	
	char read_buffer[200] = "";
	strcpy(read_buffer,(char*)read_data.PtrZ());
	
	char provided_data[200] = "";
	strcpy(provided_data,(char*)pid);
	strcat(provided_data," ");
	strcat(provided_data,(char*)hash);
	
	if(strcmp(provided_data,read_buffer))
	{
		goto out_0;
	}
	 

	
	retval = TRUE;
	
out_0 :
	file.Close();
	fs.Delete(Filename16);
	fs.Close(); 
	
 	return retval;
}
}	//end extern"C"
