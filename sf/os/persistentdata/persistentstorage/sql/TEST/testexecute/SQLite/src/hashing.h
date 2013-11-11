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
//
 
#if (!defined __TEFSQL_HASHING_H__)
#define __TEFSQL_HASHING_H__

#include <e32base.h>
#include <e32hashtab.h>

class CSQLHashUtil : public CBase
    {
public:
    CSQLHashUtil(){};
    ~CSQLHashUtil();
    TPtrC* GetStringFromNum(TInt aErrNum);
    TInt GetNumFromString(TPtrC aErrMsg);
protected:
    RHashMap<TInt, TPtrC> ihash;
    };
class CSQLOsErrHash : public CSQLHashUtil
    {
public:
    CSQLOsErrHash();
    };
class CSQLErrHash : public CSQLOsErrHash
    {
public:
    CSQLErrHash();
    };
class CSQLColTypeHash : public CSQLHashUtil
    {
public:
    CSQLColTypeHash();
    };
class CSQLTEFAction : public CSQLHashUtil
    {
public:
    CSQLTEFAction();
    };
class CSQLSFSTEFAction : public CSQLHashUtil
    {
public:
    CSQLSFSTEFAction();
	enum { ESFS_SelectIntL, ESFS_SelectInt64L, ESFS_SelectRealL,
		   ESFS_SelectTextL, ESFS_SelectBinaryL, };
	};
class CSQLCapability : public CSQLHashUtil
    {
public:
    CSQLCapability();
    };
class CSQLObject : public CSQLHashUtil
    {
public:
    CSQLObject();
    };

class CSQLPolicy : public CSQLHashUtil
	{
public:
	CSQLPolicy();
	};

#endif  // __TEFSQL_HASHING_H__

