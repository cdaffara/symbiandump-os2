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
* Test code for pipes, using dubious WINS extension for multiple processes...
* 
*
*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>	/* for MAXPATHLEN */
#include <sys/errno.h>
#include <sys/ioctl.h>

#include "CTEST.H"

void fillbuf(int seed, char* buf, int buflen)
	{
	int j;
	sleep(seed/3);
	seed += 'A';
	for (j=0; j<buflen; j++)
		{
		buf[j]=(char)seed;
		seed+=13;
		if (seed > 127)
			seed = seed - 127 + 'A';
		}
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1078
@SYMTestCaseDesc	    Tests for operations on pipes 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing to an opened pipe and a closed pipe
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void producer(int fid, int final)
	{
	test_Data;
	char buf[128];
	int nbytes;
	int i;
	int t_errno;

	test_Title("Producer");

	for (i=1; i<=8; i++)
		{
		fillbuf(i,buf,sizeof(buf));
		printf("%d",i);
		fflush(stdout);
		nbytes=write(fid, buf, sizeof(buf));
		printf(".");
		fflush(stdout);
		test(nbytes==sizeof(buf));
		}
	printf("\n");

	if (!final)
		return;

	test_Next("Writing to closed pipe");

	fillbuf(0,buf,sizeof(buf));
	close(fid);
	nbytes=write(fid, buf, sizeof(buf));
	test(nbytes<0);
	t_errno=errno;
	test(t_errno==ENOENT);
	}

#define select_test(fid)	\
	if (async) { \
		mask=E32SELECT_READ+E32SELECT_WRITE; \
		err=ioctl(fid,E32IOSELECT,(void*)&mask); \
		}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1079
@SYMTestCaseDesc	    Tests for operations on pipes 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for reading from the pipe,
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void consumer(int fid, int async)
	{
	test_Data;
	char buf[256];
	char checkbuf[256];
	int nbytes;
	int mask=E32SELECT_READ;
	int err=0;

	if (async)
		test_Title("Asynchronous consumer");
	else
		test_Title("Consumer");

		fillbuf(1,checkbuf,128);

	/* Simple read, exactly matching single write */

	test_Next("Simple read, exactly matching write");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,128);
	test(nbytes==128);
	test(memcmp(buf,checkbuf,128)==0);

		fillbuf(2,checkbuf,128);
		fillbuf(3,checkbuf+128,128);

	/* Simple read, exactly matching 2 writes */

	test_Next("Simple read, exactly matching 2 writes");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,256);

#ifdef PIPES_SUPPORT_BUFFERING
	test(nbytes==256);
#else
	test(nbytes==128);				/* truncated at first write */

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf+128,128);	/* manually continue the read */
	test(nbytes==128);
#endif
	test(memcmp(buf,checkbuf,256)==0);

		fillbuf(4,checkbuf,128);

	/* Partial read */

	test_Next("Partial read");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,100);
	test(nbytes==100);
	test(memcmp(buf,checkbuf,100)==0);

	/* Partial read, completing the write exactly */

	test_Next("Partial read, completes matching write");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,28);
	test(nbytes==28);
	test(memcmp(buf,checkbuf+100,28)==0);

		fillbuf(5,checkbuf,128);
		fillbuf(6,checkbuf+128,128);

	/* Partial read */

	test_Next("Partial read");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,100);
	test(nbytes==100);
	test(memcmp(buf,checkbuf,100)==0);

	/* Partial read, completing the write and the following write exactly */

	test_Next("Partial read across write boundary, completes next write");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,156);
#ifdef PIPES_SUPPORT_BUFFERING
	test(nbytes==156);
#else
	test(nbytes==28);				/* truncated at first write */

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf+28,128);	/* manually continue the read */
	test(nbytes==128);
#endif
	test(memcmp(buf,checkbuf+100,156)==0);

		fillbuf(7,checkbuf,128);
		fillbuf(8,checkbuf+128,128);

	/* Partial read */

	test_Next("Partial read");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,50);
	test(nbytes==50);
	test(memcmp(buf,checkbuf,50)==0);

	/* Partial read, starting part way through the write and still not completing it */

	test_Next("Partial read, starting part way through write");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,50);
	test(nbytes==50);
	test(memcmp(buf,checkbuf+50,50)==0);

	/* Partial read, completing the 1st write and a partial read on the 2nd write */

	test_Next("Partial read across write boundary");

	nbytes=read(fid,buf,50);
#ifdef PIPES_SUPPORT_BUFFERING
	test(nbytes==50);
#else
	test(nbytes==28);				/* truncated at first write */

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf+28,22);		/* manually continue the read */
	test(nbytes==22);
#endif
	test(memcmp(buf,checkbuf+100,50)==0);

	/* Partial read, again in the 2nd write */

	test_Next("Partial read, starting part way through write");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,100);
	test(nbytes==100);
	test(memcmp(buf,checkbuf+150,100)==0);

	/* Partial read, completing the 2nd write exactly */

	test_Next("Partial read completing write");

	select_test(fid);
	test(err==0);
	test(mask==E32SELECT_READ);
	nbytes=read(fid,buf,6);
	test(nbytes==6);
	test(memcmp(buf,checkbuf+250,6)==0);

	}


void do_child()
	{
	producer(2,0);		/* produce on stderr */
	producer(2,1);		/* produce on stderr */

	consumer(0,0);		/* consume on stdin */
	consumer(0,1);		/* consume on stdin */

	close(0);
	close(1);
	}

int fids[3];

void do_parent()
	{
	consumer(fids[2],0);	/* consume on child stderr */
	consumer(fids[2],1);	/* consume on child stderr */

	producer(fids[0],0);	/* produce on child stdin */
	producer(fids[0],1);	/* produce on child stdin */
	}

/* Linked with mcrt0.o, so that the exe starts the CPosixServer automatically as per the
 * plan all along.
 */

int main(int argc, char* argv[])
	{
	void* proc2;

	start_redirection_server();

	if (argc==1)
		{
		proc2 = create_process(do_child, "CHILD", "re", fids);
		if (proc2)
			start_process(proc2);
		else
			perror("Failed to start processB: ");

		if (proc2)
			{
			int exit;
			do_parent();
			exit=wait_for_process(proc2);
			printf("wait_for_process returned %d\r\n", exit);
			}
		}
	else
		{
		do_child();
		}

	return 0;
	}
