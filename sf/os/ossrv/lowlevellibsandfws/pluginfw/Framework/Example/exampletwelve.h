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
// ExampleTweleve.h
// The definition of some classes to be provided by ECom.
// 1. Using the CExampleInterface class as a base.
// 2. Example of how extended interfaces could work.
// 
//

/**
 @file
 @internalComponent
*/

#include "Interface.h"
//#include "ImplementationProxy.h"
#include "TestUtilities.h"	// For __FILE__LINE__
#include "ExtendedInterfaceImplementationProxy.h"

const TUid KImplUid1 = {0x10009E39};
const TUid KImplUid2 = {0x10009E3C};
const TUid KImplUid3 = {0x10009E3D};

// ____________________________________________________________________________
// Class CImplementationClassTwelve
//
/**
	Intended usage: This class implements the functionality promised by
	the CExampleInterface and MExampleInterfaceExtended definition classes. 
	It does little apart from provides a test instance
	which may be retrieved and run for testing purposes. This is an example of
	how extended interface is implemented in the same class as the original and 
	how to get/release the extended interface which is implemented in different class.
 */
class CImplementationClassTwelve : public CExampleInterface, public MExampleInterfaceExtended
{
// Methods
public:
	static CImplementationClassTwelve* NewL(TAny* aInitParams);
	virtual ~CImplementationClassTwelve();
	void DoMethodL();
	TInt FireAndForget();
	TUid ImplId();
	virtual void DoMethodExtended();  //from MExampleInterfaceExtended
	static TAny* GetExtendedInterfaceL(TAny* aObject,const TUid& aExtendedInterface,TUint32& aBitFlags,TAny*& releaseObject);
	static void ReleaseExtendedInterface(TAny* aObject,const TUid& aInterface);
private:
	CImplementationClassTwelve();
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
/** Uid of the extended interface */
	TUid iExtendedInterfaceUid;		
};  // End of CImplementationClassTwelve definition

// ____________________________________________________________________________
// Class CImplementationClassTwelveExtended
//
/**
	Intended usage: This class implements the functionality promised by
	the MExampleInterfaceExtended2 definition class. This is an extended interface of
	CImplementationClassTwelve. This is a sample extended interface that is
	separate from the main interface. This extended interface does nothing, but shows
    how one can set up a separately instantiated extended interface.
 */
class CImplementationClassTwelveExtended : public CBase, public MExampleInterfaceExtended2
{
// Methods
public:
	static CImplementationClassTwelveExtended* NewL();
	virtual ~CImplementationClassTwelveExtended();
	virtual void DoMethodExtended2();  //from MExampleInterfaceExtended2
private:
	CImplementationClassTwelveExtended();
// Attribute
private:
	TUid iExtendedInterfaceUid;		
};  // End of CImplementationClassTwelveExtended definition

// ____________________________________________________________________________
// Class CImplementationClassTwelveExtended2
//
/**
	Intended usage: This class implements the functionality promised by
	the MExampleInterfaceExtended2 definition class. This is an extended interface of
	CImplementationClassTwelve. It is the same as CImplementationClassTwelveExtended,
	but just shows that it is possible to have multiple extended interfaces in one implementation.
 */
class CImplementationClassTwelveExtended2 : public CBase, public MExampleInterfaceExtended2
{
// Methods
public:
	static CImplementationClassTwelveExtended2* NewL();
	virtual ~CImplementationClassTwelveExtended2();
	virtual void DoMethodExtended2(); //from MExampleInterfaceExtended2
private:
	CImplementationClassTwelveExtended2();
// Attribute
private:
	TUid iExtendedInterfaceUid;		
};  // End of CImplementationClassTwelveExtended2 definition


// ____________________________________________________________________________
// Class CImplementationClassTwelveBasic
//
/**
This class implements the functionality promised by
the CExampleInterface definition class. It does little apart from provides a test instance
which may be retrieved and run for testing purposes. This is an example that no extended interface 
is implemented in this class.
 */
class CImplementationClassTwelveBasic : public CExampleInterface
{
// Methods
public:
	static CImplementationClassTwelveBasic* NewL(TAny* aInitParams);
	virtual ~CImplementationClassTwelveBasic();
	void DoMethodL();
	TInt FireAndForget();
	TUid ImplId();
private:
	CImplementationClassTwelveBasic();
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

};  // End of CImplementationClassTwelveBasic definition

// ____________________________________________________________________________
// Class CImplementationClassTwelve2
//
/**
Intended usage: This class implements the functionality promised by
the CExampleInterface definition class. It does little apart from provides a test instance
which may be retrieved and run for testing purposes.This is an example of
how extended interface is implemented in the same class as the original
 */
class CImplementationClassTwelve2 : public CExampleInterface, public MExampleInterfaceExtended
{
// Methods
public:
	static CImplementationClassTwelve2* NewL(TAny* aInitParams);
	virtual ~CImplementationClassTwelve2();
	void DoMethodL();
	TInt FireAndForget();
	TUid ImplId();
	static TAny* GetExtendedInterfaceL(TAny* aObject,const TUid& aExtendedInterface,TUint32& aBitFlags,TAny*& releaseObject);
 public: 
	virtual void DoMethodExtended(); // From MExampleInterfaceExtended
private:
	CImplementationClassTwelve2();
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
/** Uid of the extended interface */
	TUid iExtendedInterfaceUid;			
};  // End of CImplementationClassTwelve2 definition

