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


#ifndef __TTESTTOOLENGINE_H_
#define __TTESTTOOLENGINE_H_

#include <e32base.h>
#include <unifiedcertstore.h>
#include <unifiedkeystore.h>
#include <test/testexecutestepbase.h>
#include <ct/rmpointerarray.h>
#include <securitydefs.h>

class CTestToolEngine : public CActive
	{
public:
	static CTestToolEngine* NewLC();
	~CTestToolEngine();
	void ConstructL();	
		
public: 
	TVerdict ListCerts(const TDesC& aLabel, const TInt aExpectedNumLabel);
	TVerdict ListCerts(const TDesC& aLabel1, const TDesC& aLabel2, const TInt aExpectedNumLabel);
	TVerdict ListCerts(RPointerArray<HBufC>& aCerts);
	TVerdict ListCerts(RPointerArray<HBufC>& aCerts, const TDesC& aLabel);
	TVerdict ListCerts(RPointerArray<HBufC>& aCerts, TInt aStore);
	TVerdict ListStore(RPointerArray<HBufC>& aCerts);
	TVerdict ListKeys(const TDesC& aLabel, const TInt aExpectedNumLabel);
	TVerdict ListKeys(const TDesC& aLabel1, const TDesC& aLabel2, const TInt aExpectedNumLabel);
	TVerdict ListKeys(RPointerArray<HBufC>& aKeys);
	TVerdict ListKeys(RPointerArray<HBufC>& aKeys, TInt aStore);
	TVerdict CheckApps(RPointerArray<HBufC>& aApps, const TDesC& aLabel1);
	TVerdict CheckTrust(const TDesC& aLabel,TBool aTrust);
	TVerdict GetPolicy(const TDesC& aLabel1, const TDesC& aLabel2, TInt aUser);
	TVerdict ListStoreKey(RPointerArray<HBufC>& aKeys);
	void Initilize();
	void KeyInitilize();
	TBool VerifyApps(RPointerArray<HBufC>& aApps);
	TBool VerifyTrust(TBool aTrust);
	TBool VerifyPolicy(TInt aUser);
	TBool CheckAppForUid(const TDesC& aAppName, RPointerArray<HBufC>& aApps);
	TBool CompareWithExpectedCertificates(RPointerArray<HBufC>& aCerts);
	TBool CompareWithExpectedStore(RPointerArray<HBufC>& aCerts);
	TBool CompareWithExpectedStoreKey(RPointerArray<HBufC>& aKeys);
	TBool CompareWithExpectedKeys(RPointerArray<HBufC>& aKeys);
	void FilterCerts(RMPointerArray<CCTCertInfo>& aCertList, TCertificateOwnerType& aOwnerType);
	TInt FilterCerts(RMPointerArray<CCTCertInfo>& aCertList, TDesC& aLabel, TInt& aIndex, TBool aFilterOnly = EFalse);
	TInt FilterKeys(RMPointerArray<CCTKeyInfo>& aKeyList, TDesC& aLabel, TInt& aIndex);
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CTestToolEngine();
			
private:
	enum 
		{		
		EInitialiseKeyStore,
		EFilterKey,
		EInitialise,
		EGetApps,
		EGetTrust,
		EGetPolicy,
		EFilter,
		EDone
		} iState;
		
	RFs iFs;
	CUnifiedCertStore* iCertStore;
	TPtrC iLabel1;
	TPtrC iLabel2;
	TInt iNumCert1;
	TInt iNumCert2;
	RFs iFsKeyStore;
	CUnifiedKeyStore* iKeyStore;
	RMPointerArray<CCTCertInfo> iCertInfos;
	RMPointerArray<CCTKeyInfo> iKeys;
	TInt iAppsCount;
	TInt iCertsCount;
	TInt iKeysListCount;
	RArray<TUid> iUids;
	TInt iStore;
	CCertAttributeFilter* iFilter;
	TCTKeyAttributeFilter iKeyFilter;
	TBool iPolicy;
	TInt iIndex;
	TInt iGetTrust;
	TBool iActualTrust;
	};


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

#endif //__TTESTTOOLENGINE_H_


