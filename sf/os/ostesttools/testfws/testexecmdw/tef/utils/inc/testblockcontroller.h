/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* This contains TestBlockController.h
*
*/



#ifndef __TEST_BLOCK_CONTROLLER_H__
#define __TEST_BLOCK_CONTROLLER_H__

#include <e32def.h>
#include <e32cmn.h>

#include <test/datadictionary.h>
#include <test/testexecutestepbase.h>
#include <test/activecallback.h>
#include <test/tefresult.h>
#include <test/commandprocessor.h>
#include <test/blockitems.h>
#include <test/teftimer.h>

class CTestBlockController : public CTestStep, public MTEFResult, public MTEFTimerCallback, public MCommandProcessorCallback
/**
 * @publishedPartner
 * @test
 *
 * @see		CTestStep
 * @see		MTEFResult
 * @see		MTEFTimerCallback
 * @see		MCommandProcessorCallback
 */
	{
public:
	IMPORT_C 						CTestBlockController();
	IMPORT_C virtual				~CTestBlockController();
	IMPORT_C void					SetBlockArray( TTEFItemArray* aBlockArray );
	IMPORT_C void					SetSharedData( MSharedData* aSharedData );

	IMPORT_C virtual CDataWrapper*	CreateDataL( const TDesC& aData ) =0;

	// CTestStep implementations
	IMPORT_C virtual TVerdict		doTestStepPreambleL();
	IMPORT_C virtual TVerdict		doTestStepL();
	IMPORT_C virtual TVerdict		doTestStepPostambleL();

	// MTEFResult implementation
	IMPORT_C virtual void			SetError(const TInt aError);
	IMPORT_C virtual void			SetAsyncError(const TInt aIndex, const TInt aError);
	IMPORT_C virtual void			SetBlockResult(const TVerdict aResult);
	IMPORT_C virtual TVerdict		BlockResult();

	// MTEFTimerCallback implementation
	IMPORT_C virtual void			TimerCompleted();

	// MCommandProcessorCallback implementation
	IMPORT_C virtual void			NextCommandL();

protected:
	IMPORT_C void					StartCommands();
	IMPORT_C virtual TBool			DoCommandL(TTEFBlockItem& aCommand, const TInt aAsyncErrorIndex);

	inline	void					CreateObjectL(TTEFBlockItem& aCommand);
	inline	void					RestoreObjectL(TTEFBlockItem& aCommand);
	inline	void					StoreL(TTEFBlockItem& aCommand);
	inline	void					StoreActiveScheduler(TTEFBlockItem& aCommand);
	inline	void					CommandL(TTEFBlockItem& aCommand, const TInt aAsyncErrorIndex);

private:
	class CBlockActiveScheduler : public CActiveScheduler
		{
	public:
		IMPORT_C 				CBlockActiveScheduler(CTestBlockController& aTestStep);
		IMPORT_C virtual		~CBlockActiveScheduler();
		IMPORT_C virtual void	Error(TInt aError) const;

	private:
		CTestStep& iTestStep;
		};

	TTEFItemArray*				iBlockArray;
	CDataDictionary				iDataDictionary;
	CCommandProcessor*			iCommandProcessor;
	CActiveScheduler*			iActiveScheduler;
	CTEFTimer*					iTimer;
	CPeriodic*					iPeriodic;
	TBool						iAsyncTimer;
	TBool						iTimerActive;
	TInt						iCommandIndex;
	TInt						iAsyncCount;
	MSharedData*				iSharedData;
	TBool						iDeleteSharedScheduler;

	// Helper methods
	IMPORT_C void				StartTimer(const TTEFBlockItem& aCommand);
	IMPORT_C void				CancelTimer();

	// CPerodic callback
	IMPORT_C static TBool		OutstandingCallback( TAny* aAny );
	IMPORT_C void				OutstandingCallback();

	inline void					IncAsyncCount();
	inline void					DecAsyncCount();
	};

#include <test/testblockcontroller.inl>

#endif // __TEST_BLOCK_CONTROLLER_H__
