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
* symmetriccipherbadivlength.h
*
*/




/**
 @file
 @internalTechnology
*/
#ifndef SYMMETRICCIPHERBADIVLENGTH_H
#define SYMMETRICCIPHERBADIVLENGTH_H
 
#include <test/testexecutestepbase.h>
#include "symmetriccipherstepbase.h"

class CSymmetricCipherBadIvLengthStep : public CSymmetricCipherStepBase
	{
public:
	CSymmetricCipherBadIvLengthStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
		
	};
 
_LIT(KSymmetricCipherBadIvLengthStep, "SymmetricCipherBadIvLengthStep");
 
#endif //SYMMETRICCIPHERBADIVLENGTH_H
