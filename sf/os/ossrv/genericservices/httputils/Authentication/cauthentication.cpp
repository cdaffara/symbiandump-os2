// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// authentication.cpp
// 
//

#include <e32base.h>

#include "cauthentication.h"


/**
	Creates a new instance of CAuthentication.

	@param aName The new user name. A local copy of descriptor is created. Can be a null descriptor.
	@param aPassword The new password. A local copy of descriptor is created. Can be a null descriptor.
	@param aMethod the method type of either basic or digest. Defaults to digest.
	@return A pointer to the newly created CAuthentication object.
	@leave KErrNoMemory
 */
EXPORT_C CAuthentication* CAuthentication::NewL(const TDesC8& aName, const TDesC8& aPassword, TMethod aMethod)
	{
	CAuthentication* me = new (ELeave) CAuthentication(aMethod);
	CleanupStack::PushL(me);
	me->ConstructL(aName, aPassword);
	CleanupStack::Pop(me);
	return me;
	}

/**
	Creates a new instance of CAuthentication.

	@param aUri The URI with a userinfo component.
	@param aMethod the method type of either basic or digest. Defaults to digest.
	@return A pointer to the newly created CAuthentication object.
	@leave KErrNotFound If there is no userinfo component.
	@leave KErrNoMemory
*/
EXPORT_C CAuthentication* CAuthentication::NewL(const TUriC8& aUri, TMethod aMethod)
	{
	CAuthentication* me = new (ELeave) CAuthentication(aMethod);
	CleanupStack::PushL(me);
	me->ConstructL(aUri);
	CleanupStack::Pop(me);
	return me;
	}

/**
	Constructor.
	@param aMethod enum value of type TMethod.
 */
CAuthentication::CAuthentication(TMethod aMethod):iMethodType(aMethod)
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully construct 
	the object.
	
	@param			aName A descriptor to be allocated initialised with.
	@param			aPassword A descriptor to be allocated initialised with.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed and initialized.
 */
void CAuthentication::ConstructL(const TDesC8& aName, const TDesC8& aPassword)
	{
	iName = aName.AllocL();
	iPassword = aPassword.AllocL();
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully construct 
	the object. Must set both user name and password to at least an empty string.
	
	@param			aUri The URI with a userinfo component.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed and initialized.
 */
void CAuthentication::ConstructL(const TUriC8& aUri)
	{
	// Check if user info component present.
	if(!aUri.IsPresent(EUriUserinfo))
		{
		User::Leave(KErrNotFound);
		}
	// Set name and pwd. Note that these could be empty strings.
	TPtrC8 userInfo(aUri.Extract(EUriUserinfo));
	TInt colPos = userInfo.Locate(':');
	if(KErrNotFound == colPos) // 'name@' or '@' case
		{
		iName = userInfo.AllocL();
		iPassword = KNullDesC8().AllocL();
		}	
	else // ':' found
		{
		iName = userInfo.Left(colPos).AllocL();
		iPassword = userInfo.Right(userInfo.Length()-colPos-1).AllocL();						
		}
	}

/**
	Destructor
 */
EXPORT_C CAuthentication::~CAuthentication()
	{
	delete iName;
	delete iPassword;
	}

/**
	Gets the user name.

	@return Reference to the user name descriptor.
 */
EXPORT_C const TDesC8& CAuthentication::Name() const
	{
	return *iName;
	}

/**
	Gets the password.

	@return Reference to the password descriptor.
 */
EXPORT_C const TDesC8& CAuthentication::Password() const
	{
	return *iPassword;
	}

/**
	Gets the method type.

	@return The authentication method type.
 */
EXPORT_C CAuthentication::TMethod CAuthentication::Method() const
	{
	return iMethodType;
	}

/**
	Sets the username.

	@param aName Reference to the new username descriptor.
	@leave KErrNoMemory
 */
EXPORT_C void CAuthentication::SetNameL(const TDesC8& aName)
	{
	HBufC8* temp = iName;
	iName = aName.AllocL();
	delete temp;
	}

/**
	Sets the password.

	@param aPassword Reference to the new password descriptor.
	@leave KErrNoMemory
 */
EXPORT_C void CAuthentication::SetPasswordL(const TDesC8& aPassword)
	{
	HBufC8* temp = iPassword;
	iPassword = aPassword.AllocL();
	delete temp;
	}

/**
	Sets the method type.

	@param aMethod The new authentication method type.
 */
EXPORT_C void CAuthentication::SetMethod(TMethod aMethod)
	{
	iMethodType = aMethod;
	}
