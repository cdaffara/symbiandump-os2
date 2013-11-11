// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SqlSrvSecurityMap.h"
#include "SqlSecurityImpl.h"

/**
{const TUint8*, const CSqlSecurityPolicy*} pair destructor. 
Used by RSqlMap class.

@see RSqlMap
*/
void TSqlSecurityDestructor::Destroy(const TUint8*& aKey, const CSqlSecurityPolicy*& aData)
	{
	delete const_cast <CSqlSecurityPolicy*&> (aData);
	delete [] const_cast <TUint8*&> (aKey);	
	}

/**
{const TUint8*, const TUint8*} pair destructor. 
Used by RSqlMap class.

@see RSqlMap
*/
void TSqlAttachDbDestructor::Destroy(const TUint8*& aKey, const TUint8*& aData)
	{
	delete [] const_cast <TUint8*&> (aData);
	delete [] const_cast <TUint8*&> (aKey);	
	}
