/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* tupsdbmanagementstep.h
*
*/


/**
 @file  
 @test
*/
 
#ifndef UPSCOORDINATOR_STEP_H
#define UPSCOORDINATOR_STEP_H

#include <test/testexecutestepbase.h>
#include "tupsintegstepbase.h"
#include "tupsproperty.h"
#include <e32property.h>

static const TInt KClientA = 0;	
static const TInt KClientB = 1;
static const TInt KClientC = 2;
static const TInt KPolicyEvaluator = 0;
static const TInt KDialogCreator = 1;

enum TClientState
	{
	EClientState_NULL							=0,
	EClientState_Starting 						=1,
	EClientState_PolicyEvaluatorStarted 		=2,
	EClientState_PolicyEvaluatorHolding			=3,
	EClientState_PolicyEvaluatorCompleted 		=4,
	EClientState_DialogCreatorStarted 			=5,
	EClientState_DialogCreatorPrepareHolding	=6,
	EClientState_DialogCreatorDisplayHolding	=7,
	EClientState_DialogCreatorCompleted 		=8,
	EClientState_Finished						=9
	};

class TCoordinationOperation
	{
	public:
  	TCoordinationOperation();
	TBuf<30> 		iOperation;					//Operation to perform
	TInt			iClientInstanceToOperate;	//Which Client to operate on
	TClientState 	iClientStateA;				//State of Client A expected
	TClientState  	iClientStateB;				//State of Client B expected
	TClientState	iClientStateC;				//State of Client C expected
	};

class CUPSCoordinatorStep : public CTUpsIntegStepBase
	{
public:
	CUPSCoordinatorStep();
	virtual ~CUPSCoordinatorStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();

	TInt WaitForStateL(TInt aOperation);
	TInt GetAndProcessClientStatusL(TInt aClient);
	TClientState PtrCToClientState(const TPtrC& aString);
	TPtrC ClientStateToPtrC(TClientState aState);

protected:
	// This array will hold all the services to be requested.
	RArray<TCoordinationOperation>	iArrayCoordinationOps;

	TClientState iClientState[3];
	RProperty iCheckClientProperty[3][2];

	CUpsProperty* iPropertyReader;
	TInt 	iClientInstance[3];
	TInt 	iClientHoldKey[3];
	
	TInt 	iClientCount;
	
	TName 	iTEFServerName;
	TInt32 	iExpectedClientSid;
	};

_LIT(KUPSCoordinatorStep,"UPSCoordinatorStep");

#endif
