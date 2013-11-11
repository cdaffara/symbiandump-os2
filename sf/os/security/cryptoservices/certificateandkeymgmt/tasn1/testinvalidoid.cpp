/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation for testing object identifier decoding with invalid data
*
*/


#include "testinvalidoid.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32cons.h>
#include <e32math.h>

const TInt KNumberOftests = 2;

CTestInvalidOID* CTestInvalidOID::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestInvalidOID* test = new (ELeave) CTestInvalidOID(aASN1Action);
	return test;
	}

CTestInvalidOID::CTestInvalidOID(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};


void CTestInvalidOID::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Invalid Object Identifier"));
	}


void CTestInvalidOID::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EString);
	}

TBool CTestInvalidOID::PerformTestsL(CConsoleBase& aConsole)
	{
	TASN1DecObjectIdentifier decoder;
	TInt readLength = 0;
	HBufC* decodedData= NULL;
	TInt err = KErrNone;
	TInt testNumber = 1;
	
	// test some hardcoded examples of badly formed ASN1
	
	/* check decoding of overflow example "1.2.826.0.1.1796587.1.1.1.72057594037927942"
	from DEF099095: OID parsing error can cause X509.v3 extensions to be eclipsed */
	_LIT8(KASN1overflow, "\x06\x14\x2A\x86\x3A\x00\x01\xED\xD3\x6B\x01\x01\x01\x81\x80\x80\x80\x80\x80\x80\x80\x06");
	TRAP( err, decodedData = decoder.DecodeDERL(KASN1overflow, readLength));
	if ((err == KErrOverflow ) && ( decodedData == NULL ))
		{
		iASN1Action.ReportProgressL(KErrNone, testNumber, KNumberOftests);
		}
	else
		{
		aConsole.Write(_L("ERROR!\nShould NOT be able to decode \n"));
		iASN1Action.ReportProgressL(KErrASN1EncodingError, testNumber, KNumberOftests);
		}		

	testNumber++;
	
	// check 30 bit value is ok 0x20000006 = 536870918
	_LIT8(KASN130bit, "\x06\x10\x2A\x86\x3A\x00\x01\xED\xD3\x6B\x01\x01\x01\x82\x80\x80\x80\x06");
	_LIT16(KASN130bitResult, "1.2.826.0.1.1796587.1.1.1.536870918");
	err = KErrNone;
	TRAP( err, decodedData = decoder.DecodeDERL(KASN130bit, readLength));
	TPtr pData = decodedData->Des();
	if ((err == KErrNone ) && ( pData.Compare( KASN130bitResult ) ==0  )  )
		{
		iASN1Action.ReportProgressL(KErrNone, testNumber, KNumberOftests);
		delete decodedData;
		}
	else
		{
		aConsole.Write(_L("ERROR!\nShould NOT be able to decode \n"));
		iASN1Action.ReportProgressL(KErrASN1EncodingError, testNumber, KNumberOftests);
		}		

	testNumber++;

	// check 31 bit value is ok 0x40000006 = 1073741830
	_LIT8(KASN131bit, "\x06\x10\x2A\x86\x3A\x00\x01\xED\xD3\x6B\x01\x01\x01\x84\x80\x80\x80\x06");
	_LIT16(KASN131bitResult, "1.2.826.0.1.1796587.1.1.1.1073741830");
	err = KErrNone;
	readLength = 0;
	TRAP( err, decodedData = decoder.DecodeDERL(KASN131bit, readLength));
	pData = decodedData->Des();
	if ((err == KErrNone ) && ( pData.Compare( KASN131bitResult ) ==0  ))
		{
		iASN1Action.ReportProgressL(KErrNone, testNumber, KNumberOftests);
		delete decodedData;
		decodedData = 0;
		}
	else
		{
		aConsole.Write(_L("ERROR!\nShould NOT be able to decode \n"));
		iASN1Action.ReportProgressL(KErrASN1EncodingError, testNumber, KNumberOftests);
		}		

	testNumber++;

	// check 32 bit value does NOT decode 0x80000006 = 2147483654
	_LIT8(KASN132bit, "\x06\x10\x2A\x86\x3A\x00\x01\xED\xD3\x6B\x01\x01\x01\x88\x80\x80\x80\x06");
	err = KErrNone;
	readLength = 0;
	TRAP( err, decodedData = decoder.DecodeDERL(KASN132bit, readLength));
	if ((err == KErrOverflow ) && ( decodedData == NULL ))
		{
 		iASN1Action.ReportProgressL(KErrNone, testNumber, KNumberOftests);
		}
	else
		{
		aConsole.Write(_L("ERROR!\nShould NOT be able to decode \n"));
		iASN1Action.ReportProgressL(KErrASN1EncodingError, testNumber, KNumberOftests);
		}		

	testNumber++;

	// check that too many ids causes KErrOverflow, "1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1" 
	_LIT8(KASN1ToManyIds, "\x06\x13\x2A\x86\x3A\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01");
	err = KErrNone;
	readLength = 0;
	TRAP( err, decodedData = decoder.DecodeDERL(KASN1ToManyIds, readLength));
	if ((err == KErrOverflow ) && ( decodedData == NULL ))
		{
		iASN1Action.ReportProgressL(KErrNone, testNumber, KNumberOftests);
		}
	else
		{
		aConsole.Write(_L("ERROR!\nShould NOT be able to decode \n"));
		iASN1Action.ReportProgressL(KErrASN1EncodingError, testNumber, KNumberOftests);
		}		

	/* some more examples of Bad ASN encoding can be added here */
	
	
	return(ETrue);
	}

