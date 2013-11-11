// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The context for the unit tests upon the CTransition class methods.
// 
//

#include "TransitionTransitions.h"

// ______________________________________________________________________________
//
inline CTransition_UnitTestContext::CTransition_UnitTestContext(CDataLogger& aDataLogger,
												MStateAccessor& aStateAccessor,
												MTransitionObserver& aObserver)
: CUnitTestContext(aDataLogger, aStateAccessor, aObserver)
	{
	//Do nothing
	}

inline CTransition_UnitTestContext::~CTransition_UnitTestContext()
	{
	delete iTargetTransition;
	delete iTargetContext;
	delete iTargetValidator;
	}

