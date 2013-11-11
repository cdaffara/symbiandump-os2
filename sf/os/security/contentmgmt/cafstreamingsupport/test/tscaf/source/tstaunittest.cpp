// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements the unit test steps for the Test Streaming Agent.
// 
//

#include "tstaunittest.h"

using namespace StreamAccess;

CScafTestAgentServer::CScafTestAgentServer(CScafServer& aParent)
/**
 	Constructor.
 */
	:	CScafStep(aParent)
	{
	SetTestStepName(KScafTestAgentServerStep);
	}

CScafTestAgentServer::~CScafTestAgentServer()
/**
 	Destructor.
 */
	{
	//empty
	}

TVerdict CScafTestAgentServer::doTestStepL()
	{
	INFO_PRINTF1(_L("CStaTestAgentServer test"));
	__UHEAP_MARK;
	
	RStaClient sta;
	TInt r = sta.Connect();
	
	if(KErrNone == r)
		{
		sta.Close();
		}
	else
		{
		SetTestStepResult(EFail);
		}
	
	__UHEAP_MARKEND;
	return TestStepResult();
	}

