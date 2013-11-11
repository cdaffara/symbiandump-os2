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



#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#if (!defined __TEST_ECOM_INTERFACE_H__)
#define __TEST_ECOM_INTERFACE_H__

#include <e32base.h>
#include <ecom/ecom.h>

#define CTestEComInterface_UID	0x2000371E


class CTestEComInterface  : public CBase
	{
/**
	@internalComponent
	CTestEComInterface which is an example abstract class being representative 
	of the concrete class which the client wishes to use.
	It acts as a base, for a real class to provide all the 
	functionality that a client requires.  
	It supplies instantiation & destruction by using
	the ECom framework, and functional services
	by using the methods of the actual class.
 */
public:
	//The function which instantiates an object of this type 
	//using aUid as a resolution parameter.
	//(eg NewL for a CBase derived object).
	static CTestEComInterface*	NewL(const TUid aUid);

	//Standardised destructor.
	virtual ~CTestEComInterface();

	//Pure interface method
	//Representative of a method provided on the interface by 
	//the interface definer.
	virtual void	DoMethod1L() = 0;

	//Pure interface method
	//Representative of a method provided on the interface by 
	//the interface definer.
	virtual TInt	DoMethod2L() = 0;

protected:
	//Default constructor
	inline CTestEComInterface();

	//Attributes
public:
	TUid	iDtor_ID_Key;
	};

#include "TestEComInterface.inl"

#endif /* __TEST_ECOM_INTERFACE_H__ */
