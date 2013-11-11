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
// exported as /epoc32/include/ecom/test_bed/testharnesstemplate.h
// This file contains the pointer to the global function
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __TESTHARNESSTEMPLATE_H__
#define __TESTHARNESSTEMPLATE_H__

#include <ecom/test_bed/datalogger.h>
#include <ecom/test_bed/componenttestobserver.h>

// Pointer to the global function which must be defined in the 
// test DLL. This test harness will use this function pointer to kick start the
// tests and expects it to create the derived CComponentTester object which
// knows about thes tests required AND *push it onto the clean up stack*, 
// hence the LC name. Note that it passes its func. pointer into the 
// Test_Bed library which subsequently calls it, twice during testing.
typedef CComponentTester* (*TNewComponentTestLC)(CDataLogger& aDataLogger, MComponentTestObserver&	aComponentTestObserver);

IMPORT_C TInt E32Main_TestHarness(TNewComponentTestLC aNewComponentTestLC);

#endif // __TESTHARNESSTEMPLATE_H__
