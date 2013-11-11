/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* code at the __EDIT_ME__ tags
* for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
* in the process of the client. The client initialises the server by calling the
* one and only ordinal.
*
*/


#include "cafserver.h"
#include "cafstep.h"
#include "bitsetstep.h"
#include "Consumerstep.h"
#include "contentstep.h"
#include "RecognizerStep.h"
#include "ManagerStep.h"
#include "RightsManagerStep.h"
#include "SupplierStep.h"
#include "oomstep.h"
#include "CafUtilsStep.h"
#include "CleanupStep.h"


CCAFServer* CCAFServer::NewL()
	{
	CCAFServer * server = new (ELeave) CCAFServer();
	CleanupStack::PushL(server);
	User::LeaveIfError(server->iFs.Connect());

	// Make the file session sharable
	User::LeaveIfError(server->iFs.ShareProtected());

	// CServer base class call	
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());
	server->StartL(serverName.Name());

	CleanupStack::Pop(server);
	return server;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/*
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	// __EDIT_ME__ Your server name
	CCAFServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CCAFServer::NewL());
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
/*
 * return standard error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

CTestStep* CCAFServer::CreateTestStep(const TDesC& aStepName)
/*
 * return a CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	if(aStepName == KCAFSizeStep)
		testStep = new CCAFSizeStep(*this);
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	else if(aStepName == KCAFSizeStep64)
		testStep = new CCAFSizeStep64(*this);
	else if(aStepName == KCAFSeekReadStep64)
		testStep = new CCAFSeekReadStep64(*this);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	else if(aStepName == KCAFSeekReadStep)
		testStep = new CCAFSeekReadStep(*this);
	else if(aStepName == KBasicBitsetStep)
		testStep = new CBasicBitsetStep();
	else if(aStepName == KBitsetListStep)
		testStep = new CBitsetListStep();
	else if(aStepName == KBitsetEqualityStep)
		testStep = new CBitsetEqualityStep();
	else if(aStepName == KBitsetCopyStep)
		testStep = new CBitsetCopyStep();
	else if(aStepName == KBitsetSerialiseStep)
		testStep = new CBitsetSerialiseStep();
	else if(aStepName == KBitsetPanicStep)
		testStep = new CBitsetPanicStep();
	else if(aStepName == KCAFMultiThreadCDataStep)
		testStep = new CCAFMultiThreadCDataStep(*this);
	else if(aStepName == KCAFShareModeStep)
		testStep = new CCAFShareModeStep(*this);
	else if(aStepName == KCAFMimeTypeCDataStep)
		testStep = new CCAFMimeTypeCDataStep(*this);
	else if(aStepName == KCAFRecognizeStep)
		testStep = new CCAFRecognizeStep(*this);
	else if(aStepName == KCAFRecognizerSpeedStep)
		testStep = new CCAFRecognizerSpeedStep(*this);
	else if(aStepName == KCAFBufferSizeStep)
		testStep = new CCAFBufferSizeStep(*this);
	else if(aStepName == KCAFDeleteStep)
		testStep = new CCAFDeleteStep(*this);
	else if(aStepName == KCAFCopyFileStep)
		testStep = new CCAFCopyFileStep(*this);
	else if(aStepName == KCAFRenameFileStep)
		testStep = new CCAFRenameFileStep(*this);
	else if(aStepName == KCAFMkDirStep)
		testStep = new CCAFMkDirStep(*this);
	else if(aStepName == KCAFMkDirAllStep)
		testStep = new CCAFMkDirAllStep(*this);
	else if(aStepName == KCAFRmDirStep)
		testStep = new CCAFRmDirStep(*this);
	else if(aStepName == KCAFGetDirStep)
		testStep = new CCAFGetDirStep(*this);
	else if(aStepName == KCAFManagerNotifyStep)
		testStep = new CCAFManagerNotifyStep(*this);
	else if(aStepName == KCAFManagerSetPropertyStep)
		testStep = new CCAFManagerSetPropertyStep(*this);
	else if(aStepName == KCAFManagerDisplayInfoStep)
		testStep = new CCAFManagerDisplayInfoStep(*this);
	else if(aStepName == KCAFManagerListAgentsStep)
		testStep = new CCAFManagerListAgentsStep(*this);
	else if(aStepName == KCAFManagerAgentSpecificStep)
		testStep = new CCAFManagerAgentSpecificStep(*this);
	else if(aStepName == KCAFManagerDisplayConfigStep)
		testStep = new CCAFManagerDisplayConfigStep(*this);
	else if(aStepName == KCAFManagerAttributeStep)
		testStep = new CCAFManagerAttributeStep(*this);
	else if(aStepName == KCAFManagerAttributeSetStep)
		testStep = new CCAFManagerAttributeSetStep(*this);
	else if(aStepName == KCAFManagerStringAttributeStep)
		testStep = new CCAFManagerStringAttributeStep(*this);
	else if(aStepName == KCAFManagerStringAttributeSetStep)
		testStep = new CCAFManagerStringAttributeSetStep(*this);
	else if(aStepName == KCAFRightsManagerStep)
		testStep = new CCAFRightsManagerStep(*this);
	else if(aStepName == KCAFRightsManagerListStep)
		testStep = new CCAFRightsManagerListStep(*this);
	else if(aStepName == KCAFAttributesStep)
		testStep = new CCAFAttributesStep(*this);
	else if(aStepName == KCAFStringAttributesStep)
		testStep = new CCAFStringAttributesStep(*this);
	else if(aStepName == KCAFApparcStep)
		testStep = new CCAFApparcStep(*this);
	else if(aStepName == KCAFSupplierStep)
		testStep = new CCafSupplierStep(*this);
	else if(aStepName == KCAFSupplierAsyncStep)
		testStep = new CCafSupplierAsyncStep(*this);
	else if(aStepName == KCAFClientOutputSupplierStep)
		testStep = new CCafClientOutputSupplierStep(*this);
	else if(aStepName == KCAFSupplierSerializeStep)
		testStep = new CCAFSupplierSerializeStep(*this);
	else if(aStepName == KCAFContentAttributeStep)
		testStep = new CCAFContentAttributeStep(*this);
	else if(aStepName == KCAFContentAttributeSetStep)
		testStep = new CCAFContentAttributeSetStep(*this);
	else if(aStepName == KCAFContentStringAttributeStep)
		testStep = new CCAFContentStringAttributeStep(*this);
	else if(aStepName == KCAFContentStringAttributeSetStep)
		testStep = new CCAFContentStringAttributeSetStep(*this);	
	else if(aStepName == KCAFContentNotifyStep)
		testStep = new CCAFContentNotifyStep(*this);	
	else if(aStepName == KCAFContentSetPropertyStep)
		testStep = new CCAFContentSetPropertyStep(*this);	
	else if(aStepName == KCAFContentDisplayInfoStep)
		testStep = new CCAFContentDisplayInfoStep(*this);	
	else if(aStepName == KCAFContentAgentSpecificStep)
		testStep = new CCAFContentAgentSpecificStep(*this);	
	else if(aStepName == KCAFContentRequestRightsStep)
		testStep = new CCAFContentRequestRightsStep(*this);	
	else if(aStepName == KCAFContentEmbeddedObjectsStep)
		testStep = new CCAFContentEmbeddedObjectsStep(*this);	
	else if(aStepName == KCAFContentEmbeddedObjectTypeStep)
		testStep = new CCAFContentEmbeddedObjectTypeStep(*this);	
	else if(aStepName == KCAFContentSearchStep)
		testStep = new CCAFContentSearchStep(*this);	
	else if(aStepName == KCAFContentContainerStep)
		testStep = new CCAFContentContainerStep(*this);	
	else if(aStepName == KCAFDataAttributeStep)
		testStep = new CCAFDataAttributeStep(*this);
	else if(aStepName == KCAFDataAttributeSetStep)
		testStep = new CCAFDataAttributeSetStep(*this);
	else if(aStepName == KCAFDataStringAttributeStep)
		testStep = new CCAFDataStringAttributeStep(*this);
	else if(aStepName == KCAFDataStringAttributeSetStep)
		testStep = new CCAFDataStringAttributeSetStep(*this);	
	else if(aStepName == KCAFOomStep)
		testStep = new CCafOomStep(*this);
	else if(aStepName == KCAFHandleSizeStep)
		testStep = new CCAFHandleSizeStep(*this);
	else if(aStepName == KCAFHandleSeekReadStep)
		testStep = new CCAFHandleSeekReadStep(*this);
	else if(aStepName == KCAFHandleMultiThreadCDataStep)
		testStep = new CCAFHandleMultiThreadCDataStep(*this);	
	else if(aStepName == KCAFContentIteratorStep)
		testStep = new CCAFContentIteratorStep(*this);	
	else if(aStepName == KCAFCDirStreamStep)
		testStep = new CCafCDirStreamStep(*this);	
	else if(aStepName == KCAFRAttributeSetStreamStep)
		testStep = new CCafRAttributeSetStreamStep(*this);	
	else if(aStepName == KCAFRStringAttributeSetStreamStep)
		testStep = new CCafRStringAttributeSetStreamStep(*this);	
	else if(aStepName == KCAFSupplierOutputFileStreamStep)
		testStep = new CCafSupplierOutputFileStreamStep(*this);	
	else if(aStepName == KCAFMetaDataArrayStep)
		testStep = new CCafMetaDataArrayStep(*this);	
	else if(aStepName == KCAFEmbeddedObjectStep)
		testStep = new CCafEmbeddedObjectStep(*this);	
	else if(aStepName == KCAFVirtualPathStep)
		testStep = new CCafVirtualPathStep(*this);	
	else if(aStepName == KCAFRightsInfoStep)
		testStep = new CCafRightsInfoStep(*this);	
	else if(aStepName == KCAFStreamablePtrArrayStep)
		testStep = new CCafStreamablePtrArrayStep(*this);
#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY
	else if(aStepName == KCAFHTTPRequestHeadersStep)
		testStep = new CCAFHTTPRequestHeadersStep(*this);
#endif
	else if(aStepName == KCAFExecuteIntentStep)
		testStep = new CCAFExecuteIntentStep(*this);			
	else if(aStepName == KCAF_DEF077443_Step)
		testStep = new CCAF_DEF077443_Step(*this);
	else if(aStepName == KCAFTestCleanupStep)
		testStep = new CCAFTestCleanupStep(*this);
	else if(aStepName == KCAFDRMFileOpenPerformanceStep)
		testStep = new CCAFDRMFileOpenPerformanceTest(*this);
	else if(aStepName == KCAFManagerDisplayInfoByFileHandleStep)
		testStep = new CCAFManagerDisplayInfoByFileHandleStep(*this);
	else if(aStepName == KCAFManagerAttributeByFileHandleStep)
		testStep = new CCAFManagerAttributeByFileHandleStep(*this);
	else if(aStepName == KCAFManagerAttributeSetByFileHandleStep)
		testStep = new CCAFManagerAttributeSetByFileHandleStep(*this);
	else if(aStepName == KCAFManagerStringAttributeByFileHandleStep)
		testStep = new CCAFManagerStringAttributeByFileHandleStep(*this);
	else if(aStepName == KCAFManagerStringAttributeSetByFileHandleStep)
		testStep = new CCAFManagerStringAttributeSetByFileHandleStep(*this);
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    else if(aStepName == KCAFHelperStep)     
        testStep = new CCafHelperStep();     
    else if(aStepName == KWmdrmCAFContentStep)     
        testStep = new CWmdrmCAFContentStep();     
    else if(aStepName == KWmdrmCAFDataStep)     
        testStep = new CWmdrmCAFDataStep();     
    else if(aStepName == KWmdrmCAFReadStep)     
        testStep = new CWmdrmCAFReadStep();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
	
	return testStep;
	}

