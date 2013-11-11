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

#include "GetTsyNameFromCommsDat.h"

using namespace CommsDat;

CCDModemBearerRecord* GetModemRecordL()
	{
	CMDBSession* dbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
	CleanupStack::PushL(dbSession);

	CMDBRecordSet<CCDGlobalSettingsRecord> globalSetting(KCDTIdGlobalSettingsRecord);
	globalSetting.LoadL(*dbSession);
	
	CCDModemBearerRecord *modemBearerRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
	CleanupStack::PushL(modemBearerRecord);

	modemBearerRecord->iRecordTag = ((CCDGlobalSettingsRecord*)globalSetting.iRecords[0])->iModemForPhoneServicesAndSMS; 
	modemBearerRecord->FindL(*dbSession);

	CleanupStack::Pop();	 // modemBearerRecord
	CleanupStack::PopAndDestroy(dbSession);	

	return modemBearerRecord;
	}
