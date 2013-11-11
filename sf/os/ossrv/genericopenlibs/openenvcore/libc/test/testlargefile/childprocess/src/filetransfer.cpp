// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name: filetransfer.cpp
// Description : To validate whether the file information is transferred properly between the child and parent process.
//

// INCLUDE FILES

#include "filetransfer.h"

int main( int argc, char** argv)
	{	
	__UHEAP_MARK;
	int ret = 0, nbytes = 0, fd = 0;
	char buf[20], buf1[20] = "parenttest", buf2[20] = "childtest";
	FILE* fp;
	ssize_t size = 0;
	off64_t position = 0;
	if (argc != 3)
		{
		return EInvalargc;
		}
	fd = atoi(argv[1]);
	nbytes = atoi(argv[2]);
	fp = fdopen(fd,"w+");
	if (fp == NULL)
		{
		return EFailfdopen;
		}
	position = lseek(fd, -nbytes, SEEK_CUR);
	if (position == (off64_t)-1) 
		{
		fclose(fp);
		return EFailfirstlseek;	
		}
	size = read(fd, buf, nbytes);
	if ( size != nbytes)
		{
		fclose(fp);
		return EFailfread;
		}
	ret = strncmp(buf,buf1,nbytes);
	if (ret != 0)
		{
		fclose(fp);
		return EFailstrcmp;	
		}
	size = write(fd,buf2,2);
	if ((size != -1 ) || (errno != EACCES))
		{
		fclose(fp);
		return EFailfirstfwrite;
		}
	position = lseek(fd,(-nbytes),SEEK_CUR);
	if (position == (off64_t)-1)
		{
		fclose(fp);
		return EFailsecondlseek;	
		}
	size = write(fd, buf2, nbytes); 
	if (size != nbytes)
		{
		fclose(fp);
		return EFailsecondfwrite;
		}
	fclose(fp);
	__UHEAP_MARKEND;
	return EFailNone;
	}

//  End of File
