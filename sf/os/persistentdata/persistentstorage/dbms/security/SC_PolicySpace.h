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
// CPolicySpace class
// 
//

#ifndef __SC_POLICYSPACE_H__
#define __SC_POLICYSPACE_H__

#include "D32Map.h"
#include "SC_Policy.h"

namespace DBSC
{

//Forward declarations
class CPassAllPolicy;

/**
CPolicySpace class implements MPolicySpace interface and manages a set of 
CPolicyDomain objects (one per binary policy file).
All requests for particular MPOlicy implementations go through CPolicySpace object.
The DBMS server has to create only one CPolicySpace instance during the DBMS startup.
@internalComponent
*/
NONSHARABLE_CLASS(CPolicySpace) : public CBase, public MPolicySpace
	{
public:
	static CPolicySpace* NewL(RFs& aFs, const TDesC& aPrivatePath);
	virtual ~CPolicySpace();
	virtual void Release();
	virtual const MPolicy* DbPolicyL(const TDbPolicyRequest& aDbPolicyRequest) const;
	virtual const MPolicy* TblPolicyL(const TDbPolicyRequest& aDbPolicyRequest, const TDesC& aTblName) const;
	virtual TSecureId BackupSIDL(TUid aDbUid) const;

private:
	inline CPolicySpace();
	void ConstructL(RFs& aFs, const TDesC& aPrivatePath);
	void LoadPolicyDomainsL(RFs& aFs, const TDesC& aPolicyDir);
	CPolicyDomain* PolicyDomain(TUid aDomainUid) const;

private:
	CPassAllPolicy*				iPassAllDbPolicy;
	CPassAllPolicy*				iPassAllTblPolicy;
	RMap<TInt, CPolicyDomain*>	iPDCollection;

	};

} //end of - namespace DBSC

#endif//__SC_POLICYSPACE_H__
