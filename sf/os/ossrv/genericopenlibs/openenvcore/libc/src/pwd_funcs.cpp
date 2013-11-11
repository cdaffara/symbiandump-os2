/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is a project specific include file for building the 
*                password database related functions as part of libc library.
*
*/


#include <pwd.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libc_private.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifndef EMULATOR

static pthread_mutex_t getpwent_mutex = PTHREAD_MUTEX_INITIALIZER;
int isfirstpwent=1;
#else //EMULATOR

GET_STATIC_VAR_FROM_TLS(getpwent_mutex, pthread_mutex_t)
GET_GLOBAL_VAR_FROM_TLS(isfirstpwent, int)

#define getpwent_mutex (*GET_WSD_VAR_NAME(getpwent_mutex, s)())
#define isfirstpwent (*GET_WSD_VAR_NAME(isfirstpwent, g)())
#endif //EMULATOR

#define CWD_SIZE 100

extern "C" {

//Returns the pointer to the filled struct passwd with the 
//values corresponding to the user id uid
//We simulate only the possibility of having a single user in a single group 
//as the OS does not support the multiuser password database concept as in typical
//unix systems.Hence we set default values as in "root" mode in unix.
EXPORT_C struct passwd *getpwuid(uid_t uid)
{
	struct passwd* res;
	struct passwd* tempresult;
	//coverity[alloc_fn]
	//coverity[assign]
	res = (struct passwd*)malloc (sizeof(struct passwd));
	
	//The library has to return a filled pointer to struct hence we are forced to malloc
	if(res == NULL){
		errno = ENOMEM;
		return NULL;
	}
	//We call getpwuid_r to fill the result structure
	int retval = getpwuid_r(uid, res, NULL, 0, &tempresult);
	if(retval != 0){
		//errno is set by getgrgid_r itself in this case
		//coverity[leave_without_push]
		errno = retval;
		//Since the library doesn't fill the struct passwd,its responsible for freeing them
		free (res);
		return NULL;
	}
	return res;
}

//Fills a valid pwd with the values corresponding to the user id uid
//and ensures the *result points to this value
//We simulate only the possibility of having a single user in a single group 
//as the OS does not support the multiuser password database concept as in typical
//unix systems.Hence we set default values as in "root" mode in unix.
EXPORT_C int getpwuid_r(uid_t uid, struct passwd *pwd, char* /*buffer*/,size_t /*bufsize*/, struct passwd **result)
{

	if(uid != 0){
		errno = ENOENT;
		*result = NULL;
		return errno;
	}

	//Setting values corresponding to a single user systems
	pwd->pw_name = "root";
	pwd->pw_passwd = NULL;
	pwd->pw_uid = 0;
	pwd->pw_gid = 0;
	pwd->pw_change = 0;
	pwd->pw_class = NULL;
	pwd->pw_gecos = NULL;
	pwd->pw_dir =  getcwd(NULL, CWD_SIZE);
	pwd->pw_shell = NULL;
	pwd->pw_expire = 0;
	pwd->pw_fields = 0;

	*result = pwd;
	errno = 0;
	return 0;
	
}

//Returns the pointer to the filled struct passwd with the 
//values corresponding to the user name 
//We simulate only the possibility of having a single user in a single group 
//as the OS does not support the multiuser password database concept as in typical
//unix systems.Hence we set default values as in "root" mode in unix.
EXPORT_C struct passwd *getpwnam(const char* name)
{
	struct passwd* res;
	struct passwd* tempresult;
	//coverity[alloc_fn]
	//coverity[assign]
	res = (struct passwd*)malloc (sizeof (struct passwd));
	//The library has to return a filled pointer to struct hence we are forced to malloc
	
	if(res == NULL){
		errno = ENOMEM;
		return NULL;
	}
	
	//We call getpwnam_r to fill the result structure	
	int retval = getpwnam_r (name, res, NULL, 0, &tempresult);
	if(retval != 0){
		//errno is set by getpwnam_r
		//coverity[leave_without_push]
		errno = retval;
		//Since we dont return a filled struct,we are responsible for freeing it
		free (res);
		return NULL;
	}
	return res;
}


//Fills a valid pwd with the values corresponding to the user name
//and ensures the *result points to this value
//We simulate only the possibility of having a single user in a single group 
//as the OS does not support the multiuser password database concept as in typical
//unix systems.Hence we set default values as in "root" mode in unix.
EXPORT_C int getpwnam_r(const char *name, struct passwd *pwd, char* /*buffer*/,size_t /*bufsize*/, struct passwd **result)
{

	if(strcmp (name, "root")){
		errno = ENOENT;
		*result = NULL;
		return errno;
	}

	//Setting values corresponding to a single user systems
	pwd->pw_name = "root";
	pwd->pw_passwd = NULL;
	pwd->pw_uid = 0;
	pwd->pw_gid = 0;
	pwd->pw_change = 0;
	pwd->pw_class = NULL;
	pwd->pw_gecos = NULL;
	pwd->pw_dir =  getcwd(NULL, CWD_SIZE);
	pwd->pw_shell = NULL;
	pwd->pw_expire = 0;
	pwd->pw_fields = 0;
	
	*result = pwd;
	errno = 0;
	return 0;
	
}


//On repeated calls returns pointer to the passwd struct corresponding to 
//entries in password database,but since we dont support a password database concept
//we return a pointer to a filled struct passwd just the first time the application
//calls it and subsequent calls return NULL indicating an end of the database file
//The isfirstpwent variable is used to track the call on this api
EXPORT_C struct passwd *getpwent(void){

	struct passwd *pwd;
	pthread_mutex_lock(&getpwent_mutex);
	if(isfirstpwent){
		pwd = getpwuid(0);
		isfirstpwent = 0;
	}
	else{
		pwd = NULL;
	}
	pthread_mutex_unlock(&getpwent_mutex);

	return pwd;
}

//On repeated calls sets the pointer reading the password database,to the first entry
//but since we dont support a password database concept,we return a success each time it is called
//as if it points to the first entry in the database
//The isfirstpwent variable is used to track the call on this api
EXPORT_C void setpwent(void){
	
	pthread_mutex_lock(&getpwent_mutex);
	isfirstpwent = 1;
	pthread_mutex_unlock(&getpwent_mutex);
	return;
}

//On repeated calls sets the read pointer to the end of the passwd database,
//but since we dont support a passwd database concept,we simply return
//as if it points to the last entry in the database
//The isfirstpwent variable is used to track the call on this api
EXPORT_C void endpwent(void){
	
	pthread_mutex_lock(&getpwent_mutex);
	isfirstpwent = 0;
	pthread_mutex_unlock(&getpwent_mutex);
	return;
}


} // extern "C"
