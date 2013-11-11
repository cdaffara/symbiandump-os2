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

#ifndef TUPSINTERACTIVE
#define TUPSINTERACTIVE 
 
#include <e32base.h>
#include "tups_clientapi_sysserver.h"

#include <ups/upstypes.h>

class CUPSTestSession;

class CTUPSTesterActive : public CActive
	{
	public:
		CTUPSTesterActive(const RMessage2& aMessage);
		~CTUPSTesterActive();

		// From CActive
		void DoCancel()	{return; };
		void RunL();
		virtual TInt RunError(TInt aError);
		void PackageReturnDataL(TInt aError);
		
		// Customized.
		TInt doActiveUpsRequest(CUPSTestSession* aTestServerSession);
        
        // state machine enumeration
		enum EState
		{
			EIdle,
			ERequestAuthorization,
			ESomething
		};  
		
	private:
	
		RMessage2 					iMessage;
		TUint32 					iStartTime;
		TUint32 					iEndTime;
		EState						iState;
		TUpsTestServiceReturn 		iReturnData;
		
		// data 
		CUPSTestSession* 			iServerSessionPointer;
		TUpsDecision  				iDecision;
		TPtrC 						iSerDest;
		TPtrC8 						iOpaqueData;

	} ;
	
#endif
