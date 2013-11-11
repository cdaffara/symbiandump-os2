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
 @file te_uloggermclsuitedefs.cpp
 @internalTechnology
*/

#include <e32cons.h>
#include <e32def.h>
#include "te_uloggermclsuitedefs.h"
#include "lightlogger.h"

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
	loggerSession.Stop();
	loggerSession.DeActivateInputPlugin();
	CArrayPtrFlat<HBufC8> *allplugins = new (ELeave)CArrayPtrFlat<HBufC8>(1);
	loggerSession.GetInstalledOutputPlugins(*allplugins);
	for(TInt i=0;i<(allplugins->Count());i++)
	if(allplugins->Count())
	{
		TBuf8<50> dataBuf;
		dataBuf.Copy(allplugins->At(i)->Des());
		loggerSession.RemovePluginConfigurations(dataBuf);
	}
	CArrayFixFlat<TUint8> *getfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
	loggerSession.GetPrimaryFiltersEnabled(*getfilter);
	TInt Result=loggerSession.SetPrimaryFiltersEnabled(*getfilter, EFalse);
	RArray<TUint32> get2filter;	
	loggerSession.GetSecondaryFiltersEnabled(get2filter);
	loggerSession.SetSecondaryFiltersEnabled(get2filter, EFalse);
	loggerSession.SetSecondaryFilteringEnabled(ETrue);
	loggerSession.SetBufferSize(1024);
	loggerSession.SetNotificationSize(512);
}
