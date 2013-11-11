// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : testsystem.cpp
// 
//

// INCLUDE FILES
#include <e32def.h>
#include <e32std.h>
#include <stdio.h>
#include <e32base.h>

#include <unistd.h>
#include <stdlib.h>

void systemTest(int argc, char **argv)
	{
	FILE *fp = fopen("c:\\testsystem.txt", "w");
	if (fp)
		{
		fprintf(fp, "I am executing\n");	
		fflush(fp);
		fprintf(fp, "  argc: %d", argc);
		for (int i = 0; i < argc; i++)
			fprintf(fp, "  argv[%d]: %s", i, argv[i]);
		fclose(fp);
		}
	}
	
int main(int argc, char **argv)
	{
	int retVal = 4;
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, systemTest(argc, argv));
	//Destroy cleanup stack
	delete cleanup;
	__UHEAP_MARKEND;
	if(KErrNone != error)
		retVal=error;
	return retVal;
	}
	
