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
// CPassAllPolicy class
// 
//

#ifndef __SC_PASSALLPOLICY_H__
#define __SC_PASSALLPOLICY_H__

#include <e32base.h>		//CBase
#include "D32Security.h"

namespace DBSC
{

/**
CPassAllPolicy class implements MPolicy interface.
It describes a "pass all" policy object and will be used for 
non-secure and legacy databases.
@internalComponent
*/
NONSHARABLE_CLASS(CPassAllPolicy) : public CBase, public MPolicy
	{
public:
	CPassAllPolicy(TPolicyObjType aPOType);
	virtual ~CPassAllPolicy();
	virtual TBool Check(const RMessage2&, TPolicyType) const;
	virtual TInt Get(TPolicyType, TSecurityPolicy& aPolicy) const;
	DECLARE_DB_DUMP2(aFile)

private:
	TPolicyObjType iPOType;

	};

} //end of - namespace DBSC

#endif//__SC_PASSALLPOLICY_H__
