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



/** 
	@internalComponent
	Comments : This file contains the placeholder class
				which represents the interface creation.
 */ 

#include "TestEComInterface.h"

inline CTestEComInterface::CTestEComInterface()
:	CBase()
/**
Constructor
@return None  
*/
	{
	// Do nothing
	}

inline CTestEComInterface::~CTestEComInterface()
/**
Destructor
@return None  
*/
	{
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline CTestEComInterface* CTestEComInterface::NewL(const TUid aUid)
/**
NewL creates the new interface
@return the new interface 
*/
	{
	// The CreateImplementationL method will return
	// the created item.
	return REINTERPRET_CAST	(CTestEComInterface*,
							REComSession::CreateImplementationL(aUid, _FOFF(CTestEComInterface,iDtor_ID_Key))
							);
	}
