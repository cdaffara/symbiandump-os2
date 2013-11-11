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
#if (!defined __SYMMETRICCIPHERINCREMENTALENCRYPTDECRYPT_STEP_H__)
#define __SYMMETRICCIPHERINCREMENTALENCRYPTDECRYPT_STEP_H__
#include <test/testexecutestepbase.h>
#include "symmetriccipherstepbase.h"

class CSymmetricCipherIncrementalEncryptDecryptStep : public CSymmetricCipherStepBase
	{
public:
	CSymmetricCipherIncrementalEncryptDecryptStep(TInt aOffset = 0);
	~CSymmetricCipherIncrementalEncryptDecryptStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	TInt iOffset;
	};

_LIT(KSymmetricCipherIncrementalEncryptDecryptStep,"SymmetricCipherIncrementalEncryptDecryptStep");
_LIT(KSymmetricCipherIncrementalEncryptDecryptMinus1ByteStep,"SymmetricCipherIncrementalEncryptDecryptMinus1ByteStep");
_LIT(KSymmetricCipherIncrementalEncryptDecryptMinus3BytesStep,"SymmetricCipherIncrementalEncryptDecryptMinus3BytesStep");
_LIT(KSymmetricCipherIncrementalEncryptDecryptPlus1ByteStep,"SymmetricCipherIncrementalEncryptDecryptPlus1ByteStep");
_LIT(KSymmetricCipherIncrementalEncryptDecryptPlus3BytesStep,"SymmetricCipherIncrementalEncryptDecryptPlus3BytesStep");


_LIT(KEncryptedFilePath,"c:\\encryptedfilepath.txt");
_LIT(KDecryptedFilePath,"c:\\decryptedfilepath.txt");


#endif
