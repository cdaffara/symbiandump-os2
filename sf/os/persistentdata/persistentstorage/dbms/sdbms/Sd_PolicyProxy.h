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
// CPolicyProxy class
// 
//

#ifndef __SD_POLICYPROXY_H__
#define __SD_POLICYPROXY_H__

#include <f32file.h>		//RFs
#include "D32Security.h"

using namespace DBSC;

/**
CPolicyProxy class describes an object, which encapsulates all DBMS security related
functionality, like policy loading and destroying, policy checking.
It helps also separating the real "DBMS security" code from fake implementations for
non-secure releases.
@internalComponent
*/
NONSHARABLE_CLASS(CPolicyProxy) : public CBase
	{
public:
	static CPolicyProxy* NewL(RFs& aFs,const TDesC& aPrivatePath);
	virtual ~CPolicyProxy();

	const MPolicy* DbPolicyL(const TDbPolicyRequest& aDbPolicyRequest);
	const MPolicy* TblPolicyL(const TDbPolicyRequest& aDbPolicyRequest, const TDesC& aTblName);
	const MPolicy* SqlPolicyL(const TDbPolicyRequest& aDbPolicyRequest, const TDesC& aSql, 
							  TPolicyType& aPolicyType);
	TSecureId BackupSIDL(TUid aDbUid) const;

	void CheckL(const RMessage2& aMessage, const MPolicy& aPolicy) const;
	void CheckL(TPolicyType aPolicyType, const RMessage2& aMessage, const MPolicy& aPolicy) const;

private:
	inline CPolicyProxy(RFs& aFs);
	void ConstructL(const TDesC& aPrivatePath);

private:
	RFs				iFs;
	MPolicySpace*	iPolicySpace;	//secure releases related
	MPolicy*		iPolicy;		//non-secure releases related

	};

#endif//__SD_POLICYPROXY_H__

