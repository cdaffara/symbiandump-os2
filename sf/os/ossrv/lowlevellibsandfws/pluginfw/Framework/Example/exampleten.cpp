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
// The implementation of some classes
// to be provided by ECom.
// 1. Using the CExampleInterface class as a base.
// 2. Example of how extended interfaces could work.
// 
//

/**
 @file
 @publishedAll
*/

#include "Interface.h"
#include "ecom/extendedinterfaceimplementationproxy.h"
#include "TestUtilities.h"	// For __FILE__LINE__

const TUid KImplUid1 = {0x10009E38};
const TUid KImplUid2 = {0x10009E3A};
const TUid KImplUid3 = {0x10009E3B};

// ____________________________________________________________________________
//
/**
	Intended usage: This class implements the functionality promised by
	the CExampleInterface and MExampleInterfaceExtended definition classes. 
	It does little apart from provides a test instance
	which may be retrieved and run for testing purposes. This is an example of
	how extended interface is implemented in the same class as the original and 
	how to get/release the extended interface which is implemented in different class.
 */
class CImplementationClassTen : public CExampleInterface, public MExampleInterfaceExtended
{
// Methods
public:
	static CImplementationClassTen* NewL(TAny* aInitParams);
	virtual ~CImplementationClassTen();
	void DoMethodL();
	TInt FireAndForget();
	TUid ImplId();
	virtual void DoMethodExtended();  //from MExampleInterfaceExtended
	static TAny* GetExtendedInterfaceL(TAny* aObject,const TUid& aExtendedInterface,TUint32& aBitFlags,TAny*& releaseObject);
	static void ReleaseExtendedInterface(TAny* aObject, const TUid& aInterface);
private:
	CImplementationClassTen();
	void ConstructL(TAny* aInitParams);
// Provide the CActive overloads
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
/** A place for allocating some memory in the ConstructL */
	HBufC*	iInternalDescriptor;
/** An int to be stored in TLS to test its usage */
	TInt	iTLSInt;
/** Uid of the extended interface */
	TUid iExtendedInterfaceUid;		
};  // End of CImplementationClassTen definition

// ____________________________________________________________________________
//
/**
	Intended usage: This class implements the functionality promised by
	the MExampleInterfaceExtended2 definition class. This is an extended interface of
	CImplementationClassTen. This is a sample extended interface that is
	separate from the main interface. This extended interface does nothing, but shows
    how one can set up a separately instantiated extended interface.
 */
class CImplementationClassTenExtended : public CBase, public MExampleInterfaceExtended2
{
// Methods
public:
	static CImplementationClassTenExtended* NewL();
	virtual ~CImplementationClassTenExtended();
	virtual void DoMethodExtended2();  //from MExampleInterfaceExtended2
private:
	CImplementationClassTenExtended();
// Attribute
private:
	TUid iExtendedInterfaceUid;		
};  // End of CImplementationClassTenExtended definition

// ____________________________________________________________________________
//
/**
	Intended usage: This class implements the functionality promised by
	the MExampleInterfaceExtended2 definition class. This is an extended interface of
	CImplementationClassTen. It is the same as CImplementationClassTenExtended,
	but just shows that it is possible to have multiple extended interfaces in one implementation.
 */
class CImplementationClassTenExtended2 : public CBase, public MExampleInterfaceExtended2
{
// Methods
public:
	static CImplementationClassTenExtended2* NewL();
	virtual ~CImplementationClassTenExtended2();
	virtual void DoMethodExtended2(); //from MExampleInterfaceExtended2
private:
	CImplementationClassTenExtended2();
// Attribute
private:
	TUid iExtendedInterfaceUid;		
};  // End of CImplementationClassTenExtended2 definition


// __________________________________________________________________________
// Implementation

/**
Safe construction which leaves nothing upon the cleanup stack
@param			aInitParams Initialization parameters
@return			CImplementationClassTen* a pointer to the fully instantiated CImplementationClassTen object
*/
CImplementationClassTen* CImplementationClassTen::NewL(TAny* aInitParams)
	{
	CImplementationClassTen* self=new(ELeave) CImplementationClassTen();  
	CleanupStack::PushL(self);	
	self->ConstructL(aInitParams); 
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor of CImplementationClassTen
 */
CImplementationClassTen::~CImplementationClassTen()
	{
	delete iInternalDescriptor;
	Dll::FreeTls();
	}

/**
Default Constructor : usable only by derived classes
*/
CImplementationClassTen::CImplementationClassTen()
: CExampleInterface()
	{
	//set the extended interface Uid
	iExtendedInterfaceUid.iUid = 0x10009E44;
	}

/**
Safely complete the initialization of the constructed object.
@param			aInitParams Initialization parameters
*/
void CImplementationClassTen::ConstructL(TAny* aInitParams)
	{
	TExampleInterfaceInitParams* params = REINTERPRET_CAST(TExampleInterfaceInitParams*,aInitParams);
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
Overload of the pure interface method.Representative of a method provided on
the interface by the interface definer.
 */
void CImplementationClassTen::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	ASSERT(Dll::Tls() != NULL);
	}

/**
Overload of the pure interface method asynchronous function which
an interface definer could specify.
@return			TInt KErrNone for success.
 */
TInt CImplementationClassTen::FireAndForget()
	{
	return KErrNone;			
	}

// Provide the CActive overloads
void CImplementationClassTen::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CImplementationClassTen::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassTen::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

/**
To verify the object returned by ECOM.
@return			TUid (ECOM's Implementation Uid for this class.)
 */
TUid CImplementationClassTen::ImplId()
	{
	return KImplUid1;
	}

/**
Extended interface method. Called to verify the Extended interface.
 */
void CImplementationClassTen::DoMethodExtended()
	{
	//check the extended interface uid has been set properly
	ASSERT(iExtendedInterfaceUid.iUid == 0x10009E44);
	}

/**
Get the extended interface. This method will be called by ECOM, so the method must
follow the signature defined by TProxyExtendedInterfaceGetPtrL. This must be a static
method since it is used in the proxy table.
@param			aObject A pointer to the instantiation interface (CImplementationClassTen
				in this case) instance. This will be provided by ECOM. It must be provided
				as a parameter,	as this method is a static method (and must be a static
				method because it is called by the proxy table).
@param			aExtendedInterface The extended interface to fetch. Must be a unique UID.
@param			aBitFlags Flags used for communication between plugin's and ECOM. Currently
				used to signal if an extended interface requires release.
@param			aReleaseObject return parameter, provides ECOM with the object to destroy
				if the interface requires to be released.
@return			TAny* a pointer to the fully constructed extended interface object
*/
TAny* CImplementationClassTen::GetExtendedInterfaceL(TAny* aObject,const TUid& aExtendedInterface,TUint32& aBitFlags,TAny*& aReleaseObject)

	{
	//Initialise the release bit
	aBitFlags = aBitFlags & 0xFFFFFFFE;
	TAny* ret = NULL;
	switch (aExtendedInterface.iUid)
		{
		case 0x10009E44:
		    {
	        // No release is required, so do not modify aBitFlags.
		    ret = static_cast<MExampleInterfaceExtended*>(static_cast<CImplementationClassTen*>(aObject));
		    break;
		    }
		case 0x10009E45:
		    {
		    // Indicate to caller that release is required
		    aBitFlags = aBitFlags | KReleaseRequiredMask;

		    CImplementationClassTenExtended *classExt = CImplementationClassTenExtended::NewL();
		    // Must set the release object for ECOM to release later, this will not be the same as the interface object that is returned.
		    aReleaseObject = classExt;

		    ret = static_cast<MExampleInterfaceExtended2*>(classExt);
		    break;
		    }
		case 0x10009E46:
		    {
		    // Indicate to caller that release is required
		    aBitFlags = aBitFlags | KReleaseRequiredMask;

		    CImplementationClassTenExtended2 *classExt = CImplementationClassTenExtended2::NewL();
		    // Must set the release object for ECOM to release later, this will not be the same as the interface object that is returned.
		    aReleaseObject = classExt;

		    ret = static_cast<MExampleInterfaceExtended2*>(classExt);
		    break;
		    }
		default:
			{
			break;    
			}
		}
	return ret;
	}

/**
Release the specified extended interface. This method will be called by ECOM, so the method must
follow the signature defined by TProxyExtendedInterfaceReleasePtr. This must be a static
method since it is used in the proxy table.
method.
@param			aReleaseObject provides ECOM with the object to destroy
				if the interface requires to be released.
@param			aExtendedInterface extended interface that is to be released
@return			TAny* a pointer to the fully constructed extended interface object
*/
void CImplementationClassTen::ReleaseExtendedInterface(TAny* aReleaseObject,const TUid& aExtendedInterface)
	{
	switch (aExtendedInterface.iUid)
	    {
	    case 0x10009E45:
			{
	        // this object is an interface that was a separate object from the main instantiated object, and must be cleaned up.
			CImplementationClassTenExtended* classExt = static_cast<CImplementationClassTenExtended*>(aReleaseObject);
			delete classExt;
			break;
	        }
 		case 0x10009E46:
			{
	        // this object is an interface that was a separate object from the main instantiated object, and must be cleaned up.
			CImplementationClassTenExtended2* classExt = static_cast<CImplementationClassTenExtended2*>(aReleaseObject);
			delete classExt;
			break;
	        }
	    default:
			{
			break;    
			}    
	    }
	}


// __________________________________________________________________________
//
// Implementation of CImplementationClassTenExtended

/**
Safe construction which leaves nothing upon the cleanup stack
@return			CImplementationClassTen* a pointer to the fully instantiated CImplementationClassTen object
*/
CImplementationClassTenExtended* CImplementationClassTenExtended::NewL()

	{
	CImplementationClassTenExtended* self=new(ELeave) CImplementationClassTenExtended(); 
	return self;
	}

/**
Destructor of CImplementationClassTenExtended
 */
CImplementationClassTenExtended::~CImplementationClassTenExtended()
	{
	// do nothing
	}

/**
Default Constructor : usable only by derived classes
*/
CImplementationClassTenExtended::CImplementationClassTenExtended()
	{
	//set the extended interface uid
	iExtendedInterfaceUid.iUid = 0x10009E45;
	}

/**
Extended interface.
*/
void CImplementationClassTenExtended::DoMethodExtended2()
	{
	//check the extended interface uid has been set properly
	ASSERT(iExtendedInterfaceUid.iUid == 0x10009E45);
	}

// __________________________________________________________________________
//
// Implementation of CImplementationClassTenExtended2

/**
Safe construction which leaves nothing upon the cleanup stack
@return			CImplementationClassTenExtended2* a pointer to the fully instantiated CImplementationClassTen object
*/
CImplementationClassTenExtended2* CImplementationClassTenExtended2::NewL()
	{
	CImplementationClassTenExtended2* self=new(ELeave) CImplementationClassTenExtended2(); 
	return self;
	}

/**
Destructor of CImplementationClassTenExtended2
 */
CImplementationClassTenExtended2::~CImplementationClassTenExtended2()
	{
	// do nothing
	}

/**
Default Constructor : usable only by derived classes
*/
CImplementationClassTenExtended2::CImplementationClassTenExtended2()
	{
	//set the extended interface uid
	iExtendedInterfaceUid.iUid = 0x10009E46;
	}

/**
Extended interface.
*/
void CImplementationClassTenExtended2::DoMethodExtended2()
	{
	//check the extended interface uid has been set properly
	ASSERT(iExtendedInterfaceUid.iUid == 0x10009E46);
	}

// CImplementationClassTenBasic implementation
// ____________________________________________________________________________
//
/**
This class implements the functionality promised by
the CExampleInterface definition class. It does little apart from provides a test instance
which may be retrieved and run for testing purposes. This is an example that no extended interface 
is implemented in this class.
*/
class CImplementationClassTenBasic : public CExampleInterface
{
// Methods
public:
	static CImplementationClassTenBasic* NewL(TAny* aInitParams);
	virtual ~CImplementationClassTenBasic();
	void DoMethodL();
	TInt FireAndForget();
	TUid ImplId();
private:
	CImplementationClassTenBasic();
	void ConstructL(TAny* aInitParams);
	// Provide the CActive overloads
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
/** A place for allocating some memory in the ConstructL */
	HBufC*	iInternalDescriptor;
/** An int to be stored in TLS to test its usage */
	TInt	iTLSInt;

};  // End of CImplementationClassTenBasic definition

// __________________________________________________________________________
// Implementation

/**
Safe construction which leaves nothing upon the cleanup stack
@param			aInitParams The parameter struct used for initialising this object
@return			CImplementationClassTenBasic* a pointer to the fully instantiated CImplementationClassTenBasic object
*/
CImplementationClassTenBasic* CImplementationClassTenBasic::NewL(TAny* aInitParams)
	{
	CImplementationClassTenBasic* self=new(ELeave) CImplementationClassTenBasic();  
	CleanupStack::PushL(self);	
	self->ConstructL(aInitParams); 
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor of CImplementationClassTenBasic
 */
CImplementationClassTenBasic::~CImplementationClassTenBasic()
	{
	delete iInternalDescriptor;
	Dll::FreeTls();
	}

/**
Default Constructor : usable only by derived classes
*/
CImplementationClassTenBasic::CImplementationClassTenBasic()
: CExampleInterface()
	{
	}

/**
Completes the safe construction of the CImplementationClassTenBasic object
@param			aInitParams The parameter struct used for initialising this object
 */
void CImplementationClassTenBasic::ConstructL(TAny* aInitParams)
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
Overload of the pure interface method.Representative of a method provided on
the interface by the interface definer.
 */
void CImplementationClassTenBasic::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	ASSERT(Dll::Tls() != NULL);
	}

/**
Overload of the pure interface method asynchronous function which
an interface definer could specify. 
@return			TInt KErrNone for success.
 */
TInt CImplementationClassTenBasic::FireAndForget()
	{
	return KErrNone;			
	}

// Provide the CActive overloads
void CImplementationClassTenBasic::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CImplementationClassTenBasic::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassTenBasic::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

/**
To verify the object returned by ECOM.
@return			TUid (ECOM's Implementation Uid for this class.)
 */
TUid CImplementationClassTenBasic::ImplId()
	{
	return KImplUid2;
	}

// ____________________________________________________________________________
//
/**
Intended usage: This class implements the functionality promised by
the CExampleInterface defintion class. It does little apart from provides a test instance
which may be retrieved and run for testing purposes.This is an example of
how extended interface is implemented in the same class as the original.
 */
class CImplementationClassTen2 : public CExampleInterface, public MExampleInterfaceExtended
{
// Methods
public:
	static CImplementationClassTen2* NewL(TAny* aInitParams);
	virtual ~CImplementationClassTen2();
	void DoMethodL();
	TInt FireAndForget();
	TUid ImplId();
	static TAny* GetExtendedInterfaceL(TAny* aObject,const TUid& aExtendedInterface,TUint32& aBitFlags,TAny*& releaseObject);
 public: 
	virtual void DoMethodExtended(); // From MExampleInterfaceExtended
private:
	CImplementationClassTen2();
	void ConstructL(TAny* aInitParams);
// Provide the CActive overloads
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
/** A place for allocating some memory in the ConstructL */
	HBufC*	iInternalDescriptor;
/** An int to be stored in TLS to test its usage */
	TInt	iTLSInt;
/** Uid of the extended interface */
	TUid iExtendedInterfaceUid;			
};  // End of CImplementationClassTen2 definition

// __________________________________________________________________________
// Implementation

/**
Standardised safe construction which leaves nothing the cleanup stack.
@param			aInitParams The parameter struct used for initialising this object
@return			CImplementationClassTen2* The class instance.
 */
CImplementationClassTen2* CImplementationClassTen2::NewL(TAny* aInitParams)
	{
	CImplementationClassTen2* self=new(ELeave) CImplementationClassTen2(); 
	CleanupStack::PushL(self);	
	self->ConstructL(aInitParams); 
	CleanupStack::Pop(self);
	return self;
	}
/**
Destructor of CImplementationClassTen2
 */
CImplementationClassTen2::~CImplementationClassTen2()
	{
	delete iInternalDescriptor;
	Dll::FreeTls();
	}

/**
Default Constructor
*/
CImplementationClassTen2::CImplementationClassTen2()
: CExampleInterface()
	{
	//set the extended interface uid
	iExtendedInterfaceUid.iUid = 0x10009E44;
	}

/**
Completes the safe construction of the CImplementationClassTenBasic object
@param			aInitParams The parameter struct used for initialising this object
 */
void CImplementationClassTen2::ConstructL(TAny* aInitParams)
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
Overload of the pure interface method.Representative of a method provided on
the interface by the interface definer.
 */
void CImplementationClassTen2::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	ASSERT(Dll::Tls() != NULL);
	}

/**
Overload of the pure interface method asynchronous function which
an interface definer could specify. 
@return			TInt KErrNone for success.
 */
TInt CImplementationClassTen2::FireAndForget()
	{
	return KErrNone;			
	}

// Provide the CActive overloads
void CImplementationClassTen2::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CImplementationClassTen2::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassTen2::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

/**
To verify the object returned by ECOM.
@return			TUid (ECOM's Implementation Uid for this class.)
 */
TUid CImplementationClassTen2::ImplId()
	{
	return KImplUid3;
	}

/**
Extended interface method. Called to verify the Extended interface.
 */
void CImplementationClassTen2::DoMethodExtended()
	{
	//check the extended interface uid has been set properly
	ASSERT(iExtendedInterfaceUid.iUid == 0x10009E44);
	}

/**
Get the extended interface. This method will be called by ECOM, so the method must
follow the signature defined by TProxyExtendedInterfaceGetPtrL.This must be a static
method since it is used in the proxy table.
@param			aObject A pointer to the instantiation interface (CImplementationClassTen
				in this case) instance. This will be provided by ECOM. It must be provided
				as a parameter,	as this method is a static method (and must be a static
				method because it is called by the proxy table).
@param			aExtendedInterface The extended interface to fetch. Must be a unique UID.
@param			aBitFlags Flags used for communication between plugin's and ECOM. Currently
				used to signal if an extended interface requires release.
@param			aReleaseObject return parameter, provides ECOM with the object to destroy
				if the interface requires to be released.
@return			TAny* a pointer to the fully constructed extended interface object
*/
TAny* CImplementationClassTen2::GetExtendedInterfaceL(TAny* aObject,const TUid& aExtendedInterface,TUint32& /*aBitFlags*/,TAny*& /*releaseObject*/)
	{
	TAny* ret = NULL;
	switch (aExtendedInterface.iUid)
		{
		case 0x10009E44:
		    {
	        // No release is required, so do not modify aBitFlags.
		    ret = static_cast<MExampleInterfaceExtended*>(static_cast<CImplementationClassTen2*>(aObject));
		    break;
		    }
		default:
			{
			break;    
			}    
		}
	return ret;
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy3 KImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY3(0x10009E38,CImplementationClassTen::NewL,CImplementationClassTen::GetExtendedInterfaceL,CImplementationClassTen::ReleaseExtendedInterface),
	IMPLEMENTATION_PROXY_ENTRY3(0x10009E3A,CImplementationClassTenBasic::NewL,0,0),
	IMPLEMENTATION_PROXY_ENTRY3(0x10009E3B,CImplementationClassTen2::NewL,CImplementationClassTen2::GetExtendedInterfaceL,0)
	};

EXPORT_C const TImplementationProxy3* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy3);
	return KImplementationTable;
	}
