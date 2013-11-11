/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CSWICertStoreToken class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CSWICERTSTORETOKEN_H__
#define __CSWICERTSTORETOKEN_H__

#include <ct/mcttoken.h>

class CSWICertStore;
class CCTTokenType;

/**
 * SWI Cert store token.
 */
NONSHARABLE_CLASS(CSWICertStoreToken) : public CBase, public MCTToken
	{
public:
	CSWICertStoreToken(CCTTokenType& aTokenType);
	
public:
	// Implementation of MCTToken
	virtual MCTTokenType& TokenType();
	virtual const TDesC& Label();
	virtual TCTTokenHandle Handle();

	/// Called by CSWICertStore::DoRelease
	TBool ReleaseInterface();
	
protected:
	virtual TInt& ReferenceCount();
	virtual void DoGetInterface(TUid aRequiredInterface,
							  MCTTokenInterface*& aReturnedInterface, 
							  TRequestStatus& aStatus);
	virtual TBool DoCancelGetInterface();
	virtual const TDesC& Information(TTokenInformation aRequiredInformation);

private:
	virtual ~CSWICertStoreToken();
	
private:
	CCTTokenType& iTokenType;
	TInt iRefCount;
	CSWICertStore* iCertStore;
	TInt iInterfaceRefCount;
	};

#endif
