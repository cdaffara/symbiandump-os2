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
// Description:
//

// INCLUDE FILES
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(void)
	{
	// create symlink on public directory, ref to file on private directory
	// create file
	const char* filename="myfile";
	const char* linkname="\\mylink";
	
	unlink(filename);
	int fd = open(filename, O_CREAT|O_RDWR);
	
    FILE* outFile = fopen("test_touch.txt", "w+");
    
	if (fd < 0)
		{
		int err= errno;
		char* perr = strerror(err);
		fprintf(outFile, "create file fail. because of [%d] %s\n", err, perr);
		}
	else
		{
		int bytes = write(fd, "1234567890", 10);
		if (bytes < 0)
			{
			int err= errno;
			char* perr = strerror(err);
			fprintf(outFile, "write to file fail. because of [%d] %s\n", err, perr);
			}
		else
			{
			fprintf(outFile, "write to file %d bytes\n", bytes);
			}
		close(fd);
	
		// create link
		int ret = symlink(filename, linkname);
		if (ret < 0)
			{
			int err= errno;
			char* perr = strerror(err);
			fprintf(outFile, "create symbol link fail, because of [%d] %s\n", err, perr);
			}
		}
	fclose(outFile);
	return 0;
	}
