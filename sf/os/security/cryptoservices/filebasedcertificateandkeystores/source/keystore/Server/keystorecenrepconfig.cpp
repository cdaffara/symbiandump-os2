/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* keystorecenrepconfig.h
* Contains the default values to be used by file tokens for 
* authenticating a user.
*
*/


/**
 @file 
 @released
 @internalComponent 
*/

#include <centralrepository.h>

#include "keystorecenrepconfig.h"

CKeyStoreCenrep* CKeyStoreCenrep::NewL()
	{
	CKeyStoreCenrep* keystoreCenrep = new(ELeave) CKeyStoreCenrep();
	keystoreCenrep->ConstructL();
	return keystoreCenrep;
	}

CKeyStoreCenrep::CKeyStoreCenrep()
	{}

CKeyStoreCenrep::~CKeyStoreCenrep()
	{
	delete iFileServerAuthRepository;
	}

void CKeyStoreCenrep::ConstructL()
	{
	iFileServerAuthRepository = CRepository::NewL(KUidFileTokensRepository);
	}

void CKeyStoreCenrep::AuthExpressionL(TDes& aExpression)
	{
	User::LeaveIfError(iFileServerAuthRepository->Get(EAuthExpression, aExpression));
	}

TInt CKeyStoreCenrep::FreshnessL()
	{
	TInt freshness(0);
	return User::LeaveIfError(iFileServerAuthRepository->Get(EAuthFreshness, freshness));
	}
