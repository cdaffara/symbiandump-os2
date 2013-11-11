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

#ifndef T_CENTREP_NOTIFIER_HANDLER_H
#define T_CENTREP_NOTIFIER_HANDLER_H

// Enums

/**
	Key data types used in the test
*/
enum TTestKeyType
	{
	EKeyTypeEnumBegin = 0,

	EKeyTypeInt = EKeyTypeEnumBegin,
	EKeyTypeReal,
	EKeyTypeString,
	EKeyTypeBinary,

	EKeyTypeAll,	// whole repository
	// ------------
	EKeyTypeEnumEnd
	};

/**
	All test cases
*/
enum TTestCase
	{
	ECaseEnumBegin = 0,
	// -------------
	ECaseListeningToSingleKey = ECaseEnumBegin,
	ECaseListeningToAllKeys,
	ECaseNoMultipleNotifications,
	ECaseNoNotificationAfterStopListening,
	ECaseWholeRepositoryReset,
	ECaseSingleKeyReset,
	ECaseTwoNotificationHandlers,
	ECaseNonExistentKey,
	ECaseIncorrectKeyType,
	ECaseRepeatingCalls,
	ECaseCancellingRequest,
	ECaseExplicitMethodCalls,
	ECaseExplicitRunErrorCall,
	ECaseOOMTest,
	// -------------
	ECaseEnumEnd
	};


/**
	Stores data describing a specific test case
*/
class TTestCaseInfo
	{
	public:
		TTestCaseInfo(TTestCase aTestCase)
			{
			iTestCase = aTestCase;
			}

		const TTestCase TestCase()
			{
			return iTestCase;
			}

		const TText* TestCaseTitle();

	public:
		// Methods to access test data 
		void SetTestData(TTestKeyType aKeyType);
		TTestKeyType KeyType();
		TInt KeyID();

	private:
		TTestCase iTestCase;
		TTestKeyType iKeyType;
	};


// Test implementation function

void ListeningTestL(TTestCase aTestCase);
void ListeningTestAllL(TTestCase aTestCase);
void ListeningTestAll2L(TTestCase aTestCase);
void ListeningWholeRepositoryL(TTestCase aTestCase);
void NonExistentKeyL();
void IncorrectKeyTypeL();
void ExplicitExerciseL(TTestCase aTestCase);
void CancelRequestL();

// Main test object
extern RTest TheTest;

#endif // T_CENTREP_NOTIFIER_HANDLER_H
