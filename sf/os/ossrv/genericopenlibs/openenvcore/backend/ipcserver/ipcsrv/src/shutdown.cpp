// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : shutdown.cpp
// Part of     : ipc server
// To implement the timer for transient server
// This class provides the API Implementation for the CShutDown.
// The stdio server will use this API to implement its self as transiet
// 
//
	
 

#include "shutdown.h"


// Delay Approx 2 secs Delay 
static const TInt KShutdownDelay = 200000;


//-------------------------------------------------------------------------------
// Function Name : CShutDown::NewL()
// Description   : Factory function to create an instance of CShutDown.		 
//-------------------------------------------------------------------------------	

CShutDown* CShutDown::NewL()
	{
	CShutDown* self = new (ELeave) CShutDown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


//-------------------------------------------------------------------------------
// Function Name : 	CShutDown::CShutDown()
// Description   : 	Constructor. Contructs CTimer with -1 as the priority of the 
//					Active Object. Adds itsself to the Active Scheduler
//					 				 
//-------------------------------------------------------------------------------	
	
 

CShutDown::CShutDown() : CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CShutDown::ConstructL()
// Description   : 	This API Constructs a New Asyncronous Timer. This API
//					needs to called before any use of Timer Request can be made				 				 
//-------------------------------------------------------------------------------	


void CShutDown::ConstructL()
	{
	CTimer::ConstructL();
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CShutDown::Start()
// Description   : 	This API Requests for a Event after the specified time
//					interval. The RunL method will be called as soon as the Timer
//					expires.Calling of the RunL may get delayed because of any other
//					higher proprity running Active Objects. In our case its ok as 
//					it really does not matter if the Server happens to run a little
// 					more than specified time.			 				 
//-------------------------------------------------------------------------------	

 	
void CShutDown::Start()
	{
	After(KShutdownDelay);
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CShutDown::RunL()
// Description   : 	This API is called by Active Scheduler once the time expires.
//					This will result in Stopping the Active Scheduler and intiates
//					the server exit	
//-------------------------------------------------------------------------------	
	
void CShutDown::RunL()
	{
	CActiveScheduler :: Stop();
	}
