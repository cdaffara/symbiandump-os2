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



#ifndef __TESTLINK_H__
#define __TESTLINK_H__

#include <test/testexecutestepbase.h>
#include <e32svr.h>
#include "Tlink.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

_LIT(KLink, "Link");
_LIT(KLink1, "Link1");
_LIT(KLink2, "Link2");
_LIT(KLink3, "Link3");
_LIT(KLink4, "Link4");
_LIT(KLink5, "Link5");
_LIT(KLink6, "Link6");
_LIT(KLink7, "Link7");
_LIT(KReadWriteLink, "ReadWriteLink");
_LIT(KReadLink, "ReadLink");
_LIT(KReadLink1, "ReadLink1");
_LIT(KUnlink, "Unlink");
_LIT(KUnlink1, "Unlink1");
_LIT(Klinkstat, "linkstat");
_LIT(KTestUnlinkFifo, "TestUnlinkFifo");
_LIT(KReadLinkTest, "ReadLinkTest");

class CTestLink : public CTestStep
	{
public:
	~CTestLink(); 
	CTestLink(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	int ReadPathName(TPtrC& String);
	int ReadIntParam(TInt &buflen);
	TInt Link() ;
    TInt Link1();
    TInt Link2();
    TInt Link3();
    TInt Link4();
    TInt Link5();
    TInt Link6();
    TInt Link7();
    TInt ReadWriteLink();
    TInt ReadLink();
    TInt ReadLink1();
    TInt linkstat();
    TInt Unlink();
    TInt Unlink1();
    TInt TestUnlinkFifo();
    TInt ReadLinkTest();
public:
	TInt iParamCnt;
	};
#endif 
