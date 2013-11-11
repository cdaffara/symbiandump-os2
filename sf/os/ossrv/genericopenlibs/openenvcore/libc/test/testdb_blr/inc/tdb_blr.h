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



#ifndef __TESTDB_BLR_H__
#define __TESTDB_BLR_H__

#include <test/testexecutestepbase.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/time.h>
#include <e32math.h>
#include <grp.h>
#include <pwd.h>

_LIT(Kgetgrgid_pos, "getgrgid_pos");
_LIT(Kgetgrgid_neg, "getgrgid_neg");
_LIT(Kgetgrnam_pos, "getgrnam_pos");
_LIT(Kgetgrnam_neg, "getgrnam_neg");
_LIT(Kgetgrgid_r_pos, "getgrgid_r_pos");
_LIT(Kgetgrgid_r_neg, "getgrgid_r_neg");
_LIT(Kgetgrnam_r_pos, "getgrnam_r_pos");
_LIT(Kgetgrnam_r_neg, "getgrnam_r_neg");
_LIT(Kgetgrent_combined, "getgrent_combined");
_LIT(Ksetgrent_combined1, "setgrent_combined1");

_LIT(Kgetpwuid_pos, "getpwuid_pos");
_LIT(Kgetpwnam_pos, "getpwnam_pos");
_LIT(Kgetpwuid_neg, "getpwuid_neg");
_LIT(Kgetpwnam_neg, "getpwnam_neg");		
_LIT(Kgetpwuid_r_pos, "getpwuid_r_pos");
_LIT(Kgetpwnam_r_pos, "getpwnam_r_pos");
_LIT(Kgetpwuid_r_neg, "getpwuid_r_neg");
_LIT(Kgetpwnam_r_neg, "getpwnam_r_neg");

_LIT(Kendgrent_test1, "endgrent_test1");
_LIT(Kgetpwent_test1, "getpwent_test1");
_LIT(Ksetpwent_test1, "setpwent_test1");
_LIT(Kendpwent_test1, "endpwent_test1");
class CTestDbBlr : public CTestStep
	{
public:
	~CTestDbBlr(); 
	CTestDbBlr(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
		
	// Utility functions for handling parameters etc.
	void ReadIntParam(TInt &aInt);
	void ReadStringParam(char* aString);
	TBool SameString(char *aString1, char *aString2);
	void StringToDes(char* aString, TDes &aDescriptor);
	void RandomizeString(char* aString, TInt aLength);
	TInt RandomInt(TInt aMaxValue);

    //Test methods                        
	TInt getgrgid_pos();
	TInt getgrgid_neg();
	TInt getgrnam_pos();
	TInt getgrnam_neg();
	TInt getgrgid_r_pos();
	TInt getgrgid_r_neg();
	TInt getgrnam_r_pos();
	TInt getgrnam_r_neg();
	TInt getgrent_combined();
	TInt setgrent_combined1();

	TInt getpwuid_pos();	
	TInt getpwnam_pos();
	TInt getpwuid_neg();	
	TInt getpwnam_neg();
	TInt getpwuid_r_pos();	
	TInt getpwnam_r_pos();
	TInt getpwuid_r_neg();	
	TInt getpwnam_r_neg();


	TInt endgrent_test1();
	TInt getpwent_test1();
	TInt setpwent_test1();
	TInt endpwent_test1();
	
private: //data
	//Input param cnt
	TInt iParamCnt;
    // Randomizer seed
    TInt64 iRandomizerSeed;
	};
#endif 
