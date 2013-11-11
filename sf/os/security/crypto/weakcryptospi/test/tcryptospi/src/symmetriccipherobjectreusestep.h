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
#if (!defined __SYMMETRICCIPHEROBJECTREUSE_STEP_H__)
#define __SYMMETRICCIPHEROBJECTREUSE_STEP_H__
#include <test/testexecutestepbase.h>
#include "symmetriccipherstepbase.h"

class CSymmetricCipherObjectReuseStep : public CSymmetricCipherStepBase
	{
public:
	CSymmetricCipherObjectReuseStep(TInt aOffset = 0);
	~CSymmetricCipherObjectReuseStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	TInt iOffset;
	};

_LIT(KSymmetricCipherObjectReuseStep,"SymmetricCipherObjectReuseStep");

_LIT(KTempEncryptedFilePath,"c:\\testdata\\tcryptospi\\data\\EncryptedFilePath.txt");
_LIT(KTempDecryptedFilePath,"c:\\testdata\\tcryptospi\\data\\DecryptedFilePath.txt");

#endif
