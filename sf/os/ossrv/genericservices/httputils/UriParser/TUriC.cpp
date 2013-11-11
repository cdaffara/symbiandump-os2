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
//

// System includes
#include <uri8.h>
#include <uri16.h>
#include "TUriCInternal.h"
#include <uriutilscommon.h>
#include <uriutils.h>
#include "UriUtilsInternal.h"
#include "TValidator.h"
#include "TEquiv.h"
#include <escapeutils.h>



// Panic category
//
_LIT(KUriPanicCategory,"URI-BASE");

//
//
// Implementation of TUriC8
//
//

/**
  Constructor.
	
  @since			6.0
 */
EXPORT_C TUriC8::TUriC8()
	{
	// Reset the component table and the Uri
	Reset();
	}

/**
  Retrieves the specified component in the uri.
	
  @since			6.0
  @param			aComponent	The enum specifying the component.
  @return			A constant reference to a descriptor pointer to the specified 
  component.
 */
EXPORT_C const TDesC8& TUriC8::Extract(TUriComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EUriMaxComponents && aComponent >EUriComplete, User::Panic(KUriPanicCategory, KUriUtilsErrBadComponentIndex));

	return iComponent[aComponent];
	}

/**
  Indicates whether the specified component is present in the uri.
	
  @since			6.0
  @param			aComponent	The enum specifying the component.
  @return			A boolean value of ETrue if the desired component is present, 
  or EFalse if the desired component is not present.
 */
EXPORT_C TBool TUriC8::IsPresent(TUriComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EUriMaxComponents && aComponent >EUriComplete, User::Panic(KUriPanicCategory, KUriUtilsErrBadComponentIndex));

	return TBool(iComponent[aComponent].Ptr());
	}

/**
  Checks the scheme to be valid. If there is no scheme then the return value 
  indicates an invalid scheme.
	
  @since			6.0
  @return			A boolean value of ETrue if the scheme is valid, or EFalse if
  the scheme is not.
 */
EXPORT_C TBool TUriC8::IsSchemeValid() const
	{
	if( !IsPresent(EUriScheme) )
		{
		return EFalse;
		}
	return HasValidSchemeChars(iComponent[EUriScheme]);
	}

/**
  Compares the specified component against the one in the uri passed in.
	
  @since			6.0
  @param			aUri		The uri to compare components against.
  @param			aComponent	The enum specifying the component to compare.
  @return			An integer value of zero if the components are the same,
  any other value if the components are not the same.
 */
EXPORT_C TInt TUriC8::Compare(const TUriC8& aUri, TUriComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EUriMaxComponents && aComponent >EUriComplete, User::Panic(KUriPanicCategory, KUriUtilsErrBadComponentIndex));

	// Does the component exist in both the Uri's
	if( IsPresent(aComponent) && aUri.IsPresent(aComponent) )
		{
		if( aComponent == EUriScheme || aComponent == EUriHost )
			{
			// Do case insensitive compare for scheme and host
			return (iComponent[aComponent].CompareF(aUri.iComponent[aComponent]));
			}
		else
			{
			// Do case sensitive compare for all other components
			return (iComponent[aComponent].Compare(aUri.iComponent[aComponent]));
			}
		}
	else
		return KErrNotFound;
	}

/**
  Retrieves the descriptor for the entire uri.
	
  @since			6.0
  @return			A const reference to a descriptor pointer to the uri.
 */
EXPORT_C const TDesC8& TUriC8::UriDes() const
	{
	return iUriDes;
	}

/**
  @publishedPartner
  @released
  Ensures that all components of the URI are valid. If the method	returns 
  KUriUtilsErrInvalidUri then one or more components are not valid. It is
  possible for URIs declared valid by this method to be, on detailed inspection, 
  invalid.
	
  @return	KErrNone for vaild URIs, KUriUtilsErrInvalidUri for invalid URIs or KErrNotSupported.
 */
EXPORT_C TInt TUriC8::Validate() const
	{
	return DoValidate(*this);
	}

/**
  Compares the given URI to determine if it is equivalent to this URI. It is possible
  for URIs declared NOT equivalent by this method to be equvalent to a level not checked 
  by this method.
  @publishedPartner
  @released
  @param			aUri	Contains URI to compare 
  @return			ETrue if the two URIs are equivalent otherwise EFalse
 */
EXPORT_C TInt TUriC8::Equivalent(const TUriC8& aUri) const
	{
	TInt result = EFalse;
	TRAPD(error, result = DoEquivalenceL(*this, aUri));
	if (error != KErrNone)
		{
		result = error;
		}
	return result;
	}

/**
  Retrieves the uri without the fragment component.
	
  @since			6.0
  @param			aUriNoFrag	An output parameter which is set the non-reference 
  version of the uri.
 */
EXPORT_C void TUriC8::UriWithoutFragment(TPtrC8& aUriNoFrag) const
	{
	if( IsPresent(EUriFragment) )
		{
		// There is a fragment, find where it starts
		TInt fragmentStartPos = iUriDes.Locate(KFragmentDelimiter);
		aUriNoFrag.Set(iUriDes.Left(fragmentStartPos));
		}
	else{
		aUriNoFrag.Set(iUriDes);
		}
	}

/** 
  Create a new HBufC descriptor containing the desired component or the full URI.
	
  @param    aComponent  The component to convert into Unicode (EUriScheme - EUriFragment),  or
  the full URI (EUriComplete -- the default).
  @return    The descriptor containing the desired component. 
 */
EXPORT_C HBufC* TUriC8::DisplayFormL(TUriComponent aComponent) const
	{
	TPtrC8 component;	
	
	if (aComponent == EUriComplete) 
		{
		component.Set(iUriDes);
		}
	else 
		{  // extract the specified component, will Panic if invalid
		component.Set( Extract(aComponent) );
		}
	// convert descriptor from UTF-8 into Unicode
	return EscapeUtils::ConvertToUnicodeFromUtf8L(component);
	}

/**
  @internalComponent
  Resets the descriptor pointers for the uri components and the uri.
	
  @since		6.0
  @post			All uri component information is removed and the uri descriptor is 
  set to NULL.
 */
void TUriC8::Reset()
	{
	// Set descriptor pointers to NULL and lengths to zero
	for( TInt i=0; i<EUriMaxComponents; ++i )
		{
		iComponent[i].Set(NULL,0);
		}
	iUriDes.Set(NULL,0);
	}
	
//
//
// Implementation of TUriC16
//
//
/**
  Constructor.
	
  @since		6.0
  @deprecated 	Deprecated in 9.1
 */
EXPORT_C TUriC16::TUriC16()
	{
	// Reset the component table and the Uri
	Reset();
	}

/**
  Retrieves the specified component in the uri.
	
  @since			6.0
  @deprecated 		Deprecated in 9.1
  @param			aComponent	The enum specifying the component.
  @return			A constant reference to a descriptor pointer to the specified component.
 */
EXPORT_C const TDesC16& TUriC16::Extract(TUriComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EUriMaxComponents && aComponent >EUriComplete, User::Panic(KUriPanicCategory, KUriUtilsErrBadComponentIndex));

	return iComponent[aComponent];
	}

/**
  Indicates whether the specified component is present in the uri.
	
  @since			6.0
  @deprecated Deprecated in 9.1
  @param			aComponent	The enum specifying the component.
  @return			A boolean value of ETrue if the desired component is present, 
  or EFalse if the desired component is not present.
 */
EXPORT_C TBool TUriC16::IsPresent(TUriComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EUriMaxComponents && aComponent >EUriComplete, User::Panic(KUriPanicCategory, KUriUtilsErrBadComponentIndex));

	return TBool(iComponent[aComponent].Ptr());
	}

/**
  Checks to see if the scheme is valid. If there is no scheme then the return value 
  indicates an invalid scheme (EFalse).
	
  @since		6.0
  @deprecated 	Deprecated in 9.1
  @return		A boolean value of ETrue if the scheme is valid, or EFalse if
  the scheme is not.
 */
EXPORT_C TBool TUriC16::IsSchemeValid() const
	{
	if( !IsPresent(EUriScheme) )
		{
		return EFalse;
		}
	return HasValidSchemeChars(iComponent[EUriScheme]);
	}

/**
  Compares the specified component against the one in the uri passed in.
	
  @since		6.0
  @deprecated 	Deprecated in 9.1
  @param		aUri		The uri to compare components against.
  @param		aComponent	The enum specifying the component to compare.
  @return		An integer value of zero if the components are the same, any other
  value if the components are not the same.
 */
EXPORT_C TBool TUriC16::Compare(const TUriC16& aUri, TUriComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EUriMaxComponents && aComponent >EUriComplete, User::Panic(KUriPanicCategory, KUriUtilsErrBadComponentIndex));

	// Does the component exist in both the Uri's
	if( IsPresent(aComponent) && aUri.IsPresent(aComponent) )
		{
		if( aComponent == EUriScheme || aComponent == EUriHost )
			{
			// Do case insensitive compare for scheme and host
			return (iComponent[aComponent].CompareF(aUri.iComponent[aComponent]));
			}
		else
			{
			// Do case sensitive compare for all other components
			return (iComponent[aComponent].Compare(aUri.iComponent[aComponent]));
			}
		}
	else
		return KErrNotFound;
	}

/**
  Retrieves the descriptor for the entire uri.
	
  @since			6.0
  @deprecated Deprecated in 9.1
  @return			A const reference to a descriptor pointer to the uri.
 */
EXPORT_C const TDesC16& TUriC16::UriDes() const
	{
	return iUriDes;
	}

/**
  @deprecated Deprecated in 9.1 -- provided for compatibility
	
  Ensures that all components of the URI are valid. If the method	returns 
  KUriUtilsErrInvalidUri then one or more components are not valid. It is possible
  for URIs declared valid by this method to be, on detailed inspection, invalid.
	
  @return			KErrNone for vaild URIs, KUriUtilsErrInvalidUri for invalid URIs or KErrNotSupported.
 */
EXPORT_C TInt TUriC16::Validate() const
	{
	TInt result = KErrNone;
	TRAPD(error, result = ValidateL());
	if (error)
		{
		// problem allocating resourses 
		return error;
		}
	return result;
	}

/**
  @deprecated Deprecated in 9.1 -- provided for compatibility

  Compares the given URI to determine if it is equivalent to this URI. It is possible
  for URIs declared NOT equivalent by this method to be equvalent to a level not checked 
  by this method.
  @param			aUri	Contains URI to compare 
  @return			ETrue if the two URIs are equivalent otherwise EFalse
 */
EXPORT_C TInt TUriC16::Equivalent(const TUriC16& aUri) const
	{
	TInt result = KErrNone;
	TRAPD(error, result = EquivalentL(aUri.UriDes()));
	if (error)
		{
		// problem allocating resourses 
		return error;
		}
	return result;
	}
/**
  Retrieves the uri without the fragment component.
	
  @since			6.0
  @deprecated Deprecated in 9.1
  @param			aUriNoFrag	An output parameter which is set the non-reference 
  version of the uri.
 */

EXPORT_C void TUriC16::UriWithoutFragment(TPtrC16& aUriNoFrag) const
	{
	if( IsPresent(EUriFragment) )
		{
		// There is a fragment, find where it starts
		TInt fragmentStartPos = iUriDes.Locate(KFragmentDelimiter);
		aUriNoFrag.Set(iUriDes.Left(fragmentStartPos));
		}
	else{
		aUriNoFrag.Set(iUriDes);
		}
	}

/** 
  Create a new HBufC descriptor containing the desired component or the full URI.
  @deprecated Deprecated in 9.1 - provided for compatibility	
  
  @param aComponent  The component to convert into Unicode (EUriScheme - EUriFragment),  or
  the full URI (EUriComplete -- the default).
  @return The descriptor containing the desired component. 
 */
EXPORT_C HBufC* TUriC16::DisplayFormL(TUriComponent aComponent) const
	{
	if ( aComponent == EUriComplete) // extract the full URI
		{
		return iUriDes.AllocL();	
		}
	return Extract(aComponent).AllocL();
	}

/**	
  @internalComponent
  Resets the descriptor pointers for the uri components and the uri.
	
  @since			6.0
  @post			All uri component information is removed and the uri descriptor is 
  set to NULL.
 */
void TUriC16::Reset()
	{
	// Set descriptor pointers to NULL and lengths to zero
	for( TInt i=0; i<EUriMaxComponents; ++i )
		{
		iComponent[i].Set(NULL,0);
		}
	iUriDes.Set(NULL,0);
	}

/**
  @deprecated Deprecated in 9.1 
	
  Ensures that all components of the URI are valid. If the method	returns 
  KUriUtilsErrInvalidUri then one or more components are not valid. It is possible
  for URIs declared valid by this method to be, on detailed inspection, invalid.
	
  @return			KErrNone for vaild URIs, KUriUtilsErrInvalidUri for invalid URIs or KErrNotSupported.
 */
TInt TUriC16::ValidateL() const
	{
	TInt result;
	
	HBufC8* uri8 = CreateUri8LC(UriDes());
	TUriParser8 uriParser;		
	if(uriParser.Parse(*uri8) != KErrNone)
		{
		result = KUriUtilsErrInvalidUri;
		}
	else		
		{
		result = DoValidate(uriParser);		
		}
	
	CleanupStack::PopAndDestroy(uri8);
	return result;	
	}

/**
  @deprecated Deprecated in 9.1 

  Compares the given URI to determine if it is equivalent to this URI. It is possible
  for URIs declared NOT equivalent by this method to be equvalent to a level not checked 
  by this method.
  @param			aUri	Contains URI to compare 
  @return			ETrue if the two URIs are equivalent otherwise EFalse
 */
TBool TUriC16::EquivalentL(const TDesC16& aUri) const
	{
	HBufC8* lhs = CreateUri8LC(UriDes());
	HBufC8* rhs = CreateUri8LC(aUri);
	
	TUriParser8 lhsParser;		
	if(lhsParser.Parse(*lhs) != KErrNone)
		{
		User::Leave(KUriUtilsErrInvalidUri);
		}
		
	TUriParser8 rhsParser;		
	if(rhsParser.Parse(*rhs) != KErrNone)
		{
		User::Leave(KUriUtilsErrInvalidUri);
		}
	
	TBool result = DoEquivalenceL(lhsParser, rhsParser);		
	
	CleanupStack::PopAndDestroy(2);
	return result;	
	}

//
//
// Implementation of LOCAL functions
//
//

/**
	Checks the scheme for invalid characters. The scheme is invalid if it is empty.
						
	@since			6.0
	@param			aScheme	The descriptor with the scheme.
	@return			A boolean value of ETrue if all the characters in the scheme
	are valid, otherwise EFalse.
*/
template<class TPtrCType>
LOCAL_C TBool HasValidSchemeChars(const TPtrCType& aScheme)
	{
	TInt length = aScheme.Length();
	TBool valid = ( length && TChar(aScheme[0]).IsAlpha() );

	// Start checking from the second character
	TInt i=1;
	while( valid && i<length )
		{
		TChar c = aScheme[i];
		valid = c.IsAlphaDigit() || c==KPlusChar || c==KMinusChar || c==KPeriodChar;
		++i;
		}
	return valid;
	}


//
//
// File URI Implementation - CUri8
//
//
#ifdef __SECURE_DATA__	
/**
	Generates a fully-qualified filename from a file URI object.

	The form of the returned filename depends on the file location, as follows: 	
	- public file on a fixed drive: 
		@code
		file://c/path/to/file/name.ext -> c:\path\to\file\name.ext
		@endcode

	- public file on removable media drive:
	 The fully-qualified filename is resolved by finding the first drive on which the file exists.
	 If the file does not exist, this will Leave with KErrNotFound
	 	@code
		file://ext-media/path/to/file/name.ext -> f:\path\to\file\name.ext
		@endcode
		 (where f: is the first drive found containing "\path\to\file\name.ext")
		 
	- private file on fixed drive:
		@code
		file://private/c/path/to/file/name.ext -> c:\private\0x1001234f\path\to\file\name.ext
		@endcode
		 (where 0x1001234f is the SID of the current application)

	- private file on removable media drive:
		@code
		file://private/ext-media/path/to/file/name.ext -> h:\private\0x1001234f\path\to\file\name.ext
		@endcode
		 (where h: is the first drive found containing "\private\0x1001234f\path\to\file\name.ext")

	The returned file name is not guaranteed to exist except where specified above.

	Be warned that this method may be slow when resolving special paths (like "/ext-media")
	
	@leave			KErrNotSupported URI does not specify a local file
	@leave			KErrNotFound If the URI indicates the file is on removable media and the file does not exist on any.
	@since			9.1
	@return			A descriptor containing the fully-qualified filename. 
	@pre 			Object is fully constructed.
 */
#else
 /**
	Generates a fully-qualified filename from a file URI object.

	The form of the returned filename depends on the file location, as follows: 	
	- public file on a fixed drive: 
		@code
		file://c/path/to/file/name.ext -> c:\path\to\file\name.ext
		@endcode

	- public file on removable media drive:
	 The fully-qualified filename is resolved by finding the first drive on which the file exists.
	 If the file does not exist, this will Leave with KErrNotFound
	 	@code
		file://ext-media/path/to/file/name.ext -> f:\path\to\file\name.ext
		@endcode
		 (where f: is the first drive found containing "\path\to\file\name.ext")

	The returned file name is not guaranteed to exist except where specified above.

	Be warned that this method may be slow when resolving special paths (like "/ext-media")
	
	@leave			KErrNotSupported URI does not specify a local file
	@leave			KErrNotFound If the URI indicates the file is on removable media and the file does not exist on any.
	@since			9.1
	@return			A descriptor containing the fully-qualified filename. 
	@pre 			Object is fully constructed.
 */
 #endif
EXPORT_C HBufC* TUriC8::GetFileNameL() const
	{
	return GetFileNameL(EUriFileNameFull);
	}


/**
	This creates a file name from the URI, changing the network path separator (/) to the 
	local file system path separator (\\). 
	
	If called with EUriFileNamePath, this will generate a file name containing  the path relative to the drive. 
	Any parameters in the URI are stripped off:
		@code
		http://www.foo.com/dir/file/name.ext;param1;param2 -> \dir\file\name.ext
		@endcode
	
	If called with EUriFileNameTail, this will generate a just the name, with no directories or drive.
	Any parameters in the URI are stripped off:
		@code
		http://www.foo.com/dir/file/name.ext;param1;param2 -> name.ext
		@endcode
	
	EUriFileNamePath and EUriFileNameTail can be used for any hierarchical URI (http, ftp, file, etc)
	and may have unexpected behaviour when called on any other type of URI.

	If called with EUriFileNameFull, this will convert the URI into a fully-qualified filename. this is only valid
	for file URIs and will Leave if called on any other type of URI. The form of the returned filename is described in 
	GetFileNameL with no arguments.
	
	@leave			KErrNotSupported The URI scheme cannot be converted into the desired  type
	@since			9.1
	@pre 			Object is fully constructed.
	@param			aType specifies what part of the filename is to be  converted. The default is EUriFileNameFull).
	@return			A descriptor containing the desired filename components.
 */
EXPORT_C HBufC* TUriC8::GetFileNameL(TUriFileName aType) const
	{
	TBool isFileUri = Extract(EUriScheme).Compare(KFileUriScheme8) == 0;
	if(aType == EUriFileNameFull && !isFileUri)
		{		
		User::Leave(KErrNotSupported);
		}
	return ResolveFileNameL(Extract(EUriPath), aType, isFileUri);
	}
	

//
//
// File URI Implementation - CUri16
//
//

#ifdef  __SECURE_DATA__
/**
	Generates a fully-qualified filename from a file URI object.

	The form of the returned filename depends on the file location, as follows: 	
	- public file on a fixed drive: 
		@code
		file://c/path/to/file/name.ext -> c:\path\to\file\name.ext
		@endcode

	- public file on removable media drive:
	 The fully-qualified filename is resolved by finding the first drive on which the file exists.
	 If the file does not exist, this will Leave with KErrNotFound
		@code
		file://ext-media/path/to/file/name.ext -> f:\path\to\file\name.ext
		@endcode
		 (where f: is the first drive found containing "\path\to\file\name.ext")

	- private file on fixed drive:
		@code
		file://private/c/path/to/file/name.ext -> c:\private\0x1001234f\path\to\file\name.ext
		@endcode
		 (where 0x1001234f is the SID of the current application)

	- private file on removable media drive:
		@code
		file://private/ext-media/path/to/file/name.ext -> h:\private\0x1001234f\path\to\file\name.ext
		@endcode
		 (where h: is the first drive found containing "\private\0x1001234f\path\to\file\name.ext")

	The returned file name is not guaranteed to exist except where specified above.

	Be warned that this method may be slow when resolving special paths (like "/ext-media")
	
	@leave			KErrNotSupported URI does not specify a local file
	@leave			KErrNotFound If the URI indicates the file is on removable media and the file does not exist on any.
	@since			9.1
	@return			A descriptor containing the fully-qualified filename. 
	@pre 			Object is fully constructed.
	
 */
#else 
/**
	Generates a fully-qualified filename from a file URI object.

	The form of the returned filename depends on the file location, as follows: 	
	- public file on a fixed drive: 
		@code
		file://c/path/to/file/name.ext -> c:\path\to\file\name.ext
		@endcode

	- public file on removable media drive:
	 The fully-qualified filename is resolved by finding the first drive on which the file exists.
	 If the file does not exist, this will Leave with KErrNotFound
		@code
		file://ext-media/path/to/file/name.ext -> f:\path\to\file\name.ext
		@endcode
		 (where f: is the first drive found containing "\path\to\file\name.ext")

	The returned file name is not guaranteed to exist except where specified above.

	Be warned that this method may be slow when resolving special paths (like "/ext-media")
	
	@leave			KErrNotSupported URI does not specify a local file
	@leave			KErrNotFound If the URI indicates the file is on removable media and the file does not exist on any.
	@since			9.1
	@return			A descriptor containing the fully-qualified filename. 
	@pre 			Object is fully constructed.
	
 */
 #endif
EXPORT_C HBufC* TUriC16::GetFileNameL() const
	{
	return GetFileNameL(EUriFileNameFull);
	}


/**
	This creates a file name from the URI, changing the network path separator (/) to the 
	local file system path separator (\\). 
	
	If called with EUriFileNamePath, this will generate a file name containing  the path relative to the drive. 
	Any parameters in the URI are stripped off:
		@code
		http://www.foo.com/dir/file/name.ext;param1;param2 -> \dir\file\name.ext
		@endcode
	
	If called with EUriFileNameTail, this will generate a just the name, with no directories or drive.
	Any parameters in the URI are stripped off:
		@code
		http://www.foo.com/dir/file/name.ext;param1;param2 -> name.ext
		@endcode
	
	EUriFileNamePath and EUriFileNameTail can be used for any hierarchical URI (http, ftp, file)
	and may have unexpected behaviour when called on any other type of URI.

	If called with EUriFileNameFull, this will convert the URI into a fully-qualified filename. this is only valid
	for file URIs and will Leave if called on any other type of URI. The form of the returned filename is described in 
	GetFileNameL with no arguments.
	
	@leave			KErrNotSupported The URI scheme cannot be converted into the desired  type
	@since			9.1
	@pre 			Object is fully constructed.
	@param			aType specifies what part of the filename is to be converted. The default is (EUriFileNameFull).
	@return			A descriptor containing the desired filename components.
 */
EXPORT_C HBufC* TUriC16::GetFileNameL(TUriFileName aType) const
	{
	TBool isFileUri = Extract(EUriScheme).Compare(KFileUriScheme16) == 0;

	if(aType == EUriFileNameFull &&  !isFileUri )
		{		
		User::Leave(KErrNotSupported);
		}		

	HBufC8* fileUriPath8 = HBufC8::NewLC(Extract(EUriPath).Length());
	fileUriPath8->Des().Copy(Extract(EUriPath));
	HBufC* absFileName = ResolveFileNameL(*fileUriPath8, aType, isFileUri);
	CleanupStack::PopAndDestroy(fileUriPath8);
	return absFileName;
	}


//
//
// Implementation of LOCAL functions
//
//

/**
  Function used to convert '\' to '/' and vice versa.
	
  @since			9.1
  @param			aDesPtr				A descriptor reference to the string.
  @param			aPathSeperatorFrom	A path seperator to be converted (from)
  @param			aPathSeperatorTo 	A path seperator to converte (To).
 */
void ChangePathSeparator(TDes& aDesPtr, TUint aPathSeperatorFrom, TUint aPathSeperatorTo)
	{
	for(TInt offset = aDesPtr.Length() - 1;offset >= 0;offset--)
		{
		if (aDesPtr[offset] == aPathSeperatorFrom)
			{	
				aDesPtr[offset] = TUint16(aPathSeperatorTo);
			}
		}
	}

/**
  Function used to generate a fully qualified file name for a public or private file 
  stored on a fix drive or a removable media drive from File URI path (parameter aFileUriPath).
	
  This will be called by API GetFileName() to generate and return a filename.
	
  @leave			KErrBadName 	if the path doesn't contain valid drivename.
  @since			9.1
  @param			aFileUriPath	A descriptor reference to the File URI's path component.
  @return			A pointer to a buffer containing the resolved fully qualified filename 
					if the file uri path is not empty or a top level dir on main drive (C:\\)
 */
HBufC* GetFullFileNameFromFileUriPathL(const TDesC& aFileUriPath)
	{	
	TFileName filename(KDefaultPath);
	
	TInt origLength = aFileUriPath.Length();
	if(origLength == 0)
		{
		return filename.AllocL();
		}
		
	TInt index = 0;
 	TPtrC fileUriPath(aFileUriPath);
 	
 	//skip KUriPathSeparator (/) from the Uri path
 	if (fileUriPath[0] == KUriPathSeparator)
  		{
		index++;
		}
	
	fileUriPath.Set(aFileUriPath.Right(origLength - index));

	RFs fs;
	TBool fsOpen = EFalse;
				
#ifdef __SECURE_DATA__			
	if (fileUriPath.Left(KPrivate().Length()).Compare(KPrivate) == 0)
		{
		index += KPrivate().Length();
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);		
		fsOpen = ETrue;
		User::LeaveIfError(fs.PrivatePath(filename));
		filename.Insert(0, KDefaultPath().Left((KDefaultPath().Length() - 1) )); //Insert <drive>: 
		fileUriPath.Set(aFileUriPath.Right(origLength - index));
		}
#endif
	
	TBool isExtMedia = (fileUriPath.Left(KExtMedia().Length()).Compare(KExtMedia) == 0);
	if (!isExtMedia )
		{
		TUint driveLetter = fileUriPath[0];	
		//Checking for valid driveletter (should between A to Z or a to z) and followed by Uri path seperator "/"
		if ( fileUriPath[1] != KUriPathSeparator ||
		 (  (driveLetter < 'A' || driveLetter > 'Z') && 
			(driveLetter < 'a' || driveLetter > 'z')  )  )
			{
			User::Leave(KErrBadName);
			}
		index += (KDefaultPath().Length() - 1); //skip <driveLetter>/
		filename[0] = TUint16(driveLetter); 
		}
	else
		{
		index += KExtMedia().Length();
		}
		
	filename.Append(aFileUriPath.Right(origLength - index));
		
	//Convert "/" to "\"
	ChangePathSeparator(filename, KUriPathSeparator, KFilePathSeparator);

	if (isExtMedia )
		{		
		if (fsOpen == EFalse)
			{
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);		
			fsOpen = ETrue;	
			}
		
		TDriveInfo driveInfo;
		TInt err = KErrNotFound;

		for (TInt driveNum = EDriveA; driveNum <= EDriveZ && err!=KErrNone; driveNum++)   
			{
			if (fs.Drive(driveInfo, driveNum ) == KErrNone
    			&& (driveInfo.iDriveAtt & KDriveAttRemovable))       
    			{
    			filename[0]= TInt16('A' + driveNum);
    			TUint attValue;
    			err = fs.Att(filename, attValue);
    			}
			}
  		User::LeaveIfError(err);
		}
							
	if (fsOpen)
		{
		CleanupStack::PopAndDestroy(&fs);
		}
		
	return filename.AllocL();	
	}

/**
  Creates an 8 bit URI descriptor from an 16  bit one
	
  @param			aBuf16	The full 16 bit URI descriptor
  @return			Pointer to the newly created 8 bit URI descriptor
					are valid, otherwise EFalse.
*/
HBufC8* CreateUri8LC(const TDesC16& aBuf16)
	{
	TInt length = aBuf16.Length();
	HBufC8* newBuf = HBufC8::NewMaxLC(length);
	TPtr8 des8Ptr = newBuf->Des();
	for (TInt i = 0; i < length; ++i)
		{
		des8Ptr[i] = static_cast<TText8>(aBuf16[i]);
		}
	return newBuf;
	}

/**
  Check the components of a URI to ensure they are all valid
	
  @since			8.1
  @param			aUri The URI to check
  @return			KErrNone if all components are valid, otherwise one of the 'invalid' error codes
*/
TInt DoValidate(const TUriC8& aUri)
	{
	TInt result;
	TPtrC8 subComponent(aUri.Extract(EUriScheme));
	if (!HasValidSchemeChars(subComponent))
		{
		result = KUriUtilsErrInvalidScheme;
		}
	else if(SchemeType(subComponent) == ESchemeTypeSip) 
		//To validate Sip uri
		{
		TValidatorSip validator(aUri);
		result = validator.Validate();		
		}	
	else if(SchemeType(subComponent) == ESchemeTypeTel) 
		//To validate Tel uri
		{
		TValidatorTel validator(aUri);
		result = validator.Validate();
		}
	else		
		{
		result = KErrNotSupported;		
		}
		
	return result;
	}

/**
  Compare the components of two URIs to see if they identify the same resource
	
  @since			8.1
  @param			aLhs The left hand side URI to compare
  @param			aRhs The right hand side URI to compare
  @return			ETrue if they point to the same resource, EFalse otherwise.
*/
TInt DoEquivalenceL(const TUriC8& aLhs, const TUriC8& aRhs)
	{
	TInt result;
	if (SchemeType(aLhs.Extract(EUriScheme)) == ESchemeTypeSip)
		{
		TEquivSip equiv(aLhs, aRhs);
		result = equiv.EquivalentL();
		}
	else
		{
		// unknown scheme so assume network scheme
		TEquiv equiv(aLhs, aRhs);
		result = equiv.EquivalentL();
		}
	
	return result;
	}


/**
  Get the desired part of the filename from the URI path
	
  @param			aPath A descriptor reference to the File URI's path component.
  @param			aType enum value of TUriFileName.
  @param			aIsFileUri Specifies the whether it is FileUri or not.
  @return			Pointer to the newly created URI descriptor which contains the 
  					desired part of the filename from the URI path.
*/
HBufC* ResolveFileNameL(const TDesC8& aPath, TUriFileName aType, TBool aIsFileUri)
	{	
	// replace % codes
	HBufC8* fileUriPath8 = EscapeUtils::EscapeDecodeL(aPath);
	CleanupStack::PushL(fileUriPath8);
	
	// convert to unicode 
	HBufC* fileUriPath = EscapeUtils::ConvertToUnicodeFromUtf8L(*fileUriPath8);
	CleanupStack::PopAndDestroy(fileUriPath8);
	CleanupStack::PushL(fileUriPath);			

	HBufC* absFileName = NULL; // setting to NULL makes compiler happy
	if ( aIsFileUri )
		{ 
		/* awlays do full conversion for file URIs
		 It's not optimal time-wise, but it uses the least code and is
		 forward-compatible. */
		absFileName = GetFullFileNameFromFileUriPathL(*fileUriPath);		
		CleanupStack::PopAndDestroy(fileUriPath);
		if ( aType == EUriFileNameFull )
			{	
			return absFileName;
			}
		fileUriPath = absFileName;	// code below uses fileUriPath
		CleanupStack::PushL(fileUriPath);			
		TPtr path(fileUriPath->Des());
		path.Delete(0,2);	// delete drive and ':' (always begins with drive letter and :)
		}
	else  // not a file URI
		{ // convert '/' to '\' for all other URIs
		TPtr path(fileUriPath->Des());
		ChangePathSeparator(path, KUriPathSeparator, KFilePathSeparator);
		}
		
	// EUriFileNameFull must only ever be used with a file URI. Failures should be caught by the caller
	__ASSERT_DEBUG(aType != EUriFileNameFull, 
		User::Panic(KUriPanicCategory, KUriUtilsErrBadComponentIndex));
	
	TPtrC name;
	GetFileComponent(name, *fileUriPath, aType ); // get path or tail
	if (name.Length() == fileUriPath->Length()) 
		{ // no changes, just return fileUriPath
		absFileName = fileUriPath;
		CleanupStack::Pop(fileUriPath);
		}
	else
		{
		absFileName = name.AllocL();
		CleanupStack::PopAndDestroy(fileUriPath); 
		}
	
	return absFileName;
	}

/**
  Gets the desired file path or tail.
	
  @param			aNewName An outparameter, reference to descriptor .
  @param			aOldName An inparameter descriptor reference.
  @param			aType enum value of TUriFileName.
 */
void GetFileComponent(TPtrC& aNewName, const TDesC& aOldName, TUriFileName aType )
	{
	__ASSERT_ALWAYS(aType != EUriFileNameFull, User::Panic(KUriPanicCategory, KUriUtilsErrBadComponentIndex));

	aNewName.Set(aOldName);
	// chop off everything after the first ;
	TInt pos = aNewName.Locate(KParamDelimiter);
	if( pos == 0 ) // the ; is at the start
		{	
		aNewName.Set(KNullDesC);
		}
	else if (pos > 0)
		{ // set to the text untilt he ;
		aNewName.Set(aNewName.Left(pos));
		}
	if( aType == EUriFileNameTail)
		{	
		// chop off everything before the last /
		TInt pos = aNewName.LocateReverse(KFilePathSeparator);
		if( pos >=0 ) // there is a /
			{
			if( pos >= aNewName.Length() - 1 ) // the / is at the end
				{
				aNewName.Set(KNullDesC);
				}
			else
				{ // set to the text following the last /
				aNewName.Set(aNewName.Right(aNewName.Length()-1-pos));
				}
			}
		}
	}



	
