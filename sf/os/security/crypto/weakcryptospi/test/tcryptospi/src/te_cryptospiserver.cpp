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
* Example file/test code to demonstrate how to develop a TestExecute Server
* Developers should take this project as a template and substitute their own
* for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
* in the process of the client. The client initialises the server by calling the
* one and only ordinal.
*
*/


/**
 @file
 @internalTechnology
*/

#include "te_cryptospiserver.h"

#include "symmetriccipherpositiveobjectloadstep.h"
#include "symmetriccipherencrypteddatacheckstep.h"
#include "symmetriccipherencryptdecryptstep.h"
#include "symmetriccipherincrementalencryptdecryptstep.h"
#include "symmetriccipherobjectreusestep.h"
#include "symmetriccipherprocessbeforeivsetstep.h"
#include "symmetriccipherbadivlengthstep.h"
#include "symmetriccipherctrmodeoutoforderstep.h"

#include "asymmetriccipherpositiveobjectloadstep.h"
#include "asymmetriccipherencrypteddatacheckstep.h"
#include "asymmetriccipherencryptdecryptstep.h"

#include "hashpositiveobjectloadstep.h"
#include "hashbasichashofdatastep.h"
#include "hashincrementalhashstep.h"
#include "hashincrementalhashwithresetstep.h"
#include "hashincrementalhashwithcopystep.h"
#include "hashincrementalhashwithreplicatestep.h"

#include "hmacpositiveobjectloadstep.h"
#include "hmacbasichashofdatastep.h"
#include "hmacincrementalhmacstep.h"
#include "hmacincrementalhmacwithresetstep.h"
#include "hmacincrementalhmacwithcopystep.h"
#include "hmacincrementalhmacwithreplicatestep.h"
#include "hmacsetkeycheckingstep.h"
#include "hmacsetoperationmodecheckingstep.h"

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
#include "hash_basic_data_step.h"
#include "hash_incremental_step.h"
#include "hash_incremental_with_copy_step.h"
#include "hash_incremental_with_replicate_step.h"
#include "hash_incremental_with_reset_step.h"
#include "hash_positive_object_load_step.h"

#include "mac_basic_data_step.h"
#include "mac_incremental_with_copy_step.h"
#include "mac_incremental_with_replicate_step.h"
#include "mac_incremental_reset_step.h"
#include "mac_incremental_step.h"
#include "mac_positive_object_load_step.h"
#include "symmetric_mac_basic_data_step.h" 
#include "symmetric_mac_incremental_with_replicate_step.h"
#include "symmetric_mac_incremental_with_copy_step.h"
#include "symmetric_mac_incremental_reset_step.h"
#include "symmetric_mac_incremental_re_init_step.h"
#include "symmetric_mac_incremental_step.h"

#include "plugincharsmacstep.h"

#include "plugin_chars_new_hash_step.h"
#endif

#include "randomnumbergeneratorgeneralusagestep.h"

#include "keyexchangesyncstep.h"
#include "keypairgeneratorpositiveobjectloadstep.h"
#include "signerpositiveobjectloadstep.h"
#include "verifierpositiveobjectloadstep.h"
#include "signerverifierstep.h"

#include "pluginloadstep.h"

#include "plugincharscommonstep.h"
#include "plugincharssymmetriccipherstep.h"
#include "plugincharsasymmetriccipherstep.h"
#include "plugincharshashstep.h"
#include "plugincharsrandomstep.h"
#include "plugincharsasymsignstep.h"
#include "plugincharskeyagreestep.h"
#include "plugincharsasymkeypairstep.h"
#include "plugincharsextendedstep.h"

#include "ruleselectcommonstep.h"

#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
#include "dummyecccipherloadstep.h"
#include "dummyeccsignerloadstep.h"
#endif
//#include "pluginruleselectstep.h"

_LIT(KServerName,"Te_CryptoSpi");
CTe_CryptoSpi* CTe_CryptoSpi::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_CryptoSpi * server = new (ELeave) CTe_CryptoSpi();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_CryptoSpi* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_CryptoSpi::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}



GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTe_CryptoSpi::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KSymmetricCipherPositiveObjectLoadStep)
		testStep = new CSymmetricCipherPositiveObjectLoadStep();
	else if(aStepName == KSymmetricCipherEncryptDecryptStep)
		testStep = new CSymmetricCipherEncryptDecryptStep();
	else if(aStepName == KSymmetricCipherEncryptedDataCheckStep)
		testStep = new CSymmetricCipherEncryptedDataCheckStep();
	else if(aStepName == KSymmetricCipherIncrementalEncryptDecryptStep)
		testStep = new CSymmetricCipherIncrementalEncryptDecryptStep();
	else if(aStepName == KSymmetricCipherIncrementalEncryptDecryptMinus1ByteStep)
        testStep = new CSymmetricCipherIncrementalEncryptDecryptStep(-1);
	else if(aStepName == KSymmetricCipherIncrementalEncryptDecryptMinus3BytesStep)
        testStep = new CSymmetricCipherIncrementalEncryptDecryptStep(-3);
	else if(aStepName == KSymmetricCipherIncrementalEncryptDecryptPlus1ByteStep)
        testStep = new CSymmetricCipherIncrementalEncryptDecryptStep(1);
	else if(aStepName == KSymmetricCipherIncrementalEncryptDecryptPlus3BytesStep)
        testStep = new CSymmetricCipherIncrementalEncryptDecryptStep(3);
	else if(aStepName == KSymmetricCipherObjectReuseStep)
        testStep = new CSymmetricCipherObjectReuseStep();
	else if(aStepName == KSymmetricCipherProcessBeforeIvSetStep)
		testStep = new CSymmetricCipherProcessBeforeIvSetStep();
	else if(aStepName == KSymmetricCipherBadIvLengthStep)
		testStep = new CSymmetricCipherBadIvLengthStep();
	else if(aStepName == KSymmetricCipherCtrModeOutOfOrderStep)
		testStep = new CSymmetricCipherCtrModeOutOfOrderStep();
	
	else if(aStepName == KASymmetricCipherPositiveObjectLoadStep)
		testStep = new CASymmetricCipherPositiveObjectLoadStep();
	else if(aStepName == KASymmetricCipherEncryptedDataCheckStep)
		testStep = new CASymmetricCipherEncryptedDataCheckStep();
	else if(aStepName == KASymmetricCipherEncryptDecryptStep)
		testStep = new CASymmetricCipherEncryptDecryptStep();
	
	else if(aStepName == KHashPositiveObjectLoadStep)
		testStep = new CHashPositiveObjectLoadStep();
	else if(aStepName == KHashBasicHashOfDataStep)
		testStep = new CHashBasicHashOfDataStep();
	else if(aStepName == KHashIncrementalHashStep)
		testStep = new CHashIncrementalHashStep();
	else if(aStepName == KHashIncrementalHashWithResetStep)
		testStep = new CHashIncrementalHashWithResetStep();
	else if(aStepName == KHashIncrementalHashWithCopyStep)
		testStep = new CHashIncrementalHashWithCopyStep();
	else if(aStepName == KHashIncrementalHashWithReplicateStep)
		testStep = new CHashIncrementalHashWithReplicateStep();	
	
	else if(aStepName == KHmacPositiveObjectLoadStep)
		testStep = new CHmacPositiveObjectLoadStep();
	else if(aStepName == KHmacBasicHashOfDataStep)
		testStep = new CHmacBasicHashOfDataStep();
	else if(aStepName == KHmacIncrementalHmacStep)
		testStep = new CHmacIncrementalHmacStep();
	else if(aStepName == KHmacIncrementalHmacWithResetStep)
		testStep = new CHmacIncrementalHmacWithResetStep();
	else if(aStepName == KHmacIncrementalHmacWithCopyStep)
		testStep = new CHmacIncrementalHmacWithCopyStep();
	else if(aStepName == KHmacIncrementalHmacWithReplicateStep)
		testStep = new CHmacIncrementalHmacWithReplicateStep();
	else if(aStepName == KHmacSetKeyCheckingStep)
		testStep = new CHmacSetKeyCheckingStep();
	else if(aStepName == KHmacSetOperationModeCheckingStep)
		testStep = new CHmacSetOperationModeCheckingStep(); 	
	
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	else if(aStepName == KHashPositiveLoadStep)
		testStep = new CHashPositiveLoadStep();
	else if(aStepName == KHashBasicDataStep)
		testStep = new CHashBasicDataStep();
	else if(aStepName == KHashIncrementalStep)
		testStep = new CHashIncrementalStep();
	else if(aStepName == KHashIncrementalWithCopyStep)
		testStep = new CHashIncrementalWithCopyStep();
	else if(aStepName == KHashIncrementalWithReplicateStep)
		testStep = new CHashIncrementalWithReplicateStep();	
	else if(aStepName == KHashIncrementalWithResetStep)
		testStep = new CHashIncrementalWithResetStep();
	
	else if(aStepName == KMacPositiveObjectLoadStep)
		testStep = new CMacPositiveObjectLoadStep();
	else if(aStepName == KMacBasicDataStep)
		testStep = new CMacBasicDataStep();
	else if(aStepName == KMacIncrementalStep)
		testStep = new CMacIncrementalStep();
	else if(aStepName == KMacIncrementalWithCopyStep)
		testStep = new CMacIncrementalWithCopyStep();
	else if(aStepName == KMacIncrementalWithReplicateStep)
		testStep = new CMacIncrementalWithReplicateStep();
	else if(aStepName == KMacIncrementalResetStep)
		testStep = new CMacIncrementalResetStep();
	else if(aStepName == KSymmetricMacBasicDataStep)
			testStep = new CSymmetricMacBasicDataStep();
	else if(aStepName == KSymmetricMacIncrementalWithReplicateStep)
			testStep = new CSymmetricMacIncrementalWithReplicateStep();
	else if(aStepName == KSymmetricMacIncrementalWithCopyStep)
			testStep = new CSymmetricMacIncrementalWithCopyStep();
	else if(aStepName == KSymmetricMacIncrementalResetStep)
			testStep = new CSymmetricMacIncrementalResetStep();
	else if(aStepName == KSymmetricMacIncrementalReInitStep)
				testStep = new CSymmetricMacIncrementalReInitStep();

	else if(aStepName == KSymmetricMacIncrementalStep)
			testStep = new CSymmetricMacIncrementalStep();

	else if(aStepName == KPluginCharsMacStep)
			testStep = new CPluginCharsMacStep();
	else if(aStepName == KPluginCharsNewHashStep)
		testStep = new CPluginCharsNewHashStep();
#endif
	
	else if(aStepName == KKeyExchangeSyncStep)
		testStep = new CKeyExchangeSyncStep();
	else if(aStepName == KKeyPairGeneratorPositiveObjectLoadStep)
		testStep = new CKeyPairGeneratorPositiveObjectLoadStep();
	else if(aStepName == KRandomNumberGeneratorGeneralUsageStep)
		testStep = new CRandomNumberGeneratorGeneralUsageStep();
	else if(aStepName == KSignerPositiveObjectLoadStep)
		testStep = new CSignerPositiveObjectLoadStep();
	else if(aStepName == KVerifierPositiveObjectLoadStep)
		testStep = new CVerifierPositiveObjectLoadStep();
	else if(aStepName == KSignerVerifierStep)
		testStep = new CSignerVerifierStep();
	
	else if(aStepName == KPluginLoadStep)
		testStep = new CPluginLoadStep();
	
	else if(aStepName == KPluginCharsCommonStep)
		testStep = new CPluginCharsCommonStep();
	else if(aStepName == KPluginCharsSymmetricCipherStep)
		testStep = new CPluginCharsSymmetricCipherStep();
	else if(aStepName == KPluginCharsAsymmetricCipherStep)
		testStep = new CPluginCharsAsymmetricCipherStep();
	else if(aStepName == KPluginCharsHashStep)
		testStep = new CPluginCharsHashStep();
	else if(aStepName == KPluginCharsRandomStep)
		testStep = new CPluginCharsRandomStep();
	else if(aStepName == KPluginCharsAsymSignStep)
		testStep = new CPluginCharsAsymSignStep();
	else if(aStepName == KPluginCharsKeyAgreeStep)
		testStep = new CPluginCharsKeyAgreeStep();
	else if(aStepName == KPluginCharsAsymKeyPairStep)
		testStep = new CPluginCharsAsymKeyPairStep();
	else if(aStepName == KPluginCharsExtendedStep)
		testStep = new CPluginCharsExtendedStep();
	
	else if(aStepName == KRuleSelectCommonStep)
		testStep = new CRuleSelectCommonStep();		

#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
    else if(aStepName == KDummyEccCipherLoadStep)
        testStep = new CDummyEccCipherLoadStep();
	else if(aStepName == KDummyEccSignerLoadStep)
	        testStep = new CDummyEccSignerLoadStep();
#endif
	
	return testStep;
	}
