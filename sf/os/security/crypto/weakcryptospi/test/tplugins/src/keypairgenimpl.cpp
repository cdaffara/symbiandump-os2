/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32def.h>
#include <e32cmn.h>
#include <cryptospi/keys.h>

#include "keypairgenimpl.h"

#include <cryptospi/cryptospidef.h>
#include <cryptospi/plugincharacteristics.h>
#include "pluginconfig.h"

#include "../../../source/common/inlines.h"

using namespace SoftwareCrypto;

CKeyPairGenImpl::CKeyPairGenImpl(TUid aImplementationUid) : iImplementationUid(aImplementationUid)
	{
	}

void CKeyPairGenImpl::ConstructL()
	{
	}

CKeyPairGenImpl::~CKeyPairGenImpl()
	{
	}
		
void CKeyPairGenImpl::Close()
	{
	delete this;
	}

TAny* CKeyPairGenImpl::GetExtension(TUid /*aExtensionId*/)
	{
	return 0;
	}
	
void CKeyPairGenImpl::GetCharacteristicsL(const TAny*& aPluginCharacteristics)
	{
	TInt numCiphers = sizeof(KKeyPairGeneratorCharacteristics)/sizeof(TAsymmetricKeypairGeneratorCharacteristics*);
	TInt32 implUid = ImplementationUid().iUid;
	for (TInt i = 0; i < numCiphers; ++i)
		{
		if (KKeyPairGeneratorCharacteristics[i]->cmn.iImplementationUID == implUid)
			{
			aPluginCharacteristics = KKeyPairGeneratorCharacteristics[i];
			break;
			}
		}
	}


