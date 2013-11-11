/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CPKIXChainBuilder class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __PKIXCHAINBUILDER_H__
#define __PKIXCHAINBUILDER_H__

#include <e32base.h>
#include "pkixcerts.h"

class CPKIXCertChainAO;

class CPKIXChainBuilder : public CActive
	{
	friend class CPKIXCertChainAO;

public:
	static CPKIXChainBuilder* NewL();
	static CPKIXChainBuilder* NewLC();
	~CPKIXChainBuilder();

public:
	void AddSourceL(MPKIXCertSource* aSource);//takes ownership of aSource...
	void AddIssuer(TInt& aNumberOfCertsAdded, TBool& aResult, CArrayPtrFlat<CX509Certificate>& aChain, 
		TRequestStatus& aStatus);

private:
	CPKIXChainBuilder();
	void ConstructL();
	TBool ResolveIssuersL(CArrayPtr<CX509Certificate>& aChain, 
		const RPointerArray<CX509Certificate>& aCandidates) const;

private:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	enum TState
		{
		EIdle = 0,
		EAddCandidate,
		EFinished
		};

private:
	RMPointerArray<MPKIXCertSource> iSources;

	/**
	 * We don't own that object, it's only a pointer to a certificate
	 * in the CX509CertChain from which CPKIXCertChainBase derives.
	 */
	CX509Certificate* iSubject;
	TRequestStatus* iOriginalRequestStatus;
	TBool* iResult;
	CArrayPtrFlat<CX509Certificate>* iChain;
	TInt iIndex;
	TState iState;
	RPointerArray<CX509Certificate> iCandidates;
	TInt* iNumberOfCertsAdded;
	};

#endif
