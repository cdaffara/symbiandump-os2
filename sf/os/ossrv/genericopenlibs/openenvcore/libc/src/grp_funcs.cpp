// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : grp_funcs.cpp
// Part of     : libc-db specific cpp file
// This is a project specific include file for building the 
// group database related functions as part of libc library.
// Version     : 1.0
//



#include <grp.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "libc_private.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifndef EMULATOR

static pthread_mutex_t getgrent_mutex = PTHREAD_MUTEX_INITIALIZER;
int isfirstgrent=1;
#else //EMULATOR

GET_STATIC_VAR_FROM_TLS(getgrent_mutex, pthread_mutex_t)
GET_GLOBAL_VAR_FROM_TLS(isfirstgrent, int)

#define getgrent_mutex (*GET_WSD_VAR_NAME(getgrent_mutex, s)())
#define isfirstgrent	    (*GET_WSD_VAR_NAME(isfirstgrent, g)())
#endif //EMULATOR

extern "C" {

//Returns the pointer to the filled struct group with the 
//values corresponding to the group id gid
//We simulate only the possibility of having a single user in a single group 
//as the OS does not support the multiuser and multigroup concept as in typical
//unix systems.Hence we set default values as in "root" mode in unix.
EXPORT_C struct group *getgrgid(gid_t gid)
{
	struct group* res;
	struct group* tempresult;
	//The library has to return a filled pointer to struct hence we are forced to malloc
	res = (struct group*)malloc (sizeof (struct group));
	if(res == NULL){
		errno = ENOMEM;
		return NULL;
	}
	
	res->gr_mem = (char**)malloc (sizeof (char**));
	if(res->gr_mem == NULL){

		errno = ENOMEM;
		free(res);
		return NULL;
	}
	
	//We call getgrgid_r to fill the result structure
	int retval = getgrgid_r (gid, res, NULL, 0, &tempresult);
	if(retval != 0){
		//Since the library doesn't fill the struct group,its responsible for freeing them
		free (res->gr_mem);
		free (res);
		//errno is set by getgrgid_r itself in this case
		errno = retval;
		return NULL;
	}
	return res;
}


//Fills a valid grp with the values corresponding to the group id gid
//and ensures the *result points to this value
//We simulate only the possibility of having a single user in a single group 
//as the OS does not support the multiuser and multigroup concept as in typical
//unix systems.Hence we set default values as in "root" mode in unix.
EXPORT_C int getgrgid_r(gid_t gid, struct group *grp, char* /*buffer*/,size_t /*bufsize*/, struct group **result)
{

	if(gid != 0){
		errno = ENOENT;
		*result = NULL;
		return errno;
	}
	
	//Setting values corresponding to a single user systems
	grp->gr_name = "root";
	grp->gr_passwd = NULL;
	grp->gr_gid = gid;
	grp->gr_mem[0] = "root";
	
	*result = grp;
	errno = 0;
	return 0;
	
}

//Returns the pointer to the filled struct group with the 
//values corresponding to the group name
//We simulate only the possibility of having a single user in a single group 
//as the OS does not support the multiuser and multigroup concept as in typical
//unix systems.Hence we set default values as in "root" mode in unix.
EXPORT_C struct group *getgrnam(const char* name)
{
	struct group* res;
	struct group* tempresult;
	//The library has to return a filled pointer to struct hence we are forced to malloc
	res = (struct group*)malloc (sizeof (struct group));
	if(res == NULL){
		errno = ENOMEM;
		return NULL;
	}
	
	res->gr_mem = (char**)malloc (sizeof (char**));
	if(res->gr_mem == NULL){
		errno = ENOMEM;
		free(res);
		return NULL;
	}

	//We call getgrnam_r to fill the result structure
	int retval = getgrnam_r (name, res, NULL, 0, &tempresult);
	if(retval != 0){
		//Since we dont return a filled struct,we are responsible for freeing it
		free (res->gr_mem);
		free (res);
		//errno is set by getgrnam_r
		errno = retval;
		return NULL;
	}
	return res;
}


//Fills a valid grp with the values corresponding to the group name
//and ensures the *result points to this value
//We simulate only the possibility of having a single user in a single group 
//as the OS does not support the multiuser and multigroup concept as in typical
//unix systems.Hence we set default values as in "root" mode in unix.
EXPORT_C int getgrnam_r(const char *name, struct group *grp, char* /*buffer*/,size_t /*bufsize*/, struct group **result)
{

	if(strcmp (name, "root")){
		errno = ENOENT;
		*result = NULL;
		return errno;
	}
	//Setting values corresponding to single user system
	grp->gr_name = "root";
	grp->gr_passwd = NULL;
	grp->gr_gid = 0;
	grp->gr_mem[0] = "root";
	
	*result = grp;
	errno = 0;
	return 0;
	
}

//On repeated calls returns pointer to the group struct corresponding to 
//entries in group database,but since we dont support a user group concept
//we return a pointer to a filled struct group just the first time the application
//calls it and subsequent calls return NULL indicating an end of the database file
//The isfirstgrent variable is used to track the call on this api
EXPORT_C struct group *getgrent(void){

	struct group *gr;
	pthread_mutex_lock(&getgrent_mutex);
	if(isfirstgrent){
		gr = getgrgid (0);
		isfirstgrent = 0;
	}
	else{
		gr = NULL;
	}
	pthread_mutex_unlock(&getgrent_mutex);

	return gr;
}


//On repeated calls sets the pointer reading the group database,to the first entry
//but since we dont support a user group concept,we return a success each time it is called
//as if it points to the first entry in the database
//The isfirstgrent variable is used to track the call on this api
EXPORT_C int setgrent(void){
	
	pthread_mutex_lock(&getgrent_mutex);
	if(!isfirstgrent)
		isfirstgrent = 1;
	pthread_mutex_unlock(&getgrent_mutex);
	return 0;
}

//On repeated calls sets the read pointer to the end of the group database,
//but since we dont support a user group concept,we simply return
//as if it points to the last entry in the database
//The isfirstgrent variable is used to track the call on this api
EXPORT_C void endgrent(void){
	
	pthread_mutex_lock(&getgrent_mutex);
	if(!isfirstgrent)
		isfirstgrent = 0;
	pthread_mutex_unlock(&getgrent_mutex);
	return;
}

//Resets the group database with entries in ngroups for n users,
//but since the OS doesnt support this concept,we simply return 0
EXPORT_C int setgroups(int /*n*/, const gid_t* /*ngroups*/){
	return 0;
}

} // extern "C"
