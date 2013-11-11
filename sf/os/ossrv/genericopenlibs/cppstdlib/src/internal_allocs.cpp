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
// Name        : internal_allocs.cpp
// Part of     : standard c++ library (wsd definitions)
// 
//

#include <e32cmn.h>
#include <ewsd.h>
EXPORT_C void* WSDAlloc(unsigned int aSize)
{
	return AllocatePls(aSize);
}

EXPORT_C int WSDFree(void *aPtr)
{
	 return FreePls(aPtr);
}
