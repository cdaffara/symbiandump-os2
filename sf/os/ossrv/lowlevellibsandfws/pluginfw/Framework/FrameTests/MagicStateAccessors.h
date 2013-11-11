// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definition of the class TExampleInterface_StateAccessor
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

#ifndef __MagicStateAccessors_H__
#define __MagicStateAccessors_H__

#include "StateAccessor.h"
#include "Interface.h"

/**
	Comments : State accessor for the CExampleInterface ECom object
 */
class TExampleInterface_StateAccessor : public MStateAccessor
	{
public:
	TInt InvariantTest(TAny* aTestObject);
	TInt Internalize(TAny* aTestObject);
	TInt Externalize(TAny* aTestObject);
	TUid GetDtorKey(CExampleInterface* aTestObject);
	};

#endif
