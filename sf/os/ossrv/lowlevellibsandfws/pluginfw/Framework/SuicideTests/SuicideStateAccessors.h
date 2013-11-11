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
// This file contains the definition of the class TSuicideInterface_StateAccessor
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

#ifndef __SUICIDESTATEACCESSORS_H__
#define __SUICIDESTATEACCESSORS_H__

#include "StateAccessor.h"

/**
	Comments : State accessor for the CExampleInterface suiciding Implementation object
 */
class TSuicideInterface_StateAccessor : public MStateAccessor
	{
public:
	TInt InvariantTest(TAny* aTestObject);
	TInt Internalize(TAny* aTestObject);
	TInt Externalize(TAny* aTestObject);
	};

#endif
