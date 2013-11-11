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
// TPDStreamLoader class
// 
//

#include <s32file.h>
#include "SC_Defs.h"
#include "SC_StrmIn.h"

namespace DBSC
{

/**
MPolicyDomainLoader::RunL() implementation, which is used to load security policies
from a binary file, create the related security policy objects and add them
to CPolicyDomain instance, controlled by aPolicyDomainBuilder object.
It is not called directly, but will be called back.
@param aPolicyDomainBuilder TPolicyDomainBuilder instance, which will be used to add
       created security policy objects to the controlled by it collection. 
@leave System-wide error codes
*/
void TPDStreamLoader::RunL(TPolicyDomainBuilder& aPolicyDomainBuilder)
	{
	//Load policy file version
	TVersion fileVer;
	iStream >> fileVer.iMajor;
	iStream >> fileVer.iMinor;
	iStream >> fileVer.iBuild;
	//Check policy file version against the software version
	TInt fileVersion = CVERSION(fileVer.iMajor, fileVer.iMinor);
	if(KDBSCVersion < fileVersion)
		{
		__LEAVE(KErrNotSupported);
		}
	//
	CPolicyBase::RPolicyCollection policyColl;
	CleanupClosePushL(policyColl);
	//Load database policy
	LoadPolicyCollectionL(policyColl);
	CDbPolicy* dbPolicy = CDbPolicy::NewLC(policyColl);
	aPolicyDomainBuilder.SetDbPolicyL(dbPolicy);
	CleanupStack::Pop(dbPolicy);
	//Load all table policies
	TInt16 tblCnt;
	iStream >> tblCnt;
	for(TInt i=0;i<TInt(tblCnt);++i)
		{
		TBuf<KMaxFileName> tblName;
		iStream >> tblName;
		LoadPolicyCollectionL(policyColl);
		CTblPolicy* tblPolicy = CTblPolicy::NewLC(tblName, policyColl, dbPolicy);
		aPolicyDomainBuilder.AddTblPolicyL(tblPolicy);
		CleanupStack::Pop(tblPolicy);
		}
	CleanupStack::PopAndDestroy(&policyColl);
	//Load backup&restore SID (supported after version 1.1 (including 1.1)).
	if(fileVersion >= CVERSION(KDBSCMajorVersion1, KDBSCMinorVersion1_1))
		{
		TSecureId backupSID;
		iStream >> backupSID.iId;
		aPolicyDomainBuilder.SetBackupSID(backupSID);
		}
	}

/**
Loads security policies collection from a binary file
@param aPolicyColl A reference to a security policy collection object which has
       to be filled with the loaded security policies.
@leave System-wide error codes
*/
void TPDStreamLoader::LoadPolicyCollectionL(CPolicyBase::RPolicyCollection& aPolicyColl)
	{
	aPolicyColl.Reset();
	TInt8 cnt;
	iStream >> cnt;
	for(TInt i=0;i<TInt(cnt);++i)
		{
		CPolicyBase::TPolicy policy;
		TInt8 type;
		iStream >> type;
		policy.iType = static_cast <TPolicyType> (type);
		TBuf8<sizeof(TSecurityPolicy)> buf;
		iStream >> buf;
		__LEAVE_IF_ERROR(policy.iData.Set(buf));
		__LEAVE_IF_ERROR(aPolicyColl.Append(policy));
		}
	}

} //end of - namespace DBSC
