/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file  
 @test
*/


#if (!defined UPSQUERRY_STEP_H)
#define  UPSQUERRY_STEP_H 
#include <test/testexecutestepbase.h>
#include <bautils.h>
#include "tupsintegstepbase.h"
#include "tups_clientapi_sysserver.h"
#include "tupsproperty.h"

class TServiceRequestParameters
	{
	public:
  	TServiceRequestParameters();
	TInt 		iServiceUID;
	TBuf<30> 	iServerName;
	TBuf<60> 	iDestination;
	TInt  		iExpectedError; 
	TBuf8<50>	iOpaqueData;
 	TBuf<20>	iSelectDialogOption;
	TInt		iButtonsDisplayed;	
	TInt		iDialogCreatorInvoked;
	TInt		iPolicyEvaluatorInvoked;
	TBuf<30>	iAccessGranted;
	TBool		iCloseSession;
	TBool		iHoldEvaluatorOpen;
	TBool		iHoldPrepareDialogOpen;
	TBool		iHoldDisplayDialogOpen;
	TInt		iRequestDurationThreshold;
	TBool		iLeaveDialog;
	TBool		iLeaveEvaluator;
	TBool		iCancelUpsRequest;
	TBool		iPlatSecPass; 
	TBool		iForcePrompt;
	TInt		iExpectedEvaluatorInfo;
	TInt		iSelectFingerprint; 
	TBuf<60> 	iWaitUntilFileAppears; 
	};

class COpenSession
	{
	public:
	static 	COpenSession* NewL(const TPtrC& aServerName);
	void	ConstructL(const TPtrC& aServerName);
	~COpenSession();
	TBuf<30> iServerName;  
  	RUpsTestServ* iPointerToServer;
	RUpsTestSession* iPointerToSession;
	};


class CUpsClientStep : public CTUpsIntegStepBase
	{
public:
	CUpsClientStep();
	~CUpsClientStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

 
	TBool				FindOpenSession(const TDesC& aServerName,TInt &position);
	RUpsTestSession*	GetSessionToServerL(const TDesC& aServerName);
	void				CloseSessionL(const TDesC& aServerName);
	void 				CloseAllOpenSessions();
	void 				SetTestDataL(TInt aIndex);
	void 				SetClientStatusCompleteL();
	TUpsDecision 		StringToTUpsDecision(const TPtrC& aString);
	TPtrC		 		TUpsDecisionToString(TUpsDecision aDecision);
	TInt				ButtonToInt(const TPtrC& aButton);
	TBool 				CheckResultsL(TInt aIndex);
	TBool				CheckDialogCreatorResultsL(TInt aIndex);	
	TBool				CheckPolicyEvaluatorResultsL(TInt aIndex);
	void 				HoldClientL(void);
		
	// Array containing pointers to open sessions
	// maps server name to pointer to session 
	RArray<COpenSession*> iArraySessionsInUse;

public:
	// This array will hold all the services to be requested.
	RArray<TServiceRequestParameters>	iArraySersToRequest;   
	 
	TUpsTestServiceRequestData 	iRequestData;
	TUpsTestServiceReturn		iReturnData;
	
	CUpsProperty*				iPropertyReader;
	TInt						iStepRepeat;
	TInt						iServerId;
	TName 						iTEFServerName;
	TInt32 						iExpectedClientSid;
	TInt 						iHoldClientStepKey;
	
	};

_LIT(KUPSClientStep,"UPSClientStep");

#endif
