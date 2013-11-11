/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/
#include <e32std.h>
#include <e32debug.h>

#include "randomimpl.h"
#include "pluginentry.h"
#include "pluginconfig.h"

using namespace HwCrypto;

_LIT(KLddFileName,"cryptoldd.ldd");
_LIT(KPddFileName,"crypto.h4.pdd");


CRandomImpl* CRandomImpl::NewL()
	{
	CRandomImpl* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CRandomImpl* CRandomImpl::NewLC()
	{
	CRandomImpl* self = new(ELeave)CRandomImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CRandomImpl::ConstructL()
	{
	//	RDebug::Printf("CRandomImpl::ConstructL");
	// Load PDD
    TInt r = User::LoadPhysicalDevice(KPddFileName);
	if((r != KErrNone) && (r != KErrAlreadyExists)) User::Leave(r);

	// Load LDD
    r = User::LoadLogicalDevice(KLddFileName);
	if((r != KErrNone) && (r != KErrAlreadyExists)) User::Leave(r);

	// Open driver handle
	User::LeaveIfError(iCryptoDriver.Open());
	}


void CRandomImpl::GenerateRandomBytesL(TDes8& aDest)
	{
    TRequestStatus status;
	iCryptoDriver.Random(status, aDest);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

CRandomImpl::CRandomImpl()
	{
	}


const CExtendedCharacteristics* CRandomImpl::GetExtendedCharacteristicsL()
	{
	return StaticGetExtendedCharacteristicsL();
	}

CExtendedCharacteristics* CRandomImpl::StaticGetExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

void CRandomImpl::GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics)
	{
	TInt randomNum = sizeof(KRandomCharacteristics)/sizeof(TRandomCharacteristics*);
	for (TInt i = 0; i < randomNum; i++)
		{
		if (KRandomCharacteristics[i]->cmn.iImplementationUID == ImplementationUid().iUid)
			{
			aPluginCharacteristics = KRandomCharacteristics[i];
			break;
			}
		}
	}
	
TAny* CRandomImpl::GetExtension(TUid /*aExtensionId*/)
	{
	return NULL;
	}

TUid CRandomImpl::ImplementationUid() const
	{
	return KCryptoPluginRandomUid;
	}

CRandomImpl::~CRandomImpl()
	{
	iCryptoDriver.Close();
	//	RDebug::Printf("CRandomImpl::~CRandomImpl");
	}

void CRandomImpl::Close()
	{
	delete this;
	}

// All crypto plugins must implement this, to reset
// hardware if required. Do nothing in this version
void CRandomImpl::Reset()
	{
	}

// End of file
