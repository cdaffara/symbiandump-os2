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
// The implementation of a some classes
// to be provided by ECom.
// 1. Using the CExampleInterface class as a base.
// 
//

#include "Interface.h"
#include "ImplementationProxy.h"
#include "TestUtilities.h"	// For __FILE__LINE__

// ____________________________________________________________________________
// 
/**
	Intended usage: This class implements the functionality promised by 
	the CExampleInterface defintion class. It does little apart from provides a test instance
	which may be retrieved and run for testing purposes.
	Its resolution is based upon its registered default data string that
	advertises this class as being able to handle 'text\wml' data.
	@since 7.0

 */
class CImplementationClassSeven : public CExampleInterface
{
// Methods
public:
/**
	@fn				NewL(TAny* aInitParams)
	Intended Usage	: Standardised safe construction which leaves nothing the cleanup stack.
	Error Condition	: Leaves with error code.
	@leave          KErrNoMemory.
	@since			7.0
	@param			aInitParams The parameter struct used for initialising this object
	@return			CImplementationClassSeven* The class instance.
	@pre 			None
	@post			CImplementationClassSeven has been constructed,
					and initialised.
 */
	static CImplementationClassSeven* NewL(TAny* aInitParams);

/**
	@fn				~CImplementationClassSeven()
	Intended Usage	: Default Destructor	
	Error Condition	: None	
	@since			7.0
	@pre 			CImplementationClassSeven has been constructed
	@post			CImplementationClassSeven has been completely destroyed.
 */
	virtual ~CImplementationClassSeven();

/**
	@fn				DoMethodL()
	Intended Usage	: Overload of the pure interface method
					Representative of a method provided on 
					the interface by the interface definer.
	Error Condition	: Leaves with an error code.
	@leave  		KErrNoMemory, KErrNotSupported.
	@since			7.0
	@return			None
	@pre 			CImplementationClassSeven has been constructed
	@post			Unspecified
 */	
	void DoMethodL();

/**
	@fn				FireAndForget()
	Intended Usage	: Overload of the pure interface method
					asynchronous function which 
					an interface definer could specify.  
					It allows the client to call the function in the knowledge 
					that the object will commit suicide when the 
					function completes.
	Error Condition	: None.
	@since			7.0
	@return			TInt KErrNone for success.
	@pre 			CImplementationClassSeven has been constructed
	@post			Unspecified
 */
	TInt FireAndForget();

 /**
	@fn				ImplId()
	Intended Usage	: To verify the object returned by ECOM.
	Error Condition	: None.
	@since			7.0
	@return			TUid (ECOM's Implementation Uid for this class.)
	@pre 			CImplementationClassThree has been constructed
	@post			Unspecified
 */
	TUid ImplId();

private:
/**
	@fn				CImplementationClassSeven()
	Intended Usage	: Default Constructor : usable only by derived classes	
	Error Condition	: None	
	@since			7.0
	@pre 			None
	@post			CImplementationClassSeven has been constructed
 */
	CImplementationClassSeven();

/**
	@fn				ConstructL(TAny* aInitParams)
	Intended Usage	: Completes the safe construction of the CImplementationClassSeven object
	Error Condition	: Leaves with the error code.	
	@leave          KErrNoMemory.	
	@since			7.0
	@param			aInitParams The parameter struct used for initialising this object
	@pre 			CImplementationClassSeven has been constructed
	@post			CImplementationClassSeven has been fully initialised.
 */
	void ConstructL(TAny* aInitParams);

// Provide the CActive overloads
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
/** A place for allocating some memory in the ConstructL */
	HBufC*	iInternalDescriptor;
/** An int to be stored in TLS to test its useage */	
	TInt	iTLSInt;

};  // End of CImplementationClassSeven definition

// __________________________________________________________________________
// Implementation

CImplementationClassSeven* CImplementationClassSeven::NewL(TAny* aInitParams)
// Intended Usage	: Safe construction which leaves nothing upon the cleanup stack	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @param			" "
// @return			CImplementationClassSeven* a pointer to the fully instantiated CImplementationClassSeven object
// @pre 			None
// @post			The object has been fully instantiated
// Static member
	{
	CImplementationClassSeven* self=new(ELeave) CImplementationClassSeven();  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(aInitParams); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

CImplementationClassSeven::~CImplementationClassSeven()
// Default virtual d'tor
	{
	delete iInternalDescriptor;
	}

CImplementationClassSeven::CImplementationClassSeven()
// Default c'tor for use by derived and 
// static construction methods only
: CExampleInterface()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

void CImplementationClassSeven::ConstructL(TAny*)
// Intended Usage	: Safely complete the initialization of the constructed object	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @return			void
// @pre 			CImplementationClassSeven has been constructed
// @post			The CImplementationClassSeven object has been fully instantiated
//
	{
	}

void CImplementationClassSeven::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	REINTERPRET_CAST(TInt*, Dll::Tls());
	}

TInt CImplementationClassSeven::FireAndForget()
	{
	TRAPD(error,DoMethodL());
	return error;			// Always KErrNotSupported
	}

// Provide the CActive overloads
void CImplementationClassSeven::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CImplementationClassSeven::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassSeven::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

TUid CImplementationClassSeven::ImplId()
	{
	TUid idVal = {0x10009DBC};
	return (idVal);
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10009DBC,	CImplementationClassSeven::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

