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


#include "ttesttoolengine.h"
#include <ccertattributefilter.h>
#include <mctcertstore.h>
#include <mctkeystore.h>

const TInt swinstalluid = 268452523;
const TInt swinstallocspuid = 268478646;
const TInt midletinstalluid = 270506792;
const TInt tlsuid = 268441661;
const TInt tocspuid = 268477445;


_LIT(KSWInstall, "SWInstall");
_LIT(KSWInstallOCSP, "SWInstallOCSP");
_LIT(KMidletInstall, "MidletInstall");
_LIT(KTls, "SSL/TLS");
_LIT(KTOcsp, "OCSP Test");

_LIT(KCaType, "ca");
_LIT(KUserType, "user");
_LIT(KPeerType, "peer");


/*static*/ CTestToolEngine* CTestToolEngine::NewLC()
	{
	CTestToolEngine* self = new (ELeave) CTestToolEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CTestToolEngine::CTestToolEngine() : CActive(EPriorityNormal)
	{
	iStore = -1;
	iPolicy =  EFalse;
	}

CTestToolEngine::~CTestToolEngine()
	{
	Cancel();	

	delete iCertStore;		
	delete iKeyStore;		

	delete iFilter;
	Cancel();
	iCertInfos.Close();
	iFsKeyStore.Close();
	iFs.Close();
	iUids.Reset();
	}
	
void CTestToolEngine::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	
	iCertStore = CUnifiedCertStore::NewL(iFs, ETrue);
	
	User::LeaveIfError(iFsKeyStore.Connect());	
	iKeyStore = CUnifiedKeyStore::NewL(iFsKeyStore);
	
	// This filter will accept any certificates
	iFilter = CCertAttributeFilter::NewL();
	
	CActiveScheduler::Add(this);	
	}

void CTestToolEngine::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());
		}
		
	switch (iState)
		{
		case EInitialiseKeyStore:
			{
			// Construct a catch-all filter!
			iKeyFilter.iKeyAlgorithm = CCTKeyInfo::EInvalidAlgorithm;
			iKeyFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;
			iKeyFilter.iUsage = EPKCS15UsageAll;
			if (iStore != -1)
				{
				MCTKeyStore& chosen = iKeyStore->KeyStore(iStore);
				chosen.List(iKeys, iKeyFilter, iStatus);
				}
			else
				{	
				iKeyStore->List(iKeys, iKeyFilter, iStatus);
				}
			if(iPolicy)
				{
				iState = EGetPolicy;
				}
			else if (iKeysListCount > 0)
				{
				iState = EDone;
				}
			else
				{
				iState = EFilterKey;	
				}
			SetActive();
			}
			break;
		case EFilterKey:
			{
			TInt index = 0;
			if (iLabel1.Length())    
				{
				iNumCert1 = FilterKeys(iKeys, iLabel1, index);					
				}
			if (iLabel2.Length())    
				{
				iNumCert2 = FilterKeys(iKeys, iLabel2, index);					
				}
			CActiveScheduler::Stop();	
			}
			break;	
		case EInitialise:
			{
			if (iStore != -1)
				{
				MCTCertStore& chosen = iCertStore->CertStore(iStore);
				chosen.List(iCertInfos, *iFilter, iStatus);
				}
			else
				{
				iCertStore->List(iCertInfos, *iFilter, iStatus);			
				}
			
			if(iAppsCount > 0)
				{
				iState = EGetApps;
				}
			else if (iGetTrust == 1)
				{
				iState = EGetTrust;
				}
			else if (iCertsCount > 0)
				{
				iState = EDone;
				}
			else
				{
				iState = EFilter;	
				}
			SetActive();
			}
			break;
		case EGetPolicy:
			{
			FilterKeys(iKeys, iLabel1, iIndex);
			CActiveScheduler::Stop();	
			}
			break;
		case EGetApps:
			{
			TInt index = 0;
			FilterCerts(iCertInfos, iLabel1, index);	
			iCertStore->Applications(*iCertInfos[index], iUids, iStatus);
			CActiveScheduler::Stop();
			}
			break;
		case EGetTrust:
			{
			TInt index = 0;
			FilterCerts(iCertInfos, iLabel1, index);	
			iCertStore->Trusted(*iCertInfos[index], iActualTrust, iStatus);
			CActiveScheduler::Stop();
			}
			break;
		case EFilter:
			{
			TInt index = 0;
			TBool filteronly = ETrue;
			if (iLabel1.Length())    
				{
				iNumCert1 = FilterCerts(iCertInfos, iLabel1, index, filteronly);					
				}
			if (iLabel2.Length())    
				{
				iNumCert2 = FilterCerts(iCertInfos, iLabel2, index, filteronly);					
				}
			CActiveScheduler::Stop();
			}
			break;
		case EDone:
			{
			CActiveScheduler::Stop();	
			}
			break;	
		default:
			{
			User::Panic(_L("Testtool Engine - Illegal state"), 0);
			}
		}
	}
	
TInt CTestToolEngine::RunError(TInt /*aError*/)
	{	
	CActiveScheduler::Stop();	
	
	return KErrNone;
	}

	
void CTestToolEngine::DoCancel()
	{
	//CActiveScheduler::Stop();
	}

TVerdict CTestToolEngine::ListCerts(const TDesC& aLabel1, const TInt aExpectedNumLabel)
	{
	TVerdict returnValue = EPass;
	iLabel1.Set(aLabel1);
	
	Initilize();
	if (iNumCert1 != aExpectedNumLabel)
		{
		returnValue = EFail;
		}
	return returnValue;
	}


TVerdict CTestToolEngine::ListCerts(const TDesC& aLabel1, const TDesC& aLabel2, const TInt aExpectedNumLabel)
	{
	TVerdict returnValue = EPass;
	iLabel1.Set(aLabel1);
	iLabel2.Set(aLabel2);
	
	Initilize();
	if (iNumCert1 != aExpectedNumLabel || iNumCert2 != aExpectedNumLabel)
		{
		returnValue = EFail;
		}
	return returnValue;
	}

TVerdict CTestToolEngine::ListStore(RPointerArray<HBufC>& aCerts)
	{
	TVerdict returnValue = EPass;
	iCertsCount = aCerts.Count();
	
	Initilize();
	
	if (!CompareWithExpectedStore(aCerts))
		{
		returnValue = EFail;
		}
	return returnValue;
	}
	
TVerdict CTestToolEngine::ListCerts(RPointerArray<HBufC>& aCerts)
	{
	TVerdict returnValue = EPass;
	iCertsCount = aCerts.Count();
	
	Initilize();
	
	if (!CompareWithExpectedCertificates(aCerts))
		{
		returnValue = EFail;
		}
	return returnValue;
	}

TVerdict CTestToolEngine::ListCerts(RPointerArray<HBufC>& aCerts, const TDesC& aLabel)
	{
	TVerdict returnValue = EPass;
	iLabel1.Set(aLabel);
	iCertsCount = aCerts.Count();
		
	Initilize();
	TCertificateOwnerType owner;
	if (iLabel1.CompareF(KCaType) == KErrNone) 
		{
		owner = ECACertificate;
		}
	else if (iLabel1.CompareF(KUserType) == KErrNone) 
		{
		owner = EUserCertificate;
		}
	else if (iLabel1.CompareF(KPeerType) == KErrNone) 
		{
		owner = EPeerCertificate;
		}
	FilterCerts(iCertInfos, owner);
	if (!CompareWithExpectedCertificates(aCerts))
		{
		returnValue = EFail;
		}
	return returnValue;
	}

TVerdict CTestToolEngine::ListCerts(RPointerArray<HBufC>& aCerts, TInt aStore)
	{
	TVerdict returnValue = EPass;
	iStore = aStore;
	iCertsCount = aCerts.Count();
	
	Initilize();
	if (!CompareWithExpectedCertificates(aCerts))
		{
		returnValue = EFail;
		}
	return returnValue;
	}
	
TVerdict CTestToolEngine::CheckApps(RPointerArray<HBufC>& aApps, const TDesC& aLabel)
	{
	TVerdict returnValue = EPass;
	iAppsCount = aApps.Count();
	iLabel1.Set(aLabel);
	Initilize();
	if (!VerifyApps(aApps))
		{
		returnValue = EFail;
		}
	return returnValue;
	}

TVerdict CTestToolEngine::CheckTrust(const TDesC& aLabel,TBool aTrust)
	{
	TVerdict returnValue = EPass;
	iLabel1.Set(aLabel);
	iGetTrust = 1;
	Initilize();
	if (!VerifyTrust(aTrust))
		{
		returnValue = EFail;
		}
	return returnValue;
	}

TVerdict CTestToolEngine::ListStoreKey(RPointerArray<HBufC>& aKeys)
	{
	TVerdict returnValue = EPass;
	iKeysListCount = aKeys.Count();
	
	KeyInitilize();
	
	if (!CompareWithExpectedStoreKey(aKeys))
		{
		returnValue = EFail;
		}
	return returnValue;
	}
	
TVerdict CTestToolEngine::ListKeys(const TDesC& aLabel1, const TInt aExpectedNumLabel)
	{
	TVerdict returnValue = EPass;
	iLabel1.Set(aLabel1);
	
	KeyInitilize();
	if (iNumCert1 != aExpectedNumLabel)
		{
		returnValue = EFail;
		}
	return returnValue;
	}


TVerdict CTestToolEngine::ListKeys(const TDesC& aLabel1, const TDesC& aLabel2, const TInt aExpectedNumLabel)
	{
	TVerdict returnValue = EPass;
	iLabel1.Set(aLabel1);
	iLabel2.Set(aLabel2);
	
	KeyInitilize();
	if (iNumCert1 != aExpectedNumLabel || iNumCert2 != aExpectedNumLabel)
		{
		returnValue = EFail;
		}
	return returnValue;
	}

TVerdict CTestToolEngine::ListKeys(RPointerArray<HBufC>& aKeys, TInt aStore)
	{
	TVerdict returnValue = EPass;
	iStore = aStore;
	iKeysListCount = aKeys.Count();
	
	KeyInitilize();
	if (!CompareWithExpectedKeys(aKeys))
		{
		returnValue = EFail;
		}
	return returnValue;
	}


TVerdict CTestToolEngine::ListKeys(RPointerArray<HBufC>& aKeys)
	{
	TVerdict returnValue = EPass;
	iKeysListCount = aKeys.Count();
	
	KeyInitilize();
	
	if (!CompareWithExpectedKeys(aKeys))
		{
		returnValue = EFail;
		}
	return returnValue;
	}

TVerdict CTestToolEngine::GetPolicy(const TDesC& aLabel1, const TDesC& aLabel2, TInt aUser)
	{
	TVerdict returnValue = EPass;
	iLabel1.Set(aLabel1);
	iLabel2.Set(aLabel2);
	iPolicy = ETrue;
	KeyInitilize();
	VerifyPolicy(aUser);

	return returnValue;
	}
	
void CTestToolEngine::Initilize()
	{
	iState = EInitialise;
	iCertStore->Initialize(iStatus);	
	SetActive();
	CActiveScheduler::Start();
	}
	
void CTestToolEngine::KeyInitilize()
	{
	iState = EInitialiseKeyStore;
	iKeyStore->Initialize(iStatus);	
	SetActive();
	CActiveScheduler::Start();
	}

TBool CTestToolEngine::VerifyApps(RPointerArray<HBufC>& aApps)
	{
	TInt uidsCount = iUids.Count();
	TInt appsCount = aApps.Count();
	TBool result = ETrue;
	if ( uidsCount != appsCount)
		{
		return EFalse;
		}
	for (TInt i = 0; i < uidsCount; i++)
		{
		if (iUids[i] == TUid::Uid(swinstalluid))
			{
			result = CheckAppForUid(KSWInstall, aApps);
			}
		else if (iUids[i] == TUid::Uid(swinstallocspuid))
			{
			result = CheckAppForUid(KSWInstallOCSP, aApps);
			}
		else if (iUids[i] == TUid::Uid(midletinstalluid))
			{
			result = CheckAppForUid(KMidletInstall, aApps);
			}
		else if (iUids[i] == TUid::Uid(tlsuid))
			{
			result = CheckAppForUid(KTls, aApps);
			}
		else if (iUids[i] == TUid::Uid(tocspuid))
			{
			result = CheckAppForUid(KTOcsp, aApps);
			}
		else
			{
			result = EFalse;
			}
		if (!result)
			{
			break;
			}
		}
	return result;
	}

TBool CTestToolEngine::CheckAppForUid(const TDesC& aAppName, RPointerArray<HBufC>& aApps)
	{
	TBool result = EFalse;
	for (TInt i = 0; i < aApps.Count();i++)
		{
		if (aAppName.CompareF(*aApps[i]) == KErrNone)
			{
			result = ETrue;
			break;
			}
		else
			{
			result = EFalse;
			}
		}
	return result;
	}

TBool CTestToolEngine::VerifyPolicy(TInt aUser)
	{
	TBool result = EFalse;
	_LIT(KCab, "WriteDeviceData");
	TLex lex(iLabel2);
	TUid secid;
	lex.Val(secid.iUid);
	TCompiledSecurityPolicy managementPolicy = iKeys[iIndex]->ManagementPolicy();
	TCompiledSecurityPolicy usePolicy = iKeys[iIndex]->UsePolicy();
	TUint32 mSecureid = managementPolicy.SecureId();
	TUint32 uSecureid = usePolicy.SecureId();

	if (aUser == 2)
		{
		if (mSecureid == 0xffffffff)
			{
			TCapability cab = managementPolicy.Capability(0);
			// currently only possible for keyowner other than secure id.
			if (cab == ECapabilityWriteDeviceData && iLabel2.CompareF(KCab) == KErrNone)
				{
				result = ETrue;
				}
			else
				{
				result = EFalse;
				}
			}
		else
			{
			TUid mSecureUid = TUid::Uid(mSecureid);
			if (mSecureUid == secid)
				{
				result = ETrue;
				}
			else
				{
				result = EFalse;
				}
			}
		}
	else if (aUser == 1)
		{
		if (uSecureid == 0xffffffff)
			{
			TSecurityPolicy::TType utype = usePolicy.Type();
			_LIT(KType, "ALL");
			// currently only possible for keyuser other than secure id
			if (utype == TSecurityPolicy::ETypePass && iLabel2.CompareF(KType) == KErrNone)
				{
				result = ETrue;
				}
			else
				{
				result = EFalse;
				}
			}
		else
			{
			TUid uSecureUid = TUid::Uid(uSecureid);	
			if (uSecureUid == secid)
				{
				result = ETrue;
				}
			else
				{
				result = EFalse;
				}
			}	
		}
	return result;
	}
	
TInt CTestToolEngine::FilterCerts(RMPointerArray<CCTCertInfo>& aCertList, 
                                TDesC& aLabel, TInt& aIndex, TBool aFilterOnly)
  	{
  	TInt j = 0, count = 0;
  	while (j < aCertList.Count())
  		{
  		if ( aCertList[j]->Label().MatchF(aLabel) != KErrNotFound) 
  			{
  			count++;
  			if (count == 1)
  				{
  				//first certificate to be selected for retreiving
  				aIndex = j;
  				}
  			j++;
  			}
  		else
  			{
  			if (!aFilterOnly)
  				{
  				CCTCertInfo* tmp = aCertList[j];
	  			tmp->Release();
  				aCertList.Remove(j);	
  				}
  			else
  				{
  				j++;	
  				}
  			}
  		}
  	return count;
  	}

void CTestToolEngine::FilterCerts(RMPointerArray<CCTCertInfo>& aCertList, TCertificateOwnerType& aOwnerType)
	{
	TInt j = 0;
  	while (j < aCertList.Count())
  		{
  		if ( aCertList[j]->CertificateOwnerType() != aOwnerType) 
  			{
  			CCTCertInfo* tmp = aCertList[j];
  			tmp->Release();
  			aCertList.Remove(j);
  			}
  		else
  			{
  			j++;
  			}	
  		}
	}
  
  
TInt CTestToolEngine::FilterKeys(RMPointerArray<CCTKeyInfo>& aKeyList,
							 TDesC& aLabel, TInt& aIndex)
  	{
  	TInt j = 0, count = 0;
  	while (j < aKeyList.Count())
  		{
  		if ( aKeyList[j]->Label().MatchF(aLabel) != KErrNotFound) 
  			{
  			count++;
  			if (count == 1)
  				{
  				//first key to be selected for retreiving
  				aIndex = j;
  				}
  			j++;
  			}
  		else
  			{
			j++;
  			}	
  		}
  	return count;
  	}

TBool CTestToolEngine::CompareWithExpectedCertificates(RPointerArray<HBufC>& aCerts)
	{
	if ( iCertInfos.Count() != aCerts.Count())
		{
		return EFalse;
		}
	for (TInt i = 0 ; i < iCertInfos.Count(); i++)
		{
		const TDesC& label = iCertInfos[i]->Label();
		TBool found = EFalse;
		for (TInt j = 0; j < aCerts.Count(); j++)
			{
			if (aCerts[j]->CompareF(label) == KErrNone)
				{
				found = ETrue;
				break;
				}
			}
		if (!found)
			{
			return EFalse;
			}
		}
	return ETrue;
	}
	

TBool CTestToolEngine::CompareWithExpectedKeys(RPointerArray<HBufC>& aKeys)
	{
	if ( iKeys.Count() != iKeysListCount)
		{
		return EFalse;
		}
	for (TInt i = 0 ; i < iKeys.Count(); i++)
		{
		const TDesC& label = iKeys[i]->Label();
		TBool found = EFalse;
		for (TInt j = 0; j < iKeysListCount; j++)
			{
			if (aKeys[j]->CompareF(label) == KErrNone)
				{
				found = ETrue;
				break;
				}
			}
		if (!found)
			{
			return EFalse;
			}
		}
	return ETrue;
	}
	
TBool CTestToolEngine::CompareWithExpectedStore(RPointerArray<HBufC>& aCerts)
	{
	TBool result = EFalse;
	TInt storeCount = iCertStore->CertStoreCount();
 	for (TInt i = 0; i < storeCount ; i++)
		{
		result = EFalse;
		MCTToken& token = (iCertStore->CertStore(i)).Token();
		const TDesC& tmp = token.Label();
		for (TInt j = 0; j < aCerts.Count(); j++)
			{
			if (tmp.CompareF(*aCerts[j]) == KErrNone)
				{
				result = ETrue;
				break;
				}
			}
		if (!result)
			{
			break;
			}
		}
	return result;
	}
	
	
TBool CTestToolEngine::CompareWithExpectedStoreKey(RPointerArray<HBufC>& aKeys)
	{
	TBool result = EFalse;
	TInt storeCount = iKeyStore->KeyStoreCount();
 	for (TInt i = 0; i < storeCount ; i++)
		{
		result = EFalse;
		MCTToken& token = (iKeyStore->KeyStore(i)).Token();
		const TDesC& tmp = token.Label();
		for (TInt j = 0; j < aKeys.Count(); j++)
			{
			if (tmp.CompareF(*aKeys[j]) == KErrNone)
				{
				result = ETrue;
				break;
				}
			}
		if (!result)
			{
			break;
			}
		}
	return result;
	}
	


TBool CTestToolEngine::VerifyTrust(TBool aTrust)
	{
	TBool result = EFalse;
	if (aTrust == iActualTrust)
		{
		result = ETrue;
		}
	else
		{
		result = EFalse;
		}
	return result;
	}
