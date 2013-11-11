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
* cryptospistate client API implemetation
* crypto spi API implementation
*
*/


/**
 @file 
*/

#include <cryptospi/cryptospistateapi.h>
#include <cryptospi/pluginselectorbase.h>
#include <cryptospi/cryptospidef.h>
#include "cryptospiutil.h"
#include <cryptospi/pluginentrydef.h>

using namespace CryptoSpi;

//
// Implementation of CryptoSpiStateApi
//

EXPORT_C void CCryptoSpiStateApi::SetSelector(MPluginSelector* aSelector)
	{
	Dll::SetTls(aSelector);
	}

EXPORT_C void CCryptoSpiStateApi::UnsetSelector()
	{
	Dll::SetTls(0);
	}

EXPORT_C void CCryptoSpiStateApi::EnumerateCharacteristicsL(RPointerArray<CCharacteristicsAndPluginName>& aCharacteristicsDll, TInt32 aInterface, TBool aExtended)
	{
	if (aInterface==0)
		{
		TInt interfaceCount=sizeof(KInterfacesUids)/sizeof(KInterfacesUids[0]);
		for (TInt i=0;i<interfaceCount;i++)
			{
			CryptoSpiUtil::RetrieveCharacteristicsL(KInterfacesUids[i].iUid, aCharacteristicsDll);
			}
		}
	else
		{
		CryptoSpiUtil::RetrieveCharacteristicsL(aInterface, aCharacteristicsDll);
		}
		
	if (aExtended)
		{
		TInt count=aCharacteristicsDll.Count();
		for (TInt i=0;i<count;i++)
			{
			RLibrary lib;
			CryptoSpiUtil::LoadPluginDllLC(lib, aCharacteristicsDll[i]->iDllName);
			GetExtendedCharacteristicsFuncL getExtendedCharFuncL=(GetExtendedCharacteristicsFuncL)lib.Lookup(EGetExtendedCharacteristicOrdinal);
			if (getExtendedCharFuncL)
				{
				TUid implementationUid={aCharacteristicsDll[i]->iCharacteristic->iImplementationUid};
				getExtendedCharFuncL(implementationUid, aCharacteristicsDll[i]->iExtendedCharacteristic);
				}
			CleanupStack::PopAndDestroy(&lib);
			}
		}
	}











