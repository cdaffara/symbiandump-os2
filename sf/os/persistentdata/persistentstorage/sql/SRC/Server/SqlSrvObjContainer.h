// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSRVOBJCONTAINER_H__
#define __SQLSRVOBJCONTAINER_H__

#include <e32std.h>
#include "SqlAssert.h"

/**
Indexed storage for server sdide objects.

RDbObjContainer container is used for storing IPC stream handles (HIpcStream) and 
statement handles (CSqlSrvStatement).

Behaviour when the container is destroyed:
 - all objects inside the container are destroyed too;
 
Behaviour when the an object is removed from the container:
 - the container will destroy the object;

Behaviour when the an object is added to the container:
 - the container stores the object <by_ref>, not <by_val>;

Behaviour when a look up is performed in the container for an object:
 - a pointer to the object is returned or NULL if there is no object;

@see CSqlSrvStatement
@see HIpcStream
@see RDbObjContainer::Close()
@see RDbObjContainer::AllocL()
@see RDbObjContainer::Add()
@see RDbObjContainer::Remove()
@see RDbObjContainer::Find()
@see RDbObjContainer::Count()

@internalComponent
*/
template <class T> class RDbObjContainer
	{
public:
	RDbObjContainer();
	void Close();
	void AllocL();
	TInt Add(T* aObj);
	void Remove(TInt aHandle);
	T* Find(TInt aHandle) const;
	TInt Count() const;

private:	
	TInt MakeHandle(TInt aIndex) const;
	TInt MakeIndex(TInt aHandle) const;
	
private:	
	struct TEntry
		{
		T*		iObj;
		TInt	iNext;
		};
	enum {KGranularity = 16};
	//KMaxSize value depends on how many bits are used for handles - see
	//KStmtHandleMask and KStreamHandleMask in SqlUtil.h file.
	enum {KMaxSize = 0x7FF};
	TEntry*	iEntries;
	TInt	iSize;
	TInt 	iFree;
	};

#include "SqlSrvObjContainer.inl"

#endif//__SQLSRVOBJCONTAINER_H__
