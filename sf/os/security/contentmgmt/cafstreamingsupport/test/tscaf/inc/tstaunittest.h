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
// Defines the unit test steps for the Test Streaming Agent.
// 
//

/**
 @file 
 @internalComponent
 @test
*/

#ifndef TSTAUNITTEST_H_
#define TSTAUNITTEST_H_

#include "tscafstep.h"
#include <caf/rightsinfo.h>

// Constant used to name this test case
_LIT(KScafTestAgentServerStep,"SCAFTestAgentServer");

class CScafTestAgentServer : public CScafStep
	{
public:
	CScafTestAgentServer(CScafServer& aParent);
	~CScafTestAgentServer();
	TVerdict doTestStepL();
	};

#endif /*TSTAUNITTEST_H_*/
