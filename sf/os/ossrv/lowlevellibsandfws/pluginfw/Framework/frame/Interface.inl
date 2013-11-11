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
// This file contains the placeholder class
// which represents the interface creation.
// 
//

/**
 @internalComponent
 @file
*/


const TUid KCExampleInterfaceUid = {0x10009DC0};
const TUid KCExample10InterfaceUid = {0x10009DBB};
const TUid KCExtendedInterfaceUid1 = {0x10009E44};
const TUid KCExtendedInterfaceUid2 = {0x10009E45};
const TUid KCExtendedInterfaceUid3 = {0x10009E46};

_LIT8(KImplementationTest,"text/wml");
_LIT8(KImplementationTest10,"Example 10");

inline CExampleInterface::CExampleInterface()
: CActive(CActive::EPriorityLow)
	{
	// Do nothing
	}


inline CExampleInterface::~CExampleInterface()
	{
	Cancel();
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}


inline CExampleInterface* CExampleInterface::NewL()
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching
	TUid resolverUid = {0x10009DD0};

	// Set up some empty initialisation parameters
	TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	return REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExampleInterfaceUid, 
															   _FOFF(CExampleInterface,iDtor_ID_Key),
															   &initParams,
															   ResolverParams,
															   resolverUid));
	}

inline CExampleInterface* CExampleInterface::NewL(const TDesC8& aMatchString)
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(aMatchString);
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching

	// Set up some empty initialisation parameters
	TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	return REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExampleInterfaceUid, 
															   _FOFF(CExampleInterface,iDtor_ID_Key), 
															   &initParams,
															   ResolverParams));
	}

inline CExampleInterface* CExampleInterface::NewL(TInt aIntParameterTest, const TDesC& aDesParameterTest)
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching

	// Set up the initialisation parameter passing
	TExampleInterfaceInitParams initParams;
	initParams.integer		= aIntParameterTest;
	initParams.descriptor	= &aDesParameterTest;

	// The CreateImplementationL method will return
	// the created item.
	return REINTERPRET_CAST(CExampleInterface*, REComSession::CreateImplementationL(KCExampleInterfaceUid,
												_FOFF(CExampleInterface,iDtor_ID_Key),
												&initParams,
												ResolverParams));
	}

inline CExampleInterface* CExampleInterface::NewL2(const TUid aImplementationUid)
	{
	// Set up some empty initialisation parameters
	TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;
	
	// The CreateImplementationL method will return
	// the created item.
	return REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(aImplementationUid, 
															   _FOFF(CExampleInterface,iDtor_ID_Key),
															   &initParams));
	}

inline CExampleInterface* CExampleInterface::NewL3()
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching
	
	// Set up some empty initialisation parameters
	TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// The CreateImplementationL method will return
	// the created item.
	return REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExampleInterfaceUid, 
															   _FOFF(CExampleInterface,iDtor_ID_Key),
															   &initParams,
															   ResolverParams));
	}

inline CExampleInterface* CExampleInterface::NewL4()
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching
	TUid resolverUid = {0x10009DD0};

	// Set up some empty initialisation parameters
	TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;
	
	// The CreateImplementationL method will return
	// the created item.
	return REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExampleInterfaceUid, 
															   _FOFF(CExampleInterface,iDtor_ID_Key),
															   &initParams,
															   ResolverParams,
															   resolverUid));
	}


inline CExampleInterface* CExampleInterface::NewL5()
	{
	//set up extended interface;
	RArray<TUid> extendedInterface;
	CleanupClosePushL(extendedInterface);
	extendedInterface.AppendL(KCExtendedInterfaceUid1);
	extendedInterface.AppendL(KCExtendedInterfaceUid2);
	extendedInterface.AppendL(KCExtendedInterfaceUid3);
	
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest10());
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching
	
	// The CreateImplementationL method will return the created item.
	CExampleInterface* dll = REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExample10InterfaceUid, 
																extendedInterface,
															   _FOFF(CExampleInterface,iDtor_ID_Key),
															   ResolverParams));
	CleanupStack::PopAndDestroy(&extendedInterface);
	return dll;															   
	}


inline CExampleInterface* CExampleInterface::NewL6()
	{
	//set up extended interface;
	RArray<TUid> extendedInterface;
	CleanupClosePushL(extendedInterface);
	extendedInterface.AppendL(KCExtendedInterfaceUid1);
	extendedInterface.AppendL(KCExtendedInterfaceUid2);
	extendedInterface.AppendL(KCExtendedInterfaceUid3);
	
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest10());
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching
	TUid resolverUid = {0x10009DD0};

	// The CreateImplementationL method will return the created item.
	CExampleInterface* dll = REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExample10InterfaceUid, 
																extendedInterface,
															   _FOFF(CExampleInterface,iDtor_ID_Key),
															   ResolverParams,
															   resolverUid));
	CleanupStack::PopAndDestroy(&extendedInterface);
	return dll;															   
	}


inline CExampleInterface* CExampleInterface::NewL7()
	{
	//set up extended interface;
	RArray<TUid> extendedInterface;
	CleanupClosePushL(extendedInterface);
	extendedInterface.AppendL(KCExtendedInterfaceUid1);
	extendedInterface.AppendL(KCExtendedInterfaceUid2);
	extendedInterface.AppendL(KCExtendedInterfaceUid3);
	
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest10());
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching
	
	// Set up some empty initialisation parameters
	TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// The CreateImplementationL method will return the created item.
	CExampleInterface* dll = REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExample10InterfaceUid, 
																extendedInterface,
															   _FOFF(CExampleInterface,iDtor_ID_Key),
															   ResolverParams,
															   &initParams));
	CleanupStack::PopAndDestroy(&extendedInterface);
	return dll;															   
	}


inline CExampleInterface* CExampleInterface::NewL8()
	{
	//set up extended interface;
	RArray<TUid> extendedInterface;
	CleanupClosePushL(extendedInterface);
	extendedInterface.AppendL(KCExtendedInterfaceUid1);
	extendedInterface.AppendL(KCExtendedInterfaceUid2);
	extendedInterface.AppendL(KCExtendedInterfaceUid3);
	
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest10());
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching
	TUid resolverUid = {0x10009DD0};

	// Set up some empty initialisation parameters
	TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// The CreateImplementationL method will return the created item.
	CExampleInterface* dll = REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExample10InterfaceUid, 
																extendedInterface,
															   _FOFF(CExampleInterface,iDtor_ID_Key),
															   ResolverParams,
															   resolverUid,
															   &initParams));
	CleanupStack::PopAndDestroy(&extendedInterface);
	return dll;															   
	}


	
inline void CExampleInterface::ListImplementationsL(RImplInfoPtrArray& aImplInfoArray)
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching

	REComSession::ListImplementationsL(KCExampleInterfaceUid,
									   ResolverParams, 
									   aImplInfoArray);
	}

inline void CExampleInterface::ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray)
	{
	REComSession::ListImplementationsL(KCExampleInterfaceUid, aImplInfoArray);
	}


inline void CExampleInterface::DoMethodL()
	{
	User::Leave(KErrNotSupported);
	}


inline TInt CExampleInterface::FireAndForget()
	{
	return KErrNotSupported;
	}

inline TAny* CExampleInterface::GetExtendedInterfaceL(const TUid& aInterfaceUid)
	{
	TAny *object = NULL;
	object = REComSession::GetExtendedInterfaceL(iDtor_ID_Key, aInterfaceUid);
	return object;
	}
	
inline void CExampleInterface::ManuallyReleaseExtendedInterfaceL(const TUid& aInterfaceUid)
	{
	REComSession::ManuallyReleaseExtendedInterfaceL(iDtor_ID_Key, aInterfaceUid);
	}
