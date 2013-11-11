/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalTechnology
*/
#include "ruleselectcommonstep.h"
#include "plugincharschecker.h"
#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptohashapi.h>
#include <cryptospi/cryptorandomapi.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptokeyagreementapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypair.h>
#include <cryptospi/keys.h>

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
#include <cryptospi/cryptomacapi.h>
#endif

#include <cryptospi/cryptospistateapi.h>
#include <cryptospi/ruleselector.h>

using namespace CryptoSpi;

CRuleSelectCommonStep::~CRuleSelectCommonStep()
	{
	}

CRuleSelectCommonStep::CRuleSelectCommonStep()
	{
	SetTestStepName(KRuleSelectCommonStep);
	}

TVerdict CRuleSelectCommonStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CRuleSelectCommonStep::doTestStepL()
	{
	
	INFO_PRINTF1(_L("Rule Selection - Common Characteristics Selection"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	if (TestStepResult()==EPass)
		{
		
		TInt err = 0;

		//Create Default Selection Rules Object
		CSelectionRules* rules = CSelectionRules::NewL();
		CleanupStack::PushL(rules);
		
		//*************SELECTION RULE CREATION AND ADDITION**************

		//Boolean to determine whether there are more rules to apply
		TBool moreRules;
		moreRules = ETrue;
		
		//Itterator representing the current rule. Appended to the end of each selection
		//rule INI parameter 
		TInt currentRule;
		currentRule = 1;
		
		//Total number of selection rules added to the CSelectionRules object
		TInt totalRules;
		totalRules = 0;
			
		TVariantPtrC interfaceScope;
		TVariantPtrC algorithmScope;
		TVariantPtrC charUid;
		TVariantPtrC compareOperator;
		TVariantPtrC charValue;
		TVariantPtrC charValueType;		
		TBool isOptional;
		
		
		//Whilst there are selection rules present in the INI file that have yet to be
		//added to the CSelectionRules object	
		while(moreRules)
			{
			INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
			
			//Constructs each of the parameter names required to extract the necessary 
			//selection rule data from the specified INI configuration file
			TBuf<KMaxConfigParamStringSize> interfaceScopeStr(KConfigInterfaceScopeBase);
			interfaceScopeStr.AppendNum(currentRule);
			INFO_PRINTF2(_L("interfaceScopeStr = %S"), &interfaceScopeStr);
			
			TBuf<KMaxConfigParamStringSize> algorithmScopeStr(KConfigAlgorithmScopeBase);
			algorithmScopeStr.AppendNum(currentRule);
			INFO_PRINTF2(_L("algorithmScopeStr = %S"), &algorithmScopeStr);
			
			TBuf<KMaxConfigParamStringSize> charUidStr(KConfigCharUidBase);
			charUidStr.AppendNum(currentRule);
			INFO_PRINTF2(_L("charUidStr = %S"), &charUidStr);
			
			TBuf<KMaxConfigParamStringSize> compareOperatorStr(KConfigCompareOperatorBase);
			compareOperatorStr.AppendNum(currentRule);
			INFO_PRINTF2(_L("compareOperatorStr = %S"), &compareOperatorStr);
			
			TBuf<KMaxConfigParamStringSize> charValueStr(KConfigCharValueBase);
			charValueStr.AppendNum(currentRule);
			INFO_PRINTF2(_L("charValueStr = %S"), &charValueStr);
			
			TBuf<KMaxConfigParamStringSize> charValueTypeStr(KConfigCharValueTypeBase);
			charValueTypeStr.AppendNum(currentRule);
			INFO_PRINTF2(_L("charValueTypeStr = %S"), &charValueTypeStr);
			
			TBuf<KMaxConfigParamStringSize> isOptionalStr(KConfigIsOptionalBase);
			isOptionalStr.AppendNum(currentRule);
			INFO_PRINTF2(_L("isOptionalStr = %S"), &isOptionalStr);
			
			
			//Extracts the parameter data for a single rule from the specified INI file 
			if(!GetStringFromConfig(ConfigSection(),interfaceScopeStr,interfaceScope) ||
				!GetStringFromConfig(ConfigSection(),algorithmScopeStr,algorithmScope) ||
				!GetStringFromConfig(ConfigSection(),charUidStr,charUid) ||
				!GetStringFromConfig(ConfigSection(),compareOperatorStr,compareOperator) ||
				!GetStringFromConfig(ConfigSection(),charValueTypeStr,charValueType) ||
				!GetBoolFromConfig(ConfigSection(),isOptionalStr,isOptional))
				{
				moreRules = EFalse;
				}
			else
				{
				
				//Creates a pointer to store the address of the newly created selection rule		
				CSelectionRuleContent* rule = NULL;
				
				//Creates a pointer to store the contents of the rule value parameter, so that
				//it can be passed in at construction of the selection rule.
				CCryptoParam* ruleValueParam = NULL;
				
				//Depending on the 'RuleValueType' parameter read in from the INI file, the 'RuleValue'
				//parameter will either be read in as an Integer,String or a Boolean.
				switch(charValueType)
					{
					case ERuleValueInteger:
						{
						TInt charValue;
						
						if(!GetIntFromConfig(ConfigSection(),charValueStr,charValue))
							{
							//Should the integer value be represented by a constant..
							TVariantPtrC strCharValue;
							
							if(!GetStringFromConfig(ConfigSection(),charValueStr,strCharValue))
								{
								ERR_PRINTF1(_L("** .INI Error: INTEGER Rule Value Not Initialised Correctly **"));
								SetTestStepResult(EFail);	
								}
							else
								{
								ruleValueParam = CCryptoIntParam::NewL(TInt(strCharValue),TUid(charUid));	
								}
							}
						else
							{		
							//The Integer Rule Value is encapsulated as a CCryptoIntParam object so that it can be passed in as 
							//a parameter when constructing the selection rule				
							ruleValueParam = CCryptoIntParam::NewL(charValue,charUid);											
							}
							
						break;	
						}
						
					case ERuleValueDes8:
						{		
						TPtrC charValue;
						
						if(!GetStringFromConfig(ConfigSection(),charValueStr,charValue))
							{
							ERR_PRINTF1(_L("** .INI Error: STRING Rule Value Not Initialised Correctly **"));
							SetTestStepResult(EFail);	
							}
						else
							{
							//Convert encryption key to an 8 Bit Descriptor
							HBufC8* charStr = HBufC8::NewLC(charValue.Length());
							TPtr8 charStrPtr = charStr->Des();
			
							charStrPtr.Copy(charValue);
							
							//The String Rule Value is encapsulated as a CCryptoDesC8Param object so that it can be passed in as 
							//a parameter when constructing the selection rule
							ruleValueParam = CCryptoDesC8Param::NewL(charStrPtr,charUid);
							
							CleanupStack::PopAndDestroy(charStr);
							}
							
						break;	
						}
						
					case ERuleValueDes16:
						{		
						TPtrC charValue;
						
						if(!GetStringFromConfig(ConfigSection(),charValueStr,charValue))
							{
							ERR_PRINTF1(_L("** .INI Error: STRING Rule Value Not Initialised Correctly **"));
							SetTestStepResult(EFail);	
							}
						else
							{						
							//The String Rule Value is encapsulated as a CCryptoDesC8Param object so that it can be passed in as 
							//a parameter when constructing the selection rule
							ruleValueParam = CCryptoDesC16Param::NewL(charValue,charUid);
							}
							
						break;	
						}
						
					case ERuleValueBoolean:
						{
						TBool charValue;
						
						if(!GetBoolFromConfig(ConfigSection(),charValueStr,charValue))
							{
							ERR_PRINTF1(_L("** .INI Error: BOOLEAN Rule Value Not Initialised Correctly **"));
							SetTestStepResult(EFail);	
							}
						else
							{
							//The Boolean Rule Value is encapsulated as a CCryptoInt8Param object so that it can be passed in as 
							//a parameter when constructing the selection rule
							ruleValueParam = CCryptoIntParam::NewL(charValue,charUid);
							}
							
						break;	
						}
						
					default:
						{
						ERR_PRINTF1(_L("** .INI Error: Rule Value Type Unknown **"));
						SetTestStepResult(EFail);
						break;	
						}	
					}
						
				
				if(TestStepResult() == EPass)
					{
					
					//Creates the CSelectionRuleContent object, passing in all the parameters read in
					//from the specified INI configuration file
							
					TRAP_LOG(err, rule = CSelectionRuleContent::NewL(interfaceScope,
																	algorithmScope,
																	ruleValueParam,
																	TRuleOperator((TInt)compareOperator),
																	isOptional));	
					if(err == KErrNone)
						{
						//Add the newly created selection rule to the CSelectionRules collection by
						//calling the 'AddSelectionRule' method and passing in the address of the rule
						TRAP_LOG(err,rules->AddSelectionRuleL(rule));
						
						if(err == KErrNone)
							{
							INFO_PRINTF2(_L("Rule %d Created, Added and Destoryed"), currentRule);
							
							currentRule++;
							totalRules++;						
							}
						else
							{
							ERR_PRINTF2(_L("RULE FAILURE: Failed to Add Selection Rule %d "), currentRule);
							moreRules = EFalse;
							SetTestStepResult(EFail);
							}
						}	
					else
						{
						ERR_PRINTF2(_L("RULE FAILURE: Failed to Create Selection Rule %d "), currentRule);
						moreRules = EFalse;	
						SetTestStepResult(EFail);
						}
					
						
					}
				else
					{
					moreRules = EFalse;	
					}
				}	
			}
			
		//If 1 or more selection rule objects were added successfully without error, the test step
		//can proceed to the next phase   
		if((TestStepResult() == EPass) && (totalRules > 0))
			{
			INFO_PRINTF2(_L("Successfully Added Selection Rules: %d"), totalRules);
			}
		else if((TestStepResult() == EFail) && (totalRules > 0))
			{
			ERR_PRINTF1(_L("** .INI Error: Errornous Selection Rule Data **"));
			SetTestStepResult(EFail);
			}
		else
			{
			ERR_PRINTF1(_L("** NO VALID SELECTION RULE DATA FOUND **"));	
			}

		//**************************************************************		
		
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		if(TestStepResult()==EPass)
			{	
			
			//Assume faliure, unless all is successful
			SetTestStepResult(EFail);
				
			//Create Rule Selector Object	
			CRuleSelector* ruleSelector = CRuleSelector::NewL(rules);
			CleanupStack::Pop(rules);
			CleanupStack::PushL(ruleSelector);

			//Set the Selector Passing in a pointer to the Rule Selector and SPI State	
			CCryptoSpiStateApi::SetSelector(ruleSelector);

			//Define a pointer of type CCryptoBase to store the address ofthe instantiated encryption object
			CCryptoBase* plugin = NULL;

			TVariantPtrC interfaceType;
			
			//Extract the Encryption Type parameter from the specified INI file
			if(!GetStringFromConfig(ConfigSection(),KConfigInterfaceType,interfaceType))
				{
				ERR_PRINTF1(_L("** .INI Error: Encryption Type Data Not Located **"));
				SetTestStepResult(EFail);
				}
			else
				{
				//Depending on the Interface Type retrieved, the appropriate factory object
				//is created and assigned to the previously CCryptoBase plugin pointer
				switch(interfaceType) 
					{			
					case KAsymmetricCipherInterface:
						{
		
						TPtrC16 encryptKey;
						TVariantPtrC keyType;
						TVariantPtrC algorithmUid;
						TVariantPtrC cryptoMode;
						TVariantPtrC paddingMode;
						TVariantPtrC algorithmParams;
						
						//Each of the individual parameters required to create the Asymmetric Cipher object
						//are read in from the specified INI configuration file	
						if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
							!GetStringFromConfig(ConfigSection(),KConfigCryptoMode,cryptoMode) ||
							!GetStringFromConfig(ConfigSection(),KConfigPaddingMode,paddingMode) ||
							!GetStringFromConfig(ConfigSection(),KConfigAlgorithmParams,algorithmParams))
							{
							ERR_PRINTF1(_L("** .INI Error: Asymmetric Cipher Arguments Not Located **"));
							SetTestStepResult(EFail);
							}
						else
							{
								
							//********** Rule Selector Unselcted to Create Key Pair ************
							CCryptoSpiStateApi::UnsetSelector();
							
							CCryptoParams* keyParams = CCryptoParams::NewLC(); 
							
							//Create Key Pair and Key Pair Generator Objects
				   			CKeyPair* keyPair = NULL;
							CKeyPairGenerator * keypairImpl = NULL;
							
							INFO_PRINTF1(_L("Generating RSA keys"));
							
							// create an RSA key pair
							keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
							keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);

							// create a key pair generator implementation interface
							TRAP_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(
															keypairImpl, 
															KRSAKeyPairGeneratorUid, 
															keyParams));
															
							CleanupStack::PushL(keypairImpl);

							// Create a Key Pair	
							TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, *keyParams, keyPair));
							
							CleanupStack::PushL(keyPair);
							
							CCryptoSpiStateApi::SetSelector(ruleSelector);
							//******************************************************************
							
							CAsymmetricCipher* asymmetricCipherImpl;	
								
							TRAP_LOG(err,CAsymmetricCipherFactory::CreateAsymmetricCipherL(asymmetricCipherImpl,
																							algorithmUid,
																							keyPair->PublicKey(),
																							cryptoMode,
																							paddingMode,
																							NULL));
							
							if(asymmetricCipherImpl && (err == KErrNone))
								{
								plugin = static_cast<CCryptoBase*>(asymmetricCipherImpl);
								
								INFO_PRINTF1(_L("** Successfully Loaded Asymmetric Cipher Object **"));
								SetTestStepResult(EPass);	
								}
							else
								{
								ERR_PRINTF2(_L("** Error: Loading Asymmetric Cipher Object - %d **"), err);
								SetTestStepResult(EFail);
								}
								
							CleanupStack::PopAndDestroy(3,keyParams);	
							} 
							
						break;
						}
							
					case KSymmetricCipherInterface:
						{
						
						TPtrC16 encryptKey;
						TVariantPtrC keyType;
						TVariantPtrC algorithmUid;
						TVariantPtrC cryptoMode;
						TVariantPtrC operationMode;
						TVariantPtrC paddingMode;
						TVariantPtrC algorithmParams;
						
						//Each of the individual parameters required to create the Symmetric Cipher object
						//are read in from the specified INI configuration file
						if(!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) || 
							!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType) ||
							!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
							!GetStringFromConfig(ConfigSection(),KConfigCryptoMode,cryptoMode) ||
							!GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationMode) ||
							!GetStringFromConfig(ConfigSection(),KConfigPaddingMode,paddingMode) ||
							!GetStringFromConfig(ConfigSection(),KConfigAlgorithmParams,algorithmParams))
							{
							ERR_PRINTF1(_L("** .INI Error: Symmetric Cipher Arguments Not Located **"));
							SetTestStepResult(EFail);
							}
						else
							{
							//Convert encryption key to an 8 Bit Descriptor
							HBufC8* keyStr = HBufC8::NewLC(encryptKey.Length());
							TPtr8 keyStrPtr = keyStr->Des();
			
							keyStrPtr.Copy(encryptKey);
							
							//Create an new CryptoParams object to encapsulate the key type and secret key string
							CCryptoParams* keyParams = CCryptoParams::NewL();
							CleanupStack::PushL(keyParams);
							keyParams->AddL(*keyStr,keyType);
			
							//Create Key Object
							TKeyProperty keyProperty;
							CKey* key=CKey::NewL(keyProperty,*keyParams);
							CleanupStack::PushL(key);
							
							CSymmetricCipher* symmetricCipherImpl = NULL;	
								
							TRAP_LOG(err,CSymmetricCipherFactory::CreateSymmetricCipherL(symmetricCipherImpl,
																						algorithmUid,
																						*key,
																						cryptoMode,
																						operationMode,
																						paddingMode,
																						NULL));
							
							
							if(symmetricCipherImpl && (err == KErrNone))
								{
								
								plugin = static_cast<CCryptoBase*>(symmetricCipherImpl);
								
								INFO_PRINTF1(_L("** Successfully Loaded Symmetric Cipher Object **"));
								SetTestStepResult(EPass);	
								}
							else
								{
								ERR_PRINTF2(_L("** Error: Loading Symmetric Cipher Object - %d **"), err);
								SetTestStepResult(EFail);
								} 
									
							CleanupStack::PopAndDestroy(3,keyStr);
							} 
							
						break;
						}
						
					case KHashInterface:
						{
						TVariantPtrC algorithmUid;
						TVariantPtrC operationMode;
						TVariantPtrC algorithmParams;
						
						//Each of the individual parameters required to create the Hash object
						//are read in from the specified INI configuration file	
						if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
#ifndef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT							
							!GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationMode) ||
#endif							
							!GetStringFromConfig(ConfigSection(),KConfigAlgorithmParams,algorithmParams))
							{
							ERR_PRINTF1(_L("** .INI Error: Hash Arguments Not Located **"));
							SetTestStepResult(EFail);
							}
						else
							{
							CHash* hashImpl = NULL;
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT							
							if (GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationMode))
								{
								TRAP_LOG(err,CHashFactory::CreateHashL(hashImpl,
																	algorithmUid,
																	operationMode,
																	NULL,
																	NULL));
								}
							else
								{
								TRAP_LOG(err,CHashFactory::CreateHashL(hashImpl,
																	algorithmUid,
																	NULL));								
								}
#else
							TRAP_LOG(err,CHashFactory::CreateHashL(hashImpl,
																	algorithmUid,
																	operationMode,
																	NULL,
																	NULL));
#endif							

							if(hashImpl && (err == KErrNone))
								{
								plugin = static_cast<CCryptoBase*>(hashImpl);
								
								INFO_PRINTF1(_L("** Successfully Loaded Hash Object **"));
								SetTestStepResult(EPass);	
								}
							else
								{
								ERR_PRINTF2(_L("** Error: Loading Hash Object - %d **"), err);
								SetTestStepResult(EFail);
								}	
							}
							
						break;
						}

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT							
					case KMacInterface:
						{
						TVariantPtrC algorithmUid;
						TPtrC encryptKey;
						TVariantPtrC keyType;
						
						//Each of the individual parameters required to create the Mac object
						//are read in from the specified INI configuration file	
						if (!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
							!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) ||
							!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType))
							{
							ERR_PRINTF1(_L("** .INI Error: Mac Arguments Not Located **"));
							SetTestStepResult(EFail);
							}
						else
							{
							CMac* macImpl = NULL;
							
							//Convert encryption key to an 8 Bit Descriptor
							HBufC8* keyStr = HBufC8::NewLC(encryptKey.Length());
							TPtr8 keyStrPtr = keyStr->Des();
							
							keyStrPtr.Copy(encryptKey);
											
							//Create an new CryptoParams object to encapsulate the key type and secret key string
							CCryptoParams* keyParams = CCryptoParams::NewL();
							CleanupStack::PushL(keyParams);
							keyParams->AddL(*keyStr,keyType);
							
							//Create Key Object
							TKeyProperty keyProperty;
							CKey* key=CKey::NewL(keyProperty,*keyParams);
							CleanupStack::PushL(key);
									
							TRAP_LOG(err,CMacFactory::CreateMacL(macImpl,
																algorithmUid,
																*key,
																NULL));
							
							if(macImpl && (err == KErrNone))
								{
								plugin = static_cast<CCryptoBase*>(macImpl);
								
								INFO_PRINTF1(_L("** Successfully Loaded Mac Object **"));
								SetTestStepResult(EPass);	
								}
							else
								{
								delete macImpl;
								ERR_PRINTF2(_L("** Error: Loading Mac Object - %d **"), err);
								SetTestStepResult(EFail);
								}
							
							CleanupStack::PopAndDestroy(3,keyStr);
							}
						
						break;
						}
#endif							
						
					case KRandomInterface:
						{
						
						TVariantPtrC algorithmUid;
						TVariantPtrC algorithmParams;
						
						//Each of the individual parameters required to create the Random object
						//are read in from the specified INI configuration file
						if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
							!GetStringFromConfig(ConfigSection(),KConfigAlgorithmParams,algorithmParams))
							{
							ERR_PRINTF1(_L("** .INI Error: Random Arguments Not Located **"));
							SetTestStepResult(EFail);
							}
						else
							{
							CryptoSpi::CRandom* randomImpl = NULL;
							
							TRAP_LOG(err,CRandomFactory::CreateRandomL(randomImpl,
																		algorithmUid,
																		NULL));

							if(randomImpl && (err == KErrNone))
								{
								plugin = static_cast<CCryptoBase*>(randomImpl);
								
								INFO_PRINTF1(_L("** Successfully Loaded Random Object **"));
								SetTestStepResult(EPass);	
								}
							else
								{
								ERR_PRINTF2(_L("** Error: Loading Random Object - %d **"), err);
								SetTestStepResult(EFail);
								}
								
							} 
							
						break;	
						}
						
					case KSignerInterface:
						{
						TVariantPtrC algorithmUid;
						TVariantPtrC paddingMode;
						TVariantPtrC algorithmParams;
						
						//Each of the individual parameters required to create the Signer object
						//are read in from the specified INI configuration file
						if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
							!GetStringFromConfig(ConfigSection(),KConfigPaddingMode,paddingMode) ||
							!GetStringFromConfig(ConfigSection(),KConfigAlgorithmParams,algorithmParams))
							{
							ERR_PRINTF1(_L("** .INI Error: Signer Arguments Not Located **"));
							SetTestStepResult(EFail);
							}
						else
							{
							
							//********** Rule Selector Unselected to Create Key Pair ************
							CCryptoSpiStateApi::UnsetSelector();
							
							CCryptoParams* keyParams = CCryptoParams::NewLC(); 
							
							//Create Key Pair and Key Pair Generator Objects
				   			CKeyPair* keyPair = NULL;
							CKeyPairGenerator * keypairImpl = NULL;
							
							INFO_PRINTF1(_L("Generating RSA keys"));
							
							// create an RSA key pair
							keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
							keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);

							// create a key pair generator implementation interface
							TRAP_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
																							KRSAKeyPairGeneratorUid, 
																							keyParams));
															
							CleanupStack::PushL(keypairImpl);

							// Create a Key Pair	
							TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, *keyParams, keyPair));
							
							CleanupStack::PushL(keyPair);
							
							CCryptoSpiStateApi::SetSelector(ruleSelector);
							//******************************************************************
							
							CSigner* signerImpl = NULL;	
							
						    TRAP_LOG(err,CSignatureFactory::CreateSignerL(signerImpl,
																	   		algorithmUid,
																	   		keyPair->PrivateKey(),
																	   		paddingMode,
																	   		NULL));
																	   
							if(signerImpl && (err == KErrNone))
								{
								plugin = static_cast<CCryptoBase*>(signerImpl);
								
								INFO_PRINTF1(_L("** Successfully Loaded Signer Object **"));
								SetTestStepResult(EPass);	
								}
							else
								{
								ERR_PRINTF2(_L("** Error: Loading Signer Object - %d **"), err);
								SetTestStepResult(EFail);
								}
							
							CleanupStack::PopAndDestroy(3,keyParams);	
							}
							
						break;
						}
						
					case KVerifierInterface:
						{
						TVariantPtrC algorithmUid;
						TVariantPtrC paddingMode;
						TVariantPtrC algorithmParams;
						
						//Each of the individual parameters required to create the Verifier object
						//are read in from the specified INI configuration file
						if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
							!GetStringFromConfig(ConfigSection(),KConfigPaddingMode,paddingMode) ||
							!GetStringFromConfig(ConfigSection(),KConfigAlgorithmParams,algorithmParams))
							{
							ERR_PRINTF1(_L("** .INI Error: Verifier Arguments Not Located **"));
							SetTestStepResult(EFail);
							}
						else
							{
							
							//********** Rule Selector Unselected to Create Key Pair ************
							CCryptoSpiStateApi::UnsetSelector();
							
							CCryptoParams* keyParams = CCryptoParams::NewLC(); 
							
							//Create Key Pair and Key Pair Generator Objects
				   			CKeyPair* keyPair = NULL;
							CKeyPairGenerator * keypairImpl = NULL;
							
							INFO_PRINTF1(_L("Generating RSA keys"));
							
							// create an RSA key pair
							keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
							keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);

							// create a key pair generator implementation interface
							TRAP_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
																							KRSAKeyPairGeneratorUid, 
																							keyParams));
															
							CleanupStack::PushL(keypairImpl);

							// Create a Key Pair	
							TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, *keyParams, keyPair));
							
							CleanupStack::PushL(keyPair);
							
							CCryptoSpiStateApi::SetSelector(ruleSelector);
							//******************************************************************
							
							CVerifier* verifierImpl = NULL;	
							
						    TRAP_LOG(err,CSignatureFactory::CreateVerifierL(verifierImpl,
																	   		algorithmUid,
																	   		keyPair->PublicKey(),
																	   		paddingMode,
																	   		NULL));
																	   
							if(verifierImpl && (err == KErrNone))
								{
								plugin = static_cast<CCryptoBase*>(verifierImpl);
								
								INFO_PRINTF1(_L("** Successfully Loaded Verifier Object **"));
								SetTestStepResult(EPass);	
								}
							else
								{
								ERR_PRINTF2(_L("** Error: Loading Verifier Object - %d **"), err);
								SetTestStepResult(EFail);
								}
							
							CleanupStack::PopAndDestroy(3,keyParams);
							}
							
						break;
						}
						
					case KKeypairGeneratorInterface:
						{
						TVariantPtrC algorithmUid;
						TVariantPtrC algorithmParams;
						
						//Each of the individual parameters required to create the Key Pair Generator object
						//are read in from the specified INI configuration file
						if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
							!GetStringFromConfig(ConfigSection(),KConfigAlgorithmParams,algorithmParams))
							{
							ERR_PRINTF1(_L("** .INI Error: Key Pair Generator Arguments Not Located **"));
							SetTestStepResult(EFail);
							}
						else
							{
						
							CCryptoParams* keyParams = CCryptoParams::NewLC(); 
								
							//Create Key Pair and Key Pair Generator Objects
							CKeyPairGenerator * keypairImpl = NULL;
							
							INFO_PRINTF1(_L("Generating RSA keys"));
							
							// create an RSA key pair
							keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
							keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);

							// create a key pair generator implementation interface
							TRAP_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
																							algorithmUid, 
																							keyParams));
																							
							if(keypairImpl && (err == KErrNone))
								{
								plugin = static_cast<CCryptoBase*>(keypairImpl);
								
								INFO_PRINTF1(_L("** Successfully Loaded Key Pair Generator Object **"));
								SetTestStepResult(EPass);	
								}
							else
								{
								ERR_PRINTF2(_L("** Error: Loading Key Pair Generator Object - %d **"), err);
								SetTestStepResult(EFail);
								}
								
							CleanupStack::PopAndDestroy(keyParams);
							}
						
						break;
						}
						
					case KKeyAgreementInterface:
						{
						TVariantPtrC algorithmUid;
						TVariantPtrC algorithmParams;
						
						//Each of the individual parameters required to create the Key Agreement object
						//are read in from the specified INI configuration file
						if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
							!GetStringFromConfig(ConfigSection(),KConfigAlgorithmParams,algorithmParams))
							{
							ERR_PRINTF1(_L("** .INI Error: Key Agreement Arguments Not Located **"));
							SetTestStepResult(EFail);
							}
						else
							{
							//********** Rule Selector Unselected to Create Key Pair ************
							CCryptoSpiStateApi::UnsetSelector();
	 		 
							RInteger DH_N = RInteger::NewPrimeL(64);
							CleanupClosePushL(DH_N);
							
							RInteger DH_N_MinusOne = RInteger::NewL(DH_N);
							CleanupClosePushL(DH_N_MinusOne);
							DH_N_MinusOne-=1;
							
							RInteger DH_G = RInteger::NewRandomL(TInteger::Two(), DH_N_MinusOne);	
							CleanupClosePushL(DH_G);
							
							CCryptoParams* keyParams = CCryptoParams::NewLC(); 
										
							TRAP_LOG(err,keyParams->AddL(DH_N, KDhKeyParameterNUid));
							TRAP_LOG(err,keyParams->AddL(DH_G, KDhKeyParameterGUid));
							
							//Create Key Pair and Key Pair Generator Objects
				   			CKeyPair* keyPair = NULL;
							CKeyPairGenerator * keypairImpl = NULL;
							
							INFO_PRINTF1(_L("Generating Key Pair..."));
							
							// create a key pair generator implementation interface
							TRAP_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
																							KDHKeyPairGeneratorUid, 
																							keyParams));								
							CleanupStack::PushL(keypairImpl);

							// Create a Key Pair	
							TRAP_LOG(err,keypairImpl->GenerateKeyPairL(NULL, *keyParams, keyPair));
							
							CleanupStack::PushL(keyPair);
							
							CCryptoSpiStateApi::SetSelector(ruleSelector);
							//******************************************************************
							
							CKeyAgreement* keyAgreementImpl = NULL;	
								
							TRAP_LOG(err,CKeyAgreementFactory::CreateKeyAgreementL(keyAgreementImpl,
																				algorithmUid,
																				keyPair->PrivateKey(),
																				keyParams));
																				
							if(keyAgreementImpl && (err == KErrNone))
								{
								plugin = static_cast<CCryptoBase*>(keyAgreementImpl);
								
								INFO_PRINTF1(_L("** Successfully Loaded Key Agreement Object **"));
								SetTestStepResult(EPass);	
								}
							else
								{
								ERR_PRINTF2(_L("** Error: Loading Key Agreement Object - %d **"), err);
								SetTestStepResult(EFail);
								}
								
							CleanupStack::PopAndDestroy(6,&DH_N);
							}
							
						break;
						}
						
					default:
						{
						ERR_PRINTF1(_L("** .INI Error: Interface Type Unknown **"));
						SetTestStepResult(EFail);
						break;	
						}
						
					}
				}		
				
			if(TestStepResult()==EPass)
				{
				//Push the selected implementation onto the Cleanup Stack
				CleanupStack::PushL(plugin);	
				
				//Assume faliure, unless all is successful
				SetTestStepResult(EFail);
				
				//Define a pointer of type TCharacteristics in order to store the appropriate
				//encryption object's characterisctics
				const TCharacteristics* characteristics(NULL);
					
				//Retrieve the characteristics for the symmetric cipher implementation object
				TRAP_LOG(err, plugin->GetCharacteristicsL(characteristics));
				
				TVariantPtrC exAlgorithmUid;
				TVariantPtrC exImplementationUid;
						
				if(!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmUid,exAlgorithmUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExImplementationUid,exImplementationUid))
					{
					ERR_PRINTF1(_L("** .INI Error: Expected Algorithm Arguments Not Located **"));
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("Checking Plug-in Selection..."));
					
					CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();
					
					TPtrC errorMessage;
					
					//Perform plug-in Check
					if(pluginCheck->checkSelectedPlugin(characteristics,
													exAlgorithmUid,
													exImplementationUid,
													errorMessage))
						{
						INFO_PRINTF1(_L("** PASS: Correct Plugin Implementation Selected **"));
						SetTestStepResult(EPass);	
						}
					else
						{
						ERR_PRINTF2(_L("** FAIL: Wrong Plugin Implementation Selected - %S **"),&errorMessage);	
						}
					
					CleanupStack::PopAndDestroy(pluginCheck);
					}
					
				CleanupStack::PopAndDestroy(plugin);
				}
			
			//Unselect Rule Selector	
			CCryptoSpiStateApi::UnsetSelector();
			
			//Pop and Destroy Rule Selector from the Cleanup Stack	
			CleanupStack::PopAndDestroy(ruleSelector);	
			}	
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());		
	return TestStepResult();
	}


TVerdict CRuleSelectCommonStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
	
