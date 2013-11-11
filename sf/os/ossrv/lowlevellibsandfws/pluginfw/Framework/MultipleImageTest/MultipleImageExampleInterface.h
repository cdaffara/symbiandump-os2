// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MULTIPLEIMAGEEXAMPLEINTERFACE_H__
#define __MULTIPLEIMAGEEXAMPLEINTERFACE_H__

#include <e32base.h>
#include <Ecom/ECom.h>

/**
Test interface used in ECOM Multiple Image tests
@internalComponent
*/
class CMultipleImageInterface1 : public CBase
{
public:
	static CMultipleImageInterface1* NewL();
	virtual ~CMultipleImageInterface1();
	virtual TUid ImplId() = 0;

protected:
	inline CMultipleImageInterface1();

private:
	TUid iDtor_ID_Key;
};


#include "MultipleImageExampleInterface.inl"

#endif//__MULTIPLEIMAGEEXAMPLEINTERFACE_H__

