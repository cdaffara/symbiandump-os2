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
*
*/


/**
 @file
*/

#include <e32std.h>
#include <e32math.h>
#include <e32debug.h>

#include "randomimpl.h"
#include "pluginentry.h"
#include "pluginconfig.h"
#include "securityerr.h"

using namespace SoftwareCrypto;


CRandomImpl* CRandomImpl::NewL(TUid aImplementationUid)
	{
	CRandomImpl* self = new(ELeave)CRandomImpl(aImplementationUid);
	return self;
	}

CRandomImpl* CRandomImpl::NewLC(TUid aImplementationUid)
	{
	CRandomImpl* self = NewL(aImplementationUid);
	CleanupStack::PushL(self);
	return self;
	}

void CRandomImpl::GenerateRandomBytesL(TDes8& aDestination)
	{	
    // Call the Math library to populate the buffer with random data.   
    TRAPD(err, Math::RandomL(aDestination));    
    if(err != KErrNone)
        {
        // As the end users are interested only in the security aspect of the output but not 
        // the internal states, accordingly translate the kernel side error code if required.
        err = (err == KErrNotReady) ? KErrNotSecure : err;
        
        User::Leave(err);
        }
	}

CRandomImpl::CRandomImpl(TUid aImplementationUid) : iImplementationUid(aImplementationUid)
	{
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
	
CExtendedCharacteristics* CRandomImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CRandomImpl::GetExtendedCharacteristicsL()
	{
	return CRandomImpl::CreateExtendedCharacteristicsL();
	}

TUid CRandomImpl::ImplementationUid() const
	{
	return iImplementationUid;
	}

CRandomImpl::~CRandomImpl()
	{
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
