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

#include <e32property.h> 

#include "simpancommand.h"
#include "ncmtestconsole.h"

static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
static _LIT_SECURITY_POLICY_C1(KNetworkControlPolicy, ECapabilityNetworkControl); 

_LIT(KSimPANCommandActive, "Simulate PAN connection is active");
_LIT(KSimPANCommandNone, "Simulate PAN connection is not active");

CSimPANCommand* CSimPANCommand::NewL(CUsbNcmConsole& aTestConsole, TUint aKey)
	{
	LOG_STATIC_FUNC_ENTRY
	
	CSimPANCommand* self = new(ELeave) CSimPANCommand(aTestConsole, aKey);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CSimPANCommand::~CSimPANCommand()
	{
	Cancel();
	iNcmExclusiveProp.Close();
	}

CSimPANCommand::CSimPANCommand(CUsbNcmConsole& aTestConsole, TUint aKey)
	: CNcmCommandBase(EPriorityStandard, aTestConsole, aKey)
	{
	}

void CSimPANCommand::ConstructL()
	{
	//Attach the property of Exclusive state
	User::LeaveIfError(iNcmExclusiveProp.Attach(KC32ExeSid, KIPBearerCoexistenceProperty));
	CActiveScheduler::Add(this);
	iNcmExclusiveProp.Subscribe(iStatus);
	SetActive();
	
	//Get the current value of exclusive state
	TInt state; 
	TInt err = iNcmExclusiveProp.Get(state);
	switch(err)
		{
		case KErrNotFound:
			//The property is not defined. means BtPAN and Ncm didn't start.
			DisplayExclusive(ENoneIsActive);
			break;
		case KErrNone:
			//Display the exclusive state on main console
			DisplayExclusive(state);
			break;
		default:
			User::LeaveIfError(err);
			break;
		}
 
	}

void CSimPANCommand::RunL()
/**
Get the current exclusive state and update on main console when the state changed
*/	
	{
	LOG_FUNC
	__FLOG_STATIC1(KSubSys, KLogComponent , _L8("CNcmExclusiveStateAO::RunL - iStatus = %d"), iStatus.Int());

	User::LeaveIfError(iStatus.Int());

	iNcmExclusiveProp.Subscribe(iStatus);
	SetActive();
	
	TInt exclusiveState; 
	TInt err = iNcmExclusiveProp.Get(exclusiveState);

	__FLOG_STATIC2(KSubSys, KLogComponent , _L8("Get[IPBearerCoexistence]=%d, err=%d"), exclusiveState, err);

	if(err == KErrNone)
		{
		//Display the exclusive state on main console
		DisplayExclusive(exclusiveState);
		}	
	}

void CSimPANCommand::DoCancel()
	{
	iNcmExclusiveProp.Cancel();
	}

TInt CSimPANCommand::RunError(TInt aError)
	{
	User::Panic(_L("CSimPANCommand"), aError);
	return aError;
	}

void CSimPANCommand::DoCommandL()
/**
Main function of simualate Pan active/inactive. 
It will try to get the exclusive state from P&S key. And set the value based on the iSetPAN.
There's three value for this P&S key: EExclusiveNone,ENcmOwned,EBtPanOwned

If now the command is 'active PAN'(iSetPAN=ETrue), then set the value of property to Pan active.
If now the command is 'inactive PAN'(iSetPAN=EFalse), then set the value of property to Pan inactive.
*/	
	{
	LOG_FUNC
		
	//Get the mutex which used to protect the P&S key.
	RMutex mtx;
	TInt ret;
	ret = mtx.CreateGlobal(KIPBearerCoexistenceMutex);
	if (ret == KErrAlreadyExists)
		{
		ret  = mtx.OpenGlobal(KIPBearerCoexistenceMutex);
		}	
	User::LeaveIfError(ret);
	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Get Mutex, wait()"));
	CleanupClosePushL(mtx);
 	mtx.Wait();
 	
 	RProperty prop;
 	TInt state;
 	TBool bSet = ETrue;
 	
 	//Get the exclusive state.
 	ret = prop.Get(KC32ExeSid, KIPBearerCoexistenceProperty, state);
	CleanupClosePushL(prop);
 	__FLOG_STATIC2(KSubSys, KLogComponent , _L8("Get Property[IPBearerCoexistence]: ret=%d, val=%d"), ret, state);
 	
 	switch(ret)
 		{
 		case KErrNotFound:
 			{
	 		//The property not exists. (NCM not start.)
	
		 	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Not define property yet. define it"));		
			CUsbNcmConsoleEvent* event = CUsbNcmConsoleEvent::NewL();
			event->iEvent.AppendFormat(_L("Not define property yet. define it."));
			iTestConsole.NotifyEvent(event);
	
			//Define the property
			User::LeaveIfError(RProperty::Define(KC32ExeSid, KIPBearerCoexistenceProperty, RProperty::EInt, KAllowAllPolicy, KNetworkControlPolicy));
		 	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Define Property[IPBearerCoexistence] ok"));
		 	break;
 			}
 		case KErrNone:
 			{
	 		//property exists.
	 		if(state == ENcmIsActive)
	 			{
	 			//Ncm hold the key. PAN can't active. Do nothing in this command.
	 			CUsbNcmConsoleEvent* event = CUsbNcmConsoleEvent::NewL();
	 			event->iEvent.AppendFormat(_L("NCM hold the P&S key!"));
	 			iTestConsole.NotifyEvent(event); 	
	 			bSet = EFalse;
	 			}
	 		break;
 			}
 		default:
 			{
 			//error when get P&S key.
 			CUsbNcmConsoleEvent* event = CUsbNcmConsoleEvent::NewL();
 			event->iEvent.AppendFormat(_L("Get P&S key error! err = %d"), ret);
 			iTestConsole.NotifyEvent(event);
 			bSet = EFalse;
 			break;
 			}
 		}
 	
 	if(bSet)
 		{
 		//Set the property value
		CUsbNcmConsoleEvent* event = CUsbNcmConsoleEvent::NewL();
		CleanupStack::PushL(event);
 		if(this->iSetPAN)
 			{
 			User::LeaveIfError(prop.Set(KC32ExeSid, KIPBearerCoexistenceProperty, EBTPanIsActive));
 	 		__FLOG_STATIC1(KSubSys, KLogComponent , _L8("Set Property[IPBearerCoexistence]: val = %d"), EBTPanIsActive);		
 			event->iEvent.AppendFormat(_L("Set PAN active! prop=%d"), EBTPanIsActive);			
 			}
 		else
 			{
 			User::LeaveIfError(prop.Set(KC32ExeSid, KIPBearerCoexistenceProperty, ENoneIsActive));
 	 		__FLOG_STATIC1(KSubSys, KLogComponent , _L8("Set Property[IPBearerCoexistence]: val = %d"), ENoneIsActive);		
			event->iEvent.AppendFormat(_L("Set PAN disactive! prop=%d"), ENoneIsActive);
			}
 		CleanupStack::Pop(event);
		iTestConsole.NotifyEvent(event);
					
 		}
 	
 	CleanupStack::Pop(2, &mtx); //prop, mtx
 	mtx.Signal();
 	prop.Close();
 	mtx.Close();
 	
	}

void CSimPANCommand::ChangeDescription()
	{
	if(iSetPAN)
		{
		SetDescription(KSimPANCommandActive());
		}
	else
		{
		SetDescription(KSimPANCommandNone());
		}
	}

void CSimPANCommand::DisplayExclusive(TInt aState)
/**
Display the exclusive state on main console and change the description of command by the
value of aState
  @param  aState 	Exclusive state
*/
	{
	TBuf<DISPLAY_NCM_BTPAN_LEN>	exclusive;
	iSetPAN = ETrue;
	switch(aState)
		{
		case ENoneIsActive:
			{
			exclusive = _L("None    ");
			}
			break;
		case ENcmIsActive:
			{
			exclusive = _L("NCM   ");
			}
			break;
		case EBTPanIsActive:
			{
			exclusive = _L("BT PAN  ");
			//If Bt Pan is active, the command description should be 'disactive BT PAN'
			iSetPAN = EFalse;
			}
			break;
		default:
			exclusive = _L("        ");
			break;
		}
	
	ChangeDescription();
	iTestConsole.SetDisplayItem(ENcmBtPanItem, exclusive);
	}
