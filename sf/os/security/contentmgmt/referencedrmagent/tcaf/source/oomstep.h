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
*
*/




#if (!defined __OOM_STEP_H__)
#define __OOM_STEP_H__

#include "cafstep.h"

// Constants used to name test cases 
_LIT(KCAFOomStep,"CAFOomStep");

class CCAFServer;


/* 
* The OOMTEST parameter in the ini file determines which one of the OOM tests
* will be executed
*
* @internalComponent Exposure internally
*/
class CCafOomStep : public CCAFStep
	{
public:
	CCafOomStep(CCAFServer& aParent);
	~CCafOomStep();
	virtual TVerdict doTestStepL();
	
	// OOM tests
	TVerdict OomReadContentL();
	TVerdict OomSupplyL();

private:
	CCAFServer& iParent;
	};


#endif
