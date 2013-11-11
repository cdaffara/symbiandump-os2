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
// Defines the setup utility step for copying rights objects into the test stream agents
// private directory
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TSETUPRIGHTSOBJECTS_H_
#define TSETUPRIGHTSOBJECTS_H_

#include "tscafstep.h"

class CScafSetupRightsObjects : public CScafStep
	{
public:
	CScafSetupRightsObjects(CScafServer& aParent);
	~CScafSetupRightsObjects();
	TVerdict doTestStepL();

private:
	};

// Test case identifier definition
_LIT(KScafSetupRightsObjectsStep,"SCAFSetupRightsObjects");

#endif /*TSETUPRIGHTSOBJECTS_H_*/
