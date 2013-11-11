/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation for testing bit string encoding/decoding
*
*/


#include "testbitstr.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32cons.h>

CTestBitStr* CTestBitStr::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestBitStr* test = new (ELeave) CTestBitStr(aASN1Action);
	return test;
	}

CTestBitStr::CTestBitStr(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};

void CTestBitStr::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Bit String"));
	}

TBool CTestBitStr::PerformTestsL(CConsoleBase& aConsole)
	{
	TBool pass = ETrue;
	
	
	// Test the encoding varying length bit strings by encoding 65
	// different bit strings with a single bit set in each position. Position -1
	// indicates the empty bit string.
	for (TInt8 bitNum = -1; bitNum < 64; bitNum++)
		{
		TBuf8<8> bitStr;				
		TUint numOctets;
		
	    if (bitNum >= 0)
	    	{
	    	numOctets = 1 + (bitNum / 8);
	    	}
	    else 
	    	{
	    	numOctets = 0;
	    	}
		
		bitStr.SetLength(numOctets);
		bitStr.FillZ();
				
		TUint8 valToEncode = 0;
		if (bitNum >= 0 )
			{
			// The most significant bit in the most significant byte is bit zero
			valToEncode = (TUint8) (1 << (7 - (bitNum % 8)));
			bitStr[bitNum / 8] = valToEncode;
			}
			
		// Get the encoder and decoder
		CASN1EncBitString* encoder = CASN1EncBitString::NewLC(bitStr, bitNum + 1);
		TASN1DecBitString decoder;

		// Prepare an encode buffer
		TInt totalLength = encoder->LengthDER();
		HBufC8* encodeBuffer = HBufC8::NewMaxLC(totalLength);		
		TPtr8 tEncodeBuf = encodeBuffer->Des();		
	
		// Write into the encode buffer
		TUint writeLength = 0;
		encoder->WriteDERL(tEncodeBuf, writeLength);

		// Read it out again and check lengths plus encoded value
		TInt readLength = 0;
		HBufC8* decodeBuffer = decoder.ExtractOctetStringL(tEncodeBuf, readLength);
		CleanupStack::PushL(decodeBuffer);
		TPtr8 tDecodeBuf = decodeBuffer->Des();
						
		if (writeLength != STATIC_CAST(TUint, readLength))
			{
			aConsole.Write(_L("ERROR!\n"));
			iASN1Action.ReportProgressL(KErrASN1EncodingError, 1, 1);
			pass = EFalse;			
			}
		else if (bitNum >= 0 && valToEncode != tDecodeBuf[bitNum / 8])
			{
			aConsole.Write(_L("ENCODING ERROR!\n"));
			iASN1Action.ReportProgressL(KErrASN1EncodingError, 1, 1);
			pass = EFalse;			
			}			
		else
			{
			iASN1Action.ReportProgressL(KErrNone, bitNum + 1, 65);
			}	
			
		CleanupStack::PopAndDestroy(3, encoder); // decodeBuffer, encodeBuffer, encoder			
		}
		
	return pass;
	}


