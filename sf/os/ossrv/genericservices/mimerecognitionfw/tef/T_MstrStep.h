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
 
 
#if (!defined __T_MSTR_STEP_H__)
#define __T_MSTR_STEP_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include <test/testexecutestepbase.h>
#include "TEmimeTestServer.h"
#include <datastor.h>

_LIT(KT_MstrStep,"T_Mstr");

class CT_MstrStep : public CTestStep
	{
public:
	CT_MstrStep();
	~CT_MstrStep();
	virtual TVerdict doTestStepL();
		
private:
	void doMenuL();
	void lPopulateMappingTablesL(CTypeStoreManager* aMan);
	void lCheckMappingsL(CTypeStoreManager* aTypeMan);
	void lClearMappingTablesL(CTypeStoreManager* aMan);
	
private:
	RFs iFs;	
	};

#endif


