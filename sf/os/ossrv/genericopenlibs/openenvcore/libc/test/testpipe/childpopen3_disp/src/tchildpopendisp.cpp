// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tchildpopendisp.cpp
// 
//

// INCLUDE FILES
#include <e32def.h>
#include <e32std.h>
#include <e32svr.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>


#define MAX_SIZE 50

TInt PopenDisplayChild(char** env)
	{		
	printf("%s", env[0]);  /*write data to the parent*/
	fflush(stdout);
	//	printf("%s", env[1]);  /*write data to the parent*/
	//	fflush(stdout);
	//	perror(error); /*write error to the parent*/
	sleep(2);
	return KErrNone;	
	}

TInt main(int argc, char** argv,char** envp )
	{
	int n = argc;
	if(n==1)
		{
		n=n+1;
		argv[0] = 0;
		argv[1] = 0;
		}
		
	__UHEAP_MARK;
	 PopenDisplayChild(envp);
	__UHEAP_MARKEND;
	return 0;
	}

/*class cdummy:public CTestStep
{
	public:
	cdummy()
	{
		
	}
	
	~cdummy()
	{
		
	}
	TVerdict doTestStepL()
	{
		
	}
};

void func(CTestStep* ptr, char** env)
{	
	TBuf <MAX_SIZE> buf;
	TInt len=strlen(env[0]);
	TInt i;
	for (i=0;i<len;i++)
		{
		buf[i]= *(env[0]+i);
		}
	buf[i] = '\0';
	if(ptr)
		{
		ptr->INFO_PRINTF2(_L("First Path : %S"), &buf);
		//ptr->INFO_PRINTF2("Second Path : %S", env[1]);
		}
	return;
}


TInt main(int argc,char** envp)
	{
	cdummy obj;
	func( &obj, envp);
		
#if (0)	
	//__UHEAP_MARK;
	printf("Child Process");
	printf("First Path : %s", envp[0]);
	printf("Second Path : %s", envp[1]);
	printf("********************************");
	
#endif	
	return 0;
	}*/

//  End of File



