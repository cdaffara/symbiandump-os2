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
* Contains the default values to be used by file tokens for 
* authenticating a user.
*
*/


/**
 @file 
 @released
 @internalComponent 
*/


#ifndef KEYSTORECENREPCONFIG_H
#define KEYSTORECENREPCONFIG_H

#include <e32def.h>
#include <e32cmn.h>

class CRepository;

/** The UID of the file tokens repository. */
const TUid KUidFileTokensRepository = {0x200218e6};  
	
NONSHARABLE_CLASS (CKeyStoreCenrep) : public CBase
	{
public:
	static CKeyStoreCenrep* NewL();
	virtual ~CKeyStoreCenrep();
	
private:
	CKeyStoreCenrep();
	void ConstructL();
	
public:
	void AuthExpressionL(TDes& aExpression);
	TInt FreshnessL();
	
private:
	CRepository* iFileServerAuthRepository;
	
private:
	/**
		Filetokens server repository configuration parameter Enums.
	*/
	enum TAuthKeys
		{
		EAuthExpression			= 0x00000000,	
		EAuthFreshness			= 0x00000001,   
		};
	
	

	};

#endif // KEYSTORECENREPCONFIG_H
