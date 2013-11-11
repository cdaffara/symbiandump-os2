/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation for testing null object encoding/decoding
*
*/


#include "testnull.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32cons.h>

CTestNull* CTestNull::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestNull* test = new (ELeave) CTestNull(aASN1Action);
	return test;
	}

CTestNull::CTestNull(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};

void CTestNull::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Null"));
	}

TBool CTestNull::PerformTestsL(CConsoleBase& aConsole)
	{
	// Get the encoder and decoder
	CASN1EncNull* encoder = CASN1EncNull::NewLC();
	TASN1DecNull decoder;

	// Prepare a buffer
	TInt totalLength = encoder->LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(totalLength);
	TPtr8 tBuf = buf->Des();
	
	// Write into the buffer
	TUint writeLength = 0;
	encoder->WriteDERL(tBuf, writeLength);

	// Read it out again + check lengths
	TInt readLength = 0;
	decoder.DecodeDERL(tBuf, readLength);

	if (writeLength != STATIC_CAST(TUint, readLength))
		{
		aConsole.Write(_L("ERROR!\n"));
		iASN1Action.ReportProgressL(KErrASN1EncodingError, 1, 1);
		CleanupStack::PopAndDestroy(2); // buf, encoder
		return(EFalse);
		}
	else
		{
		iASN1Action.ReportProgressL(KErrNone, 1, 1);
		CleanupStack::PopAndDestroy(2); // buf, encoder
		return(ETrue);
		}
	}

