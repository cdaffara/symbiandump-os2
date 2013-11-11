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
* Contains the declaration of  CMemSpyProcess class 
*/

#ifndef MEMSPYAPITHREADINFOITEM_H
#define MEMSPYAPITHREADINFOITEM_H

// System includes
#include <e32base.h>

class TMemSpyThreadInfoItemData;

NONSHARABLE_CLASS( CMemSpyApiThreadInfoItem ) 
	{
public:
	IMPORT_C ~CMemSpyApiThreadInfoItem();
	
	static CMemSpyApiThreadInfoItem* NewL(const TMemSpyThreadInfoItemData& aData);

	static CMemSpyApiThreadInfoItem* NewLC(const TMemSpyThreadInfoItemData& aData);
	
public:		
	IMPORT_C const TDesC& Caption() const;	
	
	IMPORT_C const TDesC& Value() const;	
	
private:
	CMemSpyApiThreadInfoItem();
	
	void ConstructL(const TMemSpyThreadInfoItemData& aData);
		
private:
	TMemSpyThreadInfoItemData *iInfoItem;
};

	
#endif // MEMSPYAPITHREADINFOITEM_H
