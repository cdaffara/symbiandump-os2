/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/


// This is a test application used by g_spawn APIs. This application
// is spawned by them.

#include <stdio.h>

int main(int argc,char *argv[])
{
	int i;
	FILE *fp;

	fp = fopen("c:\\Helloworld.txt", "w");

	if(!fp)
	{
		printf("File creation failed\n");
		return 0;
	}
	
	fprintf(fp,"argc = %d\n", argc );
		
	for(i=0;i<argc;i++)
	{
		fprintf(fp,"argv[%d] = %s\n",i,argv[i]);
	}
	
	fclose(fp);
	return 0;
}
