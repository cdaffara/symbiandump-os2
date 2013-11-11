/* Portions copyright (c) 2009 Nokia Corporation.  All rights reserved.*/
/* Description:
* This is a test application used by g_spawn APIs. This application
* is spawned by them.
*/

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
