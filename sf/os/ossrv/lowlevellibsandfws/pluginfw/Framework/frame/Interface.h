// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the Interface Definition for the
// CExampleInterface class.
// 
//

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_INTERFACE_3A1D07A10155_INCLUDED
#define _INC_INTERFACE_3A1D07A10155_INCLUDED

#include <e32base.h>

#include <ecom/ecom.h>

/**
	@internalComponent
	@since 7.0
	An example abstract class being representative of the
	concrete class which the client wishes to use.
	It acts as a base, for a real class to provide all the 
	functionality that a client requires.  
	It supplies instantiation & destruction by using
	the ECom framework, and functional services
	by using the methods of the actual class.
 */

class CExampleInterface  : public CActive
{
public:
	// The interface for passing initialisation parameters
	// to the derived class constructor.
	typedef struct _exampleInterfaceInitParams
		{
		TInt integer;
		const TDesC* descriptor;
		} TExampleInterfaceInitParams;

	//The function which instantiates an object of this type 
	//(eg NewL for a CBase derived object).
	
	static CExampleInterface* NewL();

	//The function which instantiates an object of this type 
	// using the MatchString as the resolver parameters.
	//(eg NewL for a CBase derived object).
	static CExampleInterface* NewL(const TDesC8& aMatchString);

	//The function which instantiates an object of this type with the required parameters 
	//(eg NewL for a CBase derived object).
	
	static CExampleInterface* NewL(TInt aIntParameterTest, const TDesC& aDesParameterTest);
	
	//The function which instantiates an object of this type 
	//using ImplementationUid as a resolution parameter.
	//(eg NewL for a CBase derived object).
	static CExampleInterface* NewL2(const TUid aImplementationUid);
	
	//The function which instantiates an object of this type 
	//(eg NewL for a CBase derived object).
	static CExampleInterface* NewL3();
	
	//The function which instantiates an object of this type 
	//(eg NewL for a CBase derived object).
	static CExampleInterface* NewL4();

	//The functions which instantiate object of this type with extended interface
	static CExampleInterface* NewL5();
	static CExampleInterface* NewL6();
	static CExampleInterface* NewL7();
	static CExampleInterface* NewL8();

	//Standardised destructor.
	virtual ~CExampleInterface();

	//Request a list of all available implementations which 
	//satisfy this given interface.
	
	static void ListImplementationsL(RImplInfoPtrArray& aImplInfoArray);
	static void ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray);


	//Pure interface method
	//Representative of a method provided on the interface by 
	//the interface definer.
	
	virtual void DoMethodL() = 0;

	//Pure interface method
	//Representative of an asynchronous function which 
	//an interface definer could specify.  
	//It allows the client to call the function in the knowledge 
	//that the object will commit suicide when the 
	//function completes.
	
	virtual TInt FireAndForget() = 0;

	//Pure interface method,
	//Used to cross check the implementation returned by ECOM.
	virtual TUid ImplId() = 0;
public:
	// Wrapper methods which get and release the extended interface using 
	// the member iDtor_ID_Key
	TAny* GetExtendedInterfaceL(const TUid& aInterfaceUid);
	void ManuallyReleaseExtendedInterfaceL(const TUid& aInterfaceUid);

protected:
	//Default c'tor
	inline CExampleInterface();

//Attributes
private:
	
	TUid iDtor_ID_Key;

	// Declare the Test bed state accessor as a friend
	friend class TExampleInterface_StateAccessor;

};

// Example extended interface classes
class MExampleInterfaceExtended
{
public:
	//Pure interface method
	//Representative of a method provided on the interface by 
	//the interface definer.
	virtual void DoMethodExtended() = 0;
};

class MExampleInterfaceExtended2
{
public:
	
	//Pure interface method
	//Representative of a method provided on the interface by 
	//the interface definer.
	virtual void DoMethodExtended2() = 0;
};
#include "Interface.inl"

#endif /* _INC_INTERFACE_3A1D07A10155_INCLUDED */

