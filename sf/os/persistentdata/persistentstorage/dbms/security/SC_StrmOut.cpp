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
// TPDStreamPersister class
// 
//

#include <s32file.h>
#include "SC_Defs.h"
#include "SC_StrmOut.h"

namespace DBSC
{

/**
MPolicyDomainPersister::RunL() implementation, which is used to store security policies
to a binary file.
It is not called directly, but will be called back.
@param aPolicyDomainReader TPolicyDomainReader instance, which will be used to traverse
       security policy objects in the controlled by it collection. 
@leave System-wide error codes
*/
void TPDStreamPersister::RunL(const TPolicyDomainReader& aPolicyDomainReader)
	{
	//Write software version
	TVersion softVer(KDBSCMajorVersion, KDBSCMinorVersion, KDBSCBuild);
	iStream << softVer.iMajor;
	iStream << softVer.iMinor;
	iStream << softVer.iBuild;
	//Write database policy
	const CDbPolicy& dbPolicy = aPolicyDomainReader.DbPolicy();
	ExternalizePolicyCollectionL(dbPolicy.PolicyCollection());
	//Write table policies
	aPolicyDomainReader.ResetTblPos();
	iStream << TInt16(aPolicyDomainReader.TblPolicyCount());
	const CTblPolicy* tblPolicy;
	while((tblPolicy = aPolicyDomainReader.NextTblPolicy()) != NULL)
		{
		iStream << tblPolicy->TableName();
		ExternalizePolicyCollectionL(tblPolicy->PolicyCollection());
		}
	//Write backup&restore SID
	iStream << (TUint32)aPolicyDomainReader.BackupSID();
	}

/**
Stores security policies collection to a binary file
@param aPolicyCollection A const reference to a security policy collection object which has
       to be stored to a binary file.
@leave System-wide error codes
*/
void TPDStreamPersister::ExternalizePolicyCollectionL(const CPolicyBase::RPolicyCollection& aPolicyCollection)
	{
	TInt cnt = aPolicyCollection.Count();
	iStream << TInt8(cnt);
	for(TInt i=0;i<cnt;++i)
		{
		const CPolicyBase::TPolicy& policy = aPolicyCollection[i];
		iStream << TInt8(policy.iType);
		TPtrC8 packet = policy.iData.Package();
		iStream << packet;
		}
	}

} //end of - namespace DBSC
