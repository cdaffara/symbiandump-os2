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

#include "symmetriccipherprocessbeforeivsetstep.h"

using namespace CryptoSpi;

CSymmetricCipherProcessBeforeIvSetStep::CSymmetricCipherProcessBeforeIvSetStep()
	{
	SetTestStepName(KSymmetricCipherProcessBeforeIvSetStep);
	}


TVerdict CSymmetricCipherProcessBeforeIvSetStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSymmetricCipherProcessBeforeIvSetStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Symmetric Cipher - Calling the Process methods without first setting the IV ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()!=EPass)
  		{
  		return TestStepResult(); 
  		}
		
	//Assume failure, unless all is successful
	SetTestStepResult(EFail);
		
	TVariantPtrC operationMode;
		
	CSymmetricCipher* impl = NULL;
	CKey* key = NULL;
	SetupCipherL(EFalse, EFalse, operationMode, impl, key);

	INFO_PRINTF1(_L("Plugin loaded."));
	
	CleanupStack::PushL(key);
	CleanupStack::PushL(impl);

	HBufC8* plaintext = ReadInPlaintextL();
	CleanupStack::PushL(plaintext);
				
	//Create buffer for encrypted data
	TInt maxOutputLength = impl->MaxFinalOutputLength(plaintext->Length());
	HBufC8* encrypted =	HBufC8::NewLC(maxOutputLength);
	TPtr8 encryptedPtr = encrypted->Des();

	INFO_PRINTF1(_L("Calling ProcessL()..."));

	//Perform the encryption operation
	TRAPD(err, impl->ProcessL((*plaintext), encryptedPtr));
					
	if (err == KErrNotSupported)
		{
		INFO_PRINTF1(_L("ProcessL() errored with KErrNotSupported as expected"));
		SetTestStepResult(EPass);
		}
	else if (err == KErrNone)
		{
		ERR_PRINTF1(_L("*** FAIL: ProcessL() did not error ***"));
		SetTestStepResult(EFail);						
		}
	else
		{
		ERR_PRINTF2(_L("*** FAIL: ProcessL() errored with unexpected error code - %d ***"), err);
		SetTestStepResult(EFail);				
		}
				
	if (encryptedPtr.Length() != 0)
		{
		ERR_PRINTF1(_L("*** FAIL: encryptedPtr had been modified by ProcessL() ***"));
		SetTestStepResult(EFail);					
		}
					
	if (TestStepResult() == EFail)
		{
		CleanupStack::PopAndDestroy(4, key);
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		return TestStepResult();	
		}	
				
	INFO_PRINTF1(_L("Calling ProcessFinalL()..."));

	//Perform the encryption operation
	TRAP(err, impl->ProcessFinalL((*plaintext), encryptedPtr));
				
	if (err == KErrNotSupported)
		{
		INFO_PRINTF1(_L("ProcessFinalL() errored with KErrNotSupported as expected"));
		}
	else if (err == KErrNone)
		{
		ERR_PRINTF1(_L("*** FAIL: ProcessFinalL() did not error ***"));
		SetTestStepResult(EFail);						
		}
	else
		{
		ERR_PRINTF2(_L("*** FAIL: ProcessFinalL() errored with unexpected error code - %d ***"), err);
		SetTestStepResult(EFail);				
		}
				
	if (encryptedPtr.Length() != 0)
		{
		ERR_PRINTF1(_L("*** FAIL: encryptedPtr had been modified by ProcessFinalL() ***"));
		SetTestStepResult(EFail);					
		}
	
	CleanupStack::PopAndDestroy(encrypted);
	CleanupStack::PopAndDestroy(plaintext);
	CleanupStack::PopAndDestroy(impl);
	CleanupStack::PopAndDestroy(key);				
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	return TestStepResult();		
	}


TVerdict CSymmetricCipherProcessBeforeIvSetStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
