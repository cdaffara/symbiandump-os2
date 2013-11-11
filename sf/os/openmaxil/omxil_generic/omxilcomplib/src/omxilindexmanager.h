// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalComponent
*/

#ifndef OMXILINDEXMANAGER_H
#define OMXILINDEXMANAGER_H

#include <e32base.h>

// Forward declarations


class COmxILIndexManager : public CBase
	{

public:

	inline ~COmxILIndexManager();

	inline void InsertParamIndexL(TUint aParamIndex);

	inline TInt FindParamIndex(TUint aParamIndex) const;

	inline void InsertConfigIndexL(TUint aConfigIndex);

	inline TInt FindConfigIndex(TUint aConfigIndex) const;
	
	inline RArray<TUint>& ManagedParamIndexes();

	inline RArray<TUint>& ManagedConfigIndexes();
protected:

	inline COmxILIndexManager();

protected:

	RArray<TUint> iManagedOmxParamIndexes;
	RArray<TUint> iManagedOmxConfigIndexes;

	};

#include "omxilindexmanager.inl"

#endif // OMXILINDEXMANAGER_H

