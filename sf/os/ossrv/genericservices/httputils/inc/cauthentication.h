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
// Authentication class for storing login information
// 
//

/**
 @file cauthentication.h
 @publishedPartner
 @released
*/

#ifndef __CAUTHENTICATION_H__
#define __CAUTHENTICATION_H__

#include <uri8.h>

/**
CAuthentication encapsulates username and login information. 
Uses 8-bit descriptors. Unicode text must be tranformed into UTF8 for storage.
@publishedPartner
@released
*/
class CAuthentication: public CBase
	{
public:
	/**
	   enum for TMethod
	*/
	enum TMethod
		{
		/** Basic */
		EBasic,
		/** Digest */
		EDigest
		};
public:
	IMPORT_C static CAuthentication* NewL(const TDesC8& aName, const TDesC8& aPassword, TMethod aMethod = EDigest);
	IMPORT_C static CAuthentication* NewL(const TUriC8& aUri, TMethod aMethod = EDigest);
	IMPORT_C ~CAuthentication();
	IMPORT_C const TDesC8& Name() const;
	IMPORT_C const TDesC8& Password() const;
	IMPORT_C TMethod Method() const;
	IMPORT_C void SetNameL(const TDesC8& aName);
	IMPORT_C void SetPasswordL(const TDesC8& aPassword);
	IMPORT_C void SetMethod(TMethod aMethod);
private:
	CAuthentication(TMethod aMethod);
	void ConstructL(const TDesC8& aName, const TDesC8& aPassword);
	void ConstructL(const TUriC8& aUri);
private:
	/** 
		The descriptor buffer that contains the Name.
	 */
	HBufC8*		iName;
	/** 
		The descriptor buffer that contains the Password.
	 */
	HBufC8*		iPassword;
	/** 
		The enum of type TMethod.
	 */
	TMethod		iMethodType;
	};

#endif //__CAUTHENTICATION_H__
