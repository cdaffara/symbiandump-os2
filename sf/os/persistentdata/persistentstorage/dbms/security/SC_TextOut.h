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
// CPDTextPersister
// 
//

#ifndef __SC_TEXTOUT_H__
#define __SC_TEXTOUT_H__

#include <f32file.h>
#include "SC_Defs.h"
#include "SC_Policy.h"

namespace DBSC
{

/**
CPDTextPersister class is an implementation of MPolicyDomainPersister interface and is used
for storing security policies to a text policy file. The class uses TPolicyDomainReader
class functionality for retrieving the policies of the controlled by TPolicyDomainReader
instance CPolicyDomain collection.
@see MPolicyDomainPersister
@see TPolicyDomainReader
@see CPolicyBase::RPolicyCollection
@see CPolicyDomain
@internalComponent
*/
class CPDTextPersister : public CBase, public MPolicyDomainPersister
	{
public:
	static CPDTextPersister* NewLC(RFs& aFs, const TDesC& aTextFileName);
	virtual ~CPDTextPersister();
	virtual void RunL(const TPolicyDomainReader& aPolicyDomainReader);

private:
	inline CPDTextPersister();
	void ConstructL(RFs& aFs, const TDesC& aTextFileName);
	void WriteDbPolicyL(const TPolicyDomainReader& aPolicyDomainReader);
	void WriteTblPoliciesL(const TPolicyDomainReader& aPolicyDomainReader);
	void WritePolicyCollection(const CPolicyBase::RPolicyCollection& aPolicyCollection);

private:
	RFile					iOutFile;
	TBuf8<KMaxStmtLength>	iStmt;

	};

} //end of - namespace DBSC

#endif//__SC_TEXTOUT_H__
