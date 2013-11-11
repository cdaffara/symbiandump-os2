/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

/**
 @file 
 @publishedPartner
 @released
*/
 

#include <e32base.h>

#include "ct.h"


/**
	void MCTWritableCertStore::Add(.., TBool aDeletable, ..)
	

	This is a default (dummy) implementation of the new Add(.., TBool aDeletable, ..) method
	to ensure backward compatibility with old implementations.

 	The MCTWritableCertStore is an abstract parent class for other implementation specific 
 	writable cert store classes such as  CFileCertStore, CFSCertStoreClient, CCheckedCertStore,  
 	and other partner defined classes.
 	
 	Every such child class derived from MCTWritableCertStore, provided it is written properly,
 	MUST have its own implementation for the  Add(.., TBool aDeletable, ..) method.
 	
 	Therefore, this dummy implementation in the parent class (MCTWritableCertStore) should NEVER
 	be engaged, since the child's class implementation is supposed to work. 
 	
 	Once, the method is called here, in the parent class, this simply means that child class
 	didn't manage to provide its implementation, which could in turn mean that either the child
 	class is older version, or the child class is poorly written.
 	
 	Thus, the current Add() returns with TRequestStatus& aStatus set to error 
 	value KErrNotSupported.
 */ 

inline void MCTWritableCertStore::Add( const TDesC&,
							TCertificateFormat,
							TCertificateOwnerType, 
					 		const TKeyIdentifier*,
					 		const TKeyIdentifier*,
					 		const TDesC8&, 
					 		const TBool,
					 		TRequestStatus&	aStatus	)
	{
		TRequestStatus* status = &aStatus;	
		User::RequestComplete( status, KErrNotSupported );
	}
	
