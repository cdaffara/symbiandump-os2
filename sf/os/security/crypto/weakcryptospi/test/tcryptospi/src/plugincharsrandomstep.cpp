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


/**
 @file
 @internalTechnology
*/
 
#include "plugincharsrandomstep.h"
#include "plugincharschecker.h"

#include <cryptospi/cryptorandomapi.h>

using namespace CryptoSpi;

CPluginCharsRandomStep::~CPluginCharsRandomStep()
	{
	}

CPluginCharsRandomStep::CPluginCharsRandomStep()
	{
	SetTestStepName(KPluginCharsRandomStep);
	}

TVerdict CPluginCharsRandomStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
				
TVerdict CPluginCharsRandomStep::doTestStepL()
	{
	
	INFO_PRINTF1(_L("Plugin Characteristics - Random Chracteristics"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC algorithmUid;
					
		//Each of the individual parameters required to create the Random object
		//are read in from the specified INI configuration file	
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid))
			{
			ERR_PRINTF1(_L("** .INI Error: Random Arguments Not Located **"));
			SetTestStepResult(EFail);
			}
		else
			{
			CryptoSpi::CRandom* randomImpl = NULL; 
			
			TRAPD_LOG(err,CRandomFactory::CreateRandomL(randomImpl,
												algorithmUid,
												NULL));
	
			if(randomImpl && (err == KErrNone))
				{
				
				CleanupStack::PushL(randomImpl);
				
				INFO_PRINTF1(_L("** Successfully Loaded Random Object **"));
			
				//Define a pointer of type TCharacteristics in order to store the random
				//object's characterisctics	
				const TCharacteristics* chars(NULL);
							
				//Retrieve the characteristics for the random implementation object
				TRAP_LOG(err, randomImpl->GetCharacteristicsL(chars));
				
				//Static cast the characteristics to type TRandomCharacteristics
				const TRandomCharacteristics* randomChars = static_cast<const TRandomCharacteristics*>(chars);
				
				//Retrieve all the Common characteristics that are required for all test cases
				TVariantPtrC exInterfaceUid;
				TVariantPtrC exAlgorithmUid;
				TVariantPtrC exImplementationUid;
				TVariantPtrC exCreatorName;
				TBool exFIPSApproved;
				TBool exHardwareSupported;
				TInt exMaxConcurrencySupported;
				TVariantPtrC exAlgorithmName;
				TInt exLatency;
				TInt exThroughput;
				
				//Retrieve all the Random characteristics that are required for all test cases
				TInt exRandomBlockingMode;
							
				if(!GetStringFromConfig(ConfigSection(),KConfigExInterfaceUid,exInterfaceUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmUid,exAlgorithmUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExImplementationUid,exImplementationUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExCreatorName,exCreatorName) ||
					!GetBoolFromConfig(ConfigSection(),KConfigExFIPSApproved,exFIPSApproved) ||
					!GetBoolFromConfig(ConfigSection(),KConfigExHardwareSupport,exHardwareSupported) ||
					!GetIntFromConfig(ConfigSection(),KConfigExMaxConcurrency,exMaxConcurrencySupported) ||
					!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmName,exAlgorithmName) ||
					!GetIntFromConfig(ConfigSection(),KConfigExLatency,exLatency) ||
					!GetIntFromConfig(ConfigSection(),KConfigExThroughput,exThroughput) ||
					!GetIntFromConfig(ConfigSection(),KConfigExBlockingMode,exRandomBlockingMode))
					{
					ERR_PRINTF1(_L("** .INI Error: Expected Random/Common Characteristic Arguments Not Located **"));
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("** Checking Random/Common Characteristics.... **"));
					
					CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();
					
					//Retrieve the Common Characteristics from TRandomCharacteristics 
					const TCommonCharacteristics* randomCommonChars = &randomChars->cmn;
					
					TPtrC errorMessage;

					//Perform Random/Common Characteristic Checks
					if(pluginCheck->checkCommonCharacteristics(randomCommonChars,
																exInterfaceUid,
																exAlgorithmUid,
																exImplementationUid,
																exCreatorName,
																exFIPSApproved,
																exHardwareSupported,
																exMaxConcurrencySupported,
																exAlgorithmName,
																exLatency,
																exThroughput,
																errorMessage) &&
						pluginCheck->checkRandomCharacteristics(randomChars,
																exRandomBlockingMode,
																errorMessage))
						{
						INFO_PRINTF1(_L("** PASS : Random/Common characteristics successfully match expected values **"));
						SetTestStepResult(EPass);	
						}
					else
						{
						ERR_PRINTF2(_L("** FAIL: Characteristic Mismatch - %S **"),&errorMessage);	
						}
						
					CleanupStack::PopAndDestroy(pluginCheck);	
					}
			
				CleanupStack::PopAndDestroy(randomImpl);
				}
			else
				{
				ERR_PRINTF1(_L("** Error: Loading Random Object **"));
				}
			}

		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	return TestStepResult();
	
	}

TVerdict CPluginCharsRandomStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
