// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "ReadModemRecordStep.h"
#include "GetTsyNameFromCommsDat.h"

using namespace CommsDat;

_LIT(KReadModemRecord1, "ReadModemRecord1");
_LIT(KReadModemRecord2, "ReadModemRecord2");
_LIT(KModemRecordData, "ModemRecordData");

_LIT(KTsyName, "TsyName");
_LIT(KModemInitString, "ModemInitString");
_LIT(KMessageValidityPeriod, "MessageValidityPeriod");

_LIT(KDisplayStrAttr, "%S read from record is %S");
_LIT(KDisplayIntAttr, "%S read from record is %d");

CReadModemRecordStep::CReadModemRecordStep()
	{
	SetTestStepName(KReadModemRecordStep);
	}

TVerdict CReadModemRecordStep::doTestStepL()
	{
	_LIT(KReadModemRecStart, "CReadModemRecordStep::doTestStepL begin\r\n");
	Logger().WriteFormat(KReadModemRecStart);

	CCDModemBearerRecord* modemRec = GetModemRecordL();
	CleanupStack::PushL(modemRec);

	const TDesC& tsyFromRec = modemRec->iTsyName;
	INFO_PRINTF3(KDisplayStrAttr, &KTsyName, &tsyFromRec);

	const TInt KMaxInitLen = 40;
	TBuf<KMaxInitLen> initStrFromRec;
	const TDesC8& modemInitStr = modemRec->iModemInitString;
	
	if (modemInitStr.Length() > KMaxInitLen)
		{
		 
		TPtrC8 ptr8(modemInitStr.Right(KMaxInitLen));
		initStrFromRec.Copy(ptr8);
		}
	else
		{
		initStrFromRec.Copy(modemInitStr);
		}

	INFO_PRINTF3(KDisplayStrAttr, &KModemInitString, &initStrFromRec);

	TInt validityFromRec = modemRec->iMessageValidityPeriod;
	INFO_PRINTF3(KDisplayIntAttr, &KMessageValidityPeriod, validityFromRec);

	if (ConfigSection() == KReadModemRecord1)
		{
		TESTL( WriteStringToConfig(KModemRecordData, KTsyName, tsyFromRec) );
		TESTL( WriteStringToConfig(KModemRecordData, KModemInitString, initStrFromRec) );
		TESTL( WriteIntToConfig(KModemRecordData, KMessageValidityPeriod, validityFromRec) );
		}
	else
		{
		TPtrC dataFromConfig;
		TEST( GetStringFromConfig(KModemRecordData, KTsyName, dataFromConfig) );
		TEST( tsyFromRec == dataFromConfig );

		TESTL( GetStringFromConfig(KModemRecordData, KModemInitString, dataFromConfig) );
		TEST( initStrFromRec == dataFromConfig );

		TInt validityFromConfig;
		TESTL( GetIntFromConfig(KModemRecordData, KMessageValidityPeriod, validityFromConfig) );
		TEST( validityFromRec == validityFromConfig );
		}	

	CleanupStack::PopAndDestroy(modemRec);
	return TestStepResult();
	}
