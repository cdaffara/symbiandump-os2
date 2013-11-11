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



#include "TestEComInterface2.h"

CTestEComInterface2* CTestEComInterface2::NewL()
// Intended Usage	: Safe construction which leaves nothing upon the cleanup stack	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @param			" "
// @return			CTestEComInterface2* a pointer to the fully instantiated CTestEComInterface2 object
// @pre 			None
// @post			The object has been fully instantiated
// Static member
	{
	CTestEComInterface2* self=new(ELeave) CTestEComInterface2();  // calls constructor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

CTestEComInterface2::~CTestEComInterface2()
// Default virtual d'tor
	{
	}

CTestEComInterface2::CTestEComInterface2()
:	CTestEComInterface()
,	iDoMethodL1Calls(0)
	{
	}

void CTestEComInterface2::ConstructL()
// Intended Usage	: Safely complete the initialization of the constructed object	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @return			void
// @pre 			CTestEComInterface2 has been constructed
// @post			The CTestEComInterface2 object has been fully instantiated
//
	{
	}

void CTestEComInterface2::DoMethod1L()
	{
	++iDoMethodL1Calls;
	}

TInt CTestEComInterface2::DoMethod2L()
	{
	return iDoMethodL1Calls;
	}
