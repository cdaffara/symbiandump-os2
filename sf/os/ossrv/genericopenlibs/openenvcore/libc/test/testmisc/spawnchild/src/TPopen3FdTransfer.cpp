// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : TPopen3FdTransfer.cpp
// 
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h> 

int main()
{

char buf[20];

int fd1 = open("C:\\SharedFile.txt",O_RDWR);
if(fd1 <= 0)
	{
		return -1 ;
	} 

int i = 0 ;		

do
{
  if(read(fd1,&buf[i],1) == -1)
	  return -1;
}while(buf[i++]);

int fd = atoi(buf);
	
if(write(fd,"testing",7) == -1)
	{
	return -1;
	}  
if(read(fd,buf,7) == -1)
	{
	return -1;
	}  

close(fd1); 
close(fd);

return 0;
} 
