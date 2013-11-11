// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __LOGCNTMODEL_H__
#define __LOGCNTMODEL_H__

#include <ecom/ecom.h>
#include "logcntdef.h"
#include <logwrap.h>

/** Provides an interface to decouple the internal static dependency of LogEng on 
Contacts.
 
Rather than statically linking against a specific contacts implementation dll
the log engine dynamically loads a plugin implementation of this class, to remove any need for 
a direct dependency on a specific Contacts implementation.

The interface is provided to allow logeng to retrieve a string which contains contacts information 
for a given phone number.
 
@internalAll */
class MLogCntModel
	{
public:
	/** Opens the contacts DB.	
	@capability ReadUserData WriteUserData */
	virtual void OpenContactsL()=0;
	
	/** Closes the contacts DB. 
	@capability None */
	virtual	void CloseContacts()=0;
	
	/** Attempts to find a contact item ID for the contact items which contains
	the specified telephone number in a telephone, fax or SMS type field.
	If more than one contact item contains the telephone number this should be 
	treated the same as no contact found.
	
	@capability ReadUserData
	@param aNumber Phone number string
	@param aMatchLengthFromRight Number of digits from the right of the phone number to use
	@return DB Id of the DB entry corresponding to the phone number, KLogNullContactId if none or more than one found */
	virtual TLogContactItemId MatchPhoneNumberL(const TDesC& aNumber, TInt aMatchLengthFromRight)=0;
	
	/** Gets the text data for the family and given name fields of a given contact Id.
	
	@capability ReadUserData
	@param aContactId Contact Id to find data for
	@param aName On return contains a string holding the family and given name in the desired format if found, a 0 length string otherwise. 
	The buffer for aName is supplied by the client and should be of a length sufficent to hold a standard contact string.
	@param aNameFormat Desired format of returned string - Chinese or Western format */
	virtual void ReadContactNameL(TLogContactItemId aContactId, TDes& aName, TLogContactNameFormat aNameFormat)=0;	
	};

/** Interface UID to be used by all contacts model plugin's for log engine
@internalAll */
#ifdef SYSLIBS_TEST 
    const TUid  KUidEComLogCntInterface = {0x20008030};
#else
    const TUid  KUidEComLogCntInterface = {0x20008029};
#endif
/** Provides an interface for ECom plugins
@internalAll */
class CLogCntModel: public CBase, public MLogCntModel
	{
public:
	IMPORT_C static CLogCntModel* NewL();
	IMPORT_C virtual ~CLogCntModel();
private:
	TUid iDtor_ID_Key;
	};
	
#endif		// __LOGCNTMODEL_H__

