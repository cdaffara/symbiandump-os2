/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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



/**
 @file checkfolderandfilestep.h
*/
#if (!defined __CHECKFOLDERANDFILE_STEP_H__)
#define __CHECKFOLDERANDFILE_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_regscriptcommandserver.h"

// __EDIT_ME__ - Create your own test step definitions
class CCheckFolderAndFileStep : public CTestStep
	{
public:
	CCheckFolderAndFileStep();
	~CCheckFolderAndFileStep();
	virtual TVerdict doTestStepL();
	};

/*@{*/
// Literals
_LIT(KCheckFolderAndFileStep,"CheckFolderAndFile");
/*@{*/

#endif
