/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>

#include <stdio.h>

#ifndef __EPOC32__
_LIT(KConfigPath,"c:\\System\\Data\\stdcpp_runtest.ini");
#else
_LIT(KConfigPath,"z:\\System\\Data\\stdcpp_runtest.ini");
#endif // __EPOC32__

TInt ReadConfig(TLex8& aTlex);
TInt RunProcess(TDesC8& achild, TDesC16& aParam, TInt aTimer);


TInt E32Main()
{
	TLex8 aTLex;
	TInt ret;
	TBuf8<50> child;
	ret = ReadConfig(aTLex);
	if(ret)
	{
		printf("Failed to Read with Error value %d\n", ret);
		return -1;
	}
	return 0;	
}


TInt ReadConfig(TLex8& aTLex)
{
	
	TBuf8<5400> data;
	TInt 	ret = 0;
	RFs 	aRfs;
	RFile 	aRfile;
	TInt 	asize;
	TInt tmp, iGiveAwaySum;
	
	ret = aRfs.Connect();
	
	if(KErrNone != ret )
	{
		printf("Failure to connect to the file server\n");
		return ret;
	}
	ret = aRfile.Open(aRfs,KConfigPath,EFileRead);
	
	if(KErrNone != ret )
	{
		printf("Failure to open the file\n");
		return ret;
	}
	ret = aRfile.Size(asize);
	
	if(ret != KErrNone)
	{
		printf("File with no size\n");
		return ret;
	}
	
	if(0 == asize )
	{
		printf("File with no size\n");
		return ret;
	}
	
	ret = aRfile.Read(data, asize);
	if(ret != KErrNone)
		{
		printf("Cannot read the complete file\n");
		return ret;
		}
		aTLex.Assign(data);
		TChar chr;
		TBuf8<50> copybuf;
		TBuf16<50> copybuf1;
		while (!aTLex.Eos())
			{
			TPtrC8 nexttoken = aTLex.NextToken();
			copybuf.Copy(nexttoken);
			TPtrC8 adata = aTLex.NextToken();
			TLex8 numLexer(adata);			
			if (numLexer.Val(tmp) == KErrNone)
				{
				iGiveAwaySum = tmp;
				}
			else 
				{
				//nada		
				}
			TPtrC8 nexttoken1 = aTLex.NextToken();
			copybuf1.Copy(nexttoken1);
			RunProcess(copybuf, copybuf1, tmp);				
			}
	return ret;	
}


TInt RunProcess(TDesC8& achild, TDesC16& aparam, TInt atimer)
{
	TFileName childpath;
	childpath.Copy(achild);
	TInt ret = 0;
	RProcess process;
	TRequestStatus processstatus, timerstatus;
	ret = process.Create(childpath, aparam);
	process.Resume();
	process.Logon(processstatus);
	RTimer timeout;
	timeout.CreateLocal();
	timeout.After(timerstatus, atimer*1000000);
	User::WaitForRequest(processstatus, timerstatus);
	if (processstatus == KRequestPending)
		{
		process.LogonCancel(processstatus);
		process.Kill(KErrTimedOut);
		}
	else
		{
		timeout.Cancel();
		}
	process.Close();
	timeout.Close();
	return 0;
	
}

