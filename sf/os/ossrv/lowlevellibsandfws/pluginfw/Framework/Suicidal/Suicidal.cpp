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
// The implementation of a Suicidal class
// to be provided by ECom.
// 1. Using the CExampleInterface class as a base.
// 
//

#include "TestUtilities.h"	// For __FILE__LINE__
#include "Interface.h"
#include "ImplementationProxy.h"

// ____________________________________________________________________________
// 
/**
	Intended usage: This class implements the functionality promised by 
	the CExampleInterface defintion class. It does little apart from provides a test instance
	which may be retrieved and run for testing purposes, and then commits suicide.
	Its resolution is based upon its registered default data string that
	advertises this class as being able to handle 'text\wml' data.
	@since 7.0
 */
class CSuicidalImplementation : public CExampleInterface
{
// Methods
public:
/**
	@fn				NewL()
	Intended Usage	: Standardised safe construction which leaves nothing the cleanup stack.
	Error Condition	: Leaves with error code.
	@leave          KErrNoMemory.
	@since			7.0
	@return			CSuicidalImplementation* The class instance.
	@pre 			None
	@post			CSuicidalImplementation has been constructed,
					and initialised.
 */
	static CSuicidalImplementation* NewL();

/**
	@fn				~CSuicidalImplementation()
	Intended Usage	: Default Destructor	
	Error Condition	: None	
	@since			7.0
	@pre 			CSuicidalImplementation has been constructed
	@post			CSuicidalImplementation has been completely destroyed, 
					and is no longer registered with the scheduler.
 */
	virtual ~CSuicidalImplementation();

/**
	@fn				DoMethodL()
	Intended Usage	: Overload of the pure interface method
					Representative of some method provided on 
					the interface by the interface definer.
	Error Condition	: Leaves with an error code.
	@leave  		KErrNoMemory, KErrNotSupported.
	@since			7.0
	@return			None
	@pre 			CSuicidalImplementation has been constructed
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
	@pre 			CSuicidalImplementation has been constructed
	@post			CSuicidalImplementation is active with a status of KRequestPending.
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
	@fn				CSuicidalImplementation()
	Intended Usage	: Default Constructor : usable only by derived classes	
	Error Condition	: None	
	@since			7.0
	@pre 			None
	@post			CSuicidalImplementation has been constructed
 */
	CSuicidalImplementation();

/**
	@fn				ConstructL()
	Intended Usage	: Completes the safe construction of the CSuicidalImplementation object
	Error Condition	: Leaves with the error code.	
	@leave                  KErrNoMemory.	
	@since			7.0
	@pre 			CSuicidalImplementation has been constructed
	@post			CSuicidalImplementation has been fully initialised.
 */
	void ConstructL();

// Provide the CActive overloads
/**
	@fn				RunL()
	Intended Usage	: When the object activates commit suicide. 
	Error Condition	: Not enough memory available to complete the scan.
	@leave   		KErrNoMemory
	@since			7.0
	@pre 			CSuicidalImplementation is fully constructed.
	@post			CSuicidalImplementation has committed suicide,
					and deleted itself.
 */
	void RunL();
/**
	@fn				DoCancel()
	Intended Usage	: The cancel action called by CActive::Cancel(). 
	Error Condition	: None	
	@since			7.0
	@pre 			CSuicidalImplementation is fully constructed.
	@post			CSuicidalImplementation behaviour is cancelled and
					it is no longer active on the current scheduler.
 */
	void DoCancel();

/**
	@fn				RunError(TInt aError)
	Intended Usage	: Called by the RunL leaving.
	@since			7.0
	@param			aError The error code that the RunL left with.
	@return			TInt KErrNone. 
	@pre 			CSuicidalImplementation is fully constructed.
	@post			CSuicidalImplementation is returned to a
					sensible active state.
 */
	TInt RunError(TInt aError);

};  // End of CSuicidalImplementation definition

// __________________________________________________________________________
// Implementation
const TInt KDefaultTestAllocSize = 8;

CSuicidalImplementation* CSuicidalImplementation::NewL()
// Intended Usage	: Safe construction which leaves nothing upon the cleanup stack	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @param			" "
// @return			CSuicidalImplementation* a pointer to the fully instantiated CSuicidalImplementation object
// @pre 			None
// @post			The object has been fully instantiated
// Static member
	{
	CSuicidalImplementation* self=new(ELeave) CSuicidalImplementation();  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

CSuicidalImplementation::~CSuicidalImplementation()
// Default virtual d'tor
	{
	// Do Nothing
	}

CSuicidalImplementation::CSuicidalImplementation()
// Default c'tor for use by derived and 
// static construction methods only
: CExampleInterface()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

void CSuicidalImplementation::ConstructL()
// Intended Usage	: Safely complete the initialization of the constructed object	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @return			void
// @pre 			CSuicidalImplementation has been constructed
// @post			The CSuicidalImplementation object has been fully instantiated
//
	{
	// Allocate and delete some memory 
	// so that leave testing can check 
	// for the correct handling.
	HBufC* temp = HBufC::NewL(KDefaultTestAllocSize);
	delete temp;
	}

void CSuicidalImplementation::DoMethodL()
	{
	CActiveScheduler::Add(this);
	}

TInt CSuicidalImplementation::FireAndForget()
	{
	TInt error = KErrNone;
	TRAP(error,DoMethodL());
	if(!IsActive())
		{
		SetActive();
		iStatus = KRequestPending;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, error);
		}
	return error;
	}

// Provide the CActive overloads
void CSuicidalImplementation::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	delete this;	// AAARGH : Scary self deletion!!!!
	}

void CSuicidalImplementation::DoCancel()
	{
	// Do nothing
	}

TInt CSuicidalImplementation::RunError(TInt)
	{
	delete this;		// AAARGH : Scary self deletion!!!!
	return KErrNone;	// Don't Panic
	}

TUid CSuicidalImplementation::ImplId()
	{
	TUid idVal = {0x10009DC5};
	return (idVal);
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10009DC5,	CSuicidalImplementation::NewL)	// SuicidalImplementationClass
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
