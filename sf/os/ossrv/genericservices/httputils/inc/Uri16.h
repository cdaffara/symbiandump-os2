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
// This file contains the API definition for the classes TUriC16 and 
// CUri16. These classes provide non-modifying (TUriC16) and modifying
// (CUri16) functionality for the components of a Uri as described in 
// RFC2396.
// 
//

/**
 @file Uri16.h
 @publishedAll
 @deprecated Deprecated in 9.1
*/

#ifndef __URI16_H__
#define __URI16_H__

// System includes
//
#include <e32base.h>
#include <f32file.h> 
#include <uricommon.h>


class TUriC16
/**
	Dependencies : TUriComponent.
	Comments : Provides non-modifying functionality on the components of a uri object as
	defined in RFC2396. There are 5 components; scheme, authority, path, query and fragment.

The	object holds descriptor pointers to the parsed uri components and a descriptor pointer 
to the uri. It is non-owning. It uses 16-bit descriptors.

	The functionality provided by this API allows the uri components to be extracted from the 
	parsed uri, checked for their presence in the uri and be compared with those in another 
	TUriC16 object.
	@publishedAll
	@deprecated Deprecated in 9.1
	@since 6.0
*/
	{
public:	// Methods

	IMPORT_C HBufC* GetFileNameL() const;
	IMPORT_C HBufC* GetFileNameL(TUriFileName aType) const;
	IMPORT_C const TDesC16& Extract(TUriComponent aComponent) const;
	IMPORT_C void UriWithoutFragment(TPtrC16& aUriNoFrag) const;

	IMPORT_C TBool IsPresent(TUriComponent aComponent) const;
	IMPORT_C TBool IsSchemeValid() const;

	IMPORT_C TInt Compare(const TUriC16& aUri, TUriComponent aComponent) const;

	IMPORT_C const TDesC16& UriDes() const;

	IMPORT_C TInt Validate() const;
	IMPORT_C TInt Equivalent(const TUriC16& aUri) const;
	IMPORT_C HBufC* DisplayFormL(TUriComponent aComponent = EUriComplete) const;

protected:	// Methods

	IMPORT_C TUriC16();

	void Reset();
	
private:	// Methods
	
	TInt ValidateL() const;
	TInt EquivalentL(const TDesC16& aUri) const;

protected:	// Attributes

	/** The array of descriptor pointers to the uri components.
	 */
	TPtrC16		iComponent[EUriMaxComponents];

	/** The descriptor pointer to the uri.
	 */
	TPtrC16		iUriDes;

/**
	A friend class.
	@see		CUri16 
	@since		6.0
 */
	friend class CUri16;
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
/**
	A friend class used for testing.
	@see		TUriC16StateAccessor
	@since		6.0	
 */
#else
/**
	A friend class used for testing.
	@see		TUriC16StateAccessor
	@since		6.0
	@internalComponent
 */
#endif	//SYMBIAN_ENABLE_SPLIT_HEADERS
	friend class TUriC16StateAccessor;

	};

/**
Dependencies : TUriC16
Comments : Provides functionality to parse a descriptor into the components of a uri as 
defined in RFC2396. There are 5 components; scheme, authority, path, query and fragment.

It uses 16-bit descriptors.

Format of a uri is; scheme://authority path?query\#fragment

@warning The descriptor that is parsed by an object of this class will be referenced 
by that object. If the original descriptor is no longer in scope there will be undefined 
behaviour.
@publishedAll
@deprecated Deprecated in 9.1. Use UriUtils::CreateUriL() instead
@since 6.0
*/
class TUriParser16 : public TUriC16
	{
public:	// Methods

	IMPORT_C TUriParser16();
	IMPORT_C TInt Parse(const TDesC16& aUri);
	};

/**
	Dependencies : CBase, TUriC16.
	Comments : Provides modifying functionality on the components of a uri object, as
	defined in RFC2396. There are 5 components; scheme. authority, path, query and fragment.

	The	object holds parsed uri information. It is owning. It uses 16-bit descriptors.

	The functionality provided by this API allows the uri components to be set or removed 
	from this parsed uri. Also, it provides a reference to TUriC16 object so that the non-modifying 
	functionality can be used.
	@publishedAll
	@deprecated Deprecated in 9.1
	@since 6.0
*/
class CUri16 : public CBase
	{
public: // Methods
	IMPORT_C static CUri16* CreateFileUriL(const TDesC& aFullFileName, TUint aFlags = 0);
	IMPORT_C static CUri16* CreatePrivateFileUriL(const TDesC& aRelativeFileName, TDriveNumber aDrive, TInt aFlags = 0);

	IMPORT_C static CUri16* NewL(const TUriC16& aUri);
	IMPORT_C static CUri16* NewLC(const TUriC16& aUri);
	IMPORT_C static CUri16* NewL();
	IMPORT_C static CUri16* NewLC();

	IMPORT_C static CUri16* ResolveL(const TUriC16& aBaseUri, const TUriC16& aRefUri);

	IMPORT_C ~CUri16();
	IMPORT_C const TUriC16& Uri() const;
	IMPORT_C void SetComponentL(const TDesC16& aData, TUriComponent aComponent);
	IMPORT_C void RemoveComponentL(TUriComponent aComponent);

private:	// Methods

	CUri16(const TUriC16& aNewUri);
	void ConstructL();
	void FormUriL();
	void InitializeFileUriComponentsL(const TDesC& aFileName, TDriveNumber aDrive, TUint aFlags);

private:	// Attributes

	/** The descriptor buffer that contains the uri.
	 */
	HBufC16*	iUriBuf;

	/** The parsed uri object.
	 */
	TUriC16	iUri;
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
/**
	A friend class used for testing.
	@see		TUri16StateAccessor
	@since		6.0	
 */
#else
/**
	A friend class used for testing.
	@see		TUri16StateAccessor
	@since		6.0
	@internalComponent
 */
#endif	//SYMBIAN_ENABLE_SPLIT_HEADERS
	friend class TUri16StateAccessor;

	};

/** 	
Do not use. Use TUriC8 instead
@publishedAll
@deprecated Deprecated in 9.1 
 */
typedef TUriC16			TUriC;

/** 
Do not use. Use TUriParser8 instead
@publishedAll
@deprecated Deprecated in 9.1 
 */
typedef TUriParser16	TUriParser;

/** 
Do not use. Use CUri8 instead
@publishedAll
@deprecated Deprecated in 9.1 
 */
typedef CUri16			CUri;

#endif	// __URI16_H__
