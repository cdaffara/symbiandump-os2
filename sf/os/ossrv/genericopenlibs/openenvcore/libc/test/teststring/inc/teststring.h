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



#ifndef __TESTSTRING_H__
#define __TESTSTRING_H__

#include <test/testexecutestepbase.h>

_LIT(KMemCpyBasicTest, "memcpyBasicTest");
_LIT(KMemCpyTestZeroLengthCopy, "memcpyTestZeroLengthCopy");

_LIT(KMemMoveBasicTest, "memmoveBasictest");
_LIT(KMemMoveZeroLength, "memmoveZeroLength");
_LIT(KMemMoveOverlap, "memmoveOverlap");

_LIT(KStrcpyBasictest, "strcpyBasictest");
_LIT(KStrcpyWriteOver, "strcpyWriteOver");
_LIT(KStrcpyBoundry, "strcpyBoundry");

_LIT(KStrncpyBasictest, "strncpyBasictest");
_LIT(KStrncpyCutCnt, "strncpyCutCnt");
_LIT(KStrncpyCutNul, "strncpyCutNul");
_LIT(KStrncpyNulInc, "strncpyNulInc");
_LIT(KStrncpyZeroLen, "strncpyZeroLen");
_LIT(KStrncpyZeroSrc, "strncpyZeroSrc");

_LIT(KStrcatBasictest, "strcatBasictest");
_LIT(KStrcatBoundry, "strcatBoundry");
_LIT(KStrcatMemset, "strcatMemset");


_LIT(KStrncatBasictest, "strncatBasictest");
_LIT(KStrncatBoundry, "strncatBoundry");
_LIT(KStrncatMemset, "strncatMemset");

_LIT(KMemcmpBasictest, "memcmpBasictest");
_LIT(KMemcmpUnequal, "memcmpUnequal");
_LIT(KMemcmpLtdCount, "memcmpLtdCount");
_LIT(KMemcmpZeroCount, "memcmpZeroCount");

_LIT(KStrcmpBasictest, "strcmpBasictest");
_LIT(KStrcmpMisMatchStrAndLen, "strcmpMisMatchStrAndLen");

_LIT(KStrncmpBasictest, "strncmpBasictest");
_LIT(KStrncmpMisMatchStrAndLen, "strncmpMisMatchStrAndLen");
_LIT(KStrncmpCountCheck, "strncmpCountCheck");

_LIT(KStrcollBasictest, "strcollBasictest");

_LIT(KSrxfrmBasictest, "srxfrmBasictest");

_LIT(KmemchrNotFound, "memchrNotFound");
_LIT(KmemchrBasictest, "memchrBasictest");
_LIT(KmemchrFindNul, "memchrFindNul");
_LIT(KmemchrZeroCount, "memchrZeroCount");

_LIT(KstrchrNotFound, "strchrNotFound");
_LIT(KstrchrBasicTest, "strchrBasicTest");
_LIT(KstrchrFindNul, "strchrFindNul");
_LIT(KstrchrFindFirst, "strchrFindFirst");
_LIT(KstrchrEmptyString, "strchrEmptyString");
_LIT(KstrchrNulEmptyStr, "strchrNulEmptyStr");

_LIT(KstrcspnWholeStr, "strcspnWholeStr");
_LIT(KstrcspnPartString, "strcspnPartString");
_LIT(KstrcspnNone, "strcspnNone");
_LIT(KstrcspnNullString, "strcspnNullString");
_LIT(KstrcspnNullSearch, "strcspnNullSearch");

_LIT(KstrpbrkNotFound, "strpbrkNotFound");
_LIT(KstrpbrkBasictest, "strpbrkBasictest");
_LIT(KstrpbrkEmptySearch, "strpbrkEmptySearch");
_LIT(KstrpbrkEmptyString, "strpbrkEmptyString");
_LIT(KstrpbrkBothEmpty, "strpbrkBothEmpty");
_LIT(KstrpbrkFindFirst, "strpbrkFindFirst");

_LIT(KstrrchrNotfound, "strrchrNotfound");
_LIT(KstrrchrBasictest, "strrchrBasictest");
_LIT(KstrrchrFindNul, "strrchrFindNul");
_LIT(KstrrchrFindLast, "strrchrFindLast");
_LIT(KstrrchrEmptyStr, "strrchrEmptyStr");
_LIT(KstrrchrEmptyNul, "strrchrEmptyNul");

_LIT(KstrspnWholeStr, "strspnWholeStr");
_LIT(KstrspnPartial, "strspnPartial");
_LIT(KstrspnNone, "strspnNone");
_LIT(KstrspnNulStr, "strspnNulStr");
_LIT(KstrspnNulSearch, "strspnNulSearch");

_LIT(KstrstrNotFound, "strstrNotFound");
_LIT(KstrstrBasictest, "strstrBasictest");
_LIT(KstrstrLong, "strstrLong");
_LIT(KstrstrFindEmpty, "strstrFindEmpty");
_LIT(KstrstrEmptyStr, "strstrEmptyStr");
_LIT(KstrstrEmpty, "strstrEmpty");

_LIT(KstrtokBasictest, "strtokBasictest");
_LIT(KstrtokDelim, "strtokDelim");
_LIT(KstrtokChangeDelim, "strtokChangeDelim");
_LIT(KstrtokNewStr, "strtokNewStr");
_LIT(KstrtokDiffSap, "strtokDiffSap");
_LIT(KstrtokNoTok, "strtokNoTok");
_LIT(KstrtokEmptyStr, "strtokEmptyStr");
_LIT(KstrtokNoDelim, "strtokNoDelim");
_LIT(KstrtokEmptyDelim, "strtokEmptyDelim");

_LIT(KmemsetBasictest, "memsetBasictest");
_LIT(KmemsetZeroLen, "memsetZeroLen");

_LIT(KstrerrorBasictest, "strerrorBasictest");
_LIT(KstrerrorUnknown, "strerrorUnknown");


_LIT(KstrlenEmpty, "strlenEmpty");
_LIT(KstrlenSingleChar, "strlenSingleChar");
_LIT(KstrlenMultiChar, "strlenMultiChar");

_LIT(Kbcmpsameargs, "bcmpsameargs");
_LIT(Kbcmpdiffargs, "bcmpdiffargs");
_LIT(Kbcmpzerocnt, "bcmpzerocnt");

_LIT(Kbcopyfuncheck, "bcopyfuncheck");
_LIT(Kbcopybasic, "bcopybasic");
_LIT(Kbcopyzerocopy, "bcopyzerocopy");
_LIT(Kbcopysrcconst, "bcopysrcconst");
_LIT(Kbcopynullsrc, "bcopynullsrc");
_LIT(Kbcopyoverlap, "bcopyoverlap");

_LIT(Kstrsepbasic, "strsepbasic");
_LIT(Kstrsepsrccheck, "strsepsrccheck");

_LIT(Kbzerobasic, "bzerobasic");
_LIT(Kbzeronocopy, "bzeronocopy");

_LIT(Kffsbasic, "ffsbasic");

_LIT(Kindexbasic, "indexbasic");
_LIT(Kindexnotfound, "indexnotfound");
_LIT(Kindexend, "indexend");
_LIT(Kindexbeginning, "indexbeginning");
_LIT(Kindexfindnull, "indexfindnull");
_LIT(Kindexnullsrc, "indexnullsrc");

_LIT(Krindexbasic, "rindexbasic");
_LIT(Krindexnotfound, "rindexnotfound");
_LIT(Krindexend, "rindexend");
_LIT(Krindexbeginning, "rindexbeginning");
_LIT(Krindexfindnull, "rindexfindnull");
_LIT(Krindexnullsrc, "rindexnullsrc");

_LIT(Kmemcpybasic, "memcpybasic");
_LIT(Kmemcpyzerolength, "memcpyzerolength");
_LIT(Kmemcpysrcconstantness, "memcpysrcconstantness");

_LIT(Kstpcpybasic, "stpcpybasic");

_LIT(Kstrcasecmpcasecheck, "strcasecmpcasecheck");
_LIT(Kstrcasecmplargesrc, "strcasecmplargesrc");
_LIT(Kstrcasecmpsmallsrc, "strcasecmpsmallsrc");

_LIT(Kstrncmpbasic, "strncmpbasic");
_LIT(Kstrncmplargesrc, "strncmplargesrc");
_LIT(Kstrncmpsmallsrc, "strncmpsmallsrc");

_LIT(Kstrcollbasic, "strcollbasic");
_LIT(Kstrcolllargesrc, "strcolllargesrc");
_LIT(Kstrcollsmallsrc, "strcollsmallsrc");

_LIT(Kstrndupbasic, "strndupbasic");
_LIT(Kstrnduplargecnt, "strnduplargecnt");
_LIT(Kstrndupequalcnt, "strndupequalcnt");
_LIT(Kstrndupzerocnt, "strndupzerocnt");
_LIT(Kstrndupnullsrc, "strndupnullsrc");

_LIT(Kstrnlenbasic, "strnlenbasic");
_LIT(Kstrnlensmallstr, "strnlensmallstr");
_LIT(Kstrnlennullstr, "strnlennullstr");

_LIT(Kstrtofbasic, "strtofbasic");
_LIT(Kstrtofexpntn, "strtofexpntn");
_LIT(Kstrtofsignedexpntn, "strtofsignedexpntn");
_LIT(Kstrtofinfinity, "strtofinfinity");
_LIT(Kstrtofnan, "strtofnan");

_LIT(Kstrtoimaxbasic, "strtoimaxbasic");
_LIT(Kstrtoimaxspacecheck, "strtoimaxspacecheck");
_LIT(Kstrtoimaxsignedsrc, "strtoimaxsignedsrc");
_LIT(Kstrtoimaxoctalcheck, "strtoimaxoctalcheck");
_LIT(Kstrtoimaxhexcheck, "strtoimaxhexcheck");

_LIT(Kstrtoumaxbasic, "strtoumaxbasic");
_LIT(Kstrtoumaxspacecheck, "strtoumaxspacecheck");
_LIT(Kstrtoumaxoctalcheck, "strtoumaxoctalcheck");
_LIT(Kstrtoumaxhexcheck, "strtoumaxhexcheck");
_LIT(Kstrtoldbasic, "strtoldbasic");
_LIT(Kstrtoldexpntn, "strtoldexpntn");
_LIT(Kstrtoldsignedexpntn, "strtoldsignedexpntn");
_LIT(Kstrtoldinfinity, "strtoldinfinity");
_LIT(Kstrtoldnan, "strtoldnan");
_LIT(Kstrtollbasic, "strtollbasic");
_LIT(Kstrtollspacecheck, "strtollspacecheck");
_LIT(Kstrtollsignedsrc, "strtollsignedsrc");
_LIT(Kstrtolloctalcheck, "strtolloctalcheck");
_LIT(Kstrtollhexcheck, "strtollhexcheck");
_LIT(Kstrtoullbasic, "strtoullbasic");
_LIT(Kstrtoullspacecheck, "strtoullspacecheck");
_LIT(Kstrtoulloctalcheck, "strtoulloctalcheck");
_LIT(Kstrtoullhexcheck, "strtoullhexcheck");
_LIT(Kswabbasic, "swabbasic");
_LIT(Kstrtoqbasic,"strtoqbasic");
_LIT(Kstrtoqspacecheck,"strtoqspacecheck");
_LIT(Kstrtoqsignedsrc,"strtoqsignedsrc");
_LIT(Kstrtoqoctalcheck,"strtoqoctalcheck");
_LIT(Kstrtoqhexcheck,"strtoqhexcheck");
_LIT(Kstrtouqbasic,"strtouqbasic");
_LIT(Kstrtouqspacecheck,"strtouqspacecheck");
_LIT(Kstrtouqoctalcheck,"strtouqoctalcheck");
_LIT(Kstrtouqhexcheck,"strtouqhexcheck");	
_LIT(Kstrxfrmbasic,"strxfrmbasic");
_LIT(Kstrptimebasic,"strptimebasic");
_LIT(Kstrptime_arabic,"strptime_arabic");
_LIT(Kstrftimebasic,"strftimebasic");
_LIT(Kstrptime_heutf,"strptime_heutf");
_LIT(Kstrftime_arabic,"strftime_arabic");
_LIT(Kstrftime_heutf,"strftime_heutf");
_LIT(Kstrftime_timezone,"strftime_timezone");

_LIT(KTestStrtok_r,"TestStrtok_r");
_LIT(KTestStrnstr,"TestStrnstr");
_LIT(KTestStrcasestr,"TestStrcasestr");
_LIT(KTestMemccpy,"TestMemccpy");
_LIT(KTestStpcpy,"TestStpcpy");
_LIT(KTestWcsColl,"TestWcsColl");
_LIT(KTstWcsCollUnlikestr1,"TstWcsCollUnlikestr1");
_LIT(KTstWcsCollUnlikestr2,"TstWcsCollUnlikestr2");
_LIT(KTstWcsxfrm,"TstWcsxfrm");
_LIT(KTstWcsxfrmErr,"TstWcsxfrmErr");
_LIT(KTstStrdup,"TstStrdup");
_LIT(KTstStrndup,"TstStrndup");
_LIT(KTstWcsdup,"TstWcsdup");
_LIT(KTstStrsep,"TstStrsep");
//////////
_LIT(Ktestprg1,"testprg1");
_LIT(Kstrptimebasic1,"strptimebasic1");
_LIT(KTstldexp,"Tstldexp");
_LIT(KTestmemchr_specialchars,"Testmemchr_specialchars");

_LIT(KTstStrdup1,"TstStrdup1");
_LIT(KStrlcatBasictest, "strlcatBasictest");
_LIT(KTestStrcasestr1,"TestStrcasestr1");
_LIT(Kffsbasic1, "ffsbasic1");
_LIT(KTestStrnstr1,"TestStrnstr1");
_LIT(KTestStrnstr2,"TestStrnstr2");
_LIT(KTestStrnstr3,"TestStrnstr3");
_LIT(KTestStrncasecmp,"TestStrncasecmp");
_LIT(KTestStrncasecmp1,"TestStrncasecmp1");
_LIT(KTestMemccpy1,"TestMemccpy1");
_LIT(KStrncattest1, "strncattest1");
_LIT(Kbcopyfuncheck1, "bcopyfuncheck1");
_LIT(KStrlcpyBasictest, "strlcpyBasictest");
_LIT(KStrlcpyBasictest1, "strlcpyBasictest1");
_LIT(KStrlcatBasictest1, "strlcatBasictest1");
_LIT(KTstWcsxfrm1,"TstWcsxfrm1");
_LIT(KTstWcsxfrm2,"TstWcsxfrm2");
_LIT(KTstWcsxfrm3,"TstWcsxfrm3");
_LIT(KTstStrdup2,"TstStrdup2");
_LIT(KTstStrerr_r,"TstStrerr_r");
_LIT(KTstStrerr_r1,"TstStrerr_r1");
_LIT(KTestStrcasestr2,"TestStrcasestr2");
_LIT(KTestStrnstr4,"TestStrnstr4");
_LIT(Kstrxfrmbasic1,"strxfrmbasic1");
_LIT(KTestStrptime_test,"TestStrptime_test");
_LIT(KTestStrptime_test1,"TestStrptime_test1");

class CTestString : public CTestStep
	{
public:
	~CTestString();
	CTestString(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt compare (const char *a, const char *b);
	TInt validate (int thing);
	TInt memcpyTestZeroLengthCopy();
	TInt memcpyBasicTest();
	
	TInt memmoveBasictest();
	TInt memmoveZeroLength();
	TInt memmoveOverlap();
	
	TInt strcpyBasictest();
	TInt strcpyWriteOver();
	TInt strcpyBoundry();
	
	TInt strncpyBasictest();
	TInt strncpyCutCnt();
	TInt strncpyCutNul();
	TInt strncpyNulInc();
	TInt strncpyZeroLen();
	TInt strncpyZeroSrc();
	
	TInt strcatBasictest();
	TInt strcatBoundry();
	TInt strcatMemset();


	TInt strncatBasictest();
	TInt strncatBoundry();
	TInt strncatMemset();
	
	TInt memcmpBasictest();
	TInt memcmpUnequal();
	TInt memcmpLtdCount();
	TInt memcmpZeroCount();

	TInt strcmpBasictest();
	TInt strcmpMisMatchStrAndLen();
	
	TInt strncmpBasictest();
	TInt strncmpMisMatchStrAndLen();
	TInt strncmpCountCheck();
	
	TInt strcollBasictest();

	TInt srxfrmBasictest();
	
	TInt memchrNotFound();
	TInt memchrBasictest();
	TInt memchrFindNul();
	TInt memchrZeroCount();

	TInt strchrNotFound();
	TInt strchrBasicTest();
	TInt strchrFindNul();
	TInt strchrFindFirst();
	TInt strchrEmptyString();
	TInt strchrNulEmptyStr();

	TInt strcspnWholeStr();
	TInt strcspnPartString();
	TInt strcspnNone();
	TInt strcspnNullString();
	TInt strcspnNullSearch();

	TInt strpbrkNotFound();
	TInt strpbrkBasictest();
	TInt strpbrkEmptySearch();
	TInt strpbrkEmptyString();
	TInt strpbrkBothEmpty();
	TInt strpbrkFindFirst();

	TInt strrchrNotfound();
	TInt strrchrBasictest();
	TInt strrchrFindNul();
	TInt strrchrFindLast();
	TInt strrchrEmptyStr();
	TInt strrchrEmptyNul();

	TInt strspnWholeStr();
	TInt strspnPartial();
	TInt strspnNone();
	TInt strspnNulStr();
	TInt strspnNulSearch();

	TInt strstrNotFound();
	TInt strstrBasictest();
	TInt strstrLong();
	TInt strstrFindEmpty();
	TInt strstrEmptyStr();
	TInt strstrEmpty();

	TInt strtokBasictest();
	TInt strtokDelim();
	TInt strtokChangeDelim();
	TInt strtokNewStr();
	TInt strtokDiffSap();
	TInt strtokNoTok();
	TInt strtokEmptyStr();
	TInt strtokNoDelim();
	TInt strtokEmptyDelim();

	TInt memsetBasictest();
	TInt memsetZeroLen();

	TInt strerrorBasictest();
	TInt strerrorUnknown();

	TInt strlenEmpty();
	TInt strlenSingleChar();
	TInt strlenMultiChar();

	TInt bcmpsameargs();				
	TInt bcmpdiffargs();				
	TInt bcmpzerocnt();				

	TInt bcopyfuncheck();				
	TInt bcopybasic();				
	TInt bcopyzerocopy();				
	TInt bcopysrcconst();
	TInt bcopynullsrc();
	TInt bcopyoverlap();

	TInt strsepbasic();
	TInt strsepsrccheck();

	TInt bzerobasic();
	TInt bzeronocopy();

	TInt ffsbasic();

	TInt indexbasic();
	TInt indexnotfound();
	TInt indexend();
	TInt indexbeginning();
	TInt indexfindnull();
	TInt indexnullsrc();

	TInt rindexbasic();
	TInt rindexnotfound();
	TInt rindexend();
	TInt rindexbeginning();
	TInt rindexfindnull();
	TInt rindexnullsrc();
	
	TInt memcpybasic();
	TInt memcpyzerolength();
	TInt memcpysrcconstantness();
	
	TInt stpcpybasic();
	
	TInt strcasecmpcasecheck();
	TInt strcasecmplargesrc();
	TInt strcasecmpsmallsrc();
			
	TInt strncmpbasic();
	TInt strncmplargesrc();
	TInt strncmpsmallsrc();
	
	TInt strcollbasic();
	TInt strcolllargesrc();
	TInt strcollsmallsrc();
	
	TInt strndupbasic();
	TInt strnduplargecnt();
	TInt strndupequalcnt();
	TInt strndupzerocnt();
	TInt strndupnullsrc();
	
	TInt strnlenbasic();
	TInt strnlensmallstr();
	TInt strnlennullstr();
	
	TInt strtofbasic();
	TInt strtofexpntn();
	TInt strtofsignedexpntn();
	TInt strtofinfinity();
	TInt strtofnan();

	TInt strtoimaxbasic();
	TInt strtoimaxspacecheck();
	TInt strtoimaxsignedsrc();
	TInt strtoimaxoctalcheck();
	TInt strtoimaxhexcheck();
	
	TInt strtoumaxbasic();
	TInt strtoumaxspacecheck();
	TInt strtoumaxoctalcheck();
	TInt strtoumaxhexcheck();
	
	TInt strtoldbasic();
	TInt strtoldexpntn();
	TInt strtoldsignedexpntn();
	TInt strtoldinfinity();
	TInt strtoldnan();
	
	TInt strtollbasic();
	TInt strtollspacecheck();
	TInt strtollsignedsrc();
	TInt strtolloctalcheck();
	TInt strtollhexcheck();

	TInt strtoullbasic();
	TInt strtoullspacecheck();
	TInt strtoulloctalcheck();
	TInt strtoullhexcheck();
	
	TInt swabbasic();
	
	TInt strtoqbasic();
	TInt strtoqspacecheck();
	TInt strtoqsignedsrc();
	TInt strtoqoctalcheck();
	TInt strtoqhexcheck();
	
	TInt strtouqbasic();
	TInt strtouqspacecheck();
	TInt strtouqoctalcheck();
	TInt strtouqhexcheck();	

	TInt strxfrmbasic();	
	
	TInt strptimebasic();
	TInt strftimebasic(); 
	TInt strptime_arabic();
	TInt strptime_heutf();
	TInt strftime_arabic();
	TInt strftime_heutf(); 
	TInt strftime_timezone(); 
	TInt TestStrtok_r();
	TInt TestStrnstr();
	TInt TestStrcasestr();
	TInt TestMemccpy();
	TInt TestStpcpy();
	TInt TestWcsColl();
	TInt TstWcsCollUnlikestr1();
	TInt TstWcsCollUnlikestr2();
	TInt TstWcsxfrm();
	TInt TstWcsxfrmErr();
	TInt TstStrdup();
	TInt TstStrndup();
	TInt TstWcsdup();
	TInt TstStrsep();
	/////////
	TInt testprg1(); 
	TInt strptimebasic1();
	TInt Tstldexp();
	TInt Testmemchr_specialchars();
	TInt strlcatBasictest();
	TInt TestStrcasestr1();
	TInt ffsbasic1();
	TInt TestStrnstr1();
	TInt TestStrnstr2();
	TInt TestStrnstr3();
	TInt TestStrncasecmp();
	TInt TestStrncasecmp1();
	TInt TestMemccpy1();
	TInt strncattest1();
	TInt bcopyfuncheck1();
	TInt strlcpyBasictest();
	TInt strlcpyBasictest1();
	TInt strlcatBasictest1();
	TInt TstWcsxfrm1();
	TInt TstWcsxfrm2();
	TInt TstWcsxfrm3();
	TInt TstStrdup2();
	TInt TstStrerr_r();
	TInt TstStrerr_r1();
	TInt TestStrcasestr2();
	TInt TestStrnstr4();
	TInt strxfrmbasic1();
	TInt TestStrptime_test();
	TInt TestStrptime_test1();
	};

#endif 
