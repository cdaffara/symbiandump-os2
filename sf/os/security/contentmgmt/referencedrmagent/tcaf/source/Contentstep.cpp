/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <caf/content.h>
#include <caf/data.h>
#include <caf/attribute.h>
#include "cafserver.h"
#include "bitset.h"
#include "contentstep.h"
#include "EmbeddedObject.h"
#include "StreamablePtrArray.h"
#include "attributeset.h"
#include "stringattributeset.h"
#include <caf/agentfactory.h>

using namespace ContentAccess;

/* 
 * Content attribute step
 *
 */

CCAFContentAttributeStep::~CCAFContentAttributeStep()
	{
	}

CCAFContentAttributeStep::CCAFContentAttributeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentAttributeStep);
	}

TVerdict CCAFContentAttributeStep::doTestStepL()
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
	TInt unresolvedUri = EFalse;
	TPtrC uri;
	TPtrC uniqueId;

	SetTestStepResult(EPass);

	GetIntFromConfig(ConfigSection(),_L("attribute"),attribute);
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("value"),expectedValue);
	GetIntFromConfig(ConfigSection(),_L("unresolveduri"),unresolvedUri);
	

	__UHEAP_MARK;
	
	// Open using file name
	CContent *content = CContent::NewLC(uri);
	value = expectedValue+1;
	User::LeaveIfError(content->GetAttribute(attribute, value, uniqueId));
	if(expectedValue != value )
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("CContent::GetAttribute() Expected value: %d, actual value: %d"), expectedValue, value);
		}
	CleanupStack::PopAndDestroy(content);		    		
	
	// Open using file handle
   	// cannot do if URI is unresolved - i.e. if it contains "Reference Test Agent" instead of actual private directory name
   	if (!unresolvedUri) 
		{
		RFile file;
		file.Open(iParent.Fs(), uri, EFileShareReadersOnly);		
		content = CContent::NewLC(file);
		file.Close();
		value = expectedValue+1;
		User::LeaveIfError(content->GetAttribute(attribute, value, uniqueId));
		if(expectedValue != value)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF3(_L("CContent::GetAttribute() Expected value: %d, actual value: %d"), expectedValue, value);
			}
		CleanupStack::PopAndDestroy(content);		    		
		}
		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Content attributeset step
 *
 */

CCAFContentAttributeSetStep::~CCAFContentAttributeSetStep()
	{
	}

CCAFContentAttributeSetStep::CCAFContentAttributeSetStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentAttributeSetStep);
	}

TVerdict CCAFContentAttributeSetStep::doTestStepL()
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
	TInt unresolvedUri = EFalse;
	TPtrC uri;
	TPtrC uniqueId;

	SetTestStepResult(EPass);

	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);
	GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);
	GetIntFromConfig(ConfigSection(),_L("value1"),expectedValue1);
	GetIntFromConfig(ConfigSection(),_L("value2"),expectedValue2);
	GetIntFromConfig(ConfigSection(),_L("unresolveduri"),unresolvedUri);

	__UHEAP_MARK;
	// open using filename
	CContent *content = CContent::NewLC(uri);
	RAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	attributeSet.AddL(attribute1);
	attributeSet.AddL(attribute2);

	if(uniqueId == KDefaultContentObject())
		{
		result = content->GetAttributeSet(attributeSet);
		}
	else
		{
		result = content->GetAttributeSet(attributeSet, uniqueId);
		}
	if(result != KErrNone)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CContent::GetAttribute() failed"));
		}
	
	User::LeaveIfError(attributeSet.GetValue(attribute1, value1));
	User::LeaveIfError(attributeSet.GetValue(attribute2, value2));
	if(expectedValue1 != value1 || expectedValue2 != value2 || attributeSet.Count() != 2)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CContent::GetAttributeSet() values don't match expected values"));
		}
	CleanupStack::PopAndDestroy(&attributeSet);		
	CleanupStack::PopAndDestroy(content);		
    		
   	// Open using file handle
   	// cannot do if URI is unresolved - i.e. if it contains "Reference Test Agent" instead of actual private directory name
   	if (!unresolvedUri) 
		{
		RFile file;
		file.Open(iParent.Fs(), uri, EFileShareReadersOnly);		
		content = CContent::NewLC(file);
		file.Close(); 		
	    		
		CleanupClosePushL(attributeSet);
		attributeSet.AddL(attribute1);
		attributeSet.AddL(attribute2);

		result = content->GetAttributeSet(attributeSet, uniqueId);
		if(result != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("CContent::GetAttribute() failed"));
			}
			
		User::LeaveIfError(attributeSet.GetValue(attribute1, value1));
		User::LeaveIfError(attributeSet.GetValue(attribute2, value2));
		if(expectedValue1 != value1 || expectedValue2 != value2 || attributeSet.Count() != 2)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("CContent::GetAttributeSet() values don't match expected values"));
			}
		CleanupStack::PopAndDestroy(&attributeSet);		
		CleanupStack::PopAndDestroy(content);		
	   	}
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * Content string attribute step
 *
 */

CCAFContentStringAttributeStep::~CCAFContentStringAttributeStep()
	{
	}

CCAFContentStringAttributeStep::CCAFContentStringAttributeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentStringAttributeStep);
	}

TVerdict CCAFContentStringAttributeStep::doTestStepL()
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
	TInt unresolvedUri = EFalse;
	TPtrC expectedValue;
	TBuf <200> value;
	TInt result = KErrNone;
	TPtrC uri;
	TPtrC uniqueId;

	SetTestStepResult(EPass);

	GetIntFromConfig(ConfigSection(),_L("attribute"),attribute);
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetStringFromConfig(ConfigSection(),_L("value"),expectedValue);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);
	GetIntFromConfig(ConfigSection(),_L("unresolveduri"),unresolvedUri);

	__UHEAP_MARK;
	
	// Open using file name
	CContent *content = CContent::NewLC(uri);
	
	if(uniqueId == KDefaultContentObject())
		{
		result = content->GetStringAttribute(attribute, value);
		}
	else
		{
		result = content->GetStringAttribute(attribute, value, uniqueId);
		}
	if(result != expectedResult)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("CContent::GetStringAttribute() Expected result: %d, actual result: %d"), expectedResult, result);
		}
	if(value != expectedValue)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("CContent::GetStringAttribute() Expected value: %S, actual result: %S"), &expectedValue, &value);
		}

	CleanupStack::PopAndDestroy(content);		

   	// Open using file handle
   	// cannot do if URI is unresolved - i.e. if it contains "Reference Test Agent" instead of actual private directory name
   	if (!unresolvedUri) 
	   	{
		RFile file;
		file.Open(iParent.Fs(), uri, EFileShareReadersOnly);		
		content = CContent::NewLC(file);
		file.Close(); 		

		result = content->GetStringAttribute(attribute, value, uniqueId);
		if(result != expectedResult)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF3(_L("CContent::GetStringAttribute() Expected result: %d, actual result: %d"), expectedResult, result);
			}
		else if(value != expectedValue)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF3(_L("CContent::GetStringAttribute() Expected value: %S, actual result: %S"), &expectedValue, &value);
			}
		CleanupStack::PopAndDestroy(content);		
		}		

    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Content StringAttributeSet step
 *
 */

CCAFContentStringAttributeSetStep::~CCAFContentStringAttributeSetStep()
	{
	}

CCAFContentStringAttributeSetStep::CCAFContentStringAttributeSetStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentStringAttributeSetStep);
	}

TVerdict CCAFContentStringAttributeSetStep::doTestStepL()
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

	TPtrC expectedValue1;
	TPtrC expectedValue2;
	TBuf <200> value1;
	TBuf <200> value2;
	TInt result1;
	TInt result2;
	TInt attribute1;
	TInt attribute2;
	TInt result = KErrNone;
	TInt unresolvedUri = EFalse;
	TPtrC uri;
	TPtrC uniqueId;

	SetTestStepResult(EPass);

	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);
	GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);
	GetStringFromConfig(ConfigSection(),_L("value1"),expectedValue1);
	GetStringFromConfig(ConfigSection(),_L("value2"),expectedValue2);
	GetIntFromConfig(ConfigSection(),_L("result1"),result1);
	GetIntFromConfig(ConfigSection(),_L("result2"),result2);
	GetIntFromConfig(ConfigSection(),_L("unresolveduri"),unresolvedUri);

	__UHEAP_MARK;
	
	// Open using file name
	CContent *content = CContent::NewLC(uri);
	RStringAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	attributeSet.AddL(attribute1);
	attributeSet.AddL(attribute2);
	
	if(uniqueId == KDefaultContentObject())
		{
		result = content->GetStringAttributeSet(attributeSet);	
		}
	else
		{
		result = content->GetStringAttributeSet(attributeSet, uniqueId);
		}
	if(result != KErrNone)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CContent::GetAttribute() failed"));
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
		INFO_PRINTF1(_L("CContent::GetAttributeSet() values don't match expected values"));
		}
	CleanupStack::PopAndDestroy(&attributeSet);		
	CleanupStack::PopAndDestroy(content);		
	
    		
   	// Open using file handle
   	// cannot do if URI is unresolved - i.e. if it contains "Reference Test Agent" instead of actual private directory name
   	if (!unresolvedUri) 
		{	
		RFile file;
		file.Open(iParent.Fs(), uri, EFileShareReadersOnly);		
		content = CContent::NewLC(file);
		file.Close(); 		

		CleanupClosePushL(attributeSet);
		attributeSet.AddL(attribute1);
		attributeSet.AddL(attribute2);

		result = content->GetStringAttributeSet(attributeSet, uniqueId);	
			
		if(result != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("CContent::GetAttribute() failed"));
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
			INFO_PRINTF1(_L("CContent::GetAttributeSet() values don't match expected values"));
			}
		CleanupStack::PopAndDestroy(&attributeSet);		
		CleanupStack::PopAndDestroy(content);		
		}
	__UHEAP_MARKEND;
	return TestStepResult();
	}



/* 
 * This step tests the notification functions
 *
 */

CCAFContentNotifyStep::~CCAFContentNotifyStep()
	{
	}

CCAFContentNotifyStep::CCAFContentNotifyStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentNotifyStep);
	}


TVerdict CCAFContentNotifyStep::doTestStepL()
	{
	TPtrC uri;
	TInt result;
	TRequestStatus status = KRequestPending;
	
	TInt Status1;
	TInt Cancel1;
	TInt Cancel2;
	
	
	SetTestStepResult(EPass);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetIntFromConfig(ConfigSection(),_L("Status1"),Status1);
	GetIntFromConfig(ConfigSection(),_L("Cancel1"),Cancel1);
	GetIntFromConfig(ConfigSection(),_L("Cancel2"),Cancel2);

	INFO_PRINTF2(_L("Performing notification tests on %S"), &uri);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);
	// Wait for rights 
	content->NotifyStatusChange(ERightsAvailable, status);
	User::WaitForRequest(status);
	if(status.Int() != Status1)
		{
		INFO_PRINTF3(_L("Status expected: %d returned unexpected status %d"), Status1, status.Int());
		SetTestStepResult(EFail);
		}
	result = content->CancelNotifyStatusChange(status);
	if(result != Cancel1)
		{
		INFO_PRINTF3(_L("Cancel request expected return value: %d returned unexpected value %d"), Cancel1, result);
		SetTestStepResult(EFail);
		}
	
	// Wait for rights expired but cancel before they arrive
	content->NotifyStatusChange(ERightsExpired, status);
	result = content->CancelNotifyStatusChange(status);
	if(result != Cancel2)
		{
		INFO_PRINTF3(_L("Cancel2 request expected return value: %d returned unexpected value %d"), Cancel2, result);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(content);
	
   		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step sets a property in the agents
 *
 */

CCAFContentSetPropertyStep::~CCAFContentSetPropertyStep()
	{
	}

CCAFContentSetPropertyStep::CCAFContentSetPropertyStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentSetPropertyStep);
	}


TVerdict CCAFContentSetPropertyStep::doTestStepL()
	{
	TInt expectedResult;
	TInt result;
	TPtrC uri;

	SetTestStepResult(EFail);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF2(_L("Set Property expected result: %d"), expectedResult);

	__UHEAP_MARK;

	CContent *content = CContent::NewLC(uri);
	result = content->SetProperty(EAgentPropertyBufferSize, 100);
	CleanupStack::PopAndDestroy(content);
	
	if(result == expectedResult)
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

CCAFContentDisplayInfoStep::~CCAFContentDisplayInfoStep()
	{
	}

CCAFContentDisplayInfoStep::CCAFContentDisplayInfoStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentDisplayInfoStep);
	}


TVerdict CCAFContentDisplayInfoStep::doTestStepL()
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

	CContent *content = CContent::NewLC(uri);
	TRAP(result, content->DisplayInfoL(EFileProperties));
	CleanupStack::PopAndDestroy(content);
	
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
 * This step attempts to perform an agent specific command
 *
 */

CCAFContentAgentSpecificStep::~CCAFContentAgentSpecificStep()
	{
	}

CCAFContentAgentSpecificStep::CCAFContentAgentSpecificStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentAgentSpecificStep);
	}


TVerdict CCAFContentAgentSpecificStep::doTestStepL()
	{
	TPtrC input16;
	TPtrC output16;
	TPtrC uri;
	TInt expectedResult;
	TInt command;
	TInt result;
	TBuf8 <100> actualOutput;
	TBuf8 <100> output;
	TBuf8 <100> input;


	SetTestStepResult(EPass);

	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetIntFromConfig(ConfigSection(),_L("command"),command);
	GetStringFromConfig(ConfigSection(),_L("input"),input16);
	GetStringFromConfig(ConfigSection(),_L("output"),output16);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	input.Copy(input16);
	output.Copy(output16);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);

	INFO_PRINTF1(_L("Running synchronous Agent specific command"));
	actualOutput.SetLength(0);
	result = content->AgentSpecificCommand(command, input, actualOutput);
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
	content->AgentSpecificCommand(command, input, actualOutput, status);
	User::WaitForRequest(status);
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
	
	CleanupStack::PopAndDestroy(content);		
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * This step tests the rights request functions
 *
 */

CCAFContentRequestRightsStep::~CCAFContentRequestRightsStep()
	{
	}

CCAFContentRequestRightsStep::CCAFContentRequestRightsStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentRequestRightsStep);
	}


TVerdict CCAFContentRequestRightsStep::doTestStepL()
	{
	TPtrC uri;
	TInt result;
	TRequestStatus status = KRequestPending;
	
	TInt Status1;
	TInt Cancel1;
	TInt Cancel2;
	
	
	SetTestStepResult(EPass);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetIntFromConfig(ConfigSection(),_L("Status1"),Status1);
	GetIntFromConfig(ConfigSection(),_L("Cancel1"),Cancel1);
	GetIntFromConfig(ConfigSection(),_L("Cancel2"),Cancel2);

	INFO_PRINTF2(_L("Performing Request Rights tests on %S"), &uri);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);
	// Wait for rights 
	content->RequestRights(status);
	User::WaitForRequest(status);
	if(status.Int() != Status1)
		{
		INFO_PRINTF3(_L("Status expected: %d returned unexpected status %d"), Status1, status.Int());
		SetTestStepResult(EFail);
		}
	result = content->CancelRequestRights(status);
	if(result != Cancel1)
		{
		INFO_PRINTF3(_L("Cancel request expected return value: %d returned unexpected value %d"), Cancel1, result);
		SetTestStepResult(EFail);
		}
	
	// Wait for rights expired but cancel before they arrive
	content->RequestRights(status);
	result = content->CancelRequestRights(status);
	if(result != Cancel2)
		{
		INFO_PRINTF3(_L("Cancel2 request expected return value: %d returned unexpected value %d"), Cancel2, result);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(content);
	
   		
	__UHEAP_MARKEND;
	return TestStepResult();
	}



/* 
 * This step tests the rights request functions
 *
 */

CCAFContentEmbeddedObjectsStep::~CCAFContentEmbeddedObjectsStep()
	{
	}

CCAFContentEmbeddedObjectsStep::CCAFContentEmbeddedObjectsStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentEmbeddedObjectsStep);
	}


TVerdict CCAFContentEmbeddedObjectsStep::doTestStepL()
	{
	TPtrC uri;
	TInt expectedCount;
	TInt expectedResult;
	
	SetTestStepResult(EPass);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetIntFromConfig(ConfigSection(),_L("count"),expectedCount);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF2(_L("Finding embedded objects within %S"), &uri);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);

	RStreamablePtrArray <CEmbeddedObject> array;
	CleanupClosePushL(array);

	// Wait for rights
	TRAPD(result, content->GetEmbeddedObjectsL(array));
	if(result != expectedResult)
		{
		INFO_PRINTF3(_L("GetEmbeddedObjectsL() returned result of %d, expected %d"), result, expectedResult);
		SetTestStepResult(EFail);
		}
	
	if((result == KErrNone) && (array.Count() != expectedCount))
			{
			INFO_PRINTF3(_L("GetEmbeddedObjectsL() found %d objects, expected %d"), array.Count(), expectedCount);
			SetTestStepResult(EFail);
			}

	CleanupStack::PopAndDestroy(&array);	
	CleanupStack::PopAndDestroy(content);
	
   		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step tests the rights request functions
 *
 */

CCAFContentEmbeddedObjectTypeStep::~CCAFContentEmbeddedObjectTypeStep()
	{
	}

CCAFContentEmbeddedObjectTypeStep::CCAFContentEmbeddedObjectTypeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentEmbeddedObjectTypeStep);
	}


TVerdict CCAFContentEmbeddedObjectTypeStep::doTestStepL()
	{
	TPtrC uri;
	TInt expectedContent;
	TInt expectedContainers;
	TInt expectedContainerResult;
	TInt expectedContentResult;

	SetTestStepResult(EPass);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetIntFromConfig(ConfigSection(),_L("contentcount"),expectedContent);
	GetIntFromConfig(ConfigSection(),_L("containercount"),expectedContainers);
	GetIntFromConfig(ConfigSection(),_L("containerresult"),expectedContainerResult);
	GetIntFromConfig(ConfigSection(),_L("contentresult"),expectedContentResult);

	INFO_PRINTF2(_L("Finding embedded objects within %S"), &uri);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);

	RStreamablePtrArray <CEmbeddedObject> array;
	CleanupClosePushL(array);

	// get embedded objects
	TRAPD(result, content->GetEmbeddedObjectsL(array, EContainerObject));

	if(result != expectedContainerResult)
		{
		INFO_PRINTF3(_L("GetEmbeddedObjectsL() returned result of %d, expected %d"), result, expectedContainerResult);
		SetTestStepResult(EFail);
		}

	if((result == KErrNone) && (array.Count() != expectedContainers))
		{
		INFO_PRINTF3(_L("GetEmbeddedObjectsL() found %d container objects, expected %d"), array.Count(), expectedContainers);
		SetTestStepResult(EFail);
		}

	// clear array
	array.ResetAndDestroy();
	
	// get embedded objects
	TRAP(result, content->GetEmbeddedObjectsL(array, EContentObject));

	if(result != expectedContentResult)
		{
		INFO_PRINTF3(_L("GetEmbeddedObjectsL() returned result of %d, expected %d"), result, expectedContentResult);
		SetTestStepResult(EFail);
		}

	if((result == KErrNone) && (array.Count() != expectedContent))
		{
		INFO_PRINTF3(_L("GetEmbeddedObjectsL() found %d content objects, expected %d"), array.Count(), expectedContent);
		SetTestStepResult(EFail);
		}


	CleanupStack::PopAndDestroy(&array);	
	CleanupStack::PopAndDestroy(content);
	
   		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step tests the rights request functions
 *
 */

CCAFContentSearchStep::~CCAFContentSearchStep()
	{
	}

CCAFContentSearchStep::CCAFContentSearchStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentSearchStep);
	}


TVerdict CCAFContentSearchStep::doTestStepL()
	{
	TPtrC uri;
	TPtrC mimeType16;
	TInt result;
	TInt expectedCount;
	TInt expectedResult;
	TBuf8 <255> mimeType8;
	
	SetTestStepResult(EPass);

	// Find the file, mimetype, expected return code and expected count from the INI file
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetIntFromConfig(ConfigSection(),_L("count"),expectedCount);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);
	GetStringFromConfig(ConfigSection(),_L("mimetype"),mimeType16);

	mimeType8.Copy(mimeType16);

	INFO_PRINTF3(_L("Search for embedded %S objects within %S"), &mimeType16, &uri);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);

	RStreamablePtrArray <CEmbeddedObject> array;
	CleanupClosePushL(array);

	// get embedded objects
	result = content->Search(array, mimeType8, ETrue);
	
	if(result != expectedResult)
		{
		INFO_PRINTF3(_L("Search() return value %d, expected %d"), result, expectedResult);
		SetTestStepResult(EFail);
		}
	
	if(array.Count() != expectedCount)
		{
		INFO_PRINTF3(_L("Search() found %d objects, expected %d"), array.Count(), expectedCount);
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(&array);	
	CleanupStack::PopAndDestroy(content);
	
   		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * This step tests the container open/close functions
 *
 */

CCAFContentContainerStep::~CCAFContentContainerStep()
	{
	}

CCAFContentContainerStep::CCAFContentContainerStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFContentContainerStep);
	}


TVerdict CCAFContentContainerStep::doTestStepL()
	{
	TPtrC uri;
	TPtrC uniqueId;
	TInt result;
	TInt openResult;
	TInt closeResult;
	
	
	SetTestStepResult(EPass);

	// Find the file to copy and the expected return code from the INI file
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueId"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("openResult"),openResult);
	GetIntFromConfig(ConfigSection(),_L("closeResult"),closeResult);

	INFO_PRINTF2(_L("Opening embedded container within %S"), &uri);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);

	result = content->OpenContainer(uniqueId);
	if(result != openResult)
		{
		SetTestStepResult(EFail);	
		INFO_PRINTF3(_L("OpenContainer() returned %d, expected %d"), result, openResult);
		}
	result = content->CloseContainer();
	if(result != closeResult)
		{
		SetTestStepResult(EFail);	
		INFO_PRINTF3(_L("CloseContainer() returned %d, expected %d"), result, closeResult);
		}
	CleanupStack::PopAndDestroy(content);
   		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
      
// The following methods test the various content attribute APIs for WMDRM content.     
      
TVerdict CCAFContentAttributeStep::doWmdrmTestStepL()     
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
             
    CContent *content = CContent::NewLC(*headerData);     
    TInt value;     
    User::LeaveIfError(content->GetAttribute(attribVal, value));     
         
    if(expectedValue == value)     
        {     
        SetTestStepResult(EPass);     
        }     
    else     
        {     
        INFO_PRINTF3(_L("CContent::GetAttribute() Expected value: %d, actual value: %d"), expectedValue, value);     
        }     
         
    CleanupStack::PopAndDestroy(2, headerData);                      
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
         
      
TVerdict CCAFContentAttributeSetStep::doWmdrmTestStepL()     
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
         
    CContent *content = CContent::NewLC(*headerData);     
                     
    RAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
    attributeSet.AddL(attribute1);     
    attributeSet.AddL(attribute2);     
             
    TInt result = content->GetAttributeSet(attributeSet);     
    if(result == KErrNone)     
        {     
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
            INFO_PRINTF1(_L("CContent::GetAttributeSet() values don't match expected values"));     
            }     
        }     
         
    else     
        {     
        INFO_PRINTF1(_L("CContent::GetAttributeSet() failed"));     
        }     
             
    CleanupStack::PopAndDestroy(3, headerData);          
         
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
      
      
TVerdict CCAFContentStringAttributeStep::doWmdrmTestStepL()     
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
         
    CContent *content = CContent::NewLC(*headerData);     
                 
    TBuf <200> value;     
    TInt result = content->GetStringAttribute(attribVal, value);     
    if(result == expectedResult && value == expectedValue)     
        {     
        SetTestStepResult(EPass);     
        }     
    else     
        {     
        INFO_PRINTF3(_L("CContent::GetStringAttribute() Expected result: %d, actual result: %d"), expectedResult, result);     
        INFO_PRINTF3(_L("CContent::GetStringAttribute() Expected value: %S, actual value: %S"), &expectedValue, &value);     
        }     
             
    CleanupStack::PopAndDestroy(2, headerData);          
             
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
      
      
TVerdict CCAFContentStringAttributeSetStep::doWmdrmTestStepL()     
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
         
    CContent *content = CContent::NewLC(*headerData);     
         
    RStringAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
    attributeSet.AddL(attribute1);     
    attributeSet.AddL(attribute2);     
             
    TInt result = content->GetStringAttributeSet(attributeSet);     
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
        INFO_PRINTF1(_L("CContent::GetStringAttributeSet() failed"));     
        }        
         
    CleanupStack::PopAndDestroy(3, headerData);          
      
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

