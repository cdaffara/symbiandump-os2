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
* Example CTestStep derived implementation
*
*/


/**
 @file
 @internalTechnology
*/
#include <cryptospi/cryptorandomapi.h>

#include "securityerr.h"
#include "randomnumbergeneratorgeneralusagestep.h"


using namespace CryptoSpi;

CRandomNumberGeneratorGeneralUsageStep::~CRandomNumberGeneratorGeneralUsageStep()
	{
	}

CRandomNumberGeneratorGeneralUsageStep::CRandomNumberGeneratorGeneralUsageStep()
	{
	SetTestStepName(KRandomNumberGeneratorGeneralUsageStep);
	}

TVerdict CRandomNumberGeneratorGeneralUsageStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CRandomNumberGeneratorGeneralUsageStep::doTestStepL()
	{
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		INFO_PRINTF1(_L("*** Random Number Generator - General Usage ***"));
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		TVariantPtrC algorithmUid;
		
		//Extract the necessary parameters from the specified INI file
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid))
			{
			ERR_PRINTF1(_L("** Error: Algorithm Uid Not Found **"));
			SetTestStepResult(EFail);
			}
		else
			{
			//Create a pointer for the Random Implementation Object
			CryptoSpi::CRandom* randomImpl = NULL;
			
			//Retrieve a Random Factory Object						
			TRAPD(err,CRandomFactory::CreateRandomL(randomImpl,
													algorithmUid,
													NULL)); 
				
			if(randomImpl && (err == KErrNone))
				{
				
				TBuf8<50> randomStr(50);
				
				INFO_PRINTF2(_L("*** randomStr Length: %d ***"),randomStr.Length());
				INFO_PRINTF2(_L("*** randomStr MaxLength: %d ***"),randomStr.MaxLength());
				
				TBuf<50> originalState;
				originalState.Copy(randomStr);
				
				INFO_PRINTF2(_L("*** Original Content: %S ***"),&originalState);
				
				TRAP(err,randomImpl->GenerateRandomBytesL(randomStr));
				
				//Copy the 8bit descriptor to 16bit using using a conversion macro
				TBuf<50> randomResult;
				randomResult.Copy(randomStr);
					
				if((randomResult != KEmptyString) && (randomResult != originalState) && ((err == KErrNone) || (err == KErrNotSecure)))
					{
					INFO_PRINTF1(_L("*** Random Number Generator - General Usage : PASS ***"));
					SetTestStepResult(EPass);	
					}
				else
					{
					ERR_PRINTF1(_L("*** FAIL: Failed to Fill Random String ***"));
					SetTestStepResult(EFail);	
					}	
				}
			else
				{
				ERR_PRINTF2(_L("*** Random Object Load Failure : %d ***"), err);
				User::Leave(err);	
				}
			
			//Free up the memory allocated to the Random object and set the point to NULL	
			delete randomImpl;
			randomImpl = NULL;
			
			}
		
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		}
	return TestStepResult();
	}


TVerdict CRandomNumberGeneratorGeneralUsageStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
