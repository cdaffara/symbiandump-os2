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


#ifndef _CEComEntry_H
#define _CEComEntry_H

#include <f32file.h>

NONSHARABLE_CLASS(CEComEntry) :public CBase
/**
@internalComponent
Created to avoid passing larger TEntry objects to functions.
 */

{
friend class CEComEntryTest;
public:

	static CEComEntry* NewL(const TDesC& aDllName,const TUid& aSecondUid, const TUid& aThirdUid);

	virtual ~CEComEntry();

	//Inline functions
	
	/**
	Intended Usage	:	Get the Name of EComEntry
	@return			TDesC& Name of EComEntry
	@pre 			Object is fully constructed and initialized
		
	**/
	const TDesC& GetName() const 
		{ return *iName; }
	/**
	Intended Usage	:	Get the second TUid of EComEntry
	@return			TUid of EComEntry
	@pre 			Object is fully constructed and initialized
		
	**/
	const TUid& GetSecondUid() const 
		{ return iSecondUid; }

	/**
	Intended Usage	:	Get the third TUid of EComEntry
	@return			TUid of EComEntry
	@pre 			Object is fully constructed and initialized
		
	**/
	const TUid& GetThirdUid() const 
		{ return iThirdUid; }

//This member function is only required in server side and hence configured out for client side		
#ifndef ECOM_CLIENT		
	/**
	Intended Usage	:	Get the Modified Time of EComEntry
	return			TTime& Modified time of EComEntry
	@pre 			Object is fully constructed and initialized
		
	**/
	const TTime& GetModified() const
		{ return iModified; }
	/**
	Intended Usage	:	Set the Modified Time of EComEntry
	@param			aModified New Modified Time
	@pre 			Object is fully constructed and initialized
		
	**/
	void SetModified(const TTime& aModified)  
		{ iModified = aModified.Int64(); }
#endif
		
private:

	CEComEntry(const TUid& aSecondUid,const TUid& aThirdUid);
	void ConstructL(const TDesC& aDllName);

private:
	HBufC* iName;
	TUid iSecondUid;
	TUid iThirdUid;
//This member variable is only required in server side and hence configured out for client side
#ifndef ECOM_CLIENT	
	TTime iModified;
#endif
};


#endif

