/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef __COMPILEDSECURITYPOLICY_H_
#define __COMPILEDSECURITYPOLICY_H_

#include <e32cmn.h>

/**
TCompiledSecurityPolicy is a test class used for accessing private data members
of TSecurityPolicy class. It uses the fact that TSecurityPolicy class has a friend class, 
which name is TCompiledSecurityPolicy.
@internalComponent
*/
class TCompiledSecurityPolicy
	{
public:
	TCompiledSecurityPolicy(const TSecurityPolicy& aSecurityPolicy) :
        iSecurityPolicy(aSecurityPolicy)
		{
		}
	TSecurityPolicy::TType Type() const
        {
		return static_cast <TSecurityPolicy::TType> (iSecurityPolicy.iType);
		}
	TUint32 SecureId() const
        {
        return iSecurityPolicy.iSecureId;
        }
    TUint32 VendorId() const
        {
        return iSecurityPolicy.iVendorId;
	    }
    TCapability Capability(TInt aIndex) const
		{
		if(aIndex < 3)
			{
		    return static_cast <TCapability> (iSecurityPolicy.iCaps[aIndex]);
		    }
		else if(aIndex < 7)
		    {
		    return static_cast <TCapability> (iSecurityPolicy.iExtraCaps[aIndex - 3]);
			}
		User::Invariant();
		return ECapability_None;
		}

private:
	const TSecurityPolicy& iSecurityPolicy;
};


inline TBool operator==(const TCompiledSecurityPolicy& aLeft, const TCompiledSecurityPolicy& aRight)
	{
    return aLeft.Type() == aRight.Type() && 
		aLeft.SecureId() == aRight.SecureId() &&
        aLeft.VendorId() == aRight.VendorId() &&
		aLeft.Capability(0) == aRight.Capability(0) &&
	    aLeft.Capability(1) == aRight.Capability(1) &&
	    aLeft.Capability(2) == aRight.Capability(2) &&
		aLeft.Capability(3) == aRight.Capability(3) &&
	    aLeft.Capability(4) == aRight.Capability(4) &&
        aLeft.Capability(5) == aRight.Capability(5) &&
		aLeft.Capability(6) == aRight.Capability(6);
	}

#endif // __COMPILEDSECURITYPOLICY_H_
