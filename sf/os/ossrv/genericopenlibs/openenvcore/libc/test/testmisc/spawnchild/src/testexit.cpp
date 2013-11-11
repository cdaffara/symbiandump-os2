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
// Name        : testexit.cpp
// 
//

// INCLUDE FILES
#include <e32def.h>
#include <e32std.h>
#include <stdio.h>
#include <e32base.h>

#include <stdio.h> //FILE
#include <unistd.h>
#include <stdlib.h>


void exitfunc1(void) 
	{ 	
	FILE  *fp = fopen("c:\\some.txt", "a");
	if(fp)
		{	
		fprintf(fp, "%s", "Executing exitfunc1.\n");
		fclose(fp);
		}
	}
void exitfunc2(void) 
	{ 
	FILE  *fp = fopen("c:\\some.txt", "a");
	if(fp)
		{
		fprintf(fp, "%s", "Executing exitfunc2.\n");
		fclose(fp);
		}
	}
void exitfunc3(void) 
	{ 	
	FILE  *fp = fopen("c:\\some.txt", "a");
	if(fp)
		{
		fprintf(fp, "%s", "Executing exitfunc3.\n");
		fclose(fp);
		}
	}

void exitTest()
	{

	unlink("c:\\some.txt");

	void (*vfuncp1) (void) = NULL;
	void (*vfuncp2) (void) = NULL;
	void (*vfuncp3) (void) = NULL;

	vfuncp1 = exitfunc1;
	vfuncp2 = exitfunc2;
	vfuncp3 = exitfunc3;

	atexit(vfuncp1);
	atexit(vfuncp2);
	atexit(vfuncp3);

	exit(1);
	}
	
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, exitTest());
	//Destroy cleanup stack
	delete cleanup;
	__UHEAP_MARKEND;
	return error;
	}


