/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TEST1CERTSTORETOKEN_H__
#define __TEST1CERTSTORETOKEN_H__

#include <e32base.h>
#include <ct.h>
#include "Test1certstoretokentype.h"

class CTest1CertStore;

class CTest1CertStoreToken : public CBase, public MCTToken
	{
	friend class CTest1CertStore;

public:
	static CTest1CertStoreToken* New(CTest1CertStoreTokenType::TTokenId aTokenId, 
		CTest1CertStoreTokenType& aFileCertStoreTokenType);
	virtual ~CTest1CertStoreToken();

public:	// from MCTToken
	virtual MCTTokenType& TokenType();
	virtual const TDesC& Label();
	virtual TCTTokenHandle Handle();
//	virtual TCTTokenObjectHandle Handle(MCTTokenObject& aTokenObject);
protected:	// from MCTToken
	virtual void DoRelease();
	virtual TInt& ReferenceCount();
	virtual void DoGetInterface(TUid aRequiredInterface, 
		MCTTokenInterface*& aReturnedInterface,	TRequestStatus& aStatus);
	virtual TBool DoCancelGetInterface();
	const TDesC& Information(TTokenInformation);
private:
	CTest1CertStoreToken(CTest1CertStoreTokenType::TTokenId aTokenId,
		CTest1CertStoreTokenType& aFileCertStoreTokenType);

private:
	CTest1CertStoreTokenType& iTokenType;
	TInt iRefCount;

	CTest1CertStoreTokenType::TTokenId iTokenId;

	TInt iRefCountInterface;
	TInt iRefCountInterface2;

	CTest1CertStore* iFileCertStore;
	TFileMode iFileMode;
	};

#endif
