// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __T_MAPS_H__)
#define __T_MAPS_H__
#include <test/testexecutestepbase.h>
#include "TEmimeTestServer.h"
_LIT(KTempRootDir,"c:\\");

class CT_Maps : public CTestStep
	{
public:
	CT_Maps();
	~CT_Maps();
//	virtual TVerdict doTestStepPreambleL();
//	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void testTMappingDataTypeToAppL();
	TBool cmpMappingDataTypeToAdd(TMappingDataTypeToApp& map1, TMappingDataTypeToApp& map2);
	
private:
	RFs iFs;
	};

_LIT(KT_Maps,"T_Maps");
#endif


