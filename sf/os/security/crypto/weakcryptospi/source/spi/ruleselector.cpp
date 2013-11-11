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
* Rule-based plugin selector implementation
* Rule-based plugin selector implementation
*
*/


/**
 @file
*/

#include <cryptospi/ruleselector.h>
#include <cryptospi/cryptoparams.h>
#include "cryptospiutil.h"
#include <cryptospi/cryptohashapi.h>
#include <cryptospi/cryptorandomapi.h>
#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptokeyagreementapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/pluginentrydef.h>
#include <cryptospi/hashplugin.h>
#include <cryptospi/randomplugin.h>
#include <cryptospi/symmetriccipherplugin.h>
#include <cryptospi/asymmetriccipherplugin.h>
#include <cryptospi/signerplugin.h>
#include <cryptospi/verifierplugin.h>
#include <cryptospi/keyagreementplugin.h>
#include <cryptospi/keypairgeneratorplugin.h>
#include <cryptospi/cryptospidef.h>
#include "cryptospiproperty.h"
#include "rulecharacteristics.h"

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	#include <cryptospi/cryptomacapi.h>
	#include <cryptospi/macplugin.h>
#endif

using namespace CryptoSpi;

//
// Implementation of a single selection rule content
//

EXPORT_C CSelectionRuleContent* CSelectionRuleContent::NewL(TUid aInterfaceScope,
															TUid aAlgorithmScope,
															CCryptoParam* aCharacteristicValue,
															TRuleOperator aOperator,
															TBool aIsOptional)
	{
	CSelectionRuleContent* self=new (ELeave) CSelectionRuleContent(aInterfaceScope,
																	aAlgorithmScope,
																	aCharacteristicValue,
																	aOperator,
																	aIsOptional);
	return self;
	}
	
EXPORT_C CSelectionRuleContent::~CSelectionRuleContent()
	{
	delete iCharacteristicValue;
	}

CSelectionRuleContent::CSelectionRuleContent(TUid aInterfaceScope,
											TUid aAlgorithmScope,
											CCryptoParam* aCharacteristicValue,
											TRuleOperator aOperator,
											TBool aIsOptional)
	:iInterfaceScope(aInterfaceScope),
	iAlgorithmScope(aAlgorithmScope),
	iCharacteristicValue(aCharacteristicValue),
	iOperator(aOperator),
	iIsOptional(aIsOptional)
	{
	}

TUid CSelectionRuleContent::GetInterfaceScope()
	{
	return iInterfaceScope;
	}

TUid CSelectionRuleContent::GetAlgorithmScope()
	{
	return iAlgorithmScope;
	}

TInt CSelectionRuleContent::GetOperator()
	{
	return iOperator;
	}
	
const CCryptoParam* CSelectionRuleContent::GetCharacteristicValue()
	{
	return iCharacteristicValue;
	}

TBool CSelectionRuleContent::IsOptionalRule()
	{
	return iIsOptional;
	}


//
// Impplemetation of selection rule
//

EXPORT_C CSelectionRules* CSelectionRules::NewL()
	{
	CSelectionRules* self=new (ELeave) CSelectionRules();
	return self;
	}
	
	
EXPORT_C CSelectionRules::~CSelectionRules()
	{
	iRules.ResetAndDestroy();
	}
	
EXPORT_C void CSelectionRules::AddSelectionRuleL(CSelectionRuleContent* aSelectionRule)
	{
	TUid charUid = aSelectionRule->GetCharacteristicValue()->Uid();
	TInt operator_r = aSelectionRule->GetOperator();
	TInt type = aSelectionRule->GetCharacteristicValue()->Type();
	
	switch (charUid.iUid)
		{
		case KInterfaceType:
		case KAlgorithmType:
		case KImplementationType:
			{
			if (type != CCryptoParam::EInt)
				{
				User::Leave(KErrArgument);
				}

			if (operator_r != EOpEqual &&
				operator_r != EOpNotEqual)
				{
				User::Leave(KErrArgument);
				}
				
			break;
			}
												
		case KCreatorNameType:
			{
			if (type != CCryptoParam::EDesC16)
				{
				User::Leave(KErrArgument);
				}
				
			if (operator_r != EOpAscendingSort &&
				operator_r != EOpDescendingSort &&
				operator_r != EOpEqual &&
				operator_r != EOpNotEqual)
				{
				User::Leave(KErrArgument);
				}
							
			break;
			}
				
		case KIsFIPSApprovedType:
		case KIsHardwareSupportedType:
			{
			if (type != CCryptoParam::EInt)
				{
				User::Leave(KErrArgument);
				}
			
			if (operator_r != EOpEqual &&
				operator_r != EOpNotEqual)
				{
				User::Leave(KErrArgument);
				}
				
			break;
			}
		
		case KMaxConcurrencySupportedType:
		case KLatencyType:
		case KThroughputType:
			{
			if (type != CCryptoParam::EInt)
				{
				User::Leave(KErrArgument);
				}
			
			break;
			}
				
		default:
			{
			break;
			}
		}//switch (charUid.iUid)
			
	iRules.AppendL(aSelectionRule);
	}
	
CSelectionRules::CSelectionRules()
	{
	}

RPointerArray<CSelectionRuleContent>& CSelectionRules::GetSelectionRules()
	{
	return iRules;
	}


//
// Implementation of plugin selector
//

EXPORT_C CRuleSelector* CRuleSelector::NewL(CSelectionRules* aRules)
	{
	CRuleSelector* self = CRuleSelector::NewLC(aRules);
	CleanupStack::Pop(self);
	return self;				
	}

EXPORT_C CRuleSelector* CRuleSelector::NewLC(CSelectionRules* aRules)
	{
	CRuleSelector* self=new (ELeave) CRuleSelector();
	CleanupStack::PushL(self);
	self->ConstructL(aRules);
	
	return self;
	}
	
EXPORT_C CRuleSelector::~CRuleSelector()
	{
	delete iSelectionRules;
	
	TInt interfaceCount = sizeof(KInterfacesUids)/sizeof(KInterfacesUids[0]);

	//Loop through each Interface
	for (TInt i = 0; i < interfaceCount; ++i)
		{
		RPointerArray<CRulesCharacteristicsAndPluginName>** listA = iSelectedInterfaceCharacteristics_MapA.Find(KInterfacesUids[i].iUid);
		RPointerArray<CRulesCharacteristicsAndPluginName>** listB = iSelectedInterfaceCharacteristics_MapB.Find(KInterfacesUids[i].iUid);

		if (listA)
			{
			(*listA)->ResetAndDestroy();
			delete *listA;
			}

		if (listB)
			{
			(*listB)->ResetAndDestroy();
			delete *listB;
			}
		}
		
	iSelectedInterfaceCharacteristics_MapA.Close();
	iSelectedInterfaceCharacteristics_MapB.Close();

	iNextTryCharacteristicsIndex.Close();
	
	//Unload the plugin DLLs and release the array
	TInt dllCount=iPluginDllList.Count();
	for (TInt j=0;j<dllCount;j++)
		{
		iPluginDllList[j].Close();
		}
		
	iPluginDllList.Close();
	}

CRuleSelector::CRuleSelector():
	iUseMapAToFilter(ETrue),
	iToggleUseMap(ETrue)
	{
	}

void CRuleSelector::ConstructL(CSelectionRules* aRules)
	{
	ConstructMapAL();
	TInt interfaceCount = sizeof(KInterfacesUids)/sizeof(KInterfacesUids[0]);

	//Init the next index to try records
	for (TInt i=0;i < interfaceCount;i++)
		{
		iNextTryCharacteristicsIndex.InsertL(KInterfacesUids[i].iUid, 0);
		}
		
	//Build iSelectedInterfaceCharacteristicsMap according to the rules applied
	PerformFilterL(aRules);

	if (iUseMapAToFilter)
		{
		//NOTE: After each filtering, it will toggle the iUseMapAToFilter for next process
		
		//Set iActiveInterfaceCharacteristics_Map to use iSelectedInterfaceCharacteristicsMap
		iActiveInterfaceCharacteristics_Map = &iSelectedInterfaceCharacteristics_MapA;
		}
	else
		{
		//Set iActiveInterfaceCharacteristics_Map to use iSelectedInterfaceCharacteristics_MapB
		iActiveInterfaceCharacteristics_Map = &iSelectedInterfaceCharacteristics_MapB;
		}

	iSelectionRules = aRules;
	}

void CRuleSelector::CreateHashL(CHash*& aHash,
								TUid aAlgorithmUid,
								TUid aOperationMode,
								const CKey* aKey,
								const CCryptoParams* aAlgorithmParams)
	{
	TInt found=KErrNone;
	TInt ret=KErrNone;

	SetSearchRecord(KHashInterfaceUid, 0);
	
	do 
		{
		TUid implUid={0};
		TFileName fileName;
		found=FindPlugin(KHashInterfaceUid, aAlgorithmUid, implUid, fileName);

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

void CRuleSelector::CreateHashL(CHash*& aHash,
								TUid aAlgorithmUid,
								const CCryptoParams* aAlgorithmParams)
	{
	TInt found=KErrNone;
	TInt ret=KErrNone;

	SetSearchRecord(KHashInterfaceUid, 0);
	
	do 
		{
		TUid implUid={0};
		TFileName fileName;
		found=FindPlugin(KHashInterfaceUid, aAlgorithmUid, implUid, fileName);

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

void CRuleSelector::CreateMacL(CMac*& aMac, const TUid aAlgorithmUid, const CKey& aKey, const CCryptoParams* aAlgorithmParams)
	{
	TInt found=KErrNone;
	TInt ret=KErrNone;
	
	/**
	 * Reset the search record for the mentioned interface, so that search can start from
	 * index 0.
	 */
	SetSearchRecord(KMacInterfaceUid, 0);
	
	/**
	 * The following do-while block is a serach mechanism for the right algorithm implementation.
	 * The block logic assumes the case when a user may define a very abstract level rule for 
	 * which there is several matches within an interface and across the interfaces meeting the 
	 * rule (both intra and inter interface match).
	 * The block logic has support to counter attack the ambiguity passed on by the 
	 * user application when defining the rules for the rule based selector.   
	 */
	do 
		{
		TUid implUid={0};
		TFileName fileName;
		
		/**
		 * Here 'aAlgorithmUid' can be Hash Based or Cipher Based. 
		 * KAlgorithmCipherAesXcbcMac96, KAlgorithmCipherAesXcbcPrf128, KAlgorithmHashMd2 
		 * etc...
		 */
		found=FindPlugin(KMacInterfaceUid, aAlgorithmUid, implUid, fileName);

		if (found==KErrNone)
			{
			//Load the dll and make the handle be sharable in the process
			RLibrary sharedLib;
			CryptoSpiUtil::LoadPluginDllLC(sharedLib, fileName);

			//look for the entry point in the plugin-dll.
			CreateMacFuncL func=(CreateMacFuncL)sharedLib.Lookup(ECreateMacOrdinal);
			
			if (func)
				{
				/** 
				 * The abstract base class interface to which the MAC implementation
				 * will be plugged in.  
				 */
				MMac* macPlugin=NULL;

				/**
				 * Here 'implUid' will contain the implementation of the Hash Based or
				 * Cipher Base algorithm (aAlgorithmUid) chosen by rule based selector.
				 * One Algorithm can have various implementations provided by different vendors.
				 * The choice of selection of the implementation will be dependent on the rule defined
				 * by the user application which is passed to the rule based selector when initializing.
				 * 
				 * KCryptoPluginMacAesXcbcMac96, KCryptoPluginMacAesXcbcPrf128, KCryptoPluginMacHashMd2
				 * etc... 
				 */
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
					
					/**
					 * Here an empty 'aMac' pointer delegated by the user application to the 
					 * CryptoSpi Framework is filled by the rule based selector.
					 * Rule Based selector is part of the framework.
					 */
					
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

void CRuleSelector::CreateRandomL(CRandom*& aRandom,
									TUid aAlgorithmUid,
									const CCryptoParams* aAlgorithmParams)
	{
	TInt found=KErrNone;
	TInt ret=KErrNone;

	SetSearchRecord(KRandomInterfaceUid, 0);
	
	do 
		{
		TFileName fileName;
		TUid implUid={0};
		found=FindPlugin(KRandomInterfaceUid, aAlgorithmUid, implUid, fileName);


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

void CRuleSelector::CreateSymmetricCipherL(CSymmetricCipher*& aCipher,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aCryptoMode,
											TUid aOperationMode,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams)

	{
	TInt found=KErrNone;
	TInt ret=KErrNone;

	SetSearchRecord(KSymmetricCipherInterfaceUid, 0);
	
	do 
		{
		TFileName fileName;
		TUid implUid={0};
		found=FindPlugin(KSymmetricCipherInterfaceUid, aAlgorithmUid, implUid, fileName);

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

void CRuleSelector::CreateAsymmetricCipherL(CAsymmetricCipher*& aCipher,
												TUid aAlgorithmUid,
												const CKey& aKey,
												TUid aCryptoMode,
												TUid aPaddingMode,									
												const CCryptoParams* aAlgorithmParams)
	{
	TInt found=KErrNone;
	TInt ret=KErrNone;

	SetSearchRecord(KAsymmetricCipherInterfaceUid, 0);

	do 
		{
		TFileName fileName;
		TUid implUid={0};
		found=FindPlugin(KAsymmetricCipherInterfaceUid, aAlgorithmUid, implUid, fileName);

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

void CRuleSelector::CreateSignerL(CSigner*& aSigner,
									TUid aAlgorithmUid,
									const CKey& aKey,
									TUid aPaddingMode,
									const CCryptoParams* aAlgorithmParams)
	{
	TInt found=KErrNone;
	TInt ret=KErrNone;

	SetSearchRecord(KSignerInterfaceUid, 0);

	do 
		{
		TFileName fileName;
		TUid implUid={0};		
		found=FindPlugin(KSignerInterfaceUid, aAlgorithmUid, implUid, fileName);

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

void CRuleSelector::CreateVerifierL(CVerifier*& aVerifier,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams)
	{
	TInt found=KErrNone;
	TInt ret=KErrNone;

	SetSearchRecord(KVerifierInterfaceUid, 0);

	do 
		{
		TFileName fileName;
		TUid implUid={0};		
		found=FindPlugin(KVerifierInterfaceUid, aAlgorithmUid, implUid, fileName);

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

void CRuleSelector::CreateKeyPairGeneratorL(CKeyPairGenerator*& aKeyPairGenerator,
												TUid aAlgorithmUid,
												const CCryptoParams* aAlgorithmParams)
	{
	TInt found=KErrNone;
	TInt ret=KErrNone;

	SetSearchRecord(KKeypairGeneratorInterfaceUid, 0);

	do 
		{
		TFileName fileName;
		TUid implUid={0};		
		found=FindPlugin(KKeypairGeneratorInterfaceUid, aAlgorithmUid, implUid, fileName);
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

void CRuleSelector::CreateKeyAgreementL(CKeyAgreement*& aKeyAgreement,
											TUid aAlgorithmUid,
											const CKey& aPrivateKey,
											const CCryptoParams* aAlgorithmParams)
	{
	TInt found=KErrNone;
	TInt ret=KErrNone;

	SetSearchRecord(KKeyAgreementInterfaceUid, 0);

	do 
		{
		TFileName fileName;
		TUid implUid={0};		
		found=FindPlugin(KKeyAgreementInterfaceUid, aAlgorithmUid, implUid, fileName);
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

void CRuleSelector::PerformFilterL(CSelectionRules* aRules)
	{
	const RPointerArray<CSelectionRuleContent>& rules = aRules->GetSelectionRules();
	const RHashMap<TInt32, RPointerArray<CRulesCharacteristicsAndPluginName>*>* interfaceCharMap = NULL;
	TInt rCount = rules.Count();
	
	//Iterate through each rule
	for (TInt r = 0; r < rCount; ++r)
		{
		if (iUseMapAToFilter)
			{
			interfaceCharMap = &iSelectedInterfaceCharacteristics_MapA;
			}
		else
			{
			interfaceCharMap = &iSelectedInterfaceCharacteristics_MapB;
			}
					
		TInt32 interfaceScope = rules[r]->GetInterfaceScope().iUid;
		TInt interfaceCount = sizeof(KInterfacesUids)/sizeof(KInterfacesUids[0]);
		TInt operator_r = rules[r]->GetOperator();

		if (operator_r == EOpAscendingSort || operator_r == EOpDescendingSort)
			{
			/**
			This is use to indicate don't alternate the map for this rule filtering
			*/
			iToggleUseMap = EFalse;
			}

		for (TInt i = 0; i < interfaceCount; ++i)
			{
			//Loop through each Interface
			
			TInt lastPreference =0;		//Indicate the list position of the last algorithm that met the rules
			RPointerArray<CRulesCharacteristicsAndPluginName>* const* list = interfaceCharMap->Find(KInterfacesUids[i].iUid);

			if (list)
				{
				while ((*list)->Count())
					{
					//Loop through each algorithm's characteristics list

					if (interfaceScope == 0)
						{
						PerformAlgorithmFilterL(KInterfacesUids[i].iUid, **list,
												rules[r], lastPreference);
						}//if (interfaceScope == 0)
					else
						{
						//This is an Interface rule
						
						if (interfaceScope == KInterfacesUids[i].iUid)
							{
							PerformAlgorithmFilterL(KInterfacesUids[i].iUid, **list,
													rules[r], lastPreference);
							}
						else
							{
							//Not within the specified interfaceScope, just build it to the 
							//end of iSelectedInterfaceCharacteristicsMap
							AddOptionalToCharListL(KInterfacesUids[i].iUid, *(**list)[0]);
							}
						}
					
					if (!iToggleUseMap)
						{
						//Just sorted the list, so skip the rest of the algorithms and
						//continue with next interface list
						break;	
						}
						
					//Remove from source list". As it has either been deleted or transfered to the new list.
					(*list)->Remove(0);
					
					}//while ((*list)->Count())
				} //if (list)
			else
				{
				//No CCharacteristicDllIndexList found!!!!
				//It is possible; KKeyGeneratorInterfaceUid is currently not in used.
				//Do nothing
				}
			}//for (TInt i = 0; i < NumberOfIntefaces; ++i) 

		if (iToggleUseMap)
			{
			if (iUseMapAToFilter)
				{
				//Toggle to use iSelectedInterfaceCharacteristics_MapB on next round
				iUseMapAToFilter = EFalse;
				TInt interfaceCount = sizeof(KInterfacesUids)/sizeof(KInterfacesUids[0]);
				
				//Destroy the characteristic map and lists for the next filtering process
				for (TInt i = 0; i < interfaceCount; ++i)
					{
					RPointerArray<CRulesCharacteristicsAndPluginName>** listA = iSelectedInterfaceCharacteristics_MapA.Find(KInterfacesUids[i].iUid);
		
					if (listA)
						{
						(*listA)->ResetAndDestroy();
						delete *listA;
						}
					}

				iSelectedInterfaceCharacteristics_MapA.Close();
				}
			else
				{
				//Toggle to use iSelectedInterfaceCharacteristicsMap on next round
				iUseMapAToFilter = ETrue;
				TInt interfaceCount = sizeof(KInterfacesUids)/sizeof(KInterfacesUids[0]);
				
				//Destroy the characteristic map and lists for the next filtering process
				for (TInt i = 0; i < interfaceCount; ++i)
					{
					RPointerArray<CRulesCharacteristicsAndPluginName>** listB = iSelectedInterfaceCharacteristics_MapB.Find(KInterfacesUids[i].iUid);
		
					if (listB)
						{
						(*listB)->ResetAndDestroy();
						delete *listB;
						}
					}

				iSelectedInterfaceCharacteristics_MapB.Close();
				}
			}
		else
			{
			//Just finished sorting, reuse the same map for next round
			//reset iToggleUseMap for next round
			iToggleUseMap = ETrue;
			}
		}//for (TInt r = 0; r < rules.Count(); ++r)
	}

void CRuleSelector::PerformAlgorithmFilterL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
											CSelectionRuleContent* aRules, TInt& aLastPreference)
	{
	if (aRules->GetAlgorithmScope().iUid == 0)
		{
		if (EFalse == FilterCommonCharacteristicsL(aInterfaceUid, aCharacteristicsDLL,
													aRules, aLastPreference))
			{
			//NOT FOUND! Need to go through each non-common characteristics
			if (EFalse == FilterNonCommonCharacteristicsL(aInterfaceUid, aCharacteristicsDLL,
															aRules, aLastPreference))
				{
				if (EFalse == FilterExtendedCharacteristicsL(aInterfaceUid, aCharacteristicsDLL,
																aRules, aLastPreference))
					{
					//Try adding it as optional
					TryAddToOptionalCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules);
					}
				}
			}
		}
	else
		{
		//This is an Algorithm rule
		const CCharacteristics* cmn(aCharacteristicsDLL[0]->iCharacteristic);
									
		if (aRules->GetAlgorithmScope().iUid == cmn->iAlgorithmUid)
			{
			//Matches the algorithmScope
			if (EFalse == FilterCommonCharacteristicsL(aInterfaceUid, aCharacteristicsDLL,
														aRules, aLastPreference))
				{
				//NOT FOUND! Need to go through each non-common characteristics
				if (EFalse == FilterNonCommonCharacteristicsL(aInterfaceUid, aCharacteristicsDLL,
																aRules, aLastPreference))
					{
					if (EFalse == FilterExtendedCharacteristicsL(aInterfaceUid, aCharacteristicsDLL,
																	aRules, aLastPreference))
						{
						//Try adding it as optional
						TryAddToOptionalCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules);
						}
					}
				}
			}
		else
			{
			//Not within the algorithmScope, just build it to the end of 
			//iSelectedInterfaceCharacteristicsMap
			AddOptionalToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]));
			}
		}
	}

TBool CRuleSelector::FilterCommonCharacteristicsL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
													CSelectionRuleContent* aRules, TInt& aLastPreference)
{
	const CCharacteristics* cmn(aCharacteristicsDLL[0]->iCharacteristic);
	const CCryptoParam* charValue = aRules->GetCharacteristicValue();
	
	switch (charValue->Uid().iUid)
		{
		case KInterfaceType:
			{
			FilterTInt32L(aInterfaceUid, *(aCharacteristicsDLL[0]),
							aRules, aLastPreference, cmn->iInterfaceUid);
			break;
			}
								
		case KAlgorithmType:
			{
			FilterTInt32L(aInterfaceUid, *(aCharacteristicsDLL[0]),
							aRules, aLastPreference, cmn->iAlgorithmUid);
			break;
			}
																	
		case KImplementationType:
			{
			FilterTInt32L(aInterfaceUid, *(aCharacteristicsDLL[0]),
							aRules, aLastPreference, cmn->iImplementationUid);
			break;
			}

		case KCreatorNameType:
			{
			FilterCreatorNameL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference);
			break;
			}
								
		case KIsFIPSApprovedType:
			{
			FilterTBoolL(aInterfaceUid, *(aCharacteristicsDLL[0]),
							aRules, aLastPreference, cmn->iIsFIPSApproved);
			break;
			}
						
		case KIsHardwareSupportedType:
			{
			FilterTBoolL(aInterfaceUid, *(aCharacteristicsDLL[0]),
							aRules, aLastPreference, cmn->iIsHardwareSupported);
			break;
			}
								
		case KMaxConcurrencySupportedType:
			{
			FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
						aRules, aLastPreference, cmn->iMaxConcurrencySupported);
			break;
			}
								
		case KLatencyType:
			{
			FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
						aRules, aLastPreference, cmn->iLatency);
			break;
			}
								
		case KThroughputType:
			{
			FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
						aRules, aLastPreference, cmn->iThroughput);
			break;
			}
								
		default:
			{
			//This is not a Common Characteristic
			return EFalse;
			}
		} //switch (charValue->Uid().iUid)
	
	return ETrue;
}

TBool CRuleSelector::FilterNonCommonCharacteristicsL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
													CSelectionRuleContent* aRules, TInt& aLastPreference)
	{
	const CCryptoParam* ruleCharValue = aRules->GetCharacteristicValue();

	switch (ruleCharValue->Uid().iUid)
		{
		case KBlockSizeType:
			{
			switch (aInterfaceUid)
				{
				case KHashInterface:
				case KAsyncHashInterface:
					{
					const CHashCharacteristics* charsPtr(static_cast<const CHashCharacteristics*>(
		 										aCharacteristicsDLL[0]->iCharacteristic));

					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iBlockSize);
									
					return ETrue;
					}
				case KSymmetricCipherInterface:
				case KAsyncSymmetricCipherInterface:
					{
					const CSymmetricCipherCharacteristics* charsPtr(static_cast<const CSymmetricCipherCharacteristics*>(
															aCharacteristicsDLL[0]->iCharacteristic));

					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iBlockSize);

					return ETrue;
					}
					
				default:
					{
					break;
					}
				}//switch (aInterfaceUid)
				
			break;
			}
				
		case KOutputSizeType:
			{
			switch (aInterfaceUid)
				{
				case KHashInterface:
				case KAsyncHashInterface:
					{
					const CHashCharacteristics* charsPtr(static_cast<const CHashCharacteristics*>(
		 										aCharacteristicsDLL[0]->iCharacteristic));

					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iOutputSize);
									
					return ETrue;
					}					
					
				default:
					{
					break;
					}
				}//switch (aInterfaceUid)

			break;
			}

		case KSupportedOperationModesType:
			{
			switch (aInterfaceUid)
				{
				case KHashInterface:
				case KAsyncHashInterface:
					{
					const CHashCharacteristics* charsPtr(static_cast<const CHashCharacteristics*>(
		 										aCharacteristicsDLL[0]->iCharacteristic));

					TUid uid = { ((CCryptoIntParam*)ruleCharValue)->Value() };
				
					TBool supported = charsPtr->IsOperationModeSupported(uid);
				
					FilterMultiTInt32L(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules,
										aLastPreference, supported);
									
					return ETrue;
					}

				case KSymmetricCipherInterface:
				case KAsyncSymmetricCipherInterface:
					{
					const CSymmetricCipherCharacteristics* charsPtr(static_cast<const CSymmetricCipherCharacteristics*>(
															aCharacteristicsDLL[0]->iCharacteristic));

					TUid uid = { ((CCryptoIntParam*)ruleCharValue)->Value() };

					TBool supported = charsPtr->IsOperationModeSupported(uid);

					FilterMultiTInt32L(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules,
										aLastPreference, supported);
										
					return ETrue;
					}

				default:
					{
					break;
					}
				}//switch (aInterfaceUid)

			break;
			}
			
		case KBlockingModeType:
			{
			switch (aInterfaceUid)
				{
				case KRandomInterface:
				case KAsyncRandomInterface:
					{
				  	const CRandomCharacteristics* charsPtr(static_cast<const CRandomCharacteristics*>(
				  									aCharacteristicsDLL[0]->iCharacteristic));
				  									
					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iBlockingMode);
					
					return ETrue;
					}

				default:
					{
					break;
					}
				}//switch (aInterfaceUid)

			break;
			}
			
		case KMaximumKeyLengthType:
			{
			switch (aInterfaceUid)
				{
				case KSymmetricCipherInterface:
				case KAsyncSymmetricCipherInterface:
					{
					const CSymmetricCipherCharacteristics* charsPtr(static_cast<const CSymmetricCipherCharacteristics*>(
															aCharacteristicsDLL[0]->iCharacteristic));

					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iMaximumKeyLength);

					return ETrue;
					}
				
				case KAsymmetricCipherInterface:
				case KAsyncAsymmetricCipherInterface:
					{
					const CAsymmetricCipherCharacteristics* charsPtr(static_cast<const CAsymmetricCipherCharacteristics*>(
															aCharacteristicsDLL[0]->iCharacteristic));
					
					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iMaximumKeyLength);
					
					return ETrue;
					}

				case KSignerInterface:
				case KAsyncSignerInterface:
				case KVerifierInterface:
				case KAsyncVerifierInterface:
					{
					const CAsymmetricSignatureCharacteristics* charsPtr(static_cast<const CAsymmetricSignatureCharacteristics*>(
																aCharacteristicsDLL[0]->iCharacteristic));

					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iMaximumKeyLength);

					return ETrue;
					}

				case KKeypairGeneratorInterface:
				case KAsyncKeypairGeneratorInterface:
					{
					const CKeypairGeneratorCharacteristics* charsPtr(static_cast<const CKeypairGeneratorCharacteristics*>(
																		aCharacteristicsDLL[0]->iCharacteristic));

					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iMaximumKeyLength);
																
					return ETrue;
					}

				default:
					{
					break;
					}
				}//switch (aInterfaceUid)

			break;
			}
			
		case KSupportedPaddingModesType:
			{
			switch (aInterfaceUid)
				{
				case KSymmetricCipherInterface:
				case KAsyncSymmetricCipherInterface:
					{
					const CSymmetricCipherCharacteristics* charsPtr(static_cast<const CSymmetricCipherCharacteristics*>(
															aCharacteristicsDLL[0]->iCharacteristic));

					TUid uid = { ((CCryptoIntParam*)ruleCharValue)->Value() };

					TBool supported = charsPtr->IsPaddingModeSupported(uid);
					
					FilterMultiTInt32L(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules,
										aLastPreference, supported);

					return ETrue;
					}

				
				case KAsymmetricCipherInterface:
				case KAsyncAsymmetricCipherInterface:
					{
					const CAsymmetricCipherCharacteristics* charsPtr(static_cast<const CAsymmetricCipherCharacteristics*>(
															aCharacteristicsDLL[0]->iCharacteristic));
															
					TUid uid = { ((CCryptoIntParam*)ruleCharValue)->Value() };

					TBool supported = charsPtr->IsPaddingModeSupported(uid);

					FilterMultiTInt32L(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules,
										aLastPreference, supported);											

					return ETrue;
					}

				case KSignerInterface:
				case KAsyncSignerInterface:
				case KVerifierInterface:
				case KAsyncVerifierInterface:
					{
					const CAsymmetricSignatureCharacteristics* charsPtr(static_cast<const CAsymmetricSignatureCharacteristics*>(
																aCharacteristicsDLL[0]->iCharacteristic));

					TUid uid = { ((CCryptoIntParam*)ruleCharValue)->Value() };

					TBool supported = charsPtr->IsPaddingModeSupported(uid);

					FilterMultiTInt32L(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules,
										aLastPreference, supported);

					return ETrue;
					}

				default:
					{
					break;
					}
				}//switch (aInterfaceUid)

			break;
			}
			
		case KKeySupportModeType:
			{
			switch (aInterfaceUid)
				{
				case KSymmetricCipherInterface:
				case KAsyncSymmetricCipherInterface:
					{
					const CSymmetricCipherCharacteristics* charsPtr(static_cast<const CSymmetricCipherCharacteristics*>(
															aCharacteristicsDLL[0]->iCharacteristic));
					
					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iKeySupportMode);
					
					return ETrue;
					}

				case KAsymmetricCipherInterface:
				case KAsyncAsymmetricCipherInterface:
					{
					const CAsymmetricCipherCharacteristics* charsPtr(static_cast<const CAsymmetricCipherCharacteristics*>(
															aCharacteristicsDLL[0]->iCharacteristic));
					
					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iKeySupportMode);

					return ETrue;
					}
					
				case KSignerInterface:
				case KAsyncSignerInterface:
				case KVerifierInterface:
				case KAsyncVerifierInterface:
					{
					const CAsymmetricSignatureCharacteristics* charsPtr(static_cast<const CAsymmetricSignatureCharacteristics*>(
																aCharacteristicsDLL[0]->iCharacteristic));

					FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
								aRules, aLastPreference, charsPtr->iKeySupportMode);																

					return ETrue;
					}

				default:
					{
					break;
					}
				}//switch (aInterfaceUid)

			break;
			}
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT				
		case KMacModeType:	
			{
			const CMacCharacteristics* charsPtr(static_cast<const CMacCharacteristics*>(aCharacteristicsDLL[0]->iCharacteristic));
			// The following method implements the rule for MAC interface's 'iMacMode' characteristics
			FilterTInt32L(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules, aLastPreference, charsPtr->iMacMode);
			return ETrue;
			}
#endif			
		default:
			{
			return EFalse;
			}
		}//switch (ruleCharValue->Uid().iUid)

	//Try adding it as optional
	TryAddToOptionalCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules);
		
	return ETrue;
	}

TBool CRuleSelector::FilterExtendedCharacteristicsL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
													CSelectionRuleContent* aRules, TInt& aLastPreference)
	{
	//Load all the plugins
	LoadPluginsL();
	
	//Look for the dll name
	TInt pluginCount = iPluginDllList.Count();
	TInt i=0;
	
	for (i=0;i<pluginCount;i++)
		{
		TInt pos = iPluginDllList[i].FileName().LocateReverseF('\\');
		TFileName dllFile = iPluginDllList[i].FileName().Right(pos+3);

		if (dllFile.CompareF(aCharacteristicsDLL[0]->iDllName) == 0)
			{
			break;
			}
		}
	
	//we should always find the dll	
	ASSERT(i<pluginCount);
	
	//Try to get entry point of the extended characteristic function
	GetExtendedCharacteristicsFuncL getExtendedCharFuncL = (GetExtendedCharacteristicsFuncL)iPluginDllList[i].Lookup(EGetExtendedCharacteristicOrdinal);
	
	const CCharacteristics* cmn(aCharacteristicsDLL[0]->iCharacteristic);
	TUid implUid={cmn->iImplementationUid};
	
	//Retrieve the extended characteristics
	CExtendedCharacteristics* extendedChar=NULL;
	
	(getExtendedCharFuncL)(implUid, extendedChar);
	
	
	if (extendedChar)
		{
		CleanupStack::PushL(extendedChar);
		const CCryptoParam* ruleCharValue = aRules->GetCharacteristicValue();

		switch (ruleCharValue->Type())
			{
			case CCryptoParam::EInt:
				{
				TInt charValue = 0;
				
				TRAPD(err, charValue = extendedChar->GetTIntCharacteristicL(ruleCharValue->Uid()));

				if (err)
					{
					//Not found
					CleanupStack::PopAndDestroy(extendedChar);
					return EFalse;
					}

				TInt operator_r = aRules->GetOperator();

				if (operator_r == EOpAscendingSort || operator_r == EOpDescendingSort)
					{
					TInt intCount = aCharacteristicsDLL.Count();
					
					//Fill the array with the extended characteristic value
					for (TInt i = 0; i < intCount; ++i)
						{
						TUid implimentationUid = { aCharacteristicsDLL[i]->iCharacteristic->iImplementationUid };
						CExtendedCharacteristics* extChar = NULL;
						TInt intI=0;
						
						for (intI = 0; intI < pluginCount; intI++)
							{
							TInt pos = iPluginDllList[intI].FileName().LocateReverseF('\\');
							TFileName dllFile = iPluginDllList[intI].FileName().Right(pos+3);

							if (dllFile.CompareF(aCharacteristicsDLL[i]->iDllName) == 0)
								{
								break;
								}
							}
						
						//we should always find the dll	
						ASSERT(intI < pluginCount);
							
						//Try to get entry point of the extended characteristic function
						GetExtendedCharacteristicsFuncL intGetExtendedCharFuncL = (GetExtendedCharacteristicsFuncL)iPluginDllList[intI].Lookup(EGetExtendedCharacteristicOrdinal);

						(intGetExtendedCharFuncL)(implimentationUid, extChar);
						
						//Make sure the extChar contains the characteristic it is trying to sort.
						//As RArray::Sort() could not handle Leave, we need to leave here.
						extChar->GetTIntCharacteristicL(ruleCharValue->Uid());
						aCharacteristicsDLL[i]->iExtendedCharacteristic = extChar;
						aCharacteristicsDLL[i]->iSortUid = ruleCharValue->Uid().iUid;
						}
					}

				FilterTIntL(aInterfaceUid, aCharacteristicsDLL,
							aRules, aLastPreference, charValue);

				CleanupStack::PopAndDestroy(extendedChar);
				return ETrue;
				}

			case CCryptoParam::EDesC8:
				{
			    const TDesC8* charValue = NULL;
			    
			    TRAPD(err, charValue = &(extendedChar->GetTDesC8CharacteristicL(ruleCharValue->Uid())));
			    
				if (err)
					{
					//Not found
					CleanupStack::PopAndDestroy(extendedChar);
					return EFalse;
					}

				TInt operator_r = aRules->GetOperator();

				if (operator_r == EOpAscendingSort || operator_r == EOpDescendingSort)
					{
					TInt desCount = aCharacteristicsDLL.Count();
					
					//Fill the array with the extended characteristic value
					for (TInt i = 0; i < desCount; ++i)
						{
						TUid implimentationUid = { aCharacteristicsDLL[i]->iCharacteristic->iImplementationUid };
						CExtendedCharacteristics* extChar;
						TInt desI=0;
						
						for (desI = 0; desI < pluginCount; desI++)
							{
							TInt pos = iPluginDllList[desI].FileName().LocateReverseF('\\');
							TFileName dllFile = iPluginDllList[desI].FileName().Right(pos+3);

							if (dllFile.CompareF(aCharacteristicsDLL[i]->iDllName) == 0)
								{
								break;
								}
							}
   						
						//we should always find the dll	
						ASSERT(desI < pluginCount);
							
						//Try to get entry point of the extended characteristic function
						GetExtendedCharacteristicsFuncL desGetExtendedCharFuncL = (GetExtendedCharacteristicsFuncL)iPluginDllList[desI].Lookup(EGetExtendedCharacteristicOrdinal);

						(desGetExtendedCharFuncL)(implimentationUid, extChar);
						
						//Make sure the extChar contains the characteristic it is trying to sort.
						//As RArray::Sort() could not handle Leave, we need to leave here.
						extChar->GetTDesC8CharacteristicL(ruleCharValue->Uid());
						aCharacteristicsDLL[i]->iExtendedCharacteristic = extChar;
						aCharacteristicsDLL[i]->iSortUid = ruleCharValue->Uid().iUid;
						}
					}

				FilterExtendedTDesC8L(aInterfaceUid, aCharacteristicsDLL, aRules, aLastPreference, *charValue);
				CleanupStack::PopAndDestroy(extendedChar);
				return ETrue;
				}

			default:
				{
				CleanupStack::PopAndDestroy(extendedChar);
				return EFalse;
				}
			}
		}

	return EFalse;
	}
	
void CRuleSelector::FilterTInt32L(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics,
									CSelectionRuleContent* aRules, TInt& aLastPreference, TInt32 aCharValue)
	{
	const CCryptoParam* charValue = aRules->GetCharacteristicValue();
	TInt operator_r = aRules->GetOperator();

	if (operator_r == EOpEqual)
		{
		if (aCharValue == ((CCryptoIntParam*)charValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
									
			AddToCharListL(aInterfaceUid, aCharacteristics, aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpNotEqual)
		{
		if (aCharValue != ((CCryptoIntParam*)charValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
									
			AddToCharListL(aInterfaceUid, aCharacteristics, aLastPreference);
			return;
			}
		}
	else
		{
		delete &aCharacteristics;
		return;
		}

	//Try adding it as optional
	TryAddToOptionalCharListL(aInterfaceUid, aCharacteristics, aRules);
	}
	
void CRuleSelector::FilterMultiTInt32L(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristicsDLL,
										CSelectionRuleContent* aRules, TInt& aLastPreference,
										TBool aSupport)
	{
	TInt operator_r = aRules->GetOperator();

	if (operator_r == EOpEqual)
		{
		if (aSupport)
			{
			//
			//The array of TInt32 contain the required element, so build it into
			//iSelectedInterfaceCharacteristicsMap
			//
										
			AddToCharListL(aInterfaceUid, aCharacteristicsDLL, aLastPreference);
			
			return;
			}
		}
	else
	if (operator_r == EOpNotEqual)
		{		
		if (!aSupport)
			{
			//
			//The array of TInt32 does not contain the specified element, so
			//build it into iSelectedInterfaceCharacteristicsMap
			//
													
			AddToCharListL(aInterfaceUid, aCharacteristicsDLL, aLastPreference);
			return;
			}
		}
	else
		{
		delete &aCharacteristicsDLL;
		return;
		}

	//Try adding it as optional
	TryAddToOptionalCharListL(aInterfaceUid, aCharacteristicsDLL, aRules);
	}

TInt CRuleSelector::AscendCreatorName(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CCharacteristics* charLeft(aLeft.iCharacteristic);
	const CCharacteristics* charRight(aRight.iCharacteristic);

	return charLeft->iCreatorName.CompareC(charRight->iCreatorName);
}

TInt CRuleSelector::DescendCreatorName(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CCharacteristics* charLeft(aLeft.iCharacteristic);
	const CCharacteristics* charRight(aRight.iCharacteristic);
	
	return charRight->iCreatorName.CompareC(charLeft->iCreatorName);
}

TInt CRuleSelector::AscendMaxConcurrencySupported(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CCharacteristics* charLeft(aLeft.iCharacteristic);
	const CCharacteristics* charRight(aRight.iCharacteristic);
		
	return (charLeft->iMaxConcurrencySupported) - (charRight->iMaxConcurrencySupported);
}

TInt CRuleSelector::DescendMaxConcurrencySupported(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CCharacteristics* charLeft(aLeft.iCharacteristic);
	const CCharacteristics* charRight(aRight.iCharacteristic);
		
	return (charRight->iMaxConcurrencySupported) - (charLeft->iMaxConcurrencySupported);
}

TInt CRuleSelector::AscendLatency(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CCharacteristics* charLeft(aLeft.iCharacteristic);
	const CCharacteristics* charRight(aRight.iCharacteristic);
		
	return (charLeft->iLatency) - (charRight->iLatency);
}

TInt CRuleSelector::DescendLatency(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CCharacteristics* charLeft(aLeft.iCharacteristic);
	const CCharacteristics* charRight(aRight.iCharacteristic);
		
	return (charRight->iLatency) - (charLeft->iLatency);
}

TInt CRuleSelector::AscendThroughput(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CCharacteristics* charLeft(aLeft.iCharacteristic);
	const CCharacteristics* charRight(aRight.iCharacteristic);
		
	return (charLeft->iThroughput) - (charRight->iThroughput);
}

TInt CRuleSelector::DescendThroughput(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CCharacteristics* charLeft(aLeft.iCharacteristic);
	const CCharacteristics* charRight(aRight.iCharacteristic);
		
	return (charRight->iThroughput) - (charLeft->iThroughput);
}

TInt CRuleSelector::AscendHashBlockSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CHashCharacteristics* charLeft(static_cast<const CHashCharacteristics*>(aLeft.iCharacteristic));
	const CHashCharacteristics* charRight(static_cast<const CHashCharacteristics*>(aRight.iCharacteristic));
		
	return (charLeft->iBlockSize) - (charRight->iBlockSize);
}

TInt CRuleSelector::DescendHashBlockSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CHashCharacteristics* charLeft(static_cast<const CHashCharacteristics*>(aLeft.iCharacteristic));
	const CHashCharacteristics* charRight(static_cast<const CHashCharacteristics*>(aRight.iCharacteristic));
		
	return (charRight->iBlockSize) - (charLeft->iBlockSize);
}

TInt CRuleSelector::AscendSymmetricCipherBlockSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CSymmetricCipherCharacteristics* charLeft(static_cast<const CSymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CSymmetricCipherCharacteristics* charRight(static_cast<const CSymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (charLeft->iBlockSize) - (charRight->iBlockSize);
}

TInt CRuleSelector::DescendSymmetricCipherBlockSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CSymmetricCipherCharacteristics* charLeft(static_cast<const CSymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CSymmetricCipherCharacteristics* charRight(static_cast<const CSymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (charRight->iBlockSize) - (charLeft->iBlockSize);
}

TInt CRuleSelector::AscendHashOutputSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CHashCharacteristics* charLeft(static_cast<const CHashCharacteristics*>(aLeft.iCharacteristic));
	const CHashCharacteristics* charRight(static_cast<const CHashCharacteristics*>(aRight.iCharacteristic));
		
	return (charLeft->iOutputSize) - (charRight->iOutputSize);
}

TInt CRuleSelector::DescendHashOutputSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CHashCharacteristics* charLeft(static_cast<const CHashCharacteristics*>(aLeft.iCharacteristic));
	const CHashCharacteristics* charRight(static_cast<const CHashCharacteristics*>(aRight.iCharacteristic));
		
	return (charRight->iOutputSize) - (charLeft->iOutputSize);
}

TInt CRuleSelector::AscendRandomBlockingMode(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CRandomCharacteristics* charLeft(static_cast<const CRandomCharacteristics*>(aLeft.iCharacteristic));
	const CRandomCharacteristics* charRight(static_cast<const CRandomCharacteristics*>(aRight.iCharacteristic));
		
	return (charLeft->iBlockingMode) - (charRight->iBlockingMode);
}

TInt CRuleSelector::DescendRandomBlockingMode(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CRandomCharacteristics* charLeft(static_cast<const CRandomCharacteristics*>(aLeft.iCharacteristic));
	const CRandomCharacteristics* charRight(static_cast<const CRandomCharacteristics*>(aRight.iCharacteristic));
		
	return (charRight->iBlockingMode) - (charLeft->iBlockingMode);
}

TInt CRuleSelector::AscendSymmetricCipherKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CSymmetricCipherCharacteristics* charLeft(static_cast<const CSymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CSymmetricCipherCharacteristics* charRight(static_cast<const CSymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (charLeft->iMaximumKeyLength) - (charRight->iMaximumKeyLength);
}

TInt CRuleSelector::DescendSymmetricCipherKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CSymmetricCipherCharacteristics* charLeft(static_cast<const CSymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CSymmetricCipherCharacteristics* charRight(static_cast<const CSymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (charRight->iMaximumKeyLength) - (charLeft->iMaximumKeyLength);
}

TInt CRuleSelector::AscendAsymmetricCipherKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CAsymmetricCipherCharacteristics* charLeft(static_cast<const CAsymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CAsymmetricCipherCharacteristics* charRight(static_cast<const CAsymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (charLeft->iMaximumKeyLength) - (charRight->iMaximumKeyLength);
}

TInt CRuleSelector::DescendAsymmetricCipherKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CAsymmetricCipherCharacteristics* charLeft(static_cast<const CAsymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CAsymmetricCipherCharacteristics* charRight(static_cast<const CAsymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (charRight->iMaximumKeyLength) - (charLeft->iMaximumKeyLength);
}

TInt CRuleSelector::AscendAsymmetricSignatureKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CAsymmetricSignatureCharacteristics* charLeft(static_cast<const CAsymmetricSignatureCharacteristics*>(aLeft.iCharacteristic));
	const CAsymmetricSignatureCharacteristics* charRight(static_cast<const CAsymmetricSignatureCharacteristics*>(aRight.iCharacteristic));
		
	return (charLeft->iMaximumKeyLength) - (charRight->iMaximumKeyLength);
}

TInt CRuleSelector::DescendAsymmetricSignatureKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CAsymmetricSignatureCharacteristics* charLeft(static_cast<const CAsymmetricSignatureCharacteristics*>(aLeft.iCharacteristic));
	const CAsymmetricSignatureCharacteristics* charRight(static_cast<const CAsymmetricSignatureCharacteristics*>(aRight.iCharacteristic));
		
	return (charRight->iMaximumKeyLength) - (charLeft->iMaximumKeyLength);
}

TInt CRuleSelector::AscendSymmetricCipherKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CSymmetricCipherCharacteristics* cmnLeft(static_cast<const CSymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CSymmetricCipherCharacteristics* cmnRight(static_cast<const CSymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (cmnLeft->iKeySupportMode) - (cmnRight->iKeySupportMode);
}

TInt CRuleSelector::DescendSymmetricCipherKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CSymmetricCipherCharacteristics* cmnLeft(static_cast<const CSymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CSymmetricCipherCharacteristics* cmnRight(static_cast<const CSymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (cmnRight->iKeySupportMode) - (cmnLeft->iKeySupportMode);
}

TInt CRuleSelector::AscendAsymmetricCipherKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CAsymmetricCipherCharacteristics* cmnLeft(static_cast<const CAsymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CAsymmetricCipherCharacteristics* cmnRight(static_cast<const CAsymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (cmnLeft->iKeySupportMode) - (cmnRight->iKeySupportMode);
}

TInt CRuleSelector::DescendAsymmetricCipherKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CAsymmetricCipherCharacteristics* cmnLeft(static_cast<const CAsymmetricCipherCharacteristics*>(aLeft.iCharacteristic));
	const CAsymmetricCipherCharacteristics* cmnRight(static_cast<const CAsymmetricCipherCharacteristics*>(aRight.iCharacteristic));
		
	return (cmnRight->iKeySupportMode) - (cmnLeft->iKeySupportMode);
}

TInt CRuleSelector::AscendAsymmetricSignatureKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CAsymmetricSignatureCharacteristics* cmnLeft(static_cast<const CAsymmetricSignatureCharacteristics*>(aLeft.iCharacteristic));
	const CAsymmetricSignatureCharacteristics* cmnRight(static_cast<const CAsymmetricSignatureCharacteristics*>(aRight.iCharacteristic));
		
	return (cmnLeft->iKeySupportMode) - (cmnRight->iKeySupportMode);
}

TInt CRuleSelector::DescendAsymmetricSignatureKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CAsymmetricSignatureCharacteristics* cmnLeft(static_cast<const CAsymmetricSignatureCharacteristics*>(aLeft.iCharacteristic));
	const CAsymmetricSignatureCharacteristics* cmnRight(static_cast<const CAsymmetricSignatureCharacteristics*>(aRight.iCharacteristic));
		
	return (cmnRight->iKeySupportMode) - (cmnLeft->iKeySupportMode);
}

TInt CRuleSelector::AscendAsymmetricKeypairGeneratorKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CKeypairGeneratorCharacteristics* charLeft(static_cast<const CKeypairGeneratorCharacteristics*>(aLeft.iCharacteristic));
	const CKeypairGeneratorCharacteristics* charRight(static_cast<const CKeypairGeneratorCharacteristics*>(aRight.iCharacteristic));
		
	return (charLeft->iMaximumKeyLength) - (charRight->iMaximumKeyLength);
}

TInt CRuleSelector::DescendAsymmetricKeypairGeneratorKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	const CKeypairGeneratorCharacteristics* charLeft(static_cast<const CKeypairGeneratorCharacteristics*>(aLeft.iCharacteristic));
	const CKeypairGeneratorCharacteristics* charRight(static_cast<const CKeypairGeneratorCharacteristics*>(aRight.iCharacteristic));
		
	return (charRight->iMaximumKeyLength) - (charLeft->iMaximumKeyLength);
}

void CRuleSelector::FilterCreatorNameL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
										CSelectionRuleContent* aRules, TInt& aLastPreference)
	{
	TInt operator_r = aRules->GetOperator();
	
	if (operator_r == EOpEqual)
		{
		const CCharacteristics* cmn(aCharacteristicsDLL[0]->iCharacteristic);
		
		const CCryptoDesC16Param* charValue = static_cast<const CCryptoDesC16Param*>(aRules->GetCharacteristicValue());
		
		if (cmn->iCreatorName.Compare(charValue->Value()) == 0)
			{
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpNotEqual)
		{
		const CCharacteristics* cmn(aCharacteristicsDLL[0]->iCharacteristic);

		const CCryptoDesC16Param* charValue = static_cast<const CCryptoDesC16Param*>(aRules->GetCharacteristicValue());
		
		if (cmn->iCreatorName.Compare(charValue->Value()))
			{
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpAscendingSort)
		{
		const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendCreatorName);
	
		aCharacteristicsDLL.Sort(sortOrder);
		
		return;
		}
	else
	if (operator_r == EOpDescendingSort)
		{
		const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendCreatorName);
	
		aCharacteristicsDLL.Sort(sortOrder);
		
		return;
		}
	else
		{
		//Delete this CRulesCharacteristicsAndPluginName
		delete aCharacteristicsDLL[0];
		return;
		}

	//Try adding it as optional
	TryAddToOptionalCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules);
	}

void CRuleSelector::FilterExtendedTDesC8L(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
											CSelectionRuleContent* aRules, TInt& aLastPreference, const TDesC8& aCharValue)
	{
	TInt operator_r = aRules->GetOperator();
	
	if (operator_r == EOpEqual)
		{
		const CCryptoDesC8Param* charValue = static_cast<const CCryptoDesC8Param*>(aRules->GetCharacteristicValue());

		if (aCharValue.Compare(charValue->Value()) == 0)
			{
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpNotEqual)
		{
		const CCryptoDesC8Param* charValue = static_cast<const CCryptoDesC8Param*>(aRules->GetCharacteristicValue());

		if (aCharValue.Compare(charValue->Value()))
			{
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpAscendingSort)
		{
		const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendExtendedTDesC8L);
	
		aCharacteristicsDLL.Sort(sortOrder);
		
		TInt count = aCharacteristicsDLL.Count();
				
		//Clean up the array's extended characteristic value
		for (TInt i = 0; i < count; ++i)
			{
			delete aCharacteristicsDLL[i]->iExtendedCharacteristic;
			aCharacteristicsDLL[i]->iExtendedCharacteristic = NULL;
			}
		
		return;
		}
	else
	if (operator_r == EOpDescendingSort)
		{
		const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendExtendedTDesC8L);
	
		aCharacteristicsDLL.Sort(sortOrder);
	
		TInt count = aCharacteristicsDLL.Count();
				
		//Clean up the array's extended characteristic value
		for (TInt i = 0; i < count; ++i)
			{
			delete aCharacteristicsDLL[i]->iExtendedCharacteristic;
			aCharacteristicsDLL[i]->iExtendedCharacteristic = NULL;
			}
			
		return;	
		}
	else
		{
		delete aCharacteristicsDLL[0];
		return;
		}
	
	//Try adding it as optional
	TryAddToOptionalCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules);
	}
	
void CRuleSelector::FilterTBoolL(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics,
									CSelectionRuleContent* aRules, TInt& aLastPreference, TBool aCharValue)
	{
	const CCryptoParam* charValue = aRules->GetCharacteristicValue();
	TInt operator_r = aRules->GetOperator();

	if (operator_r == EOpEqual)
		{
		if (aCharValue == ((CCryptoIntParam*)charValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
									
			AddToCharListL(aInterfaceUid, aCharacteristics, aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpNotEqual)
		{
		if (aCharValue != ((CCryptoIntParam*)charValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
									
			AddToCharListL(aInterfaceUid, aCharacteristics, aLastPreference);
			return;
			}
		}
	else
		{
		delete &aCharacteristics;
		return;
		}

	//Try adding it as optional
	TryAddToOptionalCharListL(aInterfaceUid, aCharacteristics, aRules);
	}

void CRuleSelector::FilterTIntL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
								CSelectionRuleContent* aRules, TInt& aLastPreference, TInt aCharValue)
	{
	const CCryptoParam* ruleCharValue = aRules->GetCharacteristicValue();
	TInt operator_r = aRules->GetOperator();

	if (operator_r == EOpEqual)
		{
		if (aCharValue == ((CCryptoIntParam*)ruleCharValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
											
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpNotEqual)
		{
		if (aCharValue != ((CCryptoIntParam*)ruleCharValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
											
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpGreaterThan)
		{
		if (aCharValue > ((CCryptoIntParam*)ruleCharValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
				
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpLessThan)
		{
		if (aCharValue < ((CCryptoIntParam*)ruleCharValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
								
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpGreaterThanOrEqual)
		{
		if (aCharValue >= ((CCryptoIntParam*)ruleCharValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
									
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpLessThanOrEqual)
		{
		if (aCharValue <= ((CCryptoIntParam*)ruleCharValue)->Value())
			{
			//
			//Met the rule and build it into iSelectedInterfaceCharacteristicsMap
			//
					
			AddToCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aLastPreference);
			return;
			}
		}
	else
	if (operator_r == EOpAscendingSort)
		{
		switch (ruleCharValue->Uid().iUid)
			{
			case KMaxConcurrencySupportedType:
				{		
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendMaxConcurrencySupported);
		
				aCharacteristicsDLL.Sort(sortOrder);
				break;
				}
				
			case KLatencyType:
				{
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendLatency);
		
				aCharacteristicsDLL.Sort(sortOrder);
				break;
				}
				
			case KThroughputType:
				{
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendThroughput);
		
				aCharacteristicsDLL.Sort(sortOrder);
				break;
				}
				
			case KBlockSizeType:
			  	{
			  	if (aInterfaceUid == KHashInterface || aInterfaceUid == KAsyncHashInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendHashBlockSize);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}	
			  	else if (aInterfaceUid == KSymmetricCipherInterface || aInterfaceUid == KAsyncSymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendSymmetricCipherBlockSize);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}
				else
					{
					//Characteristic not supported
					User::Leave(KErrNotSupported);
					}

			  	break;
			  	}
			  	
			case KOutputSizeType:
			  	{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendHashOutputSize);
		
					aCharacteristicsDLL.Sort(sortOrder);
			  	break;
			  	}
			  	
			case KBlockingModeType:
			  	{
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendRandomBlockingMode);
		
				aCharacteristicsDLL.Sort(sortOrder);
			  	break;
			  	}
			  	  	
			case KMaximumKeyLengthType:
			  	{
			  	if (aInterfaceUid == KSymmetricCipherInterface || aInterfaceUid == KAsyncSymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendSymmetricCipherKeyLength);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}				
			  	else if (aInterfaceUid == KAsymmetricCipherInterface || aInterfaceUid == KAsyncAsymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendAsymmetricCipherKeyLength);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}				
			  	else if (aInterfaceUid == KSignerInterface || aInterfaceUid == KAsyncSignerInterface ||
		  				aInterfaceUid == KVerifierInterface || aInterfaceUid == KVerifierInterface )
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendAsymmetricSignatureKeyLength);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}
				else if (aInterfaceUid == KKeypairGeneratorInterface ||
		  				aInterfaceUid == KAsyncKeypairGeneratorInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendAsymmetricKeypairGeneratorKeyLength);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}
			  	else
					{
					//Characteristic not supported
					User::Leave(KErrNotSupported);
					}
					
			  	break;
			  	}
			
			case KKeySupportModeType:
			  	{
			  	if (aInterfaceUid == KSymmetricCipherInterface || aInterfaceUid == KAsyncSymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendSymmetricCipherKeySupport);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}				
			  	else if (aInterfaceUid == KAsymmetricCipherInterface || aInterfaceUid == KAsyncAsymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendAsymmetricCipherKeySupport);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}				
			  	else if (aInterfaceUid == KSignerInterface || aInterfaceUid == KAsyncSignerInterface ||
		  				aInterfaceUid == KVerifierInterface || aInterfaceUid == KVerifierInterface )
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendAsymmetricSignatureKeySupport);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}
				else
					{
					//Characteristic not supported
					User::Leave(KErrNotSupported);
					}
					
			  	break;
			  	}

			default:
				{
				//Must be an Extended Characteristic
	
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(AscendExtendedCharacteristicL);
		
				aCharacteristicsDLL.Sort(sortOrder);
				
				TInt count = aCharacteristicsDLL.Count();
					
				//Clean up the array's extended characteristic value
				for (TInt i = 0; i < count; ++i)
					{
					delete aCharacteristicsDLL[i]->iExtendedCharacteristic;
					aCharacteristicsDLL[i]->iExtendedCharacteristic = NULL;
					}
				}
			}//switch (ruleCharValue->Uid().iUid)

		return;
		}
	else
	if (operator_r == EOpDescendingSort)
		{
		switch (ruleCharValue->Uid().iUid)
			{
			case KMaxConcurrencySupportedType:
				{
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendMaxConcurrencySupported);
		
				aCharacteristicsDLL.Sort(sortOrder);
				break;
				}
				
			case KLatencyType:
				{
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendLatency);
		
				aCharacteristicsDLL.Sort(sortOrder);
				break;
				}
				
			case KThroughputType:
				{
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendThroughput);
		
				aCharacteristicsDLL.Sort(sortOrder);
				break;
				}
				
			case KBlockSizeType:
			  	{
			  	if (aInterfaceUid == KHashInterface || aInterfaceUid == KAsyncHashInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendHashBlockSize);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}
			  	else if (aInterfaceUid == KSymmetricCipherInterface || aInterfaceUid == KAsyncSymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendSymmetricCipherBlockSize);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}
				else
					{
					//Characteristic not supported
					User::Leave(KErrNotSupported);
					}
					
			  	break;
			  	}
			  	
			case KOutputSizeType:
			  	{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendHashOutputSize);
			
					aCharacteristicsDLL.Sort(sortOrder);
		    	break;
			  	}
			  	
			case KBlockingModeType:
			  	{
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendRandomBlockingMode);
		
				aCharacteristicsDLL.Sort(sortOrder);
			  	break;
			  	}
			  	  	
			case KMaximumKeyLengthType:
			  	{
			  	if (aInterfaceUid == KSymmetricCipherInterface || aInterfaceUid == KAsyncSymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendSymmetricCipherKeyLength);
		
					aCharacteristicsDLL.Sort(sortOrder);
			  		}				
			  	else if (aInterfaceUid == KAsymmetricCipherInterface || aInterfaceUid == KAsyncAsymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendAsymmetricCipherKeyLength);
		
					aCharacteristicsDLL.Sort(sortOrder);
			  		}				
			  	else if (aInterfaceUid == KSignerInterface || aInterfaceUid == KAsyncSignerInterface ||
		  			aInterfaceUid == KVerifierInterface || aInterfaceUid == KVerifierInterface )
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendAsymmetricSignatureKeyLength);
		
					aCharacteristicsDLL.Sort(sortOrder);
			  		}
				else if (aInterfaceUid == KKeypairGeneratorInterface ||
		  				aInterfaceUid == KAsyncKeypairGeneratorInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendAsymmetricKeypairGeneratorKeyLength);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}
				else
					{
					//Characteristic not supported
					User::Leave(KErrNotSupported);
					}
					
			  	break;
			  	}
			
			case KKeySupportModeType:
			  	{
			  	if (aInterfaceUid == KSymmetricCipherInterface || aInterfaceUid == KAsyncSymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendSymmetricCipherKeySupport);
		
					aCharacteristicsDLL.Sort(sortOrder);
			  		}				
			  	else if (aInterfaceUid == KAsymmetricCipherInterface || aInterfaceUid == KAsyncAsymmetricCipherInterface)
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendAsymmetricCipherKeySupport);
		
					aCharacteristicsDLL.Sort(sortOrder);
			  		}				
			  	else if (aInterfaceUid == KSignerInterface || aInterfaceUid == KAsyncSignerInterface ||
		  			aInterfaceUid == KVerifierInterface || aInterfaceUid == KVerifierInterface )
			  		{
					const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendAsymmetricSignatureKeySupport);
		
					aCharacteristicsDLL.Sort(sortOrder);			  		
			  		}
				else
					{
					//Characteristic not supported
					User::Leave(KErrNotSupported);
					}
					
			  	break;
			  	}
			
			default:
				{
				//Must be an Extended Characteristic
	
				const TLinearOrder<CRulesCharacteristicsAndPluginName> sortOrder(DescendExtendedCharacteristicL);
		
				aCharacteristicsDLL.Sort(sortOrder);
				
				TInt count = aCharacteristicsDLL.Count();
					
				//Clean up the array's extended characteristic value
				for (TInt i = 0; i < count; ++i)
					{
					delete aCharacteristicsDLL[i]->iExtendedCharacteristic;
					aCharacteristicsDLL[i]->iExtendedCharacteristic = NULL;
					}
				}
			}
		
		return;
		}
		
	//Try adding it as optional
	TryAddToOptionalCharListL(aInterfaceUid, *(aCharacteristicsDLL[0]), aRules);
	}


void CRuleSelector::TryAddToOptionalCharListL(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics,
												CSelectionRuleContent* aRules)
{
	if (iToggleUseMap)
		{
		//Only if we are not sorting the list
		if (aRules->IsOptionalRule())
			{
			//Optional: Build it to the end of iSelectedInterfaceCharacteristicsMap
			AddOptionalToCharListL(aInterfaceUid, aCharacteristics);
			}
		else
			{
			delete &aCharacteristics;
			}
		}
}

void CRuleSelector::AddToCharListL(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics, TInt& aLastPreference)
{
	RPointerArray<CRulesCharacteristicsAndPluginName>** charsListPtr = NULL;
	RPointerArray<CRulesCharacteristicsAndPluginName>* charsList=NULL;
	
	if (iUseMapAToFilter)
		{
		charsListPtr = iSelectedInterfaceCharacteristics_MapB.Find(aInterfaceUid);
		}
	else
		{
		charsListPtr = iSelectedInterfaceCharacteristics_MapA.Find(aInterfaceUid);
		}
				
	//create new one if it is not in the map
	if (!charsListPtr)
		{
		RPointerArray<CRulesCharacteristicsAndPluginName>* newCharsList = new(ELeave) RPointerArray<CRulesCharacteristicsAndPluginName>;
		
		charsList = newCharsList;
	
		if (iUseMapAToFilter)
			{
			iSelectedInterfaceCharacteristics_MapB.InsertL(aInterfaceUid, newCharsList);
			}
		else
			{
			iSelectedInterfaceCharacteristics_MapA.InsertL(aInterfaceUid, newCharsList);
			}
		}
	else
		{
		//Use the existing one.
		charsList = *charsListPtr;
		}
		
	//Check the insert possition is greater than the number of objects currently in the array
	if (charsList->Count() > aLastPreference)
		{
		charsList->InsertL(&aCharacteristics, aLastPreference);
		}
	else
		{
		charsList->AppendL(&aCharacteristics);
		}
	
	aLastPreference++;
}

void CRuleSelector::AddOptionalToCharListL(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics)
{
	if(iToggleUseMap)
	{
	RPointerArray<CRulesCharacteristicsAndPluginName>** charsListPtr = NULL;
	RPointerArray<CRulesCharacteristicsAndPluginName>* charsList=NULL;
	
	if (iUseMapAToFilter)
		{
		charsListPtr = iSelectedInterfaceCharacteristics_MapB.Find(aInterfaceUid);
		}
	else
		{
		charsListPtr = iSelectedInterfaceCharacteristics_MapA.Find(aInterfaceUid);
		}
				
	//create new one if it is not in the map
	if (!charsListPtr)
		{
		RPointerArray<CRulesCharacteristicsAndPluginName>* newCharsList = new(ELeave) RPointerArray<CRulesCharacteristicsAndPluginName>;
		
		charsList = newCharsList;
	
		if (iUseMapAToFilter)
			{
			iSelectedInterfaceCharacteristics_MapB.InsertL(aInterfaceUid, newCharsList);
			}
		else
			{
			iSelectedInterfaceCharacteristics_MapA.InsertL(aInterfaceUid, newCharsList);
			}
		}
	else
		{
		//Use the existing one.
		charsList = *charsListPtr;
		}
	
	//Add it to the end of the list
	charsList->AppendL(&aCharacteristics);
	}
}

void CRuleSelector::ConstructMapAL()
	{
	TInt interfaceCount = sizeof(KInterfacesUids)/sizeof(KInterfacesUids[0]);
	
	for (TInt i = 0; i < interfaceCount; ++i)
		{
		RPointerArray<CRulesCharacteristicsAndPluginName>* characteristicsLists = new(ELeave) RPointerArray<CRulesCharacteristicsAndPluginName>;

			
		CryptoSpiUtil::RetrieveCharacteristicsL(KInterfacesUids[i].iUid, *characteristicsLists);
		iSelectedInterfaceCharacteristics_MapA.InsertL(KInterfacesUids[i].iUid, characteristicsLists);	
		}
	}

TInt CRuleSelector::FindPlugin(TUid aInterfaceUid, TUid aAlgorithmUid, TUid& aImplementationId, TFileName& aDllName)
	{
	TInt ret=KErrNone;
	
	/**
	 * 'iActiveInterfaceCharacteristics_Map' contains the sorted out interface's 
	 * characteristics/plugins according to the rule defined by user application.
	 * We collect all the interface's characteristics in 'list'. 
	 */
	RPointerArray<CRulesCharacteristicsAndPluginName>* const* list=iActiveInterfaceCharacteristics_Map->Find(aInterfaceUid.iUid);
	
	if (list)
		{
		/**
		 * Fetches the index which was set on last successful plugin search so
		 * as to start the search for a different algorithm implementation/plugin.
		 * The index was set by calling 
		 * SetSearchRecord(aInterfaceUid, index_to_start_for_next_serach)   
		 */
		
		TInt* nextCharToTryPtr=iNextTryCharacteristicsIndex.Find(aInterfaceUid.iUid);
		TInt nextCharToTry=0;
		
		if (nextCharToTryPtr)
			{
			nextCharToTry=*nextCharToTryPtr;
			}
		
		TInt count=(**list).Count();
		TBool found=EFalse;
		TInt i=0;
		
		for (i=nextCharToTry;i<count;i++)
			{
			const CCharacteristics* cmn((**list)[i]->iCharacteristic);
			
			/**
			 * A plugin symbolizes an algorithm's implementation. It has characteristics of its own.
			 * There can be several plugins dlls (i.e. dll which contains several plugins) which can contain 
			 * algorithm implementations. It is therefore desirable for the CryptoSpi framework to define 
			 * a mechanism using which a user appication can select the desired implementation by
			 * using the Rules. 
			 * The Rules are defined on the interafce level or at algorithm level.
			 * There can be a certain ambiguity when defining a Rule.
			 * ex: Interace : KHashInterface
			 * ex: Algorithm Rule: may be on block size, output size, etc... 
			 * By sorting using these rules, an Interface of the framework can have several plugins
			 * stored in the 'iActiveInterfaceCharacteristics_Map'. The first match of 'algorithmUid' 
			 * will fetch that plugin.    
			 */
			
			if (aAlgorithmUid.iUid == cmn->iAlgorithmUid)
				{
				aImplementationId.iUid = cmn->iImplementationUid;
				aDllName  = (**list)[i]->iDllName;
				found=ETrue;
				break;
				}
			}
			
		if (!found)
			{
			/**
			 * If algorithm implementation not found then we do not set the index
			 * This means the user can specify some rules which can behave as a
			 * threshold value. Numerous plugins can be selected which pass this threshold
			 * value. But if the specific algorithm UID desired doesn't match then we 
			 * return with 'KErrNotFound'. 
			 * ex: User can specify Block Size > 64 for which there can be several matches
			 * under a Hash Interface like MD2, MD5, etc. But if the user later want MD5 and 
			 * if there is no match, then we return with 'KErrNotFound'. 
			 */
			ret=KErrNotFound;
			}
		else
			{
			/**
			 * This will set the index for a particular 'aInterfaceUid'.
			 * Next time the same interface is visited, the search will 
			 * start from this index.  
			 */
			SetSearchRecord(aInterfaceUid, i+1);
			}
		}
	else
		{
		/**
		 * This can be a case when the all the plugins for that interface 
		 * don't fullfill the criteria set by the user application by 
		 * defining the rule for the rule based selector. 
		 */
		return KErrNotFound;
		}
		
	return ret;
	}

void CRuleSelector::SetSearchRecord(TUid aInterfaceUid, TInt aValue)
	{
	TInt* value=iNextTryCharacteristicsIndex.Find(aInterfaceUid.iUid);
	
	if (value)
		{
		*value=aValue;
		}
	}

void CRuleSelector::LoadPluginsL()
	{
	if (iPluginDllList.Count() == 0)
		{
		//Load all the plugins, if it hasn't done before
		for (TInt dllIndex=0; ; ++dllIndex)
			{
			TFileName filename;
			
			if (CryptoSpiUtil::DllIndexToName(dllIndex, filename) == KErrNotFound)
				{
				return;
				}

			//Load...
			RLibrary lib;
			CryptoSpiUtil::LoadPluginDllLC(lib, filename);
			iPluginDllList.AppendL(lib);
			CleanupStack::Pop(&lib);
			}
		}	
	}

//
// Implementation of CRulesCharacteristicsAndPluginName
//
CRulesCharacteristicsAndPluginName* CRulesCharacteristicsAndPluginName::NewLC(TInt32 aInterface)
	{
	CRulesCharacteristicsAndPluginName* self=new(ELeave) CRulesCharacteristicsAndPluginName();
	CleanupStack::PushL(self);
	self->ConstructL(aInterface);
	return self;		
	}
	
CRulesCharacteristicsAndPluginName::CRulesCharacteristicsAndPluginName()
	{
	}

void CRulesCharacteristicsAndPluginName::ConstructL(TInt32 aInterface)
	{
	CCharacteristicsAndPluginName::ConstructL(aInterface);
	}

CRulesCharacteristicsAndPluginName::~CRulesCharacteristicsAndPluginName()
	{
	}

TInt CRuleSelector::AscendExtendedCharacteristicL(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	CExtendedCharacteristics* extCharLeft = aLeft.iExtendedCharacteristic;
	TUid leftUid = {aLeft.iSortUid};
	TInt leftInt = extCharLeft->GetTIntCharacteristicL(leftUid);
	CExtendedCharacteristics* extCharRight = aRight.iExtendedCharacteristic;
	TUid rightUid = {aRight.iSortUid};
	TInt rightInt = extCharRight->GetTIntCharacteristicL(rightUid);
	
	return (leftInt - rightInt);
}

TInt CRuleSelector::DescendExtendedCharacteristicL(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	CExtendedCharacteristics* extCharLeft = aLeft.iExtendedCharacteristic;
	TUid leftUid = {aLeft.iSortUid};
	TInt leftInt = extCharLeft->GetTIntCharacteristicL(leftUid);
	CExtendedCharacteristics* extCharRight = aRight.iExtendedCharacteristic;
	TUid rightUid = {aRight.iSortUid};
	TInt rightInt = extCharRight->GetTIntCharacteristicL(rightUid);
	
	return (rightInt - leftInt);
}

TInt CRuleSelector::AscendExtendedTDesC8L(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	CExtendedCharacteristics* extCharLeft = aLeft.iExtendedCharacteristic;
	TUid leftUid = {aLeft.iSortUid};
	const TDesC8& leftDes = extCharLeft->GetTDesC8CharacteristicL(leftUid);
	CExtendedCharacteristics* extCharRight = aRight.iExtendedCharacteristic;
	TUid rightUid = {aRight.iSortUid};
	const TDesC8& rightDes = extCharRight->GetTDesC8CharacteristicL(rightUid);

	return leftDes.CompareC(rightDes);
}

TInt CRuleSelector::DescendExtendedTDesC8L(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight)
{
	CExtendedCharacteristics* extCharLeft = aLeft.iExtendedCharacteristic;
	TUid leftUid = {aLeft.iSortUid};
	const TDesC8& leftDes = extCharLeft->GetTDesC8CharacteristicL(leftUid);
	CExtendedCharacteristics* extCharRight = aRight.iExtendedCharacteristic;
	TUid rightUid = {aRight.iSortUid};
	const TDesC8& rightDes = extCharRight->GetTDesC8CharacteristicL(rightUid);

	return rightDes.CompareC(leftDes);
}

// Methods which are not supported and non exported can be excluded from the coverage.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

void CRuleSelector::CreateAsyncHashL(CAsyncHash*& /*aHash*/,
										TUid /*aAlgorithmUid*/,
										TUid /*aOperationMode*/,
										const CKey* /*aKey*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT

void CRuleSelector::CreateAsyncHashL(CAsyncHash*& /*aHash*/,
										TUid /*aAlgorithmUid*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRuleSelector::CreateAsyncMacL(CAsyncMac*& /*aMac*/,
									const TUid /*aAlgorithmUid*/,
									const CKey& /*aKey*/,
									const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

#endif

void CRuleSelector::CreateAsyncRandomL(CAsyncRandom*& /*aRandom*/,
										TUid /*aAlgorithmUid*/,
										const CCryptoParams* /*aAlgorithmParams*/)

	{
	User::Leave(KErrNotSupported);
	}

void CRuleSelector::CreateAsyncSymmetricCipherL(CAsyncSymmetricCipher*& /*aCipher*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aKey*/,
										TUid /*aCryptoMode*/,
										TUid /*aOperationMode*/,
										TUid /*aPaddingMode*/,
										const CCryptoParams* /*aAlgorithmParams*/)
										
	{
	User::Leave(KErrNotSupported);
	}

void CRuleSelector::CreateAsyncAsymmetricCipherL(CAsyncAsymmetricCipher*& /*aCipher*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aKey*/,
										TUid /*aCryptoMode*/,
										TUid /*aPaddingMode*/,																						
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRuleSelector::CreateAsyncSignerL(CAsyncSigner*& /*aSigner*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aKey*/,
										TUid /*aPaddingMode*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRuleSelector::CreateAsyncVerifierL(CAsyncVerifier*& /*aVerifier*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aKey*/,
										TUid /*aPaddingMode*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRuleSelector::CreateAsyncKeyPairGeneratorL(CAsyncKeyPairGenerator*& /*aKeyPairGenerator*/,
										TUid /*aAlgorithmUid*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRuleSelector::CreateAsyncKeyAgreementL(CAsyncKeyAgreement*& /*aKeyAgreement*/,
										TUid /*aAlgorithmUid*/,
										const CKey& /*aPrivateKey*/,
										const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotSupported);
	}

CRulesCharacteristicsAndPluginName* CRulesCharacteristicsAndPluginName::NewL(TInt32 aInterface)
	{
	CRulesCharacteristicsAndPluginName* self=CRulesCharacteristicsAndPluginName::NewLC(aInterface);
	CleanupStack::Pop(self);
	return self;		
	}
