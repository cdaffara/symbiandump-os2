// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
//



/**
 @file te_perfelements.cpp
 @internalTechnology
*/
#include <e32math.h> //for cvs comparison of elements
#include "te_perfelements.h"
#include "te_ostv2integsuite_defs.h"

/**
 * Initializes the acceptable variation for each type of elementb as a percentage
 */
TElement::TElement()
	{
	iAcceptedVariation[EApiName] = 0;
	iAcceptedVariation[EApiId] = 0;
	iAcceptedVariation[EApiSize] = 0;
	iAcceptedVariation[ESizeOfUptData] = 0;
	//iAcceptedVariation[EUptSchema] = 0;
	iAcceptedVariation[EUptContext] = 0;
	iAcceptedVariation[EUptPc] = 0;
	iAcceptedVariation[ETestClassification] = 0;
	iAcceptedVariation[ETestModuleUid] = 0;
	iAcceptedVariation[EClassificationFilterMatch] = 0; 
	iAcceptedVariation[EModuleUidFilterMatch] = 0;
	iAcceptedVariation[ETraceCompiledIn] = 0;
	iAcceptedVariation[EMinimumExecutionTime] = 10;
	iAcceptedVariation[ETypicalExecutionTime] = 10;
	iAcceptedVariation[EApiStackUsage] = 0;
	iAcceptedVariation[EApiHeapUsage] = 0;
	}

// eof

