// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file te_clearconfig.cpp
 @internalTechnology
*/

#include <e32cons.h>
#include <e32def.h>
#include "te_clearconfig.h"

CClearConfig::~CClearConfig()
{
	
	/*Destructor*/
}

CClearConfig::CClearConfig()
{
	/*Constructor*/


}

void CClearConfig::ClearL(RULogger& loggerSession)
	{	
//	CArrayPtrFlat<HBufC8> *activeplugins = new (ELeave)CArrayPtrFlat<HBufC8>(1);				
//	CleanupStack::PushL(activeplugins);
//	loggerSession.GetActiveOutputPlugin(*activeplugins);
//	if (activeplugins->Count())
//	{
//		TBuf<100> dataBuf;
//		dataBuf.Copy(activeplugins->At(1)->Des());
//		loggerSession.DeActivateOutputPlugin(dataBuf);
//	}
//	activeplugins->ResetAndDestroy();
//	CleanupStack::PopAndDestroy();//ptrArray	
//
//	CArrayFixFlat<TUint8> *getfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
//	loggerSession.GetPrimaryFilters(*getfilter);
//	
//	
//	TInt Result=loggerSession.RemovePrimaryFilter(*getfilter);
//	
////	CleanupStack::PopAndDestroy();//ptrArray	
//	RArray<TUint32> get2filter;	
//	loggerSession.GetSecondaryFilters(get2filter);
//				
//	
//	loggerSession.RemoveSecondaryFilter(get2filter);
//	get2filter.Close();
//	
//	loggerSession.SetBufferMode(1);
//	loggerSession.ResizeTraceBufferSize(65000);
//
//	loggerSession.SetDataNotificationSize(2);	

	//disable all primary filters
	CArrayFixFlat<TUint8>* enabledPrimaryFilters = new (ELeave)CArrayFixFlat<TUint8>(1);
	CleanupStack::PushL(enabledPrimaryFilters);
	loggerSession.GetPrimaryFiltersEnabled(*enabledPrimaryFilters);
	loggerSession.SetPrimaryFiltersEnabled(*enabledPrimaryFilters, false);
	CleanupStack::PopAndDestroy(enabledPrimaryFilters);

	//disable all secondary filters
	RArray<TUint32> enabledSecondaryFilters;
	CleanupClosePushL(enabledSecondaryFilters);
	loggerSession.GetSecondaryFiltersEnabled(enabledSecondaryFilters);
	loggerSession.SetSecondaryFiltersEnabled(enabledSecondaryFilters, false);
	CleanupStack::PopAndDestroy(); //enabledSecondaryFilters

	//enable filtering by secondary filter
	loggerSession.SetSecondaryFilteringEnabled(ETrue);

	//set buffer mode: straight, buffer-size: 65000KB, notification size: 2KB
	loggerSession.SetBufferMode(1);
	loggerSession.SetBufferSize(65000);
	loggerSession.SetNotificationSize(2);
	}
