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
// The implementation of some classes to be provided by ECom.
// 
//

/**
 @file
 @internalComponent
*/

#include "exampletwelve.h"

// __________________________________________________________________________
//
// Implementation of CImplementationClassTwelve

/**
Safe construction which leaves nothing upon the cleanup stack
@param			aInitParams Initialization parameters
@return			CImplementationClassTwelve* a pointer to the fully instantiated CImplementationClassTwelve object
*/
CImplementationClassTwelve* CImplementationClassTwelve::NewL(TAny* aInitParams)
	{
	CImplementationClassTwelve* self=new(ELeave) CImplementationClassTwelve(); 
	CleanupStack::PushL(self);	
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor of CImplementationClassTwelve
 */
CImplementationClassTwelve::~CImplementationClassTwelve()
	{
	delete iInternalDescriptor;
	Dll::SetTls(NULL);
	}

/**
Default Constructor : usable only by derived classes
*/
CImplementationClassTwelve::CImplementationClassTwelve()
: CExampleInterface()
	{
	//set the extended interface Uid
	iExtendedInterfaceUid.iUid = 0x10009E44;
	}

/**
Safely complete the initialization of the constructed object.
@param			aInitParams Initialization parameters
*/
void CImplementationClassTwelve::ConstructL(TAny* aInitParams)
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
void CImplementationClassTwelve::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	ASSERT(Dll::Tls()!=NULL);
	}

/**
Overload of the pure interface method asynchronous function which
an interface definer could specify.
@return			TInt KErrNone for success.
 */
TInt CImplementationClassTwelve::FireAndForget()
	{
	return KErrNone;			
	}

// Provide the CActive overloads
void CImplementationClassTwelve::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CImplementationClassTwelve::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassTwelve::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

/**
To verify the object returned by ECOM.
@return			TUid (ECOM's Implementation Uid for this class.)
 */
TUid CImplementationClassTwelve::ImplId()
	{
	return KImplUid1;
	}

/**
Extended interface method. Called to verify the Extended interface.
 */
void CImplementationClassTwelve::DoMethodExtended()
	{
	//check the extended interface uid has been set properly
	ASSERT(iExtendedInterfaceUid.iUid == 0x10009E44);
	}

/**
Get the extended interface. This method will be called by ECOM, so the method must
follow the signature defined by TProxyExtendedInterfaceGetPtrL. This must be a static
method since it is used in the proxy table.
@param			aObject A pointer to the instantiation interface (CImplementationClassTwelve
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
TAny* CImplementationClassTwelve::GetExtendedInterfaceL(TAny* aObject,const TUid& aExtendedInterface,TUint32& aBitFlags,TAny*& aReleaseObject)

	{
	//Initialise the release bit
	aBitFlags = aBitFlags & 0xFFFFFFFE;
	TAny* ret = NULL;
	switch (aExtendedInterface.iUid)
		{
		case 0x10009E44:
		    {
	        // No release is required, so do not modify aBitFlags.
		    ret = static_cast<MExampleInterfaceExtended*>(static_cast<CImplementationClassTwelve*>(aObject));
		    break;
		    }
		case 0x10009E45:
		    {
		    // Indicate to caller that release is required
		    aBitFlags = aBitFlags | KReleaseRequiredMask;

		    CImplementationClassTwelveExtended *classExt = CImplementationClassTwelveExtended::NewL();
		    // Must set the release object for ECOM to release later, this will not be the same as the interface object that is returned.
		    aReleaseObject = classExt;

		    ret = static_cast<MExampleInterfaceExtended2*>(classExt);
		    break;
		    }
		case 0x10009E46:
		    {
		    // Indicate to caller that release is required
		    aBitFlags = aBitFlags | KReleaseRequiredMask;

		    CImplementationClassTwelveExtended2 *classExt = CImplementationClassTwelveExtended2::NewL();
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
void CImplementationClassTwelve::ReleaseExtendedInterface(TAny* aReleaseObject,const TUid& aExtendedInterface)
	{
	switch (aExtendedInterface.iUid)
	    {
	    case 0x10009E45:
			{
	        // this object is an interface that was a separate object from the main instantiated object, and must be cleaned up.
			CImplementationClassTwelveExtended* classExt = static_cast<CImplementationClassTwelveExtended*>(aReleaseObject);
			delete classExt;
			break;
	        }
 		case 0x10009E46:
			{
	        // this object is an interface that was a separate object from the main instantiated object, and must be cleaned up.
			CImplementationClassTwelveExtended2* classExt = static_cast<CImplementationClassTwelveExtended2*>(aReleaseObject);
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
// Implementation of CImplementationClassTwelveExtended

/**
Safe construction which leaves nothing upon the cleanup stack
@return			CImplementationClassTwelve* a pointer to the fully instantiated CImplementationClassTwelve object
*/
CImplementationClassTwelveExtended* CImplementationClassTwelveExtended::NewL()

	{
	CImplementationClassTwelveExtended* self=new(ELeave) CImplementationClassTwelveExtended();  // calls c'tor
	return self;
	}

/**
Destructor of CImplementationClassTwelveExtended
 */
CImplementationClassTwelveExtended::~CImplementationClassTwelveExtended()
	{
	// do nothing
	}

/**
Default Constructor : usable only by derived classes
*/
CImplementationClassTwelveExtended::CImplementationClassTwelveExtended()
	{
	//set the extended interface uid
	iExtendedInterfaceUid.iUid = 0x10009E45;
	}

/**
Extended interface.
*/
void CImplementationClassTwelveExtended::DoMethodExtended2()
	{
	//check the extended interface uid has been set properly
	ASSERT(iExtendedInterfaceUid.iUid == 0x10009E45);
	}

// __________________________________________________________________________
//
// Implementation of CImplementationClassTwelveExtended2

/**
Safe construction which leaves nothing upon the cleanup stack
@return			CImplementationClassTwelveExtended2* a pointer to the fully instantiated CImplementationClassTwelve object
*/
CImplementationClassTwelveExtended2* CImplementationClassTwelveExtended2::NewL()
	{
	CImplementationClassTwelveExtended2* self=new(ELeave) CImplementationClassTwelveExtended2();  // calls c'tor
	return self;
	}

/**
Destructor of CImplementationClassTwelveExtended2
 */
CImplementationClassTwelveExtended2::~CImplementationClassTwelveExtended2()
	{
	// do nothing
	}

/**
Default Constructor : usable only by derived classes
*/
CImplementationClassTwelveExtended2::CImplementationClassTwelveExtended2()
	{
	//set the extended interface uid
	iExtendedInterfaceUid.iUid = 0x10009E46;
	}

/**
Extended interface.
*/
void CImplementationClassTwelveExtended2::DoMethodExtended2()
	{
	//check the extended interface uid has been set properly
	ASSERT(iExtendedInterfaceUid.iUid == 0x10009E46);
	}


// __________________________________________________________________________
// Implementation of CImplementationClassTwelveBasic

/**
Safe construction which leaves nothing upon the cleanup stack
@param			aInitParams The parameter struct used for initialising this object
@return			CImplementationClassTwelveBasic* a pointer to the fully instantiated CImplementationClassTwelveBasic object
*/
CImplementationClassTwelveBasic* CImplementationClassTwelveBasic::NewL(TAny* aInitParams)
	{
	CImplementationClassTwelveBasic* self=new(ELeave) CImplementationClassTwelveBasic();  
	CleanupStack::PushL(self);	
	self->ConstructL(aInitParams); 
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor of CImplementationClassTwelveBasic
 */
CImplementationClassTwelveBasic::~CImplementationClassTwelveBasic()
	{
	delete iInternalDescriptor;
	Dll::FreeTls();
	}

/**
Default Constructor : usable only by derived classes
*/
CImplementationClassTwelveBasic::CImplementationClassTwelveBasic()
: CExampleInterface()
	{
	}

/**
Completes the safe construction of the CImplementationClassTwelveBasic object
@param			aInitParams The parameter struct used for initialising this object
 */
void CImplementationClassTwelveBasic::ConstructL(TAny* aInitParams)
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
void CImplementationClassTwelveBasic::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	ASSERT(Dll::Tls() != NULL);
	}

/**
Overload of the pure interface method asynchronous function which
an interface definer could specify. 
@return			TInt KErrNone for success.
 */
TInt CImplementationClassTwelveBasic::FireAndForget()
	{
	return KErrNone;			
	}

// Provide the CActive overloads
void CImplementationClassTwelveBasic::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CImplementationClassTwelveBasic::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassTwelveBasic::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

/**
To verify the object returned by ECOM.
@return			TUid (ECOM's Implementation Uid for this class.)
 */
TUid CImplementationClassTwelveBasic::ImplId()
	{
	return KImplUid2;
	}

// __________________________________________________________________________
// Implementation of CImplementationClassTwelve2

/**
Standardised safe construction which leaves nothing the cleanup stack.
@param			aInitParams The parameter struct used for initialising this object
@return			CImplementationClassTwelve2* The class instance.
 */
CImplementationClassTwelve2* CImplementationClassTwelve2::NewL(TAny* aInitParams)
	{
	CImplementationClassTwelve2* self=new(ELeave) CImplementationClassTwelve2();  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(aInitParams); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}
/**
Destructor of CImplementationClassTwelve2
 */
CImplementationClassTwelve2::~CImplementationClassTwelve2()
	{
	delete iInternalDescriptor;
	Dll::FreeTls();
	}

/**
Default Constructor
*/
CImplementationClassTwelve2::CImplementationClassTwelve2()
: CExampleInterface()
	{
	//set the extended interface uid
	iExtendedInterfaceUid.iUid = 0x10009E44;
	}

/**
Completes the safe construction of the CImplementationClassTwelveBasic object
@param			aInitParams The parameter struct used for initialising this object
 */
void CImplementationClassTwelve2::ConstructL(TAny* aInitParams)
	{
	TExampleInterfaceInitParams* params = REINTERPRET_CAST(TExampleInterfaceInitParams*, aInitParams);
	if(params)
		{
		if(params->descriptor)
			iInternalDescriptor = params->descriptor->AllocL();
		}
	User::LeaveIfError(Dll::SetTls(&iTLSInt));
	}

/**
Overload of the pure interface method.Representative of a method provided on
the interface by the interface definer.
 */
void CImplementationClassTwelve2::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	ASSERT(Dll::Tls()!=NULL);
	}

/**
Overload of the pure interface method asynchronous function which
an interface definer could specify. 
@return			TInt KErrNone for success.
 */
TInt CImplementationClassTwelve2::FireAndForget()
	{
	return KErrNone;			
	}

// Provide the CActive overloads
void CImplementationClassTwelve2::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CImplementationClassTwelve2::DoCancel()
	{
	// Do nothing
	}

TInt CImplementationClassTwelve2::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

/**
To verify the object returned by ECOM.
@return			TUid (ECOM's Implementation Uid for this class.)
 */
TUid CImplementationClassTwelve2::ImplId()
	{
	return KImplUid3;
	}

/**
Extended interface method. Called to verify the Extended interface.
 */
void CImplementationClassTwelve2::DoMethodExtended()
	{
	//check the extended interface uid has been set properly
	ASSERT(iExtendedInterfaceUid.iUid == 0x10009E44);
	}

/**
Get the extended interface. This method will be called by ECOM, so the method must
follow the signature defined by TProxyExtendedInterfaceGetPtrL.This must be a static
method since it is used in the proxy table.
@param			aObject A pointer to the instantiation interface (CImplementationClassTwelve
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
TAny* CImplementationClassTwelve2::GetExtendedInterfaceL(TAny* aObject,const TUid& aExtendedInterface,TUint32& /*aBitFlags*/,TAny*& /*releaseObject*/)
	{
	TAny* ret = NULL;
	switch (aExtendedInterface.iUid)
		{
		case 0x10009E44:
		    {
	        // No release is required, so do not modify aBitFlags.
		    ret = static_cast<MExampleInterfaceExtended*>(static_cast<CImplementationClassTwelve2*>(aObject));
		    break;
		    }
		default:
			{
			break;    
			}    
		}
	return ret;
	}
