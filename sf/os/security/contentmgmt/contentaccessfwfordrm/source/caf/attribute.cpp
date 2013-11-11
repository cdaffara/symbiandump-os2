/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef REMOVE_CAF1

#include <e32std.h>
#include <caf/caftypes.h>
#include <caf/attribute.h>
#include <caf/agentinterface.h>
#include <caf/agentfactory.h>
#include "agentinfo.h"
#include <caf/bitset.h>
#include <caf/attributeset.h>

using namespace ContentAccess;

// Maximum number of attributes in CAF 1.0
const TInt KAttrTop = 10;

CAttribute* CAttribute::NewLC(TUid aAgentUid, RFile &aFile)
	{
	CAttribute* self = new(ELeave) CAttribute();
	CleanupStack::PushL(self);
	self->ConstructL(aAgentUid, aFile);
	return self;
	}

CAttribute* CAttribute::NewLC(TUid aAgentUid, const TDesC& aURI, TContentShareMode aShareMode)
	{
	CAttribute* self = new(ELeave) CAttribute();
	CleanupStack::PushL(self);
	self->ConstructL(aAgentUid, aURI, aShareMode);
	return self;
	}
	
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
CAttribute* CAttribute::NewLC(TUid aAgentUid, const TDesC8& aHeaderData)
	{
	CAttribute* self = new(ELeave) CAttribute();
	CleanupStack::PushL(self);
	self->ConstructL(aAgentUid, aHeaderData);
	return self;
	}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

CAttribute::CAttribute()
	{
	}

CAttribute::~CAttribute() 
	{ 
	// delete the attribute sets
	delete iQuerySet;
	delete iResponseSet;

	// Terminate the agent implementation
	delete iAgentContent;
	delete iAgentFactory;
	REComSession::FinalClose();
	}

void CAttribute::ConstructL(TUid aAgentUid, const TDesC& aURI, TContentShareMode aShareMode)
	{
	// create a agent factory implementation (pseudo ECOM handle)
	iAgentFactory = CAgentFactory::NewL(aAgentUid);
	iAgentContent = iAgentFactory->CreateContentBrowserL(aURI, aShareMode);

	iQuerySet = CBitset::NewL(static_cast<TInt>(KAttrTop));
	iResponseSet = CBitset::NewL(static_cast<TInt>(KAttrTop));
    }

void CAttribute::ConstructL(TUid aAgentUid, RFile& aFile)
	{
	// create a agent factory implementation (pseudo ECOM handle)
	iAgentFactory = CAgentFactory::NewL(aAgentUid);
	iAgentContent = iAgentFactory->CreateContentBrowserL(aFile);

	iQuerySet = CBitset::NewL(static_cast<TInt>(KAttrTop));
	iResponseSet = CBitset::NewL(static_cast<TInt>(KAttrTop));
    }

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
void CAttribute::ConstructL(TUid aAgentUid, const TDesC8& aHeaderData)
	{
	// create a agent factory implementation (pseudo ECOM handle)
	iAgentFactory = CAgentFactory::NewL(aAgentUid);
	iAgentContent = iAgentFactory->CreateContentBrowserL(aHeaderData);

	iQuerySet = CBitset::NewL(static_cast<TInt>(KAttrTop));
	iResponseSet = CBitset::NewL(static_cast<TInt>(KAttrTop));
    }
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

EXPORT_C void CAttribute::Reset()
	{
	iQuerySet->Reset();
	iResponseSet->Reset();
	}

EXPORT_C CBitset& CAttribute::QuerySet()
	{
	ASSERT(iQuerySet);
	return *iQuerySet;
	}

EXPORT_C const CBitset& CAttribute::ResponseSet() const
	{
	ASSERT(iResponseSet);
	return *iResponseSet;
	}

EXPORT_C void CAttribute::GetL()
	{
	TInt i = 0;
	TInt value = 0;
	TInt err = KErrNone;

	RAttributeSet aAttributeSet;
	CleanupClosePushL(aAttributeSet);
	
	for(i=0 ;i < KAttrTop ;i++ )
		{
		if(iQuerySet->IsSet(i))
			{
			aAttributeSet.AddL(i);
			}
		}
	iAgentContent ->GetAttributeSet(aAttributeSet, KDefaultContentObject());
	
	for( i = 0; i < KAttrTop; i++)
		{
		err = aAttributeSet.GetValue(i, value);
		if(err == KErrNone && value == (TInt) ETrue)
			{
			iResponseSet->Set(i);
			}
		}
	CleanupStack::PopAndDestroy(&aAttributeSet);
	}

#endif // REMOVE_CAF1
