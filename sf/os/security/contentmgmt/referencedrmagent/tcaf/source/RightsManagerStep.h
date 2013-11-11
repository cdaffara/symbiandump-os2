/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#if (!defined __RIGHTS_MANAGER_STEP_H__)
#define __RIGHTS_MANAGER_STEP_H__
#include <test/testexecutestepbase.h>

#include "cafstep.h"

// Constants used to name test cases 
_LIT(KCAFRightsManagerStep,"CAFRightsManagerStep");
_LIT(KCAFRightsManagerListStep,"CAFRightsManagerListStep");

class CCAFServer;


/* 
* Used to test CManager::CreateRightsManager()
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* result - the expected return code
*
* @internalComponent Exposure internally
*/
class CCAFRightsManagerStep : public CCAFStep
	{
public:
	CCAFRightsManagerStep(CCAFServer& aParent);
	~CCAFRightsManagerStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};



/* 
* Used to test CManager::Listxxxx() functions
* The INI file specifies the file to delete and the expected return code.
* For instance this can be used to try and delete a file that does not exist or a file
* that is read-only or has rights that don't allow it to be deleted
* INI file paramerters
*
*
* result - the expected return code
*
* @internalComponent Exposure internally
*/
class CCAFRightsManagerListStep : public CCAFStep
	{
public:
	CCAFRightsManagerListStep(CCAFServer& aParent);
	~CCAFRightsManagerListStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

#endif

