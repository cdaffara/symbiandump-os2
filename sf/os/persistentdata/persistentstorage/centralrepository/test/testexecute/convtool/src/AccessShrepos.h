// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __ACCESSSHREPOS_H__
#define __ACCESSSHREPOS_H__

#include <e32std.h>
#include "shrepos.h"
#include "setting.h"

// A fried of CSharedRepository to access private
// members of CSharedRepository
class TConvToolTester
	{
public:
	inline TConvToolTester(CSharedRepository& aRep);
	inline TUint32 DefaultMeta() const;
	inline const RDefaultMetaArray& DefaultMetaArray() const;
	inline const RSettingsArray& Settings() const;
	inline const TUid& Owner() const;
	inline const TTime& TimeStamp() const;
	inline const RPointerArray<TSettingsAccessPolicy>& SinglePolicies() const;
	inline const RRangePolicyArray& RangePolicies() const;
	inline const TSecurityPolicy& DefaultReadPolicy() const;
	inline const TSecurityPolicy& DefaultWritePolicy() const;
	inline void SetMetaDataOnCreate(TServerSetting& aSetting);
	
private:
	CSharedRepository& iSharedRep;
	};

#include "AccessShrepos.inl"

#endif //__ACCESSSHREPOS_H__
