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
// This file contains the API definition for the classes TAuthorityC16 and
// CAuthority16. These classes provide non-modifying (TAuthorityC16) and 
// modifying (CAuthority16) functionality for the components of an
// Authority as described in RFC2396.
// 
//

/**
 @file Authority16.h
 @publishedAll
 @deprecated Deprecated in 9.1
*/

#ifndef __AUTHORITY16_H__
#define __AUTHORITY16_H__

// System includes
//
#include <e32base.h>
#include <authoritycommon.h>


/**
Dependencies : TAuthorityComponent.
Comments : Provides non-modifying functionality on the components of an authority 
object as defined in RFC2396. There are 3 components; userinfo, host and port.

The	object holds descriptor pointers to the parsed authority components and a descriptor 
pointer to the authority. It is non-owning. It uses 16-bit descriptors.

The functionality provided by this API allows the authority components to be extracted
from the parsed authority, checked for their presence in the authority and be compared 
with those in another TAuthorityC16 object.
@publishedAll
@deprecated Deprecated in 9.1
@since 6.0
*/
class TAuthorityC16
	{
public:	// Methods

	IMPORT_C const TDesC16& Extract(TAuthorityComponent aComponent) const;
	IMPORT_C TBool IsPresent(TAuthorityComponent aComponent) const;
	IMPORT_C TInt Compare(const TAuthorityC16& aAuthority, TAuthorityComponent aComponent) const;
	IMPORT_C const TDesC16& AuthorityDes() const;
	IMPORT_C HBufC* DisplayFormL(TAuthorityComponent aComponent = EAuthorityComplete ) const;

protected:	// Methods


	IMPORT_C TAuthorityC16();

	void Reset();

protected:	// Attributes

	/** 
		The array of descriptor pointers to the authority components.
	 */
	TPtrC16	iComponent[EAuthorityMaxComponents];

	/** 
		The descriptor pointer to the authority.
	 */
	TPtrC16	iAuthorityDes;

/**
	A friend class.
	@see		CAuthority16
	@since		6.0
 */
	friend class CAuthority16;

/**
	A friend class used for testing.
	@see		TAuthorityC16StateAccessor
	@since		6.0
 */
	friend class TAuthorityC16StateAccessor;	

	};

/**
Dependencies : TAuthorityC16
Comments : Provides functionality to parse a descriptor into the components of an 
authority as defined in RFC2396. There are 3 components; userinfo, host and port.

This uses 16-bit descriptors.

Format of an authority is; [userinfo@]host[:port]

@warning The descriptor that is parsed by an object of this class will be referenced 
by that object. If the original descriptor is no longer in scope there will be undefined 
behaviour.
@publishedAll
@deprecated Deprecated in 9.1. Use UriUtils::CreateAuthorityL() instead
@since 6.0
*/
class TAuthorityParser16 : public TAuthorityC16
	{
public:	// Methods

	IMPORT_C TAuthorityParser16();
	IMPORT_C TInt Parse(const TDesC16& aAuthority);

	};

/**
Dependencies : CBase, TAuthorityC16.
Comments : Provides modifying functionality on the components of an authority object, as
defined in RFC2396. There are 3 components; userinfo, host and port.

The	object holds parsed authority information. It is owning. It uses 16-bit descriptors.

The functionality provided by this API allows the authority components to be set or removed 
from this parsed authority. Also, it provides a reference to TAuthorityC16 object so that 
the non-modifying functionality can be used.
@publishedAll
@deprecated Deprecated in 9.1
@since 6.0
*/
class CAuthority16 : public CBase
	{
public: // Methods

	IMPORT_C static CAuthority16* NewL(const TAuthorityC16& aAuthority);
	IMPORT_C static CAuthority16* NewLC(const TAuthorityC16& aAuthority);
	IMPORT_C static CAuthority16* NewL();
	IMPORT_C static CAuthority16* NewLC();
	IMPORT_C ~CAuthority16();

	IMPORT_C const TAuthorityC16& Authority() const;
	IMPORT_C void SetComponentL(const TDesC16& aData, TAuthorityComponent aComponent);
	IMPORT_C void SetAndEscapeComponentL(const TDesC16& aData, TAuthorityComponent aComponent);
	IMPORT_C void RemoveComponentL(TAuthorityComponent aComponent);

private:	// Methods

	CAuthority16(const TAuthorityC16& aAuthority);

	void ConstructL();

	void FormAuthorityL();

private:	// Attributes

	/** 
		The descriptor buffer that contains the authority.
	 */
	HBufC16*		iAuthorityBuf;

	/** 
		The parsed authority object.
	 */
	TAuthorityC16	iAuthority;

/**
	A friend class used for testing.
	@see		TAuthority16StateAccessor
	@since		6.0
 */
	friend class TAuthority16StateAccessor;

	};
/** 	
Do not use. Use TAuthorityC8 instead
@publishedAll
@deprecated Deprecated in 9.1
 */
typedef TAuthorityC16		TAuthorityC;

/** 	
Do not use. Use TAuthorityParser8 instead
@publishedAll
@deprecated Deprecated in 9.1
 */
typedef TAuthorityParser16	TAuthorityParser;

/**
Do not use. Use CAuthority8 instead
@publishedAll
@deprecated Deprecated in 9.1
 */
typedef CAuthority16 CAuthority;

#endif	// __AUTHORITY16_H__
