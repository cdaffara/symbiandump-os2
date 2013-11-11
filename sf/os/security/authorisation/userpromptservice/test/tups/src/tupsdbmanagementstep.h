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
*
*/


/**
 @file  
 @test
*/
 
#if (!defined __UPSDB_STEP_H__)
#define __UPSDB_STEP_H__
#include <test/testexecutestepbase.h>
#include "tupsintegstepbase.h"
#include "tups_clientapi_sysserver.h"
#include "tupsproperty.h"
#include <ups/upsclient.h>
using namespace UserPromptService;

/**
Class for storing a single instance of a UPS Database Request
*/
class CUpsDbRequest
	{
public:
	static CUpsDbRequest* NewL();
	~CUpsDbRequest();
	
	TBuf<30>	iOperation;
	TInt		iClientSid;
	TInt 		iEvaluatorId;
	TInt		iServiceId;
	TInt		iServerSid;
	TBuf8<32>	iFingerprint;
	TBuf8<50>	iClientEntity;
	TBuf<60>	iDescription;	
	TBuf<3>		iDecisionResult;
	TInt		iMajorPolicyVersion;
	TInt		iRecordId;
	TInt		iEvaluatorInfo;
	TInt		iExpectedDecisionCount;
	
	/**
	 * Pointer to the Decision Filter object
	 */
	CDecisionFilter* iDecisionFilter;	
	
protected:
	CUpsDbRequest();
	void ConstructL();
	};


class CUPSDbManagementStep : public CTUpsIntegStepBase
	{
public:
	CUPSDbManagementStep();
	~CUPSDbManagementStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	TBool CheckDecisionRecordL(TInt aIndex, const CDecisionRecord& aRecord);
	HBufC8* StringToHexLC(const TDes8 &aString);
	HBufC8* StringToBinaryLC(const TDes8 &aString);

protected:
	// This array will hold all the request data and corresponding Decision Filter
	RPointerArray<CUpsDbRequest>	iArraySersToRequest; 
	RUpsManagement 					iUpsManager;
	TInt							iStepRepeat;
	TName 							iTEFServerName;
	TInt32 							iExpectedClientSid;
	};
	
_LIT(KUPSDbManagementStep,"UPSDbManagementStep");

#endif

