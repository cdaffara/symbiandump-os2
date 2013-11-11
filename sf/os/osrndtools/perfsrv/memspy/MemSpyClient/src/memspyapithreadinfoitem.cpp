/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

#include <memspy/api/memspyapithreadinfoitem.h>
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspythreadinfoitemdata.h>

EXPORT_C CMemSpyApiThreadInfoItem::~CMemSpyApiThreadInfoItem()
	{
	delete iInfoItem;
	}

EXPORT_C const TDesC& CMemSpyApiThreadInfoItem::Caption() const
	{
	return iInfoItem->iCaption;
	}

EXPORT_C const TDesC& CMemSpyApiThreadInfoItem::Value() const
	{
	return iInfoItem->iValue;
	}

CMemSpyApiThreadInfoItem::CMemSpyApiThreadInfoItem() : iInfoItem(0)
	{
	}

void CMemSpyApiThreadInfoItem::ConstructL(const TMemSpyThreadInfoItemData& aData)
	{
	iInfoItem = new (ELeave) TMemSpyThreadInfoItemData(aData);
	}

CMemSpyApiThreadInfoItem* CMemSpyApiThreadInfoItem::NewL(const TMemSpyThreadInfoItemData& aData)
	{
	CMemSpyApiThreadInfoItem* self = CMemSpyApiThreadInfoItem::NewLC(aData);
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiThreadInfoItem* CMemSpyApiThreadInfoItem::NewLC(const TMemSpyThreadInfoItemData& aData)
	{
	CMemSpyApiThreadInfoItem* self = new (ELeave) CMemSpyApiThreadInfoItem;
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return (self);
	}

