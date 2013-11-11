/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <test/testexecutelog.h>
#include <apmstd.h>

#include "cafserver.h"
#include "ManagerStep.h"
#include "manager.h"
#include "dirstreamable.h"
#include "virtualpathptr.h"
#include "agent.h"
#include "attributeset.h"
#include "stringattributeset.h"
#include "contentIterator.h"

using namespace ContentAccess;



/* 
 * This step deletes a file using the CAF framework
 *
 */

CCAFDeleteStep::~CCAFDeleteStep()
	{
	}

CCAFDeleteStep::CCAFDeleteStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFDeleteStep);
	}


TVerdict CCAFDeleteStep::doTestStepL()
	{
	TPtrC fileName;

	TInt expectedResult;
	TInt result;

	SetTestStepResult(EFail);

	// Find the file to delete and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("filename"),fileName);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF3(_L("Delete File %S Expected result: %d"), &fileName, expectedResult);

	__UHEAP_MARK;

	TRAP(result, CManager::DeleteFileL(fileName));

	if(result != KErrNone)
		{
		INFO_PRINTF2(_L("Delete file left: %d"), result);
		if(result== expectedResult)
			{
			SetTestStepResult(EPass);
			}
		}
	else if(result == expectedResult)
		{	
		SetTestStepResult(EPass);
		}
	else INFO_PRINTF2(_L("Delete file left with error: %d"), result);
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * This step copies a file using the CAF framework
 *
 */

CCAFCopyFileStep::~CCAFCopyFileStep()
	{
	}

CCAFCopyFileStep::CCAFCopyFileStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFCopyFileStep);
	}


TVerdict CCAFCopyFileStep::doTestStepL()
	{
	TPtrC source;
	TPtrC destination;

	TInt expectedResult;
	TInt result;

	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("source"),source);
	GetStringFromConfig(ConfigSection(),_L("destination"),destination);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF4(_L("Copy %S to %S, Expected result: %d"), &source, &destination, expectedResult);

	__UHEAP_MARK;

	CManager *manager = CManager::NewLC();

	result = manager->CopyFile(source, destination);
	if(result == expectedResult)
		{
		SetTestStepResult(EPass);			
		}
	else 
		{
		INFO_PRINTF2(_L("CopyFile(source as filename overload) returned with unexpected error: %d"), result);
		}
	// set up 2nd overload testing
	RFs fs;
	RFile file;

	// read the input file and pass it to the CAF
	fs.Connect();
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());		

	result = file.Open(fs, source, EFileRead | EFileStream | EFileShareAny);
	if (result == KErrNone)
		{
		CleanupClosePushL(file);
		
		// test the RFile overload
		result = manager->CopyFile(file, destination);
		
		if ((result == expectedResult)&&(TestStepResult()==EPass))
			{
			SetTestStepResult(EPass);			
			}
		else 
			{
			INFO_PRINTF2(_L("CopyFile(source as RFile handle overload) returned with unexpected error: %d"), result);
			}
			
		CleanupStack::PopAndDestroy(&file); 
		
		}
	else if ((result == expectedResult)&&(TestStepResult()==EPass))
		{
		SetTestStepResult(EPass);			
		}
	else 
		{
		INFO_PRINTF2(_L("CopyFile(source as RFile handle overload) returned with unexpected error: %d"), result);
		}
		
	CleanupStack::PopAndDestroy(&fs); 

	CleanupStack::PopAndDestroy(manager);
	    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}
/* 
 * This step renames a file using the CAF framework
 *
 */

CCAFRenameFileStep::~CCAFRenameFileStep()
	{
	}

CCAFRenameFileStep::CCAFRenameFileStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFRenameFileStep);
	}


TVerdict CCAFRenameFileStep::doTestStepL()
	{
	TPtrC source;
	TPtrC destination;

	TInt expectedResult;
	TInt result;

	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("source"),source);
	GetStringFromConfig(ConfigSection(),_L("destination"),destination);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF4(_L("Rename from %S to %S, Expected result: %d"), &source, &destination, expectedResult);

	__UHEAP_MARK;

	CManager *manager = CManager::NewLC();
	result = manager->RenameFile(source, destination);
	CleanupStack::PopAndDestroy(manager);
	
	if(result == expectedResult)
		{	
		SetTestStepResult(EPass);
		}
	else 
		{
		INFO_PRINTF2(_L("RenameFile() returned with unexpected error: %d"), result);
		}
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * This step creates a directory using the CAF framework
 *
 */

CCAFMkDirStep::~CCAFMkDirStep()
	{
	}

CCAFMkDirStep::CCAFMkDirStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFMkDirStep);
	}


TVerdict CCAFMkDirStep::doTestStepL()
	{
	TPtrC path;
	TInt expectedResult;
	TInt result;

	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("path"),path);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF3(_L("Create directory %S, Expected result: %d"), &path, expectedResult);

	__UHEAP_MARK;

	CManager *manager = CManager::NewLC();
	result = manager->MkDir(path);
	CleanupStack::PopAndDestroy(manager);
	
	if(result == expectedResult)
		{	
		SetTestStepResult(EPass);
		}
	else 
		{
		INFO_PRINTF2(_L("MkDir() returned with unexpected error: %d"), result);
		}
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * This step creates several directory using the CAF framework
 *
 */

CCAFMkDirAllStep::~CCAFMkDirAllStep()
	{
	}

CCAFMkDirAllStep::CCAFMkDirAllStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFMkDirAllStep);
	}


TVerdict CCAFMkDirAllStep::doTestStepL()
	{
	TPtrC path;
	TInt expectedResult;
	TInt result;

	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("path"),path);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF3(_L("Create directory %S, Expected result: %d"), &path, expectedResult);

	__UHEAP_MARK;

	CManager *manager = CManager::NewLC();
	// remove directory in case it already exists
	manager->RmDir(path);
	result = manager->MkDirAll(path);
	CleanupStack::PopAndDestroy(manager);
	
	if(result == expectedResult)
		{	
		SetTestStepResult(EPass);
		}
	else 
		{
		INFO_PRINTF2(_L("MkDirAll() returned with unexpected error: %d"), result);
		}
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step removes a directory using the CAF framework
 *
 */

CCAFRmDirStep::~CCAFRmDirStep()
	{
	}

CCAFRmDirStep::CCAFRmDirStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFRmDirStep);
	}


TVerdict CCAFRmDirStep::doTestStepL()
	{
	TPtrC path;
	TInt expectedResult;
	TInt result;

	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("path"),path);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF3(_L("Remove directory %S, Expected result: %d"), &path, expectedResult);

	__UHEAP_MARK;

	CManager *manager = CManager::NewLC();
	result = manager->RmDir(path);
	CleanupStack::PopAndDestroy(manager);
	
	if(result == expectedResult)
		{	
		SetTestStepResult(EPass);
		}
	else 
		{
		INFO_PRINTF2(_L("RmDir() returned with unexpected error: %d"), result);
		}
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step lists the contents of a directory using the CAF framework
 *
 */

CCAFGetDirStep::~CCAFGetDirStep()
	{
	}

CCAFGetDirStep::CCAFGetDirStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFGetDirStep);
	}


TVerdict CCAFGetDirStep::doTestStepL()
	{
	TPtrC path;
	TInt expectedResult;
	TInt result;
	TInt GetDirAPI = 0;

	CDir *entrylist = NULL;
	CDir *dirlist = NULL;
	CDir *filelist = NULL;


	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("path"),path);
	GetIntFromConfig(ConfigSection(),_L("API"),GetDirAPI);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF3(_L("List contents of directory %S, Expected result: %d"), &path, expectedResult);

	__UHEAP_MARK;

	CManager *manager = CManager::NewLC();
	if(GetDirAPI == 1)
		{
		result = manager->GetDir(path,ESortByName, KEntryAttNormal, entrylist);
		}
	else if(GetDirAPI == 2)
		{
		result = manager->GetDir(path,ESortByName, KEntryAttNormal, entrylist, dirlist);
		}
	else
		{
		result = manager->GetDir(path,TUidType(), ESortByName, filelist);
		}
	CleanupStack::PopAndDestroy(manager);

	if(entrylist)
		{
		INFO_PRINTF2(_L("%d items in EntryList:"), entrylist->Count());	
		DisplayList(*entrylist);
		delete entrylist;
		entrylist = NULL;
		}

	if(dirlist)
		{
		INFO_PRINTF2(_L("%d items in DirList:"), dirlist->Count());	
		DisplayList(*dirlist);
		delete dirlist;
		dirlist = NULL;
		}
	
	if(filelist)
		{
		INFO_PRINTF2(_L("%d items in FileList:"), filelist->Count());	
		DisplayList(*filelist);
		delete filelist;
		filelist = NULL;
		}
	
	if(result == expectedResult)
		{	
		SetTestStepResult(EPass);
		}
	else 
		{
		INFO_PRINTF2(_L("GetDir() returned with unexpected error: %d"), result);
		}
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


void CCAFGetDirStep::DisplayList(CDir& aDir)
	{
	TInt i = 0;
	for(i = 0; i < aDir.Count(); i++)
		{
		INFO_PRINTF2(_L("			%S"), &aDir[i].iName );	
		}
	}
	
	
/* 
 * This step tests the notification functions
 *
 */

CCAFManagerNotifyStep::~CCAFManagerNotifyStep()
	{
	}

CCAFManagerNotifyStep::CCAFManagerNotifyStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerNotifyStep);
	}


TVerdict CCAFManagerNotifyStep::doTestStepL()
	{
	TPtrC uri;
	TInt result;
	TRequestStatus status = KRequestPending;
	
	TInt Status1;
	TInt Cancel1;
	TInt Cancel2;
	
	
	SetTestStepResult(EPass);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("path"),uri);
	GetIntFromConfig(ConfigSection(),_L("Status1"),Status1);
	GetIntFromConfig(ConfigSection(),_L("Cancel1"),Cancel1);
	GetIntFromConfig(ConfigSection(),_L("Cancel2"),Cancel2);

	INFO_PRINTF2(_L("Performing notification tests on %S"), &uri);

	__UHEAP_MARK;
	CManager *manager = CManager::NewLC();
	// Wait for rights 
	manager->NotifyStatusChange(uri, ERightsAvailable, status);
	User::WaitForRequest(status);
	if(status.Int() != Status1)
		{
		INFO_PRINTF3(_L("Status expected: %d returned unexpected status %d"), Status1, status.Int());
		SetTestStepResult(EFail);
		}
	result = manager->CancelNotifyStatusChange(uri, status);
	if(result != Cancel1)
		{
		INFO_PRINTF3(_L("Cancel request expected return value: %d returned unexpected value %d"), Cancel1, result);
		SetTestStepResult(EFail);
		}
	
	// Wait for rights expired but cancel before they arrive
	manager->NotifyStatusChange(uri, ERightsExpired, status);
	result = manager->CancelNotifyStatusChange(uri, status);
	if(result != Cancel2)
		{
		INFO_PRINTF3(_L("Cancel2 request expected return value: %d returned unexpected value %d"), Cancel2, result);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(manager);
	
   		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step sets a property in the agents
 *
 */

CCAFManagerSetPropertyStep::~CCAFManagerSetPropertyStep()
	{
	}

CCAFManagerSetPropertyStep::CCAFManagerSetPropertyStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerSetPropertyStep);
	}


TVerdict CCAFManagerSetPropertyStep::doTestStepL()
	{
	TInt expectedResult;
	TInt result;

	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF2(_L("Set Property expected result: %d"), expectedResult);

	__UHEAP_MARK;

	CManager *manager = CManager::NewLC();
	result = manager->SetProperty(EAgentPropertyBufferSize, 100);
	CleanupStack::PopAndDestroy(manager);
	
	// dummy test Agent expects KErrNotSupported
	if(result == expectedResult || result == KErrNotSupported)
		{	
		SetTestStepResult(EPass);
		}
	else 
		{
		INFO_PRINTF2(_L("SetProperty() returned with unexpected error: %d"), result);
		}
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * This step asks the agent to display information about a file
 *
 */

CCAFManagerDisplayInfoStep::~CCAFManagerDisplayInfoStep()
	{
	}

CCAFManagerDisplayInfoStep::CCAFManagerDisplayInfoStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerDisplayInfoStep);
	}


TVerdict CCAFManagerDisplayInfoStep::doTestStepL()
	{
	TPtrC uri;
	TInt expectedResult;
	TInt result;

	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF3(_L("DisplayInfo for %S expected result: %d"), &uri, expectedResult);

	__UHEAP_MARK;

	CManager *manager = CManager::NewLC();
	TRAP(result, manager->DisplayInfoL(EFileProperties, TVirtualPathPtr(uri)));
	CleanupStack::PopAndDestroy(manager);
	
	if(result == expectedResult)
		{	
		SetTestStepResult(EPass);
		}
	else 
		{
		INFO_PRINTF2(_L("DisplayInfoL() left with unexpected error: %d"), result);
		}
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step asks CAF for a list of agents
 *
 */

CCAFManagerListAgentsStep::~CCAFManagerListAgentsStep()
	{
	}

CCAFManagerListAgentsStep::CCAFManagerListAgentsStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerListAgentsStep);
	}


TVerdict CCAFManagerListAgentsStep::doTestStepL()
	{
	TPtrC uri;
	TInt expectedResult;
	TInt expectedNumber;
	TInt result;

	RArray <TAgent> agents;
	
	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetIntFromConfig(ConfigSection(),_L("count"),expectedNumber);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF2(_L("List agents expected result: %d"), expectedResult);

	__UHEAP_MARK;

	CManager *manager = CManager::NewLC();
	TRAP(result, manager->ListAgentsL(agents));
	CleanupStack::PopAndDestroy(manager);
	
	TInt i = 0;
	for (i = 0; i < agents.Count(); i++)
		{
		TPtrC agentName = agents[i].Name();
		INFO_PRINTF2(_L("			%S"), &agentName);
		}
		
	INFO_PRINTF2(_L("List agents expected result: %d"), expectedResult);

	if(result == expectedResult)
		{	
		if(expectedNumber == agents.Count())
			{
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(_L("Expected number of agents: %d"), expectedNumber);
			}
		}
	else 
		{
		INFO_PRINTF2(_L("ListAgents left with unexpected error: %d"), result);
		}
		
	agents.Close();
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step attempts to perform an agent specific command
 *
 */

CCAFManagerAgentSpecificStep::~CCAFManagerAgentSpecificStep()
	{
	}

CCAFManagerAgentSpecificStep::CCAFManagerAgentSpecificStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerAgentSpecificStep);
	}


TVerdict CCAFManagerAgentSpecificStep::doTestStepL()
	{
	TPtrC input16;
	TPtrC output16;
	TInt expectedResult;
	TInt command;
	TInt result;
	TBuf8 <100> actualOutput;
	TBuf8 <100> output;
	TBuf8 <100> input;
	TInt index = 0;

	RArray <TAgent> agents;
	_LIT(KTestAgentName, "Reference Test Agent");
	
	SetTestStepResult(EPass);

	GetIntFromConfig(ConfigSection(),_L("command"),command);
	GetStringFromConfig(ConfigSection(),_L("input"),input16);
	GetStringFromConfig(ConfigSection(),_L("output"),output16);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	input.Copy(input16);
	output.Copy(output16);

	__UHEAP_MARK;
	CManager *manager = CManager::NewLC();
	TRAP(result, manager->ListAgentsL(agents));

	INFO_PRINTF1(_L("Running synchronous Agent specific command"));
	actualOutput.SetLength(0);
	if(result == KErrNone && agents.Count() > 0)
		{
		for(index = 0; index < agents.Count(); index++)
			{
			if(agents[index].Name() == KTestAgentName())
				{
				break;
				}
			}
		result = manager->AgentSpecificCommand(agents[index],command, input, actualOutput);
		}
	else
		{
		SetTestStepResult(EFail);
		}
	if(result != expectedResult)
		{	
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("Async Expected result: %d, actual result: %d"), expectedResult, result);
		}
	else if(actualOutput != output)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("Async Expected output: %S, actual output: %S"), &output, &actualOutput);
		}

	INFO_PRINTF1(_L("Running asynchronous Agent specific command"));
	TRequestStatus status;
	actualOutput.SetLength(0);
	if((result == KErrNone || result == expectedResult) && agents.Count() > 0)
		{
		manager->AgentSpecificCommand(agents[index], command, input, actualOutput, status);
		User::WaitForRequest(status);
		}
	else
		{
		SetTestStepResult(EFail);
		}
	if(status.Int() != expectedResult)
		{	
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("Async Expected result: %d, actual result: %d"), expectedResult, status.Int());
		}
	else if(actualOutput != output)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("Async Expected output: %S, actual output: %S"), &output, &actualOutput);
		}
	
	CleanupStack::PopAndDestroy(manager);		
	agents.Close();
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step attempts to display configuration information
 *
 */

CCAFManagerDisplayConfigStep::~CCAFManagerDisplayConfigStep()
	{
	}

CCAFManagerDisplayConfigStep::CCAFManagerDisplayConfigStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerDisplayConfigStep);
	}

TVerdict CCAFManagerDisplayConfigStep::doTestStepL()
	{
	TInt expectedResult;
	TInt result = KErrNone;

	RArray <TAgent> agents;
	CManager *manager;

	SetTestStepResult(EPass);

	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	__UHEAP_MARK;
	manager = CManager::NewL();
	CleanupStack::PushL(manager);
	TRAP(result, manager->ListAgentsL(agents));
		
	if(result == KErrNone && agents.Count() > 0)
		{
		TRAP(result, manager->DisplayManagementInfoL(agents[0]));
		}
	else
		{
		SetTestStepResult(EFail);
		}
	if(result != expectedResult && result != KErrNotSupported) // dummy test agent expects -5
		{	
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("Expected result: %d, actual result: %d"), expectedResult, result);
		}

	CleanupStack::PopAndDestroy(manager);		
	agents.Close();
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Manager attribute step
 *
 */

CCAFManagerAttributeStep::~CCAFManagerAttributeStep()
	{
	}

CCAFManagerAttributeStep::CCAFManagerAttributeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerAttributeStep);
	}

TVerdict CCAFManagerAttributeStep::doTestStepL()
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TBool wmdrmFlag = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("wmdrmEnabled"), wmdrmFlag);     
         
    if(wmdrmFlag)     
        {     
        TVerdict verdict = doWmdrmTestStepL();     
        return verdict;     
        }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT    

	TInt attribute;
	TInt value = KErrNone;
	TInt expectedValue;
	TPtrC uri;
	TPtrC uniqueId;

	CManager *manager;

	SetTestStepResult(EPass);

	GetIntFromConfig(ConfigSection(),_L("attribute"),attribute);
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("value"),expectedValue);

	__UHEAP_MARK;
	manager = CManager::NewL();
	if(manager)
		{
		CleanupStack::PushL(manager);
		User::LeaveIfError(manager->GetAttribute(attribute, value, TVirtualPathPtr(uri, uniqueId)));
		if(expectedValue!= value)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF3(_L("CManager::GetAttribute() Expected value: %d, actual value: %d"), expectedValue, value);
			}
		CleanupStack::PopAndDestroy(manager);		
		}
	else
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CManager construction failed"));
		}
	
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Manager attributeset step
 *
 */

CCAFManagerAttributeSetStep::~CCAFManagerAttributeSetStep()
	{
	}

CCAFManagerAttributeSetStep::CCAFManagerAttributeSetStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerAttributeSetStep);
	}

TVerdict CCAFManagerAttributeSetStep::doTestStepL()
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    TBool wmdrmFlag = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("wmdrmEnabled"), wmdrmFlag);     
         
    if(wmdrmFlag)     
        {     
        TVerdict verdict = doWmdrmTestStepL();     
        return verdict;     
        }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT    

	TInt value1;
	TInt value2;
	TInt expectedValue1;
	TInt expectedValue2;
	TInt attribute1;
	TInt attribute2;
	TInt result = KErrNone;
	TPtrC uri;
	TPtrC uniqueId;

	CManager *manager;

	SetTestStepResult(EPass);

	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);
	GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);
	GetIntFromConfig(ConfigSection(),_L("value1"),expectedValue1);
	GetIntFromConfig(ConfigSection(),_L("value2"),expectedValue2);

	__UHEAP_MARK;
	manager = CManager::NewL();
	if(manager)
		{
		CleanupStack::PushL(manager);
		RAttributeSet attributeSet;
		CleanupClosePushL(attributeSet);
		attributeSet.AddL(attribute1);
		attributeSet.AddL(attribute2);
		result = manager->GetAttributeSet(attributeSet, TVirtualPathPtr(uri, uniqueId));
		if(result != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("CManager::GetAttribute() failed"));
			}
		User::LeaveIfError(attributeSet.GetValue(attribute1, value1));
		User::LeaveIfError(attributeSet.GetValue(attribute2, value2));
		if(expectedValue1 != value1 || expectedValue2 != value2 || attributeSet.Count() != 2)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("CManager::GetAttributeSet() values don't match expected values"));
			}
		CleanupStack::PopAndDestroy(&attributeSet);		
		CleanupStack::PopAndDestroy(manager);		
		}
	else
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CManager construction failed"));
		}
	
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * Manager string attribute step
 *
 */

CCAFManagerStringAttributeStep::~CCAFManagerStringAttributeStep()
	{
	}

CCAFManagerStringAttributeStep::CCAFManagerStringAttributeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerStringAttributeStep);
	}

TVerdict CCAFManagerStringAttributeStep::doTestStepL()
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    TBool wmdrmFlag = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("wmdrmEnabled"), wmdrmFlag);     
         
    if(wmdrmFlag)     
        {     
        TVerdict verdict = doWmdrmTestStepL();     
        return verdict;     
        }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT        
     
	TInt expectedResult;
	TInt attribute;
	TPtrC expectedValue;
	TBuf <200> value;
	TInt result = KErrNone;
	TPtrC uri;
	TPtrC uniqueId;

	CManager *manager;

	SetTestStepResult(EPass);

	GetIntFromConfig(ConfigSection(),_L("attribute"),attribute);
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetStringFromConfig(ConfigSection(),_L("value"),expectedValue);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	__UHEAP_MARK;
	manager = CManager::NewL();
	if(manager)
		{
		CleanupStack::PushL(manager);
		result = manager->GetStringAttribute(attribute, value, TVirtualPathPtr(uri, uniqueId));
		if(result != expectedResult)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF3(_L("CManager::GetStringAttribute() Expected result: %d, actual result: %d"), expectedResult, result);
			}
		if(value != expectedValue)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF3(_L("CManager::GetStringAttribute() Expected value: %S, actual result: %S"), &expectedValue, &value);
			}

		CleanupStack::PopAndDestroy(manager);		
		}
	else
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CManager construction failed"));
		}
	
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Manager StringAttributeSet step
 *
 */

CCAFManagerStringAttributeSetStep::~CCAFManagerStringAttributeSetStep()
	{
	}

CCAFManagerStringAttributeSetStep::CCAFManagerStringAttributeSetStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerStringAttributeSetStep);
	}

TVerdict CCAFManagerStringAttributeSetStep::doTestStepL()
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    TBool wmdrmFlag = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("wmdrmEnabled"), wmdrmFlag);     
         
    if(wmdrmFlag)     
        {     
        TVerdict verdict = doWmdrmTestStepL();     
        return verdict;     
        }     
#endif  //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
     
	TPtrC expectedValue1;
	TPtrC expectedValue2;
	TBuf <200> value1;
	TBuf <200> value2;
	TInt result1;
	TInt result2;
	TInt attribute1;
	TInt attribute2;
	TInt result = KErrNone;
	TPtrC uri;
	TPtrC uniqueId;

	CManager *manager;

	SetTestStepResult(EPass);

	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);
	GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);
	GetStringFromConfig(ConfigSection(),_L("value1"),expectedValue1);
	GetStringFromConfig(ConfigSection(),_L("value2"),expectedValue2);
	GetIntFromConfig(ConfigSection(),_L("result1"),result1);
	GetIntFromConfig(ConfigSection(),_L("result2"),result2);


	__UHEAP_MARK;
	manager = CManager::NewL();
	if(manager)
		{
		CleanupStack::PushL(manager);
		RStringAttributeSet attributeSet;
		CleanupClosePushL(attributeSet);
		attributeSet.AddL(attribute1);
		attributeSet.AddL(attribute2);
		result = manager->GetStringAttributeSet(attributeSet, TVirtualPathPtr(uri, uniqueId));
		if(result != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("CManager::GetAttribute() failed"));
			}
	if(result1 != attributeSet.GetValue(attribute1, value1))
		{
		INFO_PRINTF1(_L("RStringAttributeSet::GetValue failed"));
		}
	if(result2 != attributeSet.GetValue(attribute2, value2))
		{
		INFO_PRINTF1(_L("RStringAttributeSet::GetValue failed"));
		}
	if(value1 != expectedValue1 || value2 != expectedValue2 || attributeSet.Count() != 2)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("CManager::GetAttributeSet() values don't match expected values"));
			}
		CleanupStack::PopAndDestroy(&attributeSet);		
		CleanupStack::PopAndDestroy(manager);		
		}
	else
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CManager construction failed"));
		}
	
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}




CIteratorTestStateMachine::CIteratorTestStateMachine(CCAFContentIteratorStep *aParent) : CActive(EPriorityStandard) 
	{
	iParent = aParent;
	}
		
CIteratorTestStateMachine::~CIteratorTestStateMachine()
	{
	delete iter;	
	delete iPath;
	delete iMimeType;
	}

void CIteratorTestStateMachine::DoCancel()
	{
	// Not used
	CActiveScheduler::Stop();
	}


void CIteratorTestStateMachine::RunTestL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType)
		{
		iPath = aPath.AllocL();
		iMimeType = aMimeType.AllocL();
		iRecursive = aRecursive;
			
		// This function will only return once all files have been found
		// and the RunL() method calls CActiveScheduler::Stop()
		CActiveScheduler::Add(this);
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus *ptr = &iStatus;
		User::RequestComplete(ptr, KErrNone);
		CActiveScheduler::Start();		
		}
		
void CIteratorTestStateMachine::RunL()
	{
	TBuf <KMaxDataTypeLength> mime16;
	TVirtualPathPtr location(KNullDesC(), KNullDesC());
	
	if(iStatus.Int() != KErrNone)
		{
		delete iter;
		iter = NULL;
		CActiveScheduler::Stop();
		}
	else
		{
		switch(iState)
			{
		case 0: // create iterator
			iter = CContentIterator::NewL(*iPath, iRecursive, *iMimeType);
			iStatus = KRequestPending;
			iter->Next(iStatus);
			SetActive();
			iState = 1;
			break;
			
		case 1:   // get result of Next request
			location = iter->VirtualPath();
			mime16.Copy(iter->MimeType());
			iParent->PrintResult(location.URI(), location.UniqueId(), mime16);
			iStatus = KRequestPending;
			iter->Next(iStatus);
			SetActive();
			break;
		default:
			CActiveScheduler::Stop();
			break;
			};
		}
	}

/* 
 * Content Iterator Step
 *
 */

CCAFContentIteratorStep::~CCAFContentIteratorStep()
	{
	}

CCAFContentIteratorStep::CCAFContentIteratorStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentIteratorStep);
	}

TVerdict CCAFContentIteratorStep::doTestStepL()
	{
	TPtrC path;
	TPtrC mimeType;
	TBuf8 <KMaxDataTypeLength> mimeType8;
		
	SetTestStepResult(EPass);

	GetStringFromConfig(ConfigSection(),_L("path"),path);
	GetStringFromConfig(ConfigSection(),_L("mimetype"),mimeType);

	mimeType8.Copy(mimeType);
	
	__UHEAP_MARK;
	
	CIteratorTestStateMachine *t = new CIteratorTestStateMachine(this);
	
	t->RunTestL(path, ETrue, mimeType8);
	
	delete t;	
	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CCAFContentIteratorStep::PrintResult(const TDesC& aFileName, const TDesC& aUniqueId, const TDesC& aMimeType)
	{
	INFO_PRINTF4(_L("File: %S, UniqueId: %S, MimeType: %S"), &aFileName, &aUniqueId, &aMimeType);
	}


/* 
 * This step asks the agent to display information about a file using the file handle
 *
 */
CCAFManagerDisplayInfoByFileHandleStep::~CCAFManagerDisplayInfoByFileHandleStep()
	{
	}

CCAFManagerDisplayInfoByFileHandleStep::CCAFManagerDisplayInfoByFileHandleStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerDisplayInfoByFileHandleStep);
	}


TVerdict CCAFManagerDisplayInfoByFileHandleStep::doTestStepL()
	{
	TInt result;
	
	//initialisation for testing purposes such as  iUri, iFs, iFile, iExpectedResult and iUniqueId. 
	InitialiseFileHandleParametersL();

	SetTestStepResult(EFail);

	__UHEAP_MARK;
	CManager *manager = CManager::NewLC();

	TRAP(result, manager->DisplayInfoL(EFileProperties, iFile, iUniqueId));	
	if(result == iExpectedResult)
		{
		SetTestStepResult(EPass);
		INFO_PRINTF1(_L("CManager::DisplayInfoL()(RFile handle overload) PASSED"));
		}
	else
		{
		INFO_PRINTF1(_L("CManager::DisplayInfoL()(RFile handle overload) returned unexpected error"));
		INFO_PRINTF3(_L("CManager::DisplayInfoL()(RFile handle overload) Expected result: %d, actual result: %d"), iExpectedResult, result);	
		}
	
	//cleanup manager instance
	CleanupStack::PopAndDestroy(manager);	
	//cleanup iFs and iFile instances by closing the handles.
	CleanupStack::PopAndDestroy(2, &iFs);	

	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Manager attribute step using file handle.
 *
 */

CCAFManagerAttributeByFileHandleStep::~CCAFManagerAttributeByFileHandleStep()
	{
	}

CCAFManagerAttributeByFileHandleStep::CCAFManagerAttributeByFileHandleStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerAttributeByFileHandleStep);
	}

TVerdict CCAFManagerAttributeByFileHandleStep::doTestStepL()
	{
	TInt attribute;
	TInt value = KErrNone;
	TInt result= KErrNone;
	TInt expectedValue;

	//initialisation for testing purposes such as  iUri, iFs, iFile, iExpectedResult and iUniqueId. 
	InitialiseFileHandleParametersL();
	GetIntFromConfig(ConfigSection(),_L("attribute"),attribute);
	GetIntFromConfig(ConfigSection(),_L("value"),expectedValue);
	
	SetTestStepResult(EFail);

	__UHEAP_MARK;
	CManager *manager = CManager::NewLC();

	result = manager->GetAttribute(attribute, value, iFile, iUniqueId);
	//expectedResult has priority over the values collected.
	if(result == iExpectedResult)
		{
		if(value == expectedValue)
			{
			SetTestStepResult(EPass);
			INFO_PRINTF1(_L("CManager::GetAttribute()(RFile handle overload) PASSED"));
			}
		else
			{
			INFO_PRINTF1(_L("CManager::GetAttribute()(RFile handle overload) values don't match expected values"));
			INFO_PRINTF3(_L("CManager::GetAttribute()(RFile handle overload) Expected value: %d, actual value: %d"), expectedValue, value);	
			}	
		}
	else
		{
		INFO_PRINTF1(_L("CManager::GetAttribute()(RFile handle overload) returned unexpected error"));
		INFO_PRINTF3(_L("CManager::GetAttribute()(RFile handle overload) Expected result: %d, actual result: %d"), iExpectedResult, result);	
		}
	
	//cleanup manager instance
	CleanupStack::PopAndDestroy(manager);		
	//cleanup iFs and iFile instances by closing the handles.
	CleanupStack::PopAndDestroy(2, &iFs);
   		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Manager attributeset step by file handle.
 *
 */

CCAFManagerAttributeSetByFileHandleStep::~CCAFManagerAttributeSetByFileHandleStep()
	{
	}

CCAFManagerAttributeSetByFileHandleStep::CCAFManagerAttributeSetByFileHandleStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerAttributeSetByFileHandleStep);
	}

TVerdict CCAFManagerAttributeSetByFileHandleStep::doTestStepL()
	{
	TInt value1;
	TInt value2;
	TInt expectedValue1;
	TInt expectedValue2;
	TInt attribute1;
	TInt attribute2;
	TInt result = KErrNone;

	//initialisation for testing purposes such as  iUri, iFs, iFile, iExpectedResult and iUniqueId. 
	InitialiseFileHandleParametersL();
	GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);
	GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);
	GetIntFromConfig(ConfigSection(),_L("value1"),expectedValue1);
	GetIntFromConfig(ConfigSection(),_L("value2"),expectedValue2);

	SetTestStepResult(EFail);

	__UHEAP_MARK;
	CManager *manager = CManager::NewLC();
	
	RAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	attributeSet.AddL(attribute1);
	attributeSet.AddL(attribute2);

	result = manager->GetAttributeSet(attributeSet, iFile, iUniqueId);
	//expectedResult has priority over the values collected.		
	if(result == iExpectedResult)
		{
		User::LeaveIfError(attributeSet.GetValue(attribute1, value1));
		User::LeaveIfError(attributeSet.GetValue(attribute2, value2));
		if(value1 == expectedValue1 && value2 == expectedValue2)
			{
			SetTestStepResult(EPass);						
			INFO_PRINTF1(_L("CManager::GetAttributeSet()(RFile handle overload) PASSED"));	
			}
		else
			{
			INFO_PRINTF1(_L("CManager::GetAttributeSet()(RFile handle overload) values don't match expected values"));
			INFO_PRINTF3(_L("CManager::GetAttributeSet()(RFile handle overload) Expected value: %d, actual value: %d"), expectedValue1, value1);	
			INFO_PRINTF3(_L("CManager::GetAttributeSet()(RFile handle overload) Expected value: %d, actual value: %d"), expectedValue2, value2);	
			}	
		}
	else
		{
		INFO_PRINTF1(_L("CManager::GetAttributeSet()(RFile handle overload) returned unexpected error"));
		INFO_PRINTF3(_L("CManager::GetAttributeSet()(RFile handle overload) Expected result: %d, actual result: %d"), iExpectedResult, result);	
		}

	//cleanup manager and attributeSet instances		
	CleanupStack::PopAndDestroy(2, manager);	
	//cleanup iFs and iFile instances by closing the handles.
	CleanupStack::PopAndDestroy(2, &iFs);
	    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * Manager string attribute step by file handle
 *
 */

CCAFManagerStringAttributeByFileHandleStep::~CCAFManagerStringAttributeByFileHandleStep()
	{
	}

CCAFManagerStringAttributeByFileHandleStep::CCAFManagerStringAttributeByFileHandleStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerStringAttributeByFileHandleStep);
	}

TVerdict CCAFManagerStringAttributeByFileHandleStep::doTestStepL()
	{
	TInt attribute;
	TInt result = KErrNone;
	TPtrC expectedValue;
	TBuf <200> value;
	
	//initialisation for testing purposes such as  iUri, iFs, iFile, iExpectedResult and iUniqueId. 
	InitialiseFileHandleParametersL();
	GetIntFromConfig(ConfigSection(),_L("attribute"),attribute);
	GetStringFromConfig(ConfigSection(),_L("value"),expectedValue);
	
	SetTestStepResult(EFail);	
	
	__UHEAP_MARK;
	CManager* manager = CManager::NewLC();

	result = manager->GetStringAttribute(attribute, value, iFile, iUniqueId);
	//expectedResult has priority over the values collected.		
	if(result == iExpectedResult)
		{
		if (value == expectedValue) 
			{
			SetTestStepResult(EPass);
			INFO_PRINTF1(_L("CManager::GetStringAttribute()(RFile handle overload) PASSED"));
			}
		else
			{
			INFO_PRINTF1(_L("CManager::GetStringAttribute()(RFile handle overload) values don't match expected values"));
			INFO_PRINTF3(_L("CManager::GetStringAttribute()(RFile handle overload) Expected value: %S, actual value: %S"), &expectedValue, &value);	
			}	
		}
	else
		{
		INFO_PRINTF1(_L("CManager::GetStringAttribute()(RFile handle overload) returned unexpected error"));
		INFO_PRINTF3(_L("CManager::GetStringAttribute()(RFile handle overload) Expected result: %d, actual result: %d"), iExpectedResult, result);	
		}

	//cleanup manager instance		
	CleanupStack::PopAndDestroy(manager);	
	//cleanup iFs and iFile instances by closing the handles.
	CleanupStack::PopAndDestroy(2, &iFs);	
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Manager StringAttributeSet step by file handle.
 *
 */

CCAFManagerStringAttributeSetByFileHandleStep::~CCAFManagerStringAttributeSetByFileHandleStep()
	{
	}

CCAFManagerStringAttributeSetByFileHandleStep::CCAFManagerStringAttributeSetByFileHandleStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFManagerStringAttributeSetByFileHandleStep);
	}

TVerdict CCAFManagerStringAttributeSetByFileHandleStep::doTestStepL()
	{
	TInt attribute1;
	TInt attribute2;
	TInt result = KErrNone;
	TPtrC expectedValue1;
	TPtrC expectedValue2;
	TBuf <200> value1;
	TBuf <200> value2;
	
	//initialisation for testing purposes such as  iUri, iFs, iFile, iExpectedResult and iUniqueId. 
	InitialiseFileHandleParametersL();
	GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);
	GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);
	GetStringFromConfig(ConfigSection(),_L("value1"),expectedValue1);
	GetStringFromConfig(ConfigSection(),_L("value2"),expectedValue2);
	
	SetTestStepResult(EFail);

	__UHEAP_MARK;
	CManager* manager = CManager::NewLC();

	RStringAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	attributeSet.AddL(attribute1);
	attributeSet.AddL(attribute2);

	result = manager->GetStringAttributeSet(attributeSet, iFile, iUniqueId);
	//expectedResult has priority over the values collected.	
	if(result == iExpectedResult)
		{
		User::LeaveIfError(attributeSet.GetValue(attribute1, value1));
		User::LeaveIfError(attributeSet.GetValue(attribute2, value2));
		if (value1 == expectedValue1 && value2 == expectedValue2) 
			{
			SetTestStepResult(EPass);						
			INFO_PRINTF1(_L("CManager::GetStringAttributeSet()(RFile handle overload) PASSED"));
			}
		else
			{
			INFO_PRINTF1(_L("CManager::GetStringAttributeSet()(RFile handle overload) values don't match expected values"));
			INFO_PRINTF3(_L("CManager::GetStringAttributeSet()(RFile handle overload) Expected value: %S, actual value: %S"), &expectedValue1, &value1);	
			INFO_PRINTF3(_L("CManager::GetStringAttributeSet()(RFile handle overload) Expected value: %S, actual value: %S"), &expectedValue2, &value2);				
			}	
		}
	else
		{
		INFO_PRINTF1(_L("CManager::GetStringAttributeSet()(RFile handle overload) returned unexpected error"));
		INFO_PRINTF3(_L("CManager::GetStringAttributeSet()(RFile handle overload) Expected result: %d, actual result: %d"), iExpectedResult, result);	
		}

	//cleanup manager and attributeSet instances		
	CleanupStack::PopAndDestroy(2, manager);
	//cleanup iFs and iFile instances by closing the handles.
	CleanupStack::PopAndDestroy(2, &iFs);	
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
      
// The following methods test the various manager attribute APIs for WMDRM content.     
      
TVerdict CCAFManagerAttributeStep::doWmdrmTestStepL()     
    {     
    SetTestStepResult(EFail);     
         
    TInt attribVal;     
    GetIntFromConfig(ConfigSection(),_L("attribute"), attribVal);     
         
    TInt expectedValue;     
    GetIntFromConfig(ConfigSection(),_L("value"), expectedValue);     
      
    __UHEAP_MARK;     
         
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
         
    TInt value;     
    CManager *manager = CManager::NewLC();     
         
    User::LeaveIfError(manager->GetAttribute(*headerData, attribVal, value));     
    if(expectedValue == value)     
        {     
        SetTestStepResult(EPass);     
        }     
    else     
        {     
        INFO_PRINTF3(_L("CManager::GetAttribute() Expected value: %d, actual value: %d"), expectedValue, value);     
        }     
             
    CleanupStack::PopAndDestroy(2, headerData);          
                 
    __UHEAP_MARKEND;     
    return TestStepResult();     
    }     
         
      
TVerdict CCAFManagerAttributeSetStep::doWmdrmTestStepL()     
    {     
    SetTestStepResult(EFail);     
         
    TInt attribute1;     
    GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);     
         
    TInt attribute2;     
    GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);     
         
    TInt expectedValue1;     
    GetIntFromConfig(ConfigSection(),_L("value1"),expectedValue1);     
         
    TInt expectedValue2;     
    GetIntFromConfig(ConfigSection(),_L("value2"),expectedValue2);     
      
__UHEAP_MARK;     
         
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
                 
    RAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
    attributeSet.AddL(attribute1);     
    attributeSet.AddL(attribute2);     
             
    CManager *manager = CManager::NewLC();       
    TInt result = manager->GetAttributeSet(*headerData, attributeSet);     
    if(result == KErrNone)     
        {     
        SetTestStepResult(EPass);     
        }     
    else     
        {     
        INFO_PRINTF1(_L("CManager::GetAttributeSet() failed"));     
        }     
             
    TInt value1;         
    User::LeaveIfError(attributeSet.GetValue(attribute1, value1));     
             
    TInt value2;     
    User::LeaveIfError(attributeSet.GetValue(attribute2, value2));     
             
    if(expectedValue1 == value1 && expectedValue2 == value2 && attributeSet.Count() == 2)     
        {     
        SetTestStepResult(EPass);     
        }     
    else     
        {     
        INFO_PRINTF1(_L("CManager::GetAttributeSet() values don't match expected values"));     
        }     
             
    CleanupStack::PopAndDestroy(3, headerData);          
      
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
      
      
TVerdict CCAFManagerStringAttributeStep::doWmdrmTestStepL()     
    {     
    SetTestStepResult(EFail);     
         
    TInt attribVal;     
    GetIntFromConfig(ConfigSection(),_L("attribute"),attribVal);     
         
    TPtrC expectedValue;     
    GetStringFromConfig(ConfigSection(),_L("value"),expectedValue);     
         
    TInt expectedResult;     
    GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);     
      
__UHEAP_MARK;     
         
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
         
    CManager* manager = CManager::NewLC();           
    TBuf <200> value;     
    TInt result = manager->GetStringAttribute(*headerData, attribVal, value);     
    if(result == expectedResult && value == expectedValue)     
        {     
        SetTestStepResult(EPass);     
        }     
    else     
        {     
        INFO_PRINTF3(_L("CManager::GetStringAttribute() Expected result: %d, actual result: %d"), expectedResult, result);     
        INFO_PRINTF3(_L("CManager::GetStringAttribute() Expected value: %S, actual value: %S"), &expectedValue, &value);     
        }     
             
    CleanupStack::PopAndDestroy(2, headerData);          
         
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
      
      
TVerdict CCAFManagerStringAttributeSetStep::doWmdrmTestStepL()     
    {     
    SetTestStepResult(EFail);     
      
    TInt attribute1;         
    GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);     
         
    TInt attribute2;     
    GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);     
         
    TPtrC expectedValue1;     
    GetStringFromConfig(ConfigSection(),_L("value1"),expectedValue1);     
         
    TPtrC expectedValue2;     
    GetStringFromConfig(ConfigSection(),_L("value2"),expectedValue2);     
         
__UHEAP_MARK;     
         
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
      
    RStringAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
    attributeSet.AddL(attribute1);     
    attributeSet.AddL(attribute2);     
             
    CManager* manager = CManager::NewLC();       
    TInt result = manager->GetStringAttributeSet(*headerData, attributeSet);     
    TBuf <200> value1;     
    TBuf <200> value2;     
    if(result == KErrNone)     
        {     
        TInt result3 = attributeSet.GetValue(attribute1, value1);     
        TInt result4 = attributeSet.GetValue(attribute2, value2);     
                 
        if(value1 == expectedValue1 && value2 == expectedValue2 && attributeSet.Count() == 2     
         && result3 == KErrNone && result4 == KErrNone)     
            {     
            SetTestStepResult(EPass);     
            }     
        else     
            {     
            INFO_PRINTF3(_L("RStringAttributeSet::GetValue() for attribute1.Expected value: %S, actual value: %S"), &expectedValue1, &value1);     
            INFO_PRINTF3(_L("RStringAttributeSet::GetValue() for attribute2.Expected value: %S, actual value: %S"), &expectedValue2, &value2);     
            INFO_PRINTF3(_L("RStringAttributeSet::GetValue() for attribute1. Expected result: %d, actual result: %d"), 0, result3);     
            INFO_PRINTF3(_L("RStringAttributeSet::GetValue() for attribute2. Expected result: %d, actual result: %d"), 0, result4);      
            }     
            }     
    else     
        {     
        INFO_PRINTF1(_L("CManager::GetStringAttributeSet() failed"));     
        }        
         
    CleanupStack::PopAndDestroy(3, headerData);          
      
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
