/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* legacy plugin selector implementation
* Legacy plugin selector implementation
*
*/


/**
 @file
*/
#include "legacyselector.h"
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/cryptocharacteristics.h>
#include "cryptospiproperty.h"
#include <cryptospi/hashplugin.h>
#include <cryptospi/randomplugin.h>
#include <cryptospi/symmetriccipherplugin.h>
#include <cryptospi/asymmetriccipherplugin.h>
#include <cryptospi/signerplugin.h>
#include <cryptospi/verifierplugin.h>
#include <cryptospi/keyagreementplugin.h>
#include <cryptospi/keypairgeneratorplugin.h>
#include <cryptospi/cryptohashapi.h>
#include <cryptospi/cryptorandomapi.h>
#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptokeyagreementapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/pluginentrydef.h>
#include "cryptospiutil.h"

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	#include <cryptospi/macplugin.h>
	#include <cryptospi/cryptomacapi.h>
#endif

using namespace CryptoSpi;

CLegacySelector* CLegacySelector::NewL()
	{
	CLegacySelector* self=CLegacySelector::NewLC();
	CleanupStack::Pop(self);
	return self;		
	}

CLegacySelector* CLegacySelector::NewLC()
	{
	CLegacySelector* self=new(ELeave) CLegacySelector();
	CleanupStack::PushL(self);
	return self;
	}
CLegacySelector::CLegacySelector()
	{
	}
	
CLegacySelector::~CLegacySelector()
	{
	iCharacteristicsAndDllIndex.ResetAndDestroy();	
	}

void CLegacySelector::CreateHashL(CHash*& aHash,
									TUid aAlgorithmUid,
									TUid aOperationMode,
									const CKey* aKey,
									const CCryptoParams* aAlgorithmParams)
	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KHashInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TUid implUid={0};
		TFileName fileName;
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateHashFuncL func=(CreateHashFuncL)sharedLib.Lookup(ECreateHashOrdinal);
			if (func)
				{
				//create the plugin
				MHash* hashPlugin=NULL;
				TRAP(ret, (func)(hashPlugin, implUid, aOperationMode, aKey, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*hashPlugin);
					aHash=CHash::NewL(hashPlugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //hashPlugin, sharedLib
					break;
					}					
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
				}
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	

void CLegacySelector::CreateHashL(CHash*& aHash,
									TUid aAlgorithmUid,
									const CCryptoParams* aAlgorithmParams)
	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KHashInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TUid implUid={0};
		TFileName fileName;
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateHashFuncLv2 func=(CreateHashFuncLv2)sharedLib.Lookup(ECreateHashOrdinalv2);
			if (func)
				{
				//create the plugin
				MHash* hashPlugin=NULL;
				TRAP(ret, (func)(hashPlugin, implUid, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*hashPlugin);
					aHash=CHash::NewL(hashPlugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //hashPlugin, sharedLib
					break;
					}					
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
				}
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

void CLegacySelector::CreateMacL(CMac*& aMac,
								 TUid aAlgorithmUid,
								 const CKey& aKey,
								 const CCryptoParams* aAlgorithmParams)
	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KMacInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TUid implUid={0};
		TFileName fileName;
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateMacFuncL func=(CreateMacFuncL)sharedLib.Lookup(ECreateMacOrdinal);
			if (func)
				{
				//create the plugin
				MMac* macPlugin=NULL;

				TRAP(ret, (func)(macPlugin, implUid, aKey, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*macPlugin);
					aMac=CMac::NewL(macPlugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //macPlugin, sharedLib
					break;
					}					
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
				}
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

#endif

void CLegacySelector::CreateRandomL(CRandom*& aRandom,
									TUid aAlgorithmUid,
									const CCryptoParams* aAlgorithmParams)
	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KRandomInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TFileName fileName;
		TUid implUid={0};
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateRandomFuncL func=(CreateRandomFuncL)sharedLib.Lookup(ECreateRandomOrdinal);
			
			if (func)
				{
				//create the plugin
				MRandom* randomPlugin=NULL;
				TRAP(ret, (func)(randomPlugin, implUid, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*randomPlugin);
					aRandom=CRandom::NewL(randomPlugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //randomPlugin, sharedLib
					break;
					}
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
				}
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

void CLegacySelector::CreateSymmetricCipherL(CSymmetricCipher*& aCipher,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aCryptoMode,
											TUid aOperationMode,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams)

	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KSymmetricCipherInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TFileName fileName;
		TUid implUid={0};
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateSymmetricCipherFuncL func=(CreateSymmetricCipherFuncL)sharedLib.Lookup(ECreateSymmetricCipherOrdinal);
			if (func)
				{
				//create the plugin
				MSymmetricCipher* cipherPlugin=NULL;
				TRAP(ret, (func)(cipherPlugin, implUid, aKey, aCryptoMode, aOperationMode, aPaddingMode, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*cipherPlugin);
					aCipher=CSymmetricCipher::NewL(cipherPlugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //cipherPlugin, sharedLib
					break;
					}
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib	
				}			
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

void CLegacySelector::CreateAsymmetricCipherL(CAsymmetricCipher*& aCipher,
												TUid aAlgorithmUid,
												const CKey& aKey,
												TUid aCryptoMode,
												TUid aPaddingMode,									
												const CCryptoParams* aAlgorithmParams)
	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KAsymmetricCipherInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TFileName fileName;
		TUid implUid={0};
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateAsymmetricCipherFuncL func=(CreateAsymmetricCipherFuncL)sharedLib.Lookup(ECreateAsymmetricCipherOrdinal);
			
			if (func)
				{
				//create the plugin
				MAsymmetricCipher* cipherPlugin=NULL;
				TRAP(ret, (func)(cipherPlugin, implUid, aKey, aCryptoMode, aPaddingMode, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*cipherPlugin);
					aCipher=CAsymmetricCipher::NewL(cipherPlugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //cipherPlugin, sharedLib
					break;
					}
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib	
				}
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

void CLegacySelector::CreateSignerL(CSigner*& aSigner,
									TUid aAlgorithmUid,
									const CKey& aKey,
									TUid aPaddingMode,
									const CCryptoParams* aAlgorithmParams)
	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KSignerInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TFileName fileName;
		TUid implUid={0};		
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateSignerFuncL func=(CreateSignerFuncL)sharedLib.Lookup(ECreateSignerOrdinal);
			if (func)
				{
				//create the plugin
				MSigner* plugin=NULL;
				TRAP(ret, (func)(plugin, implUid, aKey, aPaddingMode, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*plugin);
					aSigner=CSigner::NewL(plugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //plugin, sharedLib
					break;
					}					
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib	
				}
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

void CLegacySelector::CreateVerifierL(CVerifier*& aVerifier,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams)
	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KVerifierInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TFileName fileName;
		TUid implUid={0};		
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateVerifierFuncL func=(CreateVerifierFuncL)sharedLib.Lookup(ECreateVerifierOrdinal);
			if (func)
				{
				//create the plugin
				MVerifier* plugin=NULL;
				TRAP(ret, (func)(plugin, implUid, aKey, aPaddingMode, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*plugin);
					aVerifier=CVerifier::NewL(plugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //plugin, sharedLib
					break;
					}
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
				}
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

void CLegacySelector::CreateKeyPairGeneratorL(CKeyPairGenerator*& aKeyPairGenerator,
												TUid aAlgorithmUid,
												const CCryptoParams* aAlgorithmParams)
	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KKeypairGeneratorInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TFileName fileName;
		TUid implUid={0};		
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateKeyPairGeneratorFuncL func=(CreateKeyPairGeneratorFuncL)sharedLib.Lookup(ECreateKeyPairGeneratorOrdinal);
			if (func)
				{
				//create the plugin
				MKeyPairGenerator* plugin=NULL;
				TRAP(ret, (func)(plugin, implUid, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*plugin);
					aKeyPairGenerator=CKeyPairGenerator::NewL(plugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //plugin, sharedLib
					break;
					}					
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
				}
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

void CLegacySelector::CreateKeyAgreementL(CKeyAgreement*& aKeyAgreement,
											TUid aAlgorithmUid,
											const CKey& aPrivateKey,
											const CCryptoParams* aAlgorithmParams)
	{
	CryptoSpiUtil::RetrieveCharacteristicsL(KKeyAgreementInterface, iCharacteristicsAndDllIndex);
	TInt found=KErrNone;
	TInt ret=KErrNone;
	do 
		{
		TFileName fileName;
		TUid implUid={0};		
		found=FindPlugin(aAlgorithmUid, implUid, fileName);
		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);
			
			//look for the entry point
			CreateKeyAgreementFuncL func=(CreateKeyAgreementFuncL)sharedLib.Lookup(ECreateKeyAgreementOrdinal);
			if (func)
				{
				//create the plugin
				MKeyAgreement* plugin=NULL;
				TRAP(ret, (func)(plugin, implUid, aPrivateKey, aAlgorithmParams))
				if (ret!=KErrNone)
					{
					if (ret==KErrNoMemory)
						{
						User::Leave(ret);	
						}
					CleanupStack::PopAndDestroy(&sharedLib); //sharedLib
					}
				else
					{
					CleanupClosePushL(*plugin);
					aKeyAgreement=CKeyAgreement::NewL(plugin, sharedLib.Handle());
					CleanupStack::Pop(2, &sharedLib); //plugin, sharedLib
					break;
					}					
				}
			else
				{
				CleanupStack::PopAndDestroy(&sharedLib); //sharedLib	
				}
			}
		}
	while (found!=KErrNotFound);
	
	User::LeaveIfError(ret);
	if (found!=KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	}

TInt CLegacySelector::FindPlugin(TUid aAlgorithmUid, TUid& aImplementationUid, TFileName& aDllName)
	{
	TInt ret=KErrNone;
	TInt count=iCharacteristicsAndDllIndex.Count();
	TBool found(EFalse);
	for (TInt i=iNextCharacteristic;i<count;i++)
		{
		if (aAlgorithmUid.iUid==iCharacteristicsAndDllIndex[i]->iCharacteristic->iAlgorithmUid)
			{
			aImplementationUid.iUid=iCharacteristicsAndDllIndex[i]->iCharacteristic->iImplementationUid;
			aDllName=iCharacteristicsAndDllIndex[i]->iDllName;
			found=ETrue;
			iNextCharacteristic=i+1;
			break;
			}
		}
	if (!found)
		{
		ret=KErrNotFound;
		}		
	return ret;		
	}
	

// These are stub methods for asynchronous calls, which are
// not implemented. Turn off the coverage for these methods.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

void CLegacySelector::CreateAsyncHashL(CAsyncHash*& /*aHash*/,
										TUid /*aAlgorithmUid*/,
										TUid /*aOperationMode*/,
										const CKey* /*aKey*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT

void CLegacySelector::CreateAsyncHashL(CAsyncHash*& /*aHash*/,
										TUid /*aAlgorithmUid*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}


void CLegacySelector::CreateAsyncMacL(CAsyncMac*& /*aMac*/,
									const TUid /*aAlgorithmUid*/,
									const CKey& /*aKey*/,
									const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

#endif

void CLegacySelector::CreateAsyncRandomL(CAsyncRandom*& /*aRandom*/,
										TUid /*aAlgorithmUid*/,
										const CCryptoParams* /*aAlgorithmParams*/)

	{
	User::Leave(KErrNotSupported);
	}

void CLegacySelector::CreateAsyncSymmetricCipherL(CAsyncSymmetricCipher*& /*aCipher*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aKey*/,
										TUid /*aCryptoMode*/,
										TUid /*aOperationMode*/,
										TUid /*aPaddingMode*/,
										const CCryptoParams* /*aAlgorithmParams*/)
										
	{
	User::Leave(KErrNotSupported);
	}

void CLegacySelector::CreateAsyncAsymmetricCipherL(CAsyncAsymmetricCipher*& /*aCipher*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aKey*/,
										TUid /*aCryptoMode*/,
										TUid /*aPaddingMode*/,																						
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}
										

void CLegacySelector::CreateAsyncSignerL(CAsyncSigner*& /*aSigner*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aKey*/,
										TUid /*aPaddingMode*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}
										

void CLegacySelector::CreateAsyncVerifierL(CAsyncVerifier*& /*aVerifier*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aKey*/,
										TUid /*aPaddingMode*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}
										

void CLegacySelector::CreateAsyncKeyPairGeneratorL(CAsyncKeyPairGenerator*& /*aKeyPairGenerator*/,
										TUid /*aAlgorithmUid*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}
										

void CLegacySelector::CreateAsyncKeyAgreementL(CAsyncKeyAgreement*& /*aKeyAgreement*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aPrivateKey*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

