/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cafserver.h"
#include "RightsManagerStep.h"
#include "manager.h"
#include "rightsmanager.h"
#include "virtualpathptr.h"
#include "StreamablePtrArray.h"
#include "agent.h"
#include "rightsinfo.h"
#include "virtualpath.h"


using namespace ContentAccess;


/* 
 * This step creates a rights manager
 *
 */

CCAFRightsManagerStep::~CCAFRightsManagerStep()
	{
	}

CCAFRightsManagerStep::CCAFRightsManagerStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFRightsManagerStep);
	}


TVerdict CCAFRightsManagerStep::doTestStepL()
	{
	TInt expectedResult;
	TInt result;
	CRightsManager *rightsmanager = NULL;
	
	RArray <TAgent> agents;

	SetTestStepResult(EFail);

	// Find the file to delete and the expected return code from the INI file
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	INFO_PRINTF2(_L("CreateAgentResolver Expected result: %d"), expectedResult);

	__UHEAP_MARK;
	CManager *manager = CManager::NewL();
	CleanupStack::PushL(manager);
	TRAP(result, manager->ListAgentsL(agents));
		
	if(result == KErrNone && agents.Count() > 0)
		{
		TRAP(result, rightsmanager = manager->CreateRightsManagerL(agents[0]));
		CleanupStack::PushL(rightsmanager);
		if(result != expectedResult && rightsmanager)
			{	
			INFO_PRINTF3(_L("Create Rights Manager Expected result: %d, actual result: %d"), expectedResult, result);
			}
		else
			{
			SetTestStepResult(EPass);	
			}
		CleanupStack::PopAndDestroy(rightsmanager);
		}

	CleanupStack::PopAndDestroy(manager);		
	agents.Close();
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * This step creates tests the rights manager listing functions
 *
 */

CCAFRightsManagerListStep::~CCAFRightsManagerListStep()
	{
	}

CCAFRightsManagerListStep::CCAFRightsManagerListStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFRightsManagerListStep);
	}


TVerdict CCAFRightsManagerListStep::doTestStepL()
	{
	TInt expectedResult;
	TInt result;
	TInt ListAPI;
	TPtrC uri;
	CRightsManager *rightsManager = NULL;
	
	RArray <TAgent> agents;
	RStreamablePtrArray <CRightsInfo> rights;
	RStreamablePtrArray <CVirtualPath> contentList;

	SetTestStepResult(EFail);

	// Find the file to delete and the expected return code from the INI file
	GetIntFromConfig(ConfigSection(),_L("API"),ListAPI);
	
	if(ListAPI !=8 && ListAPI !=9)
		{
		GetStringFromConfig(ConfigSection(),_L("uri"),uri);
		GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);
		}
		
	__UHEAP_MARK;
	CManager *manager = CManager::NewL();
	CleanupStack::PushL(manager);
	TRAP(result, manager->ListAgentsL(agents));
		
	if(result == KErrNone && agents.Count() > 0)
		{
		TRAP(result, rightsManager = manager->CreateRightsManagerL(agents[0]));
		CleanupStack::PopAndDestroy(manager);		
		if(rightsManager)
			{	
			CleanupStack::PushL(rightsManager);
			if(ListAPI == 1)
				{
				rightsManager->ListAllRightsL(rights);
				CleanupClosePushL(rights);
				if(rights.Count() == 1)
					{
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF1(_L("ListAllRights() is empty"));			
					}
				CleanupStack::PopAndDestroy(&rights);
				}
			else if(ListAPI == 2)
				{
				rightsManager->ListRightsL(rights, uri);
				CleanupClosePushL(rights);
				if(rights.Count() == 1)
					{
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF1(_L("ListRights(uri) is empty"));			
					}
				CleanupStack::PopAndDestroy(&rights);
				}
			else if(ListAPI == 3)
				{
				TVirtualPathPtr virtualPath = uri;
				rightsManager->ListRightsL(rights, virtualPath );
				CleanupClosePushL(rights);
				if(rights.Count() == 1)
					{
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF1(_L("ListRights(TVirtualPathPtr) is empty"));			
					}
				CleanupStack::PopAndDestroy(&rights);
				}
			else if(ListAPI == 4)
				{
				CRightsInfo *rightsInfo = CRightsInfo::NewL(KNullDesC(), KNullDesC(), ERightsTypeConsumable , ERightsStatusNone);
				CleanupStack::PushL(rightsInfo);
				rightsManager->ListContentL(contentList, *rightsInfo);
				CleanupStack::PopAndDestroy(rightsInfo);
				CleanupClosePushL(contentList);
				if(contentList.Count() == 1)
					{
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF1(_L("ListContent(CRightsInfo) is empty"));			
					}
				CleanupStack::PopAndDestroy(&contentList);
				}
			else if(ListAPI == 5)
				{
				CRightsInfo *rightsInfo = CRightsInfo::NewL(KNullDesC(), KNullDesC(), ERightsTypeConsumable , ERightsStatusNone);
				CleanupStack::PushL(rightsInfo);
				MAgentRightsBase *ptr = rightsManager->GetRightsDataL(*rightsInfo);
				CleanupStack::PopAndDestroy(rightsInfo);
				if(ptr == NULL)
					{
					SetTestStepResult(EPass);
					}
				}
			else if(ListAPI == 6)
				{
				CRightsInfo *rightsInfo = CRightsInfo::NewL(KNullDesC(), KNullDesC(), ERightsTypeStateless  , ERightsStatusNone);
				CleanupStack::PushL(rightsInfo);
				result = rightsManager->DeleteRightsObject(*rightsInfo);
				CleanupStack::PopAndDestroy(rightsInfo);
				if(result == KErrNone)
					{
					SetTestStepResult(EPass);
					}
				}
			else if(ListAPI == 7)
				{
				TVirtualPathPtr virtualPath = uri;
				result = rightsManager->DeleteAllRightsObjects(virtualPath);
				if(result == KErrNone)
					{
					SetTestStepResult(EPass);
					}
				}
			else if(ListAPI == 8 || ListAPI == 9)
				{
				//initialisation for testing purposes such as  iUri, iFs, iFile, iExpectedResult and iUniqueId. 
				InitialiseFileHandleParametersL();
				
				// test the RFile overload
				if (ListAPI ==8)
					{
					TRAP(result,rightsManager->ListRightsL(rights, iFile, iUniqueId));
					CleanupClosePushL(rights);	
											
					if(result == iExpectedResult)
						{
						SetTestStepResult(EPass);
						INFO_PRINTF1(_L("CRightsManager::ListRightsL()(RFile handle overload) PASSED"));
						}
					else
						{
						INFO_PRINTF1(_L("CRightsManager::ListRightsL()(RFile handle overload) returned unexpected error"));
						INFO_PRINTF3(_L("CRightsManager::ListRightsL()(RFile handle overload) Expected result: %d, actual result: %d"), iExpectedResult, result);	
						}	
					CleanupStack::PopAndDestroy(&rights);	
					}
				else if (ListAPI ==9)
					{
					result = rightsManager->DeleteAllRightsObjects(iFile, iUniqueId);
				
					if(result == iExpectedResult)
						{
						SetTestStepResult(EPass);
						INFO_PRINTF1(_L("CRightsManager::DeleteAllRightsObjects()(RFile handle overload) PASSED"));
						}
					else
						{
						INFO_PRINTF1(_L("CRightsManager::DeleteAllRightsObjects()(RFile handle overload) returned unexpected error"));		
						INFO_PRINTF3(_L("CRightsManager::DeleteAllRightsObjects()(RFile handle overload) Expected result: %d, actual result: %d"), iExpectedResult, result);	
						}						
					}
				//cleanup iFs and iFile instances by closing the handles.
				CleanupStack::PopAndDestroy(2, &iFs);	
				}
			CleanupStack::PopAndDestroy(rightsManager);
			}
		else
			{
			if (result == KErrNotSupported) // dummy test agent expects -5
				{
				SetTestStepResult(EPass);
				}
			else
				{
				INFO_PRINTF1(_L("Create CRightsManager Failed"));		
				}
			}
		}
	else
		{
		INFO_PRINTF1(_L("Create CManager Failed"));
		}
	agents.Close();
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

