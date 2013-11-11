// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The implementation of CImplementationClassFourteen
// 
//

/**
 @file
 @internalComponent
*/

#include "examplefourteen.h"

/**
	Intended Usage	: Standardised safe construction which leaves nothing the cleanup stack.
	Error Condition	: Leaves with error code.
	@leave          	KErrNoMemory.
	@param			aInitParams The parameter struct used for initialising this object
	@return			CImplementationClassFourteen* The class instance.
	@pre 			None
	@post			CImplementationClassFourteen has been constructed,
					and initialised.
 */
CImplementationClassFourteen* CImplementationClassFourteen::NewL(TAny* aInitParams)
	{
	CImplementationClassFourteen* self=new(ELeave) CImplementationClassFourteen(); 
	CleanupStack::PushL(self);	
	self->ConstructL(aInitParams); 
	CleanupStack::Pop(self);
	return self;
	}

/**
	Intended Usage	: Destructor of CImplementationClassFourteen
	Error Condition	: None	
	@pre 			CImplementationClassFourteen has been constructed
	@post			CImplementationClassFourteen has been completely destroyed.
 */
CImplementationClassFourteen::~CImplementationClassFourteen()
	{
	delete iInternalDescriptor;
	Dll::FreeTls();
	}
/**
	Intended Usage	: Default Constructor : usable only by derived classes	
	Error Condition	: None	
	@pre 			None
	@post			CImplementationClassFourteen has been constructed
 */
CImplementationClassFourteen::CImplementationClassFourteen()
: CExampleInterface()
	{
	}

/**
	Intended Usage	: Completes the safe construction of the CImplementationClassFourteen object
	Error Condition	: Leaves with the error code.	
	@leave          	KErrNoMemory.	
	@param			aInitParams The parameter struct used for initialising this object
	@pre 			CImplementationClassFourteen has been constructed
	@post			CImplementationClassFourteen has been fully initialised.
 */
void CImplementationClassFourteen::ConstructL(TAny* aInitParams)
	{
	TExampleInterfaceInitParams* params = REINTERPRET_CAST(TExampleInterfaceInitParams*, aInitParams);
	if(params)
		{
		if(params->descriptor)
			{
			iInternalDescriptor = params->descriptor->AllocL();
			}
		}
	User::LeaveIfError(Dll::SetTls(&iTLSInt));
	}

/**
	Intended Usage	: Overload of the pure interface method
					Representative of a method provided on 
					the interface by the interface definer.
	@return			None
 */	
void CImplementationClassFourteen::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	ASSERT(Dll::Tls()!=NULL);
	}

/**
	Intended Usage	: Overload of the pure interface method
					asynchronous function which 
					an interface definer could specify.  
	@return			TInt KErrNone for success.
 */
TInt CImplementationClassFourteen::FireAndForget()
	{
	return KErrNone;			
	}

// Provide the CActive overloads
void CImplementationClassFourteen::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CImplementationClassFourteen::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassFourteen::RunError(TInt aError)
	{
	return aError;
	}
/**
	Intended Usage	: To verify the object returned by ECOM.
	@return			TUid (ECOM's Implementation Uid for this class.)
 */
TUid CImplementationClassFourteen::ImplId()
	{
	return KImplUid;
	}


