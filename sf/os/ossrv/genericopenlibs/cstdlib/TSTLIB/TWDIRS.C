/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Test code for directory and file handling
* 
*
*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <errno.h>

#include "CTEST.H"	/* includes C interface to EPOC32 threads, and SpawnPosixServer */

test_Data;
wchar_t rootpath[MAXPATHLEN];

#define WIDENAME {L'/', L'T', L'o', L'p', L'/', L'e', L'u', L'r', L'o', 0x20ac, 0}

/* construct a directory tree in various ways */

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1082
@SYMTestCaseDesc	    Tests for operations on directory 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by creating directory with invalid name,existing directory name,wide characters
                        Tests for the error code returned while creating directories.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void make_tree()
	{
	int err,x;
	wchar_t namebuf[MAXPATHLEN], namebuf2[MAXPATHLEN];
	wchar_t toobig[MAXPATHLEN+MAXPATHLEN+1];
	wchar_t *p;

	wchar_t widename[] = WIDENAME;
	
	test_Next("Create Directory Tree - relative paths");

	err=wmkdir((wchar_t*)L"***", 0777);
	test_errno(err==-1,EINVAL);	/* bad directory name */


	err=wmkdir((wchar_t*)L"top", 0777);
	test(err==0);

	err=wmkdir((wchar_t*)L"top", 0777);
	test_errno(err==-1,EEXIST);	/* directory already exists */

	err=wmkdir(widename, 0777);
	test(err==0);

	err=wmkdir((wchar_t*)L"top/middle1/bottom1", 0777);
	test_errno(err==-1,ENOENT);	/* missing middle bit of path */

	err=wmkdir((wchar_t*)L"top/middle1", 0777);
	test(err==0);

	err=wchdir((wchar_t*)L"./top//\\/.\\.\\../top/.");
	test(err==0);

	p = wgetcwd(rootpath,sizeof(rootpath)/2);	/* save name of toplevel directory */
	test(p==rootpath);

	err=wchdir((wchar_t*)L"middle1");
	test(err==0);

	err=wchdir((wchar_t*)L"bottom2");
	test_errno(err==-1,ENOENT);	/* directory doesn't exist yet */

	p = wgetcwd(namebuf,sizeof(namebuf));	/* prepare name for tests later */
	test(p==namebuf);

	err=wmkdir((wchar_t*)L"bottom1",0777);
	test(err==0);

	err=wmkdir((wchar_t*)L"read-only",0444);
	test(err==0);

	err=wmkdir((wchar_t*)L"read-only/sub-read-only",0444);
	/* test_errno(err==-1,EACCES); */
	test(err==0);	/* Omission - EPOC32 has Win32 semantics for read-only directories */

	err=wchdir((wchar_t*)L"../../top/middle1/bottom1");
	test(err==0);

	test_Next("Create Directory Tree - absolute paths");

	p = wcscat(namebuf,(wchar_t*)L"/bottom2");
	test(p==namebuf);	/* .../top/middle1/bottom2 */

	err=wchdir(namebuf);
	test_errno(err==-1,ENOENT);	/* directory doesn't exist yet */
	
	err=wmkdir(namebuf, 0777);
	test(err==0);

	err=wchdir(namebuf);
	test(err==0);

	p = wgetcwd(namebuf,sizeof(namebuf));
	test(p==namebuf);

	err=wmkdir((wchar_t*)L"../../middle2", 0777);
	test(err==0);

	p = wgetcwd(namebuf2,sizeof(namebuf2));
	test(p==namebuf2);
	test(wcscmp(namebuf,namebuf2)==0);	/* mkdir shouldn't change cwd */

//	memset(toobig,L'a', sizeof(toobig));
//	toobig[sizeof(toobig)-1]='\0';
	for (x = 0; x < (sizeof(toobig)/2)-1; x++)
		toobig[x] = L'a';
	toobig[x] = L'\0';


	err=wmkdir(toobig,0777);
	test_errno(err<0,ENAMETOOLONG);


	test_Next("Test wgetcwd");
	
	//size too small
	p = wgetcwd(namebuf, 4);
	test_errno(0==p, ERANGE);

	//make it alloc a buffer
	p = wgetcwd(NULL, 300);
	test (NULL != p);
	free(p);

	//alloc a buffer then fail with a too small size
	p = wgetcwd(NULL, 10);
	test_errno(0==p, ERANGE);

	p = wgetcwd(namebuf2, MAXPATHLEN-1);
	test (NULL != p);

	
	test_Next("Test wrealpath");
	p = wrealpath((wchar_t*)L"/top/../top/../top/./",namebuf);
	test((0==wcscmp((wchar_t*)L"C:\\top\\", p)) || (0==wcscmp((wchar_t*)L"D:\\top\\", p)));
	test((0==wcscmp((wchar_t*)L"C:\\top\\", namebuf)) || (0==wcscmp((wchar_t*)L"D:\\top\\", namebuf)));
	p = wrealpath((wchar_t*)L"/top/../top/././top/./",namebuf);

}

/**
   Directory tree is now
 
     top / middle2   
           middle1 / bottom1
                   / bottom2
 		   / read-only / sub-read-only

@SYMTestCaseID          SYSLIB-STDLIB-CT-1083
@SYMTestCaseDesc	    Tests for operations on creating files
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by opening files which does not exists,check for closing a file twice
                        Tests for the error code returned while creating files
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void create_files()
	{
	int err;
	int fd;
	wchar_t namebuf[MAXPATHLEN],*p;

	test_Next("Creating Files - relative paths");

	err=wchdir(rootpath);
	test(err==0);

	fd = wopen((wchar_t*)L"topfile",O_RDWR+O_APPEND,0777);
	test_errno(fd<0,ENOENT);	/* doesn't exist */

	fd = wopen((wchar_t*)L"topfile",O_RDWR+O_CREAT,0777);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	err=close(fd);
	test_errno(err<0,EBADF);	/* can't close it twice */

	fd = wopen((wchar_t*)L"topfile",O_RDWR+O_APPEND,0777);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	fd = wopen((wchar_t*)L"topfile",O_RDWR+O_CREAT+O_EXCL,0777);
	test_errno(fd<0,EEXIST);	/* already exists */

	fd = wopen((wchar_t*)L"middle1/bottom2/file",O_RDONLY+O_CREAT,0444);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	fd = wopen((wchar_t*)L"middle1/bottom2/file",O_RDWR+O_APPEND,0777);
	/* test_errno(fd<0,EACCES); */
	test(fd>=0);	/* Omission - the original O_CREAT ignores the 0444 permissions */
	if (fd>=0)
		{
		err=close(fd);
		test(err==0);
		}

	err=wchmod((wchar_t*)L"middle1/bottom2/file",0444);
	test(err==0);

	fd = wopen((wchar_t*)L"middle1/bottom2/file",O_RDWR+O_APPEND,0777);
	test_errno(fd<0,EACCES);	/* not writeable */

	fd = wopen((wchar_t*)L"middle2",O_RDWR+O_CREAT,0777);
	/* test_errno(fd<0,EISDIR); */
	test_errno(fd<0,EACCES);	/* Omission - we don't do EISDIR */

	test_Next("Creating Files - absolute paths");

	err=wchdir((wchar_t*)L"middle1/bottom1");
	test(err==0);

	p = wgetcwd(namebuf,sizeof(namebuf)/2);	/* prepare name for tests later */
	test(p==namebuf);

	p = wcscat(namebuf,(wchar_t*)L"absfile");
	test(p==namebuf);

	fd = wopen(namebuf,O_RDWR+O_CREAT,0777);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	fd = wopen((wchar_t*)L"../read-only/file",O_RDWR+O_CREAT,0444);
	/* test_errno(fd<0,EACCES); */
	test(fd>=0);	/* Omission - EPOC32 has Win32 semantics for read-only directories */
	if (fd>=0)
		{
		err=close(fd);
		test(err==0);
		}
	
	}

/**
 Directory tree is now
 
     top / topfile
           middle2 /   
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
  		   / read-only / sub-read-only /
                                file
 
@SYMTestCaseID          SYSLIB-STDLIB-CT-1084
@SYMTestCaseDesc	    Tests for renaming operations 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by renaming files.Tests for the error code returned while renaming files
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void renaming()
	{
	int err;

	test_Next("Renaming");

	err=wchdir(rootpath);
	test(err==0);

	err=wrename((wchar_t*)L"middle1",(wchar_t*)L"middle2");
	test_errno(err<0,EEXIST);

	err=wrename((wchar_t*)L"middle1/bottom1/absfile",(wchar_t*)L"middle2/absfile");
	test(err==0);

	err=wrename((wchar_t*)L"middle2/absfile",(wchar_t*)L"middle1/bottom1/absfile");
	test(err==0);

	err=wrename((wchar_t*)L"middle1/bottom1/absfile",(wchar_t*)L"middle2/nonsuch/newname");
	test_errno(err<0,ENOENT);

	err=wrename((wchar_t*)L"middle1",(wchar_t*)L"middle1/bottom1/subdirectory_of_self");
	test_errno(err<0,EACCES);

	err=wrename((wchar_t*)L"middle1",(wchar_t*)L"newname");
	test(err==0);

	err=wrename((wchar_t*)L"newname/bottom2/file",(wchar_t*)L"middle2/file");
	test(err==0);

	err=wrename((wchar_t*)L"newname",(wchar_t*)L"middle1");
	test(err==0);

	err=wrename((wchar_t*)L"middle2/file",(wchar_t*)L"middle1/bottom2/file");
	test(err==0);

	err=wrename((wchar_t*)L"no such file",(wchar_t*)L"valid new name");
	test_errno(err<0,ENOENT);

	err=wrename((wchar_t*)L"no such file",(wchar_t*)L"topfile");
	test_errno(err<0,ENOENT);

	err=wrename((wchar_t*)L".",(wchar_t*)L"../different top");
	/* test_errno(err<0,EACCES);	-- can't change "." */
	test(err==0);	/* STDLIB resolves "." to full path, so this works */

	err=wrename((wchar_t*)L"../different top",rootpath);
	test(err==0);
	}

/** 
  Directory tree is now
 
     top / topfile
           middle2 /   
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
 		   / read-only / sub-read-only /
                                 file

@SYMTestCaseID          SYSLIB-STDLIB-CT-1085
@SYMTestCaseDesc	    Tests for enumeration on directories 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for using WDIR inplace of DIR
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void directory()
	{
	int err, count, i, j, fd;
	WDIR *dp;
	struct wdirent *ep;
	wchar_t name[MAXPATHLEN+1];
	off_t pos;

	test_Next("Enumerating Directories");

	err=wchdir(rootpath);
	test(err==0);

	dp=wopendir((wchar_t*)L"topfile");
	/* test_errno(dp==0,ENOTDIR); -- not convinced about this anyway */
	test_errno(dp==0,ENOENT);

	dp=wopendir((wchar_t*)L"no such file");
	test_errno(dp==0,ENOENT);

	//test something sensible happens if someone uses a WDIR inplace of a DIR
	{
		DIR *p = opendir(".");
		test(p!=0);

		// Test wants a DIR passed but won't compile under CW.
		// Force the compile by casting. The function will still get a DIR.
		// DIR inherits from WDIR.
		ep=wreaddir((WDIR*)p);  //expect a warning from this line.  p *IS* the wrong type
		test(ep != 0);
	}

	dp=wopendir((wchar_t*)L".");
	test(dp!=0);

	count=0;
	do
		{
		ep=wreaddir(dp);
		if (ep && wcscmp(ep->d_name,(wchar_t*)L".")!=0 && wcscmp(ep->d_name,(wchar_t*)L"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count==4);

	for (i=0; i<4; i++)
		{
		wrewinddir(dp);
		for (j=0; j<=i; j++)
			{
			ep=wreaddir(dp);
			test(ep!=0);
			}
		wcscpy(name,ep->d_name);
		wrewinddir(dp);
		for (j=0; j<=i; j++)
			{
			ep=wreaddir(dp);
			test(ep!=0);
			}
		test(wcscmp(name,ep->d_name)==0);
		}

	for (i=0; i<4; i++)
		{
		wrewinddir(dp);
		pos=wtelldir(dp);
		for (j=0; j<=i; j++)
			{
			pos=wtelldir(dp);
			ep=wreaddir(dp);
			test(ep!=0);
			}
		wcscpy(name,ep->d_name);
		wrewinddir(dp);
		wseekdir(dp, pos);
		ep=wreaddir(dp);
		test(ep!=0);
		test(wcscmp(name,ep->d_name)==0);
		}

	err=wclosedir(dp);
	test(err==0);

	dp=wopendir((wchar_t*)L"middle2\\");
	test(dp!=0);

	count=0;
	do
		{
		ep=wreaddir(dp);
		if (ep && wcscmp(ep->d_name,(wchar_t*)L".")!=0 && wcscmp(ep->d_name,(wchar_t*)L"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count==0);	/* empty directory */

	wrewinddir(dp);

	fd = wopen((wchar_t*)L"middle2/extrafile",O_RDWR+O_CREAT,0777);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	count=0;
	do
		{
		ep=wreaddir(dp);
		if (ep && wcscmp(ep->d_name,(wchar_t*)L".")!=0 && wcscmp(ep->d_name,(wchar_t*)L"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count==0);	/* shouldn't have noticed the change */

	wrewinddir(dp);	/* and spot the new file */
	count=0;
	do
		{
		ep=wreaddir(dp);
		if (ep && wcscmp(ep->d_name,(wchar_t*)L".")!=0 && wcscmp(ep->d_name,(wchar_t*)L"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count==1);

	wclosedir(dp);

	dp=wopendir((wchar_t*)L"/");
	test(dp!=0);

	count=0;
	do
		{
		ep=wreaddir(dp);
		if (ep && wcscmp(ep->d_name,(wchar_t*)L".")!=0 && wcscmp(ep->d_name,(wchar_t*)L"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count>0);

	wclosedir(dp);
	}

/**
 Directory tree is now
 
     top / topfile
           middle2 / extrafile  
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
 		   / read-only / sub-read-only /
                                 file

@SYMTestCaseID          SYSLIB-STDLIB-CT-1086
@SYMTestCaseDesc	    Tests for file attributes
@SYMTestPriority 	    High
@SYMTestActions  	    Tests the attributes on files and directories
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void attributes()
	{
	int err;
	struct stat s1,s2;
	int fd;
	double diff;

	test_Next("File Attributes");

	err=wchdir(rootpath);
	test(err==0);

	err=wstat((wchar_t*)L"middle earth/bag end/hobbit",&s1);
	test_errno(err<0,ENOENT);

	err=wstat((wchar_t*)L"middle1/bottom2/file",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)!=0);
	test(S_ISDIR(s1.st_mode)==0);
	test((s1.st_mode&S_IWUSR)==0);
	test(s1.st_size==0);

	err=wstat((wchar_t*)L"topfile",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)!=0);
	test(S_ISDIR(s1.st_mode)==0);
	test((s1.st_mode&S_IWUSR)!=0);
	test(s1.st_size==0);

	err=wstat((wchar_t*)L"topfile",&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s1.st_size==s2.st_size);
	diff=difftime(s1.st_mtime,s2.st_mtime);
	test(diff==(double)0.0);

	fd=wopen((wchar_t*)L"topfile", O_RDONLY, 0);
	test(fd>=0);

	err=fstat(fd,&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s1.st_size==s2.st_size);
	diff=difftime(s1.st_mtime,s2.st_mtime);
	test(diff==(double)0.0);

	err=wstat((wchar_t*)L"topfile",&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s1.st_size==s2.st_size);
	diff=difftime(s1.st_mtime,s2.st_mtime);
	test(diff==(double)0.0);

	err=close(fd);
	test(err==0);

	sleep(1);	/* to ensure that the modify time changes */

	fd=wopen((wchar_t*)L"topfile", O_RDWR+O_APPEND, 0);
	test(fd>=0);

	err=wstat((wchar_t*)L"topfile",&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s1.st_size==s2.st_size);
	/* probably not guaranteeed to have changed the modtime at this point */

	
		{
		char temp[MAXPATHLEN+1];
		wcstombs(temp,rootpath,MAXPATHLEN);
		err=write(fd,temp,3);
		test(err==3);
		}

	err=fsync(fd);
	test(err==0);

	err=close(fd);
	test(err==0);
	
	// this needs a delay here with EKA2
	// this is a slightly dodgy way of getting one
	err=wstat((wchar_t*)L"topfile",&s1);
	err=wstat((wchar_t*)L"topfile",&s2);
	err=wstat((wchar_t*)L"topfile",&s1);

	err=wstat((wchar_t*)L"topfile",&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s2.st_size==3);
	diff=difftime(s2.st_mtime,s1.st_mtime);
	test(diff>=(double)0.0);

	test_Next("Directory Attributes");

	err=wstat((wchar_t*)L"middle1",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)==0);
	test(S_ISDIR(s1.st_mode)==1);
	test((s1.st_mode&S_IWUSR)!=0);

	err=wstat((wchar_t*)L"middle1/read-only",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)==0);
	test(S_ISDIR(s1.st_mode)==1);
	test((s1.st_mode&S_IWUSR)==0);

	err=wstat((wchar_t*)L"/",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)==0);
	test(S_ISDIR(s1.st_mode)==1);

	err=waccess((wchar_t*)L"middle1/bottom1/absfile",W_OK);
	test(err==0);

	err=waccess((wchar_t*)L"middle1/bottom1/absfile",R_OK);
	test(err==0);

	err=waccess((wchar_t*)L"middle1/bottom2/file",W_OK);
	test(err!=0);

	err=waccess((wchar_t*)L"middle1/bottom2/file",R_OK);
	test(err==0);

	err=waccess((wchar_t*)L"middle1/read-only",W_OK);
	test(err==0);

	err=waccess((wchar_t*)L"middle1/read-only",R_OK);
	test(err==0);

	err=waccess((wchar_t*)L"middle1/no such directory",R_OK);
	test(err!=0);
}

/**
    Directory tree is now
  
     top / topfile
           middle2 / extrafile  
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
 		   / read-only / sub-read-only /
                                 file

@SYMTestCaseID          SYSLIB-STDLIB-CT-1087
@SYMTestCaseDesc	    Tests for searching on different drives
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by searching on z drive,test for the error codes 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
 void searching()
	{
	int err,fd;
	wchar_t wname[MAXPATHLEN+1];
	char name[MAXPATHLEN+1];
	char narrowroot[MAXPATHLEN+1];

	test_Next("Searching across drives");

	//ho hum, no wsprintf yet
	wcstombs(narrowroot, rootpath, MAXPATHLEN);
	sprintf(name,"%s/middle2/extrafile",narrowroot);
	test(-1!=mbstowcs(wname, name, MAXPATHLEN));

	err=wchdir((wchar_t*)L"z:/");
	test(err==0);

	fd=wopen(wname+2, O_RDONLY, 0);
	test_errno(fd<0,ENOENT);	// doesn't exist on z:

	wname[0]=L'?';
	fd=wopen(wname, O_RDWR, 0);
	test(fd>=0);			// found it on the original drive

	err=close(fd);
	test(err==0);
	}

/**
   Directory tree is now
 
      top / topfile
           middle2 / extrafile  
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
 		   / read-only / sub-read-only /
                                 file
 

@SYMTestCaseID          SYSLIB-STDLIB-CT-1088
@SYMTestCaseDesc	    Tests for deleting files
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by deleting files and directories.Tests for error codes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void deletion()
	{
	int err;
	wchar_t widename[] = WIDENAME;

	test_Next("Deleting - files");

	err=wchdir(rootpath);
	test(err==0);

	err=wunlink((wchar_t*)L"middle1/bottom2/file");
	test_errno(err<0,EACCES);	/* file is read-only */

	err=wchmod((wchar_t*)L"middle1/bottom2/file",0777);
	test(err==0);

	err=wunlink((wchar_t*)L"middle1/bottom2/file");
	test(err==0);

	err=wunlink((wchar_t*)L"middle2/extrafile");
	test(err==0);

	err=wunlink((wchar_t*)L"middle1/read-only/file");
	/* test_errno(err<0,EPERM);	parent directory is read-only */
	test(err==0);	/* Omission - EPOC32 uses Win32 semantics for read-only directories */

	test_Next("Deleting - directories");

	err=wchdir(rootpath);
	test(err==0);

	err=wrmdir(widename);
	test(err==0);

	err=wrmdir((wchar_t*)L"middle1");
	test_errno(err<0,EEXIST);	/* not empty */

	err=wrmdir((wchar_t*)L"middle1/bottom1");
	test_errno(err<0,EEXIST);	/* not empty */

	err=wunlink((wchar_t*)L"middle1/bottom1/absfile");
	test(err==0);

	err=wrmdir((wchar_t*)L"middle1/bottom1");
	test(err==0);

	err=wrmdir((wchar_t*)L"middle1/bottom1");
	test_errno(err<0,ENOENT);	/* already deleted */

	err=wrmdir((wchar_t*)L"middle1");
	test_errno(err<0,EEXIST);

	err=wrmdir((wchar_t*)L"middle1/bottom2");
	test(err==0);

	test_Next("Deleting - read-only directories");

	err=wrmdir((wchar_t*)L"middle1/read-only/sub-read-only");
	/* test_errno(err!=0,EACCES);	-- permission denied - read-only parent */
	test_errno(err<0,EACCES);	/* Omission - EPOC32 uses Win32 semantics */

	err=wchmod((wchar_t*)L"middle1/read-only",0777);
	test(err==0);

	err=wrmdir((wchar_t*)L"middle1/read-only/sub-read-only");
	/* test(err==0); */
	/* EPOC32 doesn't use the writeability of the parent directory, but instead looks 
	 * at the attributes of the directory itself.
	 */
	test_errno(err!=0,EACCES);

	err=wchmod((wchar_t*)L"middle1/read-only/sub-read-only",0777);
	test(err==0);

	err=wrmdir((wchar_t*)L"middle1/read-only/sub-read-only");
	test(err==0);

	err=wrmdir((wchar_t*)L"middle1/read-only");
	test(err==0);

	err=wrmdir((wchar_t*)L"middle?");
	test_errno(err<0,EINVAL);	/* no wild cards please */

	err=wrmdir((wchar_t*)L"middle1");
	test(err==0);

	err=wrmdir((wchar_t*)L"../top/middle2");
	test(err==0);

	err=wrmdir((wchar_t*)L".");
	test_errno(err<0,EEXIST);	/* not empty */

	err=wunlink((wchar_t*)L"topfile");
	test(err==0);

	err=wrmdir((wchar_t*)L".");
	test(err==0);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1089
@SYMTestCaseDesc	    Tests for creation of temporary directory and files in it.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by creating a temporary director,and files and writing to the files.
                        Check for error codes.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void temporary_files()
	{
	int err, count1, count2;
	WDIR *dp;
	struct wdirent *ep;
	FILE *fp;
	wchar_t name[L_tmpnam];
	wchar_t name2[L_tmpnam];
	char nname[L_tmpnam];
	char nname2[L_tmpnam];
	wchar_t *p;

	test_Next("Temporary files");

#define W_tmpdir   L"C:/system/temp/"
#define W_tmpfirstdir   L"C:/system/"

	dp=wopendir((wchar_t*)W_tmpdir);
	if (dp==0)
		{
		printf("  Creating the directory %S ...\n", W_tmpdir);
		err=wmkdir((wchar_t*)W_tmpfirstdir, 0777);
		err=wmkdir((wchar_t*)W_tmpdir, 0777);
 		test(err==0);
		dp=wopendir((wchar_t*)W_tmpdir);
		}
	test(dp!=0);

	count1=0;
	do
		{
		ep=wreaddir(dp);
		if (ep && wcscmp(ep->d_name,(wchar_t*)L".")!=0 && wcscmp(ep->d_name,(wchar_t*)L"..")!=0)
			count1++;
		}
	while (ep!=0);

	fp=tmpfile();
	test(fp!=0);

	err=fprintf(fp,"hello");
	test(err==5);

	wrewinddir(dp);
	count2=0;
	do
		{
		ep=wreaddir(dp);
		if (ep && wcscmp(ep->d_name,(wchar_t*)L".")!=0 && wcscmp(ep->d_name,(wchar_t*)L"..")!=0)
			count2++;
		}
	while (ep!=0);
	test(count2==count1+1);	/* EPOC32 temporary files are visible in file system */
	err=fclose(fp);
	test(err==0);

	wrewinddir(dp);
	count2=0;
	do
		{
		ep=wreaddir(dp);
		if (ep && wcscmp(ep->d_name,(wchar_t*)L".")!=0 && wcscmp(ep->d_name,(wchar_t*)L"..")!=0)
			count2++;
		}
	while (ep!=0);
	test(count2==count1);		/* should be automatically deleted */

	wclosedir(dp);

	p=wtmpnam(NULL);
	test(p!=0);

	count1=wcslen(p);
	test(count1<L_tmpnam);

	p=wtmpnam(name);
	test(p==name);

	fp=wfopen(name,(wchar_t*)L"wb+");
	test(fp!=0);

	p=wtmpnam(name2);
	test(p==name2);

	err=wcscmp(name,name2);
	test(err!=0);

	err=fclose(fp);
	test(err==0);

	err=wunlink(name);
	test(err==0);

	err=wcstombs(nname,name,50);
	test (err != -1);
	err=wcstombs(nname2,name2,50);
	test (err != -1);
	printf("  wtmpnam suggested %s and %s\n", nname, nname2);
	}

int close_console=0;
void allTests()
	{
	int err=chdir("C:\\");
	test(err==0);

	make_tree();
	create_files();
	renaming();
	directory();
	attributes();
	searching();
	deletion();
	temporary_files();

	if (close_console)
		{
		test_Close();
		close(0);
		close(1);
		close(2);
		}
	}

int main()
	{
	void* client;
	int err;

	test_Title("Directory Handling");

	allTests();

	test_Next("Do it again using the CPosixServer (for them, not me)");
	close_console=1;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */

	client=create_thread(allTests, "TDIRS tests");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);

	test_Close();
	return 0;
	}
