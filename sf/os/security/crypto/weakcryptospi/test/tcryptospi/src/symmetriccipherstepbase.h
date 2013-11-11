/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef SYMMETRICCIPHERSTEPBASE_H
#define SYMMETRICCIPHERSTEPBASE_H

#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/keys.h>
#include "te_cryptospistepbase.h"
#include "filereader.h"

using namespace CryptoSpi;

class CSymmetricCipherStepBase : public CTe_CryptoSpiStepBase
	{
protected:
	void SetupCipherL(TBool aArc4Check, TBool aRc2Check, TVariantPtrC& aOperationMode, CSymmetricCipher*& aCipher, CKey*& aKey);
	HBufC8* ReadInPlaintextL();
	HBufC8* ReadInCiphertextL();
	HBufC8* ReadInIvL();
	TInt CtrModeCalcBlockSizeL(CSymmetricCipher& aCipher);
	HBufC8* CtrModeIncrementCounterL(TDesC8& aCounter);
	HBufC8* ReadInHexCiphertextL();
	HBufC8* ReadInHexPlaintextL();
	HBufC8* ReadInHexPlainTextL(TPtrC aFile);
	HBufC8* ReadInHexIvL();
	HBufC8* ConvertFromHexFormatToRawL(TDesC8& aInput);
	
private:
	HBufC8* ReadFileL(TPtrC aFile);
	};

#endif	//SYMMETRICCIPHERSTEPBASE_H
