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
// The implementation of a some classes
// to be provided by ECom.
// 1. Using the CExampleInterface class as a base.
// 
//

#include "Interface.h"
#include "ImplementationProxy.h"
#include "ExampleResolver.h"
#include "TestUtilities.h"	// For __FILE__LINE__

// ____________________________________________________________________________
// 
/**	Intended usage: This class implements the functionality promised by 
	the CExampleInterface defintion class. It does little apart from provides a test instance
	which may be retrieved and run for testing purposes.
	Its resolution is based upon its registered default data string that
	advertises this class as being able to handle 'text\wml' data.
	@since 7.0
 */
class CImplementationClassOne : public CExampleInterface
{
// Methods
public:
/**
	@fn				NewL(TAny* aInitParams)
	Intended Usage	: Standardised safe construction which leaves nothing the cleanup stack.
	Error Condition	: Leaves with error code.
	@leave          	KErrNoMemory.
	@since			7.0
	@param			aInitParams The parameter struct used for initialising this object
	@return			CImplementationClassOne* The class instance.
	@pre 			None
	@post			CImplementationClassOne has been constructed,
					and initialised.
 */
	static CImplementationClassOne* NewL(TAny* aInitParams);

/**
	@fn				~CImplementationClassOne()
	Intended Usage	: Default Destructor	
	Error Condition	: None	
	@since			7.0
	@pre 			CImplementationClassOne has been constructed
	@post			CImplementationClassOne has been completely destroyed.
 */
	virtual ~CImplementationClassOne();

/**
	@fn				DoMethodL()
	Intended Usage	: Overload of the pure interface method
					Representative of a method provided on 
					the interface by the interface definer.
	Error Condition	: Leaves with an error code.
	@leave  		KErrNoMemory, KErrNotSupported.
	@since			7.0
	@return			None
	@pre 			CImplementationClassOne has been constructed
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
	@pre 			CImplementationClassOne has been constructed
	@post			Unspecified
 */
	TInt FireAndForget();

 /**
	@fn				ImplId()
	Intended Usage	: To verify the object returned by ECOM.
	Error Condition	: None.
	@since			7.0s
	@return			TUid (ECOM's Implementation Uid for this class.)
	@pre 			CImplementationClassThree has been constructed
	@post			Unspecified
 */
	TUid ImplId();

private:
/**
	@fn				CImplementationClassOne()
	Intended Usage	: Default Constructor : usable only by derived classes	
	Error Condition	: None	
	@since			7.0
	@pre 			None
	@post			CImplementationClassOne has been constructed
 */
	CImplementationClassOne();

/**
	@fn				ConstructL(TAny* aInitParams)
	Intended Usage	: Completes the safe construction of the CImplementationClassOne object
	Error Condition	: Leaves with the error code.	
	@leave          	KErrNoMemory.	
	@since			7.0
	@param			aInitParams The parameter struct used for initialising this object
	@pre 			CImplementationClassOne has been constructed
	@post			CImplementationClassOne has been fully initialised.
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

};  // End of CImplementationClassOne definition

// __________________________________________________________________________
// Implementation

CImplementationClassOne* CImplementationClassOne::NewL(TAny* aInitParams)
// Intended Usage	: Safe construction which leaves nothing upon the cleanup stack	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @param			" "
// @return			CImplementationClassOne* a pointer to the fully instantiated CImplementationClassOne object
// @pre 			None
// @post			The object has been fully instantiated
// Static member
	{
	CImplementationClassOne* self=new(ELeave) CImplementationClassOne();  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(aInitParams); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

CImplementationClassOne::~CImplementationClassOne()
// Default virtual d'tor
	{
	delete iInternalDescriptor;
	}

CImplementationClassOne::CImplementationClassOne()
// Default c'tor for use by derived and 
// static construction methods only
: CExampleInterface()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

void CImplementationClassOne::ConstructL(TAny* aInitParams)
// Intended Usage	: Safely complete the initialization of the constructed object	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @return			void
// @pre 			CImplementationClassOne has been constructed
// @post			The CImplementationClassOne object has been fully instantiated
//
	{
	TExampleInterfaceInitParams* params = REINTERPRET_CAST(TExampleInterfaceInitParams*,
														   aInitParams);
	if(params->descriptor)
		iInternalDescriptor = params->descriptor->AllocL();

	Dll::SetTls(&iTLSInt);
	}

void CImplementationClassOne::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	REINTERPRET_CAST(TInt*, Dll::Tls());
	}

TInt CImplementationClassOne::FireAndForget()
	{
	TRAPD(error,DoMethodL());
	return error;			// Always KErrNotSupported
	}

// Provide the CActive overloads
void CImplementationClassOne::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CImplementationClassOne::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassOne::RunError(TInt aError)
	{
	return aError;
	}

TUid CImplementationClassOne::ImplId()
	{
	TUid idVal = {0x10009DC3};
	return (idVal);
	}

// ____________________________________________________________________________
// 
/**	Intended usage: This class implements the functionality promised by 
	the CExampleInterface defintion class. It does little apart from provides a test instance
	which may be retrieved and run for testing purposes.
	Its resolution is based upon its registered default data string that
	advertises this class as being able to handle 'text\*' data.
	I.e. it should be 'found' by wildcard matching for any text handling type.
	@since 7.0

 */
class CImplementationClassTwo : public CExampleInterface
{
// Methods
public:
/**
	@fn				NewL(TAny* aParams)
	Intended Usage	: Standardised safe construction which leaves nothing the cleanup stack.
	Error Condition	: Leaves with error code.
	@leave          	KErrNoMemory.
	@since			7.0
	@return			CImplementationClassTwo* The class instance.
	@pre 			None
	@post			CImplementationClassTwo has been constructed,
					and initialised.
 */
	static CImplementationClassTwo* NewL(TAny* aParams);

/**
	@fn				~CImplementationClassTwo()
	Intended Usage	: Default Destructor	
	Error Condition	: None	
	@since			7.0
	@pre 			CImplementationClassTwo has been constructed
	@post			CImplementationClassTwo has been completely destroyed.
 */
	virtual ~CImplementationClassTwo();

/**
	@fn				DoMethodL()
	Intended Usage	: Overload of the pure interface method
					Representative of a method provided on 
					the interface by the interface definer.
	Error Condition	: Leaves with an error code.
	@leave  		KErrNoMemory, KErrNotSupported.
	@since			7.0
	@return			None
	@pre 			CImplementationClassTwo has been constructed
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
	@pre 			CImplementationClassTwo has been constructed
	@post			Unspecified
 */
	TInt FireAndForget();

 /**
	@fn				ImplId()
	Intended Usage	: To verify the object returned by ECOM.
	Error Condition	: None.
	@since			7.0s
	@return			TUid (ECOM's Implementation Uid for this class.)
	@pre 			CImplementationClassThree has been constructed
	@post			Unspecified
 */
	TUid ImplId();

private:
/**
	@fn				CImplementationClassTwo()
	Intended Usage	: Default Constructor : usable only by derived classes	
	Error Condition	: None	
	@since			7.0
	@pre 			None
	@post			CImplementationClassTwo has been constructed
 */
	CImplementationClassTwo();

/**
	@fn				ConstructL()
	Intended Usage	: Completes the safe construction of the CImplementationClassTwo object
	Error Condition	: Leaves with the error code.	
	@leave          	KErrNoMemory.	
	@since			7.0
	@pre 			CImplementationClassTwo has been constructed
	@post			CImplementationClassTwo has been fully initialised.
 */
	void ConstructL();

// Provide the CActive overloads
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
};  // End of CImplementationClassTwo definition

// __________________________________________________________________________
// Implementation

CImplementationClassTwo* CImplementationClassTwo::NewL(TAny* /* aParams */)
// Intended Usage	: Safe construction which leaves nothing upon the cleanup stack	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @param			" "
// @return			CImplementationClassTwo* a pointer to the fully instantiated CImplementationClassTwo object
// @pre 			None
// @post			The object has been fully instantiated
// Static member
	{
	CImplementationClassTwo* self=new(ELeave) CImplementationClassTwo();  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

CImplementationClassTwo::~CImplementationClassTwo()
// Default virtual d'tor
	{
	// Do Nothing
	}

CImplementationClassTwo::CImplementationClassTwo()
// Default c'tor for use by derived and 
// static construction methods only
: CExampleInterface()
	{
	CActiveScheduler::Add(this);
	}

void CImplementationClassTwo::ConstructL()
// Intended Usage	: Safely complete the initialization of the constructed object	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @return			void
// @pre 			CImplementationClassTwo has been constructed
// @post			The CImplementationClassTwo object has been fully instantiated
//
	{
	// Do Nothing
	}

void CImplementationClassTwo::DoMethodL()
	{
	// Do nothing
	}

TInt CImplementationClassTwo::FireAndForget()
	{
	TInt error = KErrNone;
	TRAP(error,DoMethodL());
	if(!IsActive())
		{
		SetActive();
		iStatus = KRequestPending;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	return error;
	}

// Provide the CActive overloads
void CImplementationClassTwo::RunL()
	{
	delete this;	// AAARGH : Scary!!!!
	}

void CImplementationClassTwo::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassTwo::RunError(TInt aError)
	{
	return aError;
	}

TUid CImplementationClassTwo::ImplId()
	{
	TUid idVal = {0x10009DC4};
	return (idVal);
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10009DC3,	CImplementationClassOne::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x10009DC4,	CImplementationClassTwo::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x10009DD0,	CExampleResolver::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

