/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testbase.h"
#include "testmanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "testbaseTraces.h"
#endif


CTestBase::CTestBase(MTestManager& aManager)
 :	iManager(aManager)
	{
	OstTraceFunctionEntry0( CTESTBASE_CTESTBASE_ENTRY );
	OstTraceFunctionExit0( CTESTBASE_CTESTBASE_EXIT );
	}

CTestBase::~CTestBase()
	{
	OstTraceFunctionEntry0( DUP1_CTESTBASE_CTESTBASE_ENTRY );
	OstTraceFunctionExit0( DUP1_CTESTBASE_CTESTBASE_EXIT );
	}

void CTestBase::ProcessKeyL(TKeyCode /*aKeyCode*/)
	{
	OstTraceFunctionEntry0( CTESTBASE_PROCESSKEYL_ENTRY );
	OstTraceFunctionExit0( CTESTBASE_PROCESSKEYL_EXIT );
	}

void CTestBase::ProcessKeyL(const TDesC8& /*aString*/)
	{
	OstTraceFunctionEntry0( DUP1_CTESTBASE_PROCESSKEYL_ENTRY );
	OstTraceFunctionExit0( DUP1_CTESTBASE_PROCESSKEYL_EXIT );
	}
