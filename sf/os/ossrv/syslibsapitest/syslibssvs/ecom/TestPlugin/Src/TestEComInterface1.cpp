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
*
*/



#include "TestEComInterface1.h"

CTestEComInterface1* CTestEComInterface1::NewL()
// Intended Usage	: Safe construction which leaves nothing upon the cleanup stack	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @param			" "
// @return			CTestEComInterface1* a pointer to the fully instantiated CTestEComInterface1 object
// @pre 			None
// @post			The object has been fully instantiated
// Static member
	{
	CTestEComInterface1* self=new(ELeave) CTestEComInterface1();  // calls constructor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

CTestEComInterface1::~CTestEComInterface1()
// Default virtual d'tor
	{
	}

CTestEComInterface1::CTestEComInterface1()
:	CTestEComInterface()
,	iDoMethodL1Calls(0)
	{
	}

void CTestEComInterface1::ConstructL()
// Intended Usage	: Safely complete the initialization of the constructed object	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @return			void
// @pre 			CTestEComInterface1 has been constructed
// @post			The CTestEComInterface1 object has been fully instantiated
//
	{
	}

void CTestEComInterface1::DoMethod1L()
	{
	++iDoMethodL1Calls;
	}

TInt CTestEComInterface1::DoMethod2L()
	{
	return iDoMethodL1Calls;
	}
