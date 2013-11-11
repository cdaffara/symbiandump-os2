// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#ifndef __CECOMSWIPLUGININTERFACEDEFINITION__
#define __CECOMSWIPLUGININTERFACEDEFINITION__

#include <e32base.h>


class CEComSwiPluginInterfaceDefiniton  : public CBase
{
public:
	// The function which instantiate an object of this type 
	inline static CEComSwiPluginInterfaceDefiniton* NewL(TUid aImplementationUid);

	// Standardised destructor.
	inline virtual ~CEComSwiPluginInterfaceDefiniton();

	// The sole pure interface method
	// Representative of a method provided on the interface by 
	// the interface definer.
	virtual void ReturnInt(TInt& aInt) = 0;

private:
	// Required attribute for the framework
	// (An identifier used during destruction)
	TUid iDtor_Id_Key;
};

// Provide the inline implementations
#include <ecom/ecom.h>		// For REComSession

inline  CEComSwiPluginInterfaceDefiniton* CEComSwiPluginInterfaceDefiniton::NewL(TUid aImplementationUid)
{
	
	TAny* ptr = REComSession::CreateImplementationL(aImplementationUid, 
								_FOFF(CEComSwiPluginInterfaceDefiniton, iDtor_Id_Key));
				
	return REINTERPRET_CAST(CEComSwiPluginInterfaceDefiniton*, ptr);
}

inline CEComSwiPluginInterfaceDefiniton::~CEComSwiPluginInterfaceDefiniton()
{
	REComSession::DestroyedImplementation(iDtor_Id_Key);
	              
}

#endif 	//  __CECOMSWIPLUGININTERFACEDEFINITION__
