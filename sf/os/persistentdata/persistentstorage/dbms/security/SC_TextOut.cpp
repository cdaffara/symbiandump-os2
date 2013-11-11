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
// CPDTextPersister class
// 
//

#include "SC_Strings.h"
#include "SC_TextOut.h"

namespace DBSC
{

/**
Non-working implementation!
There is no way to create text security poliicy representation from a TSecurityInfo instance.
*/
inline CPDTextPersister::CPDTextPersister()
	{
	}

/**
Non-working implementation!
There is no way to create text security poliicy representation from a TSecurityInfo instance.
*/
CPDTextPersister* CPDTextPersister::NewLC(RFs& aFs, const TDesC& aTextFileName)
	{
	CPDTextPersister* self = new (ELeave) CPDTextPersister;
	CleanupStack::PushL(self);
	self->ConstructL(aFs, aTextFileName);
	return self;
	}

/**
Non-working implementation!
There is no way to create text security poliicy representation from a TSecurityInfo instance.
*/
void CPDTextPersister::ConstructL(RFs& aFs, const TDesC& aTextFileName)
	{
	__LEAVE_IF_ERROR(iOutFile.Create(aFs, aTextFileName, EFileRead | EFileWrite));
	}

/**
Non-working implementation!
There is no way to create text security poliicy representation from a TSecurityInfo instance.
*/
CPDTextPersister::~CPDTextPersister()
	{
	iOutFile.Close();
	}

/**
Non-working implementation!
There is no way to create text security poliicy representation from a TSecurityInfo instance.
*/
void CPDTextPersister::RunL(const TPolicyDomainReader& aPolicyDomainReader)
	{
	WriteDbPolicyL(aPolicyDomainReader);
	WriteTblPoliciesL(aPolicyDomainReader);
	}

/**
Non-working implementation!
There is no way to create text security poliicy representation from a TSecurityInfo instance.
*/
void CPDTextPersister::WriteDbPolicyL(const TPolicyDomainReader& aPolicyDomainReader)
	{
	const CDbPolicy& dbPolicy = aPolicyDomainReader.DbPolicy();

	iStmt.Copy(KDbStr);
	iStmt.Append(KCrLf);
	__LEAVE_IF_ERROR(iOutFile.Write(iStmt));

	WritePolicyCollection(dbPolicy.PolicyCollection());
	}

/**
Non-working implementation!
There is no way to create text security poliicy representation from a TSecurityInfo instance.
*/
void CPDTextPersister::WriteTblPoliciesL(const TPolicyDomainReader& aPolicyDomainReader)
	{
	aPolicyDomainReader.ResetTblPos();
	const CTblPolicy* tblPolicy;
	while((tblPolicy = aPolicyDomainReader.NextTblPolicy()) != NULL)
		{
		iStmt.Copy(KTblStr);
		iStmt.Append(KCrLf);
		__LEAVE_IF_ERROR(iOutFile.Write(iStmt));

		iStmt.Copy(KNameStr);
		iStmt.Append(KEqStr);
		iStmt.Append(tblPolicy->TableName());
		iStmt.Append(KCrLf);
		__LEAVE_IF_ERROR(iOutFile.Write(iStmt));

		WritePolicyCollection(tblPolicy->PolicyCollection());
		}
	}

/**
Non-working implementation!
There is no way to create text security poliicy representation from a TSecurityInfo instance.
*/
void CPDTextPersister::WritePolicyCollection(const CPolicyBase::RPolicyCollection& aPolicyCollection)
	{
	TInt cnt = aPolicyCollection.Count();
	for(TInt i=0;i<cnt;++i)
		{
		const CPolicyBase::TPolicy& policy = aPolicyCollection[i];
		switch(policy.iType)
			{
			case EPTRead:
				iStmt.Copy(KReadStr);
				break;
			case EPTWrite:
				iStmt.Copy(KWriteStr);
				break;
			case EPTSchema:
				iStmt.Copy(KSchemaStr);
				break;
			default:
				__ASSERT(0);
				break;
			}
		iStmt.Append(KEqStr);
		}
	__ASSERT(0);
	}

} //end of - namespace DBSC
