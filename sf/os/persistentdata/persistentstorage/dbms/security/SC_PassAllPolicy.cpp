// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CPassAllPolicy class - "DBMS security" related code. Used in no_security environment or for
// non-secure databases
// 
//

#include "SC_PassAllPolicy.h"

namespace DBSC
{

CPassAllPolicy::CPassAllPolicy(TPolicyObjType aPOType) :
	iPOType(aPOType)
	{
	}

CPassAllPolicy::~CPassAllPolicy()
	{
	}

//The method returns always ETrue.
TBool CPassAllPolicy::Check(const RMessage2&, TPolicyType) const
	{
	return ETrue;
	}

//The method initializes aPolicy parameter with "EAlwaysPass" type policy instance and
//returns KErrNone.
TInt CPassAllPolicy::Get(TPolicyType, TSecurityPolicy& aPolicy) const
	{
	__ASSERT(0);
	aPolicy = TSecurityPolicy(TSecurityPolicy::EAlwaysPass);
	return KErrNone;
	}

#ifdef __DBDUMP__
void CPassAllPolicy::Dump(RFile& aFile) const
	{
	_LIT8(KClassName, "Class: CPassAllPolicy. this=%X");
	TBuf8<40> buf;
	_LIT8(KCrLf, "\r\n");
	_LIT8(KObjType, "Object: ");
	_LIT8(KDatabase, "Database");
	_LIT8(KTable, "Table");

	buf.Format(KClassName, this);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	(void)aFile.Write(KObjType);
	iPOType == EPOTDatabase ? (void)aFile.Write(KDatabase) : (void)aFile.Write(KTable);
	(void)aFile.Write(KCrLf);
	}
#endif

} //end of - namespace DBSC
