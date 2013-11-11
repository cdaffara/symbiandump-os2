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

#ifndef MEMSPYAPIPKERNELOBJECT_H
#define MEMSPYAPIPKERNELOBJECT_H

// System includes
#include <e32base.h>

#include <memspy/driver/memspydriverenumerationsshared.h>

class TMemSpyKernelObjectData;

NONSHARABLE_CLASS( CMemSpyApiKernelObject ) 
	{
public:
	IMPORT_C ~CMemSpyApiKernelObject();
	
	static CMemSpyApiKernelObject* NewL(const TMemSpyKernelObjectData& aData);

	static CMemSpyApiKernelObject* NewLC(const TMemSpyKernelObjectData& aData);
	
public:
	IMPORT_C TMemSpyDriverContainerType Type() const;
	
	IMPORT_C const TDesC& Name() const;	
						
	IMPORT_C TInt Count() const;
		
	IMPORT_C TInt64 Size() const;
	
private:
	CMemSpyApiKernelObject();
	
	void ConstructL(const TMemSpyKernelObjectData& aData);
		
private:
	TMemSpyKernelObjectData *iKernelObjectData;
};

	
#endif // MEMSPYAPIPKERNELOBJECT_H
