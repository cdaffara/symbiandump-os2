/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
 @internalComponent
 @test
 */

#ifndef SIMPANCOMMAND_H
#define SIMPANCOMMAND_H

#include "commandengine.h"
#include <e32property.h>

// This is a local definition of this shared enum of P&S key values.  These values should not be changed without changing
// the equivalent definition in the other IP bearer code.
enum TIPBearerCoexistenceStatus
	{
	ENoneIsActive, 
	ENcmIsActive,
	EBTPanIsActive
	};

// This is a local definition of this shared P&S id.  This value should not be changed without changing
// the equivalent definition in the other IP bearer code.
const TInt KIPBearerCoexistenceProperty = 0x10286a95;

// This is the identifer of the shared mutex used to allow safe read / write of the coexistence P&S key.
// This can only be changed if all other existing definitions of the mutex name are also changed.
_LIT(KIPBearerCoexistenceMutex, "IPBearerCoexistenceMutex");

//static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
//static _LIT_SECURITY_POLICY_C1(KNetworkControlPolicy, ECapabilityNetworkControl); 


NONSHARABLE_CLASS(CSimPANCommand) : public CNcmCommandBase
/**
Simulate PAN has an active connection or not.
It is also response for display the exclusive state on main console
*/
	{
public:
	static CSimPANCommand* NewL(CUsbNcmConsole& aUsb, TUint aKey);
	~CSimPANCommand();

public:
	//From CNcmCommandBase
	void DoCommandL();

private:
	CSimPANCommand(CUsbNcmConsole& aUsb, TUint aKey);
	void ConstructL();

private:
	//From CActive
    void RunL();
    void DoCancel();
	TInt RunError(TInt aError);

private:
    //Display the exclusive state on main screen
    void DisplayExclusive(TInt aState);
	//When the PAN is active, the command description should be 
	//changed to 'Simulate PAN disactive'
	void ChangeDescription();

private:
	//Indicate the PAN is active or not
	TBool iSetPAN;
	//The property which store the exclusive state
	RProperty iNcmExclusiveProp;
	};

#endif // SIMPANCOMMAND_H
