/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "teststring.h"
#include <unistd.h> 
#include <errno.h>
#include <string.h>
#include <e32std.h>
#include <locale.h>
#include <stdlib.h>
#include <sys/stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>

#define MIN_LEN 10
CTestString::~CTestString() 
	{ 
	}  

CTestString::CTestString(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestString::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestString::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTestString::doTestStepL()
	{
	int err;
	if(TestStepName() == KMemCpyTestZeroLengthCopy)
		{
   		INFO_PRINTF1(_L("memcpyTestZeroLengthCopy():"));
   		err = memcpyTestZeroLengthCopy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KMemCpyBasicTest)
		{
   		INFO_PRINTF1(_L("memcpyBasicTest():"));
   		err = memcpyBasicTest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	//memmove
   	else if(TestStepName() == KMemMoveBasicTest)
		{
   		INFO_PRINTF1(_L("memmoveBasictest():"));
   		err = memmoveBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KMemMoveZeroLength)
		{
   		INFO_PRINTF1(_L("memmoveZeroLength():"));
   		err = memmoveZeroLength();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KMemMoveOverlap)
		{
   		INFO_PRINTF1(_L("memmoveOverlap():"));
   		err = memmoveOverlap();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	//strcpy
   	else if(TestStepName() == KStrcpyBasictest)
		{
   		INFO_PRINTF1(_L("strcpyBasictest():"));
   		err = strcpyBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrcpyWriteOver)
		{
   		INFO_PRINTF1(_L("KStrcpyWriteOver():"));
   		err = strcpyWriteOver();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrcpyBoundry)
		{
   		INFO_PRINTF1(_L("strcpyBoundry():"));
   		err = strcpyBoundry();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	//strncpy
   	else if(TestStepName() == KStrncpyBasictest)
		{
   		INFO_PRINTF1(_L("strncpyBasictest():"));
   		err = strncpyBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrncpyCutCnt)
		{
   		INFO_PRINTF1(_L("strncpyCutCnt():"));
   		err = strncpyCutCnt();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrncpyCutNul)
		{
   		INFO_PRINTF1(_L("strncpyCutNul():"));
   		err = strncpyCutNul();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrncpyNulInc)
		{
   		INFO_PRINTF1(_L("strncpyNulInc():"));
   		err = strncpyNulInc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrncpyZeroLen)
		{
   		INFO_PRINTF1(_L("strncpyZeroLen():"));
   		err = strncpyZeroLen();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrncpyZeroSrc)
		{
   		INFO_PRINTF1(_L("strncpyZeroSrc():"));
   		err = strncpyZeroSrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	//strcat
   	else if(TestStepName() == KStrcatBasictest)
		{
   		INFO_PRINTF1(_L("strcatBasictest():"));
   		err = strcatBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrcatBoundry)
		{
   		INFO_PRINTF1(_L("strcatBoundry():"));
   		err = strcatBoundry();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrcatMemset)
		{
   		INFO_PRINTF1(_L("strcatMemset():"));
   		err = strcatMemset();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	//strncat
   	else if(TestStepName() == KStrncatBasictest)
		{
   		INFO_PRINTF1(_L("strncatBasictest():"));
   		err = strncatBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrncatBoundry)
		{
   		INFO_PRINTF1(_L("strncatBoundry():"));
   		err = strncatBoundry();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrncatMemset)
		{
   		INFO_PRINTF1(_L("strncatMemset():"));
   		err = strncatMemset();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	
   //memcmp

   	else if(TestStepName() == KMemcmpBasictest)
		{
   		INFO_PRINTF1(_L("memcmpBasictest():"));
   		err = memcmpBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KMemcmpUnequal)
		{
   		INFO_PRINTF1(_L("memcmpUnequal():"));
   		err = memcmpUnequal();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KMemcmpLtdCount)
		{
   		INFO_PRINTF1(_L("memcmpLtdCount():"));
   		err = memcmpLtdCount();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KMemcmpZeroCount)
		{
   		INFO_PRINTF1(_L("memcmpZeroCount():"));
   		err = memcmpZeroCount();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strcmp
   	else if(TestStepName() == KStrcmpBasictest)
		{
   		INFO_PRINTF1(_L("strcmpBasictest():"));
   		err = strcmpBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrcmpMisMatchStrAndLen)
		{
   		INFO_PRINTF1(_L("strcmpMisMatchStrAndLen():"));
   		err = strcmpMisMatchStrAndLen();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	
   	//strncmp
   	else if(TestStepName() == KStrncmpBasictest)
		{
   		INFO_PRINTF1(_L("strncmpBasictest():"));
   		err = strncmpBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrncmpMisMatchStrAndLen)
		{
   		INFO_PRINTF1(_L("strncmpMisMatchStrAndLen():"));
   		err = strncmpMisMatchStrAndLen();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KStrncmpCountCheck)
		{
   		INFO_PRINTF1(_L("strncmpCountCheck():"));
   		err = strncmpCountCheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   
   //strcoll
	else if(TestStepName() == KStrcollBasictest)
		{
   		INFO_PRINTF1(_L("strcollBasictest():"));
   		err = strcollBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//srxfrm
	else if(TestStepName() == KSrxfrmBasictest)
		{
   		INFO_PRINTF1(_L("srxfrmBasictest():"));
   		err = srxfrmBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//memchr
   	else if(TestStepName() == KmemchrNotFound)
		{
   		INFO_PRINTF1(_L("memchrNotFound():"));
   		err = memchrNotFound();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KmemchrBasictest)
		{
   		INFO_PRINTF1(_L("memchrBasictest():"));
   		err = memchrBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KmemchrFindNul)
		{
   		INFO_PRINTF1(_L("memchrFindNul():"));
   		err = memchrFindNul();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == KmemchrZeroCount)
		{
   		INFO_PRINTF1(_L("memchrZeroCount():"));
   		err = memchrZeroCount();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//strchr
   	else if(TestStepName() == KstrchrNotFound)
		{
   		INFO_PRINTF1(_L("strchrNotFound():"));
   		err = strchrNotFound();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrchrBasicTest)
		{
   		INFO_PRINTF1(_L("strchrBasicTest():"));
   		err = strchrBasicTest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KstrchrFindNul)
		{
   		INFO_PRINTF1(_L("strchrFindNul():"));
   		err = strchrFindNul();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == KstrchrFindFirst)
		{
   		INFO_PRINTF1(_L("strchrFindFirst():"));
   		err = strchrFindFirst();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrchrEmptyString)
		{
   		INFO_PRINTF1(_L("strchrEmptyString():"));
   		err = strchrEmptyString();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrchrNulEmptyStr)
		{
   		INFO_PRINTF1(_L("strchrNulEmptyStr():"));
   		err = strchrNulEmptyStr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strcspn
   	else if(TestStepName() == KstrcspnWholeStr)
		{
   		INFO_PRINTF1(_L("strcspnWholeStr():"));
   		err = strcspnWholeStr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == KstrcspnPartString)
		{
   		INFO_PRINTF1(_L("strcspnPartString():"));
   		err = strcspnPartString();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrcspnNone)
		{
   		INFO_PRINTF1(_L("strcspnNone():"));
   		err = strcspnNone();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrcspnNullString)
		{
   		INFO_PRINTF1(_L("strcspnNullString():"));
   		err = strcspnNullString();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KstrcspnNullSearch)
		{
   		INFO_PRINTF1(_L("strcspnNullSearch():"));
   		err = strcspnNullSearch();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//strpbrk	  	  	   	  	
	else if(TestStepName() == KstrpbrkNotFound)
		{
   		INFO_PRINTF1(_L("strpbrkNotFound():"));
   		err = strpbrkNotFound();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrpbrkBasictest)
		{
   		INFO_PRINTF1(_L("strpbrkBasictest():"));
   		err = strpbrkBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrpbrkEmptySearch)
		{
   		INFO_PRINTF1(_L("strpbrkEmptySearch():"));
   		err = strpbrkEmptySearch();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KstrpbrkEmptyString)
		{
   		INFO_PRINTF1(_L("strpbrkEmptyString():"));
   		err = strpbrkEmptyString();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == KstrpbrkBothEmpty)
		{
   		INFO_PRINTF1(_L("strpbrkBothEmpty():"));
   		err = strpbrkBothEmpty();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrpbrkFindFirst)
		{
   		INFO_PRINTF1(_L("strpbrkFindFirst():"));
   		err = strpbrkFindFirst();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	

	//strrchr
	else if(TestStepName() == KstrrchrNotfound)
		{
   		INFO_PRINTF1(_L("strrchrNotfound():"));
   		err = strrchrNotfound();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrrchrBasictest)
		{
   		INFO_PRINTF1(_L("strrchrBasictest():"));
   		err = strrchrBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrrchrFindNul)
		{
   		INFO_PRINTF1(_L("strrchrFindNul():"));
   		err = strrchrFindNul();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KstrrchrFindLast)
		{
   		INFO_PRINTF1(_L("strrchrFindLast():"));
   		err = strrchrFindLast();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == KstrrchrEmptyStr)
		{
   		INFO_PRINTF1(_L("strrchrEmptyStr():"));
   		err = strrchrEmptyStr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrrchrEmptyNul)
		{
   		INFO_PRINTF1(_L("strrchrEmptyNul():"));
   		err = strrchrEmptyNul();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strspn
	else if(TestStepName() == KstrspnWholeStr)
		{
   		INFO_PRINTF1(_L("strspnWholeStr():"));
   		err = strspnWholeStr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrspnPartial)
		{
   		INFO_PRINTF1(_L("strspnPartial():"));
   		err = strspnPartial();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrspnNone)
		{
   		INFO_PRINTF1(_L("strspnNone():"));
   		err = strspnNone();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KstrspnNulStr)
		{
   		INFO_PRINTF1(_L("strspnNulStr():"));
   		err = strspnNulStr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == KstrspnNulSearch)
		{
   		INFO_PRINTF1(_L("strspnNulSearch():"));
   		err = strspnNulSearch();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strstr
   	else if(TestStepName() == KstrstrNotFound)
		{
   		INFO_PRINTF1(_L("strstrNotFound():"));
   		err = strstrNotFound();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KstrstrBasictest)
		{
   		INFO_PRINTF1(_L("strstrBasictest():"));
   		err = strstrBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrstrLong)
		{
   		INFO_PRINTF1(_L("strstrLong():"));
   		err = strstrLong();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrstrFindEmpty)
		{
   		INFO_PRINTF1(_L("strstrFindEmpty():"));
   		err = strstrFindEmpty();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KstrstrEmptyStr)
		{
   		INFO_PRINTF1(_L("strstrEmptyStr():"));
   		err = strstrEmptyStr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == KstrstrEmpty)
		{
   		INFO_PRINTF1(_L("strstrEmpty():"));
   		err = strstrEmpty();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strtok
   	else if(TestStepName() == KstrtokBasictest)
		{
   		INFO_PRINTF1(_L("strtokBasictest():"));
   		err = strtokBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KstrtokDelim)
		{
   		INFO_PRINTF1(_L("strtokDelim():"));
   		err = strtokDelim();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrtokChangeDelim)
		{
   		INFO_PRINTF1(_L("strtokChangeDelim():"));
   		err = strtokChangeDelim();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrtokNewStr)
		{
   		INFO_PRINTF1(_L("strtokNewStr():"));
   		err = strtokNewStr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KstrtokDiffSap)
		{
   		INFO_PRINTF1(_L("strtokDiffSap():"));
   		err = strtokDiffSap();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == KstrtokNoTok)
		{
   		INFO_PRINTF1(_L("strtokNoTok():"));
   		err = strtokNoTok();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrtokEmptyStr)
		{
   		INFO_PRINTF1(_L("strtokEmptyStr():"));
   		err = strtokEmptyStr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KstrtokNoDelim)
		{
   		INFO_PRINTF1(_L("strtokNoDelim():"));
   		err = strtokNoDelim();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrtokEmptyDelim)
		{
   		INFO_PRINTF1(_L("strtokEmptyDelim():"));
   		err = strtokEmptyDelim();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//memset
   	else if(TestStepName() == KmemsetBasictest)
		{
   		INFO_PRINTF1(_L("memsetBasictest():"));
   		err = memsetBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == KmemsetZeroLen)
		{
   		INFO_PRINTF1(_L("memsetZeroLen():"));
   		err = memsetZeroLen();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strerror	   	  	
	else if(TestStepName() == KstrerrorBasictest)
		{
   		INFO_PRINTF1(_L("strerrorBasictest():"));
   		err = strerrorBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrerrorUnknown)
		{
   		INFO_PRINTF1(_L("strerrorUnknown():"));
   		err = strerrorUnknown();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   		
	//strlen
   	else if(TestStepName() == KstrlenEmpty)
		{
   		INFO_PRINTF1(_L("strlenEmpty():"));
   		err = strlenEmpty();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KstrlenSingleChar)
		{
   		INFO_PRINTF1(_L("strlenSingleChar():"));
   		err = strlenSingleChar();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KstrlenMultiChar)
		{
   		INFO_PRINTF1(_L("strlenMultiChar():"));
   		err = strlenMultiChar();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//bcmp
   	else if(TestStepName() == Kbcmpsameargs)
		{
   		INFO_PRINTF1(_L("bcmpsameargs():"));
   		err = bcmpsameargs();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kbcmpdiffargs)
		{
   		INFO_PRINTF1(_L("bcmpdiffargs():"));
   		err = bcmpdiffargs();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == Kbcmpzerocnt)
		{
   		INFO_PRINTF1(_L("bcmpzerocnt():"));
   		err = bcmpzerocnt();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//bcopy
   	else if(TestStepName() == Kbcopyfuncheck)
		{
   		INFO_PRINTF1(_L("bcopyfuncheck():"));
   		err = bcopyfuncheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kbcopybasic)
		{
   		INFO_PRINTF1(_L("bcopybasic():"));
   		err = bcopybasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kbcopyzerocopy)
		{
   		INFO_PRINTF1(_L("bcopyzerocopy():"));
   		err = bcopyzerocopy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kbcopysrcconst)
		{
   		INFO_PRINTF1(_L("bcopysrcconst():"));
   		err = bcopysrcconst();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kbcopynullsrc)
		{
   		INFO_PRINTF1(_L("bcopynullsrc():"));
   		err = bcopynullsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	
	else if(TestStepName() == Kbcopyoverlap)
		{
   		INFO_PRINTF1(_L("bcopyoverlap():"));
   		err = bcopyoverlap();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strsep
   	else if(TestStepName() == Kstrsepbasic)
		{
   		INFO_PRINTF1(_L("strsepbasic():"));
   		err = strsepbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrsepsrccheck)
		{
   		INFO_PRINTF1(_L("strsepsrccheck():"));
   		err = strsepsrccheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//bzero
   	else if(TestStepName() == Kbzerobasic)
		{
   		INFO_PRINTF1(_L("bzerobasic():"));
   		err = bzerobasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kbzeronocopy)
		{
   		INFO_PRINTF1(_L("bzeronocopy():"));
   		err = bzeronocopy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//ffs	  	  	   	  	    	
   	else if(TestStepName() == Kffsbasic)
		{
   		INFO_PRINTF1(_L("ffsbasic():"));
   		err = ffsbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
	//index 	   		  	  	   	  	
	else if(TestStepName() == Kindexbasic)
		{
   		INFO_PRINTF1(_L("indexbasic():"));
   		err = indexbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kindexnotfound)
		{
   		INFO_PRINTF1(_L("indexnotfound():"));
   		err = indexnotfound();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kindexend)
		{
   		INFO_PRINTF1(_L("indexend():"));
   		err = indexend();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kindexbeginning)
		{
   		INFO_PRINTF1(_L("indexbeginning():"));
   		err = indexbeginning();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kindexfindnull)
		{
   		INFO_PRINTF1(_L("indexfindnull():"));
   		err = indexfindnull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kindexnullsrc)
		{
   		INFO_PRINTF1(_L("indexnullsrc():"));
   		err = indexnullsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}

	//rindex	   		  	  	   	  	
	else if(TestStepName() == Krindexbasic)
		{
   		INFO_PRINTF1(_L("rindexbasic():"));
   		err = rindexbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Krindexnotfound)
		{
   		INFO_PRINTF1(_L("rindexnotfound():"));
   		err = rindexnotfound();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Krindexend)
		{
   		INFO_PRINTF1(_L("rindexend():"));
   		err = rindexend();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Krindexbeginning)
		{
   		INFO_PRINTF1(_L("rindexbeginning():"));
   		err = rindexbeginning();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Krindexfindnull)
		{
   		INFO_PRINTF1(_L("rindexfindnull():"));
   		err = rindexfindnull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Krindexnullsrc)
		{
   		INFO_PRINTF1(_L("rindexnullsrc():"));
   		err = rindexnullsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	//memcpy
   	else if(TestStepName() == Kmemcpybasic)
		{
   		INFO_PRINTF1(_L("memcpybasic():"));
   		err = memcpybasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kmemcpyzerolength)
		{
   		INFO_PRINTF1(_L("memcpyzerolength():"));
   		err = memcpyzerolength();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmemcpysrcconstantness)
		{
   		INFO_PRINTF1(_L("memcpysrcconstantness():"));
   		err = memcpysrcconstantness();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//stpcpy
	else if(TestStepName() == Kstpcpybasic)
		{
   		INFO_PRINTF1(_L("indexend():"));
   		err = indexend();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strcasecmp
   	else if(TestStepName() == Kstrcasecmpcasecheck)
		{
   		INFO_PRINTF1(_L("strcasecmpcasecheck():"));
   		err = strcasecmpcasecheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrcasecmplargesrc)
		{
   		INFO_PRINTF1(_L("strcasecmplargesrc():"));
   		err = strcasecmplargesrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kstrcasecmpsmallsrc)
		{
   		INFO_PRINTF1(_L("strcasecmpsmallsrc():"));
   		err = strcasecmpsmallsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strncmp
	else if(TestStepName() == Kstrncmpbasic)
		{
   		INFO_PRINTF1(_L("strncmpbasic():"));
   		err = strncmpbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrncmplargesrc)
		{
   		INFO_PRINTF1(_L("strncmplargesrc():"));
   		err = strncmplargesrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrncmpsmallsrc)
		{
   		INFO_PRINTF1(_L("strncmpsmallsrc():"));
   		err = strncmpsmallsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strcoll
   	else if(TestStepName() == Kstrcollbasic)
		{
   		INFO_PRINTF1(_L("strcollbasic():"));
   		err = strcollbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrcolllargesrc)
		{
   		INFO_PRINTF1(_L("strcolllargesrc():"));
   		err = strcolllargesrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kstrcollsmallsrc)
		{
   		INFO_PRINTF1(_L("strcollsmallsrc():"));
   		err = strcollsmallsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//strndup
	else if(TestStepName() == Kstrndupbasic)
		{
   		INFO_PRINTF1(_L("strndupbasic():"));
   		err = strndupbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrnduplargecnt)
		{
   		INFO_PRINTF1(_L("strnduplargecnt():"));
   		err = strnduplargecnt();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrndupequalcnt)
		{
   		INFO_PRINTF1(_L("strndupequalcnt():"));
   		err = strndupequalcnt();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrndupzerocnt)
		{
   		INFO_PRINTF1(_L("strndupzerocnt():"));
   		err = strndupzerocnt();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrndupnullsrc)
		{
   		INFO_PRINTF1(_L("strndupnullsrc():"));
   		err = strndupnullsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strnlen	   	  	    	
   	else if(TestStepName() == Kstrnlenbasic)
		{
   		INFO_PRINTF1(_L("strnlenbasic():"));
   		err = strnlenbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrnlensmallstr)
		{
   		INFO_PRINTF1(_L("strnlensmallstr():"));
   		err = strnlensmallstr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrnlennullstr)
		{
   		INFO_PRINTF1(_L("strnlennullstr():"));
   		err = strnlennullstr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strtof
	else if(TestStepName() == Kstrtofbasic)
		{
   		INFO_PRINTF1(_L("strtofbasic():"));
   		err = strtofbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtofexpntn)
		{
   		INFO_PRINTF1(_L("strtofexpntn():"));
   		err = strtofexpntn();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtofsignedexpntn)
		{
   		INFO_PRINTF1(_L("strtofsignedexpntn():"));
   		err = strtofsignedexpntn();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kstrtofinfinity)
		{
   		INFO_PRINTF1(_L("strtofinfinity():"));
   		err = strtofinfinity();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrtofnan)
		{
   		INFO_PRINTF1(_L("strtofnan():"));
   		err = strtofnan();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strtoimax
   	else if(TestStepName() == Kstrtoimaxbasic)
		{
   		INFO_PRINTF1(_L("strtoimaxbasic():"));
   		err = strtoimaxbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrtoimaxspacecheck)
		{
   		INFO_PRINTF1(_L("strtoimaxspacecheck():"));
   		err = strtoimaxspacecheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoimaxsignedsrc)
		{
   		INFO_PRINTF1(_L("strtoimaxsignedsrc():"));
   		err = strtoimaxsignedsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoimaxoctalcheck)
		{
   		INFO_PRINTF1(_L("strtoimaxoctalcheck():"));
   		err = strtoimaxoctalcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kstrtoimaxhexcheck)
		{
   		INFO_PRINTF1(_L("strtoimaxhexcheck():"));
   		err = strtoimaxhexcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//strtoumax
	else if(TestStepName() == Kstrtoumaxbasic)
		{
   		INFO_PRINTF1(_L("strtoumaxbasic():"));
   		err = strtoumaxbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoumaxspacecheck)
		{
   		INFO_PRINTF1(_L("strtoumaxspacecheck():"));
   		err = strtoumaxspacecheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrtoumaxoctalcheck)
		{
   		INFO_PRINTF1(_L("strtoumaxoctalcheck():"));
   		err = strtoumaxoctalcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoumaxhexcheck)
		{
   		INFO_PRINTF1(_L("strtoumaxhexcheck():"));
   		err = strtoumaxhexcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strtold
   	else if(TestStepName() == Kstrtoldbasic)
		{
   		INFO_PRINTF1(_L("strtoldbasic():"));
   		err = strtoldbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kstrtoldexpntn)
		{
   		INFO_PRINTF1(_L("strtoldexpntn():"));
   		err = strtoldexpntn();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrtoldsignedexpntn)
		{
   		INFO_PRINTF1(_L("strtoldsignedexpntn():"));
   		err = strtoldsignedexpntn();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoldinfinity)
		{
   		INFO_PRINTF1(_L("strtoldinfinity():"));
   		err = strtoldinfinity();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrtoldnan)
		{
   		INFO_PRINTF1(_L("strtoldnan():"));
   		err = strtoldnan();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	//strtoll
   	else if(TestStepName() == Kstrtollbasic)
		{
   		INFO_PRINTF1(_L("strtollbasic():"));
   		err = strtollbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtollspacecheck)
		{
   		INFO_PRINTF1(_L("strtollspacecheck():"));
   		err = strtollspacecheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kstrtollsignedsrc)
		{
   		INFO_PRINTF1(_L("strtollsignedsrc():"));
   		err = strtollsignedsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrtolloctalcheck)
		{
   		INFO_PRINTF1(_L("strtolloctalcheck():"));
   		err = strtolloctalcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtollhexcheck)
		{
   		INFO_PRINTF1(_L("strtollhexcheck():"));
   		err = strtollhexcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	//strtoull
	else if(TestStepName() == Kstrtoullbasic)
		{
   		INFO_PRINTF1(_L("strtoullbasic():"));
   		err = strtoullbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoullspacecheck)
		{
   		INFO_PRINTF1(_L("strtoullspacecheck():"));
   		err = strtoullspacecheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoulloctalcheck)
		{
   		INFO_PRINTF1(_L("strtoulloctalcheck():"));
   		err = strtoulloctalcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	  	   	  	    	
   	else if(TestStepName() == Kstrtoullhexcheck)
		{
   		INFO_PRINTF1(_L("strtoullhexcheck():"));
   		err = strtoullhexcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	//swab
	else if(TestStepName() == Kswabbasic)
		{
   		INFO_PRINTF1(_L("swabbasic():"));
   		err = swabbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoqbasic)
		{
   		INFO_PRINTF1(_L("strtoqbasic():"));
   		err = strtoqbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	else if(TestStepName() == Kstrtoqspacecheck)
		{
   		INFO_PRINTF1(_L("strtoqspacecheck():"));
   		err = strtoqspacecheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoqsignedsrc)
		{
   		INFO_PRINTF1(_L("strtoqsignedsrc():"));
   		err = strtoqsignedsrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoqoctalcheck)
		{
   		INFO_PRINTF1(_L("strtoqoctalcheck():"));
   		err = strtoqoctalcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoqhexcheck)
		{
   		INFO_PRINTF1(_L("strtoqhexcheck():"));
   		err = strtoqhexcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtouqbasic)
		{
   		INFO_PRINTF1(_L("strtouqbasic():"));
   		err = strtouqbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtouqspacecheck)
		{
   		INFO_PRINTF1(_L("strtouqspacecheck():"));
   		err = strtouqspacecheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtouqoctalcheck)
		{
   		INFO_PRINTF1(_L("strtouqoctalcheck():"));
   		err = strtouqoctalcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtouqhexcheck)
		{
   		INFO_PRINTF1(_L("strtouqhexcheck():"));
   		err = strtouqhexcheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	
   	else if(TestStepName() == Kstrxfrmbasic)
		{
   		INFO_PRINTF1(_L("strxfrmbasic():"));
   		err = strxfrmbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   else if(TestStepName() == Kstrptimebasic)
		{
   		INFO_PRINTF1(_L("strptimebasic():"));
   		err = strptimebasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
     	else if(TestStepName() == Kstrftimebasic)
		{
   		INFO_PRINTF1(_L("strftimebasic():"));
   		err = strftimebasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
 	else if(TestStepName() == Kstrptime_arabic)
		{
   		INFO_PRINTF1(_L("strptime_arabic():"));
   		err = strptime_arabic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrptime_heutf)
		{
   		INFO_PRINTF1(_L("strptime_heutf():"));
   		err = strptime_heutf();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		else if(TestStepName() == Kstrftime_arabic)
		{
   		INFO_PRINTF1(_L("strftime_arabic():"));
   		err = strftime_arabic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrftime_heutf)
		{
   		INFO_PRINTF1(_L("strftime_heutf():"));
   		err = strftime_heutf();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrftime_timezone)
		{
   		INFO_PRINTF1(_L("strftime_timezone():"));
   		err = strftime_timezone();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == KTestStrtok_r)
		{
   		INFO_PRINTF1(_L("TestStrtok_r():"));
   		err = TestStrtok_r();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KTestStrnstr)
		{
   		INFO_PRINTF1(_L("TestStrnstr():"));
   		err = TestStrnstr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrcasestr)
		{
   		INFO_PRINTF1(_L("TestStrcasestr():"));
   		err = TestStrcasestr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestMemccpy)
		{
   		INFO_PRINTF1(_L("TestMemccpy():"));
   		err = TestMemccpy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStpcpy)
		{
   		INFO_PRINTF1(_L("TestStpcpy():"));
   		err = TestStpcpy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestWcsColl)
		{
   		INFO_PRINTF1(_L("TestWcsColl():"));
   		err = TestWcsColl();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstWcsCollUnlikestr1)
		{
   		INFO_PRINTF1(_L("TstWcsCollUnlikestr1():"));
   		err = TstWcsCollUnlikestr1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstWcsCollUnlikestr2)
		{
   		INFO_PRINTF1(_L("TstWcsCollUnlikestr2():"));
   		err = TstWcsCollUnlikestr2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstWcsxfrm)
		{
   		INFO_PRINTF1(_L("TstWcsxfrm():"));
   		err = TstWcsxfrm();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstWcsxfrmErr)
		{
   		INFO_PRINTF1(_L("TstWcsxfrmErr():"));
   		err = TstWcsxfrmErr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstStrdup)
		{
   		INFO_PRINTF1(_L("TstStrdup():"));
   		err = TstStrdup();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstStrndup)
		{
   		INFO_PRINTF1(_L("TstStrndup():"));
   		err = TstStrndup();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	else if(TestStepName() == KTstWcsdup)
		{
   		INFO_PRINTF1(_L("TstWcsdup():"));
   		err = TstWcsdup();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstWcsdup)
		{
   		INFO_PRINTF1(_L("TstWcsdup():"));
   		err = TstWcsdup();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstStrsep)
		{
   		INFO_PRINTF1(_L("TstStrsep():"));
   		err = TstStrsep();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   else if(TestStepName() == KTstldexp)
		{
   		INFO_PRINTF1(_L("Tstldexp():"));
   		err = Tstldexp();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   else if(TestStepName() == Kstrptimebasic1)
		{
   		INFO_PRINTF1(_L("strptimebasic1():"));
   		err = strptimebasic1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   else if(TestStepName() == KTestmemchr_specialchars)
		{
   		INFO_PRINTF1(_L("Testmemchr_specialchars():"));
   		err = Testmemchr_specialchars();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrlcatBasictest)
		{
   		INFO_PRINTF1(_L("strlcatBasictest():"));
   		err = strlcatBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrcasestr1)
		{
   		INFO_PRINTF1(_L("TestStrcasestr1():"));
   		err = TestStrcasestr1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kffsbasic1)
		{
   		INFO_PRINTF1(_L("ffsbasic1():"));
   		err = ffsbasic1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrnstr1)
		{
   		INFO_PRINTF1(_L("TestStrnstr1():"));
   		err = TestStrnstr1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrnstr2)
		{
   		INFO_PRINTF1(_L("TestStrnstr2():"));
   		err = TestStrnstr2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrnstr3)
		{
   		INFO_PRINTF1(_L("TestStrnstr3():"));
   		err = TestStrnstr3();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrncasecmp)
		{
   		INFO_PRINTF1(_L("TestStrncasecmp():"));
   		err = TestStrncasecmp();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrncasecmp1)
		{
   		INFO_PRINTF1(_L("TestStrncasecmp1():"));
   		err = TestStrncasecmp1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestMemccpy1)
		{
   		INFO_PRINTF1(_L("TestMemccpy1():"));
   		err = TestMemccpy1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrncattest1)
		{
   		INFO_PRINTF1(_L("strncattest1():"));
   		err = strncattest1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kbcopyfuncheck1)
		{
   		INFO_PRINTF1(_L("bcopyfuncheck1():"));
   		err = bcopyfuncheck1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrlcpyBasictest)
		{
   		INFO_PRINTF1(_L("strlcatBasictest():"));
   		err = strlcpyBasictest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrlcpyBasictest1)
		{
   		INFO_PRINTF1(_L("strlcatBasictest1():"));
   		err = strlcpyBasictest1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KStrlcatBasictest1)
		{
   		INFO_PRINTF1(_L("strlcatBasictest1():"));
   		err = strlcatBasictest1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstWcsxfrm1)
		{
   		INFO_PRINTF1(_L("TstWcsxfrm1():"));
   		err = TstWcsxfrm1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstWcsxfrm2)
		{
   		INFO_PRINTF1(_L("TstWcsxfrm2():"));
   		err = TstWcsxfrm2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstWcsxfrm3)
		{
   		INFO_PRINTF1(_L("TstWcsxfrm3():"));
   		err = TstWcsxfrm3();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstStrdup2)
		{
   		INFO_PRINTF1(_L("TstStrdup2():"));
   		err = TstStrdup2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstStrerr_r)
		{
   		INFO_PRINTF1(_L("TstStrerr_r():"));
   		err = TstStrerr_r();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTstStrerr_r1)
		{
   		INFO_PRINTF1(_L("TstStrerr_r1():"));

   		err = TstStrerr_r1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrcasestr2)
		{
   		INFO_PRINTF1(_L("TestStrcasestr2():"));
   		err = TestStrcasestr2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrnstr4)
		{
   		INFO_PRINTF1(_L("TestStrnstr4():"));
   		err = TestStrnstr4();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrxfrmbasic1)
		{
   		INFO_PRINTF1(_L("strxfrmbasic1():"));
   		err = strxfrmbasic1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStrptime_test)
		{
   		INFO_PRINTF1(_L("TestStrptime_test():"));
   		err = TestStrptime_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}   	
   	else if(TestStepName() == KTestStrptime_test1)
		{
   		INFO_PRINTF1(_L("TestStrptime_test1():"));
   		err = TestStrptime_test1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
     return TestStepResult(); 

	}
 
// -----------------------------------------------------------------------------
// CTestString::compare
// CTestString::validate
// are supporting function for the test
// -----------------------------------------------------------------------------
//

TInt CTestString::compare (const char *a, const char *b)
	{
	return validate(a != NULL && b != NULL && (strcmp((a), (b)) == 0));
	}
	
TInt CTestString::validate (int thing)
	{
	if (!thing)
		{
		  return -1;
		}
	else 
		{
		return KErrNone;
		}
	}

// -----------------------------------------------------------------------------
// CTestString::Basictest
// -----------------------------------------------------------------------------
//
 
TInt CTestString::memcpyBasicTest()
	{
	char ch[50];

	(void) strcpy(ch, "abcdefgh");
	(void) memcpy(ch+1, "xyz", 2);
	return compare(ch, "axydefgh");
	}
	
// -----------------------------------------------------------------------------
// CTestString::TestString
// -----------------------------------------------------------------------------
//
 
TInt CTestString::memcpyTestZeroLengthCopy()
	{
	char ch[50];

	(void) strcpy(ch, "abc");
	(void) memcpy(ch, "xyz", 0);
	return compare(ch, "abc");
	}
	

// 2. memmove
TInt CTestString::memmoveBasictest()
	{
	char ch[50];
	//STEP 1	
	(void) strcpy(ch, "abcdefgh");
	(void) memmove(ch+1, "xyz", 2);
	return compare(ch, "axydefgh");		/* Basic test. */
	}

TInt CTestString::memmoveZeroLength()
	{
	char ch[50];
	//STEP 2
	(void) strcpy(ch, "abc");
	(void) memmove(ch, "xyz", 0);
	return compare(ch, "abc");			/* Zero-length copy. */
	}

TInt CTestString::memmoveOverlap()
	{
	char ch[50];

	//STEP 3
	(void) strcpy(ch, "abcdefgh");
	(void) memmove(ch+1, ch, 9);
	return compare(ch, "aabcdefgh");		/* Overlap, right-to-left. */
	}

// 3. strcpy
TInt CTestString::strcpyBasictest()
	{
	char ch[50];
	int retval;
	//STEP 1
	retval = validate (strcpy (ch, "abcd") == ch); /* Returned value. */
	if (retval == KErrNone)
		{
		retval = compare (ch, "abcd");		/* Basic test. */	
		}
	return retval;
	}

TInt CTestString::strcpyWriteOver()
	{
	char ch[50];
	int retval;

	//STEP 2
	(void) strcpy (ch, "abcd");
	(void) strcpy (ch, "x");
	retval = compare (ch, "x");			/* Writeover. */
	if (retval == KErrNone)
		{
		retval = compare (ch+2, "cd");		/* Wrote too much? */
		}
	return retval;
	}

TInt CTestString::strcpyBoundry()
	{
	char ch[50];

	//STEP 3
	(void) strcpy (ch, "");
	return compare (ch, "");			/* Boundary condition. */
	}

// 4. strncpy
TInt CTestString::strncpyBasictest()
	{
	char ch[50];
	int retval;
	
	//STEP 1
	retval = validate (strncpy (ch, "abc", 4) == ch);	/* Returned value. */
	if (retval == KErrNone)
		{
		retval = compare (ch, "abc");			/* Did the copy go right? */	
		}
	return retval;
	}
	
TInt CTestString::strncpyCutCnt()
	{
	char ch[50];

	//STEP 2
	(void) strcpy (ch, "abcdefgh");
	(void) strncpy (ch, "xyz", 2);
	return compare (ch, "xycdefgh");			/* Copy cut by count. */
	}

TInt CTestString::strncpyCutNul()
	{
	char ch[50];
			
	//STEP 3
	(void) strcpy (ch, "abcdefgh");
	(void) strncpy (ch, "xyz", 3);		/* Copy cut just before NUL. */
	return compare (ch, "xyzdefgh");
	}

TInt CTestString::strncpyNulInc()
	{
	char ch[50];
	int retval;

	//STEP 4
	(void) strcpy (ch, "abcdefgh");
	(void) strncpy (ch, "xyz", 4);		/* Copy just includes NUL. */
	retval = compare (ch, "xyz");
	if (retval == KErrNone)
		{
		retval = compare (ch+4, "efgh");			/* Wrote too much? */	
		}
	return retval;
	}

TInt CTestString::strncpyZeroLen()
	{
	char ch[50];
		
	//STEP 5
	(void) strcpy (ch, "abc");
	(void) strncpy (ch, "xyz", 0);		/* Zero-length copy. */
	return compare (ch, "abc");
	}

TInt CTestString::strncpyZeroSrc()
	{
	char ch[50];
	int retval;
	
	//STEP 6
	(void) strncpy (ch, "", 2);		/* Zero-length source. */
	retval = compare (ch, "");
	if (retval == KErrNone)
		{
		retval = compare (ch+1, "");
		}
	if (retval == KErrNone)
		{
		retval = compare (ch+2, "c");
		if (retval<0)
			{
			retval = KErrNone; //This is a negative test case. So return Success
			}
		}
	return retval;
	}

// 5. strcat
TInt CTestString::strcatBasictest()
	{
	char ch[50];
	int retval;

	//STEP 1
	(void) strcpy (ch, "ijk");
	retval = validate (strcat (ch, "lmn") == ch); /* Returned value. */
	if (retval == KErrNone)
		{
		retval = compare (ch, "ijklmn");		/* Basic test. */
		}
	return retval;
	}
	
TInt CTestString::strcatBoundry()
	{
	char ch[50];
	int retval;

	//STEP 2
	(void) strcpy (ch, "");
	(void) strcat (ch, "");
	retval = compare (ch, "");			/* Boundary conditions. */
	if (retval == KErrNone)
		{
		(void) strcpy (ch, "ab");
		(void) strcat (ch, "");
		retval = compare (ch, "ab");
		if (retval == KErrNone)
			{
			(void) strcpy (ch, "");
			(void) strcat (ch, "cd");
			retval = compare (ch, "cd");	
			}
		}
	return retval;
	}

TInt CTestString::strcatMemset()
	{
	char ch[50];

	(void) strcpy (ch, "abcdefgh");
	(void) memset(ch+3, 0, 1);
	(void) strcat(ch, "ijkl");
	return compare(ch, "abcijkl");
	}

// 6. strncat
TInt CTestString::strncatBasictest()
	{
	char ch[50];
	int retval;
	
	//STEP 1
	(void) strcpy (ch, "ijk");
	retval = validate (strncat (ch, "lmn", 99) == ch);	/* Returned value. */
	if (retval == KErrNone)
		{
		retval = compare (ch, "ijklmn");		/* Basic test. */
		}
	
	return retval;
	}
	
TInt CTestString::strncatBoundry()
	{
	int retval;
	char ch[50];
	
	//STEP 2
	(void) strcpy (ch, "");
	(void) strncat (ch, "", 99);
	retval = compare (ch, "");			/* Boundary conditions. */
	if (retval == KErrNone)
		{
		(void) strcpy (ch, "ab");
		(void) strncat (ch, "", 99);
		retval = compare (ch, "ab");
		if (retval == KErrNone)
			{
			(void) strcpy (ch, "");
			(void) strncat (ch, "cd", 99);
			retval = compare (ch, "cd");
			}
		}	
	return retval;
	}

TInt CTestString::strncatMemset()
	{
	char ch[50];

	(void) strcpy (ch, "abcdefgh");
	(void) memset(ch+3, 0, 1);
	(void) strncat(ch, "ijkl", 100);
	return compare(ch, "abcijkl");
	}


// 7. memcmp
TInt CTestString::memcmpBasictest()
	{
	int retval;

	//STEP 1
	retval = validate(memcmp("a", "a", 1) == 0);		/* Identity. */
	if (retval == KErrNone)
		{
		retval = validate(memcmp("abc", "abc", 3) == 0);	/* Multicharacter. */
		}

	return retval;
	}
	
TInt CTestString::memcmpUnequal()
	{
	int retval;

	//STEP 2
	retval = validate(memcmp("abcd", "abce", 4) < 0);	/* Honestly unequal. */
	if (retval == KErrNone)
		{
		retval = validate(memcmp("abce", "abcd", 4) > 0);	
		}
		
	return retval;
	}

TInt CTestString::memcmpLtdCount()
	{
		
	//STEP 3
	return validate(memcmp("abce", "abcd", 3) == 0);	/* Count limited. */
	}

TInt CTestString::memcmpZeroCount()
	{
	//STEP 4
	return validate(memcmp("abc", "def", 0) == 0);	/* Zero count. */	
	}


// 8. strcmp
TInt CTestString::strcmpBasictest()
	{
	int retval;
	//STEP 1
	retval = validate (strcmp ("a", "a") == 0);		/* Identity. */
	if (retval == KErrNone)
		{
		retval = validate (strcmp ("abc", "abc") == 0);	/* Multicharacter. */
		}

	return retval;
	}
	
TInt CTestString::strcmpMisMatchStrAndLen()
	{
	int retval;

	//STEP 2
	retval = validate (strcmp ("abc", "abcd") < 0);	/* Length mismatches. */
	if (retval == KErrNone)
		{
		retval = validate (strcmp ("abcd", "abc") > 0);
		}
	if (retval == KErrNone)
		{
		retval = validate (strcmp ("abcd", "abce") < 0);	/* Honest miscompares. */
		}
	if (retval == KErrNone)
		{
		retval = validate (strcmp ("abce", "abcd") > 0);
		}
	
	return retval;
	}

// 9. strncmp
TInt CTestString::strncmpBasictest()
	{
	int retval;

	//STEP 1
	retval = validate (strncmp ("a", "a", 99) == 0);	/* Identity. */
	if (retval == KErrNone)
		{
		retval = validate (strncmp ("abc", "abc", 99) == 0);	/* Multicharacter. */
		}
		
	return retval;
	}
	
TInt CTestString::strncmpMisMatchStrAndLen()
	{
	int retval;
	//STEP 2
	retval = validate (strncmp ("abc", "abcd", 99) < 0);	/* Length unequal. */
	if (retval == KErrNone)
		{
		retval = validate (strncmp ("abcd", "abc", 99) > 0);
		}
	if (retval == KErrNone)
		{
		retval = validate (strncmp ("abcd", "abce", 99) < 0);	/* Honestly unequal. */
		}
	if (retval == KErrNone)
		{
		retval = validate (strncmp ("abce", "abcd", 99) > 0);
		}
		
	return retval;
	}

TInt CTestString::strncmpCountCheck()
	{
	int retval;

	retval = validate (strncmp ("abce", "abcd", 3) == 0);	/* Count limited. */
	if (retval == KErrNone)
		{
		retval = validate (strncmp ("abce", "abc", 3) == 0);	/* Count == length. */
		}
	
	if (retval == KErrNone)
		{
		retval = validate (strncmp ("abc", "def", 0) == 0);	/* Zero count. */
		}
		
	return retval;
	}

TInt CTestString::strcollBasictest()
	{
	int retval = KErrNone;

	retval = strcoll("Strasse", "Strasse");
	
	if (retval)
		{
		INFO_PRINTF2(_L("FAILURE, error = %d"), retval);
		}
	
	return retval;
	}

TInt CTestString::srxfrmBasictest()
	{
	char to1[8], to2[8];
   	int retval = 0;   
	int maxsize = 100;
	
	strxfrm(to1, "Strasse", maxsize);
	to1[7] = '\0';
	strxfrm(to2, "Strasse", maxsize); 
	to2[7] = '\0';
	retval = strcmp(to1, to2);
	
	if (retval)
		{
		INFO_PRINTF2(_L("FAILURE, error = %d"), retval);
		}
		
	return retval;
	}
	

TInt CTestString::memchrNotFound()
	{
	return validate(memchr("abcd", 'z', 4) == NULL);	/* Not found. */
	}

TInt CTestString::memchrBasictest()
	{
	char ch[50];
	(void) strcpy(ch, "abcd");
	return validate(memchr(ch, 'c', 4) == ch+2);	/* Basic test. */	
	}
	
TInt CTestString::memchrFindNul()
	{
	char ch[50];
	(void) strcpy(ch, "abcd");
	return validate(memchr(ch, '\0', 5) == ch+4);	/* Finding NUL. */	
	}

TInt CTestString::memchrZeroCount()
	{
	return validate(memchr("abcd", 'b', 0) == NULL);	/* Zero count. */
	}

// 13. strchr
TInt CTestString::strchrNotFound()
	{
	//STEP 1
	return validate (strchr ("abcd", 'z') == NULL);	/* Not found. */		
	}

TInt CTestString::strchrBasicTest()
	{
	char ch[50];
	//STEP 2
	(void) strcpy (ch, "abcd");
	return validate (strchr (ch, 'c') == ch+2);	/* Basic test. */	
	}

TInt CTestString::strchrFindNul()
	{
	char ch[50];
	//STEP 3
	(void) strcpy (ch, "abcd");
	return validate (strchr (ch, '\0') == ch+4);	/* Finding NUL. */	
	}

TInt CTestString::strchrFindFirst()
	{
	char ch[50];
	//STEP 4
	(void) strcpy (ch, "ababa");
	return validate (strchr (ch, 'b') == ch+1);	/* Finding first. */
	}

TInt CTestString::strchrEmptyString()
	{
	char ch[50];
	//STEP 5
	(void) strcpy (ch, "");
	return validate (strchr (ch, 'b') == NULL);		/* Empty string. */
	}

TInt CTestString::strchrNulEmptyStr()
	{
	char ch[50];
	//STEP 6
	(void) strcpy (ch, "");
	return validate (strchr (ch, '\0') == ch);		/* NUL in empty string. */
	}

// 14. strcspn
TInt CTestString::strcspnWholeStr()
	{
	//STEP 1
	return validate(strcspn("abcba", "qx") == 5);	/* Whole string. */
	}

TInt CTestString::strcspnPartString()
	{
	//STEP 2
	return validate(strcspn("abcba", "cx") == 2);	/* Partial. */
	}

TInt CTestString::strcspnNone()
	{
	//STEP 3
	return validate(strcspn("abc", "abc") == 0);	/* None. */
	}

TInt CTestString::strcspnNullString()
	{
	//STEP 4
	return validate(strcspn("", "ab") == 0);	/* Null string. */
	}

TInt CTestString::strcspnNullSearch()
	{
	//STEP 5
	return validate(strcspn("abc", "") == 3);	/* Null search list. */
	}

// 15. strpbrk
TInt CTestString::strpbrkNotFound()
	{
	//STEP 1
	return validate(strpbrk("abcd", "z") == NULL);	/* Not found. */	
	}
	
TInt CTestString::strpbrkBasictest()
	{
	char ch[50];
	//STEP 2
	(void) strcpy(ch, "abcd");
	return validate(strpbrk(ch, "cb") == ch+1);	/* Basic test. */	
	}
	
TInt CTestString::strpbrkEmptySearch()
	{
	char ch[50];
	//STEP 3
	(void) strcpy(ch, "abcd");
	return validate(strpbrk(ch, "") == NULL);	/* Empty search list. */
	}

TInt CTestString::strpbrkEmptyString()
	{
	char ch[50];
	//STEP 4
	(void) strcpy(ch, "");
	return validate(strpbrk(ch, "bcde") == NULL);	/* Empty string. */	
	}

TInt CTestString::strpbrkBothEmpty()
	{
	char ch[50];
	//STEP 5
	(void) strcpy(ch, "");
	return validate(strpbrk(ch, "") == NULL);	/* Both strings empty. */
	}

TInt CTestString::strpbrkFindFirst()
	{
	char ch[50];
	//STEP 6
	(void) strcpy(ch, "abcabdea");
	return validate(strpbrk(ch, "befg") == ch+1);	/* Finding first. */	
	}


// 16. strrchr
TInt CTestString::strrchrNotfound()
	{
	//STEP 1
	return validate (strrchr ("abcd", 'z') == NULL);	/* Not found. */
	}
	
TInt CTestString::strrchrBasictest()
	{
	char ch[50];
	//STEP 2
	(void) strcpy (ch, "abcd");
	return validate (strrchr (ch, 'c') == ch+2);	/* Basic test. */
	}

TInt CTestString::strrchrFindNul()
	{
	char ch[50];
	(void) strcpy (ch, "abcd");
	return validate (strrchr (ch, '\0') == ch+4);	/* Finding NUL. */
	}

TInt CTestString::strrchrFindLast()
	{
	char ch[50];
	//STEP 4
	(void) strcpy (ch, "ababa");
	return validate (strrchr (ch, 'b') == ch+3);	/* Finding last. */
	}

TInt CTestString::strrchrEmptyStr()
	{
	char ch[50];
	//STEP 5
	(void) strcpy (ch, "");
	return validate (strrchr (ch, 'b') == NULL);	/* Empty string. */
	}

TInt CTestString::strrchrEmptyNul()
	{
	char ch[50];
	//STEP 6
	(void) strcpy (ch, "");
	return validate (strrchr (ch, '\0') == ch);	/* NUL in empty string. */
	}

// 17. strspn
TInt CTestString::strspnWholeStr()
	{
	//STEP 1
	return validate(strspn("abcba", "abc") == 5);	/* Whole string. */
	}

TInt CTestString::strspnPartial()
	{
	return validate(strspn("abcba", "ab") == 2);	/* Partial. */
	}
	
TInt CTestString::strspnNone()
	{
	return validate(strspn("abc", "qx") == 0);	/* None. */
	}

TInt CTestString::strspnNulStr()
	{
	return validate(strspn("", "ab") == 0);	/* Null string. */
	}

TInt CTestString::strspnNulSearch()
	{
	return validate(strspn("abc", "") == 0);	/* Null search list. */
	}

// 18. strstr
TInt CTestString::strstrNotFound()
	{
	return validate(strstr("abcd", "z") == NULL);	/* Not found. */
	}
	
TInt CTestString::strstrBasictest()
	{
	char ch[50];
	(void) strcpy(ch, "abcd");
	return validate(strstr(ch, "bc") == ch+1);	/* Basic test. */
	}

TInt CTestString::strstrLong()
	{
	char ch[50];
	(void) strcpy(ch, "abcd");
	return validate(strstr(ch, "abcde") == NULL);	/* Too long. */
	}

TInt CTestString::strstrFindEmpty()
	{
	char ch[50];
	(void) strcpy(ch, "abcd");
	return validate(strstr(ch, "") == ch);	/* Finding empty. */
	}

TInt CTestString::strstrEmptyStr()
	{
	char ch[50];
	(void) strcpy(ch, "");
	return validate(strstr(ch, "b") == NULL);	/* Empty string. */
	}

TInt CTestString::strstrEmpty()
	{
	char ch[50];
	(void) strcpy(ch, "");
	return validate(strstr(ch, "") == ch);	/* Empty in empty string. */
	}

// 19. strtok
TInt CTestString::strtokBasictest()
	{
	char ch[50];
	int retval;

	(void) strcpy(ch, "first, second, third");
	retval = compare(strtok(ch, ", "), "first");	/* Basic test. */
	if (retval == KErrNone)
		{
		retval = compare(ch, "first");
		}
	if (retval == KErrNone)
		{
		retval = compare(strtok((char *)NULL, ", "), "second");
		}
	if (retval == KErrNone)
		{
		retval = compare(strtok((char *)NULL, ", "), "third");
		}
	if (retval == KErrNone)
		{
		retval = validate(strtok((char *)NULL, ", ") == NULL);
		}
	
	return retval;
	}

TInt CTestString::strtokDelim()
	{
	char ch[50];
	int retval;

	(void) strcpy(ch, ", first, ");
	retval = compare(strtok(ch, ", "), "first");	/* Extra delims, 1 tok. */
	if (retval == KErrNone)
		{
		retval = validate(strtok((char *)NULL, ", ") == NULL);
		}

	return retval;
	}

TInt CTestString::strtokChangeDelim()
	{
	char ch[50];
	int retval;

	(void) strcpy(ch, "1a, 1b; 2a, 2b");
	retval = compare(strtok(ch, ", "), "1a");	/* Changing delim lists. */
	if (retval == KErrNone)
		{
		retval = compare(strtok((char *)NULL, "; "), "1b");
		}
	if (retval == KErrNone)
		{
		retval = compare(strtok((char *)NULL, ", "), "2a");
		}

	return retval;
	}

TInt CTestString::strtokNewStr()
	{
	char ch[50];
	int retval;

	(void) strcpy(ch, "x-y");
	retval = compare(strtok(ch, "-"), "x");	/* New string before done. */
	if (retval == KErrNone)
		{
		retval = compare(strtok((char *)NULL, "-"), "y");
		}
	if (retval == KErrNone)
		{
		retval = validate(strtok((char *)NULL, "-") == NULL);
		}

	return retval;
	}

TInt CTestString::strtokDiffSap()
	{
	char ch[50];
	int retval;

	(void) strcpy(ch, "a,b, c,, ,d");
	retval = compare(strtok(ch, ", "), "a");	/* Different separators. */
	if (retval == KErrNone)
		{
		retval = compare(strtok((char *)NULL, ", "), "b");
		}
	if (retval == KErrNone)
		{
		retval = compare(strtok((char *)NULL, " ,"), "c");	/* Permute list too. */
		}
	if (retval == KErrNone)
		{
		retval = compare(strtok((char *)NULL, " ,"), "d");
		}
	if (retval == KErrNone)
		{
		retval = validate(strtok((char *)NULL, ", ") == NULL);
		}
	if (retval == KErrNone)
		{
		retval = validate(strtok((char *)NULL, ", ") == NULL);	/* Persistence. */
		}

	return retval;
	}

TInt CTestString::strtokNoTok()
	{
	char ch[50];
	
	(void) strcpy(ch, ", ");
	return validate(strtok(ch, ", ") == NULL);	/* No tokens. */
	}

TInt CTestString::strtokEmptyStr()
	{
	char ch[50];
	
	(void) strcpy(ch, "");
	return validate(strtok(ch, ", ") == NULL);	/* Empty string. */
	}

TInt CTestString::strtokNoDelim()
	{
	char ch[50];
	int retval;

	(void) strcpy(ch, "abc");
	retval = compare(strtok(ch, ", "), "abc");	/* No delimiters. */
	if (retval == KErrNone)
		{
		retval = validate(strtok((char *)NULL, ", ") == NULL);
		}

	return retval;
	}

TInt CTestString::strtokEmptyDelim()
	{
	char ch[50];
	int retval;

	(void) strcpy(ch, "abc");
	retval = compare(strtok(ch, ""), "abc");	/* Empty delimiter list. */
	if (retval == KErrNone)
		{
		retval = validate(strtok((char *)NULL, "") == NULL);	
		}
	
	return retval;
	}

// 20. memset
TInt CTestString::memsetBasictest()
	{
	int retval;
	char ch[50];
	//STEP 1
	(void) strcpy(ch, "abcdefgh");
	retval = validate(memset(ch+1, 'x', 3) == ch+1);	/* Return value. */
	if (retval == KErrNone)
		{
		retval = compare(ch, "axxxefgh");		/* Basic test. */
		}
	
	return retval;
	}
	
TInt CTestString::memsetZeroLen()
	{
	char ch[50];
	//STEP 2
	(void) strcpy(ch, "axxxefgh");
	(void) memset(ch+2, 'y', 0);
	return compare(ch, "axxxefgh");		/* Zero-length set. */
	}

// 21. strerror
TInt CTestString::strerrorBasictest()
	{
  	return validate(strerror(ERANGE) != 0);
	}
	
TInt CTestString::strerrorUnknown()
	{
  	(void) strerror(-55555); //Don't know the outcome
	return KErrNone;
	}

// 22. strlen
TInt CTestString::strlenEmpty()
	{
	return validate (strlen ("") == 0);		/* Empty. */
	}

TInt CTestString::strlenSingleChar()
	{
	return validate (strlen ("a") == 1);		/* Single char. */
	}

TInt CTestString::strlenMultiChar()
	{
	return validate (strlen ("abcd") == 4);	/* Multiple chars. */
	}
// -----------------------------------------------------------------------------
// CTestString::bcmpsameargs
// <src file name : bcmp.c>
// validate for byte comparison for same strings
// -----------------------------------------------------------------------------
	
TInt CTestString::bcmpsameargs()
	{
	return validate (bcmp("a","a",1)==0);	
	}
// -----------------------------------------------------------------------------
// CTestString::bcmpdiffargs
// <src file name : bcmp.c>
// validate for byte comparison for different strings
// -----------------------------------------------------------------------------

	
TInt CTestString::bcmpdiffargs()
	{
	return validate (bcmp("abcd","abce",4)!=0);
	}

// -----------------------------------------------------------------------------
// CTestString::bcmpzerocnt
// <src file name : bcmp.c>
// validate for byte comparison for different strings but zero count
// -----------------------------------------------------------------------------
	
TInt CTestString::bcmpzerocnt()
	{
	return validate (bcmp("abc","xyz",0)==0);
	}

// -----------------------------------------------------------------------------
// CTestString::bcopyfuncheck
// <src file name : bcopy.c>
// validate for byte copy of known src string
// -----------------------------------------------------------------------------
	
TInt CTestString::bcopyfuncheck()
	{
	char ch[50];
	bcopy("abc",ch,4);
	return compare(ch,"abc");
	}

// -----------------------------------------------------------------------------
// CTestString::bcopybasic
// <src file name : bcopy.c>
// validate for byte copy of known src string in middle of another string
// -----------------------------------------------------------------------------
	
TInt CTestString::bcopybasic()
	{
	char ch[50];
    strcpy(ch, "abcdefgh");
    bcopy("xyz", ch + 1, 2);
    return compare(ch, "axydefgh");	/* Basic test. */
	}
	
// -----------------------------------------------------------------------------
// CTestString::bcopyzerocopy
// <src file name : bcopy.c>
// validate for byte copy of known src string but with zero reference count
// -----------------------------------------------------------------------------
	
TInt CTestString::bcopyzerocopy()
	{
	char ch[50];
	strcpy(ch, "abc");
  	bcopy("xyz", ch, 0);
  	return compare(ch, "abc");
	}
	
// -----------------------------------------------------------------------------
// CTestString::bcopysrcconst
// <src file name : bcopy.c>
// validate for byte copy of known src string and validate for src constantness
// -----------------------------------------------------------------------------
	
TInt CTestString::bcopysrcconst()
	{
	char ch[50],two[50];
	strcpy(ch, "hi there");
    strcpy(two, "foo");
    bcopy(ch, two, 9);
    return compare(ch, "hi there");
	}
	
// -----------------------------------------------------------------------------
// CTestString::bcopynullsrc
// <src file name : bcopy.c>
// validate for byte copy of null src string
// -----------------------------------------------------------------------------
	
TInt CTestString::bcopynullsrc()
	{
	char ch[50];
	strcpy(ch, "hi there");
	bcopy("",ch,3);
	return compare(ch,"");
	}

// -----------------------------------------------------------------------------
// CTestString::bcopyoverlap
// <src file name : bcopy.c>
// validate for byte copy of known overlapping string
// -----------------------------------------------------------------------------
	
TInt CTestString::bcopyoverlap()
	{
	char ch[50], * two;
	strcpy(ch,"abcde");
	two=ch+2;
	bcopy(ch,two,4);
	*(two+4)='\0';
	if(strcmp(two,"abcd")) return KErrGeneral;
	if(strcmp(ch,"ababcd")) return KErrGeneral;
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CTestString::strsepbasic
// <src file name : strsep.c>
// validate for finding tokens delimited by a delimiter list
// -----------------------------------------------------------------------------
	
TInt CTestString::strsepbasic()
	{
	TInt tres=KErrNone;
	char *temp;
	char *ch=(char *)malloc (12);
	strcpy(ch,"hello,world");
	temp = ch;
	char *res;
	res=strsep(&ch,",");
	if(strcmp(res,"hello")) tres=KErrGeneral;
	free(temp);
	return tres;
	}

// -----------------------------------------------------------------------------
// CTestString::strsepsrccheck
// <src file name : strsep.c>
// validate for finding tokens delimited by a delimiter list and checking for src
// -----------------------------------------------------------------------------
	
TInt CTestString::strsepsrccheck()
	{
	TInt tres=KErrNone;
	char *ch=(char *)User::Alloc(12);
	char *temp;
	strcpy(ch,"hello,world");
	temp = ch;
	strsep(&ch,",");
	if(strcmp(ch,"world")) tres=KErrGeneral;
	User::Free(temp);
	return tres;
	}

	
// -----------------------------------------------------------------------------
// CTestString::bzerobasic
// <src file name : bzero.c>
// validate for setting of zero's in known string for known locations
// -----------------------------------------------------------------------------
	
TInt CTestString::bzerobasic()
	{
	TInt res=KErrNone;
	char ch[50];
	strcpy(ch, "abcdef");
	bzero(ch + 2, 2);
	if(strcmp(ch, "ab"))
		{
		res=KErrGeneral;
		}
	if(strcmp(ch+3, ""))
		{
		res=KErrGeneral;
		}
	if(strcmp(ch + 4, "ef"))
		{
		res=KErrGeneral;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::bzeronocopy
// <src file name : bzero.c>
// validate for setting of zero's but with zero length specified
// -----------------------------------------------------------------------------
	
TInt CTestString::bzeronocopy()
	{
	char ch[50];
	strcpy(ch, "abcdef");
	bzero(ch + 2, 0);
	return compare(ch, "abcdef");
	}	
	
// -----------------------------------------------------------------------------
// CTestString::ffsbasic
// <src file name : ffs.c>
// validate for first bit which is set with a initialized number
// -----------------------------------------------------------------------------
	
TInt CTestString::ffsbasic()
	{
	TInt res=KErrNone;
	int i=0x10;
	int j=ffs(i);
	if(j!=5)
		{
		res=KErrGeneral;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::indexbasic
// <src file name : index.c>
// validate for index of first occurence of given character in a given string
// -----------------------------------------------------------------------------
	
TInt CTestString::indexbasic()
	{
	char ch[50];
	strcpy(ch,"abcd");
	return validate(index(ch, 'c') == ch + 2);
	}

// -----------------------------------------------------------------------------
// CTestString::indexnotfound
// <src file name : index.c>
// validate for index of first occurence of character not a present in a given string
// -----------------------------------------------------------------------------
	
TInt CTestString::indexnotfound()
	{
	char ch[50];
	strcpy(ch,"abcd");
	return validate(index(ch, 'z') == NULL);
	}

// -----------------------------------------------------------------------------
// CTestString::indexend
// <src file name : index.c>
// validate for index of last occurence of character present in a given string
// -----------------------------------------------------------------------------
	
TInt CTestString::indexend()
	{
    char ch[50];
	strcpy(ch,"abcd");
	return validate(index(ch, 'd') == ch+3);
	}

// -----------------------------------------------------------------------------
// CTestString::indexfindnull
// <src file name : index.c>
// validate for index of \0 present in a given string
// -----------------------------------------------------------------------------
	
TInt CTestString::indexfindnull()
	{
    char ch[50];
	strcpy(ch,"abcd");
	return validate(index(ch, '\0') == ch+4);
	}

// -----------------------------------------------------------------------------
// CTestString::indexbeginning
// <src file name : index.c>
// validate for index of first character present in a given string
// -----------------------------------------------------------------------------
	
TInt CTestString::indexbeginning()
	{
    char ch[50];
	strcpy(ch,"abcd");
	return validate(index(ch, 'a') == ch);
	}

// -----------------------------------------------------------------------------
// CTestString::indexnullsrc
// <src file name : index.c>
// validate for index of character present in a null string
// -----------------------------------------------------------------------------
	
TInt CTestString::indexnullsrc()
	{
    char ch[50];
	strcpy(ch,"");
	return validate(index(ch, 'a') == NULL);
	}

// -----------------------------------------------------------------------------
// CTestString::rindexbasic
// <src file name : rindex.c>
// validate for index of character present in a given string fromm the end
// -----------------------------------------------------------------------------
	
TInt CTestString::rindexbasic()
	{
	char ch[50];
	strcpy(ch,"cdcab");
	return validate(rindex(ch, 'c') == ch + 2);
	}

// -----------------------------------------------------------------------------
// CTestString::rindexnotfound
// <src file name : rindex.c>
// validate for index of character not present in a given string fromm the end
// -----------------------------------------------------------------------------
	
TInt CTestString::rindexnotfound()
	{
	char ch[50];
	strcpy(ch,"dcab");
	return validate(rindex(ch, 'z') == NULL);
	}

// -----------------------------------------------------------------------------
// CTestString::rindexend
// <src file name : rindex.c>
// validate for index of last character in a given string fromm the end
// -----------------------------------------------------------------------------
	
TInt CTestString::rindexend()
	{
	char ch[50];
	strcpy(ch,"dcab");
	return validate(rindex(ch, 'b') == ch+3);
	}

// -----------------------------------------------------------------------------
// CTestString::rindexfindnull
// <src file name : rindex.c>
// validate for index of \0 character in a given string fromm the end
// -----------------------------------------------------------------------------
	
TInt CTestString::rindexfindnull()
	{
    char ch[50];
	strcpy(ch,"dcab");
	return validate(rindex(ch, '\0') == ch+4);
	}

// -----------------------------------------------------------------------------
// CTestString::rindexbeginning
// <src file name : rindex.c>
// validate for index of first character in a given string fromm the end
// -----------------------------------------------------------------------------
	
TInt CTestString::rindexbeginning()
	{
	char ch[50];
	strcpy(ch,"dcab");
	return validate(rindex(ch, 'd') == ch);
	}

// -----------------------------------------------------------------------------
// CTestString::rindexnullsrc
// <src file name : rindex.c>
// validate for index of given character in a null string fromm the end
// -----------------------------------------------------------------------------
	
TInt CTestString::rindexnullsrc()
	{
    char ch[50];
	strcpy(ch,"");
	return validate(rindex(ch, 'a') == NULL);
	}

// -----------------------------------------------------------------------------
// CTestString::memcpybasic
// <src file name : memcpy.c>
// validate for copying of characters from ch string to another location
// -----------------------------------------------------------------------------
	
TInt CTestString::memcpybasic()
	{
    char ch[50];
	strcpy(ch, "abcdefgh");
	memcpy(ch + 1, "xyz", 2);
	return compare(ch, "axydefgh");
	}

// -----------------------------------------------------------------------------
// CTestString::memcpyzerolength
// <src file name : memcpy.c>
// validate for copying of characters from ch string to another but with zero count
// -----------------------------------------------------------------------------
	
TInt CTestString::memcpyzerolength()
	{
	char ch[50];
	strcpy(ch, "abc");
	memcpy(ch, "xyz", 0);
	return compare(ch, "abc");
	}

// -----------------------------------------------------------------------------
// CTestString::memcpysrcconstantness
// <src file name : memcpy.c>
// validate for constantness for source
// -----------------------------------------------------------------------------
	
TInt CTestString::memcpysrcconstantness()
	{
    char ch[50],two[50];
	strcpy(ch, "hi there");
	strcpy(two, "foo");
	memcpy(two, ch, 9);
	return compare(ch, "hi there");	
	}

// -----------------------------------------------------------------------------
// CTestString::stpcpybasic
// <src file name : stpcpy.c>
// validate for appending of characters from ch string to another 
// -----------------------------------------------------------------------------
	
TInt CTestString::stpcpybasic()
	{
	TInt res=KErrNone;
	char *ch=new char[50] ,*buff=ch;
	ch=stpcpy(ch,"hello");
	if(strcmp(buff,"hello"))
		{
		res=KErrGeneral;
		}
	stpcpy(ch," world");
	if(strcmp(buff,"hello world"))
		{
		res=KErrGeneral;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strcasecmpcasecheck
// <src file name : strcasecmp.c>
// validate for comparison of two strings without regard of their case 
// -----------------------------------------------------------------------------
	
TInt CTestString::strcasecmpcasecheck()
	{
	TInt res=KErrNone;
	if(strcasecmp("ABC","abc"))
		{
		res=KErrGeneral;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strcasecmplargesrc
// <src file name : strcasecmp.c>
// validate for comparison of two strings without regard of their case with larger src
// -----------------------------------------------------------------------------
	
TInt CTestString::strcasecmplargesrc()
	{
	TInt res=KErrNone;
	if(strcasecmp("abcde","abc")<=0) 
		{
		res=KErrGeneral;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strcasecmpsmallsrc
// <src file name : strcasecmp.c>
// validate for comparison of two strings without regard of their case with smaller src
// -----------------------------------------------------------------------------
	
TInt CTestString::strcasecmpsmallsrc()
	{
	TInt res=KErrNone;
	if(strcasecmp("abc","abcde")>=0)
		{
		res=KErrGeneral;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strncmpbasic
// <src file name : strncmp.c>
// validate for comparison of two strings with given count
// -----------------------------------------------------------------------------
	
TInt CTestString::strncmpbasic()
	{
	TInt res=KErrNone;
	if(strncmp("abcde","abc",3))
		{
		res=KErrGeneral;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strncmplargesrc
// <src file name : strncmp.c>
// validate for comparison of two strings with given count and larger src string
// -----------------------------------------------------------------------------
	
TInt CTestString::strncmplargesrc()
	{
	TInt res=KErrNone;
	if(strncmp("abcde","abc",5)<=0)
		{
		res=KErrGeneral;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strncmpsmallsrc
// <src file name : strncmp.c>
// validate for comparison of two strings with given count and smaller src string
// -----------------------------------------------------------------------------
	
TInt CTestString::strncmpsmallsrc()
	{
	TInt res=KErrNone;
	if(strncmp("abc","abcde",5)>=0)
		{
		res=KErrGeneral;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strcollbasic
// <src file name : strcoll.c>
// validate for comparison of two strings with particular locale set
// -----------------------------------------------------------------------------
	
TInt CTestString::strcollbasic()
	{
	TInt res=KErrNone;
	setlocale(LC_ALL,"ar_AE.ISO-8859-6");
	if(strcoll("abcde","abcde"))
		{
		res=KErrGeneral;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strcolllargesrc
// <src file name : strcoll.c>
// validate for comparison of two strings with particular locale set
// -----------------------------------------------------------------------------
	
TInt CTestString::strcolllargesrc()
	{
	TInt res=KErrNone;
	setlocale(LC_ALL,"ar_AE.ISO-8859-6");
	if(strcoll("abcde","abc")<=0)
		{
		res=KErrGeneral;
		}
	return res;
	}	

// -----------------------------------------------------------------------------
// CTestString::strcolllargesrc
// <src file name : strcoll.c>
// validate for comparison of two strings with particular locale set
// -----------------------------------------------------------------------------
	
TInt CTestString::strcollsmallsrc()
	{
	TInt res=KErrNone;
	setlocale(LC_ALL,"ar_AE.ISO-8859-6");
	if(strcoll("abc","abcde")>=0)
		{
		res=KErrGeneral;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strndupbasic
// <src file name : strndup.c>
// validate for duplication of given src string into another location with count specified
// -----------------------------------------------------------------------------
	
TInt CTestString::strndupbasic()
	{
	int ret;
	char *dupbasic = strndup("abcde",3);
	if(!compare(dupbasic,"abc"))
		{
		ret = KErrNone;
		}
	else
		{
		ret = KErrGeneral;
		}
	free(dupbasic);
	return ret;
	}

// -----------------------------------------------------------------------------
// CTestString::strnduplargecnt
// <src file name : strndup.c>
// validate for duplication of given src string into another location with count specified
// and count is larger than length of src string
// -----------------------------------------------------------------------------
	
TInt CTestString::strnduplargecnt()
	{
	int ret;
	char *duplarge = strndup("abcde",8);
	if(!compare(duplarge,"abcde"))
		{
		ret = KErrNone;
		}
	else
		{
		ret = KErrGeneral;
		}
	free(duplarge);
	return ret;
	}


// -----------------------------------------------------------------------------
// CTestString::strndupequalcnt
// <src file name : strndup.c>
// validate for duplication of given src string into another location with count specified
// and count is compare to length of src string
// -----------------------------------------------------------------------------
	
TInt CTestString::strndupequalcnt()
	{
	int ret;
	char *dupeql = strndup("abcde",5);
	if(!compare(dupeql,"abcde"))
		{
		ret = KErrNone;
		}
	else
		{
		ret = KErrGeneral;
		}
	free(dupeql);
	return ret;
	}

// -----------------------------------------------------------------------------
// CTestString::strndupzerocnt
// <src file name : strndup.c>
// validate for duplication of given src string into another location with count specified
// and count is zero
// -----------------------------------------------------------------------------
	
TInt CTestString::strndupzerocnt()
	{
	int ret;
	char *dupzero = strndup("abcde",0);
	if(!compare(dupzero,""))
		{
		ret = KErrNone;
		}
	else
		{
		ret = KErrGeneral;
		}
	free(dupzero);
	return ret;
	}

// -----------------------------------------------------------------------------
// CTestString::strndupnullsrc
// <src file name : strndup.c>
// validate for duplication of given src string into another location with null 
// src string
// -----------------------------------------------------------------------------
	
TInt CTestString::strndupnullsrc()
	{
	int ret;
	char *dupnull = strndup("",2);
	if(!compare(dupnull,""))
		{
		ret = KErrNone;
		}
	else
		{
		ret = KErrGeneral;
		}
	free(dupnull);
	return ret;
	}

// -----------------------------------------------------------------------------
// CTestString::strnlenbasic
// <src file name : strnlen.c>
// validate for length of given string with max specified count
// -----------------------------------------------------------------------------
	
TInt CTestString::strnlenbasic()
	{
	TInt res=KErrNone;
	char ch[50];
	strcpy(ch,"abcdef");
	if(strnlen(ch,5)!=5)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strnlensmallstr
// <src file name : strnlen.c>
// validate for length of given string with max specified count larger than string length
// -----------------------------------------------------------------------------
	
TInt CTestString::strnlensmallstr()
	{
	TInt res=KErrNone;
	char ch[50];
	strcpy(ch,"abcdef");
	if(strnlen(ch,10)!=6)
		{
		res=KErrNone;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strnlennullstr
// <src file name : strnlen.c>
// validate for length of given null string with max specified count 
// -----------------------------------------------------------------------------
	
TInt CTestString::strnlennullstr()
	{
	TInt res=KErrNone;
	char ch[50];
	strcpy(ch,"");
	if(strnlen(ch,10)!=0)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtofbasic
// <src file name : strtof.c>
// validate for given decimal number in string to float value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtofbasic()
	{
	TInt res=KErrGeneral;
	char *endp;
	float i=strtof("12.12",&endp);
	if(i==12.12||*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtofexpntn
// <src file name : strtof.c>
// validate for given decimal number in exponential notation in string to float value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtofexpntn()
	{
	TInt res=KErrGeneral;
	char *endp;
	float i=strtof("12e1",&endp);
	if(i==120||*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strtofsignedexpntn
// <src file name : strtof.c>
// validate for given signed decimal number in exponential notation in string to float 
// value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtofsignedexpntn()
	{
	TInt res=KErrGeneral;
	char *endp;
	float i=strtof("12e-1",&endp);
	if(i==1.2||*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtofinfinity
// <src file name : strtof.c>
// validate for "INF" or infinity to appropriate value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtofinfinity()
	{
	TInt res=KErrGeneral;
	char *endp;
	float i=strtof("INF",&endp);
	if(*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtofnan
// <src file name : strtof.c>
// validate for "NAN" or not a number to appropriate value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtofnan()
	{
	TInt res=KErrGeneral;
	char *endp;
	float i=strtof("NAN",&endp);
	if(*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtoimaxbasic
// <src file name : strtoimax.c>
// validate for given integer in string to intmax_t value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoimaxbasic()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoimax("12",&endp,10);
	if(i==12)
		{
		res=KErrNone;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strtoimaxspacecheck
// <src file name : strtoimax.c>
// validate for given integer in string with initial spaces to intmax_t value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoimaxspacecheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoimax("   9000",&endp,10);
	if(i==9000)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtoimaxsignedsrc
// <src file name : strtoimax.c>
// validate for given integer in string with initial spaces and sign to intmax_t value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoimaxsignedsrc()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoimax("   -9000",&endp,10);
	if(i==-9000)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtoimaxoctalcheck
// <src file name : strtoimax.c>
// validate for given integer in string octal representation to intmax_t value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoimaxoctalcheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoimax("010",&endp,8);
	if(i==8)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtoimaxhexcheck
// <src file name : strtoimax.c>
// validate for given integer in string hexadecimal representation to intmax_t value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoimaxhexcheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoimax("0xa",&endp,16);
	if(i==10)
		{
		res=KErrNone;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strtoumaxbasic
// <src file name : strtoumax.c>
// validate for given integer in string to uintmax_t value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoumaxbasic()
	{
	TInt res=KErrGeneral;
	char *endp;
	uintmax_t i=strtoumax("12",&endp,10);
	if(i==12)
		{
		res=KErrNone;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strtoumaxspacecheck
// <src file name : strtoumax.c>
// validate for given integer in string with initial spaces to uintmax_t value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoumaxspacecheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	uintmax_t i=strtoumax("   9000",&endp,10);
	if(i==9000)
		{
		res=KErrNone;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strtoumaxoctalcheck
// <src file name : strtoumax.c>
// validate for given integer in octal notation in string to uintmax_t value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoumaxoctalcheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	uintmax_t i=strtoumax("010",&endp,8);
	if(i==8)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtoumaxhexcheck
// <src file name : strtoumax.c>
// validate for given integer in hexadecimal notation in string to uintmax_t value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoumaxhexcheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	uintmax_t i=strtoumax("0xa",&endp,16);
	if(i==10)
		{
		res=KErrNone;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strtoldbasic
// <src file name : strtold.c>
// validate for given decimal in string to double value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoldbasic()
	{
	TInt res=KErrGeneral;
	char *endp;
	long double i=strtold("12.12",&endp);
	if(i==12.12||*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtoldexpntn
// <src file name : strtold.c>
// validate for given decimal in string in exponential notation to double value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoldexpntn()
	{
	TInt res=KErrGeneral;
	char *endp;
	long double i=strtold("12e1",&endp);
	if(i==120||*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strtoldsignedexpntn
// <src file name : strtold.c>
// validate for given decimal in string in signed exponential notation to double value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoldsignedexpntn()
	{
	TInt res=KErrGeneral;
	char *endp;
	long double i=strtold("12e-1",&endp);
	if(i==1.2||*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtoldinfinity
// <src file name : strtold.c>
// validate for given "INF" or infinity to corresponding value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoldinfinity()
	{
	TInt res=KErrGeneral;
	char *endp;
	long double i=strtold("INF",&endp);
	if(*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtoldnan
// <src file name : strtold.c>
// validate for given "NAN" or not a number to corresponding value
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoldnan()
	{
	TInt res=KErrGeneral;
	char *endp;
	long double i=strtold("NAN",&endp);
	if(*endp=='\0')
		{
		res=KErrNone;
		}
	return res;
	}


// -----------------------------------------------------------------------------
// CTestString::strtollbasic
// <src file name : strtoll.c>
// validate for given long int in string to long long
// -----------------------------------------------------------------------------
	
TInt CTestString::strtollbasic()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoll("999999999999990000",&endp,10);
	if(i==999999999999990000)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtollspacecheck
// <src file name : strtoll.c>
// validate for given long int in string with initial spaces to long long
// -----------------------------------------------------------------------------
	
TInt CTestString::strtollspacecheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoll("   999999999999990000",&endp,10);
	if(i==999999999999990000)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtollsignedsrc
// <src file name : strtoll.c>
// validate for given long int in string with initial spaces and sign to long long
// -----------------------------------------------------------------------------
	
TInt CTestString::strtollsignedsrc()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoll("   -999999999999990000",&endp,10);
	if(i==-999999999999990000)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtolloctalcheck
// <src file name : strtoll.c>
// validate for given long int in octal notaion in string to long long
// -----------------------------------------------------------------------------
	
TInt CTestString::strtolloctalcheck()
	{	
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoll("010",&endp,8);
	if(i==8)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtollhexcheck
// <src file name : strtoll.c>
// validate for given long int in hex notaion in string to long long
// -----------------------------------------------------------------------------
	
TInt CTestString::strtollhexcheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoll("0xa",&endp,16);
	if(i==10)
		{
		res=KErrNone;
		}
	return res;
	}

// -----------------------------------------------------------------------------
// CTestString::strtoullbasic
// <src file name : strtoll.c>
// validate for given long int in hex notaion in string to long long
// -----------------------------------------------------------------------------
	
TInt CTestString::strtoullbasic()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoull("900000",&endp,10);
	if(i==900000)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtoullspacecheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoull("   900000",&endp,10);
	if(i==900000)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtoulloctalcheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoull("010",&endp,8);
	if(i==8)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtoullhexcheck()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoull("0xa",&endp,16);
	if(i==10)
		{
		res=KErrNone;
		}
	return res;
	}


//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::swabbasic()
	{
	TInt res = KErrGeneral;
	int i=0x00003366,j=0x0;
	swab((void *)&i,(void *)&j,2);
	if(j==0x6633)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtoqbasic()
	{
	INFO_PRINTF1(_L("CTestString::strtoqbasic"));
	TInt res=KErrGeneral;
	char *endp;
	quad_t i=strtoq("900000",&endp,10);
	if(i==900000)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtoqspacecheck()
	{
	INFO_PRINTF1(_L("CTestString::strtoqspacecheck"));
	TInt res=KErrGeneral;
	char *endp;
	quad_t i=strtoq("   900000",&endp,10);
	if(i==900000)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtoqsignedsrc()
	{
	INFO_PRINTF1(_L("CTestString::strtoqsignedsrc"));
	TInt res=KErrGeneral;
	char *endp;
	quad_t i=strtoq("   -900000",&endp,10);
	if(i==-900000)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtoqoctalcheck()
	{
	INFO_PRINTF1(_L("CTestString::strtoqoctalcheck"));
	TInt res=KErrGeneral;
	char *endp;
	quad_t i=strtoq("010",&endp,8);
	if(i==8)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtoqhexcheck()
	{
	INFO_PRINTF1(_L("CTestString::strtoqhexcheck"));
	TInt res=KErrGeneral;
	char *endp;
	quad_t i=strtoq("0xa",&endp,16);
	if(i==10)
		{
		res=KErrNone;
		}
	return res;
	}


//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtouqbasic()
	{
	INFO_PRINTF1(_L("CTestString::strtouqbasic"));
	TInt res=KErrGeneral;
	char *endp;
	quad_t i=strtouq("900000",&endp,10);
	if(i==900000)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtouqspacecheck()
	{
	INFO_PRINTF1(_L("CTestString::strtouqspacecheck"));
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoull("   900000",&endp,10);
	if(i==900000)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtouqoctalcheck()
	{
	INFO_PRINTF1(_L("CTestString::strtouqoctalcheck"));
	TInt res=KErrGeneral;
	char *endp;
	quad_t i=strtouq("010",&endp,8);
	if(i==8)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strtouqhexcheck()
	{
	INFO_PRINTF1(_L("CTestString::strtouqhexcheck"));
	TInt res=KErrGeneral;
	char *endp;
	quad_t i=strtouq("0xa",&endp,16);
	if(i==10)
		{
		res=KErrNone;
		}
	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strxfrmbasic()
	{
	TInt res=KErrGeneral;
		
    char src2[20] = "abc";
    char dst1[20] = {'\0'};
    char src1[20] = "abc";
    char dst2[20] = {'\0'};
    int r = strxfrm(dst1,src1,strlen(src1));
    if(r == strlen(src1))
    	{
    	INFO_PRINTF1(_L("strxfrm successful"));
    	}
    strxfrm(dst2,src2,strlen(src2));
    if((strcmp(dst1,dst2))== 0)
        {
		res=KErrNone;
		}                          
    return res;
	}


//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strptimebasic()
	{
	INFO_PRINTF1(_L("CTestString::strptimebasic"));
	TInt len=0;
	TPtrC string;
	_LIT( Kstring1, "Parameter1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	TBuf8<100> buf1,buf2,buf3,buf4,buf5;
	buf1.Copy(string);
	char* src = (char*) buf1.Ptr();
	len=buf1.Length();
	src[len]='\0';

	_LIT( Kstring2, "Parameter2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	buf2.Copy(string);
	char * fch=(char *)buf2.Ptr();
	len=buf2.Length();
	fch[len]='\0';

	_LIT( Kstring3, "Parameter3" );
	res = GetStringFromConfig(ConfigSection(), Kstring3, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	buf3.Copy(string);
	char * loc=(char *)buf3.Ptr();
	len=buf3.Length();
	loc[len]='\0';
	
	_LIT( Kstring4, "Parameter4" );
	res = GetStringFromConfig(ConfigSection(), Kstring4, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	buf4.Copy(string);
	char * ival=(char *)buf4.Ptr();
	len=buf4.Length();
	ival[len]='\0';


	_LIT( Kstring5, "Parameter5" );
	res = GetStringFromConfig(ConfigSection(), Kstring5, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	buf5.Copy(string);
	char * tmval=(char *)buf5.Ptr();
	len=buf5.Length();
	tmval[len]='\0';

	char arg1[100] = {'\0'};
	char arg2[100] = { '%'};

	strcat(arg1,src);
	strcat(arg2,fch);
	int tm_item_val=atoi(ival);
	int tm_item=atoi(tmval);

	struct tm * t=(struct tm *) new tm;
	char * locale = setlocale(LC_TIME,loc);
	if(locale == NULL)
		{
		INFO_PRINTF1(_L("setlocale failed"));
		delete t;
		return KErrNone;	
		}

#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	char * endp=strptime(src,fch,t);
#else
	char * endp=strptime(src,fch,t);

#endif

	switch(tm_item)
		{
		case 0:
			if(t->tm_sec==tm_item_val)
				{
				res=KErrNone;
				}
			break;
		case 1: 
			if(t->tm_min==tm_item_val)
				{
				res=KErrNone;
				}
			break;
		case 2:
			if(t->tm_hour==tm_item_val)
				{
				res=KErrNone;
				}
			break;
		case 3:
			if(t->tm_mday==tm_item_val)
				{
				res=KErrNone;
				}
			break;
		case 4: 
			if(t->tm_mon==tm_item_val) 
				{
				res=KErrNone;
				}
			break;
		case 5: 
			if(t->tm_year==tm_item_val)
				{
				res=KErrNone;
				}
			break;
		case 6: 
			if(t->tm_wday==tm_item_val)
				{
				res=KErrNone;
				}
			break;
		case 7: 
			if(t->tm_yday==tm_item_val)
					{
					res=KErrNone;
					}
			break;
		case 8:
			if(t->tm_isdst==tm_item_val)
					{
					res=KErrNone;
					}
			break;
		case 9:
			if(t->tm_gmtoff==tm_item_val)
					{
					res=KErrNone;
					}
			break;
		}

	if(t != NULL)
		{	
		delete t;
		t = NULL;	
		}	

	return res;
	}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strftimebasic( )
	{

	INFO_PRINTF1(_L("CTestString::strftimebasic"));

	TInt ret=KErrGeneral;
	TInt len=0;
	TPtrC string;
	_LIT( Kstring1, "Parameter1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return ret ;
		}
	TBuf8<100> buf1,buf2,buf3,buf4,buf5;
	buf1.Copy(string);
	char* src = (char*) buf1.Ptr();
	len=buf1.Length();
	src[len]='\0';

	_LIT( Kstring2, "Parameter2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return ret ;
		}
	buf2.Copy(string);
	char * fch=(char *)buf2.Ptr();
	len=buf2.Length();
	fch[len]='\0';

	_LIT( Kstring3, "Parameter3" );
	res = GetStringFromConfig(ConfigSection(), Kstring3, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return ret ;
		}
	buf3.Copy(string);
	char * loc=(char *)buf3.Ptr();
	len=buf3.Length();
	loc[len]='\0';

	_LIT( Kstring4, "Parameter4" );
	res = GetStringFromConfig(ConfigSection(), Kstring4, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return ret ;
		}
	buf4.Copy(string);
	char * ival=(char *)buf4.Ptr();
	len=buf4.Length();
	ival[len]='\0';

	_LIT( Kstring5, "Parameter5" );
	res = GetStringFromConfig(ConfigSection(), Kstring5, string);
	if(!res)  
		{
		_LIT(Kerr , "Failed to time parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return ret ;
		}
	buf5.Copy(string);
	char * tmval=(char *)buf5.Ptr();
	len=buf5.Length();
	tmval[len]='\0';
	char sol[100] = {'\0'};
	char arg1[100]=	{'\0'};
	char arg2[100] = {'\0'};
	arg2[0] = '%';
	INFO_PRINTF1(_L("Arg successfully parsed"));
	strcat(arg1,src);
	strcat(arg2,fch);
	int tm_item_val=atoi(ival);
	int tm_item=atoi(tmval);

	struct tm * t=(struct tm *) new tm;

	if( t == NULL)
		{
		INFO_PRINTF1(_L("tm structure failed "));
		return -1;		
		}

	char * locale = setlocale(LC_TIME,loc);
	if(locale == NULL)
		{
		INFO_PRINTF1(_L("setlocale failed"));
		delete t;
		return KErrNone;	
		}

	switch(tm_item)
		{
		case 0: 
			t->tm_sec=tm_item_val;
			break;
		case 1: 
			t->tm_min=tm_item_val;
			break;
		case 2: 
			t->tm_hour=tm_item_val;
			break;
		case 3: 
			t->tm_mday=tm_item_val;
			break;
		case 4:
			t->tm_mon=tm_item_val;
			break;
		case 5: 
			t->tm_year=tm_item_val;
			break;
		case 6:
			t->tm_wday=tm_item_val;
			break;
		case 7:
			t->tm_yday=tm_item_val;
			break;
		case 8: 
			t->tm_isdst=tm_item_val;
			break;
		case 9: 
			t->tm_gmtoff=tm_item_val;
			break;
		}
	
	INFO_PRINTF1(_L("strftime called"));
	int cnt=strftime(sol,100,arg2,t);

	INFO_PRINTF2(_L("cnt = %d"),cnt);
	INFO_PRINTF1(_L("strcmp called"));

	if(!strcasecmp(sol,arg1)) 
		{
		ret=KErrNone;
		}
	if(t != NULL)
		{
		delete t;
		t = NULL;	
		}
	return ret;

	}


//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strptime_arabic()
	{

	TInt res=KErrNone;

	char * locale = setlocale(LC_TIME,"ar_AE.UTF-8");
	if(locale == NULL)
		{
		INFO_PRINTF1(_L("setlocale failed"));
		return KErrNone;	
		}

	struct tm * t=(struct tm *) new tm;

	char * endp= strptime("\xd8\xa7\xd8\xab","%a",t);

	if(t->tm_wday!=1)
		{
		res=KErrGeneral;
		}

	if(t != NULL)
		{
		delete t;
		t = NULL;	
		}
	return res;

}

//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strptime_heutf( )
	{

	TInt res=KErrNone;

	char * locale = setlocale(LC_TIME,"he_IL.UTF-8");
	if(locale == NULL)
		{
		INFO_PRINTF1(_L("setlocale failed"));
		return KErrNone;	
		}

	struct tm * t=(struct tm *) new tm;

	char * endp= strptime("\xd7\x99\xd7\xa0\xd7\x95\x2e","%b",t);

	if(t->tm_mon!=0)
		{
		res=KErrGeneral;
		}

	if(t != NULL)
		{
		delete t;
		t = NULL;	
		}
	return res;

	}


//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strftime_arabic( )
	{

	TInt res=KErrNone;
	char sol[100];

	char * locale = setlocale(LC_TIME,"ar_AE.UTF-8");
	if(locale == NULL)
		{
		INFO_PRINTF1(_L("setlocale failed"));
		return KErrNone;	
		}

	struct tm * t=(struct tm *) new tm;

	t->tm_wday=1;

	int cnt= strftime(sol,100,"%a",t);

	if(strcmp(sol,"\xd8\xa7\xd8\xab"))
		{
		res=KErrGeneral;
		}

	if(t != NULL)
		{
		delete t;
		t = NULL;	
		}
	return res;
	}


//----------------------To Do Documentation---------------------------------------
	
TInt CTestString::strftime_heutf( )
	{

	TInt res=KErrNone;
	char sol[100];

	char * locale = setlocale(LC_TIME,"he_IL.UTF-8");
	if(locale == NULL)
		{
		INFO_PRINTF1(_L("setlocale failed"));
		return KErrNone;	
		}

	struct tm * t=(struct tm *) new tm;

	t->tm_mon=0;

	int cnt= strftime(sol,100,"%b",t);

	if(strcmp(sol,"\xd7\x99\xd7\xa0\xd7\x95\x2e"))
		{
		res=KErrGeneral;
		}
	if(t != NULL)
		{
		delete t;
		t = NULL;	
		}
	return res;
	}
TInt CTestString::strptimebasic1()
	{
	
	struct tm *t=(struct tm *) new tm;
	
	if (strptime("12:33:45", "%H:%M:%S", t) == NULL)
		{
		delete t;
		return KErrGeneral;
		}


	INFO_PRINTF4(_L("hour: %d min: %d sec %d"),t->tm_hour,t->tm_min,t->tm_sec);

	delete t;
	return KErrNone;	
	}
	
TInt CTestString::Testmemchr_specialchars()
	{
	char * pch;
  	char str[] = "Hei p \xe5 deg";
  	int d = '\xe5';// special character to be looked for
  	INFO_PRINTF2(_L("the character being looked for is %c"), d);
  	pch = (char*) memchr ((void *)str, d, sizeof(str));
  	if (pch != NULL)
  		{
  		INFO_PRINTF2(_L("d found at position %d.\n"), pch-str+1);
  		}
  	else
  		{
  		INFO_PRINTF1(_L("d not found.\n"));
  		return (NULL);
  		}    	
  	return KErrNone;
	}

TInt CTestString :: strftime_timezone()
	{
	struct tm tm;
	int x;
    //initializing the time parameter.
	time_t t = time(NULL);
	if (t == (time_t)-1)
	    {
	    ERR_PRINTF1(_L("time() function failed"));
	    return KErrGeneral;
	    }
    char buf[1024];    		
    memset((void*)buf,'\0',sizeof(buf));
 	tm = *localtime(&t);
	tm.tm_zone = NULL;
	x = strftime(buf, sizeof(buf), "%Z", &tm);
    INFO_PRINTF2(_L("value returned = %d\n"),x);
	if(x!=3)
	{
	return KErrGeneral;
	}
	return KErrNone;

	}

TInt CTestString::strlcatBasictest()
	{
	char ch[50];
	int retval;
	int ret= KErrNone;
	//STEP 1
	(void) strcpy (ch, "ijk");
	
	retval = strlcat(ch,"lmn",sizeof("lmn"));
	if(retval==6)
		{
		ret = KErrNone;
		}
	else 
		{
		ret = KErrGeneral;
		}
	return ret;
	}

TInt CTestString::ffsbasic1()
	{
	TInt res=KErrNone;
	int i=0;
	int j=ffs(i);
	if(j!=0)
		{
		res=KErrGeneral;
		}
	return res;
	}

TInt CTestString::TestStrncasecmp()
	{
	TInt res=KErrNone;
	if(strncasecmp("ABC","abc",(size_t)0))
		{
		res=KErrGeneral;
		}
	return res;
	}

TInt CTestString::TestStrncasecmp1()
	{
	TInt res=KErrNone;
	if(strncasecmp("\0","\0",(size_t)5))
		{
		res=KErrGeneral;
		}
	return res;
	}
TInt CTestString::strncattest1()
	{
	char ch[50];
	int retval;
	
	//STEP 1
	(void) strcpy (ch, "ijk");
	if(!strcmp(strncat (ch, "lmn", 0),ch)) 	/* Returned value. */
		{
		retval = KErrNone;
		}
	else 
		{
		retval = KErrGeneral;
		}
	return retval;
	}

TInt CTestString::bcopyfuncheck1()
	{
	char ch[50];
	strcpy(ch,"abc");
	bcopy("abc",ch,3);
	return compare(ch,"abc");
	}
TInt CTestString::strlcpyBasictest()
	{
	char ch[50];
	int retval;
	int ret= KErrNone;
	//STEP 1
	(void) strcpy (ch, "ijk");
	
	retval = strlcpy(ch,"lmn",(size_t)0);
	if(retval==3)
		{
		ret = KErrNone;
		}
	else 
		{
		ret = KErrGeneral;
		}
	return ret;
	}
TInt CTestString::strlcpyBasictest1()
	{
	char ch[50];
	int retval;
	int ret= KErrNone;
	//STEP 1
	(void) strcpy (ch, "ijk");
	
	retval = strlcpy(ch,"lmn",(size_t)1);
	if(retval==3)
		{
		ret = KErrNone;
		}
	else 
		{
		ret = KErrGeneral;
		}
	return ret;
	}
TInt CTestString::strlcatBasictest1()
	{
	char ch[50];
	int retval;
	int ret= KErrNone;
	//STEP 1
	(void) strcpy (ch, "ijk");
	
	retval = strlcat(ch,"lmn",(size_t)0);
	if(retval==3)
		{
		ret = KErrNone;
		}
	else 
		{
		ret = KErrGeneral;
		}
	return ret;
	}
TInt CTestString::strxfrmbasic1()
	{
	TInt res=KErrGeneral;
		
    char dst1[20] = {'\0'};
    char src1[20] = "abc";

    int r = strxfrm(dst1,src1,0);
    if(r == strlen(src1))
    	{
    	INFO_PRINTF1(_L("strxfrm successful"));
    	res = KErrNone;
    	}
    
    return res;
	}

TInt CTestString::TestStrcasestr1()
	{
	INFO_PRINTF1(_L("Test strcasestr1()"));
	
	
	char* str1 = "this is a string of characters";
  	char* str2 = "";
  	char* result = strcasestr( str1, str2 );
  	if( result == NULL ) 
  		{
  		INFO_PRINTF1(_L("Test failed"));
  		return KErrGeneral;
  		}
  	else
  		{
  		if(!strcmp(str1,result))
  			{
  			INFO_PRINTF1(_L("Test passed"));
  			return KErrNone;
  			}
  		return KErrGeneral;
  		} 
	}
TInt CTestString::TestStrnstr1()
	{
	INFO_PRINTF1(_L("Test strnstr1()"));
	
	const char *largestring = "\0";
	const char *smallstring = "Bar";
	char *ptr;

	ptr = strnstr(largestring, smallstring,(size_t)3);
	
	if(ptr == NULL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	}

TInt CTestString::TestStrnstr2()
	{
	INFO_PRINTF1(_L("Test strnstr2()"));
	
	const char *largestring = "This is an example String";
	const char *smallstring = "revanth";
	char *ptr;

	ptr = strnstr(largestring, smallstring,(size_t)27);
	
	if(ptr == NULL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	}

TInt CTestString::TestStrnstr3()
	{
	INFO_PRINTF1(_L("Test strnstr3()"));
	
	const char *largestring = "This";
	const char *smallstring = "";
	char *ptr;

	ptr = strnstr(largestring, smallstring,(size_t)27);
	
	if(!strcmp(ptr,"This"))
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	}

TInt CTestString::TestMemccpy1()
	{
	INFO_PRINTF1(_L("Test memccpy1()"));
	
	char string1[60] = "The quick brown dog jumps over the lazy fox";
	char *buffer=NULL;
   	int pdest;

	pdest = (int)memccpy( buffer, string1, 's', 0 );
	if(pdest==0 && buffer==NULL)
		{	
		return KErrNone;
		}
	else 
		{
		return KErrGeneral;
		}
	}

TInt CTestString::TstWcsxfrm1()
	{
	wchar_t str1[MIN_LEN];
	wchar_t str3[MIN_LEN];
		
	if (mbstowcs(str1, "\0", MIN_LEN) == (size_t)-1)
		{
		ERR_PRINTF1(_L("Error :mbstowcs()"));
		return KErrGeneral;
		}
	
	int result1 = wcsxfrm(str3, str1, MIN_LEN);
	if(result1==0)
		{
		return KErrNone;	
		}
	else
		{
		return KErrGeneral;
		}
	}
TInt CTestString::TstWcsxfrm2()
	{
	wchar_t str1[MIN_LEN];
	wchar_t str3[MIN_LEN];
		
	if (mbstowcs(str1, "\0", MIN_LEN) == (size_t)-1)
		{
		ERR_PRINTF1(_L("Error :mbstowcs()"));
		return KErrGeneral;
		}
	
	int result1 = wcsxfrm(str3, str1, (size_t)0);
	if(result1==0)
		{
		return KErrNone;	
		}
	else
		{
		return KErrGeneral;
		}
	}
TInt CTestString::TstWcsxfrm3()
	{
	wchar_t str1[MIN_LEN];
	wchar_t str3[MIN_LEN];
		
	if (mbstowcs(str1, "abc", MIN_LEN) == (size_t)-1)
		{
		ERR_PRINTF1(_L("Error :mbstowcs()"));
		return KErrGeneral;
		}
	
	int result1 = wcsxfrm(str3, str1, (size_t)2);
	if(result1==3)
		{
		return KErrNone;	
		}
	else
		{
		return KErrGeneral;
		}
	}

TInt CTestString::TstStrdup2()	
	{
	char* dest;
	

	dest = strdup("Helloadjskl;jfklasdjfaklfjaklsfjaskldjfasklfjaklsjfklasfjaklsdjfaskdlfjklasdfjaklsjfklasjfkalsdjfklasdjfaklsdjfaklsfjaklsfjaklsjfaklsdfjaklsfjaklsdjkflasdjfklajsfklasjdfklajsdfkljasdklfjaslkfjakl;sdfjalskdfjaskl;dfjaskl;dfjaskld;fjadskl;fjas;jffjasdklfjaklsd;jfakl;sdHelloadjskl;jfklasdjfaklfjaklsfjaskldjfasklfjaklsjfklasfjaklsdjfaskdlfjklasdfjaklsjfklasjfkalsdjfklasdjfaklsdjfaklsfjaklsfjaklsjfaklsdfjaklsfjaklsdjkflasdjfklajsfklasjdfklajsdfkljasdklfjaslkfjakl;sdfjalskdfjaskl;dfjaskl;dfjaskld;fjadskl;fjas;jffjasdklfjaklsd;jfakl;sdHelloadjskl;jfklasdjfaklfjaklsfjaskldjfasklfjaklsjfklasfjaklsdjfaskdlfjklasdfjaklsjfklasjfkalsdjfklasdjfaklsdjfaklsfjaklsfjaklsjfaklsdfjaklsfjaklsdjkflasdjfklajsfklasjdfklajsdfkljasdklfjaslkfjakl;sdfjalskdfjaskl;dfjaskl;dfjaskld;fjadskl;fjas;jffjasdklfjaklsd;jfakl;sdHelloadjskl;jfklasdjfaklfjaklsfjaskldjfasklfjaklsjfklasfjaklsdjfaskdlfjklasdfjaklsjfklasjfkalsdjfklasdjfaklsdjfaklsfjaklsfjaklsjfaklsdfjaklsfjaklsdjkflasdjfklajsfklasjdfklajsdfkljasdklfjaslkfjakl;sdfjalskdfjaskl;dfjaskl;dfjaskld;fjadskl;fjas;jffjasdklfjaklsd;jfakl;sd");
	
	int ret = validate(strcmp(dest,"Helloadjskl;jfklasdjfaklfjaklsfjaskldjfasklfjaklsjfklasfjaklsdjfaskdlfjklasdfjaklsjfklasjfkalsdjfklasdjfaklsdjfaklsfjaklsfjaklsjfaklsdfjaklsfjaklsdjkflasdjfklajsfklasjdfklajsdfkljasdklfjaslkfjakl;sdfjalskdfjaskl;dfjaskl;dfjaskld;fjadskl;fjas;jffjasdklfjaklsd;jfakl;sdHelloadjskl;jfklasdjfaklfjaklsfjaskldjfasklfjaklsjfklasfjaklsdjfaskdlfjklasdfjaklsjfklasjfkalsdjfklasdjfaklsdjfaklsfjaklsfjaklsjfaklsdfjaklsfjaklsdjkflasdjfklajsfklasjdfklajsdfkljasdklfjaslkfjakl;sdfjalskdfjaskl;dfjaskl;dfjaskld;fjadskl;fjas;jffjasdklfjaklsd;jfakl;sdHelloadjskl;jfklasdjfaklfjaklsfjaskldjfasklfjaklsjfklasfjaklsdjfaskdlfjklasdfjaklsjfklasjfkalsdjfklasdjfaklsdjfaklsfjaklsfjaklsjfaklsdfjaklsfjaklsdjkflasdjfklajsfklasjdfklajsdfkljasdklfjaslkfjakl;sdfjalskdfjaskl;dfjaskl;dfjaskld;fjadskl;fjas;jffjasdklfjaklsd;jfakl;sdHelloadjskl;jfklasdjfaklfjaklsfjaskldjfasklfjaklsjfklasfjaklsdjfaskdlfjklasdfjaklsjfklasjfkalsdjfklasdjfaklsdjfaklsfjaklsfjaklsjfaklsdfjaklsfjaklsdjkflasdjfklajsfklasjdfklajsdfkljasdklfjaslkfjakl;sdfjalskdfjaskl;dfjaskl;dfjaskld;fjadskl;fjas;jffjasdklfjaklsd;jfakl;sd")==0);
	
	if (dest == NULL)
		{
		if (errno == ENOMEM)
			{
			ERR_PRINTF1(_L("Insufficient memory"));
			}
		return KErrGeneral;
		}
	else if(ret==KErrNone)
		{
		INFO_PRINTF1(_L("strdup() success"));
		}
	free(dest);
	return ret;
	}

TInt CTestString::TstStrerr_r()	
	{
	
	char msgbuf[256];
	int errnum=95;
	int ret = strerror_r(errnum, msgbuf, sizeof(msgbuf));
	
	if (ret == EINVAL)
		{
		INFO_PRINTF1(_L("strerr_r() success"));
		return KErrNone;
		}
	else 
		{
		INFO_PRINTF1(_L("strerr_r() failure"));
		return KErrGeneral;
		}
	}
TInt CTestString::TstStrerr_r1()	
	{
	
	char msgbuf[256];
	int errnum=10;
	int ret = strerror_r(errnum, msgbuf, (size_t)0);
	
	if (ret == ERANGE)
		{
		INFO_PRINTF1(_L("strerr_r() success"));
		return KErrNone;
		}
	else 
		{
		INFO_PRINTF1(_L("strerr_r() failure"));
		return KErrGeneral;
		}
	}
TInt CTestString::TestStrcasestr2()
	{
	char ch[50];
	(void) strcpy(ch, "abcd");
	return validate(strcasestr(ch, "bc") == ch+1);	/* Basic test. */
	}
TInt CTestString::TestStrnstr4()
	{
	char ch[50];
	(void) strcpy(ch, "abcd");
	return validate(strnstr(ch, "abcde",6) == NULL);	/* Basic test. */

	}
TInt CTestString::TestStrptime_test()
	{
	char *ret = setlocale(LC_ALL,"en_GB.ISO-8859-1");
	if(ret == NULL)
		{
		INFO_PRINTF1(_L("setlocale failed"));
		return KErrGeneral;
		}
	struct tm * t=(struct tm *) new tm;
	int err = 0;
	char * endp;
	endp=strptime("19","%C",t);
	if(t->tm_year!=0)
		{
		err += 1;
		}
	endp=strptime("mon","%c",t);
	endp=strptime("01/01/84","%D",t);
	if(t->tm_mday!=1 && t->tm_mon != 0 && t->tm_year != 84)
		{
		err += 1;
		}
	endp=strptime("1984","%EY",t);
	if(t->tm_year != 84)
		{
		err += 1;
		}
	endp=strptime("10","%OI",t);
	if(t->tm_hour != 10)
		{
		err += 1;
		}
	endp=strptime("1984-01-01","%F",t);	
	if(t->tm_mday!=1 && t->tm_mon != 0 && t->tm_year != 84)
		{
		err += 1;
		}
	endp=strptime("10:10","%R",t);	
	if(t->tm_hour != 10 && t->tm_min != 10)
		{
		err += 1;
		}
	endp=strptime("10:10:10","%T",t);	
	if(t->tm_hour != 10 && t->tm_min != 10 && t->tm_sec != 10 )
		{
		err += 1;
		}	
	endp=strptime("100","%j",t);
	if(t->tm_yday != 99)
		{
		err += 1;
		}	
	endp=strptime("mon","%A",t);
	if(t->tm_wday!=1)
		{
		err += 1;
		}
	endp=strptime("50","%U",t);
	if(endp == NULL)
		{
		err += 1;
		}	
	endp=strptime("50","%W",t);
	if(endp == NULL)
		{
		err += 1;
		}
	endp=strptime("1","%w",t);
	if(t->tm_wday!=1)
		{
		err += 1;
		}
	endp=strptime("20","%e",t);
	if(t->tm_mday!=20)
		{
		err += 1;
		}
	endp=strptime("jan","%B",t);
	if(t->tm_mon!=0)
		{
		err += 1;
		}
	endp=strptime("mar","%h",t);
	if(t->tm_mon!=2)
		{
		err += 1;
		}
	endp=strptime("10","%s",t);
	if(endp==NULL)
		{
		err += 1;
		}
	endp=strptime("10","%I",t);
	if(t->tm_hour!=10)
		{
		err += 1;
		}
	endp=strptime("10","%k",t);
	if(t->tm_hour!=10)
		{
		err += 1;
		}
	endp=strptime("10","%l",t);
	if(t->tm_hour!=10)
		{
		err += 1;
		}
	endp=strptime("9:10:10 am","%r",t);
	if(t->tm_hour!=9 && t->tm_min!=10 && t->tm_sec!=10)
		{
		err += 1;
		}	
	endp=strptime("10:10:10 am","%X",t);
	if(t->tm_hour!=10 && t->tm_min!=10 && t->tm_sec!=10)
		{
		err += 1;
		}
	endp=strptime("10/01/1990","%x",t);
	if(t->tm_mday!=1 && t->tm_mon!=9 && t->tm_year!=90)
		{
		err += 1;
		}
	endp=strptime("am","%p",t);
	if(endp == NULL)
		{
		err += 1;
		}
	endp=strptime("GMT","%Z",t);
	if(t->tm_yday!=364 && t->tm_isdst!=0 && t->tm_gmtoff!=0)
		{
		err += 1;
		}
	if(t != NULL)
		{	
		delete t;
		t = NULL;	
		}
	if(err != 0)
		return KErrGeneral;
	return KErrNone;
	}

TInt CTestString::TestStrptime_test1()
	{
	TInt ret = KErrGeneral;
	INFO_PRINTF1(_L("TestStrptime_test1 begins"));
	struct tm buf;
    char outbuf[10];
    time_t tt = time(NULL);
    if (tt == (time_t)-1)
    	{
    	ERR_PRINTF1(_L("time() function failed"));
    	return ret;
    	}
    buf = *localtime(&tt);
    buf.tm_zone = NULL;/*tm_zone set to NULL explicitly*/
    if (strftime(outbuf, 10, "%Z", &buf) == 0)
    	{
    	ERR_PRINTF1(_L("strftime() function failed"));
    	return ret;
    	}
    ret = KErrNone;
    TPtrC8 outbufstring((TText8*)outbuf, strlen(outbuf));
  	TBuf16<100> outbufname;
  	outbufname.Copy(outbufstring);
  	INFO_PRINTF2(_L("strftime() sets default time zone as : %S"), &outbufname);
  	INFO_PRINTF1(_L("TestStrptime_test1 ends"));
    return ret;
	}


