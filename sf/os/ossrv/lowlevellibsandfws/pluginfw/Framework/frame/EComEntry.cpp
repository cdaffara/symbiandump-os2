// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "EComEntry.h"

/**
@fn				CEComEntry()
Intended Usage	: Standardized default c'tor

@param aSecondUid: Identifies type of the Interface Implementation Collection. (collection or collection3)
			   	   It will be used by CLoadManager to decide how to initialise a plugin.	
@param  aThirdUid: Identifies a component uniquely. 	
Error Condition	: None	
**/
CEComEntry::CEComEntry(const TUid& aSecondUid,const TUid& aThirdUid):iSecondUid(aSecondUid),iThirdUid(aThirdUid) 
{
}


/**
@fn				NewL(const TEntry& aEntry)
Intended Usage	: Standardized safe construction which leaves nothing 
				on the cleanup stack.	
Error Condition	: Cannot fully construct because of memory limitations.	
@param aSecondUid: Identifies type of the Interface Implementation Collection. (collection or collection3)
			   	   It will be used by CLoadManager to decide how to initialise a plugin.	
@param aThirdUid: Identifies a component uniquely. 	
@return			A pointer to the new class
@post			CEComEntry is fully constructed, 
				and initialized.
**/
CEComEntry* CEComEntry::NewL(const TDesC& aDllName,const TUid& aSecondUid, const TUid& aThirdUid)
{
	CEComEntry* self = new(ELeave) CEComEntry(aSecondUid, aThirdUid);
	CleanupStack::PushL(self);
	self->ConstructL(aDllName);
	CleanupStack::Pop(self);
	return self;
}


/**
@fn				void ConstructL()
Intended Usage	: Standardised 2nd, (Initialisation) phase of two phase construction.
Error Condition	: None

@pre 			CEComEntry is fully constructed.
@post			CEComEntry is fully initialised.
*/
void CEComEntry::ConstructL(const TDesC& aDllName)
{
	iName = aDllName.AllocL();	
}


/**
@fn				~CEComEntry()
Intended Usage	: Standard default d'tor	
Error Condition	: None	
 */
CEComEntry::~CEComEntry()
{
	 delete iName;
}


