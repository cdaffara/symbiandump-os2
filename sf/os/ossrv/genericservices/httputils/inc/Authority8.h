// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the API definition for the classes TAuthorityC8 and
// CAuthority8. These classes provide non-modifying (TAuthorityC8) and 
// modifying (CAuthority8) functionality for the components of an
// Authority as described in RFC2396.
// 
//

/**
 @file Authority8.h
 @publishedAll
 @released
*/

#ifndef __AUTHORITY8_H__
#define __AUTHORITY8_H__

// System includes
//
#include <e32base.h>
#include <authoritycommon.h>


/**
Dependencies : TAuthorityComponent.
Comments : Provides non-modifying functionality on the components of an authority 
object as defined in RFC2396. There are 3 components; userinfo, host and port.

The	object holds descriptor pointers to the parsed authority components and a descriptor 
pointer to the authority. It is non-owning. It uses 8-bit descriptors.

The functionality provided by this API allows the authority components to be extracted
from the parsed authority, checked for their presence in the authority and be compared 
with those in another TAuthorityC8 object.
@publishedAll
@released
@since 6.0
*/
class TAuthorityC8
	{
public:	// Methods

	IMPORT_C const TDesC8& Extract(TAuthorityComponent aComponent) const;
	IMPORT_C TBool IsPresent(TAuthorityComponent aComponent) const;
	IMPORT_C TInt Compare(const TAuthorityC8& aAuthority, TAuthorityComponent aComponent) const;
	IMPORT_C const TDesC8& AuthorityDes() const;
	IMPORT_C HBufC* DisplayFormL(TAuthorityComponent aComponent = EAuthorityComplete ) const;

protected:	// Methods

	IMPORT_C TAuthorityC8();
	void Reset();

protected:	// Attributes

	/** 
		The array of descriptor pointers to the authority components.
	 */
	TPtrC8	iComponent[EAuthorityMaxComponents];

	/** 
		The descriptor pointer to the authority.
	 */
	TPtrC8	iAuthorityDes;

/**
	A friend class.
	@see		CAuthority8
	@since		6.0
 */
	friend class CAuthority8;

/**
	A friend class used for testing.
	@see		TAuthorityC8StateAccessor
	@since		6.0
 */
	friend class TAuthorityC8StateAccessor;	

	};

/**
Dependencies : TAuthorityC8
Comments : Provides functionality to parse a descriptor into the components of an 
authority as defined in RFC2396. There are 3 components; userinfo, host and port.

This uses 8-bit descriptors.


Format of an authority is; [userinfo@]host[:port]

@warning The descriptor that is parsed by an object of this class will be referenced 
by that object. If the original descriptor is no longer in scope there will be undefined 
behaviour.
@publishedAll
@released
@since 6.0
*/
class TAuthorityParser8 : public TAuthorityC8
	{
public:	// Methods

	IMPORT_C TAuthorityParser8();
	IMPORT_C TInt Parse(const TDesC8& aAuthority);

	};

/**
Dependencies : CBase, TAuthorityC8.
Comments : Provides modifying functionality on the components of an authority object, as
defined in RFC2396. There are 3 components; userinfo, host and port.

The	object holds parsed authority information. It is owning. It uses 8-bit descriptors.

The functionality provided by this API allows the authority components to be set or removed 
from this parsed authority. Also, it provides a reference to TAuthorityC8 object so that 
the non-modifying functionality can be used.
@publishedAll
@released
@since 6.0
*/
class CAuthority8 : public CBase
	{
public: // Methods

	IMPORT_C static CAuthority8* NewL(const TAuthorityC8& aAuthority);
	IMPORT_C static CAuthority8* NewLC(const TAuthorityC8& aAuthority);
	IMPORT_C static CAuthority8* NewL();
	IMPORT_C static CAuthority8* NewLC();
	IMPORT_C ~CAuthority8();

	IMPORT_C const TAuthorityC8& Authority() const;
	IMPORT_C void SetComponentL(const TDesC8& aData, TAuthorityComponent aComponent);
	IMPORT_C void SetAndEscapeComponentL(const TDesC8& aData, TAuthorityComponent aComponent);
	IMPORT_C void RemoveComponentL(TAuthorityComponent aComponent);

private:	// Methods

	CAuthority8(const TAuthorityC8& aAuthority);
	void ConstructL();
	void FormAuthorityL();

private:	// Attributes

	/** 
		The descriptor buffer that contains the authority.
	 */
	HBufC8*			iAuthorityBuf;

	/** 
		The parsed authority object.
	 */
	TAuthorityC8	iAuthority;

/**
	A friend class used for testing.
	@see		TAuthority8StateAccessor
	@since		6.0
 */
	friend class TAuthority8StateAccessor;

	};

#endif	// __AUTHORITY8_H__
