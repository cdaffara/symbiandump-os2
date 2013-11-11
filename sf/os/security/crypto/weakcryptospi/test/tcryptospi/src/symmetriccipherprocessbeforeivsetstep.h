/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology
*/
#ifndef SYMMETRICCIPHERPROCESSBEFOREIVSETSTEP_H
#define SYMMETRICCIPHERPROCESSBEFOREIVSETSTEP_H
 
#include <test/testexecutestepbase.h>
#include "symmetriccipherstepbase.h"

class CSymmetricCipherProcessBeforeIvSetStep : public CSymmetricCipherStepBase
	{
public:
	CSymmetricCipherProcessBeforeIvSetStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
		
	};
 
_LIT(KSymmetricCipherProcessBeforeIvSetStep, "SymmetricCipherProcessBeforeIvSetStep");
 
#endif //SYMMETRICCIPHERPROCESSBEFOREIVSETSTEP_H
